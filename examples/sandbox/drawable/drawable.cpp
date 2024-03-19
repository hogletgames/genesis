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

#include "drawable.h"

#include "genesis/core.h"
#include "genesis/graphics.h"

namespace GE::Examples {

Drawable::~Drawable() = default;

Drawable::Drawable(Renderer *renderer, const std::string &vert_shader,
                   const std::string &frag_shader)
{
    auto vert = Shader::create(Shader::Type::VERTEX);
    GE_ASSERT(vert->compileFromFile(vert_shader), "Failed to compile vertex shader");

    auto frag = Shader::create(Shader::Type::FRAGMENT);
    GE_ASSERT(frag->compileFromFile(frag_shader), "Failed to compile fragment shader");

    pipeline_config_t pipeline_config{};
    pipeline_config.vertex_shader = std::move(vert);
    pipeline_config.fragment_shader = std::move(frag);

    m_pipeline = renderer->createPipeline(pipeline_config);
    GE_ASSERT(m_pipeline, "Failed to create pipeline");

    m_mpv = UniformBuffer::create(sizeof(mvp_t), nullptr);
}

void Drawable::bind(Renderer *renderer, const mvp_t &mvp)
{
    m_mpv->setData(sizeof(mvp_t), &mvp);

    renderer->command()->bind(m_pipeline.get());
    renderer->command()->bind(m_pipeline.get(), "MVP", *m_mpv);
}

} // namespace GE::Examples
