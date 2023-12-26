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

#include "level_editor_gui.h"
#include "level_editor_context.h"
#include "modals.h"
#include "panels.h"
#include "windows.h"

#include "genesis/core/defer.h"
#include "genesis/gui/renderer.h"

namespace LE {

LevelEditorGUI::LevelEditorGUI(LevelEditorContext* ctx)
    : m_ctx{ctx}
    , m_menu_bar{this}
{
    createPanels();
    createWindows();
}

void LevelEditorGUI::onUpdate(GE::Timestamp ts)
{
    m_menu_bar.onUpdate(ts);
    m_panels.onUpdate(ts);
    m_windows.onUpdate(ts);
    m_modals.onUpdate(ts);
}

void LevelEditorGUI::onEvent(GE::Event* event)
{
    m_menu_bar.onEvent(event);
    m_panels.onEvent(event);
    m_windows.onEvent(event);
    m_modals.onEvent(event);
}

void LevelEditorGUI::onRender()
{
    GE::Beginner<GE::GUI::Renderer> gui_begin;
    m_menu_bar.onRender();
    m_panels.onRender();
    m_windows.onRender();
    m_modals.onRender();
}

void LevelEditorGUI::createPanels()
{
    auto editor_camera =
        GE::makeScoped<EditorCameraPanel>(m_ctx->cameraController()->camera().get());
    editor_camera->open();

    auto viewport = GE::makeScoped<ViewportPanel>(m_ctx);
    viewport->viewportSizeSignal()->connect(m_viewport_size_signal);
    viewport->open();

    auto assets = GE::makeScoped<AssetsPanel>(m_ctx->assets());
    assets->addMeshResourceSignal()->connect(
        [this] { m_windows.openWindow(AddMeshResourceWindow::NAME); });
    assets->addPipelineResourceSignal()->connect(
        [this] { m_windows.openWindow(AddPipelineResourceWindow::NAME); });
    assets->addTextureResourceSignal()->connect(
        [this] { m_windows.openWindow(AddTextureResourceWindow::NAME); });
    assets->open();

    auto scene = GE::makeScoped<ScenePanel>(m_ctx);
    scene->open();

    auto components = GE::makeScoped<ComponentsPanel>(m_ctx);
    components->open();

    auto log = GE::makeScoped<LogPanel>();
    log->open();

    m_panels.insertWindows(editor_camera, viewport, assets, scene, components, log);
}

void LevelEditorGUI::createWindows()
{
    auto add_mesh_resource = GE::makeScoped<AddMeshResourceWindow>(m_ctx);
    add_mesh_resource->errorSignal()->connect([this](std::string_view error) {
        m_modals.open<NotificationModal>("Error", error.data());
    });

    auto add_texture_resource = GE::makeScoped<AddTextureResourceWindow>(m_ctx);
    add_texture_resource->errorSignal()->connect([this](std::string_view error) {
        m_modals.open<NotificationModal>("Error", error.data());
    });

    auto add_pipeline_resource = GE::makeScoped<AddPipelineResourceWindow>(m_ctx);
    add_pipeline_resource->errorSignal()->connect([this](std::string_view error) {
        m_modals.open<NotificationModal>("Error", error.data());
    });

    m_windows.insertWindows(add_mesh_resource, add_texture_resource, add_pipeline_resource);
}

} // namespace LE
