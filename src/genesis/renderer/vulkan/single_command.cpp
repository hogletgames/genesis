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

#include "single_command.h"
#include "device.h"
#include "vulkan_exception.h"

#include "genesis/core/log.h"

namespace GE::Vulkan {

SingleCommand::SingleCommand(Shared<Device> device)
    : m_device{std::move(device)}
{
    VkCommandBufferAllocateInfo alloc_info{};
    alloc_info.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_ALLOCATE_INFO;
    alloc_info.level = VK_COMMAND_BUFFER_LEVEL_PRIMARY;
    alloc_info.commandPool = m_device->commandPool();
    alloc_info.commandBufferCount = 1;

    if (vkAllocateCommandBuffers(m_device->device(), &alloc_info, &m_cmd_buffer) !=
        VK_SUCCESS) {
        throw Vulkan::Exception{"Failed to allocate Single Command Buffer"};
    }

    VkCommandBufferBeginInfo begin_info{};
    begin_info.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_BEGIN_INFO;
    begin_info.flags = VK_COMMAND_BUFFER_USAGE_ONE_TIME_SUBMIT_BIT;

    if (vkBeginCommandBuffer(m_cmd_buffer, &begin_info) != VK_SUCCESS) {
        destroyVkHandles();
        throw Vulkan::Exception{"Failed to begin Single Command"};
    }
}

SingleCommand::~SingleCommand()
{
    if (vkEndCommandBuffer(m_cmd_buffer) != VK_SUCCESS) {
        GE_CORE_WARN("Failed to end Single Command");
        destroyVkHandles();
        return;
    }

    VkSubmitInfo submit_info{};
    submit_info.sType = VK_STRUCTURE_TYPE_SUBMIT_INFO;
    submit_info.commandBufferCount = 1;
    submit_info.pCommandBuffers = &m_cmd_buffer;

    if (vkQueueSubmit(m_device->graphicsQueue(), 1, &submit_info, VK_NULL_HANDLE) !=
        VK_SUCCESS) {
        GE_CORE_WARN("Failed to submit Single Command");
        destroyVkHandles();
        return;
    }

    vkQueueWaitIdle(m_device->graphicsQueue());
    destroyVkHandles();
}

void SingleCommand::destroyVkHandles()
{
    vkFreeCommandBuffers(m_device->device(), m_device->commandPool(), 1, &m_cmd_buffer);
    m_cmd_buffer = VK_NULL_HANDLE;
}

} // namespace GE::Vulkan
