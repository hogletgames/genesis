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

#include "render_context.h"
#include "device.h"
#include "instance.h"
#include "pipeline.h"
#include "renderer_factory.h"
#include "sdl_platform_window.h"
#include "swap_chain.h"
#include "utils.h"
#include "vulkan_exception.h"

#include "genesis/core/format.h"

namespace GE::Vulkan {

RenderContext::RenderContext() = default;

RenderContext::~RenderContext() = default;

bool RenderContext::initialize(void* window)
{
    GE_CORE_INFO("Initializing Vulkan Context...");

    try {
        m_window = makeScoped<SDL::PlatformWindow>(reinterpret_cast<SDL_Window*>(window));
        Instance::registerContext(this);
        m_surface = m_window->createSurface(Instance::getInstance());

        m_device = makeScoped<Device>(this);
        m_swap_chain = makeScoped<SwapChain>(m_device, m_surface);
        m_pipeline_layout = createPipelineLayout();
        m_pipeline = makeScoped<Pipeline>(m_device, createPipelineConfig());
        createCommandBuffers();

        m_renderer_factory = makeScoped<Vulkan::RendererFactoryImpl>(m_device);
    } catch (const Vulkan::Exception& e) {
        GE_CORE_ERR("Failed to initialize Vulkan Render Context: {}", e.what());
        shutdown();
        return false;
    }

    return true;
}

void RenderContext::shutdown()
{
    GE_CORE_INFO("Shutdown Vulkan Context");

    vkDeviceWaitIdle(m_device->getDevice());

    m_renderer_factory.reset();

    destroyCommandBuffers();
    m_pipeline.reset();
    destroyPipelineLayout();
    m_swap_chain.reset();
    m_device.reset();

    destroyVulkanHandles();
    Instance::dropContext(this);
    m_window.reset();
}

void RenderContext::drawFrame()
{
    auto [acquire_result, image_index] = m_swap_chain->acquireNextImage();

    if (acquire_result != VK_SUCCESS && acquire_result != VK_SUBOPTIMAL_KHR) {
        GE_CORE_ERR("Failed to acquire Swap Chain image");
        return;
    }

    VkResult submit_result{VK_ERROR_UNKNOWN};

    try {
        submit_result = m_swap_chain->submitCommandBuffer(&m_command_buffers[image_index],
                                                          image_index);
    } catch (const Vulkan::Exception& e) {
        GE_CORE_ERR("Failed to present Swap Chain image: {}", e.what());
        return;
    }

    if (submit_result != VK_SUCCESS) {
        GE_CORE_ERR("Failed to present Swap Chain image");
    }
}

void RenderContext::destroyVulkanHandles()
{
    vkDestroySurfaceKHR(Instance::getInstance(), m_surface, nullptr);
    m_surface = VK_NULL_HANDLE;
}

void RenderContext::createCommandBuffers()
{
    m_command_buffers.resize(m_swap_chain->getImageCount(), VK_NULL_HANDLE);

    VkCommandBufferAllocateInfo alloc_info{};
    alloc_info.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_ALLOCATE_INFO;
    alloc_info.level = VK_COMMAND_BUFFER_LEVEL_PRIMARY;
    alloc_info.commandPool = m_device->getCommandPool();
    alloc_info.commandBufferCount = m_command_buffers.size();

    if (vkAllocateCommandBuffers(m_device->getDevice(), &alloc_info,
                                 m_command_buffers.data()) != VK_SUCCESS) {
        throw Vulkan::Exception{"Failed to allocate Command Buffers"};
    }

    for (size_t i{0}; i < m_command_buffers.size(); i++) {
        VkCommandBufferBeginInfo begin_info{};
        begin_info.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_BEGIN_INFO;

        if (vkBeginCommandBuffer(m_command_buffers[i], &begin_info) != VK_SUCCESS) {
            throw Vulkan::Exception{"Failed to begin recording to Command Buffer"};
        }

        VkRenderPassBeginInfo render_pass{};
        render_pass.sType = VK_STRUCTURE_TYPE_RENDER_PASS_BEGIN_INFO;
        render_pass.renderPass = m_swap_chain->getRenderPass();
        render_pass.framebuffer = m_swap_chain->getFramebuffer(i);
        render_pass.renderArea.offset = {0, 0};
        render_pass.renderArea.extent = m_swap_chain->getExtent();

        constexpr std::array<float, 4> clear_color = {0.1f, 0.1f, 0.1f, 1.0f};
        std::array<VkClearValue, 2> clear_values{};
        std::copy(clear_color.begin(), clear_color.end(), clear_values[0].color.float32);
        clear_values[1].depthStencil = {1.0f, 0};
        render_pass.pClearValues = clear_values.data();
        render_pass.clearValueCount = clear_values.size();

        vkCmdBeginRenderPass(m_command_buffers[i], &render_pass,
                             VK_SUBPASS_CONTENTS_INLINE);

        m_pipeline->bind(m_command_buffers[i]);
        vkCmdDraw(m_command_buffers[i], 3, 1, 0, 0);

        vkCmdEndRenderPass(m_command_buffers[i]);

        if (vkEndCommandBuffer(m_command_buffers[i]) != VK_SUCCESS) {
            throw Vulkan::Exception{"Failed to record Command Buffer"};
        }
    }
}

void RenderContext::destroyCommandBuffers()
{
    vkFreeCommandBuffers(m_device->getDevice(), m_device->getCommandPool(),
                         m_command_buffers.size(), m_command_buffers.data());
    m_command_buffers = {};
}

VkPipelineLayout RenderContext::createPipelineLayout()
{
    VkPipelineLayout pipeline_layout{VK_NULL_HANDLE};
    VkPipelineLayoutCreateInfo pipeline_layout_info{};
    pipeline_layout_info.sType = VK_STRUCTURE_TYPE_PIPELINE_LAYOUT_CREATE_INFO;
    pipeline_layout_info.pSetLayouts = nullptr;
    pipeline_layout_info.setLayoutCount = 0;
    pipeline_layout_info.pPushConstantRanges = nullptr;
    pipeline_layout_info.pushConstantRangeCount = 0;

    if (vkCreatePipelineLayout(m_device->getDevice(), &pipeline_layout_info, nullptr,
                               &pipeline_layout) != VK_SUCCESS) {
        throw Vulkan::Exception{"Failed to create Pipeline Layout"};
    }

    return pipeline_layout;
}

pipeline_config_t RenderContext::createPipelineConfig()
{
    auto config = Pipeline::makeDefaultConfig(m_window->getSize());
    config.vert_shader_path = "examples/sandbox/assets/shaders/shader.vert";
    config.frag_shader_path = "examples/sandbox/assets/shaders/shader.frag";
    config.viewport.width = m_swap_chain->getExtent().width;
    config.viewport.height = m_swap_chain->getExtent().height;
    config.pipeline_layout = m_pipeline_layout;
    config.render_pass = m_swap_chain->getRenderPass();
    return config;
}

void RenderContext::destroyPipelineLayout()
{
    vkDestroyPipelineLayout(m_device->getDevice(), m_pipeline_layout, nullptr);
    m_pipeline_layout = VK_NULL_HANDLE;
}

} // namespace GE::Vulkan
