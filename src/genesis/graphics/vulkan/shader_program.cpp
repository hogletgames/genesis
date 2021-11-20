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

#include "shader_program.h"
#include "device.h"
#include "graphics_context.h"
#include "pipeline.h"
#include "swap_chain.h"
#include "vulkan_exception.h"

#include "genesis/core/format.h"

namespace GE::Vulkan {

ShaderProgram::ShaderProgram(Shared<Device> device, Shared<GE::Shader> vert,
                             Shared<GE::Shader> frag)
    : m_device{std::move(device)}
    , m_pipeline_layout{createPipelineLayout()}
    , m_pipeline{createPipeline(std::move(vert), std::move(frag))}
{}

ShaderProgram::~ShaderProgram()
{
    destroyVulkanHandles();
}

void ShaderProgram::bind(GPUCommandQueue* queue) const
{
    m_pipeline->bind(queue);
}

Scoped<Pipeline> ShaderProgram::createPipeline(Shared<Shader> vert, Shared<Shader> frag)
{
    if (vert->type() != Shader::Type::VERTEX || frag->type() != Shader::Type::FRAGMENT) {
        auto error = GE_FMTSTR("Incorrect shaders types: vertex = {}, fragment = {}",
                               vert->type(), frag->type());
        throw Vulkan::Exception{error};
    }

    // TODO: render pass from Renderer
    VkRenderPass render_pass{VK_NULL_HANDLE};

    auto pipeline_config = Pipeline::makeDefaultConfig();
    pipeline_config.vert_shader = std::move(vert);
    pipeline_config.frag_shader = std::move(frag);
    pipeline_config.pipeline_layout = m_pipeline_layout;
    pipeline_config.render_pass = render_pass;

    return makeScoped<Pipeline>(m_device, pipeline_config);
}

VkPipelineLayout ShaderProgram::createPipelineLayout()
{
    VkPipelineLayout pipeline_layout{VK_NULL_HANDLE};
    VkPipelineLayoutCreateInfo pipeline_layout_info{};
    pipeline_layout_info.sType = VK_STRUCTURE_TYPE_PIPELINE_LAYOUT_CREATE_INFO;
    pipeline_layout_info.pSetLayouts = nullptr;
    pipeline_layout_info.setLayoutCount = 0;
    pipeline_layout_info.pPushConstantRanges = nullptr;
    pipeline_layout_info.pushConstantRangeCount = 0;

    if (vkCreatePipelineLayout(m_device->device(), &pipeline_layout_info, nullptr,
                               &pipeline_layout) != VK_SUCCESS) {
        throw Vulkan::Exception{"Failed to create Pipeline Layout"};
    }

    return pipeline_layout;
}

void ShaderProgram::destroyVulkanHandles()
{
    vkDestroyPipelineLayout(m_device->device(), m_pipeline_layout, nullptr);
    m_pipeline_layout = nullptr;
}

} // namespace GE::Vulkan
