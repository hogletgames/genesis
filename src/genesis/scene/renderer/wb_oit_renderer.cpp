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

#include "genesis/graphics/framebuffer.h"
#include "genesis/graphics/graphics.h"
#include "genesis/graphics/pipeline_config.h"
#include "genesis/graphics/renderer.h"
#include "genesis/graphics/shader.h"

namespace GE::Scene {
namespace {

const std::string COLOR_SHADER_VERTEX = R"(
#version 450

layout(location = 0) in vec3 a_Position;
layout(location = 1) in vec3 a_Color;
layout(location = 2) in vec2 a_TexCoord;

layout(push_constant) uniform u_PushConstants {
    mat4 mvp;
} pc;

layout(location = 0) out vec3 v_Color;
layout(location = 1) out vec2 v_TexCoord;

void main()
{
    v_Color = a_Color;
    v_TexCoord = a_TexCoord;
    gl_Position = pc.mvp * vec4(a_Position, 1.0);
}
)";

const std::string COLOR_SHADER_FRAGMENT = R"(
#version 450

layout(location = 0) in vec3 v_Color;
layout(location = 1) in vec2 v_TexCoord;

layout(set = 0, binding = 0) uniform sampler2D u_Sprite;

layout(location = 0) out vec4 outColor;

void main()
{
    outColor = texture(u_Sprite, v_TexCoord) * vec4(v_Color, 1.0);
}
)";

const std::string ACCUM_SHADER_FRAGMENT = R"(
#version 450

layout(location = 0) in vec3 v_Color;
layout(location = 1) in vec2 v_TexCoord;

layout(set = 0, binding = 0) uniform sampler2D u_Sprite;

layout(location = 1) out vec4  outAccumColor;
layout(location = 2) out float outReveal;

