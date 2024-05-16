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

#include "buffers/staging_buffer.h"
#include "device.h"
#include "image.h"
#include "single_command.h"
#include "texture.h"

#include "genesis/core/asserts.h"
#include "genesis/graphics/texture.h"

#include <cstring>

namespace GE::Vulkan {
namespace {

constexpr VkBufferUsageFlags USAGE{VK_BUFFER_USAGE_TRANSFER_SRC_BIT |
                                   VK_BUFFER_USAGE_TRANSFER_DST_BIT};
constexpr VkMemoryPropertyFlags PROPERTIES{VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT |
                                           VK_MEMORY_PROPERTY_HOST_COHERENT_BIT};

} // namespace

StagingBuffer::StagingBuffer(Shared<Device> device)
    : BufferBase{std::move(device)}
{}

StagingBuffer::StagingBuffer(Shared<Device> device, uint32_t size, const void *data)
    : BufferBase{std::move(device)}
{
    createBuffer(size, USAGE, PROPERTIES);
    copyData(size, data, 0);
}

void StagingBuffer::copyFrom(const GE::Texture &texture)
{
    uint32_t texture_size = toTextureSize(texture.size(), texture.format());

    if (texture_size == 0) {
        return;
    }

    if (m_size < texture_size) {
        clearStagingBuffer();
        createBuffer(texture_size, USAGE, PROPERTIES);
    }

    toVulkan(texture).image()->copyTo(*this);
}

void *StagingBuffer::data()
{
    if (m_size == 0) {
        return nullptr;
    }

    if (m_mapped_memory != nullptr) {
        return m_mapped_memory;
    }

    vkMapMemory(m_device->device(), m_memory, 0, m_size, 0, &m_mapped_memory);
    return m_mapped_memory;
}

void StagingBuffer::copyTo(BufferBase *dest, uint32_t offset)
{
    GE_CORE_ASSERT(m_size > 0, "Trying to copy empty buffer");

    SingleCommand cmd{m_device};
    VkBufferCopy copy_region{};
    copy_region.srcOffset = 0;
    copy_region.dstOffset = offset;
    copy_region.size = m_size;
    vkCmdCopyBuffer(cmd.buffer(), m_buffer, dest->buffer(), 1, &copy_region);
}

void StagingBuffer::copyData(uint32_t size, const void *data, uint32_t offset)
{
    void *mem_ptr{nullptr};
    vkMapMemory(m_device->device(), m_memory, offset, size, 0, &mem_ptr);
    std::memcpy(mem_ptr, data, size);
    vkUnmapMemory(m_device->device(), m_memory);
}

void StagingBuffer::clearStagingBuffer()
{
    if (m_mapped_memory != nullptr) {
        vkUnmapMemory(m_device->device(), m_memory);
        m_mapped_memory = nullptr;
    }

    clear();
}

} // namespace GE::Vulkan
