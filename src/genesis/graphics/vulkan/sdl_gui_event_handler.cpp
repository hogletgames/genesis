/*
 * BSD 3-Clause License
 *
 * Copyright (c) 2022, Dmitry Shilnenkov
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

#include "sdl_gui_event_handler.h"

#include "genesis/app/application.h"
#include "genesis/window/input.h"

#include <SDL.h>
#include <imgui_impl_sdl2.h>

namespace {

bool onKeyEvent(const GE::KeyEvent& event, bool is_pressed)
{
    auto& io = ImGui::GetIO();
    int   key = static_cast<int>(event.getCode());

    io.KeysDown[key] = is_pressed;
    io.KeyShift = (event.getMod() & GE::KeyModFlags::SHIFT_BIT) != 0;
    io.KeyCtrl = (event.getMod() & GE::KeyModFlags::CTRL_BIT) != 0;
    io.KeyAlt = (event.getMod() & GE::KeyModFlags::ALT_BIT) != 0;
#ifdef GE_PLATFORM_WINDOWS
    io.KeySuper = false;
#else
    io.KeySuper = (event.getMod() & GE::KeyModFlags::SUPER_BIT) != 0;
#endif

    return false;
}

bool onWindowEvent(SDL_WindowEventID type, Uint32 window_id)
{
    SDL_Event event{};
    event.type = type;
    event.window.windowID = window_id;
    ImGui_ImplSDL2_ProcessEvent(&event);
    return false;
}

} // namespace

namespace GE::Vulkan::SDL {

bool EventHandler::onKeyPressedEvent(const KeyPressedEvent& event)
{
    return onKeyEvent(event, true);
}

bool EventHandler::onKeyReleasedEvent(const KeyReleasedEvent& event)
{
    return onKeyEvent(event, false);
}

bool EventHandler::onKeyTypedEvent(const KeyTypedEvent& event)
{
    auto& io = ImGui::GetIO();
    io.AddInputCharactersUTF8(event.getText());
    return false;
}

bool EventHandler::onMouseButtonPressedEvent(const MouseButtonPressedEvent& event)
{
    ::SDL_Event sdl_event{};
    sdl_event.type = SDL_MOUSEBUTTONDOWN;
    sdl_event.button.button = Input::toNativeButton(event.getMouseButton());
    ImGui_ImplSDL2_ProcessEvent(&sdl_event);
    return false;
}

bool EventHandler::onMouseButtonReleasedEvent(const MouseButtonReleasedEvent& event)
{
    ::SDL_Event sdl_event{};
    sdl_event.type = SDL_MOUSEBUTTONUP;
    sdl_event.button.button = Input::toNativeButton(event.getMouseButton());
    ImGui_ImplSDL2_ProcessEvent(&sdl_event);
    return false;
}

bool EventHandler::onMouseMovedEvent(const MouseMovedEvent& event)
{
    auto& io = ImGui::GetIO();
    auto  mouse_pos = event.getPosition();

    if ((io.ConfigFlags & ImGuiConfigFlags_ViewportsEnable) != 0) {
        mouse_pos += Application::window()->position();
    }

    io.AddMousePosEvent(mouse_pos.x, mouse_pos.y);
    return false;
}

bool EventHandler::onMouseScrolledEvent(const MouseScrolledEvent& event)
{
    auto& io = ImGui::GetIO();
    float offset_x = event.getOffset().x;
    float offset_y = event.getOffset().y;

    if (offset_x > 0.0f) {
        io.MouseWheelH += 1;
    }

    if (offset_x < 0) {
        io.MouseWheelH -= 1;
    }

    if (offset_y > 0) {
        io.MouseWheel += 1;
    }

    if (offset_y < 0) {
        io.MouseWheel -= 1;
    }

    return false;
}

bool EventHandler::onWindowMovedEvent(const WindowMovedEvent& event)
{
    return onWindowEvent(SDL_WINDOWEVENT_MOVED, event.id());
}

bool EventHandler::onWindowResizedEvent(const WindowResizedEvent& event)
{
    return onWindowEvent(SDL_WINDOWEVENT_RESIZED, event.id());
}

bool EventHandler::onWindowEnteredEvent(const WindowEnteredEvent& event)
{
    return onWindowEvent(SDL_WINDOWEVENT_ENTER, event.id());
}

bool EventHandler::onWindowLeftEvent(const WindowLeftEvent& event)
{
    return onWindowEvent(SDL_WINDOWEVENT_LEAVE, event.id());
}

bool EventHandler::onWindowFocusGainedEvent(const WindowFocusGainedEvent& event)
{
    return onWindowEvent(SDL_WINDOWEVENT_FOCUS_GAINED, event.id());
}

bool EventHandler::onWindowFocusLostEvent(const WindowFocusLostEvent& event)
{
    return onWindowEvent(SDL_WINDOWEVENT_FOCUS_LOST, event.id());
}

bool EventHandler::onWindowClosedEvent(const WindowClosedEvent& event)
{
    return onWindowEvent(SDL_WINDOWEVENT_CLOSE, event.id());
}

} // namespace GE::Vulkan::SDL
