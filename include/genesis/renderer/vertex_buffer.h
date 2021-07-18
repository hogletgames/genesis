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

#ifndef GENESIS_RENDERER_VERTEX_BUFFER_H_
#define GENESIS_RENDERER_VERTEX_BUFFER_H_

#include <genesis/core/interface.h>
#include <genesis/core/memory.h>

#include <vector>

namespace GE {

class GE_API VertexElement
{
public:
    enum class Type : uint8_t
    {
        NONE = 0,
        FLOAT,
        FLOAT2,
        FLOAT3,
        FLOAT4,
        MAT3,
        MAT4,
        INT,
        INT2,
        INT3,
        INT4,
        BOOL
    };

    VertexElement(Type type, uint32_t location, bool normalized = false);

    static uint32_t getTypeSize(Type type);

    void setOffset(uint32_t offset) { m_offset = offset; }

    Type type() const { return m_type; }
    uint32_t location() const { return m_location; }
    bool normalized() const { return m_normalized; }
    uint32_t size() const { return m_size; }
    uint32_t offset() const { return m_offset; }

    uint32_t getComponentCount() const;

private:
    Type m_type{Type::NONE};
    uint32_t m_location;
    bool m_normalized{false};
    uint32_t m_size{};
    uint32_t m_offset{};
};

class GE_API VertexBufferLayout
{
public:
    using Elements = std::vector<VertexElement>;
    using iterator = typename Elements::iterator;
    using const_iterator = typename Elements::const_iterator;

    VertexBufferLayout() = default;
    VertexBufferLayout(std::initializer_list<VertexElement> elements);

    const Elements& getElements() const { return m_elements; }
    uint32_t getStride() const { return m_stride; }

    iterator begin() { return m_elements.begin(); }
    iterator end() { return m_elements.end(); }
    const_iterator begin() const { return m_elements.begin(); }
    const_iterator end() const { return m_elements.end(); }

private:
    void calculateOffsetsAndStride();

    Elements m_elements;
    uint32_t m_stride{};
};

class GE_API VertexBuffer: public NonCopyable
{
    virtual void bind() const = 0;

    virtual void setLayout(const VertexBufferLayout& layout) = 0;
    virtual const VertexBufferLayout& getLayout() const = 0;

    virtual void setVertices(const void* vertices, uint32_t size) = 0;

    static Scoped<VertexBuffer> create(const void* vertices, uint32_t count);
    static Scoped<VertexBuffer> create(uint32_t size);
};

} // namespace GE

#endif // GENESIS_RENDERER_VERTEX_BUFFER_H_
