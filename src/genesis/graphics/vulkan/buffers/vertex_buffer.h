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

#include "buffer_base.h"

#include <genesis/graphics/vertex_buffer.h>

#include <vulkan/vulkan.h>

namespace GE::Vulkan {

class Device;

class VertexBuffer: public GE::VertexBuffer, public BufferBase
{
public:
    VertexBuffer(Shared<Device> device, uint32_t size, const void* vertices);

    void bind(GPUCommandQueue* queue) const override;
    void draw(GPUCommandQueue* queue, uint32_t vertex_count) const override;
    void draw(GPUCommandQueue* queue, GE::IndexBuffer* ibo) const override;

    NativeHandle nativeHandle() const override { return buffer(); }
    uint32_t size() const override { return m_size; }

    void setVertices(const void* vertices, uint32_t size) override;
};

} // namespace GE::Vulkan
