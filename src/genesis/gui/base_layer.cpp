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

#include "base_layer.h"

#include "genesis/core/log.h"
#include "genesis/graphics/graphics.h"
#include "genesis/graphics/render_command.h"
#include "genesis/graphics/render_context.h"
#include "genesis/window/events/event_dispatcher.h"
#include "genesis/window/events/key_events.h"
#include "genesis/window/events/mouse_events.h"

#include <imgui.h>

namespace {

bool onKeyEvent(const GE::KeyEvent& event, bool is_pressed)
{
    auto& io = ImGui::GetIO();
    int key = static_cast<int>(event.getCode());

    io.KeysDown[key] = is_pressed;
    io.KeyShift = (event.getMod() & GE::KeyModFlags::SHIFT_BIT) != 0;
    io.KeyCtrl = (event.getMod() & GE::KeyModFlags::CTRL_BIT) != 0;
    io.KeyAlt = (event.getMod() & GE::KeyModFlags::ALT_BIT) != 0;
#ifdef _WIN32
    io.KeySuper = false;
#else
    io.KeySuper = (event.getMod() & GE::KeyModFlags::SUPER_BIT) != 0;
#endif

    return false;
}

bool onKeyPressedEvent(const GE::KeyPressedEvent& event)
{
    return onKeyEvent(event, true);
}

bool onKeyReleasedEvent(const GE::KeyReleasedEvent& event)
{
    return onKeyEvent(event, false);
}

bool onKeyTypedEvent(const GE::KeyTypedEvent& event)
{
    auto& io = ImGui::GetIO();
    io.AddInputCharactersUTF8(event.getText());
    return false;
}

bool onMouseScrolledEvent(const GE::MouseScrolledEvent& event)
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

} // namespace

namespace GE::GUI {

void BaseLayer::onEvent(Event* event)
{
    EventDispatcher dispatcher{event};
    dispatcher.dispatch<KeyPressedEvent>(onKeyPressedEvent);
    dispatcher.dispatch<KeyReleasedEvent>(onKeyReleasedEvent);
    dispatcher.dispatch<KeyTypedEvent>(onKeyTypedEvent);
    dispatcher.dispatch<MouseScrolledEvent>(onMouseScrolledEvent);
}

} // namespace GE::GUI
