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

#include <genesis/core/bit.h>
#include <genesis/gui/widgets/widget.h>
#include <genesis/math/types.h>

#include <string_view>

namespace GE::GUI {

class GE_API Window: public Widget
{
public:
    using Flags = int;

    enum Flag : uint8_t
    {
        NONE = 0,
        NO_TITLE_BAR = bit<uint8_t>(0),
        NO_RESIZE = bit<uint8_t>(1),
        NO_MOVE = bit<uint8_t>(2),
        NO_SCROLLBAR = bit<uint8_t>(3),
        NO_SCROLL_WITH_MOUSE = bit<uint8_t>(4),
        NO_COLLAPSE = bit<uint8_t>(5),
        ALWAYS_AUTO_RESIZE = bit<uint8_t>(6),
        NO_BACKGROUND = bit<uint8_t>(7),
        NO_SAVED_SETTINGS = bit<uint8_t>(8),
        NO_MOUSE_INPUTS = bit<uint8_t>(9),
        MENU_BAR = bit<uint8_t>(10),
        HORIZONTAL_SCROLLBAR = bit<uint8_t>(11),
        NO_FOCUS_ON_APPEARING = bit<uint8_t>(12),
        NO_BRING_TO_FRONT_ON_FOCUS = bit<uint8_t>(13),
        ALWAYS_VERTICAL_SCROLLBAR = bit<uint8_t>(14),
        ALWAYS_HORIZONTAL_SCROLLBAR = bit<uint8_t>(15),
        NO_NAV_INPUTS = bit<uint8_t>(16),
        NO_NAV_FOCUS = bit<uint8_t>(17),
        UNSAVED_DOCUMENT = bit<uint8_t>(18),
        NO_DOCKING = bit<uint8_t>(19),
        NO_NAV = NO_NAV_INPUTS | NO_NAV_FOCUS,
        NO_DECORATION = NO_TITLE_BAR | NO_RESIZE | NO_SCROLLBAR | NO_COLLAPSE,
        NO_INPUTS = NO_MOUSE_INPUTS | NO_NAV_INPUTS | NO_NAV_FOCUS,
    };

    explicit Window(std::string_view title, bool* is_open = nullptr, Flags flags = 0);

    Vec2 position() const;
    Vec2 mousePosition() const;
    Vec2 size() const;
    Vec2 availableRegion() const;
    Vec2 contentRegionMin() const;
    Vec2 contentRegionMax() const;
    float aspectRatio() const;

    bool isFocused() const;
    bool isHovered() const;
};

} // namespace GE::GUI
