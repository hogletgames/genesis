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

#include "viewport_panel.h"
#include "level_editor_context.h"

#include "genesis/graphics.h"
#include "genesis/gui/gizmos.h"
#include "genesis/gui/widgets.h"
#include "genesis/math.h"
#include "genesis/scene.h"

using namespace GE::GUI;

namespace LE {

ViewportPanel::ViewportPanel(LevelEditorContext* ctx)
    : WindowBase(NAME)
    , m_ctx{ctx}
{
    m_window.isFocusedSignal()->connect([this](bool is_focused) { m_is_focused = is_focused; });
    m_window.isHoveredSignal()->connect([this](bool is_hovered) { m_is_hovered = is_hovered; });
    viewportSizeSignal()->connect([this](const auto& viewport) { m_viewport = viewport; });
}

void ViewportPanel::onUpdate(GE::Timestamp ts)
{
    if (m_is_focused || m_is_hovered) {
        m_ctx->cameraController()->onUpdate(ts);
    }
}

void ViewportPanel::onEvent(GE::Event* event)
{
    if (m_is_focused || m_is_hovered) {
        m_ctx->cameraController()->onEvent(event);
    }
}

void ViewportPanel::onRender()
{
    StyleVar padding{StyleVar::WINDOW_PADDING, {0.0f, 0.0f}};

    WidgetNodeGuard node{&m_window};

    auto& scene_fbo = m_ctx->sceneFbo();
    scene_fbo->renderer()->swapBuffers();
    const auto& texture = scene_fbo->colorTexture();

    node.call<Image>(texture.nativeID(), texture.size());
    node.call(&ViewportPanel::drawGizmos, this, m_ctx->selectedEntity());
}

void ViewportPanel::drawGizmos(GE::Scene::Entity* entity)
{
    if (entity->isNull()) {
        return;
    }

    const auto& camera = m_ctx->cameraController()->camera();
    const auto& view = camera->view();
    const auto& projection = camera->projection();

    auto& tc = entity->get<GE::Scene::TransformComponent>();
    auto transform_matrix = tc.transform();
    bool is_ortho = camera->type() == GE::Scene::ProjectionCamera::ORTHOGRAPHIC;

    Gizmos gizmos{m_window.position(), m_viewport, is_ortho};
    gizmos.draw(view, projection, Gizmos::TRANSLATE, Gizmos::LOCAL, &transform_matrix);

    if (gizmos.isUsing()) {
        decompose(transform_matrix, &tc.translation, &tc.rotation, &tc.scale);
        tc.rotation = GE::radians(tc.rotation);
    }
}

} // namespace LE
