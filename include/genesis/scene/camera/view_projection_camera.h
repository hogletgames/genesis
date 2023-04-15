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

#include <genesis/scene/camera/projection_camera.h>

namespace GE::Scene {

class GE_API ViewProjectionCamera: public ProjectionCamera
{
public:
    ViewProjectionCamera();

    const Mat4& view() const { return m_view; }

    void setDistance(float distance);
    void setRotationAngles(float pitch, float yaw);
    void setFocalPoint(const Vec3& focal_point);

    float distance() const { return m_distance; }
    float pitch() const { return m_pitch; }
    float yaw() const { return m_yaw; }
    const Vec3& focalPoint() const { return m_focal_point; }
    Quat orientation() const { return Quat{Vec3{-m_yaw, -m_pitch, 0.0f}}; }
    Vec3 position() const { return m_focal_point - (forwardDirection() * m_distance); }

    Vec3 upDirection() const;
    Vec3 rightDirection() const;
    Vec3 forwardDirection() const;

private:
    void calculateView();

    float m_distance{2.0f};
    float m_yaw{0.0f};
    float m_pitch{0.0f};
    Vec3 m_focal_point{0.0f};
    Mat4 m_view{1.0f};
};

} // namespace GE::Scene
