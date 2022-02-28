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
#ifndef GENESIS_GRAPHICS_VULKAN_PIPELINE_BARRIER_H_
#define GENESIS_GRAPHICS_VULKAN_PIPELINE_BARRIER_H_

#include <vector>
#include <vulkan/vulkan.h>

namespace GE::Vulkan {

class Image;

struct memory_barrier_config_t {
    VkImage image{VK_NULL_HANDLE};
    VkFormat image_format{VK_FORMAT_UNDEFINED};
    uint32_t level_count{0};
    uint32_t layer_count{0};

    VkImageLayout old_layout{VK_IMAGE_LAYOUT_UNDEFINED};
    VkImageLayout new_layout{VK_IMAGE_LAYOUT_UNDEFINED};
    uint32_t base_mip_level{0};
    uint32_t base_array_layer{0};
};

class MemoryBarrier
{
public:
    static VkImageMemoryBarrier
    createImageMemoryBarrier(const memory_barrier_config_t& config);
};

class PipelineBarrier
{
public:
    static void submit(VkCommandBuffer cmd,
                       const std::vector<VkImageMemoryBarrier>& barriers,
                       VkPipelineStageFlagBits src_stage,
                       VkPipelineStageFlagBits dst_stage);
};

} // namespace GE::Vulkan

#endif // GENESIS_GRAPHICS_VULKAN_PIPELINE_BARRIER_H_