void main()
{
    vec4 color = texture(u_Sprite, v_TexCoord) * vec4(v_Color, 1.0);
    float z = gl_FragCoord.z;

    // Calculate the weight
    float weight = max(min(1.0, max(max(color.r, color.g), color.b) * color.a), color.a) *
                   clamp(0.03 / (1e-5 + pow(z / 200, 4.0)), 1e-2, 3e3);

    // Output the premultiplied color and the weight
    outAccumColor = vec4(color.rgb * color.a, color.a) * weight;
    outReveal = color.a;
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

layout(set = 0, binding = 0) uniform sampler2D u_ColorTex;  // Opaque color texture
layout(set = 1, binding = 0) uniform sampler2D u_AccumTex;  // Accumulation texture
layout(set = 2, binding = 0) uniform sampler2D u_RevealTex; // Reveal texture

layout(location = 0) out vec4 outColor;

// epsilon number
const float EPSILON = 0.00001f;

// calculate floating point numbers equality accurately
bool isApproximatelyEqual(float a, float b)
{
    return abs(a - b) <= (abs(a) < abs(b) ? abs(b) : abs(a)) * EPSILON;
}

// get the max value between three values
float max3(vec3 v)
{
    return max(max(v.x, v.y), v.z);
}

void main()
{
    // fragment coordination
    ivec2 coords = ivec2(gl_FragCoord.xy);

    // base color
    outColor = texelFetch(u_ColorTex, coords, 0);

    // fragment revealage
    float revealage = texelFetch(u_RevealTex, coords, 0).r;

    // save the blending and color texture fetch cost if there is not a transparent fragment
    if (isApproximatelyEqual(revealage, 1.0f)) {
        return;
    }

    // fragment color
    vec4 accumulation = texelFetch(u_AccumTex, coords, 0);

    // suppress overflow
    if (isinf(max3(abs(accumulation.rgb)))) {
        accumulation.rgb = vec3(accumulation.a);
    }

    // prevent floating point precision bug
    vec3 average_color = accumulation.rgb / max(accumulation.a, EPSILON);

    // final transparent color
    vec4 final_color = vec4(average_color, 1.0f - revealage);

    // blend with the underlying color buffer
    outColor = mix(outColor, final_color, final_color.a);
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
    createComposingPipeline(m_renderer);
    recreateWbOitFramebuffer(m_renderer->size());

    auto* wb_oit_renderer = m_wb_oit_fbo->renderer();
    createOpaqueColorPipeline(wb_oit_renderer);
    createAccumulationPipeline(wb_oit_renderer);
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
    m_renderer->endFrame();
}

void WeightedBlendedOITRenderer::recreateWbOitFramebuffer(const Vec2& size)
{
    Framebuffer::config_t config{};
    config.size = size;
    config.msaa_samples = GE::Graphics::limits().max_msaa;
    config.attachments = {
        // Color attachment
        {fb_attachment_t::Type::COLOR, TextureType::TEXTURE_2D, TextureFormat::SRGBA8, Vec4{0.0f}},
        // Accumulation attachment
        {fb_attachment_t::Type::COLOR, TextureType::TEXTURE_2D, TextureFormat::RGBA16F, Vec4{0.0f}},
        // Reveal attachment
        {fb_attachment_t::Type::COLOR, TextureType::TEXTURE_2D, TextureFormat::R16F, Vec4{1.0f}},
        // Depth attachment
        {fb_attachment_t::Type::DEPTH, TextureType::TEXTURE_2D, TextureFormat::D32F, Vec4{}, 1.0f},
    };

    m_wb_oit_fbo = Framebuffer::create(config);
}

void WeightedBlendedOITRenderer::createOpaqueColorPipeline(GE::Renderer* renderer)
{
    auto vertex_shader = Shader::create(Shader::Type::VERTEX);
    GE_CORE_ASSERT(vertex_shader->compileFromSource(COLOR_SHADER_VERTEX),
                   "Failed to compile vertex shader for accumulation pipeline");

    auto fragment_shader = Shader::create(Shader::Type::FRAGMENT);
    GE_CORE_ASSERT(fragment_shader->compileFromSource(COLOR_SHADER_FRAGMENT),
                   "Failed to compile vertex shader for accumulation pipeline");

    blending_t color_blending{};
    color_blending.enabled = false;

    pipeline_config_t config{};
    config.vertex_shader = std::move(vertex_shader);
    config.fragment_shader = std::move(fragment_shader);
    config.blending = color_blending;
    config.depth_test_enable = true;
    config.depth_write_enable = true;

    m_color_pipeline = renderer->createPipeline(config);
    GE_CORE_ASSERT(m_color_pipeline, "Failed to create color pipeline");
}

void WeightedBlendedOITRenderer::createAccumulationPipeline(GE::Renderer* renderer)
{
    auto vertex_shader = Shader::create(Shader::Type::VERTEX);
    GE_CORE_ASSERT(vertex_shader->compileFromSource(COLOR_SHADER_VERTEX),
                   "Failed to compile vertex shader for accumulation pipeline");

    auto fragment_shader = Shader::create(Shader::Type::FRAGMENT);
    GE_CORE_ASSERT(fragment_shader->compileFromSource(ACCUM_SHADER_FRAGMENT),
                   "Failed to compile vertex shader for accumulation pipeline");

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
    config.vertex_shader = std::move(vertex_shader);
    config.fragment_shader = std::move(fragment_shader);
    config.blending = std::vector<blending_t>{color_blending, accum_blending, reveal_blending};
    config.depth_test_enable = true;
    config.depth_write_enable = false;

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

    blending_t blending{};
    blending.enabled = true;
    blending.src_color_factor = BlendFactor::SRC_COLOR;
    blending.dst_color_factor = BlendFactor::ONE_MINUS_SRC_COLOR;
    blending.src_alpha_factor = BlendFactor::SRC_ALPHA;
    blending.dst_alpha_factor = BlendFactor::ONE_MINUS_SRC_ALPHA;

    pipeline_config_t config{};
    config.vertex_shader = std::move(vertex_shader);
    config.fragment_shader = std::move(fragment_shader);
    config.blending = blending;
    config.depth_test_enable = false;
    config.depth_write_enable = false;

    m_composing_pipeline = renderer->createPipeline(config);
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
