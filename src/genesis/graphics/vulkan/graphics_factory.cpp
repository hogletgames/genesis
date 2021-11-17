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

#include "graphics_factory.h"
#include "buffers/index_buffer.h"
#include "buffers/vertex_buffer.h"
#include "shader.h"
#include "shader_program.h"

namespace GE::Vulkan {

GraphicsFactory::GraphicsFactory(Shared<Device> device)
    : m_device{std::move(device)}
{}

Scoped<GE::IndexBuffer> GraphicsFactory::createIndexBuffer(const uint32_t *indices,
                                                           uint32_t count) const
{
    return tryMakeScoped<Vulkan::IndexBuffer>(m_device, indices, count);
}

Scoped<GE::VertexBuffer> GraphicsFactory::createVertexBuffer(const void *vertices,
                                                             uint32_t size) const

{
    return tryMakeScoped<Vulkan::VertexBuffer>(m_device, vertices, size);
}

Scoped<GE::VertexBuffer> GraphicsFactory::createVertexBuffer(uint32_t size) const
{
    return tryMakeScoped<Vulkan::VertexBuffer>(m_device, size);
}

Scoped<GE::Shader> GraphicsFactory::createShader(Shader::Type type)
{
    return tryMakeScoped<Vulkan::Shader>(m_device, type);
}

Scoped<GE::ShaderProgram> GraphicsFactory::createShaderProgram(Shared<GE::Shader> vert,
                                                               Shared<GE::Shader> frag)
{
    return tryMakeScoped<Vulkan::ShaderProgram>(m_device, std::move(vert),
                                                std::move(frag));
}

} // namespace GE::Vulkan
