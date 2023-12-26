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

#include "renderer.h"
#include "camera/view_projection_camera.h"
#include "components.h"
#include "entity.h"
#include "scene.h"

#include "genesis/graphics/renderer.h"
#include "genesis/graphics/uniform_buffer.h"

namespace GE::Scene {
namespace {

struct vp_t {
    Mat4 view{1.0f};
    Mat4 projection{1.0f};
};

bool isValid(std::string_view entity_name, Pipeline* material, Texture* texture, Mesh* mesh)
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

} // namespace

Renderer::Renderer(GE::Renderer* renderer, const ViewProjectionCamera* camera)
    : m_renderer{renderer}
    , m_camera{camera}
    , m_vp_ubo{UniformBuffer::create(sizeof(vp_t))}
    , m_translation_ubo{UniformBuffer::create(sizeof(Mat4))}
{}

Renderer::~Renderer() = default;

void Renderer::render(const Scene& scene)
{
    m_renderer->beginFrame();

    updateViewProjectionUBO();
    scene.forEach<MaterialComponent, SpriteComponent>(
        [this](const auto& entity) { renderSprite(entity); });

    m_renderer->endFrame();
}

void Renderer::renderSprite(const GE::Scene::Entity& entity)
{
    std::string_view entity_tag = entity.get<TagComponent>().tag;

    const auto& material = entity.get<MaterialComponent>();
    auto* pipeline = material.material.get();

    const auto& sprite = entity.get<SpriteComponent>();
    auto* texture = sprite.texture.get();
    auto* mesh = sprite.mesh.get();

    if (!isValid(entity_tag, pipeline, texture, mesh)) {
        return;
    }

    auto mvp = m_camera->viewProjection() * entity.get<TransformComponent>().transform();

    auto* cmd = m_renderer->command();
    cmd->bind(pipeline);
    cmd->bind(pipeline, "u_Sprite", texture);
    cmd->pushConstant(pipeline, "pc.mvp", mvp);
    cmd->draw(*mesh);
}

void Renderer::updateViewProjectionUBO()
{
    m_vp_ubo->setObject(vp_t{m_camera->view(), m_camera->projection()});
}

} // namespace GE::Scene
