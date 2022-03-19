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

// NOLINTNEXTLINE(llvm-header-guard)
#ifndef GENESIS_GRAPHICS_VULKAN_RENDERERS_WINDOW_RENDERER_H_
#define GENESIS_GRAPHICS_VULKAN_RENDERERS_WINDOW_RENDERER_H_

#include "renderer_base.h"

#include <genesis/core/memory.h>
#include <genesis/math/types.h>

#include <vulkan/vulkan.h>

namespace GE {
class Event;
class WindowResizedEvent;
} // namespace GE

namespace GE::Vulkan {

class SwapChain;

class WindowRenderer: public RendererBase
{
public:
    WindowRenderer(Shared<Device> device, VkSurfaceKHR surface, Vec2 window_size);
    ~WindowRenderer();

    bool beginFrame(ClearMode clear_mode) override;
    void endFrame() override;
    void swapBuffers() override;

    Scoped<GE::Pipeline> createPipeline(const GE::pipeline_config_t& config) override;

    void onEvent(Event* event) override;
    bool onWindowResized(const WindowResizedEvent& event);

    SwapChain* swapChain() const { return m_swap_chain.get(); }

    VkRenderPass renderPass(ClearMode clear_mode) const
    {
        return m_render_passes[clear_mode];
    }

    uint8_t MSAASamples() const { return m_msaa_samples; }

private:
    std::vector<VkAttachmentDescription> createAttachmentDescriptions();
    void createRenderPasses();
    void createSwapChain();

    VkCommandBuffer cmdBuffer() const override;
    VkFramebuffer currentFramebuffer() const override;
    VkExtent2D extent() const override;
    VkViewport viewport() const override;

    VkSurfaceKHR m_surface{VK_NULL_HANDLE};
    Vec2 m_window_size{0.0f, 0.0f};
    uint8_t m_msaa_samples{1};

    bool m_is_framebuffer_resized{false};
    Scoped<SwapChain> m_swap_chain;
};

} // namespace GE::Vulkan

#endif // GENESIS_GRAPHICS_VULKAN_RENDERERS_WINDOW_RENDERER_H_
