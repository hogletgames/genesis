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
#ifndef GENESIS_GRAPHICS_VULKAN_SDL_GUI_H_
#define GENESIS_GRAPHICS_VULKAN_SDL_GUI_H_

#include <genesis/gui/context.h>

#include <vulkan/vulkan.h>

struct SDL_Window;

namespace GE::Vulkan {
class GraphicsContext;
} // namespace GE::Vulkan

namespace GE::Vulkan::SDL {

class GE_API GUIContext: public GUI::Context
{
public:
    GUIContext(GraphicsContext* render_context, SDL_Window* window);
    ~GUIContext();

    void begin() override;
    void end() override;

    void draw(GPUCommandQueue* queue) override;

private:
    void createDescriptorPool(VkDevice device);
    void destroyVulkanHandles();

    bool isDockingEnabled() const;
    bool isViewportEnabled() const;

    GraphicsContext* m_render_context{nullptr};
    SDL_Window* m_window{nullptr};

    VkDescriptorPool m_descriptor_pool{VK_NULL_HANDLE};
};

} // namespace GE::Vulkan::SDL

#endif // GENESIS_GRAPHICS_VULKAN_SDL_GUI_H_