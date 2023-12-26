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

#include <genesis/graphics/shader.h>

#include <vulkan/vulkan.h>

namespace GE::Vulkan {

class Device;

class Shader: public GE::Shader
{
public:
    Shader(Shared<Device> device, Type type);
    ~Shader();

    bool compileFromFile(const std::string &filepath) override;
    bool compileFromSource(const std::string &source_code) override;

    Type type() const override { return m_type; }
    void *nativeHandle() const override { return m_shader_module; }
    const ShaderInputLayout &inputLayout() const override { return m_input_layout; }

    const ResourceDescriptors &resourceDescriptors() const override
    {
        return m_resource_descriptors;
    }

    const PushConstants &pushConstants() const override { return m_push_constants; }

private:
    bool compileFromFileOrSource(const std::string &filepath, const std::string &source_code);
    bool createShaderModule(const std::vector<uint32_t> &shader_code);

    Shared<Device> m_device;
    Type m_type{Type::NONE};
    VkShaderModule m_shader_module{VK_NULL_HANDLE};
    ShaderInputLayout m_input_layout;
    ResourceDescriptors m_resource_descriptors;
    PushConstants m_push_constants;
};

inline VkShaderModule toVkShaderModule(const GE::Shader &shader)
{
    return reinterpret_cast<VkShaderModule>(shader.nativeHandle());
}

} // namespace GE::Vulkan
