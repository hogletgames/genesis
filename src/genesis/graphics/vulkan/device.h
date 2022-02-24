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
#ifndef GENESIS_GRAPHICS_VULKAN_DEVICE_H_
#define GENESIS_GRAPHICS_VULKAN_DEVICE_H_

#include <genesis/core/export.h>
#include <genesis/core/memory.h>

#include <vulkan/vulkan.h>

#include <optional>
#include <vector>

namespace GE::Vulkan {

struct queue_family_indices_t {
    std::optional<uint32_t> graphics_family{};
    std::optional<uint32_t> present_family{};

    bool isComplete() const
    {
        return graphics_family.has_value() && present_family.has_value();
    }
};

struct swap_chain_support_details_t {
    VkSurfaceCapabilitiesKHR capabilities{};
    std::vector<VkSurfaceFormatKHR> formats;
    std::vector<VkPresentModeKHR> present_mode;

    bool isValid() const { return !formats.empty() && !present_mode.empty(); }
};

class GE_API Device
{
public:
    explicit Device(VkSurfaceKHR surface);
    ~Device();

    void waitIdle();

    VkPhysicalDevice physicalDevice() const { return m_physical_device; }
    VkDevice device() const { return m_device; }
    VkCommandPool commandPool() const { return m_command_pool; }

    VkQueue graphicsQueue() const { return m_graphics_queue; }
    VkQueue presentQueue() const { return m_present_queue; }
    const queue_family_indices_t& queueIndices() const { return m_queue_indices; }

    swap_chain_support_details_t swapChainDetails() const
    {
        return querySwapChainSupport(m_physical_device);
    }

    VkFormat getSupportedFormat(const std::vector<VkFormat>& candidates,
                                VkImageTiling tiling, VkFormatFeatureFlags features);
    uint32_t findMemoryType(uint32_t type_filter, VkMemoryPropertyFlags properties);

private:
    void pickPhysicalDevice();
    void createLogicalDevice();
    void createCommandPool();

    void destroyVkHandles();

    bool isPhysicalDeviceSuitable(VkPhysicalDevice physical_device);
    queue_family_indices_t findQueueFamilies(VkPhysicalDevice physical_device);
    bool checkPhysicalDeviceExtSupport(VkPhysicalDevice physical_device);
    swap_chain_support_details_t
    querySwapChainSupport(VkPhysicalDevice physical_device) const;

    std::vector<VkExtensionProperties>
    getPhysicalDeviceExt(VkPhysicalDevice physical_device);

    VkSurfaceKHR m_surface{VK_NULL_HANDLE};
    VkPhysicalDevice m_physical_device{VK_NULL_HANDLE};
    VkDevice m_device{VK_NULL_HANDLE};
    VkCommandPool m_command_pool{VK_NULL_HANDLE};

    VkQueue m_graphics_queue{VK_NULL_HANDLE};
    VkQueue m_present_queue{VK_NULL_HANDLE};

    queue_family_indices_t m_queue_indices{};

    std::vector<const char*> m_extensions;
};

} // namespace GE::Vulkan

#endif // GENESIS_GRAPHICS_VULKAN_DEVICE_H_
