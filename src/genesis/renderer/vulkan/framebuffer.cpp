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

#include "framebuffer.h"
#include "device.h"

#include <stdexcept>

namespace GE::Vulkan {

Framebuffer::Framebuffer(GE::Shared<Device> device)
    : m_device{std::move(device)}
{
    VkFramebufferCreateInfo framebuffer_info{};
    framebuffer_info.sType = VK_STRUCTURE_TYPE_FRAMEBUFFER_CREATE_INFO;
    // framebuffer_info.renderPass = m_render_pass;
    // framebuffer_info.pAttachments = attachments.data();
    // framebuffer_info.attachmentCount = attachments.size();
    // framebuffer_info.width = m_extent.width;
    // framebuffer_info.height = m_extent.height;
    framebuffer_info.layers = 1;

    if (vkCreateFramebuffer(m_device->getDevice(), &framebuffer_info, nullptr,
                            &m_framebuffer) != VK_SUCCESS) {
        throw std::runtime_error{"Failed to create VkFramebuffer"};
    }
}

Framebuffer::~Framebuffer()
{
    if (m_framebuffer != VK_NULL_HANDLE) {
        vkDestroyFramebuffer(m_device->getDevice(), m_framebuffer, nullptr);
    }
}

} // namespace GE::Vulkan
