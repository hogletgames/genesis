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

#include "input_text.h"
#include "private/types.h"

#include "genesis/core/defer.h"

#include <imgui.h>

#include <algorithm>
#include <vector>

namespace GE::GUI {

bool InputText::call(std::string_view label, std::string* output, Flags flags)
{
    static constexpr size_t BUFFER_SIZE{1024};
    std::vector<char>       buffer(BUFFER_SIZE, 0);
    std::ranges::copy(*output, buffer.begin());

    ImGui::PushID(output);
    Defer defer{[] { ImGui::PopID(); }};

    if (!ImGui::InputText(label.data(), buffer.data(), buffer.size(), flags)) {
        return false;
    }

    *output = std::string(buffer.data());
    return true;
}

void InputTextMultiline::call(std::string* text, const Vec2& widget_size)
{
    ImGui::PushID(text->c_str());
    ImGui::InputTextMultiline("##text", text->data(), text->size(), toImVec2(widget_size),
                              ImGuiInputTextFlags_ReadOnly);
    ImGui::PopID();
}

} // namespace GE::GUI
