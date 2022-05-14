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
#include "vulkan_exception.h"

#include "genesis/core/asserts.h"

namespace {

constexpr VkSampleCountFlagBits toVkSampleCount(uint32_t sample_count)
{
    return static_cast<VkSampleCountFlagBits>(sample_count);
}

constexpr VkExtent3D toVkExtent(const GE::Vec2& size)
{
    return {static_cast<uint32_t>(size.x), static_cast<uint32_t>(size.y), 1};
}

constexpr VkImageUsageFlags toMSAAImageUsage(GE::TextureFormat format)
{
    constexpr auto default_usage{VK_IMAGE_USAGE_TRANSIENT_ATTACHMENT_BIT};
    return GE::isColorFormat(format)
               ? default_usage | VK_IMAGE_USAGE_COLOR_ATTACHMENT_BIT
               : default_usage | VK_IMAGE_USAGE_DEPTH_STENCIL_ATTACHMENT_BIT;
}

VkAttachmentDescription createColorAttachment(const GE::fb_attachment_t& config,
                                              uint32_t sample_count, bool is_msaa)
{
    VkAttachmentDescription attachment{};
    attachment.format = GE::Vulkan::toVkFormat(config.texture_format);
    attachment.loadOp = VK_ATTACHMENT_LOAD_OP_CLEAR;
    attachment.stencilLoadOp = VK_ATTACHMENT_LOAD_OP_DONT_CARE;
    attachment.stencilStoreOp = VK_ATTACHMENT_STORE_OP_DONT_CARE;
    attachment.initialLayout = VK_IMAGE_LAYOUT_UNDEFINED;

    if (is_msaa) {
        attachment.samples = toVkSampleCount(sample_count);
        attachment.storeOp = VK_ATTACHMENT_STORE_OP_DONT_CARE;
        attachment.finalLayout = VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL;
    } else {
        attachment.samples = VK_SAMPLE_COUNT_1_BIT;
        attachment.storeOp = VK_ATTACHMENT_STORE_OP_STORE;
        attachment.finalLayout = VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL;
    }

    if (!is_msaa && sample_count > 1) {
        attachment.loadOp = VK_ATTACHMENT_LOAD_OP_DONT_CARE;
    }

    return attachment;
}

VkAttachmentDescription createDepthAttachment(const GE::fb_attachment_t& config,
                                              uint32_t sample_count, bool is_msaa)
{
    VkAttachmentDescription attachment{};
    attachment.format = GE::Vulkan::toVkFormat(config.texture_format);
    attachment.loadOp = VK_ATTACHMENT_LOAD_OP_CLEAR;
    attachment.stencilLoadOp = VK_ATTACHMENT_LOAD_OP_CLEAR;
    attachment.initialLayout = VK_IMAGE_LAYOUT_UNDEFINED;
    attachment.finalLayout = VK_IMAGE_LAYOUT_DEPTH_STENCIL_ATTACHMENT_OPTIMAL;

    if (is_msaa) {
        attachment.samples = toVkSampleCount(sample_count);
        attachment.storeOp = VK_ATTACHMENT_STORE_OP_DONT_CARE;
        attachment.stencilStoreOp = VK_ATTACHMENT_STORE_OP_DONT_CARE;
    } else {
        attachment.samples = VK_SAMPLE_COUNT_1_BIT;
        attachment.storeOp = VK_ATTACHMENT_STORE_OP_STORE;
        attachment.stencilStoreOp = VK_ATTACHMENT_STORE_OP_STORE;
    }

    if (!is_msaa && sample_count > 1) {
        attachment.loadOp = VK_ATTACHMENT_LOAD_OP_DONT_CARE;
        attachment.stencilLoadOp = VK_ATTACHMENT_LOAD_OP_DONT_CARE;
    }

    return attachment;
}

VkAttachmentDescription createAttachment(const GE::fb_attachment_t& config,
                                         uint32_t sample_count, bool is_msaa = false)
{
    if (GE::isColorFormat(config.texture_format)) {
        return createColorAttachment(config, sample_count, is_msaa);
    }

    return createDepthAttachment(config, sample_count, is_msaa);
}

} // namespace

