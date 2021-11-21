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
#include "pipeline.h"
#include "vertex_buffer.h"

#include "genesis/gui/context.h"

namespace GE {

void RenderCommand::bind(Pipeline *pipeline)
{
    pipeline->bind(cmdQueue());
}

void RenderCommand::bind(VertexBuffer *buffer)
{
    buffer->bind(cmdQueue());
}

void RenderCommand::bind(IndexBuffer *buffer)
{
    buffer->bind(cmdQueue());
}

void RenderCommand::draw(VertexBuffer *buffer, uint32_t vertex_count)
{
    buffer->bind(cmdQueue());
    buffer->draw(cmdQueue(), vertex_count);
}

void RenderCommand::draw(GUI::Context *gui_layer)
{
    gui_layer->draw(cmdQueue());
}

void RenderCommand::submit(GPUCommandBuffer cmd)
{
    cmdQueue()->execCommands(cmd);
    cmdQueue()->clear();
}

} // namespace GE
