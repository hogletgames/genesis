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

#include "pipeline_barrier.h"
#include "image.h"

#include "genesis/core/asserts.h"

namespace {

VkImageAspectFlags toVkAspect(VkFormat format)
{
    switch (format) {
        case VK_FORMAT_D16_UNORM:
        case VK_FORMAT_D32_SFLOAT: return VK_IMAGE_ASPECT_DEPTH_BIT;
        case VK_FORMAT_S8_UINT: return VK_IMAGE_ASPECT_STENCIL_BIT;
        case VK_FORMAT_D16_UNORM_S8_UINT:
        case VK_FORMAT_D24_UNORM_S8_UINT:
        case VK_FORMAT_D32_SFLOAT_S8_UINT:
            return VK_IMAGE_ASPECT_DEPTH_BIT | VK_IMAGE_ASPECT_STENCIL_BIT;
        default: return VK_IMAGE_ASPECT_COLOR_BIT;
    }
}

std::pair<VkAccessFlags, VkAccessFlags> undefinedToVkAccess(VkImageLayout new_layout)
{
    switch (new_layout) {
        case VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL:
            return {0, VK_ACCESS_TRANSFER_WRITE_BIT};
        case VK_IMAGE_LAYOUT_DEPTH_ATTACHMENT_OPTIMAL:
            return {0, VK_ACCESS_DEPTH_STENCIL_ATTACHMENT_READ_BIT |
                           VK_ACCESS_DEPTH_STENCIL_ATTACHMENT_WRITE_BIT};
        case VK_IMAGE_LAYOUT_GENERAL:
        case VK_IMAGE_LAYOUT_UNDEFINED: return {0, 0};
        default: break;
    }

    GE_ASSERT(false, "Unsupported layout: {}", GE::toString(new_layout));
    return {};
}

std::pair<VkAccessFlags, VkAccessFlags> generalToVkAccess(VkImageLayout new_layout)
{
    switch (new_layout) {
        case VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL:
            return {0, VK_ACCESS_TRANSFER_WRITE_BIT};
        case VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL:
            return {0, VK_ACCESS_SHADER_READ_BIT};
        default: break;
    }

    GE_ASSERT(false, "Unsupported layout: {}", GE::toString(new_layout));
    return {};
}

std::pair<VkAccessFlags, VkAccessFlags> transferSrcToVkAccess(VkImageLayout new_layout)
{
    switch (new_layout) {
        case VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL:
            return {VK_ACCESS_TRANSFER_WRITE_BIT, VK_ACCESS_SHADER_READ_BIT};
        default: break;
    }

    GE_ASSERT(false, "Unsupported layout: {}", GE::toString(new_layout));
    return {};
}

std::pair<VkAccessFlags, VkAccessFlags> transferDstToVkAccess(VkImageLayout new_layout)
{
    switch (new_layout) {
        case VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL:
            return {VK_ACCESS_TRANSFER_WRITE_BIT, VK_ACCESS_SHADER_READ_BIT};
        case VK_IMAGE_LAYOUT_GENERAL: return {VK_ACCESS_TRANSFER_WRITE_BIT, 0};
        default: break;
    }

    GE_ASSERT(false, "Unsupported layout: {}", GE::toString(new_layout));
    return {};
}

std::pair<VkAccessFlags, VkAccessFlags> depthStencilToVkAccess(VkImageLayout new_layout)
{
    switch (new_layout) {
        case VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL:
            return {VK_ACCESS_DEPTH_STENCIL_ATTACHMENT_READ_BIT |
                        VK_ACCESS_DEPTH_STENCIL_ATTACHMENT_WRITE_BIT,
                    VK_ACCESS_SHADER_READ_BIT};
        default: break;
    }

    GE_ASSERT(false, "Unsupported layout: {}", GE::toString(new_layout));
    return {};
}

std::pair<VkAccessFlags, VkAccessFlags> shaderReadOnlyToVkAccess(VkImageLayout new_layout)
{
    switch (new_layout) {
        case VK_IMAGE_LAYOUT_TRANSFER_SRC_OPTIMAL:
            return {0, VK_ACCESS_TRANSFER_READ_BIT};
        default: break;
    }

    GE_ASSERT(false, "Unsupported layout: {}", GE::toString(new_layout));
    return {};
}

std::pair<VkAccessFlags, VkAccessFlags> toVkAccess(VkImageLayout old_layout,
                                                   VkImageLayout new_layout)
{
    switch (old_layout) {
        case VK_IMAGE_LAYOUT_UNDEFINED: return undefinedToVkAccess(new_layout);
        case VK_IMAGE_LAYOUT_GENERAL: return generalToVkAccess(new_layout);
        case VK_IMAGE_LAYOUT_TRANSFER_SRC_OPTIMAL:
            return transferSrcToVkAccess(new_layout);
        case VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL:
            return transferDstToVkAccess(new_layout);
        case VK_IMAGE_LAYOUT_DEPTH_STENCIL_ATTACHMENT_OPTIMAL:
            return depthStencilToVkAccess(new_layout);
        case VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL:
            return shaderReadOnlyToVkAccess(new_layout);
        default: break;
    }

    GE_ASSERT(false, "Unsupported layout: {}", GE::toString(old_layout));
    return {};
};

} // namespace
namespace GE::Vulkan {

VkImageMemoryBarrier
MemoryBarrier::createImageMemoryBarrier(const memory_barrier_config_t &config)
{
    auto [src_access, dst_access] = toVkAccess(config.old_layout, config.new_layout);

    VkImageMemoryBarrier barrier{};
    barrier.sType = VK_STRUCTURE_TYPE_IMAGE_MEMORY_BARRIER;
    barrier.srcAccessMask = src_access;
    barrier.dstAccessMask = dst_access;
    barrier.oldLayout = config.old_layout;
    barrier.newLayout = config.new_layout;
    barrier.srcQueueFamilyIndex = VK_QUEUE_FAMILY_IGNORED;
    barrier.dstQueueFamilyIndex = VK_QUEUE_FAMILY_IGNORED;
    barrier.image = config.image;
    barrier.subresourceRange.aspectMask = toVkAspect(config.image_format);
    barrier.subresourceRange.baseMipLevel = config.base_mip_level;
    barrier.subresourceRange.levelCount = config.level_count;
    barrier.subresourceRange.baseArrayLayer = config.base_array_layer;
    barrier.subresourceRange.layerCount = config.layer_count;

    return barrier;
}

void PipelineBarrier::submit(VkCommandBuffer cmd,
                             const std::vector<VkImageMemoryBarrier> &barriers,
                             VkPipelineStageFlagBits src_stage,
                             VkPipelineStageFlagBits dst_stage)
{
    vkCmdPipelineBarrier(cmd, src_stage, dst_stage, 0, 0, nullptr, 0, nullptr,
                         barriers.size(), barriers.data());
}

} // namespace GE::Vulkan
