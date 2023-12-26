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

#include "gui/main_menu/assets_menu.h"
#include "gui/main_menu/menu_signals.h"
#include "gui/main_menu/project_menu.h"
#include "gui/main_menu/scene_menu.h"
#include "gui/main_menu/view_menu.h"

#include <genesis/gui/window/menu_list.h>

namespace LE {

class LevelEditorGUI;

class GE_API MenuBar
{
public:
    explicit MenuBar(LevelEditorGUI* gui);

    void onUpdate(GE::Timestamp ts);
    void onEvent(GE::Event* event);
    void onRender();

    LoadSignal* loadAssetSignal() { return m_assets->loadAssetSignal(); }
    SaveSignal* saveAssetSignal() { return m_assets->saveAssetSignal(); }

    LoadSignal* loadSceneSignal() { return m_scene->loadSceneSignal(); }
    SaveSignal* saveSceneSignal() { return m_scene->saveSceneSignal(); }

    LoadSignal* loadProjectSignal() { return m_project->loadSignal(); }
    LoadFileSignal* loadRecentProjectSignal() { return m_project->loadRecentSignal(); }
    SaveFileSignal* saveProjectSignal() { return m_project->saveSignal(); }
    SaveSignal* saveProjectAsSignal() { return m_project->saveAsSignal(); }

private:
    GE::GUI::MenuList m_menus;

    GE::Shared<ProjectMenu> m_project;
    GE::Shared<AssetsMenu> m_assets;
    GE::Shared<SceneMenu> m_scene;
    GE::Shared<ViewMenu> m_view;
};

} // namespace LE
