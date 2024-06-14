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

#include "scene_panel.h"
#include "level_editor_context.h"

#include "genesis/gui/widgets.h"
#include "genesis/scene.h"
#include "genesis/window/input.h"

using namespace GE::GUI;
using namespace GE::Scene;

namespace LE {

ScenePanel::ScenePanel(LevelEditorContext* ctx)
    : WindowBase(NAME)
    , m_ctx{ctx}
{}

void ScenePanel::onRender()
{
    WidgetNode node{&m_window};
    drawScene(&node);
    drawContextMenu(&node);
}

void ScenePanel::drawScene(WidgetNode* node)
{
    m_ctx->scene()->forEachEntity([this, node](const auto& entity) { drawEntity(node, entity); });

    if (GE::Input::isButtonPressed(GE::MouseButton::LEFT) && m_window.isHovered()) {
        m_ctx->resetSelectedEntity();
    }
}

void ScenePanel::drawEntity(WidgetNode* node, const Entity& entity)
{
    auto flags = TreeNode::OPEN_ON_ARROW | TreeNode::SPAN_AVAIL_WIDTH;
    bool remove_entity{false};

    if (*m_ctx->selectedEntity() == entity) {
        flags |= TreeNode::SELECTED;
    }

    std::string_view tag = entity.get<TagComponent>().tag;

    {
        auto entity_tree_node = node->makeSubNode<TreeNode>(tag, flags);
        auto popup_context = WidgetNode::create<PopupContextItem>();

        if (popup_context.call<MenuItem>(GE_FMTSTR("Remove '{}'", tag))) {
            remove_entity = true;
        }

        if (isItemClicked()) {
            *m_ctx->selectedEntity() = entity;
        }
    }

    if (remove_entity) {
        removeEntity(entity);
    }
}

void ScenePanel::drawContextMenu(WidgetNode* node)
{
    EntityFactory entity_factory{m_ctx->scene(), m_ctx->assets()};
    auto flags = PopupFlag::MOUSE_BUTTON_RIGHT | PopupFlag::NO_OPEN_OVER_EXISTING_POPUP;

    auto context_menu_node = node->makeSubNode<PopupContextWindow>(std::string_view{}, flags);
    auto add_entity_menu = context_menu_node.makeSubNode<Menu>("Add entity");

    if (add_entity_menu.call<MenuItem>("Circle")) {
        entity_factory.createCircle("Circle");
    }
    if (add_entity_menu.call<MenuItem>("Square")) {
        entity_factory.createSquare("Square");
    }
    if (add_entity_menu.call<MenuItem>("Empty entity")) {
        entity_factory.createEmptyEntity("Entity");
    }
    if (add_entity_menu.call<MenuItem>("Camera")) {
        entity_factory.createCamera("Camera");
    }
}

void ScenePanel::removeEntity(const Entity& entity)
{
    if (entity == *m_ctx->selectedEntity()) {
        m_ctx->resetSelectedEntity();
    }

    m_ctx->scene()->destroyEntity(entity);
}

} // namespace LE
