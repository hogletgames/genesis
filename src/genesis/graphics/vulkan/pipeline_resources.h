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

#pragma once

#include <genesis/core/memory.h>
#include <genesis/graphics/pipeline.h>
#include <genesis/graphics/shader_resource_descriptors.h>

#include <vulkan/vulkan.h>

#include <optional>
#include <unordered_map>
#include <vector>

namespace GE::Vulkan {

class DescriptorPool;
class Device;
struct pipeline_config_t;

class PipelineResources
{
public:
    using DescriptorSetLayouts = std::vector<VkDescriptorSetLayout>;
    using ResourcesPerState = std::pair<GE::ResourceDescriptors, VkShaderStageFlags>;
    using Resources = std::vector<ResourcesPerState>;

    PipelineResources(Shared<Device> device, const Vulkan::pipeline_config_t& pipeline_config);
    ~PipelineResources();

    std::optional<resource_descriptor_t> resource(const std::string& name) const;
    VkDescriptorSet descriptorSet(uint32_t set) const;
    const DescriptorSetLayouts& descriptorSetLayouts() const { return m_descriptor_set_layouts; }

    VkDescriptorSetLayout descriptorSetLayout(uint32_t set) const
    {
        return m_descriptor_set_layouts.at(set);
    }

private:
    using BindingVector = std::vector<VkDescriptorSetLayoutBinding>;
    using SetLayoutBindings = std::vector<BindingVector>;

    void createDescriptorSetLayouts(const Vulkan::pipeline_config_t& pipeline_config);
    SetLayoutBindings createSetLayoutBindings(const Resources& descriptor_resources);

    void destroyVkHandles();

    Shared<Device> m_device;
    Shared<DescriptorPool> m_descriptor_pool;
    DescriptorSetLayouts m_descriptor_set_layouts;
    std::unordered_map<std::string, resource_descriptor_t> m_resources;
};

constexpr VkDescriptorType toVkDescriptorType(resource_descriptor_t::Type type)
{
    using Type = resource_descriptor_t::Type;

    switch (type) {
        case Type::UNIFORM_BUFFER: return VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER;
        case Type::COMBINED_IMAGE_SAMPLER: return VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER;
        default: break;
    }

    return VK_DESCRIPTOR_TYPE_MAX_ENUM;
}

} // namespace GE::Vulkan
