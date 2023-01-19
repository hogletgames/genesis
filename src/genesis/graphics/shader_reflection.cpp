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

#include "shader_reflection.h"

#include "genesis/core/utils.h"

#include <spirv_cross.hpp>

using AttrType = GE::shader_attribute_t::BaseType;
using DescType = GE::resource_descriptor_t::Type;

namespace {

constexpr uint32_t BYTE_BIT{8};

AttrType toAttributeType(const spirv_cross::SPIRType& spir_type)
{
    using SPIRBaseType = spirv_cross::SPIRType::BaseType;

    static const std::unordered_map<SPIRBaseType, AttrType> to_type = {
        {SPIRBaseType::Int, AttrType::INT},
        {SPIRBaseType::UInt, AttrType::UINT},
        {SPIRBaseType::Float, AttrType::FLOAT},
        {SPIRBaseType::Double, AttrType::DOUBLE}};

    return GE::getValue(to_type, spir_type.basetype, AttrType::NONE);
}

uint32_t toAttributeSize(uint32_t resource_size)
{
    return resource_size / BYTE_BIT;
}

GE::shader_attribute_t toAttribute(const spirv_cross::Compiler& compiler,
                                   const spirv_cross::Resource& resource)
{
    const auto& spir_type = compiler.get_type(resource.type_id);

    GE::shader_attribute_t attribute{};
    attribute.base_type = toAttributeType(spir_type);
    attribute.name = resource.name;
    attribute.location = compiler.get_decoration(resource.id, spv::DecorationLocation);
    attribute.size = toAttributeSize(spir_type.width);
    attribute.vec_size = spir_type.vecsize;
    attribute.vec_column = spir_type.columns;
    attribute.offset = compiler.get_decoration(resource.id, spv::DecorationOffset);

    return attribute;
}

uint32_t toResourceDescriptorCount(const spirv_cross::SmallVector<uint32_t>& type_array)
{
    uint32_t count{1};
    std::for_each(type_array.begin(), type_array.end(),
                  [&count](uint32_t dim_size) { count *= dim_size; });
    return count;
}

GE::resource_descriptor_t toResourceDescriptors(const spirv_cross::Compiler& compiler,
                                                const spirv_cross::Resource& resource,
                                                DescType type)
{
    return {
        resource.name,
        type,
        compiler.get_decoration(resource.id, spv::DecorationDescriptorSet),
        compiler.get_decoration(resource.id, spv::DecorationBinding),
        toResourceDescriptorCount(compiler.get_type(resource.type_id).array),
    };
}

} // namespace

namespace GE {

struct ShaderReflection::Impl {
    explicit Impl(std::vector<uint32_t> shader_cache)
        : compiler{std::move(shader_cache)}
    {}

    spirv_cross::Compiler compiler;
};

ShaderReflection::ShaderReflection(const std::vector<uint32_t>& shader_cache)
    : m_pimpl{tryMakeScoped<Impl>(shader_cache)}
{}

ShaderReflection::~ShaderReflection() = default;

ShaderInputLayout ShaderReflection::inputLayout() const
{
    if (!m_pimpl) {
        return {};
    }

    const auto& resources = m_pimpl->compiler.get_shader_resources();
    std::deque<shader_attribute_t> attributes;

    for (const auto& resource : resources.stage_inputs) {
        attributes.push_back(toAttribute(m_pimpl->compiler, resource));
    }

    std::sort(attributes.begin(), attributes.end(),
              [](const auto& lhs, const auto& rhs) { return lhs.location < rhs.location; });

    uint32_t offset{0};

    for (auto& attribute : attributes) {
        attribute.offset = offset;
        offset += attribute.fullSize();
    }

    return ShaderInputLayout{std::move(attributes)};
}

ResourceDescriptors ShaderReflection::resourceDescriptors() const
{
    if (!m_pimpl) {
        return {};
    }

    using Resources = spirv_cross::SmallVector<spirv_cross::Resource>;

    ResourceDescriptors descriptors;
    const auto& compiler = m_pimpl->compiler;
    const auto& resources = compiler.get_shader_resources();

    auto fill_descriptors = [&compiler, &descriptors](const Resources& resources, DescType type) {
        for (const auto& resource : resources) {
            descriptors.push_back(toResourceDescriptors(compiler, resource, type));
        }
    };

    fill_descriptors(resources.uniform_buffers, DescType::UNIFORM_BUFFER);
    fill_descriptors(resources.sampled_images, DescType::COMBINED_IMAGE_SAMPLER);
    return descriptors;
}

} // namespace GE
