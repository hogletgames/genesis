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

#include "style_var.h"
#include "private/types.h"

#include "genesis/core/utils.h"

#include <imgui.h>

namespace GE::GUI {
namespace {

ImGuiStyleVar toImGui(StyleVar::Index idx)
{
    using Index = StyleVar::Index;

    static const std::unordered_map<Index, ImGuiStyleVar> TO_IMGUI = {
        {Index::ALPHA, ImGuiStyleVar_Alpha},
        {Index::DISABLTYLED_ALPHA, ImGuiStyleVar_DisabledAlpha},
        {Index::WINDOW_PADDING, ImGuiStyleVar_WindowPadding},
        {Index::WINDOW_ROUNDING, ImGuiStyleVar_WindowRounding},
        {Index::WINDOW_BORDER_SIZE, ImGuiStyleVar_WindowBorderSize},
        {Index::WINDOW_MIN_SIZE, ImGuiStyleVar_WindowMinSize},
        {Index::WINDOW_TITLE_ALIGN, ImGuiStyleVar_WindowTitleAlign},
        {Index::CHILD_ROUNDING, ImGuiStyleVar_ChildRounding},
        {Index::CHILD_BORDER_SIZE, ImGuiStyleVar_ChildBorderSize},
        {Index::POPUP_ROUNDING, ImGuiStyleVar_PopupRounding},
        {Index::POPUP_BORDER_SIZE, ImGuiStyleVar_PopupBorderSize},
        {Index::FRAME_PADDING, ImGuiStyleVar_FramePadding},
        {Index::FRAME_ROUNDING, ImGuiStyleVar_FrameRounding},
        {Index::FRAME_BORDER_SIZE, ImGuiStyleVar_FrameBorderSize},
        {Index::ITEM_SPACING, ImGuiStyleVar_ItemSpacing},
        {Index::ITEM_INNER_SPACING, ImGuiStyleVar_ItemInnerSpacing},
        {Index::INDENT_SPACING, ImGuiStyleVar_IndentSpacing},
        {Index::CELL_PADDING, ImGuiStyleVar_CellPadding},
        {Index::SCROLLBAR_SIZE, ImGuiStyleVar_ScrollbarSize},
        {Index::SCROLLBAR_ROUNDING, ImGuiStyleVar_ScrollbarRounding},
        {Index::GRAB_MIN_SIZE, ImGuiStyleVar_GrabMinSize},
        {Index::GRAB_ROUNDING, ImGuiStyleVar_GrabRounding},
        {Index::TAB_ROUNDING, ImGuiStyleVar_TabRounding},
        {Index::BUTTON_TEXT_ALIGN, ImGuiStyleVar_ButtonTextAlign},
        {Index::SELECTABLE_TEXT_ALIGN, ImGuiStyleVar_SelectableTextAlign},
    };

    return getValue(TO_IMGUI, idx);
}

} // namespace

StyleVar::StyleVar(Index idx, float value)
{
    ImGui::PushStyleVar(toImGui(idx), value);
}

StyleVar::StyleVar(Index idx, const Vec2& value)
{
    ImGui::PushStyleVar(toImGui(idx), toImVec2(value));
}

StyleVar::~StyleVar()
{
    ImGui::PopStyleVar();
}

} // namespace GE::GUI
