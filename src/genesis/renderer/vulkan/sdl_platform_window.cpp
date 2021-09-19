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

#include "sdl_platform_window.h"
#include "vulkan_exception.h"

#include "genesis/core/format.h"

// To suppress clang-tidy warnings about missed `memcpy()` in SDL2
#include <cstring>

#include <SDL_vulkan.h>

namespace GE::Vulkan::SDL {

PlatformWindow::PlatformWindow(SDL_Window *window)
    : m_window{window}
{}

std::vector<const char *> PlatformWindow::vulkanExtensions()
{
    uint32_t name_count{0};
    SDL_Vulkan_GetInstanceExtensions(m_window, &name_count, nullptr);

    std::vector<const char *> names(name_count);
    SDL_Vulkan_GetInstanceExtensions(m_window, &name_count, names.data());

    return names;
}

VkSurfaceKHR PlatformWindow::createSurface(VkInstance instance)
{
    VkSurfaceKHR surface{VK_NULL_HANDLE};

    if (SDL_Vulkan_CreateSurface(m_window, instance, &surface) == SDL_FALSE) {
        auto error = GE_FMTSTR("Failed to create Vulkan SDL Surface: {}", SDL_GetError());
        throw Vulkan::Exception{error};
    }

    return surface;
}

const char *PlatformWindow::title()
{
    return SDL_GetWindowTitle(m_window);
}

Vec2 PlatformWindow::windowSize()
{
    int width{0};
    int height{0};
    SDL_GetWindowSize(m_window, &width, &height);
    return {width, height};
}

} // namespace GE::Vulkan::SDL
