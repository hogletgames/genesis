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

#include "render_context.h"
#include "device.h"
#include "instance.h"
#include "sdl_platform_window.h"
#include "vulkan_exception.h"

#include "genesis/core/log.h"

namespace GE::Vulkan {

RenderContext::RenderContext() = default;

RenderContext::~RenderContext() = default;

bool RenderContext::initialize(void* window)
{
    GE_CORE_INFO("Initializing Vulkan Context...");

    try {
        m_window = makeScoped<SDL::PlatformWindow>(reinterpret_cast<SDL_Window*>(window));
        Instance::registerContext(this);
        m_surface = m_window->createSurface(Instance::instance());

        m_device = makeScoped<Device>(this);
    } catch (const Vulkan::Exception& e) {
        GE_CORE_ERR("Failed to initialize Vulkan Render Context: {}", e.what());
        shutdown();
        return false;
    }

    return true;
}

void RenderContext::shutdown()
{
    GE_CORE_INFO("Shutdown Vulkan Context");

    m_device.reset();

    destroyVulkanHandles();
    Instance::dropContext(this);
    m_window.reset();
}

void RenderContext::destroyVulkanHandles()
{
    vkDestroySurfaceKHR(Instance::instance(), m_surface, nullptr);
    m_surface = VK_NULL_HANDLE;
}

} // namespace GE::Vulkan
