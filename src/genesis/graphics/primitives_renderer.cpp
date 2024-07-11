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

#include "primitives_renderer.h"
#include "pipeline.h"
#include "pipeline_config.h"
#include "renderer.h"
#include "vertex_buffer.h"

#include "genesis/core/asserts.h"

namespace GE {
namespace {

constexpr uint32_t CIRCLE_SEGMENT_COUNT{64};
constexpr uint32_t BOX_VERTEX_COUNT{4};

constexpr std::string_view SHADER_VERT = R"(
#version 450

layout(location = 0) in vec2 a_Position;

layout(push_constant) uniform u_PushConstants {
    mat4 transform;
    vec4 color;
} pc;

layout(location = 0) out vec4 v_Color;

void main()
{
    v_Color = pc.color;
    gl_Position = pc.transform * vec4(a_Position, 0.0, 1.0);
}
)";

constexpr std::string_view SHADER_FRAG = R"(
#version 450

layout(location = 0) in  vec4 v_Color;

layout(location = 0) out vec4 out_Color;

void main()
{
    out_Color = v_Color;
}
)";

Scoped<Pipeline> createPipeline(Renderer* renderer)
{
    pipeline_config_t config;
    config.primitive_topology = PrimitiveTopology::LINE_STRIP;
    config.depth_test_enable = false;
    config.depth_write_enable = false;

    config.vertex_shader->compileFromSource(SHADER_VERT.data());
    GE_CORE_ASSERT(config.vertex_shader, "Failed to complie vertex shader");

    config.fragment_shader->compileFromSource(SHADER_FRAG.data());
    GE_CORE_ASSERT(config.fragment_shader, "Failed to complie fragment shader");

    return renderer->createPipeline(config);
}

Scoped<VertexBuffer> createCircleVBO(uint64_t segment_count)
{
    constexpr float CIRCLE_RADIUS{0.5f};

    float angle = 2.0f * M_PI / segment_count;
    std::vector<Vec2> vertices(segment_count);

    for (int i{0}; i < segment_count; i++) {
        float point_angle = angle * i;
        vertices[i] = Vec2{std::sin(point_angle), std::cos(point_angle)} * CIRCLE_RADIUS;
    }

    vertices.push_back(vertices[0]);
    return VertexBuffer::create(vertices.size() * sizeof(Vec2), vertices.data());
}

Scoped<VertexBuffer> createSquareVBO()
{
    constexpr std::array<Vec2, 5> SQUARE_VERTICES = {
        Vec2{-0.5f, 0.5f}, Vec2{0.5f, 0.5f}, Vec2{0.5f, -0.5f}, Vec2{-0.5, -0.5}, Vec2{-0.5, 0.5},
    };

    return VertexBuffer::create(SQUARE_VERTICES.size() * sizeof(Vec2), SQUARE_VERTICES.data());
}

} // namespace

PrimitivesRenderer::PrimitivesRenderer(Renderer* renderer)
    : m_renderer{renderer}
    , m_pipeline{createPipeline(m_renderer)}
    , m_circle_vbo{createCircleVBO(CIRCLE_SEGMENT_COUNT)}
    , m_square_vbo{createSquareVBO()}
{}

PrimitivesRenderer::~PrimitivesRenderer() = default;

void PrimitivesRenderer::begin()
{
    m_renderer->command()->bind(m_pipeline.get());
}

void PrimitivesRenderer::end() {}

void PrimitivesRenderer::renderCircle(const Mat4& transform, const Vec4& color)
{
    auto* cmd = m_renderer->command();
    auto* pipeline = m_pipeline.get();

    cmd->pushConstant(pipeline, "pc.transform", transform);
    cmd->pushConstant(pipeline, "pc.color", color);
    cmd->draw(m_circle_vbo.get(), CIRCLE_SEGMENT_COUNT + 1);
}

void PrimitivesRenderer::renderSquare(const Mat4& transform, const Vec4& color)
{
    auto* cmd = m_renderer->command();
    auto* pipeline = m_pipeline.get();

    cmd->pushConstant(pipeline, "pc.transform", transform);
    cmd->pushConstant(pipeline, "pc.color", color);
    cmd->draw(m_square_vbo.get(), BOX_VERTEX_COUNT + 1);
}

} // namespace GE
