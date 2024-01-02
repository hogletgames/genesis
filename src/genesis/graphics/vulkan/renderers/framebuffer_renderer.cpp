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
#include "single_command.h"
#include "texture.h"
#include "utils.h"
#include "vulkan_exception.h"

using AttachmentDescriptions = std::vector<VkAttachmentDescription>;

namespace {

VkExtent2D toVkExtent(const GE::Vec2& size)
{
    return {static_cast<uint32_t>(size.x), static_cast<uint32_t>(size.y)};
}

VkViewport toVkViewport(const GE::Vec2& size)
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

bool isColorAttachment(const VkAttachmentDescription& attachment)
{
    return GE::isColorFormat(GE::Vulkan::toTextureFormat(attachment.format));
}

inline constexpr bool isOneSampleAttachment(uint32_t sample_count,
                                            VkSampleCountFlagBits sample_flags)
{
    return sample_count == 1 || sample_flags != VK_SAMPLE_COUNT_1_BIT;
}

void fillClearNoneAttachment(VkAttachmentDescription* attachment)
{
    attachment->loadOp = VK_ATTACHMENT_LOAD_OP_DONT_CARE;
    attachment->stencilLoadOp = VK_ATTACHMENT_LOAD_OP_DONT_CARE;
}

void fillClearColorAttachment(VkAttachmentDescription* attachment)
{
    if (isColorAttachment(*attachment)) {
        attachment->loadOp = VK_ATTACHMENT_LOAD_OP_CLEAR;
        attachment->stencilLoadOp = VK_ATTACHMENT_LOAD_OP_DONT_CARE;
    } else {
        attachment->loadOp = VK_ATTACHMENT_LOAD_OP_DONT_CARE;
        attachment->stencilLoadOp = VK_ATTACHMENT_LOAD_OP_DONT_CARE;
    }
}

void fillClearDepthAttachment(VkAttachmentDescription* attachment)
{
    if (isColorAttachment(*attachment)) {
        attachment->loadOp = VK_ATTACHMENT_LOAD_OP_DONT_CARE;
        attachment->stencilLoadOp = VK_ATTACHMENT_LOAD_OP_DONT_CARE;
    } else {
        attachment->loadOp = VK_ATTACHMENT_LOAD_OP_CLEAR;
        attachment->stencilLoadOp = VK_ATTACHMENT_LOAD_OP_CLEAR;
    }
}

void fillClearAllAttachment(VkAttachmentDescription* attachment)
{
    if (isColorAttachment(*attachment)) {
        attachment->loadOp = VK_ATTACHMENT_LOAD_OP_CLEAR;
        attachment->stencilLoadOp = VK_ATTACHMENT_LOAD_OP_DONT_CARE;
    } else {
        attachment->loadOp = VK_ATTACHMENT_LOAD_OP_CLEAR;
        attachment->stencilLoadOp = VK_ATTACHMENT_LOAD_OP_CLEAR;
    }
}

void prepareClearNoneAttachments(AttachmentDescriptions* attachments)
{
    for (auto& attachment : *attachments) {
        fillClearNoneAttachment(&attachment);
    }
}

void prepareClearColorAttachments(AttachmentDescriptions* attachments, uint32_t sample_count)
{
    for (auto& attachment : *attachments) {
        if (isOneSampleAttachment(sample_count, attachment.samples)) {
            fillClearColorAttachment(&attachment);
        } else {
            fillClearNoneAttachment(&attachment);
        }
    }
}

void prepareClearDepthAttachments(AttachmentDescriptions* attachments, uint32_t sample_count)
{
    for (auto& attachment : *attachments) {
        if (isOneSampleAttachment(sample_count, attachment.samples)) {
            fillClearDepthAttachment(&attachment);
        } else {
            fillClearNoneAttachment(&attachment);
        }
    }
}

void prepareClearAllAttachments(AttachmentDescriptions* attachments, uint32_t sample_count)
{
    for (auto& attachment : *attachments) {
        if (isOneSampleAttachment(sample_count, attachment.samples)) {
            fillClearAllAttachment(&attachment);
        } else {
            fillClearNoneAttachment(&attachment);
        }
    }
}
} // namespace

