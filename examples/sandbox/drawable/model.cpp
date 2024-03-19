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

#include "model.h"

#include "genesis/core.h"
#include "genesis/graphics.h"

namespace {

constexpr auto VERTEX_SHADER{"examples/sandbox/assets/shaders/model_shader.vert"};
constexpr auto FRAGMENT_SHADER{"examples/sandbox/assets/shaders/model_shader.frag"};

} // namespace

namespace GE::Examples {

Model::Model(Renderer* renderer, std::string_view model, std::string_view texture)
    : Drawable{renderer, VERTEX_SHADER, FRAGMENT_SHADER}
{
    GE_ASSERT(m_mesh.fromObj(model), "Failed to load mesh");

    m_texture = GE::TextureLoader{texture.data()}.load();
    GE_ASSERT(m_texture, "Failed to load texture");
}

void Model::draw(Renderer* renderer, const mvp_t& mvp)
{
    bind(renderer, mvp);
    renderer->command()->bind(m_pipeline.get(), "u_Texture", *m_texture);
    renderer->command()->draw(m_mesh);
}

} // namespace GE::Examples
