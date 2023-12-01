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

#include <genesis/gui/widgets/widget_node.h>
#include <genesis/math/types.h>

namespace GE::GUI {

class GE_API Gizmos
{
public:
    enum Operation : uint8_t
    {
        TRANSLATE,
        ROTATE,
        SCALE,
    };

    enum Mode : uint8_t
    {
        LOCAL,
        GLOBAL,
    };

    Gizmos(const Vec2& window_pos, const Vec2& window_size, bool is_ortho = false);

    void draw(const Mat4& view, const Mat4& projection, Operation operation, Mode mode,
              Mat4* matrix, float* snap = nullptr);

    bool isOver() const;
    bool isUsing() const;
    const Mat4& deltaMatrix() const { return m_matrix_delta; }

private:
    Mat4 m_matrix_delta{0.0f};
};

} // namespace GE::GUI
