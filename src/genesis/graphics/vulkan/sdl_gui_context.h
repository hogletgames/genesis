/*
 * BSD 3-Clause License
 *
 * Copyright (c) 2021-2022, Dmitry Shilnenkov
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

#pragma once

#include <genesis/core/memory.h>
#include <genesis/gui/context.h>
#include <vulkan/vulkan.h>

namespace GE::Vulkan {
class Device;
class Texture;
class WindowRenderer;
} // namespace GE::Vulkan

namespace GE::Vulkan::SDL {

class EventHandler;

class GE_API GUIContext: public GUI::Context
{
public:
    GUIContext(void* window, Shared<Device> device, WindowRenderer* window_renderer);
    ~GUIContext();

    void begin() override;
    void end() override;

    void draw(GPUCommandQueue* queue) override;

    GUI::EventHandler* eventHandler() override;

private:
    void createDescriptorPool();
    void destroyVulkanHandles();

    bool isDockingEnabled() const;
    bool isViewportEnabled() const;

    Shared<Device>                    m_device;
    Scoped<Vulkan::SDL::EventHandler> m_event_handler;
    VkDescriptorPool                  m_descriptor_pool{VK_NULL_HANDLE};
};

VkDescriptorSet createGuiTextureID(const Vulkan::Texture& texture);
void destroyGuiTextureID(VkDescriptorSet texture_id);

} // namespace GE::Vulkan::SDL
