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

class GE_API StyleVar: public NonCopyable
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

class GE_API StyleColor: public NonCopyable
{
public:
    enum Index
    {
        TEXT,
        TEXT_DISABLED,
        WINDOW_BG,
        CHILD_BG,
        POPUP_BG,
        BORDER,
        BORDER_SHADOW,
        FRAME_BG,
        FRAME_BG_HOVERED,
        FRAME_BG_ACTIVE,
        TITLE_BG,
        TITLE_BG_ACTIVE,
        TITLE_BG_COLLAPSED,
        MENU_BAR_BG,
        SCROLLBAR_BG,
        SCROLLBAR_GRAB,
        SCROLLBAR_GRAB_HOVERED,
        SCROLLBAR_GRAB_ACTIVE,
        CHECK_MARK,
        SLIDER_GRAB,
        SLIDER_GRAB_ACTIVE,
        BUTTON,
        BUTTON_HOVERED,
        BUTTON_ACTIVE,
        HEADER,
        HEADER_HOVERED,
        HEADER_ACTIVE,
        SEPARATOR,
        SEPARATOR_HOVERED,
        SEPARATOR_ACTIVE,
        RESIZE_GRIP,
        RESIZE_GRIP_HOVERED,
        RESIZE_GRIP_ACTIVE,
        TAB,
        TAB_HOVERED,
        TAB_ACTIVE,
        TAB_UNFOCUSED,
        TAB_UNFOCUSED_ACTIVE,
        DOCKING_PREVIEW,
        DOCKING_EMPTY_BG,
        PLOT_LINES,
        PLOT_LINES_HOVERED,
        PLOT_HISTOGRAM,
        PLOT_HISTOGRAM_HOVERED,
        TABLE_HEADERBG,
        TABLE_BORDER_STRONG,
        TABLE_BORDER_LIGHT,
        TABLE_ROW_BG,
        TABLE_ROW_BG_ALT,
        TEXT_SELECTED_BG,
        DRAG_DROP_TARGET,
        NAV_HIGHLIGHT,
        NAV_WINDOWING_HIGHLIGHT,
        NAV_WINDOWING_DIM_BG,
        MODAL_WINDOW_DIM_BG,
        COUNT
    };

    StyleColor(Index idx, uint32_t value);
    StyleColor(Index idx, const Vec4& value);
    ~StyleColor();
};

std::array<Vec4, StyleColor::COUNT> getStyleColors();

} // namespace GE::GUI
