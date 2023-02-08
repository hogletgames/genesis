/*
 * BSD 3-Clause License
 *
 * Copyright (c) 2022, Dmitry Shilnenkov
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

#include "shape.h"

#include "genesis/core.h"
#include "genesis/graphics.h"

namespace {

constexpr auto VERTEX_SHADER{"examples/sandbox/assets/shaders/shape_shader.vert"};
constexpr auto FRAGMENT_SHADER{"examples/sandbox/assets/shaders/shape_shader.frag"};

} // namespace

namespace GE::Examples {

Shape::Shape(Renderer* renderer)
    : Drawable{renderer, VERTEX_SHADER, FRAGMENT_SHADER}
{}

Shape::~Shape() = default;

void Shape::draw(Renderer* renderer, const mvp_t& mvp)
{
    bind(renderer, mvp);
    renderer->command()->draw(m_vbo.get(), m_ibo.get());
}

void Shape::setVertices(const std::vector<vertex_t>& vertices)
{
    m_vbo = VertexBuffer::create(vertices.size() * sizeof(vertex_t), vertices.data());
    GE_ASSERT(m_vbo, "Failed to create vertex buffer");
}

void Shape::setIndices(const std::vector<uint32_t>& indices)
{
    m_ibo = IndexBuffer::create(indices.data(), indices.size());
    GE_ASSERT(m_ibo, "Failed to create vertex buffer");
}

} // namespace GE::Examples
