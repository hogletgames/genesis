/*
 * BSD 3-Clause License
 *
 * Copyright (c) 2021, Dmitry Shilnenkov
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
#ifndef GENESIS_RENDERER_VULKAN_RENDER_CONTEXT_H_
#define GENESIS_RENDERER_VULKAN_RENDER_CONTEXT_H_

#include <genesis/renderer/render_context.h>

#include <vulkan/vulkan.h>

#include <vector>

namespace GE::Vulkan {

namespace SDL {
class PlatformWindow;
} // namespace SDL

class RenderContext: public GE::RenderContext
{
public:
    RenderContext();
    ~RenderContext();

    bool initialize(void* window) override;
    void shutdown() override;

    Renderer::API API() const override { return Renderer::API::VULKAN; }

    const Scoped<SDL::PlatformWindow>& platformWindow() const { return m_window; }
    VkSurfaceKHR surface() const { return m_surface; }

private:
    void destroyVulkanHandles();

    Scoped<SDL::PlatformWindow> m_window;
    VkSurfaceKHR m_surface{VK_NULL_HANDLE};
};

} // namespace GE::Vulkan

#endif // GENESIS_RENDERER_VULKAN_RENDER_CONTEXT_H_
