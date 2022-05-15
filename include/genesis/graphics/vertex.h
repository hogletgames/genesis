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

#pragma once

#include "genesis/core/hash.h"
#include <genesis/math/types.h>

namespace GE {

struct vertex_t {
    GE::Vec3 position{0.0f, 0.0f, 0.0f};
    GE::Vec3 color{0.0f, 0.0f, 0.0f};
    GE::Vec2 tex_coord{0.0f, 0.0f};
};

constexpr bool operator==(const vertex_t& lhs, const vertex_t& rhs)
{
    return std::tie(lhs.position, lhs.color, lhs.tex_coord) ==
           std::tie(rhs.position, rhs.color, rhs.tex_coord);
}

constexpr bool operator!=(const vertex_t& lhs, const vertex_t& rhs)
{
    return !(lhs == rhs);
}

} // namespace GE

template<>
struct std::hash<GE::vertex_t> {
    size_t operator()(const GE::vertex_t& vertex) const
    {
        return GE::combinedHash(vertex.position, vertex.color, vertex.tex_coord);
    }
};
