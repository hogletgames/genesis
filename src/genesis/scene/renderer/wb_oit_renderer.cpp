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
#include "components/material_component.h"
#include "scene.h"

#include "genesis/graphics/framebuffer.h"
#include "genesis/graphics/renderer.h"

namespace GE::Scene {
namespace {

const std::string ACCUM_SHADER_VERTEX = R"(
#version 450

layout(location = 0) in vec3 a_Position;
layout(location = 1) in vec3 a_Color;
layout(location = 2) in vec2 a_TexCoord;

layout(push_constant) uniform u_PushConstants {
    mat4 mvp;
} pc;

layout(location = 0) out vec2 v_TexCoord;

void main()
{
    v_TexCoord = a_TexCoord;
    gl_Position = pc.mvp * vec4(a_Position, 1.0);
}
)";

const std::string ACCUM_SHADER_FRAGMENT = R"(
#version 450

layout(location = 0) in vec2 v_TexCoord;

layout(set = 0, binding = 0) uniform sampler2D u_Sprite;

layout(location = 0) out vec4 outAccumColor;
layout(location = 1) out float outReveal;

void main()
{
    vec4 texColor = texture(u_Sprite, v_TexCoord);

    // Assuming pre-multiplied alpha for the accumulation buffer
    outAccumColor = vec4(texColor.rgb * texColor.a, texColor.a);

    // For the reveal factor, write out the alpha value directly.
    outReveal = texColor.a;
}
)";

const std::string COMPOSING_SHADER_VERTEX = R"(
#version 450

// Full-screen quad vertices
vec2 positions[6] = vec2[](
    vec2(-1.0, -1.0),
    vec2(1.0, -1.0),
    vec2(1.0, 1.0),
    vec2(-1.0, -1.0),
    vec2(1.0, 1.0),
    vec2(-1.0, 1.0)
);

void main() {
    gl_Position = vec4(positions[gl_VertexIndex], 0.0, 1.0);
}
)";

const std::string COMPOSING_SHADER_FRAGMENT = R"(
#version 450

layout(push_constant) uniform u_PushConstants {
    vec2 accumTexSize;
} pc;

layout(set = 0, binding = 0) uniform sampler2D u_AccumTex;  // Accumulation texture
layout(set = 0, binding = 1) uniform sampler2D u_RevealTex; // Reveal texture

layout(location = 0) out vec4 outColor;

void main() {
    vec2  uv     = gl_FragCoord.xy / pc.accumTexSize;
    vec4  accum  = texture(u_AccumTex, uv);
    float reveal = texture(u_RevealTex, uv).r;

    // Normalize accumulated color by alpha
    vec3 color  = accum.rgb / max(accum.a, 0.0001);

    // Compute final alpha from reveal factor
    float alpha = 1.0 - reveal;

    outColor = vec4(color, alpha);
}
)";

bool isOpaque(const Entity& entity)
{
    return entity.get<SpriteComponent>().texture->isOpaque();
}

} // namespace

WeightedBlendedOITRenderer::WeightedBlendedOITRenderer(GE::Renderer* renderer,
                                                       const ViewProjectionCamera* camera)
    : RendererBase{renderer, camera}
{
    recreateAccumulationFramebuffer(m_renderer->size());
    createAccumulationPipeline(m_accumulation_fbo->renderer());
    createComposingPipeline(m_renderer);
}

void WeightedBlendedOITRenderer::render(const Scene& scene)
{
    if (m_renderer->size() != m_accumulation_fbo->size()) {
        recreateAccumulationFramebuffer(m_renderer->size());
    }

    renderTransparentEntities(scene);

    m_renderer->beginFrame();

    renderOpaqueEntities(scene);
    composeScene();

    m_renderer->endFrame();
}

void WeightedBlendedOITRenderer::recreateAccumulationFramebuffer(const Vec2& size)
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

