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

namespace GE {
namespace {

constexpr uint32_t CIRCLE_VERTEX_COUNT{64};

constexpr std::string_view CIRCLE_VERT = R"(
#version 450

layout(location = 0) in vec2 a_Position;

layout(push_constant) uniform u_PushConstants {
    float radius;
    vec2 center;
    vec4 color;
} pc;

layout(location = 0) out vec4 v_Color;

void main()
{
    vec2 vertex_position = a_Position * pc.radius + pc.center;
    gl_Position = vec4(vertex_position, 0.0, 1.0);
    v_Color = pc.color;
}
)";

constexpr std::string_view CIRCLE_FRAG = R"(
#version 450

layout(location = 0) in  vec4 v_Color;

layout(location = 0) out vec4 out_Color;

void main()
{
    out_Color = v_Color;
}
)";

Scoped<Pipeline> makeCirclePipeline(Renderer* renderer)
{
    pipeline_config_t config;
    config.vertex_shader->compileFromSource(CIRCLE_VERT.data());
    config.fragment_shader->compileFromSource(CIRCLE_FRAG.data());
    config.primitive_topology = PrimitiveTopology::LINE_STRIP;
    config.polygon_mode = PolygonMode::LINE;
    config.depth_test_enable = false;
    config.depth_write_enable = false;

    return renderer->createPipeline(config);
}

Scoped<VertexBuffer> makeCircleVertices(uint64_t segment_count)
{
    float angle = 2 * M_PI_2 / segment_count;
    std::vector<Vec2> vertices(segment_count);

    for (int i{0}; i < segment_count; i++) {
        float point_angle = angle * i;
        vertices[i].x = std::cos(point_angle);
        vertices[i].y = std::sin(point_angle);
    }

    vertices.push_back(vertices[0]);
    return VertexBuffer::create(vertices.size(), vertices.data());
}

} // namespace

PrimitivesRenderer::PrimitivesRenderer(Renderer* renderer)
    : m_renderer{renderer}
    , m_circle{makeCirclePipeline(m_renderer), makeCircleVertices(CIRCLE_VERTEX_COUNT)}
{}

void PrimitivesRenderer::renderCircle(float radius, const Vec2& center, const Vec3& color)
{
    auto* cmd = m_renderer->command();
    auto* pipeline = m_circle.pipeline.get();

    cmd->bind(pipeline);
    cmd->pushConstant(pipeline, "pc.radius", radius);
    cmd->pushConstant(pipeline, "pc.center", center);
    cmd->pushConstant(pipeline, "pc.color", color);
    cmd->draw(m_circle.vbo.get(), CIRCLE_VERTEX_COUNT);
}

} // namespace GE
