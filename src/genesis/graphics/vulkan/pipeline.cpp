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
#include "blending.h"
#include "buffers/uniform_buffer.h"
#include "command_buffer.h"
#include "device.h"
#include "image.h"
#include "input_stage_descriptions.h"
#include "pipeline_resources.h"
#include "shader.h"
#include "shader_data_type_size.h"
#include "texture.h"
#include "utils.h"
#include "vulkan_exception.h"

#include "genesis/core/asserts.h"
#include "genesis/core/log.h"
#include "genesis/graphics/gpu_command_queue.h"

namespace GE::Vulkan {
namespace {

constexpr auto SHADER_ENTRYPOINT = "main";

VkPipelineColorBlendAttachmentState
toVkPipelineColorBlendAttachmentState(const blending_t& blending)
{
    VkPipelineColorBlendAttachmentState color_blend_attachment{};
    color_blend_attachment.colorWriteMask = VK_COLOR_COMPONENT_R_BIT | VK_COLOR_COMPONENT_G_BIT |
                                            VK_COLOR_COMPONENT_B_BIT | VK_COLOR_COMPONENT_A_BIT;
    color_blend_attachment.blendEnable = toVkBool(blending.enabled);
    color_blend_attachment.srcColorBlendFactor = toVkBlendFactor(blending.src_color_factor);
    color_blend_attachment.dstColorBlendFactor = toVkBlendFactor(blending.dst_color_factor);
    color_blend_attachment.colorBlendOp = toVkBlendOp(blending.color_op);
    color_blend_attachment.srcAlphaBlendFactor = toVkBlendFactor(blending.src_alpha_factor);
    color_blend_attachment.dstAlphaBlendFactor = toVkBlendFactor(blending.dst_alpha_factor);
    color_blend_attachment.alphaBlendOp = toVkBlendOp(blending.alpha_op);

    return color_blend_attachment;
}

std::vector<VkPipelineColorBlendAttachmentState>
toColorBlendAttacmentStates(const pipeline_config_t::BlendingCondig& blending, uint32_t count)
{
    std::vector<VkPipelineColorBlendAttachmentState> color_blend_attachments(count);

    if (std::holds_alternative<blending_t>(blending)) {
        std::fill(color_blend_attachments.begin(), color_blend_attachments.end(),
                  toVkPipelineColorBlendAttachmentState(std::get<blending_t>(blending)));
    } else {
        const auto& blendings = std::get<std::vector<blending_t>>(blending);
        GE_CORE_ASSERT(blendings.size() == count, "Invalid blending config count '{} != {}'",
                       blendings.size(), count);

        std::transform(blendings.cbegin(), blendings.cend(), color_blend_attachments.begin(),
                       &toVkPipelineColorBlendAttachmentState);
    }

    return color_blend_attachments;
}

bool isPushConstantValid(const push_constant_t& push_constant, uint32_t expected_size)
{
    if (push_constant.size != expected_size) {
        GE_CORE_ERR("Push constant size mismatch: expected {}, got {}", expected_size,
                    push_constant.size);
        return false;
    }

    return true;
}

} // namespace

Pipeline::Pipeline(Shared<Device> device, const Vulkan::pipeline_config_t& config)
    : m_device{std::move(device)}
    , m_resources{makeScoped<PipelineResources>(m_device, config)}
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
        vkCmdBindPipeline(toVkCommandBuffer(cmd_buffer), VK_PIPELINE_BIND_POINT_GRAPHICS,
                          m_pipeline);
    });
}

void Pipeline::bind(GPUCommandQueue* queue, const std::string& name, const GE::UniformBuffer& ubo)
{
    VkDescriptorBufferInfo info{};
    info.buffer = toVkBuffer(ubo.nativeHandle());
    info.range = ubo.size();

    VkWriteDescriptorSet write_descriptor_set{};
    write_descriptor_set.pBufferInfo = &info;

    bindResource(queue, name, &write_descriptor_set);
}

void Pipeline::bind(GPUCommandQueue* queue, const std::string& name, const GE::Texture& texture)
{
    const auto* vk_texture = toVulkan(texture);

    VkDescriptorImageInfo info{};
    info.imageLayout = VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL;
    info.imageView = vk_texture->image()->view();
    info.sampler = vk_texture->sampler();

    VkWriteDescriptorSet write_descriptor_set{};
    write_descriptor_set.pImageInfo = &info;

    bindResource(queue, name, &write_descriptor_set);
}

