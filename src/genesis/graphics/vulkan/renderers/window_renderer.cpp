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

#include "window_renderer.h"
#include "descriptor_pool.h"
#include "device.h"
#include "image.h"
#include "pipeline.h"
#include "pipeline_barrier.h"
#include "pipeline_config.h"
#include "swap_chain.h"
#include "utils.h"

#include "genesis/core/enum.h"
#include "genesis/core/log.h"
#include "genesis/core/utils.h"
#include "genesis/graphics/render_command.h"
#include "genesis/window/events/event_dispatcher.h"
#include "genesis/window/events/window_events.h"

namespace {

VkViewport toFlippedVkViewport(const VkExtent2D& extent)
{
    return {0.0f,                                      // x
            static_cast<float>(extent.height),         // y
            static_cast<float>(extent.width),          // width
            -1.0f * static_cast<float>(extent.height), // height
            0.0f,                                      // min depth
            1.0f};                                     // max depth
}

} // namespace

namespace GE::Vulkan {

WindowRenderer::WindowRenderer(Shared<Device> device, const config_t& config)
    : RendererBase{std::move(device)}
    , m_surface{config.surface}
    , m_window_size{config.window_size}
    , m_msaa_samples{config.msaa_samples}
{
    createSwapChain();
    createCommandBuffers(m_swap_chain->imageCount());
    createRenderingAttachments();
}

WindowRenderer::~WindowRenderer() = default;

bool WindowRenderer::beginFrame(ClearMode clear_mode)
{
    auto acquire_result = m_swap_chain->acquireNextImage();

    if (acquire_result == VK_ERROR_OUT_OF_DATE_KHR) {
        m_swap_chain->recreate(m_window_size);
        return false;
    }

    if (acquire_result != VK_SUCCESS && acquire_result != VK_SUBOPTIMAL_KHR) {
        GE_CORE_ERR("Failed to acquire Swap Chain image: {}", toString(acquire_result));
        return false;
    }

    if (!beginRendering(clear_mode)) {
        return false;
    }

    updateDynamicState();
    return true;
}

void WindowRenderer::endFrame()
{
    VkCommandBuffer* cmd = &m_cmd_buffers[m_swap_chain->currentImageIndex()];
    m_render_command.submit(*cmd);
    endRendering();
}

void WindowRenderer::swapBuffers()
{
    VkCommandBuffer* cmd = &m_cmd_buffers[m_swap_chain->currentImageIndex()];
    m_swap_chain->submitCommandBuffer(cmd);

    auto present_result = m_swap_chain->presentImage();

    if (present_result == VK_ERROR_OUT_OF_DATE_KHR || present_result == VK_SUBOPTIMAL_KHR ||
        m_is_framebuffer_resized) {
        m_swap_chain->recreate(m_window_size);
        m_is_framebuffer_resized = false;
    } else if (present_result != VK_SUCCESS) {
        GE_CORE_ERR("Failed to present Swap Chain Image");
    }

    m_descriptor_pool->reset();
}

Scoped<GE::Pipeline> WindowRenderer::createPipeline(const GE::pipeline_config_t& config)
{
    auto vulkan_config = Vulkan::Pipeline::createDefaultConfig(config);
    vulkan_config.pipeline_cache = m_pipeline_cache;
    vulkan_config.color_formats = {m_swap_chain->colorFormat()};
    vulkan_config.depth_format = m_swap_chain->depthFormat();
    vulkan_config.front_face = VK_FRONT_FACE_COUNTER_CLOCKWISE;
    vulkan_config.msaa_samples = toVkSampleCountFlag(m_msaa_samples);
    vulkan_config.descriptor_pool = m_descriptor_pool;
    return tryMakeScoped<Vulkan::Pipeline>(m_device, vulkan_config);
}

void WindowRenderer::onEvent(Event* event)
{
    EventDispatcher dispatcher{event};
    dispatcher.dispatch<WindowResizedEvent>(GE_EVENT_MEM_FN(onWindowResized));
}

bool WindowRenderer::onWindowResized(const WindowResizedEvent& event)
{
    m_window_size = event.size();
    m_is_framebuffer_resized = true;
    return false;
}

void WindowRenderer::createSwapChain()
{
    SwapChain::options_t options{};
    options.surface = m_surface;
    options.window_size = m_window_size;
    options.msaa_samples = m_msaa_samples;
    m_swap_chain = makeScoped<SwapChain>(m_device, options);
}

void WindowRenderer::createRenderingAttachments()
{
    VkRenderingAttachmentInfoKHR color_attachment{};
    color_attachment.sType = VK_STRUCTURE_TYPE_RENDERING_ATTACHMENT_INFO;
    color_attachment.imageLayout = VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL;
    color_attachment.loadOp = VK_ATTACHMENT_LOAD_OP_DONT_CARE;
    color_attachment.storeOp = VK_ATTACHMENT_STORE_OP_STORE;
    color_attachment.clearValue = toVkClearColorValue(Vec4{0.0f, 0.0f, 0.0f, 1.0f});

    if (m_msaa_samples > 1) {
        color_attachment.resolveMode = VK_RESOLVE_MODE_AVERAGE_BIT;
        color_attachment.resolveImageLayout = VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL;
    }

    VkRenderingAttachmentInfo depth_attachment{};
    depth_attachment.sType = VK_STRUCTURE_TYPE_RENDERING_ATTACHMENT_INFO;
    depth_attachment.imageLayout = VK_IMAGE_LAYOUT_DEPTH_STENCIL_ATTACHMENT_OPTIMAL;
    depth_attachment.loadOp = VK_ATTACHMENT_LOAD_OP_DONT_CARE;
    depth_attachment.storeOp = VK_ATTACHMENT_STORE_OP_DONT_CARE;
    depth_attachment.clearValue = toVkClearDepthStencilValue(1.0f);

    m_color_rendering_attachments = {color_attachment};
    m_depth_rendering_attachment = depth_attachment;
}

void WindowRenderer::transitImageLayoutBeforeRendering(VkCommandBuffer cmd)
{
    VkImageMemoryBarrier barrier{};
    barrier.sType = VK_STRUCTURE_TYPE_IMAGE_MEMORY_BARRIER;
    barrier.srcAccessMask = VK_ACCESS_NONE;
    barrier.dstAccessMask =
        VK_ACCESS_COLOR_ATTACHMENT_WRITE_BIT | VK_ACCESS_COLOR_ATTACHMENT_READ_BIT;
    barrier.oldLayout = VK_IMAGE_LAYOUT_UNDEFINED;
    barrier.newLayout = VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL;
    barrier.srcQueueFamilyIndex = VK_QUEUE_FAMILY_IGNORED;
    barrier.dstQueueFamilyIndex = VK_QUEUE_FAMILY_IGNORED;
    barrier.image = m_swap_chain->currentImage();
    barrier.subresourceRange.aspectMask = toVkAspect(m_swap_chain->colorFormat());
    barrier.subresourceRange.baseMipLevel = 0;
    barrier.subresourceRange.levelCount = 1;
    barrier.subresourceRange.baseArrayLayer = 0;
    barrier.subresourceRange.layerCount = 1;

    PipelineBarrier::submit(cmd, {barrier}, VK_PIPELINE_STAGE_BOTTOM_OF_PIPE_BIT,
                            VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT);
}

void WindowRenderer::transitImageLayoutAfterRendering(VkCommandBuffer cmd)
{
    std::vector<VkImageMemoryBarrier> barriers;

    VkImageMemoryBarrier barrier{};
    barrier.sType = VK_STRUCTURE_TYPE_IMAGE_MEMORY_BARRIER;
    barrier.srcAccessMask = VK_ACCESS_COLOR_ATTACHMENT_WRITE_BIT;
    barrier.dstAccessMask = VK_ACCESS_NONE;
    barrier.oldLayout = VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL;
    barrier.newLayout = VK_IMAGE_LAYOUT_PRESENT_SRC_KHR;
    barrier.srcQueueFamilyIndex = VK_QUEUE_FAMILY_IGNORED;
    barrier.dstQueueFamilyIndex = VK_QUEUE_FAMILY_IGNORED;
    barrier.image = m_swap_chain->currentImage();
    barrier.subresourceRange.aspectMask = toVkAspect(m_swap_chain->colorFormat());
    barrier.subresourceRange.baseMipLevel = 0;
    barrier.subresourceRange.levelCount = 1;
    barrier.subresourceRange.baseArrayLayer = 0;
    barrier.subresourceRange.layerCount = 1;

    PipelineBarrier::submit(cmd, {barrier}, VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT,
                            VK_PIPELINE_STAGE_BOTTOM_OF_PIPE_BIT);
}

VkCommandBuffer WindowRenderer::cmdBuffer() const
{
    return m_cmd_buffers[m_swap_chain->currentImageIndex()];
}

VkExtent2D WindowRenderer::extent() const
{
    return m_swap_chain->extent();
}

VkViewport WindowRenderer::viewport() const
{
    return toFlippedVkViewport(m_swap_chain->extent());
}

const std::vector<VkRenderingAttachmentInfo>&
WindowRenderer::colorRenderingAttachments(ClearMode clear_mode)
{
    bool  should_clear = clear_mode == CLEAR_COLOR || clear_mode == CLEAR_ALL;
    auto& color_attachment = m_color_rendering_attachments[0];

    if (m_msaa_samples > 1) {
        color_attachment.imageView = m_swap_chain->colorMSAAImage()->view();
        color_attachment.resolveImageView = m_swap_chain->currentImageView();
        color_attachment.loadOp = VK_ATTACHMENT_LOAD_OP_CLEAR;
    } else {
        color_attachment.imageView = m_swap_chain->currentImageView();
        color_attachment.loadOp = should_clear ? VK_ATTACHMENT_LOAD_OP_CLEAR
                                               : VK_ATTACHMENT_LOAD_OP_DONT_CARE;
    }

    return m_color_rendering_attachments;
}

std::optional<VkRenderingAttachmentInfo>
WindowRenderer::depthRenderingAttachment(ClearMode clear_mode)
{
    bool should_clear = clear_mode == CLEAR_DEPTH || clear_mode == CLEAR_ALL;

    m_depth_rendering_attachment.imageView = m_swap_chain->depthImage()->view();
    m_depth_rendering_attachment.loadOp = should_clear ? VK_ATTACHMENT_LOAD_OP_CLEAR
                                                       : VK_ATTACHMENT_LOAD_OP_DONT_CARE;

    return m_depth_rendering_attachment;
}

} // namespace GE::Vulkan
