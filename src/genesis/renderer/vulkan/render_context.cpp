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
#include "sdl_platform_window.h"
#include "utils.h"
#include "vulkan_exception.h"

#include "genesis/core/log.h"
#include "genesis/core/version.h"

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

RenderContext::RenderContext() = default;

RenderContext::~RenderContext()
{
    destroyVulkanHandles();
}

bool RenderContext::initialize(void* window)
{
    GE_CORE_INFO("Initializing Vulkan Context...");

    try {
        m_window = makeScoped<SDL::PlatformWindow>(reinterpret_cast<SDL_Window*>(window));
        createInstance();
#ifndef GE_DISABLE_DEBUG
        createDebugUtilsMessenger();
#endif // GE_DISABLE_DEBUG
        m_surface = m_window->createSurface(m_instance);
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

    destroyVulkanHandles();
    m_window.reset();
}

void RenderContext::createInstance()
{
    auto window_extensions = m_window->vulkanExtensions();

#ifndef GE_DISABLE_DEBUG
    window_extensions.push_back(VK_EXT_DEBUG_UTILS_EXTENSION_NAME);
#endif // GE_DISABLE_DEBUG

    VkApplicationInfo app_info{};
    app_info.sType = VK_STRUCTURE_TYPE_APPLICATION_INFO;
    app_info.pApplicationName = m_window->title();
    app_info.apiVersion = VK_MAKE_VERSION(1, 0, 0);
    app_info.pEngineName = ENGINE_NAME;
    app_info.engineVersion = VK_MAKE_VERSION(VER_MAJOR, VER_MINOR, VER_PATCH);
    app_info.apiVersion = VK_API_VERSION_1_2;

    VkInstanceCreateInfo instance_info{};
    instance_info.sType = VK_STRUCTURE_TYPE_INSTANCE_CREATE_INFO;
    instance_info.pApplicationInfo = &app_info;
    instance_info.enabledExtensionCount = window_extensions.size();
    instance_info.ppEnabledExtensionNames = window_extensions.data();

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

void RenderContext::createDebugUtilsMessenger()
{
    auto debug_info = getDebugMsgrCreateInfo(VULKAN_SEVERITY);

    if (createDebugUtilsMessengerEXT(m_instance, &debug_info, nullptr, &m_debug_utils) !=
        VK_SUCCESS) {
        throw Vulkan::Exception{"Failed to create Vulkan Debug Messenger"};
    }
}

void RenderContext::destroyVulkanHandles()
{
    vkDestroySurfaceKHR(m_instance, m_surface, nullptr);
    m_surface = VK_NULL_HANDLE;

    destroyDebugUtilsMessengerEXT(m_instance, m_debug_utils, nullptr);
    m_debug_utils = VK_NULL_HANDLE;

    vkDestroyInstance(m_instance, nullptr);
    m_instance = VK_NULL_HANDLE;
}

} // namespace GE::Vulkan
