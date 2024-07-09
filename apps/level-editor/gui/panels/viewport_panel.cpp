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
#include "genesis/window/events/event_dispatcher.h"
#include "genesis/window/events/mouse_events.h"

using namespace GE::GUI;

namespace LE {

ViewportPanel::ViewportPanel(LevelEditorContext* ctx)
    : WindowBase(NAME)
    , m_ctx{ctx}
{}

void ViewportPanel::onUpdate(GE::Timestamp ts)
{
    if (isPanelActive()) {
        m_ctx->cameraController()->onUpdate(ts);
    }
}

void ViewportPanel::onEvent(GE::Event* event)
{
    if (isPanelActive()) {
        m_ctx->cameraController()->onEvent(event);
    }

    GE::EventDispatcher dispatcher{event};
    dispatcher.dispatch<GE::MouseButtonReleasedEvent>(
        GE::toEventHandler(&ViewportPanel::onMouseButtonReleased, this));
}

void ViewportPanel::onRender()
{
    StyleVar padding{StyleVar::WINDOW_PADDING, {0.0f, 0.0f}};

    WidgetNode window{&m_window};

    auto& scene_fbo = m_ctx->sceneFbo();
    scene_fbo->renderer()->swapBuffers();
    const auto& texture = scene_fbo->colorTexture();

    window.call(&ViewportPanel::updateWindowParameters, this);
    window.call<Image>(texture.nativeID(), texture.size());
    window.call(&ViewportPanel::drawGizmos, this, m_ctx->selectedEntity());

    if (isPanelActive()) {
        m_mouse_position = m_window.mousePosition();
    }
}

void ViewportPanel::updateWindowParameters()
{
    if (m_viewport != m_window.availableRegion()) {
        m_viewport = m_window.availableRegion();
        m_viewport_changed_signal(m_viewport);
    }

    m_is_focused = m_window.isFocused();
    m_is_hovered = m_window.isHovered();
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
    auto parent_transform = GE::Scene::parentalTransforms(*entity);
    auto transform_matrix = parent_transform * tc.transform();
    bool is_ortho = camera->type() == GE::Scene::ProjectionCamera::ORTHOGRAPHIC;
    auto position = m_window.position() + m_window.contentRegionMin();

    Gizmos gizmos{position, m_viewport, is_ortho};
    gizmos.draw(view, projection, Gizmos::TRANSLATE, Gizmos::LOCAL, &transform_matrix);

    if (gizmos.isUsing()) {
        auto local_transfrom = GE::inverse(parent_transform) * transform_matrix;
        decompose(local_transfrom, &tc.translation, &tc.rotation, &tc.scale);
        tc.rotation = GE::radians(tc.rotation);
    }
}

bool ViewportPanel::onMouseButtonReleased(const GE::MouseButtonReleasedEvent& event)
{
    if (isPanelActive() && event.getMouseButton() == GE::MouseButton::LEFT) {
        *m_ctx->selectedEntity() = m_ctx->entityPicker()->getEntityByPosition(m_mouse_position);
    }

    return false;
}

} // namespace LE
