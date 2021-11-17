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

#include "swap_chain.h"
#include "device.h"
#include "image.h"
#include "vulkan_exception.h"

#include "genesis/core/log.h"

namespace {

constexpr uint64_t VULKAN_TIMEOUT_NONE = std::numeric_limits<uint64_t>::max();
constexpr size_t MAX_FRAMES_IN_FLIGHT{3};

uint32_t imageCountFromCaps(const VkSurfaceCapabilitiesKHR& capabilities)
{
    uint32_t image_count = capabilities.minImageCount + 1;

    if (capabilities.maxImageCount > 0 && image_count > capabilities.maxImageCount) {
        image_count = capabilities.maxImageCount;
    }

    return image_count;
}

VkSurfaceFormatKHR chooseSurfaceFormat(const std::vector<VkSurfaceFormatKHR>& formats)
{
    for (const auto& format : formats) {
        if (format.format == VK_FORMAT_B8G8R8A8_SRGB &&
            format.colorSpace == VK_COLORSPACE_SRGB_NONLINEAR_KHR) {
            return format;
        }
    }

    return formats[0];
}

VkPresentModeKHR choosePresentMode(const std::vector<VkPresentModeKHR>& present_modes)
{
    if (std::find(present_modes.begin(), present_modes.end(),
                  VK_PRESENT_MODE_MAILBOX_KHR) != present_modes.end()) {
        return VK_PRESENT_MODE_MAILBOX_KHR;
    }

    return VK_PRESENT_MODE_FIFO_KHR;
}

} // namespace

