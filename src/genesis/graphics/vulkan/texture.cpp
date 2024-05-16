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

#include "texture.h"
#include "buffers/staging_buffer.h"
#include "device.h"
#include "image.h"
#include "pipeline_barrier.h"
#include "sdl_gui_context.h"
#include "single_command.h"
#include "vulkan_exception.h"

#include "genesis/core/asserts.h"
#include "genesis/core/utils.h"

namespace {

constexpr uint32_t MAX_ANISOTROPY{16};

std::unordered_map<GE::TextureFormat, VkFormat> toVkFormatMap()
{
    return {
        {GE::TextureFormat::R8, VK_FORMAT_R8_UNORM},
        {GE::TextureFormat::RGB8, VK_FORMAT_R8G8B8_UNORM},
        {GE::TextureFormat::RGBA8, VK_FORMAT_R8G8B8A8_UNORM},
        {GE::TextureFormat::SRGB8, VK_FORMAT_R8G8B8_SRGB},
        {GE::TextureFormat::SRGBA8, VK_FORMAT_R8G8B8A8_SRGB},
        {GE::TextureFormat::R16F, VK_FORMAT_R16_SFLOAT},
        {GE::TextureFormat::RGBA16F, VK_FORMAT_R16G16B16A16_SFLOAT},
        {GE::TextureFormat::R32F, VK_FORMAT_R32_SFLOAT},
        {GE::TextureFormat::R32_UINT, VK_FORMAT_R32_UINT},
        {GE::TextureFormat::R32_INT, VK_FORMAT_R32_SINT},
        {GE::TextureFormat::RGBA32F, VK_FORMAT_R32G32B32A32_SFLOAT},
        {GE::TextureFormat::D32F, VK_FORMAT_D32_SFLOAT},
        {GE::TextureFormat::D24S8, VK_FORMAT_D24_UNORM_S8_UINT},
        {GE::TextureFormat::D32S8, VK_FORMAT_D32_SFLOAT_S8_UINT},
    };
}

VkFilter toVkFilter(GE::TextureFilter filter)
{
    switch (filter) {
        case GE::TextureFilter::NEAREST:
        case GE::TextureFilter::NEAREST_MIPMAP_NEAREST:
        case GE::TextureFilter::NEAREST_MIPMAP_LINEAR: return VK_FILTER_NEAREST;
        case GE::TextureFilter::LINEAR:
        case GE::TextureFilter::LINEAR_MIPMAP_NEAREST:
        case GE::TextureFilter::LINEAR_MIPMAP_LINEAR:
        default: return VK_FILTER_LINEAR;
    }
}

VkSamplerMipmapMode toVkMipmapMode(GE::TextureFilter filter)
{
    switch (filter) {
        case GE::TextureFilter::NEAREST:
        case GE::TextureFilter::NEAREST_MIPMAP_NEAREST:
        case GE::TextureFilter::LINEAR_MIPMAP_NEAREST: return VK_SAMPLER_MIPMAP_MODE_NEAREST;
        case GE::TextureFilter::NEAREST_MIPMAP_LINEAR:
        case GE::TextureFilter::LINEAR:
        case GE::TextureFilter::LINEAR_MIPMAP_LINEAR:
        default: return VK_SAMPLER_MIPMAP_MODE_LINEAR;
    }
}

VkSamplerAddressMode toVkSamplerAddressMode(GE::TextureWrap wrap)
{
    using Wrap = GE::TextureWrap;
    static const std::unordered_map<GE::TextureWrap, VkSamplerAddressMode> to_addr_mode = {
        {Wrap::CLAMP_TO_EDGE, VK_SAMPLER_ADDRESS_MODE_CLAMP_TO_EDGE},
        {Wrap::CLAMP_TO_BORDER, VK_SAMPLER_ADDRESS_MODE_CLAMP_TO_BORDER},
        {Wrap::REPEAT, VK_SAMPLER_ADDRESS_MODE_REPEAT},
        {Wrap::MIRROR_REPEAT, VK_SAMPLER_ADDRESS_MODE_MIRRORED_REPEAT}};

    return GE::getValue(to_addr_mode, wrap, VK_SAMPLER_ADDRESS_MODE_CLAMP_TO_EDGE);
}

VkSamplerAddressMode toVkSamplerAddressMode(GE::TextureFormat format, GE::TextureWrap wrap)
{
    if (wrap != GE::TextureWrap::AUTO) {
        return toVkSamplerAddressMode(wrap);
    }

    return GE::isColorFormat(format) ? VK_SAMPLER_ADDRESS_MODE_REPEAT
                                     : VK_SAMPLER_ADDRESS_MODE_CLAMP_TO_EDGE;
}

VkExtent3D toVkExtent(const GE::texture_config_t& config)
{
    return {config.width, config.height, config.depth};
}

uint32_t toMipLevels(const GE::texture_config_t& config)
{
    if (config.mip_levels != 0) {
        return config.mip_levels;
    }

    return GE::Texture::calculateMipLevels(config.width, config.height);
}

VkImageUsageFlags toVkUsage(const GE::texture_config_t& config)
{
    static constexpr VkImageUsageFlags default_usage = VK_IMAGE_USAGE_TRANSFER_SRC_BIT |
                                                       VK_IMAGE_USAGE_TRANSFER_DST_BIT |
                                                       VK_IMAGE_USAGE_SAMPLED_BIT;

    if (GE::isDepthFormat(config.format)) {
        return default_usage | VK_IMAGE_USAGE_DEPTH_STENCIL_ATTACHMENT_BIT;
    }

    VkImageUsageFlags usage = default_usage | VK_IMAGE_USAGE_COLOR_ATTACHMENT_BIT;

    if (config.image_storage) {
        usage |= VK_IMAGE_USAGE_STORAGE_BIT;
    }

    return usage;
}

GE::TextureFilter toMinFiler(const GE::texture_config_t& config)
{
    if (config.min_filter != GE::TextureFilter::AUTO) {
        return config.min_filter;
    }

    if (GE::isDepthFormat(config.format)) {
        return GE::TextureFilter::NEAREST;
    }

    if (config.mip_levels == 1) {
        return GE::TextureFilter::LINEAR;
    }

    return GE::TextureFilter::NEAREST_MIPMAP_LINEAR;
}

VkFilter toVkMagFilter(const GE::texture_config_t& config)
{
    if (config.mag_filter != GE::TextureFilter::AUTO) {
        return toVkFilter(config.mag_filter);
    }

    return GE::isColorFormat(config.format) ? VK_FILTER_LINEAR : VK_FILTER_NEAREST;
}

} // namespace

