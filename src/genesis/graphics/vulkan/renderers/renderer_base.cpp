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

#include "renderer_base.h"
#include "command_buffer.h"
#include "descriptor_pool.h"
#include "device.h"
#include "texture.h"
#include "utils.h"
#include "vulkan_exception.h"

namespace GE::Vulkan {

RendererBase::RendererBase(Shared<Device> device)
    : m_device{std::move(device)}
    , m_descriptor_pool{makeShared<DescriptorPool>(m_device)}
{
    createCommandPool();
    createPipelineCache();
}

RendererBase::~RendererBase()
{
    destroyVkHandles();
}

void RendererBase::createCommandPool()
{
    VkCommandPoolCreateInfo create_info{};
    create_info.sType = VK_STRUCTURE_TYPE_COMMAND_POOL_CREATE_INFO;
    create_info.queueFamilyIndex = m_device->queueIndices().graphics_family.value();
    create_info.flags = VK_COMMAND_POOL_CREATE_RESET_COMMAND_BUFFER_BIT;

    if (vkCreateCommandPool(m_device->device(), &create_info, nullptr, &m_command_pool) !=
        VK_SUCCESS) {
        throw Vulkan::Exception{"Failed to create Command Pool"};
    }
}

void RendererBase::createCommandBuffers(uint32_t count)
{
    m_cmd_buffers.resize(count);

    VkCommandBufferAllocateInfo alloc_info{};
    alloc_info.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_ALLOCATE_INFO;
    alloc_info.level = VK_COMMAND_BUFFER_LEVEL_PRIMARY;
    alloc_info.commandPool = m_command_pool;
    alloc_info.commandBufferCount = count;

    if (vkAllocateCommandBuffers(m_device->device(), &alloc_info, m_cmd_buffers.data()) !=
        VK_SUCCESS) {
        throw Vulkan::Exception{"Failed to allocate Command Buffers"};
    }
}

void RendererBase::createPipelineCache()
{
    VkPipelineCacheCreateInfo create_info{};
    create_info.sType = VK_STRUCTURE_TYPE_PIPELINE_CACHE_CREATE_INFO;

    if (vkCreatePipelineCache(m_device->device(), &create_info, nullptr, &m_pipeline_cache) !=
        VK_SUCCESS) {
        throw Vulkan::Exception{"Failed to create Pipeline Cache"};
    }
}

bool RendererBase::beginRendering(ClearMode clear_mode)
{
    VkCommandBuffer cmd = cmdBuffer();

    VkCommandBufferBeginInfo cmd_buffer_info{};
    cmd_buffer_info.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_BEGIN_INFO;
    cmd_buffer_info.flags = VK_COMMAND_BUFFER_USAGE_SIMULTANEOUS_USE_BIT;

    if (vkBeginCommandBuffer(cmd, &cmd_buffer_info) != VK_SUCCESS) {
        GE_CORE_ERR("Failed to begin Command Buffer");
        return false;
    }

    transitImageLayoutBeforeRendering(cmd);

    const auto& colorAttachments = colorRenderingAttachments(clear_mode);
    const auto& depthAttachment = depthRenderingAttachment(clear_mode);

    VkRenderingInfoKHR rendering_info{};
    rendering_info.sType = VK_STRUCTURE_TYPE_RENDERING_INFO;
    rendering_info.renderArea = {{0, 0}, extent()};
    rendering_info.layerCount = 1;
    rendering_info.colorAttachmentCount = colorAttachments.size();
    rendering_info.pColorAttachments = colorAttachments.data();

    if (depthAttachment.has_value()) {
        rendering_info.pDepthAttachment = &depthAttachment.value();
    }

    cmdBeginRendering(cmd, &rendering_info);
    return true;
}

void RendererBase::updateDynamicState()
{
    VkCommandBuffer cmd = cmdBuffer();

    VkViewport dynamic_viewport = viewport();
    vkCmdSetViewport(cmd, 0, 1, &dynamic_viewport);

    VkRect2D scissor = {{0, 0}, extent()};
    vkCmdSetScissor(cmd, 0, 1, &scissor);
}

bool RendererBase::endRendering()
{
    VkCommandBuffer cmd = cmdBuffer();

    cmdEndRendering(cmd);
    transitImageLayoutAfterRendering(cmd);

    if (vkEndCommandBuffer(cmd) != VK_SUCCESS) {
        GE_CORE_ERR("Failed to end Command Buffer");
        return false;
    }

    return true;
}

void RendererBase::draw(GPUCommandQueue* queue, uint32_t vertex_count, uint32_t instance_count,
                        uint32_t first_vertex, uint32_t first_instance)
{
    queue->enqueue([vertex_count, instance_count, first_vertex, first_instance](void* cmd) {
        vkCmdDraw(toVkCommandBuffer(cmd), vertex_count, instance_count, first_vertex,
                  first_instance);
    });
}

void RendererBase::destroyVkHandles()
{
    m_device->waitIdle();

    vkDestroyPipelineCache(m_device->device(), m_pipeline_cache, nullptr);
    m_pipeline_cache = VK_NULL_HANDLE;

    vkDestroyCommandPool(m_device->device(), m_command_pool, nullptr);
    m_command_pool = VK_NULL_HANDLE;
    m_cmd_buffers.clear();

    m_device.reset();
}

VkClearValue toVkClearColorValue(const GE::ClearColorType& clear_color)
{
    VkClearColorValue vk_clear_color{};

    if (std::holds_alternative<Vec4>(clear_color)) {
        const auto& value = std::get<Vec4>(clear_color);
        std::copy_n(value_ptr(value), Vec4::length(), vk_clear_color.float32);
    } else if (std::holds_alternative<IVec4>(clear_color)) {
        const auto& value = std::get<IVec4>(clear_color);
        std::copy_n(value_ptr(value), IVec4::length(), vk_clear_color.int32);
    } else if (std::holds_alternative<UVec4>(clear_color)) {
        const auto& value = std::get<UVec4>(clear_color);
        std::copy_n(value_ptr(value), UVec4::length(), vk_clear_color.uint32);
    }

    return VkClearValue{vk_clear_color};
}

} // namespace GE::Vulkan
