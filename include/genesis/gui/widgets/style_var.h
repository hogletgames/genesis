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

#include <genesis/core/interface.h>
#include <genesis/math/types.h>

namespace GE::GUI {

class StyleVar: public NonCopyable
{
public:
    enum Index
    {
        ALPHA,
        DISABLTYLED_ALPHA,
        WINDOW_PADDING,
        WINDOW_ROUNDING,
        WINDOW_BORDER_SIZE,
        WINDOW_MIN_SIZE,
        WINDOW_TITLE_ALIGN,
        CHILD_ROUNDING,
        CHILD_BORDER_SIZE,
        POPUP_ROUNDING,
        POPUP_BORDER_SIZE,
        FRAME_PADDING,
        FRAME_ROUNDING,
        FRAME_BORDER_SIZE,
        ITEM_SPACING,
        ITEM_INNER_SPACING,
        INDENT_SPACING,
        CELL_PADDING,
        SCROLLBAR_SIZE,
        SCROLLBAR_ROUNDING,
        GRAB_MIN_SIZE,
        GRAB_ROUNDING,
        TAB_ROUNDING,
        BUTTON_TEXT_ALIGN,
        SELECTABLE_TEXT_ALIGN,
    };

    StyleVar(Index idx, float value);
    StyleVar(Index idx, const Vec2& value);
    ~StyleVar();
};

} // namespace GE::GUI
