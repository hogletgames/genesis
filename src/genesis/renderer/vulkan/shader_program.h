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
#ifndef GENESIS_RENDERER_VULKAN_SHADER_PROGRAM_H_
#define GENESIS_RENDERER_VULKAN_SHADER_PROGRAM_H_

#include <genesis/renderer/shader_program.h>

#include <genesis/renderer/gpu_command_queue.h>
#include <vulkan/vulkan.h>

namespace GE::Vulkan {

class Device;
class Pipeline;

class ShaderProgram: public GE::ShaderProgram
{
public:
    ShaderProgram(Shared<Device> device, Shared<GE::Shader> vert,
                  Shared<GE::Shader> frag);
    ~ShaderProgram();

    void bind(GPUCommandQueue *queue) const override;

private:
    Scoped<Pipeline> createPipeline(Shared<Shader> vert, Shared<Shader> frag);
    VkPipelineLayout createPipelineLayout();
    void destroyVulkanHandles();

    Shared<Device> m_device;
    VkPipelineLayout m_pipeline_layout{VK_NULL_HANDLE};
    Scoped<Pipeline> m_pipeline;
};

} // namespace GE::Vulkan

#endif // GENESIS_RENDERER_VULKAN_SHADER_PROGRAM_H_
