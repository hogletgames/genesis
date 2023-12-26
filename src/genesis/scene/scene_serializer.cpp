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

#include "scene_serializer.h"
#include "component_list.h"
#include "components/yaml_convert.h"
#include "entity.h"
#include "scene.h"

#include "genesis/core/log.h"

#include <fstream>

namespace GE::Scene {

SceneSerializer::SceneSerializer(Scene *scene)
    : m_scene{scene}
{}

bool SceneSerializer::serialize(const std::string &config_filepath)
{
    if (!serializeScene()) {
        GE_CORE_ERR("Failed serialize the scene '{}'", m_scene->name());
        return false;
    }

    std::ofstream fout{config_filepath};

    if (!fout) {
        GE_CORE_ERR("Failed open scene config file '{}'", config_filepath);
        return false;
    }

    fout << m_serialized_scene;
    return true;
}

bool SceneSerializer::serializeScene()
{
    m_serialized_scene["scene"]["name"] = m_scene->name();
    auto entities = m_serialized_scene["scene"]["entities"];
    m_scene->forEachEntity(
        [this, &entities](const auto &entity) { entities.push_back(serializeEntity(entity)); });
    return true;
}

YAML::Node SceneSerializer::serializeEntity(const Entity &entity)
{
    YAML::Node serialized_entity;
    auto components = serialized_entity["entity"]["components"];

    forEachType<ComponentList>([&components, &entity](const auto &component) {
        using Component = std::decay_t<decltype(component)>;

        if (entity.has<Component>()) {
            components.push_back(entity.get<Component>());
        }
    });

    return serialized_entity;
}

} // namespace GE::Scene
