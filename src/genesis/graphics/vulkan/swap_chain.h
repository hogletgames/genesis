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

#pragma once

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
    struct options_t {
        VkSurfaceKHR surface{VK_NULL_HANDLE};
        Vec2         window_size{0.0f, 0.0f};
        uint8_t      msaa_samples{1};
    };

    SwapChain(Shared<Vulkan::Device> device, const options_t& options);
    ~SwapChain();

    bool recreate(const Vec2& window_size);

    VkResult acquireNextImage();
    VkResult submitCommandBuffer(VkCommandBuffer* command_buffer);
    VkResult presentImage();

    const VkExtent2D& extent() const { return m_extent; }
    VkSwapchainKHR swapChain() const { return m_swap_chain; }
    VkImage currentImage() const { return m_swap_chain_images[m_current_image]; }
    VkImageView currentImageView() const { return m_swap_chain_image_views[m_current_image]; }
    const Scoped<Image>& colorMSAAImage() const { return m_color_msaa_image; }
    const Scoped<Image>& depthImage() const { return m_depth_image; }

    const VkFormat& colorFormat() const { return m_image_format; }
    const VkFormat& depthFormat() const { return m_depth_format; }

    uint32_t imageCount() const { return m_swap_chain_images.size(); }
    uint32_t minImageCount() const { return m_min_image_count; }
    uint32_t currentImageIndex() const { return m_current_image; }

    static VkSurfaceFormatKHR chooseSurfaceFormat(const std::vector<VkSurfaceFormatKHR>& formats);

    static VkFormat choseDepthFormat(Device* device);

private:
    void createSwapChainWithResources(VkSwapchainKHR old_swap_chain, const Vec2& window_size);
    void createSwapChain(VkSwapchainKHR old_swap_chain, const Vec2& window_size);
    void createImageViews();
    void createColorMSAAImage();
    void createDepthImage();
    void createSyncObjects();

    void destroySwapChainResources();
    void destroySyncObjectHandles();
    void destroySwapChain(VkSwapchainKHR swap_chain);
    void destroyVkHandles();

    VkImageView createImageView(VkImage            image,
                                VkFormat           format,
                                VkImageAspectFlags aspect_flags,
                                uint32_t           mip_levels);
    VkExtent2D chooseSwapExtent(const VkSurfaceCapabilitiesKHR& capabilities,
                                const Vec2&                     window_size) const;

    Shared<Vulkan::Device> m_device;
    VkSurfaceKHR           m_surface{VK_NULL_HANDLE};
    uint8_t                m_msaa_samples{1};

    VkSwapchainKHR           m_swap_chain{VK_NULL_HANDLE};
    std::vector<VkImage>     m_swap_chain_images;
    std::vector<VkImageView> m_swap_chain_image_views;

    uint32_t m_min_image_count{0};
    uint32_t m_current_frame{0};
    uint32_t m_current_image{0};

    Scoped<Image> m_color_msaa_image;
    Scoped<Image> m_depth_image;

    std::vector<VkSemaphore> m_image_available_semaphores;
    std::vector<VkSemaphore> m_render_finished_semaphores;
    std::vector<VkFence>     m_in_flight_fences;
    std::vector<VkFence>     m_images_in_flight;

    VkFormat   m_image_format{VK_FORMAT_UNDEFINED};
    VkFormat   m_depth_format{VK_FORMAT_UNDEFINED};
    VkExtent2D m_extent{};
};

} // namespace GE::Vulkan
