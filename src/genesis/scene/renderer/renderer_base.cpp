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

#include "renderer/renderer_base.h"
#include "camera/view_projection_camera.h"
#include "components.h"
#include "entity.h"
#include "entity_node.h"

#include "genesis/core/log.h"
#include "genesis/graphics/render_command.h"
#include "genesis/graphics/renderer.h"

namespace GE::Scene {

RendererBase::RendererBase(GE::Renderer* renderer, const ViewProjectionCamera* camera)
    : m_renderer{renderer}
    , m_camera{camera}
{}

void RendererBase::renderEntity(GE::Renderer* renderer, Pipeline* pipeline, const Entity& entity)
{
    std::string_view entity_tag = entity.get<TagComponent>().tag;

    const auto& sprite = entity.get<SpriteComponent>();
    auto* texture = sprite.texture.get();
    auto* mesh = sprite.mesh.get();

    if (!isValid(entity_tag, pipeline, texture, mesh)) {
        return;
    }

    auto entity_transform = entity.get<TransformComponent>().transform();
    auto parent_transform = parentTransform(entity);
    auto mvp = m_camera->viewProjection() * parent_transform * entity_transform;

    auto* cmd = renderer->command();
    cmd->bind(pipeline);
    cmd->bind(pipeline, "u_Sprite", *texture);
    cmd->pushConstant(pipeline, "pc.mvp", mvp);
    cmd->draw(*mesh);
}

bool RendererBase::isValid(std::string_view entity_name, Pipeline* material, Texture* texture,
                           Mesh* mesh) const
{
    if (material == nullptr) {
        GE_CORE_ERR("A pipeline for an entity '{}' is null", entity_name);
        return false;
    }

    if (texture == nullptr) {
        GE_CORE_ERR("A texture for an entity '{}' is null", entity_name);
        return false;
    }

    if (mesh == nullptr) {
        GE_CORE_ERR("A mesh for an entity '{}' is null", entity_name);
        return false;
    }

    return true;
}

Mat4 parentTransform(const Entity& entity)
{
    Mat4 parent_transform{1.0f};
    auto parent_entity_node = EntityNode{entity}.parentNode();

    while (!parent_entity_node.isNull()) {
        auto transform = parent_entity_node.entity().get<TransformComponent>().transform();
        parent_transform = transform * parent_transform;
        parent_entity_node = parent_entity_node.parentNode();
    }

    return parent_transform;
}

} // namespace GE::Scene
