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

#include "genesis/core/exception.h"
#include "genesis/core/format.h"
#include "genesis/core/log.h"
#include "genesis/core/utils.h"
#include "genesis/graphics/render_context.h"
#include "genesis/window/events/key_events.h"
#include "genesis/window/events/mouse_events.h"
#include "genesis/window/events/window_events.h"
#include "genesis/window/input.h"

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

int renderAPIToWindowFlag(GE::Graphics::API api)
{
    using API = GE::Graphics::API;

    static constexpr int default_flag{0};
    static std::unordered_map<API, int> api_to_flag = {{API::VULKAN, SDL_WINDOW_VULKAN}};
    int flag = GE::toType(api_to_flag, api, default_flag);

    if (flag == default_flag) {
        GE_CORE_ERR("Failed to get SDL Window Flag: unsupported API '{}'", api);
    }

    return flag;
}

} // namespace

namespace GE::SDL {

Window::Window(settings_t settings)
    : m_settings{std::move(settings)}
{
    auto flags = SDL_WINDOW_RESIZABLE | SDL_WINDOW_ALLOW_HIGHDPI | SDL_WINDOW_SHOWN |
                 renderAPIToWindowFlag(settings.renderer.api);

    m_window = SDL_CreateWindow(m_settings.title.c_str(), SDL_WINDOWPOS_CENTERED,
                                SDL_WINDOWPOS_CENTERED, m_settings.size.x,
                                m_settings.size.y, flags);

    if (m_window == nullptr) {
        auto error = GE_FMTSTR("Failed to create SDL Window: {}", SDL_GetError());
        throw Exception{error};
    }

    m_context = RenderContext::create(settings.renderer.api);

    if (!m_context->initialize(m_window)) {
        throw Exception{"Failed to initialize Render Context"};
    }

    GE_CORE_INFO("Window '{}' has been created", m_settings.title);
}

Window::~Window()
{
    if (m_window != nullptr) {
        m_context->shutdown();
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
    GE_CORE_INFO("Shutdown SDL Window");
    SDL_Quit();
}

void Window::onUpdate()
{
    m_context->drawFrame();
}

void Window::attachEventListener(EventListener* listener)
{
    m_event_listeners.push_back(listener);
}

void Window::detachEventListener(EventListener* listener)
{
    m_event_listeners.remove(listener);
}

void Window::setVSync([[maybe_unused]] bool enabled) {}

void Window::emitEvent(Event* event)
{
    for (auto* listener : m_event_listeners) {
        listener->onEvent(event);
    }
}

void Window::pollEvents()
{
    SDL_Event sdl_event{};

    while (SDL_PollEvent(&sdl_event) != 0) {
        switch (sdl_event.type) {
            case SDL_MOUSEMOTION:
            case SDL_MOUSEWHEEL:
            case SDL_MOUSEBUTTONDOWN:
            case SDL_MOUSEBUTTONUP: {
                onMouseEvent(sdl_event);
                break;
            }
            case SDL_KEYDOWN:
            case SDL_KEYUP:
            case SDL_TEXTINPUT: {
                onKeyboardEvent(sdl_event);
                break;
            }
            case SDL_WINDOWEVENT: {
                onWindowEvent(sdl_event);
                break;
            }
            case SDL_QUIT: {
                WindowClosedEvent event{};
                emitEvent(&event);
                break;
            }
            default: break;
        }
    }
}

void Window::onMouseEvent(const SDL_Event& sdl_event)
{
    switch (sdl_event.type) {
        case SDL_MOUSEMOTION: {
            MouseMovedEvent event{{sdl_event.motion.x, sdl_event.motion.y}};
            emitEvent(&event);
            break;
        }
        case SDL_MOUSEWHEEL: {
            MouseScrolledEvent event{{sdl_event.wheel.x, sdl_event.wheel.y}};
            emitEvent(&event);
            break;
        }
        case SDL_MOUSEBUTTONDOWN: {
            MouseButton button = Input::fromNativeButton(sdl_event.button.button);
            MouseButtonPressedEvent event{button};
            emitEvent(&event);
            break;
        }
        case SDL_MOUSEBUTTONUP: {
            MouseButton button = Input::fromNativeButton(sdl_event.button.button);
            MouseButtonReleasedEvent event{button};
            emitEvent(&event);
            break;
        }
        default: break;
    }
}

void Window::onKeyboardEvent(const SDL_Event& sdl_event)
{
    switch (sdl_event.type) {
        case SDL_KEYDOWN: {
            KeyCode code = Input::fromNativeKeyCode(sdl_event.key.keysym.sym);
            KeyModFlags mod = Input::fromNativeKeyMod(sdl_event.key.keysym.mod);
            uint32_t repeat_count = sdl_event.key.repeat;
            KeyPressedEvent event{code, mod, repeat_count};
            emitEvent(&event);
            break;
        }
        case SDL_KEYUP: {
            KeyCode code = Input::fromNativeKeyCode(sdl_event.key.keysym.sym);
            KeyModFlags mod = Input::fromNativeKeyMod(sdl_event.key.keysym.mod);
            KeyReleasedEvent event{code, mod};
            emitEvent(&event);
            break;
        }
        case SDL_TEXTINPUT: {
            KeyTypedEvent event{sdl_event.text.text};
            emitEvent(&event);
            break;
        }
        default: break;
    }
}

void Window::onWindowEvent(const SDL_Event& sdl_event)
{
    switch (sdl_event.window.event) {
        case SDL_WINDOWEVENT_RESIZED: {
            Vec2 size{sdl_event.window.data1, sdl_event.window.data2};
            m_settings.size = size;
            WindowResizedEvent event{size};
            emitEvent(&event);
            break;
        }
        case SDL_WINDOWEVENT_CLOSE: {
            WindowClosedEvent event{};
            emitEvent(&event);
            break;
        }
        case SDL_WINDOWEVENT_MAXIMIZED: {
            WindowMaximizedEvent event{};
            emitEvent(&event);
            break;
        }
        case SDL_WINDOWEVENT_MINIMIZED: {
            WindowMinimizedEvent event{};
            emitEvent(&event);
            break;
        }
        case SDL_WINDOWEVENT_SHOWN:
        case SDL_WINDOWEVENT_RESTORED: {
            WindowRestoredEvent event{};
            emitEvent(&event);
            break;
        }
        default: break;
    }
}

} // namespace GE::SDL
