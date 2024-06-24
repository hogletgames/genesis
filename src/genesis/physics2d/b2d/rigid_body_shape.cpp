/*
 * BSD 3-Clause License
 *
 * Copyright (c) 2024, Dmitry Shilnenkov
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

#include "rigid_body_shape.h"

#include "math_types.h"

namespace GE::P2D::Box2D {
namespace {}

b2FixtureDef toFixtureDef(const body_shape_config_base_t& config)
{
    b2FixtureDef fixture_def{};
    fixture_def.friction = config.friction;
    fixture_def.restitution = config.restitution;
    fixture_def.restitutionThreshold = config.restitution_threshold;
    fixture_def.density = config.density;
    return fixture_def;
}

b2PolygonShape toShape(const box_body_shape_config_t& config)
{
    b2PolygonShape shape;
    shape.SetAsBox(config.size.x, config.size.y, toB2Vec2(config.center), config.angle);
    return shape;
}

b2CircleShape toShape(const circle_body_shape_config_t& config)
{
    b2CircleShape shape{};
    shape.m_p = toB2Vec2(config.offset);
    shape.m_radius = config.radius;
    return shape;
}

} // namespace GE::P2D::Box2D
