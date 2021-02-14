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
#include "window.h"

#include "genesis/core/asserts.h"
#include "genesis/core/log.h"

#include <SDL.h>

namespace GE::SDL {

Window::Window(settings_t settings)
    : m_settings{std::move(settings)}
{
    auto flags = SDL_WINDOW_RESIZABLE | SDL_WINDOW_ALLOW_HIGHDPI | SDL_WINDOW_SHOWN;

    m_window = SDL_CreateWindow(m_settings.title.c_str(), SDL_WINDOWPOS_CENTERED,
                                SDL_WINDOWPOS_CENTERED, m_settings.size.x,
                                m_settings.size.y, flags);
    GE_CORE_ASSERT(m_window, "Failed to create SDL Window: {}", SDL_GetError());

    GE_CORE_INFO("Window '{}' has been created", m_settings.title);
}

Window::~Window()
{
    if (m_window != nullptr) {
        SDL_DestroyWindow(m_window);
        GE_CORE_INFO("Window '{}' has been destroyed", m_settings.title);
    }
}

bool Window::initialize()
{
    GE_CORE_INFO("Initializing SDL Window...");

    if (SDL_Init(SDL_INIT_VIDEO) != 0) {
        GE_CORE_ERR("Failed to initialize SDL: {}", SDL_GetError());
        return false;
    }

    return true;
}

void Window::shutdown()
{
    GE_CORE_INFO("Shutting down SDL Window...");
    SDL_Quit();
}

void Window::setVSync([[maybe_unused]] bool enabled) {}

} // namespace GE::SDL
