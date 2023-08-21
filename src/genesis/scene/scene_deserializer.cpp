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
#include "scene.h"

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
    auto node = YAML::LoadFile(config_filepath);

    if (node.IsNull()) {
        GE_CORE_ERR("Failed to load a scene config file from the '{}'", config_filepath);
        return false;
    }

    m_scene->clear();
    m_scene->setName(node["scene"]["name"].as<std::string>());
    return loadEntities(node["scene"]["entities"]);
}

bool SceneDeserializer::loadEntities(const YAML::Node &node)
{
    for (auto entity_node : node) {
        if (!loadEntity(entity_node["entity"])) {
            return false;
        }
    }

    return true;
}

bool SceneDeserializer::loadEntity(const YAML::Node &node)
{
    auto entity = m_scene->createEntity({});

    for (auto component_node : node["components"]) {
        if (!loadComponent(&entity, component_node)) {
            GE_CORE_ERR("Failed to load components for an entity");
            return false;
        }
    }

    return true;
}

bool SceneDeserializer::loadComponent(Entity *entity, const YAML::Node &node)
{
    using Loader = bool (SceneDeserializer::*)(Entity *, const YAML::Node &);

    static const std::unordered_map<std::string, Loader> LOADERS = {
        {MaterialComponent::NAME.data(), &SceneDeserializer::loadCameraComponent},
        {MaterialComponent::NAME.data(), &SceneDeserializer::loadMaterialComponent},
        {SpriteComponent::NAME.data(), &SceneDeserializer::loadSpriteComponent},
        {TagComponent::NAME.data(), &SceneDeserializer::loadTagComponent},
        {TransformComponent::NAME.data(), &SceneDeserializer::loadTransformComponent},
    };

    if (auto loader = getValue(LOADERS, node["type"].as<std::string>())) {
        return std::invoke(loader, this, entity, node);
    }

    return false;
}

bool SceneDeserializer::loadCameraComponent(Entity *entity, const YAML::Node &node)
{
    entity->add<CameraComponent>() = node.as<CameraComponent>();
    return false;
}

bool SceneDeserializer::loadMaterialComponent(Entity *entity, const YAML::Node &node)
{
    auto &material = entity->add<MaterialComponent>();
    material = node.as<MaterialComponent>();
    material.loadMaterial(m_assets);
    return material.isValid();
}

bool SceneDeserializer::loadSpriteComponent(Entity *entity, const YAML::Node &node)
{
    auto &sprite = entity->add<SpriteComponent>();
    sprite = node.as<SpriteComponent>();
    sprite.loadAll(m_assets);
    return sprite.isValid();
}

bool SceneDeserializer::loadTagComponent(Entity *entity, const YAML::Node &node)
{
    entity->get<TagComponent>() = node.as<TagComponent>();
    return true;
}

bool SceneDeserializer::loadTransformComponent(Entity *entity, const YAML::Node &node)
{
    entity->get<TransformComponent>() = node.as<TransformComponent>();
    return true;
}

} // namespace GE::Scene
