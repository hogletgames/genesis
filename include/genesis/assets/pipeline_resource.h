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

#pragma once

#include <genesis/assets/resource_base.h>
#include <genesis/core/memory.h>
#include <genesis/graphics/pipeline.h>

namespace GE {
class Renderer;
} // namespace GE

namespace GE::Assets {

class GE_API PipelineResource: public ResourceBase
{
public:
    PipelineResource(const ResourceID& id, std::string vertex_shader, std::string fragment_shader);

    void accept(ResourceVisitor* visitor) override;

    bool createPipeline(Renderer* renderer);

    const std::string& fragmentShader() const { return m_fragment_shader_path; }
    const std::string& vertexShader() const { return m_vertex_shader_path; }
    const Shared<Pipeline>& pipeline() const { return m_pipeline; }

    static Scoped<PipelineResource> create(const ResourceID& id,
                                           const std::string& vertex_shader_path,
                                           const std::string& fragment_shader_path);

private:
    std::string m_vertex_shader_path;
    std::string m_fragment_shader_path;

    Shared<Shader> m_vertex_shader;
    Shared<Shader> m_fragment_shader;
    Shared<Pipeline> m_pipeline;
};

} // namespace GE::Assets
