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

#include "camera/projection_camera.h"

#include "genesis/core/asserts.h"
#include "genesis/core/utils.h"
#include "genesis/math/camera.h"
#include "genesis/math/trigonometric.h"

using std::string_view_literals::operator""sv;

namespace {

std::unordered_map<GE::Scene::ProjectionCamera::Type, std::string_view> getProjToStringMap()
{
    static const std::unordered_map<GE::Scene::ProjectionCamera::Type, std::string_view>
        PROJECTION_TO_STRING = {
            {GE::Scene::ProjectionCamera::ORTHOGRAPHIC, "Orthographic"sv},
            {GE::Scene::ProjectionCamera::PERSPECTIVE, "Perspective"sv},
        };

    return PROJECTION_TO_STRING;
}

} // namespace

namespace GE::Scene {

ProjectionCamera::ProjectionCamera()
{
    calculateProjection();
}

void ProjectionCamera::setType(ProjectionCamera::Type type)
{
    m_type = type;
    calculateProjection();
}

void ProjectionCamera::setOrthoOptions(const ortho_options_t& options)
{
    m_ortho_options = options;
    m_type = ORTHOGRAPHIC;
    calculateProjection();
}

void ProjectionCamera::setPerspectiveOptions(const perspective_options_t& options)
{
    m_perspective_options = options;
    m_type = PERSPECTIVE;
    calculateProjection();
}

void ProjectionCamera::setViewport(const Vec2& viewport)
{
    GE_ASSERT(viewport.length() > 0.0f, "Incorrect viewport: {}", toString(viewport));
    m_viewport = viewport;
    calculateProjection();
}

void ProjectionCamera::calculateProjection()
{
    switch (m_type) {
        case ORTHOGRAPHIC: calculateOrthoProjection(); break;
        case PERSPECTIVE: calculatePerspectiveProjection(); break;
        default: GE_ASSERT(false, "Unknown projection type: {}", static_cast<int>(m_type)); break;
    }
}

void ProjectionCamera::calculateOrthoProjection()
{
    float aspect_ratio = aspectRatio();
    float left = -m_ortho_options.size * aspect_ratio * 0.5f;
    float right = m_ortho_options.size * aspect_ratio * 0.5f;
    float bottom = -m_ortho_options.size * 0.5f;
    float top = m_ortho_options.size * 0.5f;

    m_projection = ortho(left, right, bottom, top);
}

void ProjectionCamera::calculatePerspectiveProjection()
{
    const auto& options = m_perspective_options;
    m_projection = perspective(radians(options.fov), aspectRatio(), options.near, options.far);
}

float ProjectionCamera::aspectRatio() const
{
    return m_viewport.x / m_viewport.y;
}

ProjectionCamera::Type toProjectionType(std::string_view type_string)
{
    static const auto TO_TYPE{swapKeyAndValue(getProjToStringMap())};
    return getValue(TO_TYPE, type_string);
}

} // namespace GE::Scene

namespace GE {

std::string toString(Scene::ProjectionCamera::Type type)
{
    static const auto TO_STRING{getProjToStringMap()};
    return getValue(TO_STRING, type, "Unknown"sv).data();
}

} // namespace GE
