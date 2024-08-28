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

void DeferredScenePanelCommands::selectEntity(const Entity& entity)
{
    enqueue([this, entity] { *m_ctx->selectedEntity() = entity; });
}

void DeferredScenePanelCommands::removeEntity(const Entity& entity)
{
    enqueue([this, entity] {
        if (entity == *m_ctx->selectedEntity()) {
            m_ctx->resetSelectedEntity();
        }

        EntityNode{entity}.destoryEntityWithChildren(m_ctx->scene());
    });
}

void DeferredScenePanelCommands::appendEntity(const Entity& dst, const Entity& src)
{
    enqueue([dst, src] {
        if (EntityNode src_node{src}; !src_node.hasChild(dst)) {
            EntityNode{dst}.insert(src);
        }
    });
}

void DeferredScenePanelCommands::appendToChildren(const Entity& dst, const Entity& src)
{
    enqueue([dst, src] {
        if (EntityNode src_node{src}; !src_node.hasChild(dst)) {
            EntityNode{dst}.appendChild(src);
        }
    });
}

ScenePanel::ScenePanel(LevelEditorContext* ctx)
    : WindowBase(NAME)
    , m_ctx{ctx}
    , m_commands{m_ctx}
{}

void ScenePanel::onRender()
{
    WidgetNode node{&m_window};
    drawScene(&node);
    drawContextMenu(&node);
}

void ScenePanel::drawScene(WidgetNode* node)
{
    if (auto head_entity = m_ctx->scene()->headEntity(); !head_entity.isNull()) {
        drawEntities(node, EntityNode{head_entity});
    }

    if (GE::Input::isButtonPressed(GE::MouseButton::LEFT) && m_window.isHovered()) {
        m_ctx->resetSelectedEntity();
    }

    m_commands.submit();
    m_is_select_entity_handled = false;
}

// NOLINTNEXTLINE(misc-no-recursion)
void ScenePanel::drawEntities(WidgetNode* node, const EntityNode& entity)
{
    auto current_entity = entity;

    while (!current_entity.isNull()) {
        drawEntity(node, current_entity.entity());
        current_entity = current_entity.nextNode();
    }
}

// NOLINTNEXTLINE(misc-no-recursion)
void ScenePanel::drawEntity(WidgetNode* node, const Entity& entity)
{
    TreeNode::Flags flags =
        TreeNode::OPEN_ON_ARROW | TreeNode::SPAN_AVAIL_WIDTH | TreeNode::FRAME_PADDING;
    if (*m_ctx->selectedEntity() == entity) {
        flags |= TreeNode::SELECTED;
    }
    if (!EntityNode{entity}.hasChildNode()) {
        flags |= TreeNode::LEAF;
    }

    std::string_view tag = entity.get<TagComponent>().tag;
    auto entity_tree_node = node->makeSubNode<TreeNode>(tag, flags);

    if (auto popup_context = WidgetNode::create<PopupContextItem>();
        popup_context.call<MenuItem>(GE_FMTSTR("Remove '{}'", tag))) {
        m_commands.removeEntity(entity);
    }

    if (!m_is_select_entity_handled && isItemClicked()) {
        m_commands.selectEntity(entity);
        m_is_select_entity_handled = true;
    }

    drawEntityDragDrop(entity);

    if (auto child_node = EntityNode{entity}.childNode();
        entity_tree_node.isOpened() && !child_node.isNull()) {
        drawEntities(&entity_tree_node, child_node);
    }
}

void ScenePanel::drawEntityDragDrop(const Entity& entity)
{
    constexpr std::string_view PAYLOAD_TYPE{"entity_payload"};
    constexpr std::string_view POPUP_ID{"scene_panel_entity_drop_popup"};

    auto entity_handle = entity.nativeHandle();
    const auto& tag = entity.get<TagComponent>();

    DragDropPayload drag_drop_paylaod{PAYLOAD_TYPE.data(), &entity_handle};
    WidgetNode::create<DragDropSource>(drag_drop_paylaod, tag.tag);
    WidgetNode::create<DragDropTarget>(&drag_drop_paylaod);

    if (drag_drop_paylaod.accepted()) {
        auto src_entity_handle = *drag_drop_paylaod.get<Entity::NativeHandle>();
        m_drag_drop_src_entity = m_ctx->scene()->entity(src_entity_handle);
        m_drag_drop_dst_entity = entity;

        OpenPopup::call(POPUP_ID);
    }

    if (entity != m_drag_drop_dst_entity) {
        return;
    }

    auto dst_tag = entity.get<TagComponent>().tag;
    auto drop_entity_popup = WidgetNode::create<PopupContextItem>(POPUP_ID);

    if (drop_entity_popup.call<MenuItem>(GE_FMTSTR("Append to '{}'", dst_tag))) {
        m_commands.appendEntity(m_drag_drop_dst_entity, m_drag_drop_src_entity);
        resetDragDropEntities();
    }
    if (drop_entity_popup.call<MenuItem>(GE_FMTSTR("Append to children of '{}'", dst_tag))) {
        m_commands.appendToChildren(m_drag_drop_dst_entity, m_drag_drop_src_entity);
        resetDragDropEntities();
    }
}

void ScenePanel::drawContextMenu(WidgetNode* node)
{
    constexpr std::string_view POPUP_ID{"empty_space_popup"};

    EntityFactory entity_factory{m_ctx->scene(), m_ctx->assets()};
    auto flags = PopupFlag::MOUSE_BUTTON_RIGHT | PopupFlag::NO_OPEN_OVER_EXISTING_POPUP |
                 PopupFlag::NO_OPEN_OVER_ITEMS;

    auto context_menu_node = node->makeSubNode<PopupContextWindow>(POPUP_ID, flags);
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

void ScenePanel::resetDragDropEntities()
{
    m_drag_drop_dst_entity = Entity{};
    m_drag_drop_src_entity = Entity{};
}

} // namespace LE
