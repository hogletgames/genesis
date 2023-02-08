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

namespace {

constexpr auto VIKING_ROOM_MODEL{"examples/sdl2-vulkan/models/viking_room.obj"};
constexpr auto VIKING_ROOM_TEXTURE{"examples/sandbox/assets/textures/viking_room.png"};

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

void GUILayer::onUpdate([[maybe_unused]] Timestamp ts) {}

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
            drawTransformTreeNode(&node, gui_window.get());
        }
    }
}

void GUILayer::drawTransformTreeNode(GUI::WidgetNodeGuard* node_guard, GuiLayerWindow* gui_window)
{
    GUI::TreeNode tree_node{gui_window->name(), GUI::TreeNode::FRAMED};
    auto node = node_guard->subNode(&tree_node);
    auto rotation_degrees = glm::degrees(*gui_window->objectRotation());

    node.call<GUI::Text>("Transform:");
    node.call<GUI::ValueEditor>("Translation", gui_window->objectTranslation(), 0.05f, -10.0f,
                                10.0f);
    if (node.call<GUI::ValueEditor>("Rotation", &rotation_degrees, 1.0f, -180.0f, 180.0f)) {
        *gui_window->objectRotation() = glm::radians(rotation_degrees);
    }
    node.call<GUI::ValueEditor>("Scale", gui_window->objectScale(), 0.01f, 0.0f, 10.0f);
}

void GUILayer::drawGuiWindows()
{
    for (auto& gui_window : m_gui_windows) {
        gui_window->draw();
    }
}

} // namespace GE::Examples
