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

#include "editor_camera_panel.h"

#include "genesis/gui/widgets.h"
#include "genesis/math/trigonometric.h"
#include "genesis/scene/camera/view_projection_camera.h"

using namespace GE::GUI;

namespace LE {

EditorCameraPanel::EditorCameraPanel(GE::Scene::ViewProjectionCamera* camera)
    : WindowBase{NAME}
    , m_camera{camera}
{}

void EditorCameraPanel::onRender()
{
    WidgetNode node{&m_window};
    drawView(&node);
    drawProjectionCombo(&node);
    drawProjectionOptions(&node);
    drawReadOnlyOptions(&node);
}

void EditorCameraPanel::drawView(WidgetNode* node)
{
    if (auto position = m_camera->position();
        node->call<::ValueEditor>("Position", &position, 0.1f, -100.0f, 100.0f)) {
        m_camera->setPosition(position);
    }

    if (auto rotation = GE::degrees(m_camera->rotation());
        node->call<::ValueEditor>("Rotation", &rotation, 0.1f, -360.0f, 360.0f)) {
        m_camera->setRotation(GE::radians(rotation));
    }
}

void EditorCameraPanel::drawProjectionCombo(WidgetNode* node)
{
    static const std::vector<std::string> PROJECTIONS = {
        GE::toString(GE::Scene::ViewProjectionCamera::ORTHOGRAPHIC),
        GE::toString(GE::Scene::ViewProjectionCamera::PERSPECTIVE),
    };

    auto     current_projection = GE::toString(m_camera->type());
    ComboBox combo{"Projection type", PROJECTIONS, current_projection};
    node->subNode(&combo);

    if (combo.selectedItem() != current_projection) {
        m_camera->setType(GE::Scene::toProjectionType(combo.selectedItem()));
    }
}

void EditorCameraPanel::drawPerspectiveProjection(WidgetNode* node)
{
    auto [fov, near, far] = m_camera->perspectiveOptions();
    node->call<::ValueEditor>("Field of view", &fov, 0.1f, 0.0f, 180.0f);
    node->call<::ValueEditor>("Near", &near, 0.1f, 0.0f, 100.0f);
    node->call<::ValueEditor>("Far", &far, 0.1f, 0.0f, 100.0f);

    m_camera->setPerspectiveOptions({.fov = fov, .near = near, .far = far});
}

void EditorCameraPanel::drawOrthoProjection(WidgetNode* node)
{
    auto [size, near, far] = m_camera->orthographicOptions();
    node->call<::ValueEditor>("Size", &size, 0.1f, 0.0f, 10.0f);
    node->call<::ValueEditor>("Near", &near, 0.1f, 0.0f, 100.f);
    node->call<::ValueEditor>("Far", &far, 0.1f, 0.0f, 100.0f);

    m_camera->setOrthoOptions({.size = size, .near = near, .far = far});
}

void EditorCameraPanel::drawProjectionOptions(WidgetNode* node)
{
    switch (m_camera->type()) {
        case GE::Scene::ViewProjectionCamera::PERSPECTIVE: drawPerspectiveProjection(node); break;
        case GE::Scene::ViewProjectionCamera::ORTHOGRAPHIC: drawOrthoProjection(node); break;
        default: break;
    }
}

void EditorCameraPanel::drawReadOnlyOptions(WidgetNode* node)
{
    node->call<::Text>("Direction: %s", GE::toString(m_camera->direction()).c_str());
}

} // namespace LE
