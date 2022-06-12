/*
 * BSD 3-Clause License
 *
 * Copyright (c) 2021, Dmitry Shilnenkov
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

#include "triangle_layer.h"

#include "genesis/core/asserts.h"
#include "genesis/graphics/graphics.h"
#include "genesis/graphics/pipeline.h"
#include "genesis/graphics/render_command.h"
#include "genesis/graphics/shader.h"
#include "genesis/graphics/vertex_buffer.h"
#include "genesis/math/types.h"

namespace {

constexpr auto VERTEX_SHADER = R"(#version 450

layout(location = 0) in vec3 a_Position;
layout(location = 1) in vec3 a_Color;

layout(location = 0) out vec3 v_Color;

void main()
{
    v_Color = a_Color;
    gl_Position = vec4(a_Position, 1.0);
}
)";

constexpr auto FRAGMENT_SHADER = R"(#version 450

layout(location = 0) in vec3 v_Color;

layout(location = 0) out vec4 Color;

void main()
{
    Color = vec4(v_Color, 1.0);
}
)";

struct vertex_t {
    GE::Vec3 position;
    GE::Vec3 color;
};

} // namespace

namespace GE::Examples {

void TriangleLayer::onAttached()
{
    // Shader Program
    Shared<Shader> vert = Shader::create(Shader::Type::VERTEX);
    GE_ASSERT(vert->compileFromSource(VERTEX_SHADER), "Failed to compile vertex shader");

    Shared<Shader> frag = Shader::create(Shader::Type::FRAGMENT);
    GE_ASSERT(frag->compileFromSource(FRAGMENT_SHADER), "Failed to compile fragment shader");

    pipeline_config_t pipeline_config{};
    pipeline_config.vertex_shader = vert;
    pipeline_config.fragment_shader = frag;
    m_pipeline = Graphics::windowRenderer()->createPipeline(pipeline_config);
    GE_ASSERT(m_pipeline, "Failed to create Pipeline");

    // VBO
    static const std::vector<vertex_t> triangle = {
        {{-0.5f, -0.5f, 0.0f}, {1.0f, 0.0f, 0.0f}},
        {{0.5f, -0.5f, 0.0f}, {0.0f, 0.1f, 0.0f}},
        {{0.0f, 0.5f, 0.0f}, {0.0f, 0.0f, 1.0f}},
    };

    m_vbo = VertexBuffer::create(triangle.size() * sizeof(vertex_t), triangle.data());
}

void TriangleLayer::onDetached()
{
    m_pipeline.reset();
    m_vbo.reset();
}

void TriangleLayer::onUpdate([[maybe_unused]] Timestamp ts) {}

void TriangleLayer::onEvent([[maybe_unused]] Event *event) {}

void TriangleLayer::onRender()
{
    Graphics::command()->bind(m_pipeline.get());
    Graphics::command()->draw(m_vbo.get(), 3);
}

} // namespace GE::Examples
