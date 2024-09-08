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

#include "settings_deserializer.h"
#include "settings.h"
#include "yaml_convert.h"

#include <fstream>

#include <yaml-cpp/yaml.h>

namespace LE {
namespace {

bool validate(const YAML::Node &node)
{
    if (auto project = node["projects"]; !project || !project.IsMap()) {
        GE_ERR("Failed to load a settings config file: 'projects' node is not a map");
        return false;
    }

    if (!node["current_project"]) {
        GE_ERR("Failed to load a settings config file: 'current_project' node is null");
        return false;
    }

    return true;
}

} // namespace

SettingsDeserializer::SettingsDeserializer(Settings *settings)
    : m_settings{settings}
{}

bool SettingsDeserializer::deserialize(const std::string &filepath)
{
    YAML::Node node;

    try {
        node = YAML::LoadFile(filepath);
    } catch (const std::exception &e) {
        GE_CORE_ERR("Failed to load a settings config file from the '{}': {}", filepath, e.what());
        return false;
    }

    if (!validate(node)) {
        return false;
    }

    try {
        m_settings->setAppSettings(node["app_settings"].as<AppSettings>());
    } catch (const std::exception &e) {
        GE_WARN("Failed to load app settings: {}", e.what());
    }

    try {
        m_settings->setProjectPaths(node["projects"].as<Settings::ProjectPaths>());
    } catch (const std::exception &e) {
        GE_WARN("Failed to load projects settings: {}", e.what());
    }

    try {
        if (auto current_project = node["current_project"].as<std::string>();
            !current_project.empty()) {
            return m_settings->setCurrentProject(current_project);
        }
    } catch (const std::exception &e) {
        GE_WARN("Failed to current project settings: {}", e.what());
    }

    return true;
}

} // namespace LE
