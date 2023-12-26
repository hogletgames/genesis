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

#include "menu_bar.h"
#include "gui/level_editor_gui.h"
#include "gui/main_menu/view_menu.h"
#include "level_editor_context.h"

#include "genesis/gui/widgets.h"

using namespace GE::GUI;

namespace LE {

MenuBar::MenuBar(LevelEditorGUI *gui)
    : m_project{GE::makeShared<ProjectMenu>(gui->context()->settings())}
    , m_assets{GE::makeShared<AssetsMenu>()}
    , m_scene{GE::makeShared<SceneMenu>()}
    , m_view{GE::makeShared<ViewMenu>(gui->panels())}
{
    m_menus.appendMenu(m_project);
    m_menus.appendMenu(m_assets);
    m_menus.appendMenu(m_scene);
    m_menus.appendMenu(m_view);
}

void MenuBar::onUpdate(GE::Timestamp ts)
{
    m_menus.onUpdate(ts);
}

void MenuBar::onEvent(GE::Event *event)
{
    m_menus.onEvent(event);
}

void MenuBar::onRender()
{
    MainMenuBar bar;
    WidgetNodeGuard bar_node{&bar};
    m_menus.onRender(&bar_node);
}

} // namespace LE