void Pipeline::pushConstant(GPUCommandQueue* queue, const std::string& name, bool value)
{
    pushConstantIfValid(queue, name, value);
}

void Pipeline::pushConstant(GPUCommandQueue* queue, const std::string& name, int32_t value)
{
    pushConstantIfValid(queue, name, value);
}

void Pipeline::pushConstant(GPUCommandQueue* queue, const std::string& name, uint32_t value)
{
    pushConstantIfValid(queue, name, value);
}

void Pipeline::pushConstant(GPUCommandQueue* queue, const std::string& name, float value)
{
    pushConstantIfValid(queue, name, value);
}

void Pipeline::pushConstant(GPUCommandQueue* queue, const std::string& name, double value)
{
    pushConstantIfValid(queue, name, value);
}

void Pipeline::pushConstant(GPUCommandQueue* queue, const std::string& name, const Vec2& value)
{
    pushConstantIfValid(queue, name, value);
}

void Pipeline::pushConstant(GPUCommandQueue* queue, const std::string& name, const Vec3& value)
{
    pushConstantIfValid(queue, name, value);
}

void Pipeline::pushConstant(GPUCommandQueue* queue, const std::string& name, const Mat4& value)
{
    pushConstantIfValid(queue, name, value);
}

Vulkan::pipeline_config_t Pipeline::createDefaultConfig(GE::pipeline_config_t base_config)
{
    Vulkan::pipeline_config_t config{std::move(base_config)};

    config.input_assembly_state.sType = VK_STRUCTURE_TYPE_PIPELINE_INPUT_ASSEMBLY_STATE_CREATE_INFO;
    config.input_assembly_state.topology = VK_PRIMITIVE_TOPOLOGY_TRIANGLE_LIST;
    config.input_assembly_state.primitiveRestartEnable = VK_FALSE;

    config.viewport_state.sType = VK_STRUCTURE_TYPE_PIPELINE_VIEWPORT_STATE_CREATE_INFO;
    config.viewport_state.viewportCount = 1;
    config.viewport_state.pViewports = nullptr;
    config.viewport_state.scissorCount = 1;
    config.viewport_state.pScissors = nullptr;

    config.rasterization_state.sType = VK_STRUCTURE_TYPE_PIPELINE_RASTERIZATION_STATE_CREATE_INFO;
    config.rasterization_state.depthClampEnable = VK_FALSE;
    config.rasterization_state.rasterizerDiscardEnable = VK_FALSE;
    config.rasterization_state.polygonMode = VK_POLYGON_MODE_FILL;
    config.rasterization_state.lineWidth = 1.0f;
    config.rasterization_state.cullMode = VK_CULL_MODE_BACK_BIT;
    config.rasterization_state.frontFace = VK_FRONT_FACE_COUNTER_CLOCKWISE;
    config.rasterization_state.depthBiasEnable = VK_FALSE;
    config.rasterization_state.depthBiasConstantFactor = 0.0f; // Optional
    config.rasterization_state.depthBiasClamp = 0.0f;          // Optional
    config.rasterization_state.depthBiasSlopeFactor = 0.0f;    // Optional

    config.multisample_state.sType = VK_STRUCTURE_TYPE_PIPELINE_MULTISAMPLE_STATE_CREATE_INFO;
    config.multisample_state.sampleShadingEnable = VK_FALSE;
    config.multisample_state.rasterizationSamples = VK_SAMPLE_COUNT_1_BIT;
    config.multisample_state.minSampleShading = 1.0f;          // Optional
    config.multisample_state.pSampleMask = nullptr;            // Optional
    config.multisample_state.alphaToCoverageEnable = VK_FALSE; // Optional
    config.multisample_state.alphaToOneEnable = VK_FALSE;      // Optional

    config.depth_stencil_state.sType = VK_STRUCTURE_TYPE_PIPELINE_DEPTH_STENCIL_STATE_CREATE_INFO;
    config.depth_stencil_state.depthTestEnable = toVkBool(config.depth_test_enable);
    config.depth_stencil_state.depthWriteEnable = toVkBool(config.depth_write_enable);
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
    const auto& descriptor_set_layouts = m_resources->descriptorSetLayouts();
    const auto& push_constant_ranges = m_resources->pushConstantRanges();

    VkPipelineLayoutCreateInfo pipeline_layout_info{};
    pipeline_layout_info.sType = VK_STRUCTURE_TYPE_PIPELINE_LAYOUT_CREATE_INFO;

    if (!descriptor_set_layouts.empty()) {
        pipeline_layout_info.setLayoutCount = descriptor_set_layouts.size();
        pipeline_layout_info.pSetLayouts = descriptor_set_layouts.data();
    }

    if (!push_constant_ranges.empty()) {
        pipeline_layout_info.pushConstantRangeCount = push_constant_ranges.size();
        pipeline_layout_info.pPushConstantRanges = push_constant_ranges.data();
    }

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
    vert_shader_stage_info.module = toVkShaderModule(*config.vertex_shader);
    vert_shader_stage_info.pName = SHADER_ENTRYPOINT;

    VkPipelineShaderStageCreateInfo frag_shader_stage_info{};
    frag_shader_stage_info.sType = VK_STRUCTURE_TYPE_PIPELINE_SHADER_STAGE_CREATE_INFO;
    frag_shader_stage_info.stage = VK_SHADER_STAGE_FRAGMENT_BIT;
    frag_shader_stage_info.module = toVkShaderModule(*config.fragment_shader);
    frag_shader_stage_info.pName = SHADER_ENTRYPOINT;

    std::array<VkPipelineShaderStageCreateInfo, 2> shader_stages = {
        vert_shader_stage_info,
        frag_shader_stage_info,
    };

    auto vert_layout = config.vertex_shader->inputLayout();
    auto binding_description = vertexInputBindDescription(vert_layout);
    auto attribute_descriptions = vertexInputAttributeDescriptions(vert_layout);

    VkPipelineVertexInputStateCreateInfo vertex_input_state{};
    vertex_input_state.sType = VK_STRUCTURE_TYPE_PIPELINE_VERTEX_INPUT_STATE_CREATE_INFO;
    if (!attribute_descriptions.empty()) {
        vertex_input_state.vertexBindingDescriptionCount = 1;
        vertex_input_state.pVertexBindingDescriptions = &binding_description;
        vertex_input_state.vertexAttributeDescriptionCount = attribute_descriptions.size();
        vertex_input_state.pVertexAttributeDescriptions = attribute_descriptions.data();
    }

    config.rasterization_state.frontFace = config.front_face;
    config.multisample_state.rasterizationSamples = config.msaa_samples;

    auto color_blend_attachments =
        toColorBlendAttacmentStates(config.blending, config.color_formats.size());

    VkPipelineColorBlendStateCreateInfo color_blend_state{};
    color_blend_state.sType = VK_STRUCTURE_TYPE_PIPELINE_COLOR_BLEND_STATE_CREATE_INFO;
    color_blend_state.logicOpEnable = VK_FALSE;
    color_blend_state.logicOp = VK_LOGIC_OP_COPY;
    color_blend_state.attachmentCount = color_blend_attachments.size();
    color_blend_state.pAttachments = color_blend_attachments.data();
    color_blend_state.blendConstants[0] = 1.0f;
    color_blend_state.blendConstants[1] = 1.0f;
    color_blend_state.blendConstants[2] = 1.0f;
    color_blend_state.blendConstants[3] = 1.0f;

    VkPipelineRenderingCreateInfo rendering_info{};
    rendering_info.sType = VK_STRUCTURE_TYPE_PIPELINE_RENDERING_CREATE_INFO;
    rendering_info.colorAttachmentCount = config.color_formats.size();
    rendering_info.pColorAttachmentFormats = config.color_formats.data();
    rendering_info.depthAttachmentFormat = config.depth_format;

    VkGraphicsPipelineCreateInfo pipeline_info{};
    pipeline_info.sType = VK_STRUCTURE_TYPE_GRAPHICS_PIPELINE_CREATE_INFO;
    pipeline_info.pNext = &rendering_info;
    pipeline_info.pStages = shader_stages.data();
    pipeline_info.stageCount = shader_stages.size();
    pipeline_info.pVertexInputState = &vertex_input_state;
    pipeline_info.pInputAssemblyState = &config.input_assembly_state;
    pipeline_info.pViewportState = &config.viewport_state;
    pipeline_info.pRasterizationState = &config.rasterization_state;
    pipeline_info.pMultisampleState = &config.multisample_state;
    pipeline_info.pDepthStencilState = &config.depth_stencil_state;
    pipeline_info.pColorBlendState = &color_blend_state;
    pipeline_info.pDynamicState = &config.dynamic_state;
    pipeline_info.layout = m_pipeline_layout;
    pipeline_info.basePipelineHandle = VK_NULL_HANDLE; // Optional
    pipeline_info.basePipelineIndex = -1;              // Optional

    if (vkCreateGraphicsPipelines(m_device->device(), config.pipeline_cache, 1, &pipeline_info,
                                  nullptr, &m_pipeline) != VK_SUCCESS) {
        throw Vulkan::Exception{"Failed to create Graphics Pipeline"};
    }
}

