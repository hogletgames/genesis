/*
 * BSD 3-Clause License
 *
 * Copyright (c) 2023, Dmitry Shilnenkov
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

#include <genesis/core/bit.h>
#include <genesis/math/types.h>

#include <string>

namespace GE::GUI {

class InputText
{
public:
    enum Flags
    {
        NONE = 0,
        CHARS_DECIMAL = bit(0),
        CHARS_HEXADECIMAL = bit(1),
        CHARS_UPPERCASE = bit(2),
        CHARS_NO_BLANK = bit(3),
        AUTO_SELECT_ALL = bit(4),
        ENTER_RETURNS_TRUE = bit(5),
        CALLBACK_COMPLETION = bit(6),
        CALLBACK_HISTORY = bit(7),
        CALLBACK_ALWAYS = bit(8),
        CALLBACK_CHAR_FILTER = bit(9),
        ALLOW_TAB_INPUT = bit(10),
        CTRL_ENTER_FOR_NEW_LINE = bit(11),
        NO_HORIZONTAL_SCROLL = bit(12),
        ALWAYS_OVERWRITE = bit(13),
        READ_ONLY = bit(14),
        PASSWORD = bit(15),
        NO_UNDOREDO = bit(16),
        CHARS_SCIENTIFIC = bit(17),
        CALLBACK_RESIZE = bit(18),
        CALLBACK_EDIT = bit(19),
        ESCAPE_CLEARS_ALL = bit(20),
    };

    static bool call(std::string_view label, std::string* output, Flags flags = NONE);
};

class InputTextMultiline
{
public:
    static void call(std::string* text, const Vec2& widget_size);
};

} // namespace GE::GUI
