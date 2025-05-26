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

#include "world.h"
#include "math_types.h"
#include "rigid_body.h"

#include <box2d/box2d.h>

namespace GE::P2D::Box2D {
namespace {

b2WorldId createWorld(const Vec2& gravity)
{
    b2WorldDef world_def{b2DefaultWorldDef()};
    world_def.gravity = toB2Vec2(gravity);

    return b2CreateWorld(&world_def);
}

} // namespace

World::World(const Vec2& gravity)
    : m_world{createWorld(gravity)}
{}

World::~World()
{
    b2DestroyWorld(m_world);
}

void World::step(Timestamp ts, int32_t sub_step_count)
{
    b2World_Step(m_world, ts.sec(), sub_step_count);
}

Scoped<P2D::RigidBody> World::createRigidBody(RigidBody::Type type,
                                              const Vec2&     position,
                                              float           angle)
{
    b2BodyDef body_def{b2DefaultBodyDef()};
    body_def.type = fromRigidBody(type);
    body_def.position = toB2Vec2(position);
    body_def.rotation = b2MakeRot(angle);

    return makeScoped<Box2D::RigidBody>(b2CreateBody(m_world, &body_def));
}

} // namespace GE::P2D::Box2D
