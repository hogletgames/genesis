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

#pragma once

#include "pipeline_barrier.h"

#include <genesis/core/memory.h>

#include <vulkan/vulkan.h>

#include <vector>

namespace GE {
class StagingBuffer;
}

namespace GE::Vulkan {

class Device;
struct memory_barrier_config_t;

struct image_config_t {
    VkImageViewType view_type{VK_IMAGE_VIEW_TYPE_2D};
    VkExtent3D extent{0, 0, 1};
    uint32_t mip_levels{1};
    uint32_t layers{1};
    VkSampleCountFlagBits samples{VK_SAMPLE_COUNT_1_BIT};
    VkFormat format{};
    VkImageTiling tiling{};
    VkImageUsageFlags usage{};
    VkMemoryPropertyFlags memory_properties{};
    VkImageAspectFlags aspect_mask{};
};

class Image
{
public:
    Image(Shared<Device> device, const image_config_t& config);
    ~Image();

    void copyFrom(const GE::StagingBuffer& buffer, const std::vector<VkBufferImageCopy>& regions);
    void copyTo(const GE::StagingBuffer& buffer);

    VkImageMemoryBarrier imageMemoryBarrier() const;

    VkImage image() const { return m_image; }
    VkImageView view() const { return m_image_view; }

    const VkExtent3D& extent() const { return m_extent; }
    VkFormat format() const { return m_format; }
    uint32_t MIPLevels() const { return m_mip_levels; }
    uint32_t layers() const { return m_layers; }

private:
    void createImage(const image_config_t& config);
    void allocateMemory(VkMemoryPropertyFlags properties);
    void createImageView(const image_config_t& config);

    void destroyVulkanHandles();

    uint32_t getMemoryType(uint32_t type_filter, VkMemoryPropertyFlags properties);

    void transitionImageLayout(VkImageLayout new_layout);
    void copyToImage(const GE::StagingBuffer& buffer,
                     const std::vector<VkBufferImageCopy>& regions);
    void copyFromImage(const GE::StagingBuffer& buffer);
    void createMipmaps();

    Shared<Device> m_device;

    VkImage m_image{VK_NULL_HANDLE};
    VkDeviceMemory m_memory{VK_NULL_HANDLE};
    VkImageView m_image_view{VK_NULL_HANDLE};

    VkExtent3D m_extent{};
    VkFormat m_format{VK_FORMAT_UNDEFINED};
    uint32_t m_mip_levels{};
    uint32_t m_layers{};
};

} // namespace GE::Vulkan
