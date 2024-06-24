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

#include "rigid_body.h"
#include "math_types.h"
#include "rigid_body_shape.h"

#include "genesis/core/asserts.h"

#include <box2d/box2d.h>

namespace GE::P2D::Box2D {

RigidBody::RigidBody(const b2BodyId& body)
    : m_body{body}
{}

RigidBody::~RigidBody()
{
    b2DestroyBody(m_body);
}

void RigidBody::createShape(const box_body_shape_config_t& shape_config)
{
    auto shape{toB2ShapeDef(shape_config)};
    auto polygon{toB2Polygon(shape_config)};

    b2CreatePolygonShape(m_body, &shape, &polygon);
}

void RigidBody::createShape(const circle_body_shape_config_t& shape_config)
{
    auto shape{toB2ShapeDef(shape_config)};
    auto circle{toB2Circle(shape_config)};

    b2CreateCircleShape(m_body, &shape, &circle);
}

void RigidBody::setFixedRotation(bool flag)
{
    b2Body_SetFixedRotation(m_body, flag);
}

bool RigidBody::isFixedRotation() const
{
    return b2Body_IsFixedRotation(m_body);
}

Vec2 RigidBody::position() const
{
    return toVec2(b2Body_GetPosition(m_body));
}

float RigidBody::angle() const
{
    return b2Rot_GetAngle(b2Body_GetRotation(m_body));
}

RigidBody::Type toRigidBody(b2BodyType type)
{
    switch (type) {
        case b2_staticBody: return RigidBody::Type::STATIC;
        case b2_dynamicBody: return RigidBody::Type::DYNAMIC;
        case b2_kinematicBody: return RigidBody::Type::KINEMATIC;
        default: GE_CORE_ASSERT(false, "Unsupported body type '{}'", toString(type));
    }

    return RigidBody::Type::STATIC;
}

::b2BodyType fromRigidBody(RigidBody::Type type)
{
    switch (type) {
        case RigidBody::Type::STATIC: return b2_staticBody;
        case RigidBody::Type::DYNAMIC: return b2_dynamicBody;
        case RigidBody::Type::KINEMATIC: return b2_kinematicBody;
        default: GE_CORE_ASSERT(false, "Unsupported body type '{}'", toString(type));
    }

    return b2_staticBody;
}

} // namespace GE::P2D::Box2D
