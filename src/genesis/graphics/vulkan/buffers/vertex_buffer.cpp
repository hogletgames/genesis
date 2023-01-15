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

#include "buffers/vertex_buffer.h"
#include "buffers/staging_buffer.h"
#include "command_buffer.h"
#include "device.h"
#include "vulkan_exception.h"

#include "genesis/core/asserts.h"
#include "genesis/graphics/gpu_command_queue.h"

namespace GE::Vulkan {

VertexBuffer::VertexBuffer(Shared<Device> device, const void *vertices, uint32_t size)
    : BufferBase{std::move(device)}
    , m_size(size)
{
    VkBufferUsageFlags usage = VK_BUFFER_USAGE_TRANSFER_DST_BIT | VK_BUFFER_USAGE_VERTEX_BUFFER_BIT;
    VkMemoryPropertyFlagBits properties = VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT;
    createBuffer(size, usage, properties);

    if (vertices != nullptr) {
        setVertices(vertices, size);
    }
}

VertexBuffer::VertexBuffer(Shared<Device> device, uint32_t size)
    : VertexBuffer{std::move(device), nullptr, size}
{}

void VertexBuffer::bind(GPUCommandQueue *queue) const
{
    queue->enqueue([this](void *cmd) {
        constexpr VkDeviceSize offsets{0};
        vkCmdBindVertexBuffers(cmdBuffer(cmd), 0, 1, &m_buffer, &offsets);
    });
}

void VertexBuffer::draw(GPUCommandQueue *queue, uint32_t vertex_count) const
{
    queue->enqueue([vertex_count](void *cmd) { vkCmdDraw(cmdBuffer(cmd), vertex_count, 1, 0, 0); });
}

void VertexBuffer::setVertices(const void *vertices, uint32_t size)
{
    GE_ASSERT(m_size >= size, "Vertex Buffer overflow");
    StagingBuffer staging_buffer{m_device, vertices, size};
    staging_buffer.copyTo(this);
}

} // namespace GE::Vulkan
