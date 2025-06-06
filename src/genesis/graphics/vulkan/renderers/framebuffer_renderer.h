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

#include "renderer_base.h"

namespace GE::Vulkan {

class Device;
class Framebuffer;

class FramebufferRenderer: public RendererBase
{
public:
    FramebufferRenderer(Shared<Device> device, Vulkan::Framebuffer* framebuffer);
    ~FramebufferRenderer();

    bool beginFrame(ClearMode clear_mode = CLEAR_ALL) override;
    void endFrame() override;
    void swapBuffers() override;

    void onEvent([[maybe_unused]] Event* event) override {};

    Vec2 size() const override;

    Scoped<GE::Pipeline> createPipeline(const pipeline_config_t& config) override;

private:
    void createSyncObjects();
    void destroyVkHandles();

    bool submit();

    void transitImageLayoutBeforeRendering(VkCommandBuffer cmd) override;
    void transitImageLayoutAfterRendering(VkCommandBuffer cmd) override;

    VkCommandBuffer cmdBuffer() const override;
    VkExtent2D extent() const override;
    VkViewport viewport() const override;

    const std::vector<VkRenderingAttachmentInfo>&
    colorRenderingAttachments(ClearMode clear_mode) override;
    std::optional<VkRenderingAttachmentInfo>
    depthRenderingAttachment(ClearMode clear_mode) override;

    Vulkan::Framebuffer* m_framebuffer{nullptr};
    VkFence              m_in_flight_fence{VK_NULL_HANDLE};
};

} // namespace GE::Vulkan
