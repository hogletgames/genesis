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

#pragma once

#include <genesis/core/export.h>
#include <genesis/scene/scene.h>

namespace GE::Assets {
class Registry;
} // namespace GE::Assets

namespace YAML {
class Node;
} // namespace YAML

namespace GE::Scene {

class Entity;

class GE_API SceneDeserializer
{
public:
    SceneDeserializer(Scene* scene, Assets::Registry* assets);

    bool deserialize(const std::string& config_filepath);

private:
    Entity loadEntities(Scene* scene, const YAML::Node& node);
    Entity loadEntity(Scene* scene, const YAML::Node& node);
    bool loadComponent(Entity* entity, const YAML::Node& node);
    void loadCameraComponent(Entity* entity, const YAML::Node& node);
    void loadMaterialComponent(Entity* entity, const YAML::Node& node);
    void loadSpriteComponent(Entity* entity, const YAML::Node& node);
    void loadTagComponent(Entity* entity, const YAML::Node& node);
    void loadTransformComponent(Entity* entity, const YAML::Node& node);

    Scene m_scene_buffer;
    Scene* m_scene{nullptr};
    Assets::Registry* m_assets{nullptr};
};

} // namespace GE::Scene
