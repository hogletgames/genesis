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

#include "graphics_context.h"
#include "device.h"
#include "graphics_factory.h"
#include "instance.h"
#include "renderers/window_renderer.h"
#include "sdl_gui_context.h"
#include "vulkan_exception.h"

#include "genesis/core/log.h"

#include <SDL_vulkan.h>

namespace {

VkSurfaceKHR createSurface(void* window)
{
    VkSurfaceKHR surface{VK_NULL_HANDLE};
    auto* sdl_window = reinterpret_cast<SDL_Window*>(window);

    if (::SDL_Vulkan_CreateSurface(sdl_window, GE::Vulkan::Instance::instance(),
                                   &surface) == SDL_FALSE) {
        throw GE::Vulkan::Exception{"Failed to create Vulkan SDL Surface"};
    }

    return surface;
}

inline GE::Vec2 getWindowSize(void* window)
{
    auto* sdl_window = reinterpret_cast<SDL_Window*>(window);
    int width{0};
    int height{0};
    ::SDL_GetWindowSize(sdl_window, &width, &height);
    return {width, height};
}

} // namespace

namespace GE::Vulkan {

GraphicsContext::GraphicsContext() = default;

GraphicsContext::~GraphicsContext()
{
    clearResources();
}

bool GraphicsContext::initialize(const config_t& config)
{
    GE_CORE_INFO("Initializing Vulkan Context...");
    Instance::initialize(config.window, config.app_name);

    m_surface = createSurface(config.window);
    Vec2 window_size = getWindowSize(config.window);

    try {
        m_device = makeScoped<Device>(m_surface);
        m_window_renderer = makeScoped<WindowRenderer>(m_device, m_surface, window_size);
        m_factory = makeScoped<Vulkan::GraphicsFactory>(m_device);
        m_gui =
            makeScoped<SDL::GUIContext>(config.window, m_device, m_window_renderer.get());
    } catch (const Vulkan::Exception& e) {
        GE_CORE_ERR("Failed to initialize context: {}", e.what());
        clearResources();
        return false;
    }

    return true;
}

void GraphicsContext::shutdown()
{
    clearResources();
}

Renderer* GraphicsContext::windowRenderer()
{
    return m_window_renderer.get();
}

void GraphicsContext::clearResources()
{
    if (m_device == nullptr) {
        Instance::shutdown();
        return;
    }

    GE_CORE_INFO("Shutdown Vulkan Context");

    m_device->waitIdle();

    m_gui.reset();
    m_factory.reset();
    m_window_renderer.reset();
    m_device.reset();
    destroyVulkanHandles();

    Instance::shutdown();
}

void GraphicsContext::destroyVulkanHandles()
{
    vkDestroySurfaceKHR(Instance::instance(), m_surface, nullptr);
    m_surface = VK_NULL_HANDLE;
}

} // namespace GE::Vulkan
