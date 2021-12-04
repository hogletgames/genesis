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
#include "device.h"
#include "pipeline_barrier.h"
#include "vulkan_exception.h"

namespace GE::Vulkan {

Image::Image(Shared<Device> device, const image_config_t& config)
    : m_device{std::move(device)}
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

memory_barrier_config_t Image::memoryBarrierConfig() const
{
    memory_barrier_config_t config{};
    config.image = m_image;
    config.image_format = m_format;
    config.level_count = m_mip_levels;
    config.layer_count = m_layers;

    return config;
}

void Image::createImage(const image_config_t& config)
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
    alloc_info.memoryTypeIndex =
        getMemoryType(mem_requirements.memoryTypeBits, properties);

    if (vkAllocateMemory(m_device->device(), &alloc_info, nullptr, &m_memory) !=
        VK_SUCCESS) {
        throw Vulkan::Exception{"Failed to allocate Image Memory"};
    }

    vkBindImageMemory(m_device->device(), m_image, m_memory, 0);
}

void Image::createImageView(const image_config_t& config)
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

    if (vkCreateImageView(m_device->device(), &view_info, nullptr, &m_image_view) !=
        VK_SUCCESS) {
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

} // namespace GE::Vulkan
