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

#include "device.h"
#include "instance.h"
#include "render_context.h"
#include "vulkan_exception.h"

#include "genesis/core/format.h"
#include "genesis/core/log.h"

#include <unordered_set>

namespace {

std::string toString(VkPhysicalDevice physical_device)
{
    VkPhysicalDeviceProperties device_properties{};
    vkGetPhysicalDeviceProperties(physical_device, &device_properties);
    return device_properties.deviceName;
}

std::string toString(const std::vector<VkPhysicalDevice> &physical_devices)
{
    std::string string;

    for (size_t i{0}; i < physical_devices.size(); i++) {
        const auto &device = physical_devices[i];
        string += GE_FMTSTR("- Name: {}", ::toString(device));
        string += i != physical_devices.size() - 1 ? "\n" : "";
    }

    return string;
}

bool isGraphicQueue(VkQueueFamilyProperties queue_family)
{
    return (queue_family.queueFlags & VK_QUEUE_GRAPHICS_BIT) != 0;
}

bool isPresentSupported(VkPhysicalDevice physical_device, VkSurfaceKHR surface,
                        uint32_t queue_family_idx)
{
    VkBool32 is_present_supported{VK_FALSE};
    vkGetPhysicalDeviceSurfaceSupportKHR(physical_device, queue_family_idx, surface,
                                         &is_present_supported);

    return is_present_supported == VK_TRUE;
}

} // namespace

namespace GE::Vulkan {

Device::Device(Vulkan::RenderContext *context)
    : m_context{context}
{
    m_extensions = {VK_KHR_SWAPCHAIN_EXTENSION_NAME};

    try {
        pickPhysicalDevice();
        createLogicalDevice();
        createCommandPool();
    } catch (const Vulkan::Exception &e) {
        destroyVkHandles();
        throw;
    }
}

Device::~Device()
{
    destroyVkHandles();
}

void Device::waitIdle()
{
    vkDeviceWaitIdle(m_device);
}

VkFormat Device::getSupportedFormat(const std::vector<VkFormat> &candidates,
                                    VkImageTiling tiling, VkFormatFeatureFlags features)
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

    uint32_t device_count{0};
    vkEnumeratePhysicalDevices(instance, &device_count, nullptr);

    std::vector<VkPhysicalDevice> devices(device_count);
    vkEnumeratePhysicalDevices(instance, &device_count, devices.data());

    GE_CORE_INFO("Physical Device List: \n{}", ::toString(devices));

    for (const auto &device : devices) {
        if (isPhysicalDeviceSuitable(device)) {
            GE_CORE_INFO("Picked Physical Device: {}", ::toString(device));
            m_physical_device = device;
            return;
        }
    }

    throw Vulkan::Exception{"Failed to pick Physical Device"};
}

void Device::createLogicalDevice()
{
    std::vector<VkDeviceQueueCreateInfo> queue_create_infos;
    std::unordered_set<uint32_t> unique_queue_families = {
        m_queue_indices.graphics_family.value(), m_queue_indices.present_family.value()};

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
    device_features.samplerAnisotropy = VK_TRUE;
    device_features.sampleRateShading = VK_TRUE;

    VkDeviceCreateInfo create_info{};
    create_info.sType = VK_STRUCTURE_TYPE_DEVICE_CREATE_INFO;
    create_info.pQueueCreateInfos = queue_create_infos.data();
    create_info.queueCreateInfoCount = queue_create_infos.size();
    create_info.pEnabledFeatures = &device_features;
    create_info.ppEnabledExtensionNames = m_extensions.data();
    create_info.enabledExtensionCount = m_extensions.size();

    if (vkCreateDevice(m_physical_device, &create_info, nullptr, &m_device) !=
        VK_SUCCESS) {
        throw Vulkan::Exception{"Failed to create Logical Device"};
    }

    vkGetDeviceQueue(m_device, m_queue_indices.graphics_family.value(), 0,
                     &m_graphics_queue);
    vkGetDeviceQueue(m_device, m_queue_indices.present_family.value(), 0,
                     &m_present_queue);
}

