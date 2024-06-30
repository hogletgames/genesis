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

#include "level_editor_context.h"

#include <genesis/gui/window/window_base.h>

namespace GE::GUI {
class WidgetNode;
} // namespace GE::GUI

namespace GE::Scene {
class ViewProjectionCamera;
} // namespace GE::Scene

namespace LE {

class GE_API EditorCameraPanel: public GE::GUI::WindowBase
{
public:
    explicit EditorCameraPanel(LevelEditorContext* ctx);

    void onRender() override;

    static constexpr auto NAME{"Editor camera"};

private:
    void drawView(GE::GUI::WidgetNode* node);
    void drawProjectionCombo(GE::GUI::WidgetNode* node);
    void drawPerspectiveProjection(GE::GUI::WidgetNode* node);
    void drawOrthoProjection(GE::GUI::WidgetNode* node);
    void drawProjectionOptions(GE::GUI::WidgetNode* node);
    void drawReadOnlyOptions(GE::GUI::WidgetNode* node);

    void drawSceneExecutor(GE::GUI::WidgetNode* node);
    void saveScene();
    void loadSerializedScene();

    GE::Scene::ViewProjectionCamera* camera() { return m_ctx->cameraController()->camera().get(); }

    LevelEditorContext* m_ctx{nullptr};
    std::string m_saved_scene_path;
};

} // namespace LE
