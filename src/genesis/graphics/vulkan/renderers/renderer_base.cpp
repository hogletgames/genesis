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
#include "device.h"
#include "texture.h"
#include "vulkan_exception.h"

namespace GE::Vulkan {

RendererBase::RendererBase(Shared<Device> device)
    : m_device{std::move(device)}
{
    createCommandPool();
    createPipelineCache();
}

RendererBase::~RendererBase()
{
    destroyVkHandles();
}

VkRenderPass
RendererBase::createRenderPass(const std::vector<VkAttachmentDescription>& descriptions)
{
    std::vector<VkAttachmentReference> color_refs;
    std::vector<VkAttachmentReference> depth_refs;

    for (uint32_t i{0}; i < descriptions.size(); i++) {
        VkAttachmentReference ref{};
        ref.attachment = i;

        if (isColorFormat(toTextureFormat(descriptions[i].format))) {
            ref.layout = VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL;
            color_refs.push_back(ref);
        } else {
            ref.layout = VK_IMAGE_LAYOUT_DEPTH_STENCIL_ATTACHMENT_OPTIMAL;
            depth_refs.push_back(ref);
        }
    }

    VkSubpassDescription subpass{};
    subpass.pipelineBindPoint = VK_PIPELINE_BIND_POINT_GRAPHICS;
    subpass.colorAttachmentCount = color_refs.size();
    subpass.pColorAttachments = color_refs.data();
    subpass.pDepthStencilAttachment = depth_refs.data();

    std::array<VkSubpassDependency, 2> dependencies{};

    dependencies[0].srcSubpass = VK_SUBPASS_EXTERNAL;
    dependencies[0].dstSubpass = 0;
    dependencies[0].srcStageMask = VK_PIPELINE_STAGE_FRAGMENT_SHADER_BIT;
    dependencies[0].dstStageMask = VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT;
    dependencies[0].srcAccessMask = VK_ACCESS_SHADER_READ_BIT;
    dependencies[0].dstAccessMask = VK_ACCESS_COLOR_ATTACHMENT_WRITE_BIT;
    dependencies[0].dependencyFlags = VK_DEPENDENCY_BY_REGION_BIT;

    dependencies[1].srcSubpass = 0;
    dependencies[1].dstSubpass = VK_SUBPASS_EXTERNAL;
    dependencies[1].srcStageMask = VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT;
    dependencies[1].dstStageMask = VK_PIPELINE_STAGE_FRAGMENT_SHADER_BIT;
    dependencies[1].srcAccessMask = VK_ACCESS_COLOR_ATTACHMENT_WRITE_BIT;
    dependencies[1].dstAccessMask = VK_ACCESS_SHADER_READ_BIT;
    dependencies[1].dependencyFlags = VK_DEPENDENCY_BY_REGION_BIT;

    VkRenderPassCreateInfo render_pass_info{};
    render_pass_info.sType = VK_STRUCTURE_TYPE_RENDER_PASS_CREATE_INFO;
    render_pass_info.attachmentCount = descriptions.size();
    render_pass_info.pAttachments = descriptions.data();
    render_pass_info.subpassCount = 1;
    render_pass_info.pSubpasses = &subpass;
    render_pass_info.dependencyCount = dependencies.size();
    render_pass_info.pDependencies = dependencies.data();

    VkRenderPass render_pass{};

    if (vkCreateRenderPass(m_device->device(), &render_pass_info, nullptr,
                           &render_pass) != VK_SUCCESS) {
        throw Vulkan::Exception{"Failed to create Render Pass"};
    }

    return render_pass;
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

    if (vkCreatePipelineCache(m_device->device(), &create_info, nullptr,
                              &m_pipeline_cache) != VK_SUCCESS) {
        throw Vulkan::Exception{"Failed to create Pipeline Cache"};
    }
}

void RendererBase::destroyVkHandles()
{
    m_device->waitIdle();

    vkDestroyPipelineCache(m_device->device(), m_pipeline_cache, nullptr);
    m_pipeline_cache = VK_NULL_HANDLE;

    vkDestroyCommandPool(m_device->device(), m_command_pool, nullptr);
    m_command_pool = VK_NULL_HANDLE;
    m_cmd_buffers.clear();

    for (auto* render_pass : m_render_passes) {
        vkDestroyRenderPass(m_device->device(), render_pass, nullptr);
        render_pass = VK_NULL_HANDLE;
    }

    m_device.reset();
}

bool RendererBase::beginRenderPass(ClearMode clear_mode)
{
    VkCommandBuffer cmd = cmdBuffer();

    VkCommandBufferBeginInfo cmd_buffer_info{};
    cmd_buffer_info.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_BEGIN_INFO;
    cmd_buffer_info.flags = VK_COMMAND_BUFFER_USAGE_SIMULTANEOUS_USE_BIT;

    if (vkBeginCommandBuffer(cmd, &cmd_buffer_info) != VK_SUCCESS) {
        GE_CORE_ERR("Failed to begin Command Buffer");
        return false;
    }

    VkRenderPassBeginInfo render_pass_info{};
    render_pass_info.sType = VK_STRUCTURE_TYPE_RENDER_PASS_BEGIN_INFO;
    render_pass_info.renderPass = m_render_passes[clear_mode];
    render_pass_info.framebuffer = currentFramebuffer();
    render_pass_info.renderArea.offset = {0, 0};
    render_pass_info.renderArea.extent = extent();
    render_pass_info.clearValueCount = m_clear_values.size();
    render_pass_info.pClearValues = m_clear_values.data();

    vkCmdBeginRenderPass(cmd, &render_pass_info, VK_SUBPASS_CONTENTS_INLINE);
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

bool RendererBase::endRenderPass()
{
    VkCommandBuffer cmd = cmdBuffer();
    vkCmdEndRenderPass(cmd);

    if (vkEndCommandBuffer(cmd) != VK_SUCCESS) {
        GE_CORE_ERR("Failed to end Render Pass");
        return false;
    }

    return true;
}

} // namespace GE::Vulkan