namespace GE::Vulkan {

FramebufferRenderer::FramebufferRenderer(Shared<Device> device, Vulkan::Framebuffer* framebuffer)
    : RendererBase{std::move(device)}
    , m_framebuffer{framebuffer}
{
    createClearValues();
    createRenderPasses();
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
    if (!beginRenderPass(clear_mode)) {
        return false;
    }

    updateDynamicState();
    return true;
}

void FramebufferRenderer::endFrame()
{
    VkCommandBuffer cmd = cmdBuffer();
    m_render_command.submit(cmd);

    if (!endRenderPass() || !submit()) {
        return;
    }

    if (m_framebuffer->hasDepthAttachment()) {
        depthImagePipelineBarrier();
    }

    m_descriptor_pool->reset();
}

void FramebufferRenderer::swapBuffers()
{
    vkWaitForFences(m_device->device(), 1, &m_in_flight_fence, VK_TRUE,
                    std::numeric_limits<uint64_t>::max());
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

void FramebufferRenderer::createClearValues()
{
    auto clear_color = toVkClearColorValue(m_framebuffer->clearColor());
    auto clear_depth = toVkClearDepthStencilValue(m_framebuffer->clearDepth());
    m_clear_values.reserve(m_framebuffer->attachments().size());

    for (const auto& attachment : m_framebuffer->attachments()) {
        if (isColorFormat(toTextureFormat(attachment.format))) {
            m_clear_values.emplace_back(clear_color);
        } else {
            m_clear_values.emplace_back(clear_depth);
        }
    }
}

void FramebufferRenderer::createRenderPasses()
{
    auto attachments = m_framebuffer->attachments();
    uint32_t sample_count = m_framebuffer->MSAASamples();
    bool is_multisampled = sample_count > 1;

    prepareClearNoneAttachments(&attachments);
    m_render_passes[CLEAR_NONE] = createRenderPass(attachments, is_multisampled);

    prepareClearColorAttachments(&attachments, sample_count);
    m_render_passes[CLEAR_COLOR] = createRenderPass(attachments, is_multisampled);

    prepareClearDepthAttachments(&attachments, sample_count);
    m_render_passes[CLEAR_DEPTH] = createRenderPass(attachments, is_multisampled);

    prepareClearAllAttachments(&attachments, sample_count);
    m_render_passes[CLEAR_ALL] = createRenderPass(attachments, is_multisampled);
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

void FramebufferRenderer::depthImagePipelineBarrier()
{
    auto barrier_config = m_framebuffer->depthTexture().image()->memoryBarrierConfig();
    barrier_config.old_layout = VK_IMAGE_LAYOUT_DEPTH_STENCIL_ATTACHMENT_OPTIMAL;
    barrier_config.new_layout = VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL;

    auto barrier = MemoryBarrier::createImageMemoryBarrier(barrier_config);
    SingleCommand cmd{m_device};

    PipelineBarrier::submit(cmd.buffer(), {barrier}, VK_PIPELINE_STAGE_EARLY_FRAGMENT_TESTS_BIT,
                            VK_PIPELINE_STAGE_FRAGMENT_SHADER_BIT);
}

VkCommandBuffer FramebufferRenderer::cmdBuffer() const
{
    return m_cmd_buffers.front();
}

VkFramebuffer FramebufferRenderer::currentFramebuffer() const
{
    return m_framebuffer->framebuffer();
}

VkExtent2D FramebufferRenderer::extent() const
{
    return toVkExtent(m_framebuffer->size());
}

VkViewport FramebufferRenderer::viewport() const
{
    return toVkViewport(m_framebuffer->size());
}

} // namespace GE::Vulkan
