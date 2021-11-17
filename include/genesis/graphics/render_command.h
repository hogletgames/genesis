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

#ifndef GENESIS_GRAPHICS_RENDER_COMMAND_H_
#define GENESIS_GRAPHICS_RENDER_COMMAND_H_

#include <genesis/core/interface.h>
#include <genesis/core/memory.h>
#include <genesis/graphics/gpu_command_queue.h>
#include <genesis/gui/context.h>
#include <genesis/math/types.h>

namespace GE {

class IndexBuffer;
class RenderContext;
class ShaderProgram;
class VertexBuffer;

class GE_API RenderCommand
{
public:
    static void bind(ShaderProgram* shader_program);
    static void bind(VertexBuffer* buffer);
    static void bind(IndexBuffer* buffer);

    static void draw(VertexBuffer* buffer, uint32_t vertex_count);
    static void draw(GUI::Context* gui_layer);

    static void submit(GPUCommandBuffer cmd);

private:
    RenderCommand() = default;

    static RenderCommand* get()
    {
        static RenderCommand instance;
        return &instance;
    }

    static GPUCommandQueue* cmdQueue() { return &get()->m_cmd_queue; }

    GPUCommandQueue m_cmd_queue;
};

} // namespace GE

#endif // GENESIS_GRAPHICS_RENDER_COMMAND_H_
