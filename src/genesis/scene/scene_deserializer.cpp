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

#include "scene_deserializer.h"
#include "components.h"
#include "entity.h"
#include "entity_node.h"
#include "scene.h"

#include "genesis/core/log.h"
#include "genesis/core/utils.h"

#include <yaml-cpp/yaml.h>

#include <unordered_map>

#define BIND_LOADER(mem_function)                                                                  \
    [this](auto* entity, const auto& node) { mem_function(entity, node); }

#define ADD_LOADER(component_type) {component_type::NAME.data(), &load<component_type>}

#define ADD_MEM_FN_LOADER(component_type)                                                          \
    {component_type::NAME.data(), BIND_LOADER(load##component_type)}

namespace GE::Scene {
namespace {

template<typename ComponentType>
void load(Entity* entity, const YAML::Node& node)
{
    if (!entity->has<ComponentType>()) {
        entity->add<ComponentType>();
    }

    entity->get<ComponentType>() = node.as<ComponentType>();
}

} // namespace

SceneDeserializer::SceneDeserializer(Scene* scene, Assets::Registry* assets)
    : m_scene{scene}
    , m_assets{assets}
{}

bool SceneDeserializer::deserialize(const std::string& config_filepath)
{
    m_scene_buffer.clear();

    try {
        YAML::Node node = YAML::LoadFile(config_filepath);

        if (auto version = node["scene"]["serialization_version"].as<uint32_t>();
            version < Scene::SERIALIZATION_VERSION) {
            GE_CORE_ERR("Inconsistent serialization version: {}, expected: {}", version,
                        Scene::SERIALIZATION_VERSION);
            return false;
        }

        m_scene_buffer.setName(node["scene"]["name"].as<std::string>());

        if (auto entities = node["scene"]["entities"]; entities.size() > 0) {
            loadEntities(&m_scene_buffer, entities);
        }
    } catch (const std::exception& e) {
        GE_CORE_ERR("Failed to deserialize a scene from a file '{}': '{}'", config_filepath,
                    e.what());
        return false;
    }

    *m_scene = std::move(m_scene_buffer);
    return true;
}

// NOLINTNEXTLINE(misc-no-recursion)
Entity SceneDeserializer::loadEntities(Scene* scene, const YAML::Node& node)
{
    auto first_entity = loadEntity(scene, node[0]);
    auto last_entity = first_entity;

    for (uint32_t i{1}; i < node.size(); i++) {
        auto entity = loadEntity(scene, node[i]);
        EntityNode{last_entity}.insert(entity);
        last_entity = entity;
    }

    return first_entity;
}

// NOLINTNEXTLINE(misc-no-recursion)
Entity SceneDeserializer::loadEntity(Scene* scene, const YAML::Node& node)
{
    auto entity = scene->createEntity();

    for (auto component_node : node["components"]) {
        if (!loadComponent(&entity, component_node)) {
            GE_CORE_ERR("Failed to load components for an entity");
        }
    }

    if (auto children_node = node["children"]; children_node.IsDefined()) {
        EntityNode{entity}.appendChild(loadEntities(scene, children_node));
    }

    return entity;
}

bool SceneDeserializer::loadComponent(Entity* entity, const YAML::Node& node)
{
    using Loader = std::function<void(Entity*, const YAML::Node&)>;

    const std::unordered_map<std::string, Loader> LOADERS = {
        ADD_LOADER(CameraComponent),
        ADD_MEM_FN_LOADER(MaterialComponent),
        ADD_LOADER(RigidBody2DComponent),
        ADD_LOADER(BoxCollider2DComponent),
        ADD_LOADER(CircleCollider2DComponent),
        ADD_MEM_FN_LOADER(SpriteComponent),
        ADD_LOADER(TagComponent),
        ADD_LOADER(TransformComponent),
    };

    if (!node["type"].IsDefined()) {
        GE_CORE_ERR("Failed to load component: 'type' is not defined");
        return false;
    }

    auto type = node["type"].as<std::string>();

    if (auto loader = getValue(LOADERS, type); loader) {
        try {
            std::invoke(loader, entity, node);
        } catch (const std::exception& e) {
            GE_CORE_WARN("Failed to load component '{}': '{}'", type, e.what());
        }
    }

    return true;
}

void SceneDeserializer::loadMaterialComponent(Entity* entity, const YAML::Node& node)
{
    if (auto material = node.as<MaterialComponent>(); material.loadMaterial(m_assets)) {
        entity->add<MaterialComponent>() = material;
    }
}

void SceneDeserializer::loadSpriteComponent(Entity* entity, const YAML::Node& node)
{
    if (auto sprite = node.as<SpriteComponent>(); sprite.loadAll(m_assets)) {
        entity->add<SpriteComponent>() = sprite;
    }
}

} // namespace GE::Scene
