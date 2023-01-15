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

#include <genesis/window/events/event.h>
#include <genesis/window/key_codes.h>

namespace GE {

class GE_API KeyEvent: public Event
{
public:
    KeyCode getCode() const { return m_code; }
    KeyModFlags getMod() const { return m_mod; }

protected:
    KeyEvent() = default;
    KeyEvent(KeyCode code, KeyModFlags mod);

    KeyCode m_code{KeyCode::UNKNOWN};
    KeyModFlags m_mod{KeyModFlags::NONE};
};

class GE_API KeyPressedEvent: public KeyEvent
{
public:
    KeyPressedEvent() = default;
    KeyPressedEvent(KeyCode code, KeyModFlags mod, uint32_t repeat_count);

    std::string asString() const override;
    uint32_t getRepeatCount() const { return m_repeat_count; }

    GE_DECLARE_EVENT_DESCRIPTOR(KeyPressedEvent)

private:
    uint32_t m_repeat_count{};
};

class GE_API KeyReleasedEvent: public KeyEvent
{
public:
    KeyReleasedEvent() = default;
    KeyReleasedEvent(KeyCode code, KeyModFlags mod);

    std::string asString() const override;

    GE_DECLARE_EVENT_DESCRIPTOR(KeyReleasedEvent)
};

class GE_API KeyTypedEvent: public Event
{
public:
    KeyTypedEvent() = default;
    explicit KeyTypedEvent(const char* text);

    std::string asString() const override;
    const char* getText() const { return m_text; }

    GE_DECLARE_EVENT_DESCRIPTOR(KeyTypedEvent)

private:
    const char* m_text{nullptr};
};

} // namespace GE
