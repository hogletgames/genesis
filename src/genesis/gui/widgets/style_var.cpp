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

ImGuiCol toImGui(StyleColor::Index idx)
{
    using Index = StyleColor::Index;

    static const std::unordered_map<Index, ImGuiStyleVar> TO_IMGUI = {
        {Index::TEXT, ImGuiCol_Text},
        {Index::TEXT_DISABLED, ImGuiCol_TextDisabled},
        {Index::WINDOW_BG, ImGuiCol_WindowBg},
        {Index::CHILD_BG, ImGuiCol_ChildBg},
        {Index::POPUP_BG, ImGuiCol_PopupBg},
        {Index::BORDER, ImGuiCol_Border},
        {Index::BORDER_SHADOW, ImGuiCol_BorderShadow},
        {Index::FRAME_BG, ImGuiCol_FrameBg},
        {Index::FRAME_BG_HOVERED, ImGuiCol_FrameBgHovered},
        {Index::FRAME_BG_ACTIVE, ImGuiCol_FrameBgActive},
        {Index::TITLE_BG, ImGuiCol_TitleBg},
        {Index::TITLE_BG_ACTIVE, ImGuiCol_TitleBgActive},
        {Index::TITLE_BG_COLLAPSED, ImGuiCol_TitleBgCollapsed},
        {Index::MENU_BAR_BG, ImGuiCol_MenuBarBg},
        {Index::SCROLLBAR_BG, ImGuiCol_ScrollbarBg},
        {Index::SCROLLBAR_GRAB, ImGuiCol_ScrollbarGrab},
        {Index::SCROLLBAR_GRAB_HOVERED, ImGuiCol_ScrollbarGrabHovered},
        {Index::SCROLLBAR_GRAB_ACTIVE, ImGuiCol_ScrollbarGrabActive},
        {Index::CHECK_MARK, ImGuiCol_CheckMark},
        {Index::SLIDER_GRAB, ImGuiCol_SliderGrab},
        {Index::SLIDER_GRAB_ACTIVE, ImGuiCol_SliderGrabActive},
        {Index::BUTTON, ImGuiCol_Button},
        {Index::BUTTON_HOVERED, ImGuiCol_ButtonHovered},
        {Index::BUTTON_ACTIVE, ImGuiCol_ButtonActive},
        {Index::HEADER, ImGuiCol_Header},
        {Index::HEADER_HOVERED, ImGuiCol_HeaderHovered},
        {Index::HEADER_ACTIVE, ImGuiCol_HeaderActive},
        {Index::SEPARATOR, ImGuiCol_Separator},
        {Index::SEPARATOR_HOVERED, ImGuiCol_SeparatorHovered},
        {Index::SEPARATOR_ACTIVE, ImGuiCol_SeparatorActive},
        {Index::RESIZE_GRIP, ImGuiCol_ResizeGrip},
        {Index::RESIZE_GRIP_HOVERED, ImGuiCol_ResizeGripHovered},
        {Index::RESIZE_GRIP_ACTIVE, ImGuiCol_ResizeGripActive},
        {Index::TAB, ImGuiCol_Tab},
        {Index::TAB_HOVERED, ImGuiCol_TabHovered},
        {Index::TAB_ACTIVE, ImGuiCol_TabActive},
        {Index::TAB_UNFOCUSED, ImGuiCol_TabUnfocused},
        {Index::TAB_UNFOCUSED_ACTIVE, ImGuiCol_TabUnfocusedActive},
        {Index::DOCKING_PREVIEW, ImGuiCol_DockingPreview},
        {Index::DOCKING_EMPTY_BG, ImGuiCol_DockingEmptyBg},
        {Index::PLOT_LINES, ImGuiCol_PlotLines},
        {Index::PLOT_LINES_HOVERED, ImGuiCol_PlotLinesHovered},
        {Index::PLOT_HISTOGRAM, ImGuiCol_PlotHistogram},
        {Index::PLOT_HISTOGRAM_HOVERED, ImGuiCol_PlotHistogramHovered},
        {Index::TABLE_HEADERBG, ImGuiCol_TableHeaderBg},
        {Index::TABLE_BORDER_STRONG, ImGuiCol_TableBorderStrong},
        {Index::TABLE_BORDER_LIGHT, ImGuiCol_TableBorderLight},
        {Index::TABLE_ROW_BG, ImGuiCol_TableRowBg},
        {Index::TABLE_ROW_BG_ALT, ImGuiCol_TableRowBgAlt},
        {Index::TEXT_SELECTED_BG, ImGuiCol_TextSelectedBg},
        {Index::DRAG_DROP_TARGET, ImGuiCol_DragDropTarget},
        {Index::NAV_HIGHLIGHT, ImGuiCol_NavHighlight},
        {Index::NAV_WINDOWING_HIGHLIGHT, ImGuiCol_NavWindowingHighlight},
        {Index::NAV_WINDOWING_DIM_BG, ImGuiCol_NavWindowingDimBg},
        {Index::MODAL_WINDOW_DIM_BG, ImGuiCol_ModalWindowDimBg},
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

StyleColor::StyleColor(Index idx, uint32_t value)
{
    ImGui::PushStyleColor(toImGui(idx), value);
}

StyleColor::StyleColor(Index idx, const Vec4& value)
{
    ImGui::PushStyleColor(toImGui(idx), toImVec4(value));
}

StyleColor::~StyleColor()
{
    ImGui::PopStyleColor();
}

std::array<Vec4, StyleColor::COUNT> getStyleColors()
{
    std::array<Vec4, StyleColor::COUNT> result{};
    auto*                               colors = ImGui::GetStyle().Colors;
    std::transform(colors, colors + ImGuiCol_COUNT, result.begin(), &toVec4);
    return result;
}

} // namespace GE::GUI
