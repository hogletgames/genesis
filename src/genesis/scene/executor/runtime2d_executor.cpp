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
#include "entity_node.h"
#include "scene.h"
#include "scene_serializer.h"

#include "genesis/math/types.h"
#include "genesis/physics2d/rigid_body.h"
#include "genesis/physics2d/world.h"
#include "glm/gtc/matrix_inverse.hpp"
#include "renderer/renderer_base.h"

namespace GE::Scene {
namespace {

constexpr int32_t SUB_STEP_COUNT{4};

Mat4 entityTransform(const Entity& entity)
{
    return entity.get<TransformComponent>().transform();
}

Mat4 rigidBodyTransform(const P2D::RigidBody& rigid_body)
{
    return makeTransform2D(rigid_body.position(), rigid_body.angle());
}

void updateTransform(Entity* entity, const Mat4& parent_transform)
{
    const auto& rigid_body = *entity->get<RigidBody2DComponent>().body;
    auto        local_transform = affineInverse(parent_transform) * rigidBodyTransform(rigid_body);
    auto [translation, rotation, scale] = decompose(local_transform);

    auto& transform = entity->get<TransformComponent>();
    transform.translation = translation;
    transform.rotation = rotation;
}

// NOLINTNEXTLINE(misc-no-recursion)
void updateEntities(const EntityNode& node, const Mat4& parent_transform = Mat4{1.0f})
{
    auto current_entity = node;

    while (!current_entity.isNull()) {
        if (current_entity.entity().has<RigidBody2DComponent>()) {
            updateTransform(&current_entity.entity(), parent_transform);
        }

        if (current_entity.hasChildNode()) {
            updateEntities(current_entity.childNode(),
                           parent_transform * entityTransform(current_entity.entity()));
        }

        current_entity = current_entity.nextNode();
    }
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

    m_world->step(timestamp, SUB_STEP_COUNT);
    updateEntities(EntityNode{m_scene->headEntity()});
}

void Runtime2DExecutor::initializePhysics2D()
{
    m_scene->forEach<RigidBody2DComponent>([this](Entity& entity) {
        const auto& transform_component = entity.get<TransformComponent>();
        auto        transform = parentTransform(entity) * transform_component.transform();
        auto [translation, rotation, scale] = decompose(transform);

        auto& rigid_body = entity.get<RigidBody2DComponent>();
        rigid_body.body = m_world->createRigidBody(rigid_body.body_type, translation, rotation.z);

        if (entity.has<CircleCollider2DComponent>()) {
            float scale_max = std::max(scale.x, scale.y);

            auto circle_shape = entity.get<CircleCollider2DComponent>();
            circle_shape.radius *= scale_max;
            circle_shape.offset *= scale_max;
            rigid_body.body->createShape(circle_shape);
        } else if (entity.has<BoxCollider2DComponent>()) {
            Vec2 scale_2d{scale};

            auto box_shape = entity.get<BoxCollider2DComponent>();
            box_shape.size *= scale_2d;
            box_shape.center *= scale_2d;
            rigid_body.body->createShape(box_shape);
        }
    });
}

void Runtime2DExecutor::resetRigidBody2D()
{
    m_scene->forEach<RigidBody2DComponent>(
        [](Entity& entity) { entity.get<RigidBody2DComponent>().body.reset(); });
}

} // namespace GE::Scene
