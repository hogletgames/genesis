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

#include "mouse_events.h"

#include "genesis/core/format.h"

namespace GE {

MouseMovedEvent::MouseMovedEvent(const Vec2& position, const Vec2& offset, uint32_t window_id)
    : m_position{position}
    , m_offset{offset}
    , m_window_id{window_id}
{}

std::string MouseMovedEvent::asString() const
{
    return GE_FMTSTR("MouseMovedEvent: position {}, offset {}, window {}", toString(m_position),
                     toString(m_offset), m_window_id);
}

MouseScrolledEvent::MouseScrolledEvent(const Vec2& offset)
    : m_offset{offset}
{}

std::string MouseScrolledEvent::asString() const
{
    return GE_FMTSTR("MouseScrolledEvent: {}", toString(m_offset));
}

MouseButtonEvent::MouseButtonEvent(MouseButton button)
    : m_button{button}
{}

MouseButtonPressedEvent::MouseButtonPressedEvent(MouseButton button)
    : MouseButtonEvent{button}
{}

std::string MouseButtonPressedEvent::asString() const
{
    return GE_FMTSTR("MouseButtonPressedEvent: {}", toString(m_button));
}

MouseButtonReleasedEvent::MouseButtonReleasedEvent(MouseButton button)
    : MouseButtonEvent{button}
{}

std::string MouseButtonReleasedEvent::asString() const
{
    return GE_FMTSTR("MouseButtonReleasedEvent: {}", toString(m_button));
}

} // namespace GE
