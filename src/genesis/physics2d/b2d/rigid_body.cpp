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

#include <box2d/b2_circle_shape.h>
#include <box2d/b2_fixture.h>
#include <box2d/b2_polygon_shape.h>
#include <box2d/b2_world.h>

namespace GE::P2D::Box2D {

RigidBody::RigidBody(b2Body* body)
    : m_body{body}
{}

RigidBody::~RigidBody()
{
    m_body->GetWorld()->DestroyBody(m_body);
}

void RigidBody::createFixure(const box_body_shape_config_t& shape_config)
{
    b2PolygonShape shape{toShape(shape_config)};
    b2FixtureDef fixture_def{toFixtureDef(shape_config)};
    fixture_def.shape = &shape;

    m_body->CreateFixture(&fixture_def);
}

void RigidBody::createFixure(const circle_body_shape_config_t& shape_config)
{
    b2CircleShape shape{toShape(shape_config)};
    b2FixtureDef fixture_def{toFixtureDef(shape_config)};
    fixture_def.shape = &shape;

    m_body->CreateFixture(&fixture_def);
}

void RigidBody::setFixedRotation(bool flag)
{
    m_body->SetFixedRotation(flag);
}

bool RigidBody::isFixedRotation() const
{
    return m_body->IsFixedRotation();
}

Vec2 RigidBody::position() const
{
    return toVec2(m_body->GetPosition());
}

float RigidBody::angle() const
{
    return m_body->GetAngle();
}

RigidBody::Type toRigidBody(b2BodyType type)
{
    switch (type) {
        case b2_staticBody: return RigidBody::Type::STATIC;
        case b2_dynamicBody: return RigidBody::Type::DYNAMIC;
        case b2_kinematicBody: return RigidBody::Type::KINEMATIC;
        default: GE_CORE_ASSERT(false, "Unsuported body type '{}'", type);
    }

    return RigidBody::Type::STATIC;
}

b2BodyType fromRigidBody(RigidBody::Type type)
{
    switch (type) {
        case RigidBody::Type::STATIC: return b2_staticBody;
        case RigidBody::Type::DYNAMIC: return b2_dynamicBody;
        case RigidBody::Type::KINEMATIC: return b2_kinematicBody;
        default: GE_CORE_ASSERT(false, "Unsupported body type '{}'", type);
    }

    return b2_staticBody;
}

} // namespace GE::P2D::Box2D
