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

#include "sdl_render_context.h"

// To suppress clang-tidy warnings about missed `memcpy()` in SDL2
#include <cstring>

#include <SDL_vulkan.h>

namespace {

inline SDL_Window* toSDLWindow(void* window)
{
    return reinterpret_cast<SDL_Window*>(window);
}

} // namespace

namespace GE::Vulkan::SDL {

std::vector<const char*> RenderContext::getWindowExtensions(void* window) const
{
    auto* sdl_window = toSDLWindow(window);

    uint32_t name_count{0};
    SDL_Vulkan_GetInstanceExtensions(sdl_window, &name_count, nullptr);

    std::vector<const char*> names(name_count);
    SDL_Vulkan_GetInstanceExtensions(sdl_window, &name_count, names.data());

    return names;
}

const char* RenderContext::getAppName(void* window) const
{
    return SDL_GetWindowTitle(toSDLWindow(window));
}

bool RenderContext::createSurface(void* window)
{
    return SDL_Vulkan_CreateSurface(toSDLWindow(window), m_instance, &m_surface) ==
           SDL_TRUE;
}

} // namespace GE::Vulkan::SDL
