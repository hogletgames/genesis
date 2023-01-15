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

#include <genesis/core/memory.h>
#include <genesis/graphics/renderer.h>

#include <vulkan/vulkan.h>

#include <vector>

namespace GE::Vulkan {

class Device;

class RendererBase: public GE::Renderer
{
public:
    ~RendererBase();

    RenderCommand* command() override { return &m_render_command; }

protected:
    explicit RendererBase(Shared<Device> device);

    VkRenderPass createRenderPass(const std::vector<VkAttachmentDescription>& descriptions,
                                  bool is_multisampled = false);
    void createCommandPool();
    void createCommandBuffers(uint32_t count);
    void createPipelineCache();

    bool beginRenderPass(ClearMode clear_mode);
    void updateDynamicState();
    bool endRenderPass();

    virtual VkCommandBuffer cmdBuffer() const = 0;
    virtual VkFramebuffer currentFramebuffer() const = 0;
    virtual VkExtent2D extent() const = 0;
    virtual VkViewport viewport() const = 0;

    Shared<Device> m_device;

    VkPipelineCache m_pipeline_cache{VK_NULL_HANDLE};
    VkCommandPool m_command_pool{VK_NULL_HANDLE};
    std::array<VkRenderPass, 4> m_render_passes{VK_NULL_HANDLE};
    std::vector<VkCommandBuffer> m_cmd_buffers;
    std::vector<VkClearValue> m_clear_values;

    RenderCommand m_render_command;

private:
    void destroyVkHandles();
};

} // namespace GE::Vulkan
