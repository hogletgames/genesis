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

#include <genesis/math/trigonometric.h>
#include <genesis/math/types.h>

#include <glm/ext/matrix_transform.hpp>
#include <glm/gtx/matrix_decompose.hpp>
#include <glm/gtx/rotate_vector.hpp>

namespace GE {

using glm::rotate;
using glm::scale;
using glm::translate;

inline Mat4 makeTransform3D(const Vec3& translation,
                            const Vec3& rotation,
                            const Vec3& scale = Vec3{1.0f})
{
    return GE::translate(Mat4{1.0f}, translation) * toMat4(Quat{rotation}) * GE::scale(scale);
}

inline Mat4 makeTransform2D(const Vec2& translation,
                            float       rotation_z,
                            const Vec2& scale = Vec2{1.0f, 1.0f})
{
    return makeTransform3D(Vec3{translation, 0.0f}, Vec3{0.0f, 0.0f, rotation_z},
                           Vec3{scale, 1.0f});
}

inline std::tuple<Vec3, Vec3, Vec3> decompose(const Mat4& transform)
{
    Vec3 scale;
    Quat orientation;
    Vec3 translation;
    Vec3 skew;
    Vec4 perspective;

    glm::decompose(transform, scale, orientation, translation, skew, perspective);
    return {translation, eulerAngles(orientation), scale};
}

} // namespace GE
