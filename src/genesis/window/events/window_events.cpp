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

#include "window_events.h"

#include "genesis/core/format.h"
#include "genesis/core/format_user_type.h"

namespace GE {

WindowEvent::WindowEvent(uint32_t id)
    : m_id{id}
{}

WindowMovedEvent::WindowMovedEvent(uint32_t id, Vec2 position)
    : WindowEvent{id}
    , m_position{position}
{}

std::string WindowMovedEvent::asString() const
{
    return GE_FMTSTR("WindowMovedEvent: {}", toString(m_position));
}

WindowResizedEvent::WindowResizedEvent(uint32_t id, const Vec2& size)
    : WindowEvent{id}
    , m_size{size}
{}

std::string WindowResizedEvent::asString() const
{
    return GE_FMTSTR("WindowResizedEvent: {}", toString(m_size));
}

std::string WindowMinimizedEvent::asString() const
{
    return "WindowMinimizedEvent";
}

std::string WindowMaximizedEvent::asString() const
{
    return "WindowMaximizedEvent";
}

std::string WindowRestoredEvent::asString() const
{
    return "WindowRestoredEvent";
}

std::string WindowEnteredEvent::asString() const
{
    return "WindowEnteredEvent";
}

std::string WindowLeftEvent::asString() const
{
    return "WindowLeftEvent";
}

std::string WindowFocusGainedEvent::asString() const
{
    return "WindowFocusGainedEvent";
}
std::string WindowFocusLostEvent::asString() const
{
    return "WindowFocusLostEvent";
}

std::string WindowClosedEvent::asString() const
{
    return "WindowClosedEvent";
}

} // namespace GE
