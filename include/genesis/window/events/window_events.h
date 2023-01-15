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

#pragma once

#include <genesis/math/types.h>
#include <genesis/window/events/event.h>

namespace GE {

class GE_API WindowEvent: public Event
{
public:
    explicit WindowEvent(uint32_t id = 0);

    uint32_t id() const { return m_id; }

protected:
    uint32_t m_id{};
};

class GE_API WindowMovedEvent: public WindowEvent
{
public:
    WindowMovedEvent() = default;
    WindowMovedEvent(uint32_t id, Vec2 position);

    std::string asString() const override;
    const Vec2& position() const { return m_position; }

    GE_DECLARE_EVENT_DESCRIPTOR(WindowMovedEvent);

private:
    Vec2 m_position{};
};

class GE_API WindowResizedEvent: public WindowEvent
{
public:
    WindowResizedEvent() = default;
    WindowResizedEvent(uint32_t id, const Vec2& size);

    std::string asString() const override;
    const Vec2& size() const { return m_size; }

    GE_DECLARE_EVENT_DESCRIPTOR(WindowResizedEvent)

private:
    Vec2 m_size{};
};

class GE_API WindowMinimizedEvent: public WindowEvent
{
public:
    using WindowEvent::WindowEvent;

    std::string asString() const override;

    GE_DECLARE_EVENT_DESCRIPTOR(WindowMinimizedEvent)
};

class GE_API WindowMaximizedEvent: public WindowEvent
{
public:
    using WindowEvent::WindowEvent;

    std::string asString() const override;

    GE_DECLARE_EVENT_DESCRIPTOR(WindowMaximizedEvent)
};

class GE_API WindowRestoredEvent: public WindowEvent
{
public:
    using WindowEvent::WindowEvent;

    std::string asString() const override;

    GE_DECLARE_EVENT_DESCRIPTOR(WindowRestoredEvent)
};

class GE_API WindowEnteredEvent: public WindowEvent
{
public:
    using WindowEvent::WindowEvent;

    std::string asString() const override;

    GE_DECLARE_EVENT_DESCRIPTOR(WindowEnteredEvent);
};

class GE_API WindowLeftEvent: public WindowEvent
{
public:
    using WindowEvent::WindowEvent;

    std::string asString() const override;

    GE_DECLARE_EVENT_DESCRIPTOR(WindowLeftEvent);
};

class GE_API WindowFocusGainedEvent: public WindowEvent
{
public:
    using WindowEvent::WindowEvent;

    std::string asString() const override;

    GE_DECLARE_EVENT_DESCRIPTOR(WindowFocusGainedEvent);
};

class GE_API WindowFocusLostEvent: public WindowEvent
{
public:
    using WindowEvent::WindowEvent;

    std::string asString() const override;

    GE_DECLARE_EVENT_DESCRIPTOR(WindowFocusLostEvent);
};

class GE_API WindowClosedEvent: public WindowEvent
{
public:
    using WindowEvent::WindowEvent;

    std::string asString() const override;

    GE_DECLARE_EVENT_DESCRIPTOR(WindowClosedEvent)
};

} // namespace GE
