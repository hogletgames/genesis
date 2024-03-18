/*
 * BSD 3-Clause License
 *
 * Copyright (c) 2022, Dmitry Shilnenkov
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

#include "utils.h"
#include "instance.h"

#include "genesis/core/asserts.h"

#define GET_INSTANCE_PROC_ADDR(func_name) getInstanceProcAddr<PFN_##func_name>(#func_name)

namespace GE::Vulkan {
namespace {

template<typename Func>
Func getInstanceProcAddr(std::string_view func_name)
{
    auto func =
        reinterpret_cast<Func>(vkGetInstanceProcAddr(Instance::instance(), func_name.data()));
    GE_CORE_ASSERT(func != nullptr, "Failed to load function: {}", func_name);
    return func;
}

} // namespace

VkSampleCountFlagBits toVkSampleCountFlag(uint8_t sample_count)
{
    return static_cast<VkSampleCountFlagBits>(sample_count);
}

void cmdBeginRendering(VkCommandBuffer cmd_buffer, const VkRenderingInfo* rendering_info)
{
    static auto begin_rendering = GET_INSTANCE_PROC_ADDR(vkCmdBeginRenderingKHR);
    begin_rendering(cmd_buffer, rendering_info);
}

void cmdEndRendering(VkCommandBuffer cmd_buffer)
{
    static auto end_rendering = GET_INSTANCE_PROC_ADDR(vkCmdEndRenderingKHR);
    end_rendering(cmd_buffer);
}

} // namespace GE::Vulkan
