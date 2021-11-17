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

// NOLINTNEXTLINE(llvm-header-guard)
#ifndef GENESIS_GRAPHICS_VULKAN_SWAP_CHAIN_H_
#define GENESIS_GRAPHICS_VULKAN_SWAP_CHAIN_H_

#include <genesis/core/memory.h>
#include <genesis/math/types.h>

#include <vulkan/vulkan.h>

#include <vector>

namespace GE::Vulkan {

class Device;
class Image;

class SwapChain
{
public:
    SwapChain(Shared<Vulkan::Device> device, VkSurfaceKHR surface);
    ~SwapChain();

    std::pair<VkResult, uint32_t> acquireNextImage();
    VkResult submitCommandBuffer(VkCommandBuffer* command_buffer, uint32_t image_idx);

    VkRenderPass getRenderPass() const { return m_render_pass; }
    const VkExtent2D& getExtent() const { return m_extent; }
    uint32_t getImageCount() const { return m_swap_chain_images.size(); }
    uint32_t minImageCount() const { return m_min_image_count; }
    VkFramebuffer getFramebuffer(size_t i) const { return m_framebuffers[i]; }

private:
    void createSwapChain(VkSurfaceKHR surface);
    void createImageViews();
    void createRenderPass();
    void createColorResources();
    void createDepthResources();
    void createFramebuffers();
    void createSyncObjects();

    void destroyVkHandles();

    VkImageView createImageView(VkImage image, VkFormat format,
                                VkImageAspectFlags aspect_flags, uint32_t mip_levels);
    VkExtent2D chooseSwapExtent(const VkSurfaceCapabilitiesKHR& capabilities) const;
    VkFormat getDepthFormat();

    Shared<Vulkan::Device> m_device;

    VkSwapchainKHR m_swap_chain{VK_NULL_HANDLE};
    std::vector<VkImage> m_swap_chain_images;
    std::vector<VkImageView> m_swap_chain_image_views;
    std::vector<VkFramebuffer> m_framebuffers;
    uint32_t m_min_image_count{0};
    uint32_t m_current_frame{0};

    Scoped<Image> m_color_image;
    Scoped<Image> m_depth_image;

    std::vector<VkSemaphore> m_image_available_semaphores;
    std::vector<VkSemaphore> m_render_finished_semaphores;
    std::vector<VkFence> m_in_flight_fences;
    std::vector<VkFence> m_images_in_flight;

    VkRenderPass m_render_pass{VK_NULL_HANDLE};

    VkFormat m_image_format{};
    VkExtent2D m_extent{};
    Vec2 m_window_size{};
};

} // namespace GE::Vulkan

#endif // GENESIS_GRAPHICS_VULKAN_SWAP_CHAIN_H_