void Pipeline::bindResource(GPUCommandQueue* queue, const std::string& name,
                            VkWriteDescriptorSet* write_descriptor_set)
{
    auto resource = m_resources->resource(name);

    if (!resource.has_value()) {
        GE_CORE_ERR("Failed to bind '{}': there is no such resource", name);
        return;
    }

    auto* descriptor_set = m_resources->descriptorSet(resource.value());
    if (descriptor_set == VK_NULL_HANDLE) {
        GE_CORE_ERR("Failed to allocate Descriptor Set for '{}'", name);
        return;
    }

    write_descriptor_set->sType = VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET;
    write_descriptor_set->dstSet = descriptor_set;
    write_descriptor_set->dstBinding = resource->binding;
    write_descriptor_set->descriptorCount = resource->count;
    write_descriptor_set->descriptorType = toVkDescriptorType(resource->type);

    vkUpdateDescriptorSets(m_device->device(), 1, write_descriptor_set, 0, nullptr);

    queue->enqueue([this, set = resource->set, descriptor_set](void* cmd) {
        vkCmdBindDescriptorSets(toVkCommandBuffer(cmd), VK_PIPELINE_BIND_POINT_GRAPHICS,
                                m_pipeline_layout, set, 1, &descriptor_set, 0, nullptr);
    });
}

