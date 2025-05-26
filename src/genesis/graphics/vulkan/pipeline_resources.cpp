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

#include "pipeline_resources.h"
#include "descriptor_pool.h"
#include "device.h"
#include "pipeline_config.h"
#include "vulkan_exception.h"

#include "genesis/core/utils.h"
#include "genesis/graphics/shader.h"

namespace {

class SetLayoutBindingBuilder
{
public:
    explicit SetLayoutBindingBuilder(const GE::resource_descriptor_t& desc)
    {
        m_binding.binding = desc.binding;
        m_binding.descriptorType = GE::Vulkan::toVkDescriptorType(desc.type);
        m_binding.descriptorCount = desc.count;
        m_binding.pImmutableSamplers = nullptr;
    }

    SetLayoutBindingBuilder& stage(VkShaderStageFlags stage)
    {
        m_binding.stageFlags = stage;
        return *this;
    }

    VkDescriptorSetLayoutBinding build() { return m_binding; }

private:
    VkDescriptorSetLayoutBinding m_binding{};
};

uint32_t maxSetValue(const GE::Vulkan::PipelineResources::Resources& descriptor_resources)
{
    uint32_t max_set{0};

    for (const auto& [resources, _] : descriptor_resources) {
        for (const auto& resource : resources) {
            max_set = std::max(max_set, resource.set);
        }
    }

    return max_set;
}

} // namespace

namespace GE::Vulkan {

PipelineResources::PipelineResources(Shared<Device>                   device,
                                     const Vulkan::pipeline_config_t& pipeline_config)
    : m_device{std::move(device)}
    , m_descriptor_pool{pipeline_config.descriptor_pool}
{
    createDescriptorSetLayouts(pipeline_config);
    updatePushConstants(VK_SHADER_STAGE_VERTEX_BIT, pipeline_config.vertex_shader->pushConstants());
    updatePushConstants(VK_SHADER_STAGE_FRAGMENT_BIT,
                        pipeline_config.fragment_shader->pushConstants());
    createPushConstantRanges();
}

PipelineResources::~PipelineResources()
{
    destroyVkHandles();
}

std::optional<resource_descriptor_t> PipelineResources::resource(const std::string& name) const
{
    if (auto it = m_resources.find(name); it != m_resources.end()) {
        return it->second;
    }

    GE_CORE_ERR("Failed to find pipeline resource: {}", name);
    return {};
}

VkDescriptorSet PipelineResources::descriptorSet(const resource_descriptor_t& set_descriptor)
{
    auto*    set_layout = m_descriptor_set_layouts[set_descriptor.set];
    uint32_t binding = set_descriptor.binding;

    // This is a hack to reuse descriptor sets created for multi-binded resources. That
    // way. we still create a separate descriptor set for each resources unless there are many
    // bindings within the descriptor set. This hack only works when the resources are bound in
    // order starting from the binding = 0.

    try {
        return m_descriptor_pool->allocateDescriptorSet(set_layout, binding > 0);
    } catch (const GE::Exception& e) {
        GE_CORE_ERR("Failed to allocate '{}' descriptor set for: '{}'", set_descriptor.name,
                    e.what());
        return VK_NULL_HANDLE;
    }
}

const push_constant_t* PipelineResources::pushConstant(const std::string& name) const
{
    if (auto it = m_push_constants.find(name); it != m_push_constants.end()) {
        return &it->second;
    }

    GE_CORE_ERR("Failed to find push constant: {}", name);
    return nullptr;
}

void PipelineResources::createDescriptorSetLayouts(const Vulkan::pipeline_config_t& pipeline_config)
{
    auto bindings = createSetLayoutBindings({
        {pipeline_config.vertex_shader->resourceDescriptors(), VK_SHADER_STAGE_VERTEX_BIT},
        {pipeline_config.fragment_shader->resourceDescriptors(), VK_SHADER_STAGE_FRAGMENT_BIT},
    });

    if (bindings.empty()) {
        return;
    }

    m_descriptor_set_layouts.resize(bindings.size());

    for (size_t i{0}; i < m_descriptor_set_layouts.size(); i++) {
        VkDescriptorSetLayoutCreateInfo layout_info{};
        layout_info.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_SET_LAYOUT_CREATE_INFO;
        layout_info.bindingCount = bindings[i].size();
        layout_info.pBindings = bindings[i].data();

        if (vkCreateDescriptorSetLayout(m_device->device(), &layout_info, nullptr,
                                        &m_descriptor_set_layouts[i]) != VK_SUCCESS) {
            throw Vulkan::Exception{"Failed to create Descriptor Set Layout!"};
        }
    }
}

PipelineResources::SetLayoutBindings
PipelineResources::createSetLayoutBindings(const Resources& descriptor_resources)
{
    SetLayoutBindings bindings(maxSetValue(descriptor_resources) + 1);

    for (const auto& [resources, stage] : descriptor_resources) {
        for (const auto& resource : resources) {
            auto binding = SetLayoutBindingBuilder{resource}.stage(stage).build();
            bindings[resource.set].emplace_back(binding);
            m_resources.emplace(resource.name, resource);
        }
    }

    return bindings;
}

void PipelineResources::updatePushConstants(VkShaderStageFlagBits shader_stage,
                                            const PushConstants&  push_constants)
{
    for (const auto& push_constant : push_constants) {
        const auto& name = push_constant.name;

        if (!m_push_constants.contains(name)) {
            m_push_constants[name] = push_constant;
        }

        m_push_constants[name].pipeline_stages |= shader_stage;
    }
}

void PipelineResources::createPushConstantRanges()
{
    std::unordered_map<VkShaderStageFlags, std::pair<uint32_t, uint32_t>> range_offsets;

    for (const auto& [_, push_constant] : m_push_constants) {
        auto& [offset, size] = range_offsets[push_constant.pipeline_stages];
        offset = std::min(offset, push_constant.offset);
        size = std::max(size, push_constant.offset + push_constant.size);
    }

    for (const auto& [stages, offsets] : range_offsets) {
        VkPushConstantRange range{};
        range.stageFlags = stages;
        range.offset = offsets.first;
        range.size = offsets.second;

        m_push_constant_ranges.push_back(range);
    }
}

void PipelineResources::destroyVkHandles()
{
    for (auto* descriptor_set_layout : m_descriptor_set_layouts) {
        vkDestroyDescriptorSetLayout(m_device->device(), descriptor_set_layout, nullptr);
    }

    m_descriptor_set_layouts.clear();
}

} // namespace GE::Vulkan