namespace GE::Vulkan {

SwapChain::SwapChain(Shared<Vulkan::Device> device, VkSurfaceKHR surface)
    : m_device{std::move(device)}
{
    try {
        createSwapChain(surface);
        createImageViews();
        createRenderPass();
        // TODO: add multisampling support
        // createColorResources();
        createDepthResources();
        createFramebuffers();
        createSyncObjects();
    } catch (const Vulkan::Exception& e) {
        destroyVkHandles();
        throw;
    }
}

SwapChain::~SwapChain()
{
    destroyVkHandles();
}

std::pair<VkResult, uint32_t> SwapChain::acquireNextImage()
{
    vkWaitForFences(m_device->device(), 1, &m_in_flight_fences[m_current_frame], VK_TRUE,
                    VULKAN_TIMEOUT_NONE);

    uint32_t image_index{};
    VkResult result = vkAcquireNextImageKHR(
        m_device->device(), m_swap_chain, VULKAN_TIMEOUT_NONE,
        m_image_available_semaphores[m_current_frame], VK_NULL_HANDLE, &image_index);

    return {result, image_index};
}

VkResult SwapChain::submitCommandBuffer(VkCommandBuffer* command_buffer,
                                        uint32_t image_idx)
{
    if (m_images_in_flight[image_idx] != VK_NULL_HANDLE) {
        vkWaitForFences(m_device->device(), 1, &m_images_in_flight[image_idx], VK_TRUE,
                        VULKAN_TIMEOUT_NONE);
    }

    m_images_in_flight[image_idx] = m_in_flight_fences[m_current_frame];

    VkPipelineStageFlags wait_stages = VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT;

    VkSubmitInfo submit_info{};
    submit_info.sType = VK_STRUCTURE_TYPE_SUBMIT_INFO;
    submit_info.waitSemaphoreCount = 1;
    submit_info.pWaitSemaphores = &m_image_available_semaphores[m_current_frame];
    submit_info.pWaitDstStageMask = &wait_stages;
    submit_info.commandBufferCount = 1;
    submit_info.pCommandBuffers = command_buffer;
    submit_info.signalSemaphoreCount = 1;
    submit_info.pSignalSemaphores = &m_render_finished_semaphores[m_current_frame];

    vkResetFences(m_device->device(), 1, &m_in_flight_fences[m_current_frame]);

    if (auto submit_result = vkQueueSubmit(m_device->graphicsQueue(), 1, &submit_info,
                                           m_in_flight_fences[m_current_frame]);
        submit_result != VK_SUCCESS) {
        GE_CORE_ERR("Failed to submit Draw Command Buffer");
        return submit_result;
    }

    VkPresentInfoKHR present_info{};
    present_info.sType = VK_STRUCTURE_TYPE_PRESENT_INFO_KHR;
    present_info.waitSemaphoreCount = 1;
    present_info.pWaitSemaphores = &m_render_finished_semaphores[m_current_frame];
    present_info.swapchainCount = 1;
    present_info.pSwapchains = &m_swap_chain;
    present_info.pImageIndices = &image_idx;
    present_info.pResults = nullptr;

    auto present_result = vkQueuePresentKHR(m_device->presentQueue(), &present_info);
    m_current_frame = (m_current_frame + 1) % MAX_FRAMES_IN_FLIGHT;

    return present_result;
}

void SwapChain::createSwapChain(VkSurfaceKHR surface)
{
    VkDevice device = m_device->device();

    const auto& swap_chain_details = m_device->swapChainDetails();
    auto surface_format = chooseSurfaceFormat(swap_chain_details.formats);
    auto present_mode = choosePresentMode(swap_chain_details.present_mode);
    m_extent = chooseSwapExtent(swap_chain_details.capabilities);
    m_min_image_count = imageCountFromCaps(swap_chain_details.capabilities);

    VkSwapchainCreateInfoKHR create_info{};
    create_info.sType = VK_STRUCTURE_TYPE_SWAPCHAIN_CREATE_INFO_KHR;
    create_info.surface = surface;
    create_info.minImageCount = m_min_image_count;
    create_info.imageFormat = surface_format.format;
    create_info.imageColorSpace = surface_format.colorSpace;
    create_info.imageExtent = m_extent;
    create_info.imageArrayLayers = 1;
    create_info.imageUsage = VK_IMAGE_USAGE_COLOR_ATTACHMENT_BIT;
    create_info.preTransform = swap_chain_details.capabilities.currentTransform;
    create_info.compositeAlpha = VK_COMPOSITE_ALPHA_OPAQUE_BIT_KHR;
    create_info.presentMode = present_mode;
    create_info.clipped = VK_TRUE;
    create_info.oldSwapchain = VK_NULL_HANDLE;

    const auto& indices = m_device->queueIndices();
    std::array<uint32_t, 2> queue_family_indices = {indices.graphics_family.value(),
                                                    indices.present_family.value()};

    if (indices.graphics_family != indices.present_family) {
        create_info.imageSharingMode = VK_SHARING_MODE_CONCURRENT;
        create_info.queueFamilyIndexCount = queue_family_indices.size();
        create_info.pQueueFamilyIndices = queue_family_indices.data();
    } else {
        create_info.imageSharingMode = VK_SHARING_MODE_EXCLUSIVE;
    }

    if (vkCreateSwapchainKHR(device, &create_info, nullptr, &m_swap_chain) !=
        VK_SUCCESS) {
        throw Vulkan::Exception{"Failed to create Swap Chain"};
    }

    uint32_t image_count{0};
    vkGetSwapchainImagesKHR(device, m_swap_chain, &image_count, nullptr);
    m_swap_chain_images.resize(image_count);
    vkGetSwapchainImagesKHR(device, m_swap_chain, &image_count,
                            m_swap_chain_images.data());

    m_image_format = surface_format.format;
}

void SwapChain::createImageViews()
{
    m_swap_chain_image_views.resize(m_swap_chain_images.size());

    for (size_t i{0}; i < m_swap_chain_image_views.size(); i++) {
        m_swap_chain_image_views[i] = createImageView(
            m_swap_chain_images[i], m_image_format, VK_IMAGE_ASPECT_COLOR_BIT, 1);
    }
}

void SwapChain::createRenderPass()
{
    VkAttachmentDescription color_attachment{};
    color_attachment.format = m_image_format;
    color_attachment.samples = VK_SAMPLE_COUNT_1_BIT;
    color_attachment.loadOp = VK_ATTACHMENT_LOAD_OP_CLEAR;
    color_attachment.storeOp = VK_ATTACHMENT_STORE_OP_STORE;
    color_attachment.stencilLoadOp = VK_ATTACHMENT_LOAD_OP_DONT_CARE;
    color_attachment.stencilStoreOp = VK_ATTACHMENT_STORE_OP_DONT_CARE;
    color_attachment.initialLayout = VK_IMAGE_LAYOUT_UNDEFINED;
    color_attachment.finalLayout = VK_IMAGE_LAYOUT_PRESENT_SRC_KHR;

    VkAttachmentReference color_attachment_ref{};
    color_attachment_ref.attachment = 0;
    color_attachment_ref.layout = VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL;

    VkAttachmentDescription depth_attachment{};
    depth_attachment.format = getDepthFormat();
    depth_attachment.samples = VK_SAMPLE_COUNT_1_BIT;
    depth_attachment.loadOp = VK_ATTACHMENT_LOAD_OP_CLEAR;
    depth_attachment.storeOp = VK_ATTACHMENT_STORE_OP_DONT_CARE;
    depth_attachment.stencilLoadOp = VK_ATTACHMENT_LOAD_OP_DONT_CARE;
    depth_attachment.stencilStoreOp = VK_ATTACHMENT_STORE_OP_DONT_CARE;
    depth_attachment.initialLayout = VK_IMAGE_LAYOUT_UNDEFINED;
    depth_attachment.finalLayout = VK_IMAGE_LAYOUT_DEPTH_STENCIL_ATTACHMENT_OPTIMAL;

    VkAttachmentReference depth_attachment_ref{};
    depth_attachment_ref.attachment = 1;
    depth_attachment_ref.layout = VK_IMAGE_LAYOUT_DEPTH_STENCIL_ATTACHMENT_OPTIMAL;

    VkSubpassDescription subpass{};
    subpass.pipelineBindPoint = VK_PIPELINE_BIND_POINT_GRAPHICS;
    subpass.colorAttachmentCount = 1;
    subpass.pColorAttachments = &color_attachment_ref;
    subpass.pDepthStencilAttachment = &depth_attachment_ref;

    VkSubpassDependency dependency{};
    dependency.srcSubpass = VK_SUBPASS_EXTERNAL;
    dependency.srcStageMask = VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT;
    dependency.srcAccessMask = 0;
    dependency.dstSubpass = 0;
    dependency.dstStageMask = VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT;
    dependency.dstAccessMask = VK_ACCESS_COLOR_ATTACHMENT_WRITE_BIT;

    std::array<VkAttachmentDescription, 2> attachments = {
        color_attachment,
        depth_attachment,
    };

    VkRenderPassCreateInfo render_pass_info{};
    render_pass_info.sType = VK_STRUCTURE_TYPE_RENDER_PASS_CREATE_INFO;
    render_pass_info.attachmentCount = attachments.size();
    render_pass_info.pAttachments = attachments.data();
    render_pass_info.subpassCount = 1;
    render_pass_info.pSubpasses = &subpass;
    render_pass_info.dependencyCount = 1;
    render_pass_info.pDependencies = &dependency;

    if (vkCreateRenderPass(m_device->device(), &render_pass_info, nullptr,
                           &m_render_pass) != VK_SUCCESS) {
        throw Vulkan::Exception{"Failed to create Render Pass"};
    }
}

void SwapChain::createColorResources()
{
    image_config_t config{};
    config.extent.width = m_extent.width;
    config.extent.height = m_extent.height;
    config.mip_levels = 1;
    config.format = m_image_format;
    config.tiling = VK_IMAGE_TILING_OPTIMAL;
    config.usage =
        VK_IMAGE_USAGE_TRANSIENT_ATTACHMENT_BIT | VK_IMAGE_USAGE_COLOR_ATTACHMENT_BIT;
    config.memory_properties = VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT;
    config.aspect_mask = VK_IMAGE_ASPECT_COLOR_BIT;

    m_color_image = makeScoped<Image>(m_device, config);
}

void SwapChain::createDepthResources()
{
    image_config_t config{};
    config.extent.width = m_extent.width;
    config.extent.height = m_extent.height;
    config.mip_levels = 1;
    config.format = getDepthFormat();
    config.tiling = VK_IMAGE_TILING_OPTIMAL;
    config.usage = VK_IMAGE_USAGE_DEPTH_STENCIL_ATTACHMENT_BIT;
    config.memory_properties = VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT;
    config.aspect_mask = VK_IMAGE_ASPECT_DEPTH_BIT;

    m_depth_image = makeScoped<Image>(m_device, config);
}

void SwapChain::createFramebuffers()
{
    m_framebuffers.resize(m_swap_chain_image_views.size());

    for (size_t i = 0; i < m_swap_chain_image_views.size(); i++) {
        std::array<VkImageView, 2> attachments = {
            m_swap_chain_image_views[i],
            m_depth_image->view(),
        };

        VkFramebufferCreateInfo framebuffer_info{};
        framebuffer_info.sType = VK_STRUCTURE_TYPE_FRAMEBUFFER_CREATE_INFO;
        framebuffer_info.renderPass = m_render_pass;
        framebuffer_info.attachmentCount = attachments.size();
        framebuffer_info.pAttachments = attachments.data();
        framebuffer_info.width = m_extent.width;
        framebuffer_info.height = m_extent.height;
        framebuffer_info.layers = 1;

        if (vkCreateFramebuffer(m_device->device(), &framebuffer_info, nullptr,
                                &m_framebuffers[i]) != VK_SUCCESS) {
            throw Vulkan::Exception{"Failed to create Framebuffer"};
        }
    }
}

void SwapChain::createSyncObjects()
{
    m_image_available_semaphores.resize(MAX_FRAMES_IN_FLIGHT);
    m_render_finished_semaphores.resize(MAX_FRAMES_IN_FLIGHT);
    m_in_flight_fences.resize(MAX_FRAMES_IN_FLIGHT);
    m_images_in_flight.resize(m_swap_chain_images.size(), VK_NULL_HANDLE);

    VkSemaphoreCreateInfo semaphore_info{};
    semaphore_info.sType = VK_STRUCTURE_TYPE_SEMAPHORE_CREATE_INFO;

    VkFenceCreateInfo fence_info{};
    fence_info.sType = VK_STRUCTURE_TYPE_FENCE_CREATE_INFO;
    fence_info.flags = VK_FENCE_CREATE_SIGNALED_BIT;

    for (size_t i{0}; i < MAX_FRAMES_IN_FLIGHT; i++) {
        if (vkCreateSemaphore(m_device->device(), &semaphore_info, nullptr,
                              &m_image_available_semaphores[i]) != VK_SUCCESS) {
            throw Vulkan::Exception{"Failed to create an image available semaphore"};
        }

        if (vkCreateSemaphore(m_device->device(), &semaphore_info, nullptr,
                              &m_render_finished_semaphores[i]) != VK_SUCCESS) {
            throw Vulkan::Exception{"Failed to create a render finished semaphore"};
        }

        if (vkCreateFence(m_device->device(), &fence_info, nullptr,
                          &m_in_flight_fences[i]) != VK_SUCCESS) {
            throw Vulkan::Exception{"Failed to create an in flight fence"};
        }
    }
}

void SwapChain::destroyVkHandles()
{
    for (size_t i{0}; i < m_image_available_semaphores.size(); i++) {
        vkDestroyFence(m_device->device(), m_in_flight_fences[i], nullptr);
        m_in_flight_fences[i] = VK_NULL_HANDLE;

        vkDestroySemaphore(m_device->device(), m_render_finished_semaphores[i], nullptr);
        m_render_finished_semaphores[i] = VK_NULL_HANDLE;

        vkDestroySemaphore(m_device->device(), m_image_available_semaphores[i], nullptr);
        m_image_available_semaphores[i] = VK_NULL_HANDLE;
    }

    for (size_t i{0}; i < m_swap_chain_images.size(); i++) {
        vkDestroyImageView(m_device->device(), m_swap_chain_image_views[i], nullptr);
        m_swap_chain_image_views[i] = VK_NULL_HANDLE;

        vkDestroyFramebuffer(m_device->device(), m_framebuffers[i], nullptr);
        m_framebuffers[i] = VK_NULL_HANDLE;
    }

    m_depth_image.reset();
    m_color_image.reset();

    vkDestroyRenderPass(m_device->device(), m_render_pass, nullptr);
    m_render_pass = VK_NULL_HANDLE;

    vkDestroySwapchainKHR(m_device->device(), m_swap_chain, nullptr);
    m_swap_chain = VK_NULL_HANDLE;
}

VkImageView SwapChain::createImageView(VkImage image, VkFormat format,
                                       VkImageAspectFlags aspect_flags,
                                       uint32_t mip_levels)
{
    VkImageView image_view{VK_NULL_HANDLE};

    VkImageViewCreateInfo view_info{};
    view_info.sType = VK_STRUCTURE_TYPE_IMAGE_VIEW_CREATE_INFO;
    view_info.image = image;
    view_info.viewType = VK_IMAGE_VIEW_TYPE_2D;
    view_info.format = format;
    view_info.subresourceRange.aspectMask = aspect_flags;
    view_info.subresourceRange.baseMipLevel = 0;
    view_info.subresourceRange.levelCount = mip_levels;
    view_info.subresourceRange.baseArrayLayer = 0;
    view_info.subresourceRange.layerCount = 1;

    if (vkCreateImageView(m_device->device(), &view_info, nullptr, &image_view) !=
        VK_SUCCESS) {
        throw Vulkan::Exception{"Failed to create Texture Image View"};
    }

    return image_view;
}

VkExtent2D SwapChain::chooseSwapExtent(const VkSurfaceCapabilitiesKHR& capabilities) const
{
    if (capabilities.currentExtent.width != std::numeric_limits<uint32_t>::max()) {
        return capabilities.currentExtent;
    }

    VkExtent2D extent{static_cast<uint32_t>(m_window_size.x),
                      static_cast<uint32_t>(m_window_size.y)};

    extent.width = std::max(capabilities.minImageExtent.width,
                            std::min(capabilities.maxImageExtent.width, extent.width));
    extent.height = std::max(capabilities.minImageExtent.height,
                             std::min(capabilities.maxImageExtent.height, extent.height));

    return extent;
}

VkFormat SwapChain::getDepthFormat()
{
    static const std::vector<VkFormat> candidates = {
        VK_FORMAT_D32_SFLOAT,
        VK_FORMAT_D32_SFLOAT_S8_UINT,
        VK_FORMAT_D24_UNORM_S8_UINT,
    };

    return m_device->getSupportedFormat(candidates, VK_IMAGE_TILING_OPTIMAL,
                                        VK_FORMAT_FEATURE_DEPTH_STENCIL_ATTACHMENT_BIT);
}

} // namespace GE::Vulkan
