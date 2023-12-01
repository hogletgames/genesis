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

#include "gizmos/gizmos.h"

#include "genesis/core/utils.h"

#include <imgui.h>

#include <ImGuizmo.h>

namespace GE::GUI {
namespace {

ImGuizmo::OPERATION toImGuizmo(Gizmos::Operation operation)
{
    static const std::unordered_map<Gizmos::Operation, ImGuizmo::OPERATION> TO_IM_GUIZMO = {
        {Gizmos::Operation::TRANSLATE, ImGuizmo::TRANSLATE},
        {Gizmos::Operation::ROTATE, ImGuizmo::ROTATE},
        {Gizmos::Operation::SCALE, ImGuizmo::SCALE},
    };

    return getValue(TO_IM_GUIZMO, operation);
}

ImGuizmo::MODE toImGuizmo(Gizmos::Mode mode)
{
    static const std::unordered_map<Gizmos::Mode, ImGuizmo::MODE> TO_IM_GUIZMO = {
        {Gizmos::Mode::LOCAL, ImGuizmo::LOCAL},
        {Gizmos::Mode::GLOBAL, ImGuizmo::WORLD},
    };

    return getValue(TO_IM_GUIZMO, mode);
}

} // namespace

Gizmos::Gizmos(const Vec2& window_pos, const Vec2& window_size, bool is_ortho)
{
    ImGuizmo::BeginFrame();
    ImGuizmo::SetOrthographic(is_ortho);
    ImGuizmo::SetDrawlist();
    ImGuizmo::SetRect(window_pos.x, window_pos.y, window_size.x, window_size.y);
}

void Gizmos::draw(const Mat4& view, const Mat4& projection, Operation operation, Mode mode,
                  Mat4* matrix, float* snap)
{
    ImGuizmo::Manipulate(value_ptr(view), value_ptr(projection), toImGuizmo(operation),
                         toImGuizmo(mode), value_ptr(*matrix), value_ptr(m_matrix_delta), snap);
}

bool Gizmos::isOver() const
{
    return ImGuizmo::IsOver();
}

bool Gizmos::isUsing() const
{
    return ImGuizmo::IsUsing();
}

} // namespace GE::GUI
