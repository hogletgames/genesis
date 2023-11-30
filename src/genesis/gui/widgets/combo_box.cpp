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

#include "widgets/combo_box.h"

#include <imgui.h>

namespace GE::GUI {
namespace {

bool beginCombo(std::string_view name, const ComboBox::Items& items, std::string_view current_item,
                ComboBox::Flags flags, std::string_view* selected_item)
{
    *selected_item = current_item;

    if (!ImGui::BeginCombo(name.data(), current_item.data(), flags)) {
        return false;
    }

    for (std::string_view item : items) {
        bool is_selected = item == *selected_item;

        if (ImGui::Selectable(item.data(), is_selected)) {
            *selected_item = item;
        }

        if (is_selected) {
            ImGui::SetItemDefaultFocus();
        }
    }

    return true;
}

} // namespace

ComboBox::ComboBox(std::string_view name, Items items, std::string_view current_item, Flags flags)
    : m_items{std::move(items)}
    , m_current_item{current_item}
    , m_selected_item{current_item}
{
    setBeginFunc(&beginCombo, name, m_items, m_current_item, flags, &m_selected_item);
    setEndFunc(&ImGui::EndCombo);
}

void ComboBox::emitSignals()
{
    if (m_selected_item != m_current_item) {
        m_item_changed(m_selected_item);
    }
}

} // namespace GE::GUI
