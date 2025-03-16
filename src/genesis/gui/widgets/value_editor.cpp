/*
 * BSD 3-Clause License
 *
 * Copyright (c) 2022, Dmitry Shilnenkov
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

#include "value_editor.h"

#include "genesis/core/defer.h"

#include <imgui.h>

namespace GE::GUI {

bool ValueEditor::call(std::string_view label, Vec2* value, float v_speed, float v_min, float v_max,
                       std::string_view format, ValueEditor::Flags flags)
{
    ImGui::PushID(value);
    GE_DEFER([] { ImGui::PopID(); });
    return ImGui::DragFloat2(label.data(), value_ptr(*value), v_speed, v_min, v_max, format.data(),
                             flags);
}

bool ValueEditor::call(std::string_view label, Vec3* value, float v_speed, float v_min, float v_max,
                       std::string_view format, Flags flags)
{
    ImGui::PushID(value);
    GE_DEFER([] { ImGui::PopID(); });
    return ImGui::DragFloat3(label.data(), value_ptr(*value), v_speed, v_min, v_max, format.data(),
                             flags);
}

bool ValueEditor::call(std::string_view label, float* value, float v_speed, float v_min,
                       float v_max, std::string_view format, ValueEditor::Flags flags)
{
    ImGui::PushID(value);
    GE_DEFER([] { ImGui::PopID(); });
    return ImGui::DragFloat(label.data(), value, v_speed, v_min, v_max, format.data(), flags);
}

} // namespace GE::GUI
