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

#include "renderer/wb_oit_renderer.h"
#include "components.h"
#include "scene.h"

#include "genesis/graphics/framebuffer.h"
#include "genesis/graphics/renderer.h"

namespace GE::Scene {

WeightedBlendedOITRenderer::WeightedBlendedOITRenderer(const ViewProjectionCamera* camera,
                                                       Assets::Registry* registry)
    : RendererBase{camera}
{
    m_accumulation_pipeline
}

void WeightedBlendedOITRenderer::render(GE::Renderer* renderer, const Scene& scene)
{
    if (renderer->size() != m_accumulation_fbo->size()) {
        recreateAccumulationFramebuffer(renderer->size());
    }

    renderOpaqueEntities(renderer, scene);
    renderTransparentEntities(scene);
    composeScene(renderer, scene);
}

void WeightedBlendedOITRenderer::createAccumulationFramebuffer(const Vec2& size)
{
    Framebuffer::config_t config{};
    config.size = size;
    config.msaa_samples = 1;
    config.attachments = {
        {fb_attachment_t::Type::COLOR, TextureType::TEXTURE_2D, TextureFormat::RGBA32F},
        {fb_attachment_t::Type::COLOR, TextureType::TEXTURE_2D, TextureFormat::R32F},
        {fb_attachment_t::Type::DEPTH, TextureType::TEXTURE_2D, TextureFormat::D32F},
    };

    m_accumulation_fbo = Framebuffer::create(config);
}

void WeightedBlendedOITRenderer::renderOpaqueEntities(GE::Renderer* renderer, const Scene& scene)
{
    renderer->beginFrame();

    scene.forEach<MaterialComponent, SpriteComponent>([this, renderer](const auto& entity) {
        if (entity.get<SpriteComponent>().texture->isOpaque()) {
            renderSprite(renderer, entity);
        }
    });

    renderer->endFrame();
}

void WeightedBlendedOITRenderer::renderTransparentEntities(const Scene& scene)
{
    auto* renderer = m_accumulation_fbo->renderer();

    renderer->beginFrame();

    scene.forEach<MaterialComponent, SpriteComponent>([this, renderer](const auto& entity) {
        if (entity.get<SpriteComponent>().texture->isOpaque()) {
            renderTransparentSprite(renderer, entity);
        }
    });

    renderer->endFrame();
}

void WeightedBlendedOITRenderer::composeScene(GE::Renderer* renderer, const Scene& scene)
{
    renderer->beginFrame(Renderer::CLEAR_NONE);

    renderer->endFrame();
}

void WeightedBlendedOITRenderer::renderTransparentSprite(GE::Renderer* renderer,
                                                         const Entity& entity)
{
    std::string_view entity_tag = entity.get<TagComponent>().tag;

    const auto& sprite = entity.get<SpriteComponent>();
    auto* texture = sprite.texture.get();
    auto* mesh = sprite.mesh.get();

    if (!isValid(entity_tag, m_accumulation_pipeline, texture, mesh)) {
        return;
    }

    auto mvp = m_camera->viewProjection() * entity.get<TransformComponent>().transform();

    auto* cmd = renderer->command();
    cmd->bind(m_accumulation_pipeline.get());
    cmd->bind(m_accumulation_pipeline.get(), "u_Sprite", texture);
    cmd->pushConstant(m_accumulation_pipeline.get(), "pc.mvp", mvp);
    cmd->draw(*mesh);
}

} // namespace GE::Scene
