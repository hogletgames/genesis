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

#include "cube.h"

namespace GE::Examples {

Cube::Cube(Renderer *renderer)
    : Shape(renderer)
{
    static const std::vector<vertex_t> VERTICES = {
        // front
        {{-0.5f, 0.5f, 0.5f}, {1.0f, 0.0f, 0.0f}},  // 0
        {{-0.5f, -0.5f, 0.5f}, {0.0f, 1.0f, 0.0f}}, // 1
        {{0.5f, -0.5f, 0.5f}, {0.0f, 0.0f, 1.0f}},  // 2
        {{0.5f, 0.5f, 0.5f}, {0.0f, 1.0f, 0.0f}},   // 3
        // left
        {{-0.5f, -0.5f, -0.5f}, {0.0f, 0.0f, 1.0f}}, // 4
        {{-0.5f, 0.5f, -0.5f}, {0.0f, 1.0f, 0.0f}},  // 5
        // bottom
        {{0.5f, -0.5f, -0.5f}, {1.0f, 0.0f, 0.0f}}, // 6
        // right
        {{0.5f, 0.5f, -0.5f}, {0.0f, 0.0f, 1.0f}}, // 7
    };

    static const std::vector<uint32_t> INDICES = {
        // front
        0, 1, 2, 2, 3, 0,
        // left
        4, 1, 5, 0, 5, 1,
        // bottom
        1, 4, 2, 2, 4, 6,
        // right
        3, 2, 7, 7, 2, 6,
        // top
        5, 0, 3, 3, 7, 5,
        // back
        7, 6, 4, 4, 5, 7,
        //
    };

    setVertices(VERTICES);
    setIndices(INDICES);
}

} // namespace GE::Examples
