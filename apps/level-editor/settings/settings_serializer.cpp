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

#include "settings_serializer.h"
#include "yaml_convert.h"

#include "genesis/core/log.h"

#include <yaml-cpp/yaml.h>

#include <fstream>

namespace LE {

SettingsSerializer::SettingsSerializer(Settings *settings)
    : m_settings{settings}
{}

bool SettingsSerializer::serialize(const std::string &filepath)
{
    YAML::Node yaml_settings = serializeSettings();
    return writeSettings(yaml_settings, filepath);
}

YAML::Node SettingsSerializer::serializeSettings()
{
    YAML::Node node;
    node["current_project"] = m_settings->currentProject()->name();
    node["projects"] = m_settings->projectPaths();
    node["resources"] = m_settings->resourePaths();

    return node;
}

bool SettingsSerializer::writeSettings(const YAML::Node &node, const std::string &filepath)
{
    std::ofstream fout{filepath};

    if (!fout) {
        GE_CORE_ERR("Failed open the project settings file '{}'", filepath);
        return false;
    }

    fout << node;
    return true;
}

} // namespace LE
