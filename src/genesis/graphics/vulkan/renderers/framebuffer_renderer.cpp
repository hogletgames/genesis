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
#include "device.h"
#include "framebuffer.h"
#include "image.h"
#include "pipeline.h"
#include "pipeline_barrier.h"
#include "single_command.h"
#include "texture.h"
#include "vulkan_exception.h"

namespace {

VkClearValue toVkClearColorValue(const GE::Vec4& clear_color)
{
    return {{{clear_color.x, clear_color.y, clear_color.z, clear_color.w}}};
}

VkClearValue toVkClearDepthStencilValue(float clear_depth)
{
    VkClearValue clear_value;
    clear_value.depthStencil = {clear_depth, 0};
    return clear_value;
}

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

void fillClearNoneAttachmentsOp(std::vector<VkAttachmentDescription>* attachments)
{
    for (auto& attachment : *attachments) {
        attachment.loadOp = VK_ATTACHMENT_LOAD_OP_DONT_CARE;
        attachment.stencilLoadOp = VK_ATTACHMENT_LOAD_OP_DONT_CARE;
    }
}

void fillClearColorAttachmentsOp(std::vector<VkAttachmentDescription>* attachments)
{
    for (auto& attachment : *attachments) {
        if (isColorAttachment(attachment)) {
            attachment.loadOp = VK_ATTACHMENT_LOAD_OP_CLEAR;
            attachment.stencilLoadOp = VK_ATTACHMENT_LOAD_OP_DONT_CARE;
        } else {
            attachment.loadOp = VK_ATTACHMENT_LOAD_OP_DONT_CARE;
            attachment.stencilLoadOp = VK_ATTACHMENT_LOAD_OP_DONT_CARE;
        }
    }
}

void fillClearDepthAttachmentsOp(std::vector<VkAttachmentDescription>* attachments)
{
    for (auto& attachment : *attachments) {
        if (isColorAttachment(attachment)) {
            attachment.loadOp = VK_ATTACHMENT_LOAD_OP_DONT_CARE;
            attachment.stencilLoadOp = VK_ATTACHMENT_LOAD_OP_DONT_CARE;
        } else {
            attachment.loadOp = VK_ATTACHMENT_LOAD_OP_CLEAR;
            attachment.stencilLoadOp = VK_ATTACHMENT_LOAD_OP_CLEAR;
        }
    }
}

void fillClearAllAttachmentsOp(std::vector<VkAttachmentDescription>* attachments)
{
    for (auto& attachment : *attachments) {
        if (isColorAttachment(attachment)) {
            attachment.loadOp = VK_ATTACHMENT_LOAD_OP_CLEAR;
            attachment.stencilLoadOp = VK_ATTACHMENT_LOAD_OP_DONT_CARE;
        } else {
            attachment.loadOp = VK_ATTACHMENT_LOAD_OP_CLEAR;
            attachment.stencilLoadOp = VK_ATTACHMENT_LOAD_OP_CLEAR;
        }
    }
}

} // namespace

namespace GE::Vulkan {

FramebufferRenderer::FramebufferRenderer(Shared<Device> device,
                                         Vulkan::Framebuffer* framebuffer)
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
}

void FramebufferRenderer::swapBuffers()
{
    vkWaitForFences(m_device->device(), 1, &m_in_flight_fence, VK_TRUE,
                    std::numeric_limits<uint64_t>::max());
}

Scoped<GE::Pipeline>
FramebufferRenderer::createPipeline(const GE::pipeline_config_t& config)
{
    auto vulkan_config = Vulkan::Pipeline::makeDefaultConfig();
    vulkan_config.base = config;
    vulkan_config.pipeline_cache = m_pipeline_cache;
    vulkan_config.render_pass = m_render_passes[CLEAR_ALL];
    vulkan_config.front_face = VK_FRONT_FACE_CLOCKWISE;
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

    fillClearNoneAttachmentsOp(&attachments);
    m_render_passes[CLEAR_NONE] = createRenderPass(attachments);

    if (m_framebuffer->MSSASamples() > 1) {
        m_render_passes[CLEAR_COLOR] = createRenderPass(attachments);
        m_render_passes[CLEAR_DEPTH] = createRenderPass(attachments);
        m_render_passes[CLEAR_ALL] = createRenderPass(attachments);
        return;
    }

    fillClearColorAttachmentsOp(&attachments);
    m_render_passes[CLEAR_COLOR] = createRenderPass(attachments);

    fillClearDepthAttachmentsOp(&attachments);
    m_render_passes[CLEAR_DEPTH] = createRenderPass(attachments);

    fillClearAllAttachmentsOp(&attachments);
    m_render_passes[CLEAR_ALL] = createRenderPass(attachments);
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

    PipelineBarrier::submit(cmd.buffer(), {barrier},
                            VK_PIPELINE_STAGE_EARLY_FRAGMENT_TESTS_BIT,
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
