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

void Framebuffer::createAttachments()
{
    for (const auto& attachment_config : m_config.attachments) {
        m_attachments.push_back(createAttachment(attachment_config));

        auto type = attachment_config.texture_type;
        auto format = attachment_config.texture_format;

        if (isColorFormat(format)) {
            m_color_texture = createTexture(type, format);
            m_image_views.push_back(m_color_texture->image()->view());
            ++m_color_attachment_count;
        } else if (isDepthFormat(format)) {
            GE_CORE_ASSERT(!m_depth_texture,
                           "Framebuffer must have only one depth attachment");
            m_depth_texture = createTexture(type, format);
            m_image_views.push_back(m_depth_texture->image()->view());
        }
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
    m_color_texture.reset();
    m_depth_texture.reset();

    vkDestroyFramebuffer(m_device->device(), m_framebuffer, nullptr);
    m_framebuffer = VK_NULL_HANDLE;
}

VkAttachmentDescription Framebuffer::createAttachment(const fb_attachment_t& config) const
{
    VkAttachmentDescription attachment{};
    attachment.format = toVkFormat(config.texture_format);
    attachment.samples = toVkSampleCount(m_config.msaa_samples);
    attachment.loadOp = VK_ATTACHMENT_LOAD_OP_CLEAR;
    attachment.storeOp = VK_ATTACHMENT_STORE_OP_STORE;
    attachment.initialLayout = VK_IMAGE_LAYOUT_UNDEFINED;

    if (isColorFormat(config.texture_format)) {
        attachment.stencilLoadOp = VK_ATTACHMENT_LOAD_OP_DONT_CARE;
        attachment.stencilStoreOp = VK_ATTACHMENT_STORE_OP_DONT_CARE;
        attachment.finalLayout = VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL;
    } else {
        attachment.stencilLoadOp = VK_ATTACHMENT_LOAD_OP_CLEAR;
        attachment.stencilStoreOp = VK_ATTACHMENT_STORE_OP_STORE;
        attachment.finalLayout = VK_IMAGE_LAYOUT_DEPTH_STENCIL_ATTACHMENT_OPTIMAL;
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

const Vulkan::Texture& Framebuffer::colorTexture() const
{
    return *m_color_texture;
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
