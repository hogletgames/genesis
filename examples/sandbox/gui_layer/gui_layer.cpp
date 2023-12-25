/*
 * BSD 3-Clause License
 *
 * Copyright (c) 2021, Dmitry Shilnenkov
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

#include "gui_layer.h"
#include "drawable/cube.h"
#include "drawable/model.h"
#include "drawable/triangle.h"
#include "gui_layer_window.h"

#include "genesis/core.h"
#include "genesis/gui.h"
#include "genesis/math.h"
#include "genesis/scene.h"

namespace {

constexpr auto VIKING_ROOM_MODEL{"examples/sdl2-vulkan/models/viking_room.obj"};
constexpr auto VIKING_ROOM_TEXTURE{"examples/sandbox/assets/textures/viking_room.png"};

void drawView(GE::GUI::WidgetNodeGuard* node,
              const GE::Shared<GE::Scene::ViewProjectionCamera>& camera)
{
    if (auto position = camera->position();
        node->call<GE::GUI::ValueEditor>("Position", &position, 1.0f, -100.0f, 100.0f)) {
        camera->setPosition(position);
    }

    if (auto rotation = GE::degrees(camera->rotation());
        node->call<GE::GUI::ValueEditor>("Rotation", &rotation, 1.0f, -360.0f, 360.0f)) {
        camera->setRotation(GE::radians(rotation));
    }
}

void drawProjectionCombo(GE::GUI::WidgetNodeGuard* node,
                         const GE::Shared<GE::Scene::ViewProjectionCamera>& camera)
{
    static const std::vector<std::string> PROJECTIONS = {
        GE::toString(GE::Scene::ViewProjectionCamera::ORTHOGRAPHIC),
        GE::toString(GE::Scene::ViewProjectionCamera::PERSPECTIVE),
    };

    GE::GUI::ComboBox combo{"Projection type", PROJECTIONS, GE::toString(camera->type())};
    combo.itemChangedSignal()->connect([&camera](std::string_view projection) {
        camera->setType(GE::Scene::toProjectionType(projection));
    });
    node->subNode(&combo);
}

void drawPerspectiveProjection(GE::GUI::WidgetNodeGuard* node,
                               const GE::Shared<GE::Scene::ViewProjectionCamera>& camera)
{
    auto [fov, near, far] = camera->perspectiveOptions();
    node->call<GE::GUI::ValueEditor>("Field of view", &fov, 0.1f, 0.0f, 180.0f);
    node->call<GE::GUI::ValueEditor>("Near", &near, 0.1f, 0.0f, 100.0f);
    node->call<GE::GUI::ValueEditor>("Far", &far, 0.1f, 0.0f, 100.0f);

    camera->setPerspectiveOptions({fov, near, far});
}

void drawOrthoProjection(GE::GUI::WidgetNodeGuard* node,
                         const GE::Shared<GE::Scene::ViewProjectionCamera>& camera)
{
    auto [size, near, far] = camera->orthographicOptions();
    node->call<GE::GUI::ValueEditor>("Size", &size, 0.1f, 0.0f, 10.0f);
    node->call<GE::GUI::ValueEditor>("Near", &near, 0.1f, 0.0f, 100.f);
    node->call<GE::GUI::ValueEditor>("Far", &far, 0.1f, 0.0f, 100.0f);

    camera->setOrthoOptions({size, near, far});
}

void drawProjectionOptions(GE::GUI::WidgetNodeGuard* node,
                           const GE::Shared<GE::Scene::ViewProjectionCamera>& camera)
{
    switch (camera->type()) {
        case GE::Scene::ViewProjectionCamera::PERSPECTIVE:
            drawPerspectiveProjection(node, camera);
            break;
        case GE::Scene::ViewProjectionCamera::ORTHOGRAPHIC:
            drawOrthoProjection(node, camera);
            break;
    }
}

void drawReadOnlyOptions(GE::GUI::WidgetNodeGuard* node,
                         const GE::Shared<GE::Scene::ViewProjectionCamera>& camera)
{
    node->call<GE::GUI::Text>("Direction: %s", GE::toString(camera->direction()).c_str());
}

void drawCameraOptions(GE::GUI::WidgetNodeGuard* parent, GE::Examples::GuiLayerWindow* window)
{
    GE::GUI::TreeNode camera_tree{window->name(), GE::GUI::TreeNode::FRAMED};
    auto node = parent->subNode(&camera_tree);
    auto camera = window->camera();

    drawView(&node, camera);
    drawProjectionCombo(&node, camera);
    drawProjectionOptions(&node, camera);
    drawReadOnlyOptions(&node, camera);
}

} // namespace

namespace GE::Examples {

GUILayer::GUILayer() = default;

GUILayer::~GUILayer() = default;

void GUILayer::onAttached()
{
    m_gui_windows.push_back(GuiLayerWindow::create<Triangle>("Triangle"));
    m_gui_windows.push_back(GuiLayerWindow::create<Cube>("Cube"));
    m_gui_windows.push_back(
        GuiLayerWindow::create<Model>("Viking Room", VIKING_ROOM_MODEL, VIKING_ROOM_TEXTURE));
}

void GUILayer::onDetached()
{
    m_gui_windows.clear();
}

void GUILayer::onUpdate(Timestamp ts)
{
    for (auto& gui_window : m_gui_windows) {
        gui_window->onUpdate(ts);
    }
}

void GUILayer::onEvent(Event* event)
{
    BaseLayer::onEvent(event);

    for (auto& gui_window : m_gui_windows) {
        gui_window->onEvent(event);
    }
}

void GUILayer::onRender()
{
    Beginner<GUI::Renderer> gui_begin;
    drawCheckboxWindow();
    drawGuiWindows();
}

void GUILayer::drawCheckboxWindow()
{
    GUI::Window window{"Windows Checkbox"};
    GUI::WidgetNodeGuard node{&window};

    for (auto& gui_window : m_gui_windows) {
        if (node.call<GUI::Checkbox>(gui_window->name(), gui_window->isOpenFlag());
            *gui_window->isOpenFlag()) {
            drawCameraOptions(&node, gui_window.get());
        }
    }
}

void GUILayer::drawGuiWindows()
{
    for (auto& gui_window : m_gui_windows) {
        gui_window->draw();
    }
}

} // namespace GE::Examples
