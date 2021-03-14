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

#ifndef GENESIS_WINDOW_KEY_CODES_H_
#define GENESIS_WINDOW_KEY_CODES_H_

#include <genesis/core/enum.h>
#include <genesis/core/utils.h>

namespace GE {

enum class KeyCode : uint16_t
{
    UNKNOWN = 0x00,

    BACKSPACE = 0x08,
    TAB = 0x09,
    RETURN = 0x0D,
    ESCAPE = 0x1B,

    SPACE = ' ',
    EXCLAIM = '!',
    QUOTE_DBL = '"',
    HASH = '#',
    PERCENT = '%',
    DOLLAR = '$',
    AMPERSAND = '&',
    QUOTE = '\'',
    LEFT_PAREN = '(',
    RIGHT_PAREN = ')',
    ASTERISK = '*',
    PLUS = '+',
    COMMA = ',',
    MINUS = '-',
    PERIOD = '.',
    SLASH = '/',

    D0 = '0',
    D1 = '1',
    D2 = '2',
    D3 = '3',
    D4 = '4',
    D5 = '5',
    D6 = '6',
    D7 = '7',
    D8 = '8',
    D9 = '9',

    COLON = ':',
    SEMICOLON = ';',
    LESS = '<',
    EQUALS = '=',
    GREATER = '>',
    QUESTION = '?',
    AT = '@',

    A = 'A',
    B = 'B',
    C = 'C',
    D = 'D',
    E = 'E',
    F = 'F',
    G = 'G',
    H = 'H',
    I = 'I',
    J = 'J',
    K = 'K',
    L = 'L',
    M = 'M',
    N = 'N',
    O = 'O',
    P = 'P',
    Q = 'Q',
    R = 'R',
    S = 'S',
    T = 'T',
    U = 'U',
    V = 'V',
    W = 'W',
    X = 'X',
    Y = 'Y',
    Z = 'Z',

    LEFT_BRACKET = '[',
    BACKSLASH = '\\',
    RIGHT_BRACKET = ']',
    CARET = '^',
    UNDERSCORE = '_',
    BACK_QUOTE = '`',

    ENTER = 0x0101,
    INSERT = 0x0104,
    DELETE = 0x0105,
    RIGHT = 0x0106,
    LEFT = 0x0107,
    DOWN = 0x0108,
    UP = 0x0109,
    PAGE_UP = 0x010A,
    PAGE_DOWN = 0x010B,
    HOME = 0x010C,
    END = 0x010D,
    CAPS_LOCK = 0x010E,
    SCROLL_LOCK = 0x010F,
    NUM_LOCK = 0x0110,
    PRINT_SCREEN = 0x0111,
    PAUSE = 0x0112,

    F1 = 0x0113,
    F2 = 0x0114,
    F3 = 0x0115,
    F4 = 0x0116,
    F5 = 0x0117,
    F6 = 0x0118,
    F7 = 0x0119,
    F8 = 0x011A,
    F9 = 0x011B,
    F10 = 0x011C,
    F11 = 0x011D,
    F12 = 0x011E,
    F13 = 0x011F,
    F14 = 0x0120,
    F15 = 0x0121,
    F16 = 0x0122,
    F17 = 0x0123,
    F18 = 0x0124,
    F19 = 0x0125,
    F20 = 0x0126,
    F21 = 0x0127,
    F22 = 0x0128,
    F23 = 0x0129,
    F24 = 0x012A,
    F25 = 0x012B,

    KP0 = 0x012C,
    KP1 = 0x012D,
    KP2 = 0x012E,
    KP3 = 0x012f,
    KP4 = 0x0130,
    KP5 = 0x0131,
    KP6 = 0x0132,
    KP7 = 0x0133,
    KP8 = 0x0134,
    KP9 = 0x0135,
    KP_PERIOD = 0x136,
    KP_DECIMAL = 0x0137,
    KP_DIVIDE = 0x0138,
    KP_MULTIPLY = 0x0139,
    KP_MINUS = 0x013A,
    KP_PLUS = 0x013B,
    KP_ENTER = 0x013C,
    KP_EQUAL = 0x013D,

    LSHIFT = 0x013E,
    LCTRL = 0x013F,
    LALT = 0x0140,
    LSUPER = 0x0141,
    RSHIFT = 0x0142,
    RCTRL = 0x0143,
    RALT = 0x0144,
    RSUPER = 0x0145,
    MENU = 0x0146
};

enum KeyModFlags : uint8_t
{
    NONE = 0,
    SHIFT_BIT = GE_BIT(1),
    CTRL_BIT = GE_BIT(2),
    ALT_BIT = GE_BIT(3),
    SUPER_BIT = GE_BIT(4),
    NUM_LOCK_BIT = GE_BIT(5),
    CAPS_LOCK_BIT = GE_BIT(6)
};

} // namespace GE

GE_EXTEND_ENUM_RANGE(::GE::KeyCode, ::GE::KeyCode::UNKNOWN, ::GE::KeyCode::MENU)

#endif // GENESIS_WINDOW_KEY_CODES_H_