void WeightedBlendedOITRenderer::createAccumulationPipeline(GE::Renderer* renderer)
{
    auto vertex_shader = Shader::create(Shader::Type::VERTEX);
    GE_CORE_ASSERT(vertex_shader->compileFromSource(ACCUM_SHADER_VERTEX),
                   "Failed to compile vertex shader for accumulation pipeline");

    auto fragment_shader = Shader::create(Shader::Type::FRAGMENT);
    GE_CORE_ASSERT(fragment_shader->compileFromSource(ACCUM_SHADER_FRAGMENT),
                   "Failed to compile vertex shader for accumulation pipeline");

    pipeline_config_t config{};
    config.vertex_shader = std::move(vertex_shader);
    config.fragment_shader = std::move(fragment_shader);
    config.enable_blending = false;

    m_accumulation_pipeline = renderer->createPipeline(config);
    GE_CORE_ASSERT(m_accumulation_pipeline, "Failed to create accumulation pipeline");
}

void WeightedBlendedOITRenderer::createComposingPipeline(GE::Renderer* renderer)
{
    auto vertex_shader = Shader::create(Shader::Type::VERTEX);
    GE_CORE_ASSERT(vertex_shader->compileFromSource(COMPOSING_SHADER_VERTEX),
                   "Failed to compile vertex shader for composing pipeline");

    auto fragment_shader = Shader::create(Shader::Type::FRAGMENT);
    GE_CORE_ASSERT(fragment_shader->compileFromSource(COMPOSING_SHADER_FRAGMENT),
                   "Failed to compile vertex shader for composing pipeline");

    pipeline_config_t config{};
    config.vertex_shader = std::move(vertex_shader);
    config.fragment_shader = std::move(fragment_shader);
    config.enable_blending = false;

    m_composing_pipeline = renderer->createPipeline(config);
    GE_CORE_ASSERT(m_accumulation_pipeline, "Failed to create composing pipeline");
}

void WeightedBlendedOITRenderer::renderTransparentEntities(const Scene& scene)
{
    auto* renderer = m_accumulation_fbo->renderer();

    renderer->beginFrame();

    scene.forEach<MaterialComponent, SpriteComponent>([this](const auto& entity) {
        if (!isOpaque(entity)) {
            renderTransparentSprite(entity);
        }
    });

    renderer->endFrame();
    renderer->swapBuffers();
}

void WeightedBlendedOITRenderer::renderOpaqueEntities(const Scene& scene)
{
    scene.forEach<MaterialComponent, SpriteComponent>([this](const auto& entity) {
        if (isOpaque(entity)) {
            renderSprite(m_renderer, entity);
        }
    });
}

void WeightedBlendedOITRenderer::composeScene()
{
    auto* cmd = m_renderer->command();
    auto* pipeline = m_composing_pipeline.get();

    cmd->bind(pipeline);
    cmd->pushConstant(pipeline, "pc.accumTexSize", m_accumulation_fbo->colorTexture(0).size());
    cmd->bind(pipeline, "u_AccumTex", m_accumulation_fbo->colorTexture(0));
    cmd->bind(pipeline, "u_RevealTex", m_accumulation_fbo->colorTexture(1));
    cmd->draw(6, 1, 0, 0);
}

void WeightedBlendedOITRenderer::renderTransparentSprite(const Entity& entity)
{
    std::string_view entity_tag = entity.get<TagComponent>().tag;

    const auto& sprite = entity.get<SpriteComponent>();
    auto* texture = sprite.texture.get();
    auto* mesh = sprite.mesh.get();

    if (!isValid(entity_tag, m_accumulation_pipeline.get(), texture, mesh)) {
        return;
    }

    auto mvp = m_camera->viewProjection() * entity.get<TransformComponent>().transform();

    auto* cmd = m_accumulation_fbo->renderer()->command();
    cmd->bind(m_accumulation_pipeline.get());
    cmd->bind(m_accumulation_pipeline.get(), "u_Sprite", *texture);
    cmd->pushConstant(m_accumulation_pipeline.get(), "pc.mvp", mvp);
    cmd->draw(*mesh);
}

} // namespace GE::Scene
