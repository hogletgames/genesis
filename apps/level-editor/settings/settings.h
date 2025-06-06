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

#include <genesis/core/export.h>
#include <genesis/core/memory.h>

#include <map>
#include <string>
#include <vector>

namespace LE {

class GE_API Settings
{
public:
    using ProjectPaths = std::map<std::string, std::string>;

    void setAppSettings(const AppSettings& settings) { m_app_settings = settings; }
    const AppSettings& appSettings() const { return m_app_settings; }

    ProjectSettings* currentProject() { return &m_current_project; }
    const ProjectPaths& projectPaths() const { return m_projects; }

    void setProjectPath(const std::string& name, const std::string& path);
    void setProjectPaths(ProjectPaths projects) { m_projects = std::move(projects); }

    bool setCurrentProject(const std::string& project_name);
    void closeCurrentProject();
    ProjectSettings& createProject(const std::string& name);

private:
    AppSettings     m_app_settings;
    ProjectSettings m_current_project;
    ProjectPaths    m_projects;
};

} // namespace LE
