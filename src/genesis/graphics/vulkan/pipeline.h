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

#include <genesis/graphics/pipeline.h>
#include <genesis/math/types.h>

#include <vulkan/vulkan.h>

namespace GE {
class Shader;
struct pipeline_config_t;
struct push_constant_t;
} // namespace GE

namespace GE::Vulkan {

class DescriptorPool;
class Device;
class PipelineResources;
struct pipeline_config_t;

class Pipeline: public GE::Pipeline
{
public:
    Pipeline(Shared<Device> device, const Vulkan::pipeline_config_t& config);
    ~Pipeline();

    void bind(GPUCommandQueue* queue) override;
    void bind(GPUCommandQueue* queue, const std::string& name,
              const GE::UniformBuffer& ubo) override;
    void bind(GPUCommandQueue* queue, const std::string& name, const GE::Texture& texture) override;

    void pushConstant(GPUCommandQueue* queue, const std::string& name, bool value) override;
    void pushConstant(GPUCommandQueue* queue, const std::string& name, int32_t value) override;
    void pushConstant(GPUCommandQueue* queue, const std::string& name, uint32_t value) override;
    void pushConstant(GPUCommandQueue* queue, const std::string& name, float value) override;
    void pushConstant(GPUCommandQueue* queue, const std::string& name, double value) override;
    void pushConstant(GPUCommandQueue* queue, const std::string& name, const Vec2& value) override;
    void pushConstant(GPUCommandQueue* queue, const std::string& name, const Vec3& value) override;
    void pushConstant(GPUCommandQueue* queue, const std::string& name, const Mat4& value) override;

    NativeHandle nativeHandle() const override { return m_pipeline; }

    static Vulkan::pipeline_config_t createDefaultConfig(GE::pipeline_config_t base_config);

private:
    void createPipelineLayout();
    void createPipeline(Vulkan::pipeline_config_t config);

    void bindResource(GPUCommandQueue* queue, const std::string& name,
                      VkWriteDescriptorSet* write_descriptor_set);

    template<typename T>
    void pushConstantIfValid(GPUCommandQueue* queue, const std::string& name, T value);
    template<typename T>
    void pushConstantCmd(GPUCommandQueue* queue, const push_constant_t& push_constant, T value);

    void destroyVkHandles();

    Shared<Device> m_device;
    VkPipeline m_pipeline{VK_NULL_HANDLE};
    VkPipelineLayout m_pipeline_layout{VK_NULL_HANDLE};
    Scoped<PipelineResources> m_resources;
};

} // namespace GE::Vulkan