void Device::createCommandPool()
{
    auto queue_family_indices = findQueueFamilies(m_physical_device);

    VkCommandPoolCreateInfo create_info{};
    create_info.sType = VK_STRUCTURE_TYPE_COMMAND_POOL_CREATE_INFO;
    create_info.queueFamilyIndex = queue_family_indices.graphics_family.value();
    create_info.flags = VK_COMMAND_POOL_CREATE_RESET_COMMAND_BUFFER_BIT;

    if (vkCreateCommandPool(m_device, &create_info, nullptr, &m_command_pool) !=
        VK_SUCCESS) {
        throw Vulkan::Exception{"Failed to create Command Pool"};
    }
}

void Device::destroyVkHandles()
{
    vkDestroyCommandPool(m_device, m_command_pool, nullptr);
    m_command_pool = VK_NULL_HANDLE;

    vkDestroyDevice(m_device, nullptr);
    m_device = VK_NULL_HANDLE;
}

bool Device::isPhysicalDeviceSuitable(VkPhysicalDevice physical_device)
{
    if (m_queue_indices = findQueueFamilies(physical_device);
        !m_queue_indices.isComplete()) {
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

    return features.samplerAnisotropy == VK_TRUE;
}

queue_family_indices_t Device::findQueueFamilies(VkPhysicalDevice physical_device)
{
    queue_family_indices_t indices{};

    uint32_t queue_family_count{0};
    vkGetPhysicalDeviceQueueFamilyProperties(physical_device, &queue_family_count,
                                             nullptr);

    std::vector<VkQueueFamilyProperties> queue_families(queue_family_count);
    vkGetPhysicalDeviceQueueFamilyProperties(physical_device, &queue_family_count,
                                             queue_families.data());

    for (uint32_t i{0}; i < queue_family_count; i++) {
        if (isGraphicQueue(queue_families[i])) {
            indices.graphics_family = i;
        }

        if (isPresentSupported(physical_device, m_context->surface(), i)) {
            indices.present_family = i;
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
        bool is_type_suitable = (type_filter & mem_type) != 0;
        bool is_properties_suitable =
            (mem_properties.memoryTypes[i].propertyFlags & properties) != 0;

        if (is_type_suitable && is_properties_suitable) {
            return i;
        }
    }

    throw Vulkan::Exception{"Failed to find suitable memory type"};
}

bool Device::checkPhysicalDeviceExtSupport(VkPhysicalDevice physical_device)
{
    auto device_extensions = getPhysicalDeviceExt(physical_device);
    std::unordered_set<std::string> required_extensions = {m_extensions.begin(),
                                                           m_extensions.end()};

    for (const auto &extension : device_extensions) {
        required_extensions.erase(extension.extensionName);
    }

    return required_extensions.empty();
}

swap_chain_support_details_t
Device::querySwapChainSupport(VkPhysicalDevice physical_device) const
{
    VkSurfaceKHR surface = m_context->surface();

    // Capabilities
    swap_chain_support_details_t details{};
    vkGetPhysicalDeviceSurfaceCapabilitiesKHR(physical_device, surface,
                                              &details.capabilities);

    // Format
    uint32_t format_count{0};
    vkGetPhysicalDeviceSurfaceFormatsKHR(physical_device, surface, &format_count,
                                         nullptr);

    details.formats.resize(format_count);
    vkGetPhysicalDeviceSurfaceFormatsKHR(physical_device, surface, &format_count,
                                         details.formats.data());

    // Present mode
    uint32_t present_mode_count{0};
    vkGetPhysicalDeviceSurfacePresentModesKHR(physical_device, surface,
                                              &present_mode_count, nullptr);

    details.present_mode.resize(present_mode_count);
    vkGetPhysicalDeviceSurfacePresentModesKHR(
        physical_device, surface, &present_mode_count, details.present_mode.data());

    return details;
}

std::vector<VkExtensionProperties>
Device::getPhysicalDeviceExt(VkPhysicalDevice physical_device)
{
    uint32_t count{0};
    vkEnumerateDeviceExtensionProperties(physical_device, nullptr, &count, nullptr);

    std::vector<VkExtensionProperties> extensions(count);
    vkEnumerateDeviceExtensionProperties(physical_device, nullptr, &count,
                                         extensions.data());

    return extensions;
}

} // namespace GE::Vulkan
