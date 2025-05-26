/*
 * BSD 3-Clause License
 *
 * Copyright (c) 2024, Dmitry Shilnenkov
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

#include <genesis/core/export.h>
#include <genesis/core/memory.h>
#include <genesis/gui/window/window_base.h>
#include <genesis/scene/executor/executor_factory.h>

namespace GE {
class Texture;
} // namespace GE

namespace GE::Assets {
class Registry;
} // namespace GE::Assets

namespace GE::GUI {
class WidgetNode;
} // namespace GE::GUI

namespace GE::Scene {
class IExecutor;
} // namespace GE::Scene

namespace LE {

class LevelEditorContext;

class GE_API SceneExecutorRenderer
{
public:
    explicit SceneExecutorRenderer(LevelEditorContext* ctx);

    void render(GE::GUI::WidgetNode*);

private:
    using RenderState = void (SceneExecutorRenderer::*)(GE::GUI::WidgetNode*);

    void loadIcons(const GE::Assets::Registry& assets);
    RenderState getRenderState(const GE::Scene::IExecutor& executor);

    void renderStoppedScene(GE::GUI::WidgetNode* node);
    void renderActiveScene(GE::GUI::WidgetNode* node);
    void renderPausedScene(GE::GUI::WidgetNode* node);

    void renderStartButton(GE::GUI::WidgetNode* node);
    void renderPauseButton(GE::GUI::WidgetNode* node);
    void renderResumeButton(GE::GUI::WidgetNode* node);
    void renderStopButton(GE::GUI::WidgetNode* node);

    LevelEditorContext*        m_ctx{nullptr};
    RenderState                m_render_state{nullptr};
    GE::Scene::ExecutorFactory m_factory;

    GE::Shared<GE::Texture> m_play_button_icon;
    GE::Shared<GE::Texture> m_simulate_button_icon;
    GE::Shared<GE::Texture> m_step_button_icon;
    GE::Shared<GE::Texture> m_pause_button_icon;
    GE::Shared<GE::Texture> m_stop_button_icon;
};

class GE_API Toolbar: public GE::GUI::WindowBase
{
public:
    explicit Toolbar(LevelEditorContext* ctx);

    void onRender();

    static constexpr std::string_view NAME = "Toolbar";

private:
    LevelEditorContext* m_ctx;

    SceneExecutorRenderer m_scene_executor_renderer;
};

} // namespace LE
