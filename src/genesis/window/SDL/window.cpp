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
#include "genesis/core/utils.h"

#include <SDL.h>

namespace {

#ifndef GE_DISABLE_DEBUG
const char* categoryToString(int category)
{
    const char* default_category = "Unknown";
    static const std::unordered_map<int, const char*> cat_to_str = {
        {SDL_LOG_CATEGORY_APPLICATION, "App"}, {SDL_LOG_CATEGORY_ERROR, "Error"},
        {SDL_LOG_CATEGORY_ASSERT, "Assert"},   {SDL_LOG_CATEGORY_SYSTEM, "System"},
        {SDL_LOG_CATEGORY_AUDIO, "Audio"},     {SDL_LOG_CATEGORY_VIDEO, "Video"},
        {SDL_LOG_CATEGORY_RENDER, "Render"},   {SDL_LOG_CATEGORY_INPUT, "Input"},
        {SDL_LOG_CATEGORY_TEST, "Test"},       {SDL_LOG_CATEGORY_CUSTOM, "Custom"}};

    return GE::toType(cat_to_str, category, default_category);
}

void debugCallback([[maybe_unused]] void* userdata, int category,
                   SDL_LogPriority priority, const char* message)
{
    const char* category_str = categoryToString(category);
    const char* pattern = "[SDL {}]: {}";

    switch (priority) {
        case SDL_LOG_PRIORITY_VERBOSE:
            GE_CORE_TRACE(pattern, category_str, message);
            break;
        case SDL_LOG_PRIORITY_DEBUG: GE_CORE_DBG(pattern, category_str, message); break;
        case SDL_LOG_PRIORITY_INFO: GE_CORE_INFO(pattern, category_str, message); break;
        case SDL_LOG_PRIORITY_WARN: GE_CORE_WARN(pattern, category_str, message); break;
        case SDL_LOG_PRIORITY_ERROR: GE_CORE_ERR(pattern, category_str, message); break;
        case SDL_LOG_PRIORITY_CRITICAL:
            GE_CORE_CRIT(pattern, category_str, message);
            break;
        default: GE_CORE_ERR("[SDL {}/Unknown]: {}", category_str, message); break;
    }
}
#endif // GE_DISABLE_DEBUG

} // namespace

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

#ifndef GE_DISABLE_DEBUG
    SDL_LogSetAllPriority(SDL_LOG_PRIORITY_INFO);
    SDL_LogSetOutputFunction(debugCallback, nullptr);
#endif // GE_DISABLE_DEBUG

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