namespace GE::Vulkan {

Framebuffer::Framebuffer(Shared<Device> device, Framebuffer::config_t config)
    : m_device{std::move(device)}
    , m_config{std::move(config)}
{
    createAttachments();
    m_renderer = makeScoped<FramebufferRenderer>(m_device, this);
    createFramebuffer();
}

Framebuffer::~Framebuffer()
{
    m_device->waitIdle();
    destroyVkHandles();
    m_renderer.reset();
}

void Framebuffer::resize(const Vec2& size)
{
    if (m_config.size == size) {
        return;
    }

    m_renderer->swapBuffers();
    destroyVkHandles();

    m_config.size = size;
    createAttachments();
    createFramebuffer();
}

Renderer* Framebuffer::renderer()
{
    return m_renderer.get();
}

void Framebuffer::createResources(const fb_attachment_t& attachment_config)
{
    auto type = attachment_config.texture_type;
    auto format = attachment_config.texture_format;
    auto texture = createTexture(type, format);

    m_attachments.push_back(createAttachment(attachment_config, m_config.msaa_samples));
    m_image_views.push_back(texture->image()->view());

    if (isColorFormat(format)) {
        m_color_textures.push_back(std::move(texture));
        ++m_color_attachment_count;
    } else {
        GE_CORE_ASSERT(!m_depth_texture, "Framebuffer must have only one depth "
                                         "attachment");
        m_depth_texture = std::move(texture);
    }
}

void Framebuffer::createMSAAResources(const fb_attachment_t& attachment_config)
{
    auto type = attachment_config.texture_type;
    auto format = attachment_config.texture_format;
    auto image = createMSAAImage(type, format);

    m_attachments.push_back(
        createAttachment(attachment_config, m_config.msaa_samples, true));
    m_image_views.push_back(image->view());

    if (isColorFormat(format)) {
        m_color_msaa_images.push_back(std::move(image));
    } else {
        m_depth_msaa_image = std::move(image);
    }
}

void Framebuffer::createAttachments()
{
    for (const auto& config : m_config.attachments) {
        if (m_config.msaa_samples > 1) {
            createMSAAResources(config);
        }

        createResources(config);
    }
}

void Framebuffer::createFramebuffer()
{
    VkFramebufferCreateInfo create_info{};
    create_info.sType = VK_STRUCTURE_TYPE_FRAMEBUFFER_CREATE_INFO;
    create_info.renderPass = m_renderer->renderPass(Renderer::CLEAR_ALL);
    create_info.attachmentCount = m_image_views.size();
    create_info.pAttachments = m_image_views.data();
    create_info.width = static_cast<uint32_t>(m_config.size.x);
    create_info.height = static_cast<uint32_t>(m_config.size.y);
    create_info.layers = m_config.layers;

    if (vkCreateFramebuffer(m_device->device(), &create_info, nullptr, &m_framebuffer) !=
        VK_SUCCESS) {
        throw Vulkan::Exception{"Failed to create Framebuffer"};
    }
}

void Framebuffer::destroyVkHandles()
{
    m_attachments.clear();
    m_image_views.clear();
    m_color_msaa_images.clear();
    m_color_textures.clear();
    m_depth_texture.reset();

    vkDestroyFramebuffer(m_device->device(), m_framebuffer, nullptr);
    m_framebuffer = VK_NULL_HANDLE;
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
    config.samples = toVkSampleCount(m_config.msaa_samples);
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
    return m_color_attachment_count;
}

bool Framebuffer::hasDepthAttachment() const
{
    return m_depth_texture != nullptr;
}

} // namespace GE::Vulkan
