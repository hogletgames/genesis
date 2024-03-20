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
#include <genesis/core/memory.h>
#include <genesis/graphics/gpu_command_queue.h>
#include <genesis/graphics/pipeline.h>

namespace GE {

namespace GUI {
class Context;
} // namespace GUI

class IndexBuffer;
class Mesh;
class Renderer;
class Texture;
class UniformBuffer;
class VertexBuffer;

class GE_API RenderCommand
{
public:
    explicit RenderCommand(Renderer* renderer);

    void bind(Pipeline* pipeline);
    void bind(VertexBuffer* buffer);
    void bind(IndexBuffer* buffer);
    void bind(Pipeline* pipeline, const std::string& resource_name, const UniformBuffer& buffer);
    void bind(Pipeline* pipeline, const std::string& resource_name, const Texture& texture);
    template<typename T>
    void pushConstant(Pipeline* pipeline, const std::string& name, const T& value);

    void draw(const Mesh& mesh);
    void draw(VertexBuffer* buffer, uint32_t vertex_count);
    void draw(VertexBuffer* vbo, IndexBuffer* ibo);
    void draw(GUI::Context* gui_layer);
    void draw(uint32_t vertex_count, uint32_t instance_count, uint32_t first_vertex,
              uint32_t first_instance);

    void submit(GPUCommandBuffer cmd);

private:
    Renderer* m_renderer{nullptr};
    GPUCommandQueue m_cmd_queue;
};

template<class T>
void RenderCommand::pushConstant(Pipeline* pipeline, const std::string& name, const T& value)
{
    pipeline->pushConstant(&m_cmd_queue, name, value);
}

} // namespace GE
