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

#pragma once

#include <vulkan/vulkan.h>

#include <functional>
#include <type_traits>
#include <utility>
#include <vector>

namespace GE::Vulkan {

template<typename FuncPFN, typename... Args>
VkResult loadInstanceFuncAndCall(const char* func_name, VkInstance instance, Args&&... args)
{
    auto* void_func = vkGetInstanceProcAddr(instance, func_name);

    if (void_func == nullptr) {
        return VK_ERROR_EXTENSION_NOT_PRESENT;
    }

    auto* func = reinterpret_cast<FuncPFN>(void_func);
    using FuncRet = std::result_of_t<FuncPFN(VkInstance, Args...)>;
    constexpr bool is_return_void = std::is_same_v<FuncRet, void>;

    if constexpr (is_return_void) {
        func(instance, std::forward<Args>(args)...);
    } else {
        return func(instance, std::forward<Args>(args)...);
    }

    return VK_SUCCESS;
}

inline VkResult createDebugUtilsMessengerEXT(VkInstance instance,
                                             const VkDebugUtilsMessengerCreateInfoEXT* create_info,
                                             const VkAllocationCallbacks* allocator,
                                             VkDebugUtilsMessengerEXT* debug_messenger)
{
    return loadInstanceFuncAndCall<PFN_vkCreateDebugUtilsMessengerEXT>(
        "vkCreateDebugUtilsMessengerEXT", instance, create_info, allocator, debug_messenger);
}

inline void destroyDebugUtilsMessengerEXT(VkInstance instance,
                                          VkDebugUtilsMessengerEXT debug_messenger,
                                          const VkAllocationCallbacks* allocator)
{
    loadInstanceFuncAndCall<PFN_vkDestroyDebugUtilsMessengerEXT>(
        "vkDestroyDebugUtilsMessengerEXT", instance, debug_messenger, allocator);
}

template<typename T, typename Func, typename... Args>
std::vector<T> vulkanGet(Func&& f, Args&&... args)
{
    uint32_t count{0};
    std::invoke(std::forward<Func>(f), std::forward<Args>(args)..., &count, nullptr);

    std::vector<T> data(count);
    std::invoke(std::forward<Func>(f), std::forward<Args>(args)..., &count, data.data());

    return data;
}

VkSampleCountFlagBits toVkSampleCountFlag(uint8_t sample_count);

void cmdBeginRendering(VkCommandBuffer cmd_buffer, const VkRenderingInfo* rendering_info);
void cmdEndRendering(VkCommandBuffer cmd_buffer);

} // namespace GE::Vulkan
