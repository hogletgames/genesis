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
#include "camera/view_projection_camera.h"
#include "components.h"
#include "scene.h"

#include "genesis/assets/registry.h"
#include "genesis/graphics/framebuffer.h"
#include "genesis/graphics/graphics.h"
#include "genesis/graphics/pipeline_config.h"
#include "genesis/graphics/renderer.h"
#include "genesis/graphics/shader.h"

namespace GE::Scene {
namespace {

const Assets::ResourceID COLOR_PIPELINE{"genesis", Assets::Group::PIPELINES, "sprite"};
const Assets::ResourceID ACCUMULATION_PIPELINE{"genesis", Assets::Group::PIPELINES,
                                               "wb_oit_accumulation"};
const Assets::ResourceID COMPOSING_PIPELINE{"genesis", Assets::Group::PIPELINES,
                                            "wb_oit_composing"};

bool isOpaque(const Entity& entity)
{
    return entity.get<SpriteComponent>().texture->isOpaque();
}

} // namespace

WeightedBlendedOITRenderer::WeightedBlendedOITRenderer(GE::Renderer* renderer,
                                                       const Assets::Registry& assets,
                                                       const ViewProjectionCamera* camera)
    : RendererBase{renderer, camera}
{
    recreateWbOitFramebuffer(m_renderer->size());

    auto* wb_oit_renderer = m_wb_oit_fbo->renderer();
    createOpaqueColorPipeline(wb_oit_renderer, assets);
    createAccumulationPipeline(wb_oit_renderer, assets);
    createComposingPipeline(m_renderer, assets);
}

void WeightedBlendedOITRenderer::render(const Scene& scene)
{
    if (m_renderer->size() != m_wb_oit_fbo->size()) {
        recreateWbOitFramebuffer(m_renderer->size());
    }

    auto* wb_oit_renderer = m_wb_oit_fbo->renderer();

    wb_oit_renderer->beginFrame(Renderer::CLEAR_ALL);
    renderOpaqueEntities(wb_oit_renderer, scene);
    renderTransparentEntities(wb_oit_renderer, scene);
    wb_oit_renderer->endFrame();
    wb_oit_renderer->swapBuffers();

    m_renderer->beginFrame(Renderer::CLEAR_ALL);
    composeScene(m_renderer);
    renderPhysicsColliders(scene);
    m_renderer->endFrame();
}

void WeightedBlendedOITRenderer::recreateWbOitFramebuffer(const Vec2& size)
{
    Framebuffer::config_t config{};
    config.size = size;
    config.msaa_samples = GE::Graphics::limits().max_msaa;
    config.attachments = {
        // Color attachment
        {.type = fb_attachment_t::Type::COLOR,
         .texture_type = TextureType::TEXTURE_2D,
         .texture_format = TextureFormat::SRGBA8,
         .clear_color = Vec4{0.0f}},
        // Accumulation attachment
        {.type = fb_attachment_t::Type::COLOR,
         .texture_type = TextureType::TEXTURE_2D,
         .texture_format = TextureFormat::RGBA16F,
         .clear_color = Vec4{0.0f}},
        // Reveal attachment
        {.type = fb_attachment_t::Type::COLOR,
         .texture_type = TextureType::TEXTURE_2D,
         .texture_format = TextureFormat::R16F,
         .clear_color = Vec4{1.0f}},
        // Depth attachment
        {.type = fb_attachment_t::Type::DEPTH,
         .texture_type = TextureType::TEXTURE_2D,
         .texture_format = TextureFormat::D32F,
         .clear_depth = 1.0f},
    };

    m_wb_oit_fbo = Framebuffer::create(config);
}

void WeightedBlendedOITRenderer::createOpaqueColorPipeline(GE::Renderer* renderer,
                                                           const Assets::Registry& assets)
{
    auto pipeline_resource = assets.get<Assets::PipelineResource>(COLOR_PIPELINE);
    GE_CORE_ASSERT(pipeline_resource, "Failed to find color pipeline resource '{}'",
                   COLOR_PIPELINE.asString());

    blending_t color_blending{};
    color_blending.enabled = false;

    pipeline_config_t config{};
    config.blending = color_blending;
    config.depth_test_enable = true;
    config.depth_write_enable = true;

    m_color_pipeline = pipeline_resource->createPipeline(renderer, config);
    GE_CORE_ASSERT(m_color_pipeline, "Failed to create color pipeline");
}

void WeightedBlendedOITRenderer::createAccumulationPipeline(GE::Renderer* renderer,
                                                            const Assets::Registry& assets)
{
    auto pipeline_resource = assets.get<Assets::PipelineResource>(ACCUMULATION_PIPELINE);
    GE_CORE_ASSERT(pipeline_resource, "Failed to find accumulation pipeline resource '{}'",
                   ACCUMULATION_PIPELINE.asString());

    blending_t color_blending{};
    color_blending.enabled = false;

    blending_t accum_blending{};
    accum_blending.enabled = true;
    accum_blending.src_color_factor = BlendFactor::ONE;
    accum_blending.dst_color_factor = BlendFactor::ONE;
    accum_blending.src_alpha_factor = BlendFactor::ONE;
    accum_blending.dst_alpha_factor = BlendFactor::ONE;

    blending_t reveal_blending{};
    reveal_blending.enabled = true;
    reveal_blending.src_color_factor = BlendFactor::ZERO;
    reveal_blending.dst_color_factor = BlendFactor::ONE_MINUS_SRC_COLOR;
    reveal_blending.src_alpha_factor = BlendFactor::ZERO;
    reveal_blending.dst_alpha_factor = BlendFactor::ONE_MINUS_SRC_ALPHA;

    pipeline_config_t config{};
    config.blending = std::vector<blending_t>{color_blending, accum_blending, reveal_blending};
    config.depth_test_enable = true;
    config.depth_write_enable = false;

    m_accumulation_pipeline = pipeline_resource->createPipeline(renderer, config);
    GE_CORE_ASSERT(m_accumulation_pipeline, "Failed to create accumulation pipeline");
}

void WeightedBlendedOITRenderer::createComposingPipeline(GE::Renderer* renderer,
                                                         const Assets::Registry& assets)
{
    auto pipeline_resource = assets.get<Assets::PipelineResource>(COMPOSING_PIPELINE);
    GE_CORE_ASSERT(pipeline_resource, "Failed to find composing pipeline resource '{}'",
                   COMPOSING_PIPELINE.asString());

    blending_t blending{};
    blending.enabled = true;
    blending.src_color_factor = BlendFactor::SRC_COLOR;
    blending.dst_color_factor = BlendFactor::ONE_MINUS_SRC_COLOR;
    blending.src_alpha_factor = BlendFactor::SRC_ALPHA;
    blending.dst_alpha_factor = BlendFactor::ONE_MINUS_SRC_ALPHA;

    pipeline_config_t config{};
    config.blending = blending;
    config.depth_test_enable = false;
    config.depth_write_enable = false;

    m_composing_pipeline = pipeline_resource->createPipeline(renderer, config);
    GE_CORE_ASSERT(m_composing_pipeline, "Failed to create composing pipeline");
}

void WeightedBlendedOITRenderer::renderOpaqueEntities(GE::Renderer* renderer, const Scene& scene)
{
    scene.forEach<SpriteComponent>([this, renderer](const auto& entity) {
        if (isOpaque(entity)) {
            renderEntity(renderer, m_color_pipeline.get(), entity);
        }
    });
}

void WeightedBlendedOITRenderer::renderTransparentEntities(GE::Renderer* renderer,
                                                           const Scene& scene)
{
    scene.forEach<SpriteComponent>([this, renderer](const auto& entity) {
        if (!isOpaque(entity)) {
            renderEntity(renderer, m_accumulation_pipeline.get(), entity);
        }
    });
}

void WeightedBlendedOITRenderer::renderPhysicsColliders(const Scene& scene)
{
    m_primitives_renderer.begin();
    scene.forEach<RigidBody2DComponent>(
        [this](const auto& entity) { renderPhysics2DColliders(entity); });
    m_primitives_renderer.end();
}

void WeightedBlendedOITRenderer::composeScene(GE::Renderer* renderer)
{
    auto* cmd = renderer->command();
    auto* pipeline = m_composing_pipeline.get();

    constexpr uint32_t VERTEX_COUNT{6};

    cmd->bind(pipeline);
    cmd->bind(pipeline, "u_ColorTex", m_wb_oit_fbo->colorTexture(0));
    cmd->bind(pipeline, "u_AccumTex", m_wb_oit_fbo->colorTexture(1));
    cmd->bind(pipeline, "u_RevealTex", m_wb_oit_fbo->colorTexture(2));
    cmd->draw(VERTEX_COUNT, 1, 0, 0);
}

} // namespace GE::Scene
