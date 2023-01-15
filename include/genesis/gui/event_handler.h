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

#pragma once

#include <genesis/core/interface.h>
#include <genesis/window/events/key_events.h>
#include <genesis/window/events/mouse_events.h>
#include <genesis/window/events/window_events.h>

namespace GE::GUI {

class GE_API EventHandler: public Interface
{
public:
    virtual bool onKeyPressedEvent(const KeyPressedEvent& event) = 0;
    virtual bool onKeyReleasedEvent(const KeyReleasedEvent& event) = 0;
    virtual bool onKeyTypedEvent(const KeyTypedEvent& event) = 0;

    virtual bool onMouseButtonPressedEvent(const MouseButtonPressedEvent& event) = 0;
    virtual bool onMouseButtonReleasedEvent(const MouseButtonReleasedEvent& event) = 0;
    virtual bool onMouseMovedEvent(const MouseMovedEvent& event) = 0;
    virtual bool onMouseScrolledEvent(const MouseScrolledEvent& event) = 0;

    virtual bool onWindowMovedEvent(const WindowMovedEvent& event) = 0;
    virtual bool onWindowResizedEvent(const WindowResizedEvent& event) = 0;
    virtual bool onWindowEnteredEvent(const WindowEnteredEvent& event) = 0;
    virtual bool onWindowLeftEvent(const WindowLeftEvent& event) = 0;
    virtual bool onWindowFocusGainedEvent(const WindowFocusGainedEvent& event) = 0;
    virtual bool onWindowFocusLostEvent(const WindowFocusLostEvent& event) = 0;
    virtual bool onWindowClosedEvent(const WindowClosedEvent& event) = 0;
};

} // namespace GE::GUI
