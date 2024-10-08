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

#include "app_settings.h"
#include "project_settings.h"
#include "settings.h"

#include <genesis/scene/camera/yaml_convert.h>

#include <yaml-cpp/node/convert.h>
#include <yaml-cpp/node/node.h>

namespace YAML {

template<>
struct convert<LE::AppSettings> {
    static bool decode(const Node& node, LE::AppSettings& app_settings)
    {
        auto camera = GE::makeShared<GE::Scene::ViewProjectionCamera>();
        *camera = node["camera"].as<GE::Scene::ViewProjectionCamera>();

        app_settings.setCamera(camera);
        return true;
    }

    static Node encode(const LE::AppSettings& project)
    {
        Node node;
        node["camera"] = *project.camera();
        return node;
    }
};

template<>
struct convert<LE::ProjectSettings> {
    static bool decode(const Node& node, LE::ProjectSettings& project)
    {
        project.setName(node["name"].as<std::string>())
            .setProjectPath(node["project_path"].as<std::string>())
            .setAssetsPath(node["assets_path"].as<std::string>())
            .setScenePath(node["scene_path"].as<std::string>());
        return true;
    }

    static Node encode(const LE::ProjectSettings& project)
    {
        Node node;
        node["name"] = project.name();
        node["project_path"] = project.projectPath();
        node["assets_path"] = project.assetsPath();
        node["scene_path"] = project.scenePath();
        return node;
    }
};

} // namespace YAML
