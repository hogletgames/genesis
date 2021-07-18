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

#include "vertex_buffer.h"
#include "renderer_factory.h"

#include "genesis/core/utils.h"

namespace {

constexpr size_t FLOAT_SIZE{4};
constexpr size_t INT_SIZE{4};
constexpr size_t BOOL_SIZE{1};

uint32_t toComponentCount(GE::VertexElement::Type type)
{
    using Type = GE::VertexElement::Type;

    static std::unordered_map<Type, uint32_t> to_comp_count = {
        {Type::FLOAT, 1},    {Type::FLOAT2, 2},   {Type::FLOAT3, 3}, {Type::FLOAT4, 4},
        {Type::MAT3, 3 * 3}, {Type::MAT4, 4 * 4}, {Type::INT, 1},    {Type::INT2, 2},
        {Type::INT3, 3},     {Type::INT4, 4},     {Type::BOOL, 1}};

    return GE::toType(to_comp_count, type, 0U);
}

} // namespace

namespace GE {

VertexElement::VertexElement(Type type, uint32_t location, bool normalized)
    : m_type{type}
    , m_location{location}
    , m_normalized{normalized}
    , m_size{getTypeSize(m_type)}
{}

uint32_t VertexElement::getTypeSize(Type type)
{
    static std::unordered_map<Type, uint32_t> to_size = {
        {Type::FLOAT, FLOAT_SIZE},  {Type::FLOAT2, FLOAT_SIZE},
        {Type::FLOAT3, FLOAT_SIZE}, {Type::FLOAT4, FLOAT_SIZE},
        {Type::MAT3, FLOAT_SIZE},   {Type::MAT4, FLOAT_SIZE},
        {Type::INT, INT_SIZE},      {Type::INT2, INT_SIZE},
        {Type::INT3, INT_SIZE},     {Type::INT4, INT_SIZE},
        {Type::BOOL, BOOL_SIZE}};

    return toType(to_size, type, 0U) * toComponentCount(type);
}

uint32_t VertexElement::getComponentCount() const
{
    return toComponentCount(m_type);
}

VertexBufferLayout::VertexBufferLayout(std::initializer_list<VertexElement> elements)
    : m_elements{elements}
{
    calculateOffsetsAndStride();
}

void VertexBufferLayout::calculateOffsetsAndStride()
{
    m_stride = 0;

    for (auto &element : m_elements) {
        element.setOffset(m_stride);
        m_stride += element.size();
    }
}

Scoped<VertexBuffer> VertexBuffer::create(const void *vertices, uint32_t count)
{
    return RendererFactory::createVertexBuffer(vertices, count);
}

Scoped<VertexBuffer> VertexBuffer::create(uint32_t size)
{
    return RendererFactory::createVertexBuffer(size);
}

} // namespace GE
