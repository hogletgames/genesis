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
#include <genesis/graphics/pipeline_config.h>

namespace GE {
class Pipeline;
class Renderer;
class Shader;
} // namespace GE

namespace GE::Assets {

class Package;

class GE_API PipelineResource: public ResourceBase
{
public:
    class Factory;

    struct config_t {
        std::string name;
        std::string vertex_shader_path;
        std::string fragment_shader_path;
    };

    const std::string& fragmentShaderPath() const { return m_fragment_shader_path; }
    const std::string& vertexShaderPath() const { return m_vertex_shader_path; }

    Scoped<Pipeline> createPipeline(GE::Renderer* renderer, pipeline_config_t config = {}) const;

    static constexpr Group GROUP{Group::PIPELINES};

private:
    PipelineResource(const std::string& package, const config_t& config);

    std::string m_vertex_shader_path;
    std::string m_fragment_shader_path;

    Shared<Shader> m_vertex_shader;
    Shared<Shader> m_fragment_shader;
};

class PipelineResource::Factory
{
    friend Package;
    static Shared<PipelineResource> create(const std::string& package, const config_t& config);
};

} // namespace GE::Assets
