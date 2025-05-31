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

#include "components_panel.h"
#include "level_editor_context.h"

#include "genesis/gui/widgets.h"
#include "genesis/math/trigonometric.h"
#include "genesis/scene/component_list.h"
#include "genesis/scene/components.h"
#include "genesis/scene/entity.h"

using namespace GE::GUI;
using namespace GE::Scene;
using namespace GE::P2D;

namespace LE {
namespace {

template<typename T>
const char* isLoaded(const GE::Shared<T>& resource)
{
    return resource != nullptr ? "loaded" : "null";
}

template<typename T>
bool isNewComponentSuitable(const Entity& entity)
{
    return !entity.has<T>();
}

} // namespace

ComponentsPanel::ComponentsPanel(LevelEditorContext* ctx)
    : WindowBase{NAME}
    , m_ctx{ctx}
{}

void ComponentsPanel::onRender()
{
    WidgetNode node{&m_window};
    if (!m_ctx->selectedEntity()->isNull()) {
        drawEntityComponents(m_ctx->selectedEntity());
        drawAddNewComponents(&node, m_ctx->selectedEntity());
    }
}

void ComponentsPanel::drawEntityComponents(Entity* entity)
{
    GE::forEachType<ComponentList>([this, entity](const auto& component) {
        using Component = std::decay_t<decltype(component)>;
        if (entity->has<Component>()) {
            draw<Component>(entity);
        }
    });
}

void ComponentsPanel::drawAddNewComponents(WidgetNode* node, Entity* entity)
{
    constexpr std::string_view ADD_COMPONENT_POPUP{"add_component_popup"};

    if (node->call<Button>("Add component")) {
        node->call<OpenPopup>(ADD_COMPONENT_POPUP);
    }

    auto add_component_popup = WidgetNode::create<Popup>(ADD_COMPONENT_POPUP);
    GE::forEachType<ComponentList>([&add_component_popup, entity](const auto& component) {
        using Component = std::decay_t<decltype(component)>;

        if (isNewComponentSuitable<Component>(*entity) &&
            add_component_popup.call<MenuItem>(Component::NAME)) {
            entity->add<Component>();
            CloseCurrentPopup::call();
        }
    });
}

template<typename Component>
void ComponentsPanel::draw(Entity* entity)
{
    auto flags = TreeNode::DEFAULT_OPEN | TreeNode::FRAMED | TreeNode::SPAN_AVAIL_WIDTH |
                 TreeNode::ALLOW_ITEM_OVERLAP | TreeNode::FRAME_PADDING;

    auto node = WidgetNode::create<TreeNode>(Component::NAME, flags);
    draw(&node, &entity->get<Component>());

    auto popup_context = WidgetNode::create<PopupContextWindow>(Component::NAME);
    if (popup_context.template call<MenuItem>(GE_FMTSTR("Remove '{}'", Component::NAME))) {
        entity->remove<Component>();
    }
}

void ComponentsPanel::draw(WidgetNode* node, CameraComponent* camera)
{
    bool is_primary_camera = *m_ctx->selectedEntity() == m_ctx->scene()->mainCamera();

    if (node->call<Checkbox>("Main camera", &is_primary_camera)) {
        m_ctx->scene()->setMainCamera(is_primary_camera ? *m_ctx->selectedEntity()
                                                        : GE::Scene::Entity{});
    }
    node->call<Checkbox>("Fixed aspect ratio", &camera->fixed_aspect_ratio);
}

void ComponentsPanel::draw(WidgetNode* node, MaterialComponent* material)
{
    auto material_id = material->materialID().asString();

    node->call<Text>("Material: %s (%s)", material_id.c_str(),
                     isLoaded(material->pipeline_resource));
    if (node->call<Button>("Load")) {
        material->loadMaterial(m_ctx->assets());
    }
}

void ComponentsPanel::draw(WidgetNode* node, TagComponent* tag)
{
    node->call<InputText>("Tag", &tag->tag);
}

void ComponentsPanel::draw(WidgetNode* node, TransformComponent* transform)
{
    node->call<ValueEditor>("Translation", &transform->translation, 0.05f, -10.0f, 10.0f);
    if (auto angles = GE::degrees(transform->rotation);
        node->call<ValueEditor>("Rotation", &angles, 1.0f, -360.0f, 360.0f)) {
        transform->rotation = GE::radians(angles);
    }
    node->call<ValueEditor>("Scale", &transform->scale, 0.1, 0.0f, 10.0f);
}

void ComponentsPanel::draw(WidgetNode* node, SpriteComponent* sprite)
{
    auto mesh_id = sprite->meshID().asString();
    auto texture_id = sprite->textureID().asString();

    node->call<Text>("Mesh: %s (%s)", mesh_id.data(), isLoaded(sprite->mesh));
    node->call<Text>("Texture: %s (%s)", texture_id.data(), isLoaded(sprite->texture));
    if (node->call<Button>("Load")) {
        sprite->loadAll(m_ctx->assets());
    }
}

void ComponentsPanel::draw(WidgetNode* node, RigidBody2DComponent* rigid_body)
{
    static const std::vector<std::string> BODY_TYPES = {
        GE::toString(RigidBody::Type::STATIC),
        GE::toString(RigidBody::Type::DYNAMIC),
        GE::toString(RigidBody::Type::KINEMATIC),
    };

    auto     type_string = GE::toString(rigid_body->body_type);
    ComboBox type_combo{"Type", BODY_TYPES, type_string};
    node->subNode(&type_combo);

    if (type_combo.selectedItem() != type_string) {
        rigid_body->body_type = toRigidBodyType(type_combo.selectedItem());
    }

    node->call<Checkbox>("Fixed rotation", &rigid_body->fixed_rotation);
}

void ComponentsPanel::draw(WidgetNode* node, body_shape_config_base_t* shape_config)
{
    node->call<ValueEditor>("Friction", &shape_config->friction, 0.1f, 0.0f, 10.0f);
    node->call<ValueEditor>("Restitution", &shape_config->restitution, 0.1f, 0.0f, 10.0f);
    node->call<ValueEditor>("Density", &shape_config->density, 0.1f, 0.0f, 10.0f);
}

void ComponentsPanel::draw(WidgetNode* node, BoxCollider2DComponent* collider)
{
    node->call<Checkbox>("Show collider", &collider->show_collider);
    draw(node, static_cast<body_shape_config_base_t*>(collider));
    node->call<ValueEditor>("Size", &collider->size, 0.1f, 0.0f, 10.0f);
    node->call<ValueEditor>("Center", &collider->center, 0.05f, -10.0f, 10.0f);
    if (float angle = GE::degrees(collider->angle);
        node->call<ValueEditor>("Angle", &angle, 1.0f, -360.0f, 360.0f)) {
        collider->angle = GE::radians(angle);
    }
}

void ComponentsPanel::draw(WidgetNode* node, CircleCollider2DComponent* collider)
{
    node->call<Checkbox>("Show collider", &collider->show_collider);
    draw(node, static_cast<body_shape_config_base_t*>(collider));
    node->call<ValueEditor>("Offset", &collider->offset, 0.05f, -10.0f, 10.0f);
    node->call<ValueEditor>("Radius", &collider->radius, 0.05f, 0.0f, 10.0f);
}

} // namespace LE
