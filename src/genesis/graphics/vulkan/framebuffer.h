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

// NOLINTNEXTLINE(llvm-header-guard)
#ifndef GENESIS_GRAPHICS_VULKAN_FRAMEBUFFER_H_
#define GENESIS_GRAPHICS_VULKAN_FRAMEBUFFER_H_

#include "texture.h"

#include <genesis/graphics/framebuffer.h>

#include <vulkan/vulkan.h>

namespace GE::Vulkan {

class Device;
class FramebufferRenderer;

class Framebuffer: public GE::Framebuffer
{
public:
    Framebuffer(Shared<Device> device, config_t config);
    ~Framebuffer();

    void resize(const Vec2& size) override;

    Renderer* renderer() override;
    const Vec2& size() const override { return m_config.size; }
    uint32_t MSSASamples() const override { return m_config.msaa_samples; }
    const Vec4& clearColor() const override { return m_config.clear_color; }
    float clearDepth() const override { return m_config.clear_depth; }

    const Vulkan::Texture& colorTexture() const override;
    const Vulkan::Texture& depthTexture() const override;
    uint32_t colorAttachmentCount() const override;
    bool hasDepthAttachment() const override;

    const std::vector<VkAttachmentDescription>& attachments() const
    {
        return m_attachments;
    }

    VkFramebuffer framebuffer() const { return m_framebuffer; }

private:
    void createAttachments();
    void createFramebuffer();

    void destroyVkHandles();

    VkAttachmentDescription createAttachment(const fb_attachment_t& config) const;
    Scoped<Vulkan::Texture> createTexture(TextureType type, TextureFormat format);

    Shared<Device> m_device;
    Scoped<FramebufferRenderer> m_renderer;
    config_t m_config;

    VkFramebuffer m_framebuffer{VK_NULL_HANDLE};
    std::vector<VkAttachmentDescription> m_attachments;
    std::vector<VkImageView> m_image_views;
    Scoped<Vulkan::Texture> m_color_texture;
    Scoped<Vulkan::Texture> m_depth_texture;
    uint32_t m_color_attachment_count{0};
};

} // namespace GE::Vulkan

#endif // GENESIS_GRAPHICS_VULKAN_FRAMEBUFFER_H_
