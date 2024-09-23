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

#include "toolbar.h"
#include "level_editor_context.h"

#include "genesis/assets/registry.h"
#include "genesis/assets/texture_resource.h"
#include "genesis/graphics/texture.h"
#include "genesis/gui.h"
#include "genesis/scene/executor/executor_factory.h"
#include "genesis/scene/executor/iexecutor.h"
#include "genesis/scene/executor/runtime2d_executor.h"

using namespace GE;
using namespace GE::GUI;

namespace LE {
namespace {

constexpr Window::Flags WINDOW_FLAGS{Window::NO_DOCKING | Window::NO_TITLE_BAR | Window::NO_RESIZE |
                                     Window::NO_MOVE | Window::NO_COLLAPSE | Window::NO_SCROLLBAR |
                                     Window::NO_SCROLL_WITH_MOUSE};

Shared<Texture> loadIcon(const Assets::Registry& assets, const std::string& icon_name)
{
    Assets::ResourceID id{"level_editor", Assets::Group::TEXTURES, icon_name};
    return assets.get<Assets::TextureResource>(id)->texture();
}

} // namespace

SceneExecutorRenderer::SceneExecutorRenderer(LevelEditorContext* ctx)
    : m_ctx{ctx}
    , m_render_state{getRenderState(*m_ctx->sceneExecutor())}
    , m_factory{m_ctx->scene(), m_ctx->assets(), m_ctx->world().get()}
{
    loadIcons(*m_ctx->assets());
}

void SceneExecutorRenderer::render(WidgetNode* node)
{
    (this->*m_render_state)(node);
}

void SceneExecutorRenderer::loadIcons(const Assets::Registry& assets)
{
    m_play_button_icon = loadIcon(assets, "play_button");
    m_simulate_button_icon = loadIcon(assets, "simulate_button");
    m_step_button_icon = loadIcon(assets, "step_button");
    m_pause_button_icon = loadIcon(assets, "pause_button");
    m_stop_button_icon = loadIcon(assets, "stop_button");
}

SceneExecutorRenderer::RenderState
SceneExecutorRenderer::getRenderState(const Scene::IExecutor& executor)
{
    if (executor.type() == Scene::DummyExecutor::TYPE) {
        return &SceneExecutorRenderer::renderStoppedScene;
    }

    if (executor.type() == Scene::Runtime2DExecutor::TYPE) {
        return executor.isPaused() ? &SceneExecutorRenderer::renderPausedScene
                                   : &SceneExecutorRenderer::renderActiveScene;
    }

    return &SceneExecutorRenderer::renderStoppedScene;
}

void SceneExecutorRenderer::renderStoppedScene(WidgetNode* node)
{
    renderStartButton(node);
}

void SceneExecutorRenderer::renderActiveScene(WidgetNode* node)
{
    renderPauseButton(node);
    node->call<SameLine>();
    renderStopButton(node);
}

void SceneExecutorRenderer::renderPausedScene(WidgetNode* node)
{
    renderResumeButton(node);
    node->call<SameLine>();
    renderStopButton(node);
}

void SceneExecutorRenderer::renderStartButton(WidgetNode* node)
{
    if (node->call<ImageButton>("play_button", m_play_button_icon->nativeID(),
                                m_play_button_icon->size())) {
        m_factory.saveScene();
        m_ctx->sceneExecutor() = m_factory.create(Scene::Runtime2DExecutor::TYPE);
        m_render_state = &SceneExecutorRenderer::renderActiveScene;
    }
}

void SceneExecutorRenderer::renderPauseButton(GE::GUI::WidgetNode* node)
{
    if (node->call<ImageButton>("pause_button", m_pause_button_icon->nativeID(),
                                m_pause_button_icon->size())) {
        m_ctx->sceneExecutor()->pause();
        m_render_state = &SceneExecutorRenderer::renderPausedScene;
    }
}

void SceneExecutorRenderer::renderResumeButton(GE::GUI::WidgetNode* node)
{
    if (node->call<ImageButton>("play_button", m_play_button_icon->nativeID(),
                                m_play_button_icon->size())) {
        m_ctx->sceneExecutor()->resume();
        m_render_state = &SceneExecutorRenderer::renderActiveScene;
    }
}

void SceneExecutorRenderer::renderStopButton(GE::GUI::WidgetNode* node)
{
    if (node->call<ImageButton>("stop_button", m_stop_button_icon->nativeID(),
                                m_stop_button_icon->size())) {
        m_ctx->sceneExecutor() = m_factory.create(Scene::DummyExecutor::TYPE);
        m_factory.restoreScene();
        m_render_state = &SceneExecutorRenderer::renderStoppedScene;
    }
}

Toolbar::Toolbar(LevelEditorContext* ctx)
    : WindowBase{NAME, WINDOW_FLAGS, true}
    , m_ctx{ctx}
    , m_scene_executor_renderer{m_ctx}
{}

void Toolbar::onRender()
{
    auto colors = getStyleColors();
    auto button_hovered = colors[StyleColor::BUTTON_HOVERED];
    auto button_active = colors[StyleColor::BUTTON_ACTIVE];

    button_hovered.w = 0.5f;
    button_active.w = 0.5f;

    StyleVar window_padding_style{StyleVar::WINDOW_PADDING, GE::Vec2{0.0f, 2.0f}};
    StyleVar item_inner_spacing_style{StyleVar::ITEM_INNER_SPACING, GE::Vec2{0.0f, 0.0f}};
    StyleColor button_color{StyleColor::BUTTON, GE::Vec4{0.0f, 0.0f, 0.0f, 0.0f}};
    StyleColor button_hovered_style{StyleColor::BUTTON_HOVERED, button_hovered};
    StyleColor button_active_style{StyleColor::BUTTON_ACTIVE, button_active};

    WidgetNode widget_node{&m_window};
    m_scene_executor_renderer.render(&widget_node);
}

} // namespace LE
