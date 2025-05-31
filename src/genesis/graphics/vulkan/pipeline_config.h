/*
 * BSD 3-Clause License
 *
 * Copyright (c) 2024, Dmitry Shilnenkov
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

#include <genesis/graphics/pipeline_config.h>

#include <vulkan/vulkan.h>

namespace GE::Vulkan {

class DescriptorPool;

struct pipeline_config_t: GE::pipeline_config_t {
    VkPipelineCache pipeline_cache{VK_NULL_HANDLE};
    std::vector<VkFormat> color_formats{VK_FORMAT_UNDEFINED};
    VkFormat depth_format{VK_FORMAT_UNDEFINED};
    VkFrontFace front_face{VK_FRONT_FACE_COUNTER_CLOCKWISE};
    VkSampleCountFlagBits msaa_samples{VK_SAMPLE_COUNT_1_BIT};
    Shared<DescriptorPool> descriptor_pool{}; // NOLINT(readability-redundant-member-init)

    VkPipelineViewportStateCreateInfo viewport_state{};
    VkPipelineInputAssemblyStateCreateInfo input_assembly_state{};
    VkPipelineRasterizationStateCreateInfo rasterization_state{};
    VkPipelineMultisampleStateCreateInfo multisample_state{};
    VkPipelineDepthStencilStateCreateInfo depth_stencil_state{};
    std::vector<VkDynamicState> dynamic_state_list{}; // NOLINT(readability-redundant-member-init)
    VkPipelineDynamicStateCreateInfo dynamic_state{};
};

VkBlendFactor toVkBlendFactor(BlendFactor factor);
VkBlendOp toVkBlendOp(BlendOp op);
VkPrimitiveTopology toVkPrimitiveTopology(PrimitiveTopology topology);
VkPolygonMode toVkPolygonMode(PolygonMode mode);

} // namespace GE::Vulkan
