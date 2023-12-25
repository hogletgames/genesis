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

#include <genesis/core/export.h>
#include <genesis/math/types.h>

namespace GE::Scene {

class GE_API ProjectionCamera
{
public:
    enum Type : uint8_t
    {
        ORTHOGRAPHIC,
        PERSPECTIVE
    };

    struct ortho_options_t {
        float size{SIZE_DEFAULT};
        float near{NEAR_DEFAULT};
        float far{FAR_DEFAULT};

        static constexpr float SIZE_DEFAULT{2.0f};
        static constexpr float NEAR_DEFAULT{-1.0f};
        static constexpr float FAR_DEFAULT{10.0f};
    };

    struct perspective_options_t {
        float fov{FOV_DEFAULT};
        float near{NEAR_DEFAULT};
        float far{FAR_DEFAULT};

        static constexpr float FOV_DEFAULT{60.0f};
        static constexpr float NEAR_DEFAULT{0.1f};
        static constexpr float FAR_DEFAULT{100.0f};
    };

    ProjectionCamera();

    const Mat4& projection() const { return m_projection; }
    const Vec2& viewport() const { return m_viewport; }

    void setType(Type type);
    void setOrthoOptions(const ortho_options_t& options);
    void setPerspectiveOptions(const perspective_options_t& options);
    void setViewport(const Vec2& viewport);

    Type type() const { return m_type; }
    const ortho_options_t& orthographicOptions() const { return m_ortho_options; }
    const perspective_options_t& perspectiveOptions() const { return m_perspective_options; }

private:
    void calculateProjection();
    void calculateOrthoProjection();
    void calculatePerspectiveProjection();

    float aspectRatio() const;

    Type m_type{Type::ORTHOGRAPHIC};
    ortho_options_t m_ortho_options;
    perspective_options_t m_perspective_options;

    Mat4 m_projection{1.0f};
    Vec2 m_viewport{720.0f, 480.0f};
};

ProjectionCamera::Type toProjectionType(std::string_view type_string);

} // namespace GE::Scene

namespace GE {

std::string toString(Scene::ProjectionCamera::Type type);

} // namespace GE
