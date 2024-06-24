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
#include <genesis/gui/widgets/widget.h>

#include <string_view>

namespace GE::GUI {

struct PopupFlag {
    enum Flags
    {
        NONE = 0,
        MOUSE_BUTTON_LEFT = 0,
        MOUSE_BUTTON_RIGHT = 1,
        MOUSE_BUTTON_MIDDLE = 2,
        NO_REOPEN = bit(5),
        NO_OPEN_OVER_EXISTING_POPUP = bit(7),
        NO_OPEN_OVER_ITEMS = bit(8),
        ANY_POPUP_ID = bit(10),
        ANY_POPUP_LEVEL = bit(11),
        ANY_POPUP = ANY_POPUP_ID | ANY_POPUP_LEVEL,
    };
};

using PopupFlags = int;

class GE_API PopupContextWindow: public Widget
{
public:
    explicit PopupContextWindow(std::string_view str_id = {},
                                PopupFlags flags = PopupFlag::MOUSE_BUTTON_RIGHT);
};

class GE_API PopupContextItem: public Widget
{
public:
    explicit PopupContextItem(std::string_view str_id = {},
                              PopupFlags flags = PopupFlag::MOUSE_BUTTON_RIGHT);
};

class GE_API Popup: public Widget
{
public:
    explicit Popup(std::string_view str_id, PopupFlags flags = PopupFlag::NONE);
};

class OpenPopup
{
public:
    OpenPopup() = delete;
    ~OpenPopup() = delete;

    static void call(std::string_view str_id);
};

} // namespace GE::GUI
