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

#include "render_command.h"
#include "index_buffer.h"
#include "mesh.h"
#include "pipeline.h"
#include "renderer.h"
#include "vertex_buffer.h"

#include "genesis/gui/context.h"

namespace GE {

RenderCommand::RenderCommand(Renderer *renderer)
    : m_renderer{renderer}
{}

void RenderCommand::bind(Pipeline *pipeline)
{
    m_cmd_queue.setCurrentPipeline(pipeline);
}

void RenderCommand::bind(VertexBuffer *buffer)
{
    buffer->bind(&m_cmd_queue);
}

void RenderCommand::bind(IndexBuffer *buffer)
{
    buffer->bind(&m_cmd_queue);
}

void RenderCommand::bind(Pipeline *pipeline, const std::string &resource_name,
                         const UniformBuffer &buffer)
{
    pipeline->bind(&m_cmd_queue, resource_name, buffer);
}

void RenderCommand::bind(Pipeline *pipeline, const std::string &resource_name,
                         const Texture &texture)
{
    pipeline->bind(&m_cmd_queue, resource_name, texture);
}

void RenderCommand::draw(const Mesh &mesh)
{
    mesh.draw(&m_cmd_queue);
}

void RenderCommand::draw(VertexBuffer *buffer, uint32_t vertex_count)
{
    buffer->bind(&m_cmd_queue);
    buffer->draw(&m_cmd_queue, vertex_count);
}

void RenderCommand::draw(VertexBuffer *vbo, IndexBuffer *ibo)
{
    vbo->bind(&m_cmd_queue);
    ibo->bind(&m_cmd_queue);
    vbo->draw(&m_cmd_queue, ibo);
}

void RenderCommand::draw(GUI::Context *gui_layer)
{
    gui_layer->draw(&m_cmd_queue);
}

void RenderCommand::draw(uint32_t vertex_count, uint32_t instance_count, uint32_t first_vertex,
                         uint32_t first_instance)
{
    m_renderer->draw(&m_cmd_queue, vertex_count, instance_count, first_vertex, first_instance);
}

void RenderCommand::submit(GPUCommandBuffer cmd)
{
    m_cmd_queue.execCommands(cmd);
    m_cmd_queue.clear();
}

} // namespace GE
