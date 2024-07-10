/*
 * BSD 3-Clause License
 *
 * Copyright (c) 2023, Dmitry Shilnenkov
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

#include "pipeline_resource.h"
#include "assets_exception.h"
#include "resource_visitor.h"

#include "genesis/core/log.h"
#include "genesis/graphics/pipeline_config.h"
#include "genesis/graphics/renderer.h"
#include "genesis/graphics/shader.h"

namespace GE::Assets {

PipelineResource::PipelineResource(const ResourceID &id, std::string vertex_shader,
                                   std::string fragment_shader)
    : ResourceBase{id}
    , m_vertex_shader_path{std::move(vertex_shader)}
    , m_fragment_shader_path{std::move(fragment_shader)}
    , m_vertex_shader{Shader::create(Shader::Type::VERTEX)}
    , m_fragment_shader{Shader::create(Shader::Type::FRAGMENT)}
{
    if (!m_vertex_shader->compileFromFile(m_vertex_shader_path)) {
        throw Assets::Exception{"Failed to compile a vertex shader for a pipeline resource"};
    }

    if (!m_fragment_shader->compileFromFile(m_fragment_shader_path)) {
        throw Assets::Exception{"Failed to compile a fragment shaders for a pipeline resource"};
    }
}

void PipelineResource::accept(ResourceVisitor *visitor)
{
    visitor->visit(this);
}

bool PipelineResource::createPipeline(Renderer *renderer)
{
    pipeline_config_t config{m_vertex_shader, m_fragment_shader};

    if (m_pipeline = renderer->createPipeline(config); !m_pipeline) {
        GE_CORE_ERR("Failed to pipeline for pipeline resource");
        return false;
    }

    return true;
}

Scoped<PipelineResource> PipelineResource::create(const ResourceID &id,
                                                  const std::string &vertex_shader_path,
                                                  const std::string &fragment_shader_path)
{
    return tryMakeScoped<PipelineResource>(id, vertex_shader_path, fragment_shader_path);
}

} // namespace GE::Assets
