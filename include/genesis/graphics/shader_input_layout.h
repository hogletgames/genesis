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

#pragma once

#include <genesis/core/interface.h>

#include <deque>
#include <string>

namespace GE {

struct shader_attribute_t {
    enum class BaseType : uint8_t
    {
        NONE = 0,
        INT,
        UINT,
        FLOAT,
        DOUBLE
    };

    BaseType    base_type{BaseType::NONE};
    std::string name;
    uint32_t    location{};
    uint32_t    size{0};
    uint32_t    vec_size{0};
    uint32_t    vec_column{0};
    uint32_t    offset{0};

    uint32_t fullSize() const { return size * vec_size * vec_column; }
};

inline bool operator==(const GE::shader_attribute_t& lhs, const GE::shader_attribute_t& rhs)
{
    return lhs.base_type == rhs.base_type && lhs.name == rhs.name && lhs.size == rhs.size &&
           lhs.offset == rhs.offset && lhs.location == rhs.location;
}

class GE_API ShaderInputLayout
{
public:
    ShaderInputLayout() = default;
    explicit ShaderInputLayout(std::deque<shader_attribute_t> attributes);

    void append(const shader_attribute_t& attribute);
    void clear();

    const std::deque<shader_attribute_t>& attributes() const { return m_attributes; }
    uint32_t stride() const { return m_stride; }

private:
    std::deque<shader_attribute_t> m_attributes;
    uint32_t                       m_stride{0};
};

} // namespace GE
