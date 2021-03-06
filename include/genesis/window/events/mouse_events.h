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

#ifndef GENESIS_WINDOW_EVENTS_MOUSE_EVENTS_H_
#define GENESIS_WINDOW_EVENTS_MOUSE_EVENTS_H_

#include <genesis/math/types.h>
#include <genesis/window/events/event.h>
#include <genesis/window/mouse_button_codes.h>

namespace GE {

class GE_API MouseMovedEvent: public Event
{
public:
    MouseMovedEvent() = default;
    explicit MouseMovedEvent(const Vec2& position);

    std::string asString() const override;
    const Vec2& getPosition() const { return m_position; }

    GE_DECLARE_EVENT_DESCRIPTOR(MouseMovedEvent);

private:
    Vec2 m_position{};
};

class GE_API MouseScrolledEvent: public Event
{
public:
    MouseScrolledEvent() = default;
    explicit MouseScrolledEvent(const Vec2& offset);

    std::string asString() const override;
    const Vec2& getOffset() const { return m_offset; }

    GE_DECLARE_EVENT_DESCRIPTOR(MouseScrolledEvent)

private:
    Vec2 m_offset{};
};

class GE_API MouseButtonEvent: public Event
{
public:
    MouseButton getMouseButton() const { return m_button; }

protected:
    MouseButtonEvent() = default;
    explicit MouseButtonEvent(MouseButton button);

    MouseButton m_button{MouseButton::UNKNOWN};
};

class GE_API MouseButtonPressedEvent: public MouseButtonEvent
{
public:
    MouseButtonPressedEvent() = default;
    explicit MouseButtonPressedEvent(MouseButton button);

    std::string asString() const override;

    GE_DECLARE_EVENT_DESCRIPTOR(MouseButtonPressedEvent)
};

class GE_API MouseButtonReleasedEvent: public MouseButtonEvent
{
public:
    MouseButtonReleasedEvent() = default;
    explicit MouseButtonReleasedEvent(MouseButton button);

    std::string asString() const override;

    GE_DECLARE_EVENT_DESCRIPTOR(MouseButtonReleasedEvent)
};

} // namespace GE

#endif // GENESIS_WINDOW_EVENTS_MOUSE_EVENTS_H_
