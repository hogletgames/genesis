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

#include "executor/runtime2d_executor.h"
#include "components/physics2d_components.h"
#include "components/transform_component.h"
#include "entity.h"
#include "scene.h"

#include "genesis/math/types.h"
#include "genesis/physics2d/rigid_body.h"
#include "genesis/physics2d/world.h"

namespace GE::Scene {
namespace {

constexpr int32_t VELOCITY_ITERATIONS{2};
constexpr int32_t POSITION_ITERATIONS{3};

void updateEntityTransform(Entity& entity)
{
    const auto& rigid_body = entity.get<RigidBody2DComponent>().body;
    auto body_position = rigid_body->position();

    auto& transform = entity.get<TransformComponent>();
    transform.translation.x = body_position.x;
    transform.translation.y = body_position.y;
    transform.rotation.z = rigid_body->angle();
}

} // namespace

Runtime2DExecutor::Runtime2DExecutor(Scene* scene, P2D::World* physics_world)
    : m_scene{scene}
    , m_world{physics_world}
{
    initializePhysics2D();
}

Runtime2DExecutor::~Runtime2DExecutor()
{
    resetRigidBody2D();
}

void Runtime2DExecutor::onUpdate(Timestamp timestamp)
{
    if (m_is_paused) {
        return;
    }

    m_world->step(timestamp, VELOCITY_ITERATIONS, POSITION_ITERATIONS);
    m_scene->forEachEntity(&updateEntityTransform);
}

void Runtime2DExecutor::initializePhysics2D()
{
    m_scene->forEach<RigidBody2DComponent>([this](Entity& entity) {
        const auto& transform = entity.get<TransformComponent>();
        auto& rigid_body = entity.get<RigidBody2DComponent>();

        rigid_body.body = m_world->createRigidBody(rigid_body.body_type, transform.translation,
                                                   rigid_body.fixed_rotation);

        if (entity.has<CircleCollider2DComponent>()) {
            rigid_body.body->createFixure(entity.get<CircleCollider2DComponent>());
        } else if (entity.has<BoxCollider2DComponent>()) {
            rigid_body.body->createFixure(entity.get<BoxCollider2DComponent>());
        }
    });
}

void Runtime2DExecutor::resetRigidBody2D()
{
    m_scene->forEach<RigidBody2DComponent>(
        [](Entity& entity) { entity.get<RigidBody2DComponent>().body.reset(); });
}

} // namespace GE::Scene
