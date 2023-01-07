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

#include "instance.h"
#include "utils.h"
#include "vulkan_exception.h"

#include "genesis/core/asserts.h"
#include "genesis/core/version.h"

#include <SDL_vulkan.h>

namespace {

constexpr int VULKAN_SEVERITY{VK_DEBUG_UTILS_MESSAGE_SEVERITY_ERROR_BIT_EXT |
                              VK_DEBUG_UTILS_MESSAGE_SEVERITY_WARNING_BIT_EXT |
                              VK_DEBUG_UTILS_MESSAGE_SEVERITY_INFO_BIT_EXT};

VKAPI_ATTR VkBool32 VKAPI_CALL debugCallback(
    VkDebugUtilsMessageSeverityFlagBitsEXT severity, VkDebugUtilsMessageTypeFlagsEXT type,
    const VkDebugUtilsMessengerCallbackDataEXT* callback_data,
    [[maybe_unused]] void* user_data)
{
    const char* type_str = "Unknown";
    const char* pattern = "[Vk {}]: {}";

    if (type >= VK_DEBUG_UTILS_MESSAGE_TYPE_PERFORMANCE_BIT_EXT) {
        type_str = "Performance";
    } else if (type >= VK_DEBUG_UTILS_MESSAGE_TYPE_VALIDATION_BIT_EXT) {
        type_str = "Validation";
    } else if (type >= VK_DEBUG_UTILS_MESSAGE_TYPE_GENERAL_BIT_EXT) {
        type_str = "General";
    }

    if (severity >= VK_DEBUG_UTILS_MESSAGE_SEVERITY_ERROR_BIT_EXT) {
        GE_CORE_ERR(pattern, type_str, callback_data->pMessage);
    } else if (severity >= VK_DEBUG_UTILS_MESSAGE_SEVERITY_WARNING_BIT_EXT) {
        GE_CORE_WARN(pattern, type_str, callback_data->pMessage);
    } else if (severity >= VK_DEBUG_UTILS_MESSAGE_SEVERITY_INFO_BIT_EXT) {
        GE_CORE_INFO(pattern, type_str, callback_data->pMessage);
    } else if (severity >= VK_DEBUG_UTILS_MESSAGE_SEVERITY_VERBOSE_BIT_EXT) {
        GE_CORE_DBG(pattern, type_str, callback_data->pMessage);
    } else {
        GE_CORE_ERR("[Vk {}/Unknown]: {}", type_str, callback_data->pMessage);
    }

    return VK_FALSE;
}

VkDebugUtilsMessengerCreateInfoEXT
getDebugMsgrCreateInfo(VkDebugUtilsMessageSeverityFlagsEXT severity)
{
    VkDebugUtilsMessengerCreateInfoEXT create_info{};
    create_info.sType = VK_STRUCTURE_TYPE_DEBUG_UTILS_MESSENGER_CREATE_INFO_EXT;
    create_info.messageSeverity = severity;
    create_info.messageType = VK_DEBUG_UTILS_MESSAGE_TYPE_GENERAL_BIT_EXT |
                              VK_DEBUG_UTILS_MESSAGE_TYPE_VALIDATION_BIT_EXT |
                              VK_DEBUG_UTILS_MESSAGE_TYPE_PERFORMANCE_BIT_EXT;
    create_info.pfnUserCallback = debugCallback;
    create_info.pUserData = nullptr;

    return create_info;
}

} // namespace

namespace GE::Vulkan {

void Instance::initialize(void* native_window, std::string_view app_name)
{
    get()->createInstance(native_window, app_name);
#ifndef GE_DISABLE_DEBUG
    get()->createDebugUtilsMessenger();
#endif // GE_DISABLE_DEBUG
}

void Instance::shutdown()
{
    get()->destroyVulkanHandles();
}

void Instance::createInstance(void* native_window, std::string_view app_name)
{
    GE_CORE_INFO("Creating Vulkan Instance...");
    auto* window = reinterpret_cast<SDL_Window*>(native_window);
    auto window_extensions =
        vulkanGet<const char*>(::SDL_Vulkan_GetInstanceExtensions, window);

#ifndef GE_DISABLE_DEBUG
    window_extensions.push_back(VK_EXT_DEBUG_UTILS_EXTENSION_NAME);
#endif // GE_DISABLE_DEBUG

#if GE_PLATFORM_APPLE
    window_extensions.push_back(VK_KHR_PORTABILITY_ENUMERATION_EXTENSION_NAME);
#endif // GE_PLATFORM_APPLE

    VkApplicationInfo app_info{};
    app_info.sType = VK_STRUCTURE_TYPE_APPLICATION_INFO;
    app_info.pApplicationName = app_name.data();
    app_info.apiVersion = VK_MAKE_VERSION(1, 0, 0);
    app_info.pEngineName = ENGINE_NAME.data();
    app_info.engineVersion = VK_MAKE_VERSION(VERSION_MAJOR, VERSION_MAJOR, VERSION_MAJOR);
    app_info.apiVersion = VK_API_VERSION_1_2;

    VkInstanceCreateInfo instance_info{};
    instance_info.sType = VK_STRUCTURE_TYPE_INSTANCE_CREATE_INFO;
    instance_info.pApplicationInfo = &app_info;
    instance_info.enabledExtensionCount = window_extensions.size();
    instance_info.ppEnabledExtensionNames = window_extensions.data();

#if GE_PLATFORM_APPLE
    instance_info.flags |= VK_INSTANCE_CREATE_ENUMERATE_PORTABILITY_BIT_KHR;
#endif // GE_PLATFORM_APPLE

#ifndef GE_DISABLE_DEBUG
    auto debug_info = getDebugMsgrCreateInfo(VULKAN_SEVERITY);
    std::array<const char*, 1> validation_layers = {"VK_LAYER_KHRONOS_validation"};

    instance_info.ppEnabledLayerNames = validation_layers.data();
    instance_info.enabledLayerCount = validation_layers.size();
    instance_info.pNext = &debug_info;
#endif // GE_DISABLE_DEBUG

    if (vkCreateInstance(&instance_info, nullptr, &m_instance) != VK_SUCCESS) {
        throw Vulkan::Exception{"Failed to create Vulkan Instance"};
    }
}

void Instance::createDebugUtilsMessenger()
{
    GE_CORE_INFO("Creating Vulkan Debug Messenger...");
    auto debug_info = getDebugMsgrCreateInfo(VULKAN_SEVERITY);

    if (createDebugUtilsMessengerEXT(m_instance, &debug_info, nullptr, &m_debug_utils) !=
        VK_SUCCESS) {
        throw Vulkan::Exception{"Failed to create Vulkan Debug Messenger"};
    }
}

void Instance::destroyVulkanHandles()
{
    GE_CORE_INFO("Destroying Vulkan Instance...");
    destroyDebugUtilsMessengerEXT(m_instance, m_debug_utils, nullptr);
    m_debug_utils = VK_NULL_HANDLE;

    vkDestroyInstance(m_instance, nullptr);
    m_instance = VK_NULL_HANDLE;
}

} // namespace GE::Vulkan
