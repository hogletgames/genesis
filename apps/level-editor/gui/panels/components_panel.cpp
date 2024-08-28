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

namespace LE {
namespace {

template<typename T>
const char *isLoaded(const GE::Shared<T> &resource)
{
    return resource != nullptr ? "loaded" : "null";
}

} // namespace

ComponentsPanel::ComponentsPanel(LevelEditorContext *ctx)
    : WindowBase{NAME}
    , m_ctx{ctx}
{}

void ComponentsPanel::onRender()
{
    WidgetNode node{&m_window};
    if (!m_ctx->selectedEntity()->isNull()) {
        drawEntityComponents(m_ctx->selectedEntity());
    }
}

void ComponentsPanel::drawEntityComponents(Entity *entity)
{
    GE::forEachType<ComponentList>([this, entity](const auto &component) {
        using Component = std::decay_t<decltype(component)>;
        if (entity->has<Component>()) {
            draw<Component>(entity);
        }
    });
}

template<typename Component>
void ComponentsPanel::draw(Entity *entity)
{
    auto flags = TreeNode::DEFAULT_OPEN | TreeNode::FRAMED | TreeNode::SPAN_AVAIL_WIDTH |
                 TreeNode::ALLOW_ITEM_OVERLAP | TreeNode::FRAME_PADDING;

    auto tree_node = WidgetNode::create<TreeNode>(Component::NAME, flags);
    draw(&tree_node, &entity->get<Component>());
}

void ComponentsPanel::draw(WidgetNode *node, GE::Scene::CameraComponent *camera)
{
    bool is_primary_camera = *m_ctx->selectedEntity() == m_ctx->scene()->mainCamera();

    if (node->call<Checkbox>("Main camera", &is_primary_camera)) {
        m_ctx->scene()->setMainCamera(is_primary_camera ? *m_ctx->selectedEntity()
                                                        : GE::Scene::Entity{});
    }
    node->call<Checkbox>("Fixed aspect ratio", &camera->fixed_aspect_ratio);
}

void ComponentsPanel::draw(WidgetNode *node, MaterialComponent *material)
{
    auto material_id = material->materialID().asString();

    node->call<Text>("Material: %s (%s)", material_id.c_str(),
                     isLoaded(material->pipeline_resource));
    if (node->call<Button>("Load")) {
        material->loadMaterial(m_ctx->assets());
    }
}

void ComponentsPanel::draw(WidgetNode *node, TagComponent *tag)
{
    node->call<Text>("Tag: %s", tag->tag.c_str());
}

void ComponentsPanel::draw(WidgetNode *node, TransformComponent *transform)
{
    node->call<ValueEditor>("Translation", &transform->translation, 0.05f, -10.0f, 10.0f);
    if (auto angles = GE::degrees(transform->rotation);
        node->call<ValueEditor>("Rotation", &angles, 1.0f, -360.0f, 360.0f)) {
        transform->rotation = GE::radians(angles);
    }
    node->call<ValueEditor>("Scale", &transform->scale, 0.1, 0.0f, 10.0f);
}

void ComponentsPanel::draw(WidgetNode *node, SpriteComponent *sprite)
{
    auto mesh_id = sprite->meshID().asString();
    auto texture_id = sprite->textureID().asString();

    node->call<Text>("Mesh: %s (%s)", mesh_id.data(), isLoaded(sprite->mesh));
    node->call<Text>("Texture: %s (%s)", texture_id.data(), isLoaded(sprite->texture));
    if (node->call<Button>("Load")) {
        sprite->loadAll(m_ctx->assets());
    }
}

} // namespace LE