namespace GE::Vulkan {

Texture::Texture(Shared<Device> device, const texture_config_t& config)
    : m_device{std::move(device)}
    , m_size{config.width, config.height}
    , m_format{config.format}
    , m_is_opaque(config.is_opaque)
{
    createImage(config);
    createSampler(config);
}

Texture::~Texture()
{
    destroyVkHandles();
}

Texture::NativeID Texture::nativeID() const
{
    if (m_descriptor_set == VK_NULL_HANDLE) {
        m_descriptor_set = SDL::createGuiTextureID(*this);
    }

    return m_descriptor_set;
}

void Texture::createImage(const texture_config_t& config)
{
    image_config_t image_config{};
    image_config.view_type = toVkImageViewType(config.type);
    image_config.extent = toVkExtent(config);
    image_config.mip_levels = toMipLevels(config);
    image_config.samples = VK_SAMPLE_COUNT_1_BIT;
    image_config.format = toVkFormat(config.format);
    image_config.tiling = VK_IMAGE_TILING_OPTIMAL;
    image_config.usage = toVkUsage(config);
    image_config.memory_properties = VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT;
    image_config.aspect_mask = toVkImageAspect(config.format);

    m_image = makeScoped<Image>(m_device, image_config);

    if (config.image_storage) {
        colorImageBarrier();
    }
}

void Texture::createSampler(const texture_config_t& config)
{
    auto min_filter = toMinFiler(config);

    VkSamplerCreateInfo create_info{};
    create_info.sType = VK_STRUCTURE_TYPE_SAMPLER_CREATE_INFO;
    create_info.flags = 0;
    create_info.magFilter = toVkMagFilter(config);
    create_info.minFilter = toVkFilter(min_filter);
    create_info.mipmapMode = toVkMipmapMode(min_filter);
    create_info.addressModeU = toVkSamplerAddressMode(config.format, config.wrap_u);
    create_info.addressModeV = toVkSamplerAddressMode(config.format, config.wrap_v);
    create_info.addressModeW = toVkSamplerAddressMode(config.format, config.wrap_w);
    create_info.mipLodBias = 0.0f;
    create_info.anisotropyEnable = {};
    create_info.maxAnisotropy = MAX_ANISOTROPY;
    create_info.compareEnable = VK_FALSE;
    create_info.compareOp = VK_COMPARE_OP_NEVER;
    create_info.minLod = 0.0f;
    create_info.maxLod = static_cast<float>(config.mip_levels);
    create_info.borderColor = VK_BORDER_COLOR_FLOAT_OPAQUE_BLACK;
    create_info.unnormalizedCoordinates = VK_FALSE;

    if (vkCreateSampler(m_device->device(), &create_info, nullptr, &m_sampler) != VK_SUCCESS) {
        throw Vulkan::Exception{"Failed to create Sampler"};
    }
}

void Texture::destroyVkHandles()
{
    m_device->waitIdle();

    vkDestroySampler(m_device->device(), m_sampler, nullptr);
    m_sampler = VK_NULL_HANDLE;

    m_image.reset();

    SDL::destroyGuiTextureID(m_descriptor_set);
    m_descriptor_set = VK_NULL_HANDLE;
}

void Texture::colorImageBarrier()
{
    SingleCommand cmd{m_device};

    auto barrier = m_image->imageMemoryBarrier();
    barrier.srcAccessMask = VK_ACCESS_NONE;
    barrier.dstAccessMask = VK_ACCESS_NONE;
    barrier.oldLayout = VK_IMAGE_LAYOUT_UNDEFINED;
    barrier.newLayout = VK_IMAGE_LAYOUT_GENERAL;

    PipelineBarrier::submit(cmd.buffer(), {barrier}, VK_PIPELINE_STAGE_ALL_COMMANDS_BIT,
                            VK_PIPELINE_STAGE_ALL_COMMANDS_BIT);
}

bool Texture2D::setData(const void* data, uint32_t size)
{
    VkBufferImageCopy region{};
    region.bufferOffset = 0;
    region.bufferRowLength = 0;
    region.bufferImageHeight = 0;
    region.imageSubresource.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT;
    region.imageSubresource.mipLevel = 0;
    region.imageSubresource.baseArrayLayer = 0;
    region.imageSubresource.layerCount = m_image->layers();
    region.imageOffset = {0, 0, 0};
    region.imageExtent = m_image->extent();

    StagingBuffer buffer{m_device, size, data};
    m_image->copyFrom(buffer, {region});
    return true;
}

uint32_t Texture2D::checkDepth(uint32_t depth)
{
    GE_CORE_ASSERT(depth == 1, "Expected depth=1, got={}", depth);
    return depth;
}

uint32_t Texture2D::checkLayers(uint32_t layers)
{
    GE_CORE_ASSERT(layers == 1, "Expected layers=1, got={}", layers);
    return layers;
}

VkFormat toVkFormat(TextureFormat format)
{
    static const auto to_format = toVkFormatMap();
    return getValue(to_format, format, VK_FORMAT_UNDEFINED);
}

TextureFormat toTextureFormat(VkFormat format)
{
    static const auto to_format = swapKeyAndValue(toVkFormatMap());
    return getValue(to_format, format, TextureFormat::UNKNOWN);
}

VkImageViewType toVkImageViewType(TextureType type)
{
    static const std::unordered_map<TextureType, VkImageViewType> to_type = {
        {TextureType::TEXTURE_2D, VK_IMAGE_VIEW_TYPE_2D},
    };

    return getValue(to_type, type, VK_IMAGE_VIEW_TYPE_MAX_ENUM);
}

VkImageAspectFlags toVkImageAspect(TextureFormat format)
{
    if (isDepthFormat(format)) {
        return VK_IMAGE_ASPECT_DEPTH_BIT;
    }

    return VK_IMAGE_ASPECT_COLOR_BIT;
}

} // namespace GE::Vulkan
