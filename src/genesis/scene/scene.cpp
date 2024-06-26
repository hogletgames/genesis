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

#include "scene.h"
#include "components/relationship_components.h"
#include "components/tag_component.h"
#include "components/transform_component.h"
#include "entity.h"

namespace GE::Scene {
namespace {

constexpr auto DEFAULT_ENTITY_NAME{"Entity"};

} // namespace

Scene::Scene(Scene&& other) noexcept
    : m_name{std::move(other.m_name)}
    , m_registry{std::move(other.m_registry)}
    , m_main_camera{other.m_main_camera}
{}

Scene& Scene::operator=(Scene&& other) noexcept
{
    if (this != &other) {
        m_name = std::move(other.m_name);
        m_registry = std::move(other.m_registry);
        m_main_camera = other.m_main_camera;
    }

    return *this;
}

Entity Scene::createEntity(std::string_view name)
{
    auto entity = m_registry.create();
    entity.add<TagComponent>(!name.empty() ? name.data() : DEFAULT_ENTITY_NAME);
    entity.add<TransformComponent>();
    entity.add<NodeComponent>();

    if (m_registry.size() == 1) {
        entity.add<HeadNodeComponent>();
        entity.add<TailNodeComponent>();
    }

    return entity;
}

Entity Scene::entity(Entity::NativeHandle entity_handle)
{
    return m_registry.entity(entity_handle);
}

void Scene::destroyEntity(const Entity& entity)
{
    m_registry.destroy(entity);
}

void Scene::destroyEntity(Entity::NativeHandle entity_handle)
{
    m_registry.destroy(entity_handle);
}

void Scene::clear()
{
    m_registry.clear();
}

Entity Scene::headEntity() const
{
    return m_registry.firstEntityWith<HeadNodeComponent>();
}

Entity Scene::tailEnity() const
{
    return m_registry.firstEntityWith<TailNodeComponent>();
}

void Scene::forEachEntity(const Scene::ForeachCallback& callback)
{
    m_registry.eachEntity(callback);
}

void Scene::forEachEntity(const Scene::ForeachConstCallback& callback) const
{
    m_registry.eachEntity(callback);
}

} // namespace GE::Scene
