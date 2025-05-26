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

#include <genesis/gui/event_handler.h>

namespace GE::Vulkan::SDL {

class GE_API EventHandler: public GE::GUI::EventHandler
{
public:
    bool onKeyPressedEvent(const KeyPressedEvent& event) override;
    bool onKeyReleasedEvent(const KeyReleasedEvent& event) override;
    bool onKeyTypedEvent(const KeyTypedEvent& event) override;

    bool onMouseButtonPressedEvent(const MouseButtonPressedEvent& event) override;
    bool onMouseButtonReleasedEvent(const MouseButtonReleasedEvent& event) override;
    bool onMouseMovedEvent(const MouseMovedEvent& event) override;
    bool onMouseScrolledEvent(const MouseScrolledEvent& event) override;

    bool onWindowMovedEvent(const WindowMovedEvent& event) override;
    bool onWindowResizedEvent(const WindowResizedEvent& event) override;
    bool onWindowEnteredEvent(const WindowEnteredEvent& event) override;
    bool onWindowLeftEvent(const WindowLeftEvent& event) override;
    bool onWindowFocusGainedEvent(const WindowFocusGainedEvent& event) override;
    bool onWindowFocusLostEvent(const WindowFocusLostEvent& event) override;
    bool onWindowClosedEvent(const WindowClosedEvent& event) override;
};

} // namespace GE::Vulkan::SDL
