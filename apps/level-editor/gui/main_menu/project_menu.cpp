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

#include "project_menu.h"
#include "settings/settings.h"

#include "genesis/gui/widgets.h"

using namespace GE::GUI;

namespace LE {

ProjectMenu::ProjectMenu(Settings* settings)
    : m_settings{settings}
{}

void ProjectMenu::onRender(WidgetNodeGuard* bar_node)
{
    std::string_view current_project_path = m_settings->currentProject()->projectPath();

    Menu menu{"Project"};
    auto project_node = bar_node->subNode(&menu);
    if (project_node.call<MenuItem>("Load")) {
        m_load_signal();
    }
    if (project_node.call<MenuItem>("Save", std::string_view{}, false,
                                    !current_project_path.empty())) {
        m_save_signal(current_project_path);
    }
    if (project_node.call<MenuItem>("Save As...")) {
        m_save_as_signal();
    }

    Menu recent_projects{"Recent projects", !m_settings->projectPaths().empty()};
    auto recent_projects_node = project_node.subNode(&recent_projects);
    for (const auto& [name, path] : m_settings->projectPaths()) {
        if (recent_projects_node.call<MenuItem>(name)) {
            m_load_recent_signal(path);
        }
    }
}

} // namespace LE
