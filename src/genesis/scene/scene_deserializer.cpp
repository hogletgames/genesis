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

namespace GE::Scene {

SceneDeserializer::SceneDeserializer(Scene *scene, Assets::Registry *assets)
    : m_scene{scene}
    , m_assets{assets}
{}

bool SceneDeserializer::deserialize(const std::string &config_filepath)
{
    m_scene_buffer.clear();

    try {
        YAML::Node node = YAML::LoadFile(config_filepath);

        if (auto version = node["scene"]["serialization_version"].as<uint32_t>();
            version < Scene::SERIALIZATION_VERSION) {
            GE_CORE_ERR("Inconcistent serialization version: {}, expected: {}", version,
                        Scene::SERIALIZATION_VERSION);
            return false;
        }

        m_scene_buffer.setName(node["scene"]["name"].as<std::string>());

        if (auto entities = node["scene"]["entities"]; entities.size() > 0) {
            loadEntities(&m_scene_buffer, entities);
        }
    } catch (const std::exception &e) {
        GE_CORE_ERR("Failed to deserialize a scene from a file '{}': '{}'", config_filepath,
                    e.what());
        return false;
    }

    *m_scene = std::move(m_scene_buffer);
    return true;
}

// NOLINTNEXTLINE(misc-no-recursion)
Entity SceneDeserializer::loadEntities(Scene *scene, const YAML::Node &node)
{
    auto first_entity = loadEntity(scene, node[0]);
    auto last_entity = first_entity;

    for (uint32_t i{1}; i < node.size(); i++) {
        auto entity = loadEntity(scene, node[i]);

        if (auto children_node = node[i]["children"]; children_node.IsDefined()) {
            EntityNode{entity}.appendChild(loadEntities(scene, children_node));
        }

        EntityNode{last_entity}.insert(entity);
        last_entity = entity;
    }

    return first_entity;
}

Entity SceneDeserializer::loadEntity(Scene *scene, const YAML::Node &node)
{
    auto entity = scene->createEntity();

    for (auto component_node : node["components"]) {
        if (!loadComponent(&entity, component_node)) {
            GE_CORE_ERR("Failed to load components for an entity");
        }
    }

    return entity;
}

bool SceneDeserializer::loadComponent(Entity *entity, const YAML::Node &node)
{
    using Loader = void (SceneDeserializer::*)(Entity *, const YAML::Node &);

    static const std::unordered_map<std::string, Loader> LOADERS = {
        {CameraComponent::NAME.data(), &SceneDeserializer::loadCameraComponent},
        {MaterialComponent::NAME.data(), &SceneDeserializer::loadMaterialComponent},
        {SpriteComponent::NAME.data(), &SceneDeserializer::loadSpriteComponent},
        {TagComponent::NAME.data(), &SceneDeserializer::loadTagComponent},
        {TransformComponent::NAME.data(), &SceneDeserializer::loadTransformComponent},
    };

    if (!node["type"].IsDefined()) {
        GE_CORE_ERR("Failed to load component: 'type' is not defined");
        return false;
    }

    auto type = node["type"].as<std::string>();

    if (auto loader = getValue(LOADERS, type); loader) {
        try {
            std::invoke(loader, this, entity, node);
        } catch (const std::exception &e) {
            GE_CORE_WARN("Failed to load component '{}': '{}'", type, e.what());
        }
    }

    return true;
}

void SceneDeserializer::loadCameraComponent(Entity *entity, const YAML::Node &node)
{
    entity->add<CameraComponent>() = node.as<CameraComponent>();
}

void SceneDeserializer::loadMaterialComponent(Entity *entity, const YAML::Node &node)
{
    auto &material = entity->add<MaterialComponent>();
    material = node.as<MaterialComponent>();
    material.loadMaterial(m_assets);
}

void SceneDeserializer::loadSpriteComponent(Entity *entity, const YAML::Node &node)
{
    auto &sprite = entity->add<SpriteComponent>();
    sprite = node.as<SpriteComponent>();
    sprite.loadAll(m_assets);
}

void SceneDeserializer::loadTagComponent(Entity *entity, const YAML::Node &node)
{
    entity->get<TagComponent>() = node.as<TagComponent>();
}

void SceneDeserializer::loadTransformComponent(Entity *entity, const YAML::Node &node)
{
    entity->get<TransformComponent>() = node.as<TransformComponent>();
}

} // namespace GE::Scene
