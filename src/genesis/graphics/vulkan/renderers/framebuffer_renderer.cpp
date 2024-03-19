/*
 * BSD 3-Clause License
 *
 * Copyright (c) 2022, Dmitry Shilnenkov
 * All rights reserved.
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions are met:
 *
 * * Redistributions of source code must retain the above copyright notice, this
 *   list of conditions and the following disclaimer.
 *
 * * Redistributions in binary form must reproduce the above copyright notice,
 *   this list of conditions and the following disclaimer in the documentation
 *   and/or other materials provided with the distribution.
 *
 * * Neither the name of the copyright holder nor the names of its
 *   contributors may be used to endorse or promote products derived from
 *   this software without specific prior written permission.
 *
 * THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS"
 * AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
 * IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE
 * DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDER OR CONTRIBUTORS BE LIABLE
 * FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL
 * DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR
 * SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER
 * CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY,
 * OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE
 * OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 */

#include "framebuffer_renderer.h"
#include "descriptor_pool.h"
#include "device.h"
#include "framebuffer.h"
#include "image.h"
#include "pipeline.h"
#include "pipeline_barrier.h"
#include "texture.h"
#include "utils.h"
#include "vulkan_exception.h"

namespace GE::Vulkan {
namespace {

VkExtent2D toVkExtent(const Vec2& size)
{
    return {static_cast<uint32_t>(size.x), static_cast<uint32_t>(size.y)};
}

VkViewport toVkViewport(const Vec2& size)
{
    return {
        0.0f,   // x
        0.0f,   // y
        size.x, // width
        size.y, // height
        0.0f,   // min depth
        1.0f    // max depth
    };
}

} // namespace

FramebufferRenderer::FramebufferRenderer(Shared<Device> device, Vulkan::Framebuffer* framebuffer)
    : RendererBase{std::move(device)}
    , m_framebuffer{framebuffer}
{
    createCommandBuffers(1);
    createSyncObjects();
}

FramebufferRenderer::~FramebufferRenderer()
{
    m_device->waitIdle();
    destroyVkHandles();
}

bool FramebufferRenderer::beginFrame(Renderer::ClearMode clear_mode)
{
    if (!beginRendering(clear_mode)) {
        return false;
    }

    updateDynamicState();
    return true;
}

void FramebufferRenderer::endFrame()
{
    m_render_command.submit(cmdBuffer());
    endRendering();
}

void FramebufferRenderer::swapBuffers()
{
    submit();
    vkWaitForFences(m_device->device(), 1, &m_in_flight_fence, VK_TRUE,
                    std::numeric_limits<uint64_t>::max());
    m_descriptor_pool->reset();
}

Scoped<GE::Pipeline> FramebufferRenderer::createPipeline(const GE::pipeline_config_t& config)
{
    auto color_formats = [](GE::Framebuffer* framebuffer) {
        std::vector<VkFormat> formats;

        for (uint32_t i{0}; i < framebuffer->colorAttachmentCount(); i++) {
            formats.emplace_back(toVkFormat(framebuffer->colorTexture(i).format()));
        }

        return formats;
    };

    auto vulkan_config = Vulkan::Pipeline::createDefaultConfig(config);
    vulkan_config.pipeline_cache = m_pipeline_cache;
    vulkan_config.color_formats = color_formats(m_framebuffer);
    vulkan_config.depth_format = toVkFormat(m_framebuffer->depthTexture().format());
    vulkan_config.front_face = VK_FRONT_FACE_CLOCKWISE;
    vulkan_config.msaa_samples = toVkSampleCountFlag(m_framebuffer->MSAASamples());
    vulkan_config.descriptor_pool = m_descriptor_pool;
    return tryMakeScoped<Vulkan::Pipeline>(m_device, vulkan_config);
}

void FramebufferRenderer::createSyncObjects()
{
    VkFenceCreateInfo create_info{};
    create_info.sType = VK_STRUCTURE_TYPE_FENCE_CREATE_INFO;

    if (vkCreateFence(m_device->device(), &create_info, nullptr, &m_in_flight_fence) !=
        VK_SUCCESS) {
        throw Vulkan::Exception{"Failed to create in flight Fence"};
    }
}

void FramebufferRenderer::destroyVkHandles()
{
    vkDestroyFence(m_device->device(), m_in_flight_fence, nullptr);
    m_in_flight_fence = VK_NULL_HANDLE;
}

bool FramebufferRenderer::submit()
{
    VkCommandBuffer cmd = cmdBuffer();

    VkSubmitInfo submit_info{};
    submit_info.sType = VK_STRUCTURE_TYPE_SUBMIT_INFO;
    submit_info.waitSemaphoreCount = 0;
    submit_info.pWaitSemaphores = nullptr;
    submit_info.pWaitDstStageMask = nullptr;
    submit_info.commandBufferCount = 1;
    submit_info.pCommandBuffers = &cmd;
    submit_info.signalSemaphoreCount = 0;
    submit_info.pSignalSemaphores = nullptr;

    vkResetFences(m_device->device(), 1, &m_in_flight_fence);

    if (vkQueueSubmit(m_device->graphicsQueue(), 1, &submit_info, m_in_flight_fence) !=
        VK_SUCCESS) {
        GE_CORE_ERR("Failed to submit framebuffer graphics queue");
        return false;
    }

    return true;
}

void FramebufferRenderer::transitImageLayoutBeforeRendering(VkCommandBuffer cmd)
{
    // Color attachments

    std::vector<VkImageMemoryBarrier> color_barriers;

    for (uint32_t i{0}; i < m_framebuffer->colorAttachmentCount(); i++) {
        auto barrier = m_framebuffer->colorTexture(i).image()->imageMemoryBarrier();
        barrier.srcAccessMask = VK_ACCESS_NONE;
        barrier.dstAccessMask =
            VK_ACCESS_COLOR_ATTACHMENT_WRITE_BIT | VK_ACCESS_COLOR_ATTACHMENT_READ_BIT;
        barrier.oldLayout = VK_IMAGE_LAYOUT_UNDEFINED;
        barrier.newLayout = VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL;

        color_barriers.push_back(barrier);
    }

    PipelineBarrier::submit(cmd, color_barriers, VK_PIPELINE_STAGE_BOTTOM_OF_PIPE_BIT,
                            VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT);

    // Depth attachment

    if (m_framebuffer->hasDepthAttachment()) {
        auto barrier = m_framebuffer->depthTexture().image()->imageMemoryBarrier();
        barrier.srcAccessMask = VK_ACCESS_NONE;
        barrier.dstAccessMask = VK_ACCESS_DEPTH_STENCIL_ATTACHMENT_READ_BIT |
                                VK_ACCESS_DEPTH_STENCIL_ATTACHMENT_WRITE_BIT;
        barrier.oldLayout = VK_IMAGE_LAYOUT_UNDEFINED;
        barrier.newLayout = VK_IMAGE_LAYOUT_DEPTH_STENCIL_ATTACHMENT_OPTIMAL;

        PipelineBarrier::submit(cmd, {barrier}, VK_PIPELINE_STAGE_TOP_OF_PIPE_BIT,
                                VK_PIPELINE_STAGE_EARLY_FRAGMENT_TESTS_BIT);
    }
}

void FramebufferRenderer::transitImageLayoutAfterRendering(VkCommandBuffer cmd)
{
    // Color attachments

    std::vector<VkImageMemoryBarrier> color_barriers;

    for (uint32_t i{0}; i < m_framebuffer->colorAttachmentCount(); i++) {
        auto barrier = m_framebuffer->colorTexture(i).image()->imageMemoryBarrier();
        barrier.srcAccessMask = VK_ACCESS_COLOR_ATTACHMENT_WRITE_BIT;
        barrier.dstAccessMask = VK_ACCESS_NONE;
        barrier.oldLayout = VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL;
        barrier.newLayout = VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL;

        color_barriers.push_back(barrier);
    }

    PipelineBarrier::submit(cmd, color_barriers, VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT,
                            VK_PIPELINE_STAGE_BOTTOM_OF_PIPE_BIT);

    // Depth attachment

    if (m_framebuffer->hasDepthAttachment()) {
        auto barrier = m_framebuffer->depthTexture().image()->imageMemoryBarrier();
        barrier.srcAccessMask = VK_ACCESS_DEPTH_STENCIL_ATTACHMENT_READ_BIT |
                                VK_ACCESS_DEPTH_STENCIL_ATTACHMENT_WRITE_BIT;
        barrier.dstAccessMask = VK_ACCESS_NONE;
        barrier.oldLayout = VK_IMAGE_LAYOUT_DEPTH_STENCIL_ATTACHMENT_OPTIMAL;
        barrier.newLayout = VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL;

        PipelineBarrier::submit(cmd, {barrier}, VK_PIPELINE_STAGE_LATE_FRAGMENT_TESTS_BIT,
                                VK_PIPELINE_STAGE_BOTTOM_OF_PIPE_BIT);
    }
}

VkCommandBuffer FramebufferRenderer::cmdBuffer() const
{
    return m_cmd_buffers.front();
}

VkExtent2D FramebufferRenderer::extent() const
{
    return toVkExtent(m_framebuffer->size());
}

VkViewport FramebufferRenderer::viewport() const
{
    return toVkViewport(m_framebuffer->size());
}

const std::vector<VkRenderingAttachmentInfo>&
FramebufferRenderer::colorRenderingAttachments(ClearMode clear_mode)
{
    return m_framebuffer->colorRenderingAttachments(clear_mode);
}

const VkRenderingAttachmentInfo& FramebufferRenderer::depthRenderingAttachment(ClearMode clear_mode)
{
    return m_framebuffer->depthRenderingAttachment(clear_mode);
}

} // namespace GE::Vulkan
