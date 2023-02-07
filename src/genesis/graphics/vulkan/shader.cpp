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

#include "shader.h"
#include "device.h"

#include "genesis/core/log.h"
#include "genesis/graphics/shader_precompiler.h"
#include "genesis/graphics/shader_reflection.h"

namespace GE::Vulkan {

Shader::Shader(Shared<Device> device, Shader::Type type)
    : m_device{std::move(device)}
    , m_type{type}
{}

Shader::~Shader()
{
    vkDestroyShaderModule(m_device->device(), m_shader_module, nullptr);
}

bool Shader::compileFromFile(const std::string& filepath)
{
    return compileFromFileOrSource(filepath, {});
}

bool Shader::compileFromSource(const std::string& source_code)
{
    return compileFromFileOrSource({}, source_code);
}

bool Shader::compileFromFileOrSource(const std::string& filepath, const std::string& source_code)
{
    ShaderCache shader_cache;

    if (!filepath.empty()) {
        shader_cache = ShaderPrecompiler::compileFromFile(m_type, filepath);
    } else if (!source_code.empty()) {
        shader_cache = ShaderPrecompiler::compileFromSource(m_type, source_code);
    } else {
        GE_CORE_ERR("There must be either filepath or Shader source code");
        return false;
    }

    if (shader_cache.empty()) {
        GE_CORE_ERR("Failed to get Shader Cache");
        return false;
    }

    if (!createShaderModule(shader_cache)) {
        return false;
    }

    ShaderReflection reflection{shader_cache};
    m_input_layout = reflection.inputLayout();
    m_resource_descriptors = reflection.resourceDescriptors();
    return true;
}

bool Shader::createShaderModule(const std::vector<uint32_t>& shader_code)
{
    VkShaderModuleCreateInfo create_info{};
    create_info.sType = VK_STRUCTURE_TYPE_SHADER_MODULE_CREATE_INFO;
    create_info.codeSize = shader_code.size() * sizeof(uint32_t);
    create_info.pCode = shader_code.data();

    if (vkCreateShaderModule(m_device->device(), &create_info, nullptr, &m_shader_module) !=
        VK_SUCCESS) {
        GE_CORE_ERR("Failed to create Vulkan Shader Module");
        return false;
    }

    return true;
}

} // namespace GE::Vulkan
