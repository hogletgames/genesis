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
#include "context.h"
#include "event_handler.h"

#include "genesis/core/utils.h"
#include "genesis/graphics/graphics.h"
#include "genesis/window/events/event_dispatcher.h"

namespace GE::GUI {

void BaseLayer::onEvent(Event* event)
{
    auto*           handler = Graphics::gui()->eventHandler();
    EventDispatcher dispatcher{event};

    dispatcher.dispatch<KeyPressedEvent>(toEventHandler(&EventHandler::onKeyPressedEvent, handler));
    dispatcher.dispatch<KeyReleasedEvent>(
        toEventHandler(&EventHandler::onKeyReleasedEvent, handler));
    dispatcher.dispatch<KeyTypedEvent>(toEventHandler(&EventHandler::onKeyTypedEvent, handler));

    dispatcher.dispatch<MouseButtonPressedEvent>(
        toEventHandler(&EventHandler::onMouseButtonPressedEvent, handler));
    dispatcher.dispatch<MouseButtonReleasedEvent>(
        toEventHandler(&EventHandler::onMouseButtonReleasedEvent, handler));
    dispatcher.dispatch<MouseMovedEvent>(toEventHandler(&EventHandler::onMouseMovedEvent, handler));
    dispatcher.dispatch<MouseScrolledEvent>(
        toEventHandler(&EventHandler::onMouseScrolledEvent, handler));

    dispatcher.dispatch<WindowMovedEvent>(
        toEventHandler(&EventHandler::onWindowMovedEvent, handler));
    dispatcher.dispatch<WindowResizedEvent>(
        toEventHandler(&EventHandler::onWindowResizedEvent, handler));
    dispatcher.dispatch<WindowEnteredEvent>(
        toEventHandler(&EventHandler::onWindowEnteredEvent, handler));
    dispatcher.dispatch<WindowLeftEvent>(toEventHandler(&EventHandler::onWindowLeftEvent, handler));
    dispatcher.dispatch<WindowFocusGainedEvent>(
        toEventHandler(&EventHandler::onWindowFocusGainedEvent, handler));
    dispatcher.dispatch<WindowFocusLostEvent>(
        toEventHandler(&EventHandler::onWindowFocusLostEvent, handler));
    dispatcher.dispatch<WindowClosedEvent>(
        toEventHandler(&EventHandler::onWindowClosedEvent, handler));
}

} // namespace GE::GUI
