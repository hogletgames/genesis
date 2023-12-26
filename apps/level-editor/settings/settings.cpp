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

#include "settings.h"
#include "project_settings.h"

namespace LE {

void Settings::setProjectPath(const std::string& name, const std::string& path)
{
    m_projects.emplace(name, path);
}

bool Settings::setCurrentProject(const std::string& project_name)
{
    auto it = m_projects.find(project_name);

    if (it == m_projects.end()) {
        GE_ERR("Failed to find the path for a project '{}'", project_name);
        return false;
    }

    auto project = ProjectSettings::fromFile(it->second);

    if (!project.has_value()) {
        GE_ERR("Failed to load the project settings '{}' from the file '{}'", project_name,
               it->second);
        return false;
    }

    m_current_project = project.value();
    return true;
}

void Settings::closeCurrentProject()
{
    m_current_project = ProjectSettings();
}

ProjectSettings& Settings::createProject(const std::string& name)
{
    m_current_project = ProjectSettings();
    m_current_project.setName(name);
    return m_current_project;
}

} // namespace LE
