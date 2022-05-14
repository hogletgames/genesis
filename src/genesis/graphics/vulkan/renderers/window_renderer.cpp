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
#include "device.h"
#include "pipeline.h"
#include "swap_chain.h"
#include "vulkan_exception.h"

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

WindowRenderer::WindowRenderer(Shared<Device> device, VkSurfaceKHR surface,
                               Vec2 window_size)
    : RendererBase{std::move(device)}
    , m_surface{surface}
    , m_window_size{window_size}
{
    m_clear_values = {{}, {}};

    createRenderPasses();
    createSwapChain();
    createCommandBuffers(m_swap_chain->imageCount());
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

    if (!beginRenderPass(clear_mode)) {
        return false;
    }

    updateDynamicState();
    return true;
}

void WindowRenderer::endFrame()
{
    VkCommandBuffer* cmd = &m_cmd_buffers[m_swap_chain->currentImage()];
    m_render_command.submit(*cmd);

    if (!endRenderPass()) {
        return;
    }

    m_swap_chain->submitCommandBuffer(cmd);
}

void WindowRenderer::swapBuffers()
{
    auto present_result = m_swap_chain->presentImage();

    if (present_result == VK_ERROR_OUT_OF_DATE_KHR ||
        present_result == VK_SUBOPTIMAL_KHR || m_is_framebuffer_resized) {
        m_swap_chain->recreate(m_window_size);
        m_is_framebuffer_resized = false;
    } else if (present_result != VK_SUCCESS) {
        GE_CORE_ERR("Failed to present Swap Chain Image");
    }
}

Scoped<GE::Pipeline> WindowRenderer::createPipeline(const GE::pipeline_config_t& config)
{
    auto vulkan_config = Vulkan::Pipeline::createDefaultConfig(config);
    vulkan_config.pipeline_cache = m_pipeline_cache;
    vulkan_config.render_pass = m_render_passes[CLEAR_ALL];
    vulkan_config.front_face = VK_FRONT_FACE_COUNTER_CLOCKWISE;
    return tryMakeScoped<Vulkan::Pipeline>(m_device, vulkan_config);
}

void WindowRenderer::onEvent(Event* event)
{
    EventDispatcher dispatcher{event};
    dispatcher.dispatch<WindowResizedEvent>(GE_EVENT_MEM_FN(onWindowResized));
}

bool WindowRenderer::onWindowResized(const WindowResizedEvent& event)
{
    m_window_size = event.getSize();
    m_is_framebuffer_resized = true;
    return false;
}

void WindowRenderer::createRenderPasses()
{
    VkFormat color_format =
        SwapChain::chooseSurfaceFormat(m_device->swapChainDetails().formats).format;
    VkFormat depth_format = SwapChain::choseDepthFormat(m_device.get());

    VkAttachmentDescription color_attachment{};
    color_attachment.format = color_format;
    color_attachment.samples = VK_SAMPLE_COUNT_1_BIT;
    color_attachment.loadOp = VK_ATTACHMENT_LOAD_OP_CLEAR;
    color_attachment.storeOp = VK_ATTACHMENT_STORE_OP_STORE;
    color_attachment.stencilLoadOp = VK_ATTACHMENT_LOAD_OP_DONT_CARE;
    color_attachment.stencilStoreOp = VK_ATTACHMENT_STORE_OP_DONT_CARE;
    color_attachment.initialLayout = VK_IMAGE_LAYOUT_UNDEFINED;
    color_attachment.finalLayout = VK_IMAGE_LAYOUT_PRESENT_SRC_KHR;

    VkAttachmentDescription depth_attachment{};
    depth_attachment.format = depth_format;
    depth_attachment.samples = VK_SAMPLE_COUNT_1_BIT;
    depth_attachment.loadOp = VK_ATTACHMENT_LOAD_OP_CLEAR;
    depth_attachment.storeOp = VK_ATTACHMENT_STORE_OP_DONT_CARE;
    depth_attachment.stencilLoadOp = VK_ATTACHMENT_LOAD_OP_DONT_CARE;
    depth_attachment.stencilStoreOp = VK_ATTACHMENT_STORE_OP_DONT_CARE;
    depth_attachment.initialLayout = VK_IMAGE_LAYOUT_UNDEFINED;
    depth_attachment.finalLayout = VK_IMAGE_LAYOUT_DEPTH_STENCIL_ATTACHMENT_OPTIMAL;

    std::vector<VkAttachmentDescription> description = {
        color_attachment,
        depth_attachment,
    };

    description[0].loadOp = VK_ATTACHMENT_LOAD_OP_DONT_CARE;
    description[1].loadOp = VK_ATTACHMENT_LOAD_OP_DONT_CARE;
    m_render_passes[CLEAR_NONE] = createRenderPass(description);

    description[0].loadOp = VK_ATTACHMENT_LOAD_OP_CLEAR;
    description[1].loadOp = VK_ATTACHMENT_LOAD_OP_DONT_CARE;
    m_render_passes[CLEAR_COLOR] = createRenderPass(description);

    description[0].loadOp = VK_ATTACHMENT_LOAD_OP_DONT_CARE;
    description[1].loadOp = VK_ATTACHMENT_LOAD_OP_CLEAR;
    m_render_passes[CLEAR_DEPTH] = createRenderPass(description);

    description[0].loadOp = VK_ATTACHMENT_LOAD_OP_CLEAR;
    description[1].loadOp = VK_ATTACHMENT_LOAD_OP_CLEAR;
    m_render_passes[CLEAR_ALL] = createRenderPass(description);
}

void WindowRenderer::createSwapChain()
{
    SwapChain::options_t options{};
    options.surface = m_surface;
    options.render_pass = m_render_passes[CLEAR_ALL];
    options.window_size = m_window_size;
    m_swap_chain = makeScoped<SwapChain>(m_device, options);
}

VkCommandBuffer WindowRenderer::cmdBuffer() const
{
    return m_cmd_buffers[m_swap_chain->currentImage()];
}

VkFramebuffer WindowRenderer::currentFramebuffer() const
{
    return m_swap_chain->currentFramebuffer();
}

VkExtent2D WindowRenderer::extent() const
{
    return m_swap_chain->extent();
}

VkViewport WindowRenderer::viewport() const
{
    return toFlippedVkViewport(m_swap_chain->extent());
}

} // namespace GE::Vulkan
