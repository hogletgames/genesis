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

#include "graphics_context.h"
#include "device.h"
#include "graphics_factory.h"
#include "instance.h"
#include "sdl_gui_context.h"
#include "sdl_platform_window.h"
#include "swap_chain.h"
#include "vulkan_exception.h"

#include "genesis/core/asserts.h"
#include "genesis/core/log.h"
#include "genesis/graphics/graphics.h"
#include "genesis/graphics/render_command.h"

namespace GE::Vulkan {

GraphicsContext::GraphicsContext() = default;

GraphicsContext::~GraphicsContext() = default;

bool GraphicsContext::initialize(void* window)
{
    GE_CORE_INFO("Initializing Vulkan Context...");

    try {
        m_window = makeScoped<SDL::PlatformWindow>(reinterpret_cast<SDL_Window*>(window));
        Instance::registerContext(this);
        m_surface = m_window->createSurface(Instance::instance());

        m_device = makeScoped<Device>(this);
        m_swap_chain = makeScoped<SwapChain>(m_device, m_surface);
        createCommandBuffers();

        m_factory = makeScoped<Vulkan::GraphicsFactory>(m_device);
        m_gui = makeScoped<SDL::GUIContext>(this, m_window->window());
    } catch (const Vulkan::Exception& e) {
        GE_CORE_ERR("Failed to initialize Vulkan Render Context: {}", e.what());
        shutdown();
        return false;
    }

    return true;
}

void GraphicsContext::shutdown()
{
    GE_CORE_INFO("Shutdown Vulkan Context");

    if (m_device) {
        m_device->waitIdle();
        destroyCommandBuffers();
    }

    m_gui.reset();
    m_factory.reset();

    m_swap_chain.reset();
    m_device.reset();

    destroyVulkanHandles();
    Instance::dropContext(this);
    m_window.reset();
    m_gui.reset();
}

void GraphicsContext::drawFrame()
{
    if (!m_swap_chain &&
        !(m_swap_chain = tryMakeScoped<SwapChain>(m_device, m_surface))) {
        return;
    }

    auto [acquire_result, image_index] = m_swap_chain->acquireNextImage();

    if (acquire_result == VK_ERROR_OUT_OF_DATE_KHR) {
        m_device->waitIdle();
        m_swap_chain.reset();
        return;
    }

    if (acquire_result != VK_SUCCESS && acquire_result != VK_SUBOPTIMAL_KHR) {
        GE_CORE_ERR("Failed to acquire Swap Chain image");
        return;
    }

    if (!prepareRenderCommand(image_index)) {
        GE_CORE_ERR("Failed to prepare Render Command");
        return;
    }

    if (m_swap_chain->submitCommandBuffer(&m_command_buffers[image_index], image_index) !=
        VK_SUCCESS) {
        GE_CORE_ERR("Failed to present Swap Chain image");
        return;
    }
}

void GraphicsContext::destroyVulkanHandles()
{
    if (Instance::instance() != VK_NULL_HANDLE) {
        vkDestroySurfaceKHR(Instance::instance(), m_surface, nullptr);
        m_surface = VK_NULL_HANDLE;
    }
}

void GraphicsContext::createCommandBuffers()
{
    m_command_buffers.resize(m_swap_chain->getImageCount(), VK_NULL_HANDLE);

    VkCommandBufferAllocateInfo alloc_info{};
    alloc_info.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_ALLOCATE_INFO;
    alloc_info.level = VK_COMMAND_BUFFER_LEVEL_PRIMARY;
    alloc_info.commandPool = m_device->commandPool();
    alloc_info.commandBufferCount = m_command_buffers.size();

    if (vkAllocateCommandBuffers(m_device->device(), &alloc_info,
                                 m_command_buffers.data()) != VK_SUCCESS) {
        throw Vulkan::Exception{"Failed to allocate Command Buffers"};
    }
}

void GraphicsContext::destroyCommandBuffers()
{
    vkFreeCommandBuffers(m_device->device(), m_device->commandPool(),
                         m_command_buffers.size(), m_command_buffers.data());
    m_command_buffers = {};
}

bool GraphicsContext::prepareRenderCommand(uint32_t image_idx)
{
    if (!beginRenderCommand(image_idx)) {
        return false;
    }

    RenderCommand::submit(m_command_buffers[image_idx]);
    return endRenderCommand(image_idx);
}

bool GraphicsContext::beginRenderCommand(uint32_t image_idx)
{
    VkCommandBuffer cmd = m_command_buffers[image_idx];

    vkResetCommandBuffer(cmd, 0);

    VkCommandBufferBeginInfo begin_info{};
    begin_info.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_BEGIN_INFO;
    begin_info.flags = 0;
    begin_info.pInheritanceInfo = nullptr;

    if (vkBeginCommandBuffer(cmd, &begin_info) != VK_SUCCESS) {
        GE_CORE_ERR("Failed to begin Command Buffer");
        return false;
    }

    setRenderPass(cmd, currentFBO(image_idx));
    setViewportAndScissor(cmd);
    return true;
}

void GraphicsContext::setRenderPass(VkCommandBuffer cmd, VkFramebuffer fbo)
{
    std::array<VkClearValue, 2> clear_values{};
    std::array<float, 4> clear_color = {0.0f, 0.0f, 0.0f, 1.0f};
    std::copy(clear_color.begin(), clear_color.end(), clear_values[0].color.float32);
    clear_values[1].depthStencil = {1.0f, 0};

    VkRenderPassBeginInfo render_pass_info{};
    render_pass_info.sType = VK_STRUCTURE_TYPE_RENDER_PASS_BEGIN_INFO;
    render_pass_info.renderPass = m_swap_chain->getRenderPass();
    render_pass_info.framebuffer = fbo;
    render_pass_info.renderArea.offset = {0, 0};
    render_pass_info.renderArea.extent = m_swap_chain->getExtent();
    render_pass_info.pClearValues = clear_values.data();
    render_pass_info.clearValueCount = clear_values.size();

    vkCmdBeginRenderPass(cmd, &render_pass_info, VK_SUBPASS_CONTENTS_INLINE);
}

void GraphicsContext::setViewportAndScissor(VkCommandBuffer cmd)
{
    VkViewport viewport{};
    viewport.width = static_cast<float>(m_swap_chain->getExtent().width);
    viewport.height = static_cast<float>(m_swap_chain->getExtent().height);
    viewport.minDepth = 0.0f;
    viewport.maxDepth = 1.0f;

    VkRect2D scissor{};
    scissor.extent = m_swap_chain->getExtent();

    vkCmdSetViewport(cmd, 0, 1, &viewport);
    vkCmdSetScissor(cmd, 0, 1, &scissor);
}

bool GraphicsContext::endRenderCommand(uint32_t image_idx)
{
    VkCommandBuffer cmd = m_command_buffers[image_idx];
    vkCmdEndRenderPass(cmd);

    if (vkEndCommandBuffer(cmd) != VK_SUCCESS) {
        GE_CORE_ERR("Failed to record Command Buffer");
        return false;
    }

    return true;
}

VkFramebuffer GraphicsContext::currentFBO(uint32_t image_idx)
{
    return m_swap_chain->getFramebuffer(image_idx);
}

VkRenderPass GraphicsContext::renderPass()
{
    return m_swap_chain->getRenderPass();
}

Shared<Vulkan::GraphicsContext> currentContext()
{
    auto context = Graphics::context();
    GE_CORE_ASSERT(context->API() == Graphics::API::VULKAN, "Incorrect Graphics API: {}",
                   static_cast<int>(context->API()));
    return staticPtrCast<Vulkan::GraphicsContext>(context);
}

} // namespace GE::Vulkan
