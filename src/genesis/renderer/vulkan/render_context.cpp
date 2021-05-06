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

    m_renderer_factory.reset();

    m_pipeline.reset();
    destroyPipelineLayout();
    m_swap_chain.reset();
    m_device.reset();

    destroyVulkanHandles();
    Instance::dropContext(this);
    m_window.reset();
}

void RenderContext::destroyVulkanHandles()
{
    vkDestroySurfaceKHR(Instance::getInstance(), m_surface, nullptr);
    m_surface = VK_NULL_HANDLE;
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