template<typename T>
void Pipeline::pushConstantIfValid(GPUCommandQueue* queue, const std::string& name, T value)
{
    if (const auto* push_constant = m_resources->pushConstant(name);
        push_constant != nullptr && isPushConstantValid(*push_constant, DATA_TYPE_SIZE<T>)) {
        pushConstantCmd(queue, *push_constant, value);
    }
}

template<typename T>
void Pipeline::pushConstantCmd(GPUCommandQueue* queue, const push_constant_t& push_constant,
                               T value)
{
    queue->enqueue([this, &push_constant, value](void* cmd_buffer) {
        vkCmdPushConstants(toVkCommandBuffer(cmd_buffer), m_pipeline_layout,
                           push_constant.pipeline_stages, push_constant.offset, push_constant.size,
                           &value);
    });
}

template<>
void Pipeline::pushConstantCmd<bool>(GPUCommandQueue* queue, const push_constant_t& push_constant,
                                     bool value)
{
    queue->enqueue([this, &push_constant, vk_value = toVkBool(value)](void* cmd_buffer) {
        vkCmdPushConstants(toVkCommandBuffer(cmd_buffer), m_pipeline_layout,
                           push_constant.pipeline_stages, push_constant.offset, push_constant.size,
                           &vk_value);
    });
}

template<>
void Pipeline::pushConstantCmd<const Vec3&>(GPUCommandQueue* queue,
                                            const push_constant_t& push_constant, const Vec3& value)
{
    queue->enqueue([this, &push_constant, &value](void* cmd_buffer) {
        vkCmdPushConstants(toVkCommandBuffer(cmd_buffer), m_pipeline_layout,
                           push_constant.pipeline_stages, push_constant.offset, push_constant.size,
                           value_ptr(value));
    });
}

template<>
void Pipeline::pushConstantCmd<const Mat4&>(GPUCommandQueue* queue,
                                            const push_constant_t& push_constant, const Mat4& value)
{
    queue->enqueue([this, &push_constant, &value](void* cmd_buffer) {
        vkCmdPushConstants(toVkCommandBuffer(cmd_buffer), m_pipeline_layout,
                           push_constant.pipeline_stages, push_constant.offset, push_constant.size,
                           value_ptr(value));
    });
}

void Pipeline::destroyVkHandles()
{
    m_device->waitIdle();

    vkDestroyPipeline(m_device->device(), m_pipeline, nullptr);
    m_pipeline = VK_NULL_HANDLE;

    vkDestroyPipelineLayout(m_device->device(), m_pipeline_layout, nullptr);
    m_pipeline_layout = VK_NULL_HANDLE;

    m_resources.reset();
}

} // namespace GE::Vulkan
