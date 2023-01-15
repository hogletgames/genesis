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

#include "buffer_base.h"
#include "device.h"
#include "vulkan_exception.h"

#include "genesis/core/asserts.h"

namespace GE::Vulkan {

BufferBase::BufferBase(Shared<Device> device)
    : m_device{std::move(device)}
{}

BufferBase::~BufferBase()
{
    destroyVkHandles();
}

void BufferBase::createBuffer(uint32_t size, VkBufferUsageFlags usage,
                              VkMemoryPropertyFlags properties)
{
    GE_ASSERT(m_buffer == VK_NULL_HANDLE, "Buffer has already been allocated");

    VkBufferCreateInfo buffer_info{};
    buffer_info.sType = VK_STRUCTURE_TYPE_BUFFER_CREATE_INFO;
    buffer_info.size = size;
    buffer_info.usage = usage;
    buffer_info.sharingMode = VK_SHARING_MODE_EXCLUSIVE;

    if (vkCreateBuffer(m_device->device(), &buffer_info, nullptr, &m_buffer) != VK_SUCCESS) {
        throw Vulkan::Exception{"Failed to create Buffer"};
    }

    VkMemoryRequirements mem_requirements{};
    vkGetBufferMemoryRequirements(m_device->device(), m_buffer, &mem_requirements);

    VkMemoryAllocateInfo alloc_info{};
    alloc_info.sType = VK_STRUCTURE_TYPE_MEMORY_ALLOCATE_INFO;
    alloc_info.allocationSize = mem_requirements.size;
    alloc_info.memoryTypeIndex =
        m_device->findMemoryType(mem_requirements.memoryTypeBits, properties);

    if (vkAllocateMemory(m_device->device(), &alloc_info, nullptr, &m_memory) != VK_SUCCESS) {
        throw Vulkan::Exception{"Failed to allocate Buffer Memory"};
    }

    vkBindBufferMemory(m_device->device(), m_buffer, m_memory, 0);
}

void BufferBase::destroyVkHandles()
{
    vkDestroyBuffer(m_device->device(), m_buffer, nullptr);
    m_buffer = VK_NULL_HANDLE;

    vkFreeMemory(m_device->device(), m_memory, nullptr);
    m_memory = VK_NULL_HANDLE;
}

} // namespace GE::Vulkan