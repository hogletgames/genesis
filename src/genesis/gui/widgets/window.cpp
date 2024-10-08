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

#include "window.h"
#include "private/flag_utils.h"
#include "private/types.h"

#include <vector>

#include <imgui.h>

namespace GE::GUI {
namespace {

ImGuiWindowFlags toImGuiWindowFlags(Window::Flags flags)
{
    static const std::vector<std::pair<Window::Flag, ImGuiWindowFlags_>> TO_IMGUI_FLAGS = {
        {Window::NONE, ImGuiWindowFlags_None},
        {Window::NO_TITLE_BAR, ImGuiWindowFlags_NoTitleBar},
        {Window::NO_RESIZE, ImGuiWindowFlags_NoResize},
        {Window::NO_MOVE, ImGuiWindowFlags_NoMove},
        {Window::NO_SCROLLBAR, ImGuiWindowFlags_NoScrollbar},
        {Window::NO_SCROLL_WITH_MOUSE, ImGuiWindowFlags_NoScrollWithMouse},
        {Window::NO_COLLAPSE, ImGuiWindowFlags_NoCollapse},
        {Window::ALWAYS_AUTO_RESIZE, ImGuiWindowFlags_AlwaysAutoResize},
        {Window::NO_BACKGROUND, ImGuiWindowFlags_NoBackground},
        {Window::NO_SAVED_SETTINGS, ImGuiWindowFlags_NoSavedSettings},
        {Window::NO_MOUSE_INPUTS, ImGuiWindowFlags_NoMouseInputs},
        {Window::MENU_BAR, ImGuiWindowFlags_MenuBar},
        {Window::HORIZONTAL_SCROLLBAR, ImGuiWindowFlags_HorizontalScrollbar},
        {Window::NO_FOCUS_ON_APPEARING, ImGuiWindowFlags_NoFocusOnAppearing},
        {Window::NO_BRING_TO_FRONT_ON_FOCUS, ImGuiWindowFlags_NoBringToFrontOnFocus},
        {Window::ALWAYS_VERTICAL_SCROLLBAR, ImGuiWindowFlags_AlwaysVerticalScrollbar},
        {Window::ALWAYS_HORIZONTAL_SCROLLBAR, ImGuiWindowFlags_AlwaysHorizontalScrollbar},
        {Window::NO_NAV_INPUTS, ImGuiWindowFlags_NoNavInputs},
        {Window::NO_NAV_FOCUS, ImGuiWindowFlags_NoNavFocus},
        {Window::UNSAVED_DOCUMENT, ImGuiWindowFlags_UnsavedDocument},
        {Window::NO_DOCKING, ImGuiWindowFlags_NoDocking},
    };

    return toImGuiFlags<ImGuiWindowFlags>(TO_IMGUI_FLAGS, flags);
}

} // namespace

Window::Window(std::string_view title, bool* is_open, Flags flags)
{
    setBeginFunc([title, is_open, flags = toImGuiWindowFlags(flags)] {
        if (is_open != nullptr && !*is_open) {
            return false;
        }

        return ImGui::Begin(title.data(), is_open, flags);
    });

    setEndFunc(&ImGui::End);
    setForceEnd();
}

Vec2 Window::position() const
{
    return toVec2(ImGui::GetWindowPos());
}

Vec2 Window::mousePosition() const
{
    auto mouse_position = toVec2(ImGui::GetMousePos());
    auto window_position = toVec2(ImGui::GetWindowPos());
    auto content_region_min = toVec2(ImGui::GetWindowContentRegionMin());
    auto content_region_screen = window_position + content_region_min;
    auto mouse_pos_in_content_region = mouse_position - content_region_screen;

    return mouse_pos_in_content_region;
}

Vec2 Window::size() const
{
    return toVec2(ImGui::GetWindowSize());
}

Vec2 Window::availableRegion() const
{
    return toVec2(ImGui::GetContentRegionAvail());
}

Vec2 Window::contentRegionMin() const
{
    return toVec2(ImGui::GetWindowContentRegionMin());
}

Vec2 Window::contentRegionMax() const
{
    return toVec2(ImGui::GetWindowContentRegionMax());
}

float Window::aspectRatio() const
{
    if (auto window_size = size(); window_size.y > 0.0f) {
        return window_size.x / window_size.y;
    }

    return 0.0f;
}

bool Window::isFocused() const
{
    return ImGui::IsWindowFocused();
}

bool Window::isHovered() const
{
    return ImGui::IsWindowHovered();
}

} // namespace GE::GUI
