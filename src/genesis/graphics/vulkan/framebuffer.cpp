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
#include "image.h"
#include "renderers/framebuffer_renderer.h"
#include "texture.h"
#include "utils.h"

#include "genesis/core/asserts.h"

namespace GE::Vulkan {
namespace {

constexpr VkExtent3D toVkExtent(const Vec2& size)
{
    return {static_cast<uint32_t>(size.x), static_cast<uint32_t>(size.y), 1};
}

constexpr VkImageUsageFlags toMSAAImageUsage(TextureFormat format)
{
    constexpr auto default_usage{VK_IMAGE_USAGE_TRANSIENT_ATTACHMENT_BIT};
    return isColorFormat(format) ? default_usage | VK_IMAGE_USAGE_COLOR_ATTACHMENT_BIT
                                 : default_usage | VK_IMAGE_USAGE_DEPTH_STENCIL_ATTACHMENT_BIT;
}

} // namespace

Framebuffer::Framebuffer(Shared<Device> device, Framebuffer::config_t config)
    : m_device{std::move(device)}
    , m_config{std::move(config)}
{
    createAttachments();
    m_renderer = makeScoped<FramebufferRenderer>(m_device, this);
}

Framebuffer::~Framebuffer()
{
    m_device->waitIdle();
}

void Framebuffer::resize(const Vec2& size)
{
    if (m_config.size == size) {
        return;
    }

    clearResources();

    m_config.size = size;
    createAttachments();
}

Renderer* Framebuffer::renderer()
{
    return m_renderer.get();
}

void Framebuffer::createMSAAResources(const fb_attachment_t& attachment_config)
{
    auto type = attachment_config.texture_type;
    auto format = attachment_config.texture_format;
    auto image = createMSAAImage(type, format);

    if (isColorFormat(format)) {
        m_color_msaa_images.push_back(std::move(image));
    } else {
        m_depth_msaa_image = std::move(image);
    }
}

void Framebuffer::createAttachments()
{
    for (const auto& attachment : m_config.attachments) {
        if (m_config.msaa_samples > 1) {
            createMSAAResources(attachment);
        }

        auto texture = createTexture(attachment.texture_type, attachment.texture_format);

        if (attachment.type == fb_attachment_t::Type::COLOR) {
            m_color_rendering_attachments.push_back(createColorRenderingAttachment(texture));
            m_color_textures.push_back(std::move(texture));
        } else {
            GE_CORE_ASSERT(!m_depth_texture, "Framebuffer must have only one depth "
                                             "attachment");
            m_depth_rendering_attachment = createDepthRenderingAttachment(texture);
            m_depth_texture = std::move(texture);
        }
    }
}

VkRenderingAttachmentInfo
Framebuffer::createColorRenderingAttachment(const Scoped<Vulkan::Texture>& texture)
{
    VkRenderingAttachmentInfo attachment{};
    attachment.sType = VK_STRUCTURE_TYPE_RENDERING_ATTACHMENT_INFO;
    attachment.imageLayout = VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL;
    attachment.loadOp = VK_ATTACHMENT_LOAD_OP_DONT_CARE;
    attachment.storeOp = VK_ATTACHMENT_STORE_OP_STORE;
    attachment.clearValue = toVkClearColorValue(m_config.clear_color);

    if (m_config.msaa_samples > 1) {
        attachment.imageView = m_color_msaa_images.back()->view();
        attachment.resolveImageView = texture->image()->view();
        attachment.resolveImageLayout = VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL;
        attachment.resolveMode = VK_RESOLVE_MODE_AVERAGE_BIT;
    } else {
        attachment.imageView = texture->image()->view();
    }

    return attachment;
}

VkRenderingAttachmentInfo
Framebuffer::createDepthRenderingAttachment(const Scoped<Vulkan::Texture>& texture)
{
    VkRenderingAttachmentInfo attachment{};
    attachment.sType = VK_STRUCTURE_TYPE_RENDERING_ATTACHMENT_INFO;
    attachment.imageLayout = VK_IMAGE_LAYOUT_DEPTH_STENCIL_ATTACHMENT_OPTIMAL;
    attachment.loadOp = VK_ATTACHMENT_LOAD_OP_DONT_CARE;
    attachment.storeOp = VK_ATTACHMENT_STORE_OP_DONT_CARE;
    attachment.clearValue = toVkClearDepthStencilValue(m_config.clear_depth);

    if (m_config.msaa_samples > 1) {
        attachment.imageView = m_depth_msaa_image->view();
        attachment.resolveImageView = texture->image()->view();
        attachment.resolveImageLayout = VK_IMAGE_LAYOUT_DEPTH_STENCIL_ATTACHMENT_OPTIMAL;
        attachment.resolveMode = VK_RESOLVE_MODE_SAMPLE_ZERO_BIT;
    } else {
        attachment.imageView = texture->image()->view();
    }

    return attachment;
}

Scoped<Vulkan::Texture> Framebuffer::createTexture(TextureType type, TextureFormat format)
{
    texture_config_t config{};
    config.type = type;
    config.format = format;
    config.width = static_cast<uint32_t>(m_config.size.x);
    config.height = static_cast<uint32_t>(m_config.size.y);
    config.layers = m_config.layers;
    config.mip_levels = 1;

    if (isColorFormat(format)) {
        config.wrap_u = TextureWrap::CLAMP_TO_EDGE;
        config.wrap_v = TextureWrap::CLAMP_TO_EDGE;
    } else {
        config.wrap_u = TextureWrap::CLAMP_TO_BORDER;
        config.wrap_v = TextureWrap::CLAMP_TO_BORDER;
    }

    return makeScoped<Vulkan::Texture2D>(m_device, config);
}

Scoped<Image> Framebuffer::createMSAAImage(TextureType type, TextureFormat format)
{
    image_config_t config{};
    config.view_type = toVkImageViewType(type);
    config.extent = toVkExtent(m_config.size);
    config.mip_levels = 1;
    config.layers = m_config.layers;
    config.samples = toVkSampleCountFlag(m_config.msaa_samples);
    config.format = toVkFormat(format);
    config.tiling = VK_IMAGE_TILING_OPTIMAL;
    config.usage = toMSAAImageUsage(format);
    config.memory_properties = VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT;
    config.aspect_mask = toVkImageAspect(format);

    return makeScoped<Image>(m_device, config);
}

const Vulkan::Texture& Framebuffer::colorTexture(size_t i) const
{
    return *m_color_textures[i];
}

const Vulkan::Texture& Framebuffer::depthTexture() const
{
    return *m_depth_texture;
}

uint32_t Framebuffer::colorAttachmentCount() const
{
    return m_color_textures.size();
}

bool Framebuffer::hasDepthAttachment() const
{
    return m_depth_texture != nullptr;
}

const std::vector<VkRenderingAttachmentInfo>&
Framebuffer::colorRenderingAttachments(Renderer::ClearMode clear_mode)
{
    bool should_clear = clear_mode == Renderer::CLEAR_COLOR || clear_mode == Renderer::CLEAR_ALL;

    for (uint32_t i{0}; i < m_color_rendering_attachments.size(); i++) {
        auto& color_attachment = m_color_rendering_attachments[i];
        color_attachment.loadOp = should_clear ? VK_ATTACHMENT_LOAD_OP_CLEAR
                                               : VK_ATTACHMENT_LOAD_OP_DONT_CARE;
    }

    return m_color_rendering_attachments;
}

const VkRenderingAttachmentInfo&
Framebuffer::depthRenderingAttachment(Renderer::ClearMode clear_mode)
{
    bool should_clear = clear_mode == Renderer::CLEAR_DEPTH || clear_mode == Renderer::CLEAR_ALL;
    m_depth_rendering_attachment.loadOp = should_clear ? VK_ATTACHMENT_LOAD_OP_CLEAR
                                                       : VK_ATTACHMENT_LOAD_OP_DONT_CARE;
    return m_depth_rendering_attachment;
}

void Framebuffer::clearResources()
{
    m_color_textures.clear();
    m_color_msaa_images.clear();
    m_color_rendering_attachments.clear();

    m_depth_texture.reset();
    m_depth_msaa_image.reset();
    m_depth_rendering_attachment = {};
}

} // namespace GE::Vulkan
