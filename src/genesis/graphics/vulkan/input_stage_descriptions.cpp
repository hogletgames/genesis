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

#include "input_stage_descriptions.h"
#include "vulkan_exception.h"

#include <optional>

namespace {

std::optional<VkFormat> getIntFormat(uint32_t vec_size)
{
    switch (vec_size) {
        case 1: return VK_FORMAT_R8_SINT;
        case 2: return VK_FORMAT_R8G8_SINT;
        case 3: return VK_FORMAT_R8G8B8_SINT;
        case 4: return VK_FORMAT_R8G8B8A8_SINT;
        default: return {};
    }
}

std::optional<VkFormat> getUintFormat(uint32_t vec_size)
{
    switch (vec_size) {
        case 1: return VK_FORMAT_R32_UINT;
        case 2: return VK_FORMAT_R32G32_UINT;
        case 3: return VK_FORMAT_R32G32B32_UINT;
        case 4: return VK_FORMAT_R32G32B32A32_UINT;
        default: return {};
    }
}

std::optional<VkFormat> getFloatFormat(uint32_t vec_size)
{
    switch (vec_size) {
        case 1: return VK_FORMAT_R32_SFLOAT;
        case 2: return VK_FORMAT_R32G32_SFLOAT;
        case 3: return VK_FORMAT_R32G32B32_SFLOAT;
        case 4: return VK_FORMAT_R32G32B32A32_SFLOAT;
        default: return {};
    }
}

std::optional<VkFormat> getDoubleFormat(uint32_t vec_size)
{
    switch (vec_size) {
        case 1: return VK_FORMAT_R64_SFLOAT;
        case 2: return VK_FORMAT_R64G64_SFLOAT;
        case 3: return VK_FORMAT_R64G64B64_SFLOAT;
        case 4: return VK_FORMAT_R64G64B64A64_SFLOAT;
        default: return {};
    }
}

std::optional<VkFormat> getFormat(const GE::shader_attribute_t& attribute)
{
    using AttrType = GE::shader_attribute_t::BaseType;

    switch (attribute.base_type) {
        case AttrType::INT: return getIntFormat(attribute.vec_size);
        case AttrType::UINT: return getUintFormat(attribute.vec_size);
        case AttrType::FLOAT: return getFloatFormat(attribute.vec_size);
        case AttrType::DOUBLE: return getDoubleFormat(attribute.vec_size);
        case AttrType::NONE:
        default: return {};
    }
}

VkVertexInputAttributeDescription
inputAttributeDescription(const GE::shader_attribute_t& attribute)
{
    auto format = getFormat(attribute);

    if (!format.has_value()) {
        throw GE::Vulkan::Exception{"Failed to get attribute format"};
    }

    VkVertexInputAttributeDescription description{};
    description.binding = 0;
    description.location = attribute.location;
    description.offset = attribute.offset;
    description.format = format.value();

    return description;
}
/*
 * TODO: compute location according to
 *       https://www.khronos.org/registry/vulkan/specs/1.2-extensions/html/vkspec.html#fxvertex-attrib-location
 */
std::vector<VkVertexInputAttributeDescription>
matrixInputAttributeDescriptions(GE::shader_attribute_t attribute)
{
    std::vector<VkVertexInputAttributeDescription> descriptions;

    for (uint32_t i{0}; i < attribute.vec_column; i++) {
        descriptions.push_back(inputAttributeDescription(attribute));

        attribute.location += 1;
        attribute.offset += attribute.size * attribute.vec_column * i;
    }

    return descriptions;
}

} // namespace

namespace GE::Vulkan {

std::vector<VkVertexInputAttributeDescription>
vertexInputAttributeDescriptions(const ShaderInputLayout& input_layout)
{
    std::vector<VkVertexInputAttributeDescription> descriptions;

    for (const auto& attribute : input_layout.attributes()) {
        if (attribute.vec_column == 1) {
            descriptions.push_back(inputAttributeDescription(attribute));
        } else {
            auto matrix_attributes = matrixInputAttributeDescriptions(attribute);
            std::copy(matrix_attributes.begin(), matrix_attributes.end(),
                      std::back_inserter(descriptions));
        }
    }

    return descriptions;
}

VkVertexInputBindingDescription
vertexInputBindDescription(const ShaderInputLayout& input_layout)
{
    VkVertexInputBindingDescription description{};
    description.binding = 0;
    description.stride = input_layout.stride();
    description.inputRate = VK_VERTEX_INPUT_RATE_VERTEX;

    return description;
}

} // namespace GE::Vulkan
