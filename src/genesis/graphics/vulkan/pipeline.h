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

#pragma once

#include <genesis/graphics/pipeline.h>
#include <genesis/math/types.h>

#include <vulkan/vulkan.h>

#include <vector>

namespace GE {
class Shader;
} // namespace GE

namespace GE::Vulkan {

class Device;

struct pipeline_config_t: GE::pipeline_config_t {
    VkPipelineCache pipeline_cache{VK_NULL_HANDLE};
    VkRenderPass render_pass{VK_NULL_HANDLE};
    uint32_t subpass{0};
    VkFrontFace front_face{VK_FRONT_FACE_COUNTER_CLOCKWISE};
    VkSampleCountFlagBits msaa_samples{VK_SAMPLE_COUNT_1_BIT};

    VkPipelineViewportStateCreateInfo viewport_state{};
    VkPipelineInputAssemblyStateCreateInfo input_assembly_state{};
    VkPipelineRasterizationStateCreateInfo rasterization_state{};
    VkPipelineMultisampleStateCreateInfo multisample_state{};
    VkPipelineColorBlendAttachmentState color_blend_attachment{};
    VkPipelineColorBlendStateCreateInfo color_blend_state{};
    VkPipelineDepthStencilStateCreateInfo depth_stencil_state{};
    std::vector<VkDynamicState> dynamic_state_list{};
    VkPipelineDynamicStateCreateInfo dynamic_state{};
};

class Pipeline: public GE::Pipeline
{
public:
    Pipeline(Shared<Device> device, const Vulkan::pipeline_config_t& config);
    ~Pipeline();

    void bind(GPUCommandQueue* queue) override;

    static Vulkan::pipeline_config_t
    createDefaultConfig(GE::pipeline_config_t base_config);

private:
    void createPipelineLayout();
    void createPipeline(Vulkan::pipeline_config_t config);
    void destroyVkHandles();

    Shared<Device> m_device;
    VkPipeline m_pipeline{VK_NULL_HANDLE};
    VkPipelineLayout m_pipeline_layout{VK_NULL_HANDLE};
};

} // namespace GE::Vulkan
