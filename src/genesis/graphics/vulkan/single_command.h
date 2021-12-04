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

// NOLINTNEXTLINE(llvm-header-guard)
#ifndef GENESIS_GRAPHICS_VULKAN_SINGLE_COMMAND_H_
#define GENESIS_GRAPHICS_VULKAN_SINGLE_COMMAND_H_

#include <genesis/core/interface.h>
#include <genesis/core/memory.h>

#include <vulkan/vulkan.h>

namespace GE::Vulkan {

class Device;

class SingleCommand: public NonCopyable
{
public:
    enum QueueFamily : uint8_t
    {
        QUEUE_UNKNOWN = 0,
        QUEUE_GRAPHICS,
        QUEUE_TRANSFER,
        QUEUE_COMPUTE
    };

    explicit SingleCommand(Shared<Device> device,
                           QueueFamily queue_family = QUEUE_GRAPHICS);
    ~SingleCommand();

    VkCommandBuffer getCmdBuffer() { return m_cmd_buffer; }

private:
    void destroyVkHandles();

    VkQueue getQueue(QueueFamily family);

    Shared<Device> m_device;
    QueueFamily m_queue_family{QUEUE_UNKNOWN};
    VkCommandBuffer m_cmd_buffer{VK_NULL_HANDLE};
};

} // namespace GE::Vulkan

#endif // GENESIS_GRAPHICS_VULKAN_SINGLE_COMMAND_H_
