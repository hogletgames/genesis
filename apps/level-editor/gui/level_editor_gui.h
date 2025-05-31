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

#include "gui/main_menu/menu_bar.h"
#include "gui/panels/viewport_panel.h"
#include "gui/toolbar/toolbar.h"

#include <genesis/gui/window/modal_windows.h>
#include <genesis/gui/window/window_map.h>

namespace LE {

class LevelEditorContext;

class GE_API LevelEditorGUI
{
public:
    using ViewportChangedSignal = ViewportPanel::ViewportChangedSignal;

    explicit LevelEditorGUI(LevelEditorContext* ctx);

    void onUpdate(GE::Timestamp ts);
    void onEvent(GE::Event* event);
    void onRender();

    LevelEditorContext* context() const { return m_ctx; }
    MenuBar* menuBar() { return &m_menu_bar; }
    GE::GUI::WindowMap* panels() { return &m_panels; }
    GE::GUI::WindowMap* windows() { return &m_windows; }

    ViewportChangedSignal* viewportChangedSignal() { return &m_viewport_changed_signal; }
    LoadSignal* loadAssetSignal() { return m_menu_bar.loadAssetSignal(); }
    SaveSignal* saveAssetSignal() { return m_menu_bar.saveAssetSignal(); }
    LoadSignal* loadSceneSignal() { return m_menu_bar.loadSceneSignal(); }
    SaveSignal* saveSceneSignal() { return m_menu_bar.saveSceneSignal(); }
    LoadSignal* loadProjectSignal() { return m_menu_bar.loadProjectSignal(); }
    LoadFileSignal* loadRecentProjectSignal() { return m_menu_bar.loadRecentProjectSignal(); }
    SaveFileSignal* saveProjectSignal() { return m_menu_bar.saveProjectSignal(); }
    SaveSignal* saveProjectAsSignal() { return m_menu_bar.saveProjectAsSignal(); }

private:
    void createPanels();
    void createWindows();

    LevelEditorContext*   m_ctx{nullptr};
    MenuBar               m_menu_bar;
    Toolbar               m_toolbar;
    GE::GUI::WindowMap    m_panels;
    GE::GUI::WindowMap    m_windows;
    GE::GUI::ModalWindows m_modals;

    ViewportChangedSignal m_viewport_changed_signal;
};

} // namespace LE
