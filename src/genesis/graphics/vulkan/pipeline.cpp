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

#include "pipeline.h"
#include "command_buffer.h"
#include "device.h"
#include "input_stage_descriptions.h"
#include "shader.h"
#include "vulkan_exception.h"

#include "genesis/core/log.h"
#include "genesis/graphics/gpu_command_queue.h"

namespace {

constexpr auto SHADER_ENTRYPOINT = "main";

} // namespace

namespace GE::Vulkan {

Pipeline::Pipeline(Shared<Device> device, const Vulkan::pipeline_config_t& config)
    : m_device{std::move(device)}
{
    createPipelineLayout();
    createPipeline(config);
}

Pipeline::~Pipeline()
{
    destroyVkHandles();
}

void Pipeline::bind(GPUCommandQueue* queue)
{
    queue->enqueue([this](void* cmd_buffer) {
        vkCmdBindPipeline(cmdBuffer(cmd_buffer), VK_PIPELINE_BIND_POINT_GRAPHICS,
                          m_pipeline);
    });
}

Vulkan::pipeline_config_t Pipeline::makeDefaultConfig()
{
    Vulkan::pipeline_config_t config{};

    config.input_assembly_state.sType =
        VK_STRUCTURE_TYPE_PIPELINE_INPUT_ASSEMBLY_STATE_CREATE_INFO;
    config.input_assembly_state.topology = VK_PRIMITIVE_TOPOLOGY_TRIANGLE_LIST;
    config.input_assembly_state.primitiveRestartEnable = VK_FALSE;

    config.viewport_state.sType = VK_STRUCTURE_TYPE_PIPELINE_VIEWPORT_STATE_CREATE_INFO;
    config.viewport_state.viewportCount = 1;
    config.viewport_state.pViewports = nullptr;
    config.viewport_state.scissorCount = 1;
    config.viewport_state.pScissors = nullptr;

    config.rasterization_state.sType =
        VK_STRUCTURE_TYPE_PIPELINE_RASTERIZATION_STATE_CREATE_INFO;
    config.rasterization_state.depthClampEnable = VK_FALSE;
    config.rasterization_state.rasterizerDiscardEnable = VK_FALSE;
    config.rasterization_state.polygonMode = VK_POLYGON_MODE_FILL;
    config.rasterization_state.lineWidth = 1.0f;
    config.rasterization_state.cullMode = VK_CULL_MODE_NONE;
    config.rasterization_state.frontFace = VK_FRONT_FACE_CLOCKWISE;
    config.rasterization_state.depthBiasEnable = VK_FALSE;
    config.rasterization_state.depthBiasConstantFactor = 0.0f; // Optional
    config.rasterization_state.depthBiasClamp = 0.0f;          // Optional
    config.rasterization_state.depthBiasSlopeFactor = 0.0f;    // Optional

    config.multisample_state.sType =
        VK_STRUCTURE_TYPE_PIPELINE_MULTISAMPLE_STATE_CREATE_INFO;
    config.multisample_state.sampleShadingEnable = VK_FALSE;
    config.multisample_state.rasterizationSamples = VK_SAMPLE_COUNT_1_BIT;
    config.multisample_state.minSampleShading = 1.0f;          // Optional
    config.multisample_state.pSampleMask = nullptr;            // Optional
    config.multisample_state.alphaToCoverageEnable = VK_FALSE; // Optional
    config.multisample_state.alphaToOneEnable = VK_FALSE;      // Optional

    config.color_blend_attachment.colorWriteMask =
        VK_COLOR_COMPONENT_R_BIT | VK_COLOR_COMPONENT_G_BIT | VK_COLOR_COMPONENT_B_BIT |
        VK_COLOR_COMPONENT_A_BIT;
    config.color_blend_attachment.blendEnable = VK_FALSE;
    config.color_blend_attachment.srcColorBlendFactor = VK_BLEND_FACTOR_ONE;  // Optional
    config.color_blend_attachment.dstColorBlendFactor = VK_BLEND_FACTOR_ZERO; // Optional
    config.color_blend_attachment.colorBlendOp = VK_BLEND_OP_ADD;             // Optional
    config.color_blend_attachment.srcAlphaBlendFactor = VK_BLEND_FACTOR_ONE;  // Optional
    config.color_blend_attachment.dstAlphaBlendFactor = VK_BLEND_FACTOR_ZERO; // Optional
    config.color_blend_attachment.alphaBlendOp = VK_BLEND_OP_ADD;             // Optional

    config.color_blend_state.sType =
        VK_STRUCTURE_TYPE_PIPELINE_COLOR_BLEND_STATE_CREATE_INFO;
    config.color_blend_state.logicOpEnable = VK_FALSE;
    config.color_blend_state.pAttachments = &config.color_blend_attachment;
    config.color_blend_state.attachmentCount = 1;
    config.color_blend_state.logicOp = VK_LOGIC_OP_COPY; // Optional
    config.color_blend_state.blendConstants[0] = 0.0f;   // Optional
    config.color_blend_state.blendConstants[1] = 0.0f;   // Optional
    config.color_blend_state.blendConstants[2] = 0.0f;   // Optional
    config.color_blend_state.blendConstants[3] = 0.0f;   // Optional

    config.depth_stencil_state.sType =
        VK_STRUCTURE_TYPE_PIPELINE_DEPTH_STENCIL_STATE_CREATE_INFO;
    config.depth_stencil_state.depthTestEnable = VK_TRUE;
    config.depth_stencil_state.depthWriteEnable = VK_TRUE;
    config.depth_stencil_state.depthCompareOp = VK_COMPARE_OP_LESS;
    config.depth_stencil_state.depthBoundsTestEnable = VK_FALSE;
    config.depth_stencil_state.stencilTestEnable = VK_FALSE;
    config.depth_stencil_state.minDepthBounds = 0.0f; // Optional
    config.depth_stencil_state.maxDepthBounds = 1.0f; // Optional
    config.depth_stencil_state.front = {};            // Optional
    config.depth_stencil_state.back = {};             // Optional

    config.dynamic_state_list = {VK_DYNAMIC_STATE_VIEWPORT, VK_DYNAMIC_STATE_SCISSOR};

    config.dynamic_state.sType = VK_STRUCTURE_TYPE_PIPELINE_DYNAMIC_STATE_CREATE_INFO;
    config.dynamic_state.dynamicStateCount = config.dynamic_state_list.size();
    config.dynamic_state.pDynamicStates = config.dynamic_state_list.data();
    config.dynamic_state.flags = 0;

    return config;
}

void Pipeline::createPipelineLayout()
{
    VkPipelineLayoutCreateInfo pipeline_layout_info{};
    pipeline_layout_info.sType = VK_STRUCTURE_TYPE_PIPELINE_LAYOUT_CREATE_INFO;
    pipeline_layout_info.pSetLayouts = nullptr;
    pipeline_layout_info.setLayoutCount = 0;
    pipeline_layout_info.pPushConstantRanges = nullptr;
    pipeline_layout_info.pushConstantRangeCount = 0;

    if (vkCreatePipelineLayout(m_device->device(), &pipeline_layout_info, nullptr,
                               &m_pipeline_layout) != VK_SUCCESS) {
        throw Vulkan::Exception{"Failed to create Pipeline Layout"};
    }
}

void Pipeline::createPipeline(Vulkan::pipeline_config_t config)
{
    VkPipelineShaderStageCreateInfo vert_shader_stage_info{};
    vert_shader_stage_info.sType = VK_STRUCTURE_TYPE_PIPELINE_SHADER_STAGE_CREATE_INFO;
    vert_shader_stage_info.stage = VK_SHADER_STAGE_VERTEX_BIT;
    vert_shader_stage_info.module =
        vulkanShaderHandle(config.base.vertex_shader->nativeHandle());
    vert_shader_stage_info.pName = SHADER_ENTRYPOINT;

    VkPipelineShaderStageCreateInfo frag_shader_stage_info{};
    frag_shader_stage_info.sType = VK_STRUCTURE_TYPE_PIPELINE_SHADER_STAGE_CREATE_INFO;
    frag_shader_stage_info.stage = VK_SHADER_STAGE_FRAGMENT_BIT;
    frag_shader_stage_info.module =
        vulkanShaderHandle(config.base.fragment_shader->nativeHandle());
    frag_shader_stage_info.pName = SHADER_ENTRYPOINT;

    std::array<VkPipelineShaderStageCreateInfo, 2> shader_stages = {
        vert_shader_stage_info,
        frag_shader_stage_info,
    };

    auto vert_layout = config.base.vertex_shader->inputLayout();
    auto binding_description = vertexInputBindDescription(vert_layout);
    auto attribute_descriptions = vertexInputAttributeDescriptions(vert_layout);

    VkPipelineVertexInputStateCreateInfo vertex_input_state{};
    vertex_input_state.sType = VK_STRUCTURE_TYPE_PIPELINE_VERTEX_INPUT_STATE_CREATE_INFO;
    vertex_input_state.vertexBindingDescriptionCount = 1;
    vertex_input_state.pVertexBindingDescriptions = &binding_description;
    vertex_input_state.vertexAttributeDescriptionCount = attribute_descriptions.size();
    vertex_input_state.pVertexAttributeDescriptions = attribute_descriptions.data();

    config.rasterization_state.frontFace = config.front_face;

    VkGraphicsPipelineCreateInfo pipeline_info{};
    pipeline_info.sType = VK_STRUCTURE_TYPE_GRAPHICS_PIPELINE_CREATE_INFO;
    pipeline_info.pStages = shader_stages.data();
    pipeline_info.stageCount = shader_stages.size();
    pipeline_info.pVertexInputState = &vertex_input_state;
    pipeline_info.pInputAssemblyState = &config.input_assembly_state;
    pipeline_info.pViewportState = &config.viewport_state;
    pipeline_info.pRasterizationState = &config.rasterization_state;
    pipeline_info.pMultisampleState = &config.multisample_state;
    pipeline_info.pDepthStencilState = &config.depth_stencil_state;
    pipeline_info.pColorBlendState = &config.color_blend_state;
    pipeline_info.pDynamicState = &config.dynamic_state;
    pipeline_info.layout = m_pipeline_layout;
    pipeline_info.renderPass = config.render_pass;
    pipeline_info.subpass = config.subpass;
    pipeline_info.basePipelineHandle = VK_NULL_HANDLE; // Optional
    pipeline_info.basePipelineIndex = -1;              // Optional

    if (vkCreateGraphicsPipelines(m_device->device(), config.pipeline_cache, 1,
                                  &pipeline_info, nullptr, &m_pipeline) != VK_SUCCESS) {
        throw Vulkan::Exception{"Failed to create Graphics Pipeline"};
    }
}

void Pipeline::destroyVkHandles()
{
    m_device->waitIdle();

    vkDestroyPipeline(m_device->device(), m_pipeline, nullptr);
    m_pipeline = VK_NULL_HANDLE;

    vkDestroyPipelineLayout(m_device->device(), m_pipeline_layout, nullptr);
    m_pipeline_layout = VK_NULL_HANDLE;
}

} // namespace GE::Vulkan
