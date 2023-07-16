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

#include "key_events.h"

#include "genesis/core/format.h"
#include "genesis/core/format_user_type.h"

namespace GE {

KeyEvent::KeyEvent(KeyCode code, KeyModFlags mod)
    : m_code{code}
    , m_mod{mod}
{}

KeyPressedEvent::KeyPressedEvent(KeyCode code, KeyModFlags mod, uint32_t repeat_count)
    : KeyEvent{code, mod}
    , m_repeat_count{repeat_count}
{}

std::string KeyPressedEvent::asString() const
{
    return GE_FMTSTR("KeyPressedEvent: Key: '{}', Mod: '{:#010b}' ({})", m_code,
                     static_cast<uint8_t>(m_mod), m_repeat_count);
}

KeyReleasedEvent::KeyReleasedEvent(KeyCode code, KeyModFlags mod)
    : KeyEvent{code, mod}
{}

std::string KeyReleasedEvent::asString() const
{
    return GE_FMTSTR("KeyReleasedEvent: Key: '{}', Mod: '{:#010b}'", m_code,
                     static_cast<uint8_t>(m_mod));
}

KeyTypedEvent::KeyTypedEvent(const char* text)
    : m_text{text}
{}

std::string KeyTypedEvent::asString() const
{
    return GE_FMTSTR("KeyTypedEvent: '{}'", m_text);
}

} // namespace GE
