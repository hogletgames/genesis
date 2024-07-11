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

#include <genesis/core/export.h>

#include <map>
#include <string>

namespace LE {

class GE_API ResourcePaths
{
public:
    ResourcePaths& setResourcesDir(const std::string& path);
    ResourcePaths& setPlayButtonIconPath(const std::string& path);
    ResourcePaths& setSimulationButtonIconPath(const std::string& path);
    ResourcePaths& setStepButtonIconPath(const std::string& path);
    ResourcePaths& setPauseButtonIconPath(const std::string& path);
    ResourcePaths& setStopButtonIconPath(const std::string& path);

    const std::string& resorcesDir() const;
    std::string playButtonIconPath() const;
    std::string simulationButtonIconPath() const;
    std::string stepButtonIconPath() const;
    std::string pauseButtonIconPath() const;
    std::string stopButtonIconPath() const;

private:
    std::string m_resources_dir;
    std::string m_play_button_icon_path;
    std::string m_simulation_button_icon_path;
    std::string m_step_button_icon_path;
    std::string m_pause_button_icon_path;
    std::string m_stop_button_icon_path;
};

class GE_API Settings
{
public:
    using ProjectPaths = std::map<std::string, std::string>;

    ProjectSettings* currentProject() { return &m_current_project; }
    const ProjectPaths& projectPaths() const { return m_projects; }
    const ResourcePaths& resourePaths() const { return m_resources; }

    void setProjectPath(const std::string& name, const std::string& path);
    void setProjectPaths(ProjectPaths projects) { m_projects = std::move(projects); }
    void setResourcePaths(ResourcePaths resources) { m_resources = std::move(resources); }

    bool setCurrentProject(const std::string& project_name);
    void closeCurrentProject();
    ProjectSettings& createProject(const std::string& name);

private:
    ProjectSettings m_current_project;
    ProjectPaths m_projects;
    ResourcePaths m_resources;
};

} // namespace LE
