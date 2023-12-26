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

#include "descriptor_pool.h"
#include "device.h"
#include "vulkan_exception.h"

#include <array>

namespace {

constexpr size_t DESCRIPTOR_TYPE_COUNT{11};

using SizeMultipliers = std::array<std::pair<VkDescriptorType, float>, DESCRIPTOR_TYPE_COUNT>;

constexpr SizeMultipliers SIZE_MULTIPLIERS = {
    std::make_pair(VK_DESCRIPTOR_TYPE_SAMPLER, 0.5f),
    std::make_pair(VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER, 4.0f),
    std::make_pair(VK_DESCRIPTOR_TYPE_SAMPLED_IMAGE, 4.0f),
    std::make_pair(VK_DESCRIPTOR_TYPE_STORAGE_IMAGE, 1.0f),
    std::make_pair(VK_DESCRIPTOR_TYPE_UNIFORM_TEXEL_BUFFER, 1.0f),
    std::make_pair(VK_DESCRIPTOR_TYPE_STORAGE_TEXEL_BUFFER, 1.0f),
    std::make_pair(VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER, 2.0f),
    std::make_pair(VK_DESCRIPTOR_TYPE_STORAGE_BUFFER, 2.0f),
    std::make_pair(VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER_DYNAMIC, 1.0f),
    std::make_pair(VK_DESCRIPTOR_TYPE_STORAGE_BUFFER_DYNAMIC, 1.0f),
    std::make_pair(VK_DESCRIPTOR_TYPE_INPUT_ATTACHMENT, 0.05f),
};

std::vector<VkDescriptorPoolSize> createSizes(uint32_t max_count)
{
    std::vector<VkDescriptorPoolSize> sizes{SIZE_MULTIPLIERS.size()};

    for (uint32_t i{0}; i < SIZE_MULTIPLIERS.size(); i++) {
        sizes[i].type = SIZE_MULTIPLIERS[i].first;
        sizes[i].descriptorCount = static_cast<uint32_t>(SIZE_MULTIPLIERS[i].second * max_count);
    }

    return sizes;
}

} // namespace

namespace GE::Vulkan {

DescriptorPool::DescriptorPool(Shared<Device> device, uint32_t count)
    : m_device{std::move(device)}
    , m_sizes{createSizes(count)}
    , m_max_count{count}
{}

DescriptorPool::~DescriptorPool()
{
    destroyVkHandles();
}

VkDescriptorSet DescriptorPool::allocateDescriptorSet(VkDescriptorSetLayout layout)
{
    VkDescriptorSet set{VK_NULL_HANDLE};
    auto* pool = getPool();
    auto result = allocateDescriptorSet(pool, layout, &set);

    if (result == VK_SUCCESS) {
        return set;
    }

    if (result == VK_ERROR_FRAGMENTED_POOL || result == VK_ERROR_OUT_OF_POOL_MEMORY) {
        pool = createPool();
        m_pools.push_back(pool);
    }

    if (allocateDescriptorSet(pool, layout, &set) != VK_SUCCESS) {
        throw Vulkan::Exception("Failed to allocate Descriptor Set");
    }

    return set;
}

void DescriptorPool::reset()
{
    std::for_each(m_pools.begin(), m_pools.end(),
                  [this](auto pool) { vkResetDescriptorPool(m_device->device(), pool, 0); });
}

VkResult DescriptorPool::allocateDescriptorSet(VkDescriptorPool pool, VkDescriptorSetLayout layout,
                                               VkDescriptorSet* set)
{
    VkDescriptorSetAllocateInfo alloc_info{};
    alloc_info.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_SET_ALLOCATE_INFO;
    alloc_info.descriptorPool = pool;
    alloc_info.pSetLayouts = &layout;
    alloc_info.descriptorSetCount = 1;

    return vkAllocateDescriptorSets(m_device->device(), &alloc_info, set);
}

VkDescriptorPool DescriptorPool::getPool()
{
    if (m_pools.empty()) {
        m_pools.push_back(createPool());
    }

    return m_pools.back();
}

VkDescriptorPool DescriptorPool::createPool()
{
    VkDescriptorPoolCreateInfo create_info{};
    create_info.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_POOL_CREATE_INFO;
    create_info.maxSets = m_max_count;
    create_info.poolSizeCount = m_sizes.size();
    create_info.pPoolSizes = m_sizes.data();

    VkDescriptorPool pool{VK_NULL_HANDLE};

    if (vkCreateDescriptorPool(m_device->device(), &create_info, nullptr, &pool) != VK_SUCCESS) {
        throw Vulkan::Exception("Failed to create Descriptor Pool");
    }

    return pool;
}

void DescriptorPool::destroyVkHandles()
{
    for (auto& pool : m_pools) {
        vkDestroyDescriptorPool(m_device->device(), pool, nullptr);
    }

    m_pools.clear();
}

} // namespace GE::Vulkan
