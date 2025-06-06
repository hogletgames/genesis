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

#pragma once

#include <genesis/core/memory.h>
#include <genesis/graphics/graphics_context.h>

#include <vulkan/vulkan.h>

#include <vector>

namespace GE::Vulkan {

namespace SDL {
class GUIContext;
} // namespace SDL

class Device;
class WindowRenderer;

class GraphicsContext: public GE::GraphicsContext
{
public:
    GraphicsContext();
    ~GraphicsContext();

    bool initialize(const config_t& config) override;
    void shutdown() override;

    GE::GraphicsFactory* factory() override { return m_factory.get(); }
    GE::Renderer* windowRenderer() override;
    GE::GUI::Context* gui() override { return m_gui.get(); }

    const limits_t& limits() const override;

private:
    Scoped<WindowRenderer> createWindowRenderer(const config_t& renderer_config);

    void clearResources();
    void destroyVulkanHandles();

    VkSurfaceKHR           m_surface{VK_NULL_HANDLE};
    Shared<Vulkan::Device> m_device;

    Scoped<GE::GraphicsFactory> m_factory;
    Scoped<WindowRenderer>      m_window_renderer;
    Scoped<GE::GUI::Context>    m_gui;
};

} // namespace GE::Vulkan
