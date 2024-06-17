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
#include "entity_node.h"
#include "scene.h"

#include "genesis/core/log.h"

#include <fstream>

namespace GE::Scene {

SceneSerializer::SceneSerializer(Scene *scene)
    : m_scene{scene}
{}

bool SceneSerializer::serialize(const std::string &config_filepath)
{
    auto serialized_scene = serializeScene();
    std::ofstream fout{config_filepath};

    if (!fout) {
        GE_CORE_ERR("Failed open scene config file '{}'", config_filepath);
        return false;
    }

    fout << serialized_scene;
    return true;
}

YAML::Node SceneSerializer::serializeScene()
{
    YAML::Node serialized_scene;
    serialized_scene["scene"]["name"] = m_scene->name();
    serialized_scene["scene"]["serialization_version"] = Scene::SERIALIZATION_VERSION;

    if (auto head_enity = m_scene->headEntity(); !head_enity.isNull()) {
        auto entities = serialized_scene["scene"]["entities"];
        serializeEntityNodes(&entities, EntityNode{head_enity}); // NOLINT(misc-no-recursion)
    }

    return serialized_scene;
}

// NOLINTNEXTLINE(misc-no-recursion)
void SceneSerializer::serializeEntityNode(YAML::Node *root, const EntityNode &entity)
{
    forEachType<ComponentList>([root, &entity = entity.entity()](const auto &component) {
        using Component = std::decay_t<decltype(component)>;
        auto components_node = (*root)["components"];

        if (entity.has<Component>()) {
            components_node.push_back(entity.get<Component>());
        }
    });

    if (entity.hasChildNode()) {
        auto children_node = (*root)["children"];
        serializeEntityNodes(&children_node, entity.childNode());
    }
}

// NOLINTNEXTLINE(misc-no-recursion)
void SceneSerializer::serializeEntityNodes(YAML::Node *root, const EntityNode &entity)
{
    for (auto current_entity = entity; !current_entity.isNull();
         current_entity = current_entity.nextNode()) {
        YAML::Node serialized_entity{YAML::NodeType::Sequence};
        serializeEntityNode(&serialized_entity, current_entity);
        root->push_back(serialized_entity);
    }
}

} // namespace GE::Scene
