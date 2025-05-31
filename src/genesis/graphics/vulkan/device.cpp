/*
 * BSD 3-Clause License
 *
 * Copyright (c) 2021-2022, Dmitry Shilnenkov
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

#include "device.h"
#include "instance.h"
#include "utils.h"
#include "vulkan_exception.h"

#include "genesis/core/format.h"
#include "genesis/core/log.h"

#include <unordered_set>

namespace GE::Vulkan {
namespace {

std::string toString(VkPhysicalDevice physical_device)
{
    VkPhysicalDeviceProperties device_properties{};
    vkGetPhysicalDeviceProperties(physical_device, &device_properties);
    return device_properties.deviceName;
}

std::string toString(const std::vector<VkPhysicalDevice>& physical_devices)
{
    std::string string;

    for (size_t i{0}; i < physical_devices.size(); i++) {
        const auto& device = physical_devices[i];
        string += GE_FMTSTR("- Name: {}", toString(device));
        string += i != physical_devices.size() - 1 ? "\n" : "";
    }

    return string;
}

bool isGraphicQueue(VkQueueFamilyProperties queue_family)
{
    return (queue_family.queueFlags & VK_QUEUE_GRAPHICS_BIT) != 0;
}

bool isTransferQueue(VkQueueFamilyProperties queue_family)
{
    return (queue_family.queueFlags & VK_QUEUE_TRANSFER_BIT) != 0;
}

bool isComputeQueue(VkQueueFamilyProperties queue_family)
{
    return (queue_family.queueFlags & VK_QUEUE_COMPUTE_BIT) != 0;
}

bool isPresentSupported(VkPhysicalDevice physical_device,
                        VkSurfaceKHR     surface,
                        uint32_t         queue_family_idx)
{
    VkBool32 is_present_supported{VK_FALSE};
    vkGetPhysicalDeviceSurfaceSupportKHR(physical_device, queue_family_idx, surface,
                                         &is_present_supported);

    return is_present_supported == VK_TRUE;
}

uint8_t getMaxMSAA(VkSampleCountFlags device_count)
{
    static constexpr std::array<VkSampleCountFlags, 6> expected_counts = {
        VK_SAMPLE_COUNT_64_BIT, VK_SAMPLE_COUNT_32_BIT, VK_SAMPLE_COUNT_16_BIT,
        VK_SAMPLE_COUNT_8_BIT,  VK_SAMPLE_COUNT_4_BIT,  VK_SAMPLE_COUNT_2_BIT,
    };

    for (auto expected_count : expected_counts) {
        if ((device_count & expected_count) != 0) {
            return static_cast<uint8_t>(expected_count);
        }
    }

    return static_cast<uint8_t>(VK_SAMPLE_COUNT_1_BIT);
}

void appendMandatoryDeviceExtensions(VkPhysicalDevice          physical_device,
                                     std::vector<const char*>* ext)
{
    static const std::unordered_set<std::string_view> mandatory_ext = {
        "VK_KHR_portability_subset",
    };

    auto device_extensions = vulkanGet<VkExtensionProperties>(
        ::vkEnumerateDeviceExtensionProperties, physical_device, nullptr);

    for (const auto& device_ext : device_extensions) {
        if (auto it = mandatory_ext.find(device_ext.extensionName); it != mandatory_ext.end()) {
            ext->push_back(it->data());
        }
    }
}

} // namespace

Device::Device(VkSurfaceKHR surface)
    : m_surface{surface}
{
    m_extensions = {
        VK_KHR_SWAPCHAIN_EXTENSION_NAME,
        VK_KHR_MAINTENANCE1_EXTENSION_NAME,
        VK_KHR_DYNAMIC_RENDERING_EXTENSION_NAME,
    };

    pickPhysicalDevice();
    createLogicalDevice();
    createCommandPool();
    fillLimits();
}

Device::~Device()
{
    destroyVkHandles();
}

void Device::waitIdle()
{
    vkDeviceWaitIdle(m_device);
}

VkFormat Device::getSupportedFormat(const std::vector<VkFormat>& candidates,
                                    VkImageTiling                tiling,
                                    VkFormatFeatureFlags         features)
{
    for (auto format : candidates) {
        VkFormatProperties props{};
        vkGetPhysicalDeviceFormatProperties(m_physical_device, format, &props);

        bool has_linear_features = (props.linearTilingFeatures & features) == features;
        bool has_optimal_features = (props.optimalTilingFeatures & features) == features;

        if (tiling == VK_IMAGE_TILING_LINEAR && has_linear_features) {
            return format;
        }

        if (tiling == VK_IMAGE_TILING_OPTIMAL && has_optimal_features) {
            return format;
        }
    }

    auto error = GE_FMTSTR("Failed to find supported format, features: 0x{:08X}",
                           static_cast<int>(features));
    throw Vulkan::Exception{error};
}

void Device::pickPhysicalDevice()
{
    VkInstance instance = Instance::instance();
    auto       devices = vulkanGet<VkPhysicalDevice>(::vkEnumeratePhysicalDevices, instance);

    GE_CORE_INFO("Physical Device List: \n{}", toString(devices));

    for (const auto& device : devices) {
        if (isPhysicalDeviceSuitable(device)) {
            GE_CORE_INFO("Picked Physical Device: {}", toString(device));
            m_physical_device = device;
            appendMandatoryDeviceExtensions(m_physical_device, &m_extensions);
            return;
        }
    }

    throw Vulkan::Exception{"Failed to pick Physical Device"};
}

void Device::createLogicalDevice()
{
    std::vector<VkDeviceQueueCreateInfo> queue_create_infos;
    std::unordered_set<uint32_t>         unique_queue_families = {
        m_queue_indices.graphics_family.value(),
        m_queue_indices.present_family.value(),
        m_queue_indices.transfer_family.value(),
        m_queue_indices.compute_queue.value(),
    };

    float queue_priorities{1.0f};

    for (uint32_t queue_family : unique_queue_families) {
        VkDeviceQueueCreateInfo queue_create_info{};
        queue_create_info.sType = VK_STRUCTURE_TYPE_DEVICE_QUEUE_CREATE_INFO;
        queue_create_info.queueFamilyIndex = queue_family;
        queue_create_info.queueCount = 1;
        queue_create_info.pQueuePriorities = &queue_priorities;

        queue_create_infos.push_back(queue_create_info);
    }

    VkPhysicalDeviceFeatures device_features{};
    device_features.independentBlend = VK_TRUE;
    device_features.sampleRateShading = VK_TRUE;
    device_features.samplerAnisotropy = VK_TRUE;

    VkPhysicalDeviceDynamicRenderingFeatures dynamic_rendering_features{};
    dynamic_rendering_features.sType = VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_DYNAMIC_RENDERING_FEATURES;
    dynamic_rendering_features.dynamicRendering = VK_TRUE;

    VkDeviceCreateInfo create_info{};
    create_info.sType = VK_STRUCTURE_TYPE_DEVICE_CREATE_INFO;
    create_info.pNext = &dynamic_rendering_features;
    create_info.pQueueCreateInfos = queue_create_infos.data();
    create_info.queueCreateInfoCount = queue_create_infos.size();
    create_info.pEnabledFeatures = &device_features;
    create_info.ppEnabledExtensionNames = m_extensions.data();
    create_info.enabledExtensionCount = m_extensions.size();

    if (vkCreateDevice(m_physical_device, &create_info, nullptr, &m_device) != VK_SUCCESS) {
        throw Vulkan::Exception{"Failed to create Logical Device"};
    }

    vkGetDeviceQueue(m_device, m_queue_indices.graphics_family.value(), 0, &m_graphics_queue);
    vkGetDeviceQueue(m_device, m_queue_indices.present_family.value(), 0, &m_present_queue);
    vkGetDeviceQueue(m_device, m_queue_indices.transfer_family.value(), 0, &m_transfer_queue);
    vkGetDeviceQueue(m_device, m_queue_indices.compute_queue.value(), 0, &m_compute_queue);
}

void Device::createCommandPool()
{
    auto queue_family_indices = findQueueFamilies(m_physical_device);

    VkCommandPoolCreateInfo create_info{};
    create_info.sType = VK_STRUCTURE_TYPE_COMMAND_POOL_CREATE_INFO;
    create_info.queueFamilyIndex = queue_family_indices.graphics_family.value();
    create_info.flags = VK_COMMAND_POOL_CREATE_RESET_COMMAND_BUFFER_BIT;

    if (vkCreateCommandPool(m_device, &create_info, nullptr, &m_command_pool) != VK_SUCCESS) {
        throw Vulkan::Exception{"Failed to create Command Pool"};
    }
}

void Device::fillLimits()
{
    VkPhysicalDeviceProperties properties{};
    vkGetPhysicalDeviceProperties(m_physical_device, &properties);

    m_limits.max_msaa = getMaxMSAA(properties.limits.framebufferColorSampleCounts &
                                   properties.limits.framebufferDepthSampleCounts);
}

void Device::destroyVkHandles()
{
    if (m_device == VK_NULL_HANDLE) {
        return;
    }

    vkDestroyCommandPool(m_device, m_command_pool, nullptr);
    m_command_pool = VK_NULL_HANDLE;

    vkDestroyDevice(m_device, nullptr);
    m_device = VK_NULL_HANDLE;
}

bool Device::isPhysicalDeviceSuitable(VkPhysicalDevice physical_device)
{
    if (m_queue_indices = findQueueFamilies(physical_device); !m_queue_indices.isComplete()) {
        return false;
    }

    if (!checkPhysicalDeviceExtSupport(physical_device)) {
        return false;
    }

    if (!querySwapChainSupport(physical_device).isValid()) {
        return false;
    }

    VkPhysicalDeviceFeatures features{};
    vkGetPhysicalDeviceFeatures(physical_device, &features);

    return features.independentBlend == VK_TRUE && features.samplerAnisotropy == VK_TRUE &&
           features.sampleRateShading == VK_TRUE;
}

queue_family_indices_t Device::findQueueFamilies(VkPhysicalDevice physical_device)
{
    queue_family_indices_t indices{};
    auto                   queue_families = vulkanGet<VkQueueFamilyProperties>(
        ::vkGetPhysicalDeviceQueueFamilyProperties, physical_device);

    for (uint32_t i{0}; i < queue_families.size(); i++) {
        if (isGraphicQueue(queue_families[i])) {
            indices.graphics_family = i;
        }

        if (isPresentSupported(physical_device, m_surface, i)) {
            indices.present_family = i;
        }

        if (isTransferQueue(queue_families[i])) {
            indices.transfer_family = i;
        }

        if (isComputeQueue(queue_families[i])) {
            indices.compute_queue = i;
        }

        if (indices.isComplete()) {
            break;
        }
    }

    return indices;
}

uint32_t Device::findMemoryType(uint32_t type_filter, VkMemoryPropertyFlags properties)
{
    VkPhysicalDeviceMemoryProperties mem_properties{};
    vkGetPhysicalDeviceMemoryProperties(m_physical_device, &mem_properties);

    for (uint32_t i{0}; i < mem_properties.memoryTypeCount; i++) {
        uint32_t mem_type = 1 << i;
        bool     is_type_suitable = (type_filter & mem_type) != 0;
        bool     is_properties_suitable =
            (mem_properties.memoryTypes[i].propertyFlags & properties) != 0;

        if (is_type_suitable && is_properties_suitable) {
            return i;
        }
    }

    throw Vulkan::Exception{"Failed to find suitable memory type"};
}

bool Device::checkPhysicalDeviceExtSupport(VkPhysicalDevice physical_device)
{
    auto device_extensions = vulkanGet<VkExtensionProperties>(
        ::vkEnumerateDeviceExtensionProperties, physical_device, nullptr);
    std::unordered_set<std::string> required_extensions = {m_extensions.begin(),
                                                           m_extensions.end()};

    for (const auto& extension : device_extensions) {
        required_extensions.erase(extension.extensionName);
    }

    return required_extensions.empty();
}

swap_chain_support_details_t Device::querySwapChainSupport(VkPhysicalDevice physical_device) const
{
    swap_chain_support_details_t details{};
    details.formats = vulkanGet<VkSurfaceFormatKHR>(::vkGetPhysicalDeviceSurfaceFormatsKHR,
                                                    physical_device, m_surface);
    details.present_mode = vulkanGet<VkPresentModeKHR>(::vkGetPhysicalDeviceSurfacePresentModesKHR,
                                                       physical_device, m_surface);
    vkGetPhysicalDeviceSurfaceCapabilitiesKHR(physical_device, m_surface, &details.capabilities);

    return details;
}

} // namespace GE::Vulkan
