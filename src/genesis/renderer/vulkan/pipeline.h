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

// NOLINTNEXTLINE(llvm-header-guard)
#ifndef GENESIS_RENDERER_VULKAN_PIPELINE_H_
#define GENESIS_RENDERER_VULKAN_PIPELINE_H_

#include <genesis/core/memory.h>
#include <genesis/math/types.h>

#include <genesis/renderer/gpu_command_queue.h>
#include <vulkan/vulkan.h>

namespace GE {
class Shader;
} // namespace GE

namespace GE::Vulkan {

class Device;

struct pipeline_config_t {
    VkPipelineViewportStateCreateInfo viewport_state;
    VkPipelineInputAssemblyStateCreateInfo input_assembly_state;
    VkPipelineRasterizationStateCreateInfo rasterization_state;
    VkPipelineMultisampleStateCreateInfo multisample_state;
    VkPipelineColorBlendAttachmentState color_blend_attachment;
    VkPipelineColorBlendStateCreateInfo color_blend_state;
    VkPipelineDepthStencilStateCreateInfo depth_stencil_state;
    std::vector<VkDynamicState> dynamic_state_list;
    VkPipelineDynamicStateCreateInfo dynamic_state;

    Shared<GE::Shader> vert_shader;
    Shared<GE::Shader> frag_shader;
    VkPipelineLayout pipeline_layout{VK_NULL_HANDLE};
    VkRenderPass render_pass{VK_NULL_HANDLE};
    uint32_t subpass{0};
};

class Pipeline
{
public:
    Pipeline(Shared<Device> device, const pipeline_config_t& config);
    ~Pipeline();

    void bind(GPUCommandQueue* queue);

    static pipeline_config_t makeDefaultConfig();

private:
    void createPipeline(const pipeline_config_t& config);

    void destroyVkHandles();

    Shared<Device> m_device;
    VkPipeline m_pipeline{VK_NULL_HANDLE};
};

} // namespace GE::Vulkan

#endif // GENESIS_RENDERER_VULKAN_PIPELINE_H_
