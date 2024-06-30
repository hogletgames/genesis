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

#include "widgets/popup.h"

#include "genesis/core/bit.h"

#include <imgui.h>

namespace GE::GUI {
namespace {

ImGuiPopupFlags toImGuiPopupFlags(PopupFlags flags)
{
    ImGuiPopupFlags imgui_flags{flags & ImGuiPopupFlags_MouseButtonMask_};

    if (checkBits<PopupFlags>(flags, PopupFlag::NO_REOPEN)) {
        imgui_flags = setBits<ImGuiPopupFlags>(imgui_flags, ImGuiPopupFlags_NoReopen);
    }
    if (checkBits<PopupFlags>(flags, PopupFlag::NO_OPEN_OVER_EXISTING_POPUP)) {
        imgui_flags =
            setBits<ImGuiPopupFlags>(imgui_flags, ImGuiPopupFlags_NoOpenOverExistingPopup);
    }
    if (checkBits<PopupFlags>(flags, PopupFlag::NO_OPEN_OVER_ITEMS)) {
        imgui_flags = setBits<ImGuiPopupFlags>(imgui_flags, ImGuiPopupFlags_NoOpenOverItems);
    }
    if (checkBits<PopupFlags>(flags, PopupFlag::ANY_POPUP_ID)) {
        imgui_flags = setBits<ImGuiPopupFlags>(imgui_flags, ImGuiPopupFlags_AnyPopupId);
    }
    if (checkBits<PopupFlags>(flags, PopupFlag::ANY_POPUP_LEVEL)) {
        imgui_flags = setBits<ImGuiPopupFlags>(imgui_flags, ImGuiPopupFlags_AnyPopupLevel);
    }
    if (checkBits<PopupFlags>(flags, PopupFlag::ANY_POPUP)) {
        imgui_flags = setBits<ImGuiPopupFlags>(imgui_flags, ImGuiPopupFlags_AnyPopup);
    }

    return imgui_flags;
}

} // namespace

PopupContextWindow::PopupContextWindow(std::string_view str_id, PopupFlags flags)
{
    setBeginFunc(ImGui::BeginPopupContextWindow, str_id.data(), toImGuiPopupFlags(flags));
    setEndFunc(ImGui::EndPopup);
}

PopupContextItem::PopupContextItem(std::string_view str_id, PopupFlags flags)
{
    setBeginFunc(&ImGui::BeginPopupContextItem, str_id.data(), toImGuiPopupFlags(flags));
    setEndFunc(&ImGui::EndPopup);
}

Popup::Popup(std::string_view str_id, PopupFlags flags)
{
    setBeginFunc(&ImGui::BeginPopup, str_id.data(), toImGuiPopupFlags(flags));
    setEndFunc(&ImGui::EndPopup);
}

void OpenPopup::call(std::string_view str_id)
{
    ImGui::OpenPopup(str_id.data());
}

void CloseCurrentPopup::call()
{
    ImGui::CloseCurrentPopup();
}

} // namespace GE::GUI
