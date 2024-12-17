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

#include "image.h"
#include "buffers/staging_buffer.h"
#include "device.h"
#include "pipeline_barrier.h"
#include "single_command.h"
#include "vulkan_exception.h"

namespace GE::Vulkan {
namespace {

constexpr int32_t divideBlit(int32_t value)
{
    return value > 1 ? value / 2 : 1;
}

constexpr int32_t divideMip(int32_t value)
{
    return value > 1 ? value / 2 : value;
}

constexpr VkOffset3D toVkOffset3D(const VkExtent3D &extent)
{
    return {
        .x = static_cast<int32_t>(extent.width),
        .y = static_cast<int32_t>(extent.height),
        .z = static_cast<int32_t>(extent.depth),
    };
}

constexpr VkOffset3D toVkBlitDstOffset(const VkOffset3D &offset)
{
    return {divideBlit(offset.x), divideBlit(offset.y), divideBlit(offset.z)};
}

constexpr VkOffset3D toNextMipOffset(const VkOffset3D &offset)
{
    return {divideMip(offset.x), divideMip(offset.y), divideMip(offset.z)};
}

bool isLinearFilterSupported(const Device &device, VkFormat format)
{
    VkFormatProperties format_properties{};
    vkGetPhysicalDeviceFormatProperties(device.physicalDevice(), format, &format_properties);

    return (format_properties.optimalTilingFeatures &
            VK_FORMAT_FEATURE_SAMPLED_IMAGE_FILTER_LINEAR_BIT) != 0;
}

void generateMipLevel(VkCommandBuffer cmd, VkImage image, VkImageMemoryBarrier *barrier,
                      const VkOffset3D &mip_offset, uint32_t mip_level)
{
    barrier->subresourceRange.baseMipLevel = mip_level - 1;
    barrier->srcAccessMask = VK_ACCESS_TRANSFER_WRITE_BIT;
    barrier->dstAccessMask = VK_ACCESS_TRANSFER_READ_BIT;
    barrier->oldLayout = VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL;
    barrier->newLayout = VK_IMAGE_LAYOUT_TRANSFER_SRC_OPTIMAL;

    PipelineBarrier::submit(cmd, {*barrier}, VK_PIPELINE_STAGE_TRANSFER_BIT,
                            VK_PIPELINE_STAGE_TRANSFER_BIT);

    VkImageBlit blit{};
    blit.srcOffsets[0] = {};
    blit.srcOffsets[1] = mip_offset;
    blit.srcSubresource.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT;
    blit.srcSubresource.mipLevel = mip_level - 1;
    blit.srcSubresource.baseArrayLayer = 0;
    blit.srcSubresource.layerCount = 1;
    blit.dstOffsets[0] = {};
    blit.dstOffsets[1] = toVkBlitDstOffset(mip_offset);
    blit.dstSubresource.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT;
    blit.dstSubresource.mipLevel = mip_level;
    blit.dstSubresource.baseArrayLayer = 0;
    blit.dstSubresource.layerCount = 1;

    vkCmdBlitImage(cmd, image, VK_IMAGE_LAYOUT_TRANSFER_SRC_OPTIMAL, image,
                   VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL, 1, &blit, VK_FILTER_LINEAR);

    barrier->srcAccessMask = VK_ACCESS_TRANSFER_READ_BIT;
    barrier->dstAccessMask = VK_ACCESS_SHADER_READ_BIT;
    barrier->oldLayout = VK_IMAGE_LAYOUT_TRANSFER_SRC_OPTIMAL;
    barrier->newLayout = VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL;

    PipelineBarrier::submit(cmd, {*barrier}, VK_PIPELINE_STAGE_TRANSFER_BIT,
                            VK_PIPELINE_STAGE_FRAGMENT_SHADER_BIT);
}

} // namespace

Image::Image(Shared<Device> device, const image_config_t &config)
    : m_device{std::move(device)}
    , m_extent{config.extent}
    , m_format{config.format}
    , m_mip_levels{config.mip_levels}
    , m_layers{config.layers}
{
    createImage(config);
    allocateMemory(config.memory_properties);
    createImageView(config);
}

Image::~Image()
{
    destroyVulkanHandles();
}

void Image::copyFrom(const GE::StagingBuffer &buffer, const std::vector<VkBufferImageCopy> &regions)
{
    transitionImageLayout(VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL);
    copyToImage(buffer, regions);
    createMipmaps();
}

void Image::copyTo(const GE::StagingBuffer &buffer)
{
    transitionImageLayout(VK_IMAGE_LAYOUT_TRANSFER_SRC_OPTIMAL);
    copyFromImage(buffer);
};

VkImageMemoryBarrier Image::imageMemoryBarrier() const
{
    VkImageMemoryBarrier barrier{};
    barrier.sType = VK_STRUCTURE_TYPE_IMAGE_MEMORY_BARRIER;
    barrier.srcAccessMask = VK_ACCESS_NONE;
    barrier.dstAccessMask = VK_ACCESS_NONE;
    barrier.oldLayout = VK_IMAGE_LAYOUT_UNDEFINED;
    barrier.newLayout = VK_IMAGE_LAYOUT_UNDEFINED;
    barrier.srcQueueFamilyIndex = VK_QUEUE_FAMILY_IGNORED;
    barrier.dstQueueFamilyIndex = VK_QUEUE_FAMILY_IGNORED;
    barrier.image = m_image;
    barrier.subresourceRange.aspectMask = toVkAspect(m_format);
    barrier.subresourceRange.baseMipLevel = 0;
    barrier.subresourceRange.levelCount = m_mip_levels;
    barrier.subresourceRange.baseArrayLayer = 0;
    barrier.subresourceRange.layerCount = m_layers;

    return barrier;
}

void Image::createImage(const image_config_t &config)
{
    VkImageCreateInfo image_info{};
    image_info.sType = VK_STRUCTURE_TYPE_IMAGE_CREATE_INFO;
    image_info.imageType = VK_IMAGE_TYPE_2D;
    image_info.extent = config.extent;
    image_info.mipLevels = config.mip_levels;
    image_info.arrayLayers = 1;
    image_info.format = config.format;
    image_info.tiling = config.tiling;
    image_info.initialLayout = VK_IMAGE_LAYOUT_UNDEFINED;
    image_info.usage = config.usage;
    image_info.sharingMode = VK_SHARING_MODE_EXCLUSIVE;
    image_info.samples = config.samples;
    image_info.flags = 0;

    if (vkCreateImage(m_device->device(), &image_info, nullptr, &m_image) != VK_SUCCESS) {
        throw std::runtime_error{"Failed to create Image"};
    }
}

void Image::allocateMemory(VkMemoryPropertyFlags properties)
{
    VkMemoryRequirements mem_requirements{};
    vkGetImageMemoryRequirements(m_device->device(), m_image, &mem_requirements);

    VkMemoryAllocateInfo alloc_info{};
    alloc_info.sType = VK_STRUCTURE_TYPE_MEMORY_ALLOCATE_INFO;
    alloc_info.allocationSize = mem_requirements.size;
    alloc_info.memoryTypeIndex = getMemoryType(mem_requirements.memoryTypeBits, properties);

    if (vkAllocateMemory(m_device->device(), &alloc_info, nullptr, &m_memory) != VK_SUCCESS) {
        throw Vulkan::Exception{"Failed to allocate Image Memory"};
    }

    vkBindImageMemory(m_device->device(), m_image, m_memory, 0);
}

void Image::createImageView(const image_config_t &config)
{
    VkImageViewCreateInfo view_info{};
    view_info.sType = VK_STRUCTURE_TYPE_IMAGE_VIEW_CREATE_INFO;
    view_info.image = m_image;
    view_info.viewType = config.view_type;
    view_info.format = config.format;
    view_info.subresourceRange.aspectMask = config.aspect_mask;
    view_info.subresourceRange.baseMipLevel = 0;
    view_info.subresourceRange.levelCount = config.mip_levels;
    view_info.subresourceRange.baseArrayLayer = 0;
    view_info.subresourceRange.layerCount = config.layers;

    if (vkCreateImageView(m_device->device(), &view_info, nullptr, &m_image_view) != VK_SUCCESS) {
        throw Vulkan::Exception{"Failed to create Texture Image View"};
    }
}

void Image::destroyVulkanHandles()
{
    vkDestroyImageView(m_device->device(), m_image_view, nullptr);
    m_image_view = VK_NULL_HANDLE;

    vkDestroyImage(m_device->device(), m_image, nullptr);
    m_image = VK_NULL_HANDLE;

    vkFreeMemory(m_device->device(), m_memory, nullptr);
    m_memory = VK_NULL_HANDLE;
}

uint32_t Image::getMemoryType(uint32_t type_filter, VkMemoryPropertyFlags properties)
{
    VkPhysicalDeviceMemoryProperties mem_properties{};
    vkGetPhysicalDeviceMemoryProperties(m_device->physicalDevice(), &mem_properties);

    for (uint32_t i{0}; i < mem_properties.memoryTypeCount; i++) {
        uint32_t mem_type = 1 << i;
        bool is_type_suitable = (type_filter & mem_type) != 0;
        bool is_properties_suitable =
            (mem_properties.memoryTypes[i].propertyFlags & properties) != 0;

        if (is_type_suitable && is_properties_suitable) {
            return i;
        }
    }

    throw Vulkan::Exception{"Failed to find suitable memory type"};
}

void Image::transitionImageLayout(VkImageLayout new_layout)
{
    auto barrier = imageMemoryBarrier();
    barrier.oldLayout = VK_IMAGE_LAYOUT_UNDEFINED;
    barrier.newLayout = new_layout;
    barrier.dstAccessMask = VK_ACCESS_TRANSFER_READ_BIT;

    SingleCommand cmd{m_device, SingleCommand::QUEUE_TRANSFER};
    PipelineBarrier::submit(cmd.buffer(), {barrier}, VK_PIPELINE_STAGE_TOP_OF_PIPE_BIT,
                            VK_PIPELINE_STAGE_TRANSFER_BIT);
}

void Image::copyToImage(const GE::StagingBuffer &buffer,
                        const std::vector<VkBufferImageCopy> &regions)
{
    SingleCommand cmd{m_device, SingleCommand::QUEUE_TRANSFER};
    vkCmdCopyBufferToImage(cmd.buffer(), toVkBuffer(buffer.nativeHandle()), m_image,
                           VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL, regions.size(), regions.data());
}

void Image::copyFromImage(const GE::StagingBuffer &buffer)
{
    VkBufferImageCopy region{};
    region.bufferOffset = 0;
    region.bufferRowLength = 0;
    region.bufferImageHeight = 0;
    region.imageSubresource.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT;
    region.imageSubresource.mipLevel = 0;
    region.imageSubresource.baseArrayLayer = 0;
    region.imageSubresource.layerCount = 1;
    region.imageOffset = {};
    region.imageExtent = m_extent;

    SingleCommand cmd{m_device, SingleCommand::QUEUE_TRANSFER};
    vkCmdCopyImageToBuffer(cmd.buffer(), m_image, VK_IMAGE_LAYOUT_TRANSFER_SRC_OPTIMAL,
                           toVkBuffer(buffer.nativeHandle()), 1, &region);
}

void Image::createMipmaps()
{
    if (!isLinearFilterSupported(*m_device, m_format)) {
        throw Vulkan::Exception("Image doesn't support linear blitting");
    }

    SingleCommand cmd{m_device, SingleCommand::QUEUE_COMPUTE};
    VkOffset3D mip_offset = toVkOffset3D(m_extent);

    auto barrier = imageMemoryBarrier();
    barrier.subresourceRange.levelCount = 1;

    for (uint32_t i{1}; i < m_mip_levels; i++) {
        generateMipLevel(cmd.buffer(), m_image, &barrier, mip_offset, i);
        mip_offset = toNextMipOffset(mip_offset);
    }

    barrier.subresourceRange.baseMipLevel = m_mip_levels - 1;
    barrier.srcAccessMask = VK_ACCESS_TRANSFER_WRITE_BIT;
    barrier.dstAccessMask = VK_ACCESS_SHADER_READ_BIT;
    barrier.oldLayout = VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL;
    barrier.newLayout = VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL;

    PipelineBarrier::submit(cmd.buffer(), {barrier}, VK_PIPELINE_STAGE_TRANSFER_BIT,
                            VK_PIPELINE_STAGE_FRAGMENT_SHADER_BIT);
}

} // namespace GE::Vulkan
