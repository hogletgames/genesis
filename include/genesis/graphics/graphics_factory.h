/*
 * BSD 3-Clause License
 *
 * Copyright (c) 2021-2022, Dmitry Shilnenkov
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
#include <genesis/graphics/framebuffer.h>
#include <genesis/graphics/shader.h>

namespace GE {

class IndexBuffer;
class VertexBuffer;
class Texture;
class StagingBuffer;
class UniformBuffer;

struct texture_config_t;

class GE_API GraphicsFactory: public Interface
{
public:
    virtual Scoped<Framebuffer> createFramebuffer(const Framebuffer::config_t& config) const = 0;

    virtual Scoped<IndexBuffer> createIndexBuffer(const uint32_t* indices,
                                                  uint32_t        count) const = 0;
    virtual Scoped<VertexBuffer> createVertexBuffer(uint32_t size, const void* vertices) const = 0;
    virtual Scoped<StagingBuffer> createStagingBuffer() const = 0;
    virtual Scoped<UniformBuffer> createUniformBuffer(uint32_t size, const void* data) const = 0;

    virtual Scoped<Shader> createShader(Shader::Type type) = 0;

    virtual Scoped<Texture> createTexture(const texture_config_t& config) = 0;
};

} // namespace GE
