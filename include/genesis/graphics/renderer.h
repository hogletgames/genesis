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

#pragma once

#include <genesis/core/interface.h>
#include <genesis/graphics/render_command.h>
#include <genesis/math/types.h>

namespace GE {

class Event;
class GPUCommandQueue;
class Pipeline;
class RenderCommand;
struct pipeline_config_t;

class GE_API Renderer: public Interface
{
public:
    enum ClearMode : uint8_t
    {
        CLEAR_NONE = 0,
        CLEAR_COLOR,
        CLEAR_DEPTH,
        CLEAR_ALL
    };

    virtual void draw(GPUCommandQueue* queue, uint32_t vertex_count, uint32_t instance_count,
                      uint32_t first_vertex, uint32_t first_instance) = 0;

    virtual bool beginFrame(ClearMode clear_mode = CLEAR_ALL) = 0;
    virtual void endFrame() = 0;
    virtual void swapBuffers() = 0;

    virtual void onEvent(Event* event) = 0;

    virtual Vec2 size() const = 0;
    virtual RenderCommand* command() = 0;

    virtual Scoped<Pipeline> createPipeline(const pipeline_config_t& config) = 0;
};

} // namespace GE
