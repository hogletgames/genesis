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

#include "project_settings.h"
#include "settings.h"

#include <yaml-cpp/yaml.h>

namespace YAML {

template<>
struct convert<LE::ResourcePaths> {
    static bool decode(const Node& node, LE::ResourcePaths& resources)
    {
        resources.setResourcesDir(node["resources_dir"].as<std::string>())
            .setPlayButtonIconPath(node["play_button_icon"].as<std::string>())
            .setSimulationButtonIconPath(node["simulation_button_icon"].as<std::string>())
            .setStepButtonIconPath(node["step_button_icon"].as<std::string>())
            .setPauseButtonIconPath(node["pause_button_icon"].as<std::string>())
            .setStepButtonIconPath(node["stop_button_icon"].as<std::string>());
        return true;
    }

    static Node encode(const LE::ResourcePaths& resources)
    {
        Node node;
        node["resources_dir"] = resources.resorcesDir();
        node["play_button_icon"] = resources.playButtonIconPath();
        node["simulation_button_icon"] = resources.simulationButtonIconPath();
        node["step_button_icon"] = resources.stepButtonIconPath();
        node["pause_button_icon"] = resources.pauseButtonIconPath();
        node["stop_button_icon"] = resources.stepButtonIconPath();
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
