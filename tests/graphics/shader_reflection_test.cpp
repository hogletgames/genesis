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

#include "genesis/core/log.h"
#include "genesis/graphics/shader_precompiler.h"
#include "genesis/graphics/shader_reflection.h"

#include <gtest/gtest.h>

namespace {

constexpr auto INPUT_LAYOUT_SHADER = "tests/graphics/data/input_layout_test.vert";
constexpr auto BIND_DESC_SHADER = "tests/graphics/data/binding_description_test.vert";

class ShaderInputLayoutTest: public testing::Test
{
protected:
    void SetUp() override
    {
        GE::Log::initialize({});

        expected_layout = fillExpectedLayout();
        ASSERT_EQ(expected_layout.stride(), 248);
    }

    void TearDown() override { GE::Log::shutdown(); }

    GE::ShaderInputLayout fillExpectedLayout()
    {
        using AttrType = GE::shader_attribute_t::BaseType;

        GE::ShaderInputLayout layout{{{AttrType::INT, "a_int", 0, 4, 1, 1, 0},
                                      {AttrType::INT, "a_ivec2", 1, 4, 2, 1, 4},
                                      {AttrType::UINT, "a_uint", 2, 4, 1, 1, 12},
                                      {AttrType::UINT, "a_uvec3", 3, 4, 3, 1, 16},
                                      {AttrType::FLOAT, "a_float", 4, 4, 1, 1, 28},
                                      {AttrType::FLOAT, "a_vec3", 5, 4, 3, 1, 32},
                                      {AttrType::FLOAT, "a_mat3", 6, 4, 3, 3, 44},
                                      {AttrType::DOUBLE, "a_double", 9, 8, 1, 1, 80},
                                      {AttrType::DOUBLE, "a_dvec4", 10, 8, 4, 1, 88},
                                      {AttrType::DOUBLE, "a_dmat4", 11, 8, 4, 4, 120}}};
        return layout;
    }

    GE::ShaderReflection shader_reflection{
        GE::ShaderPrecompiler::compileFromFile(GE::Shader::Type::VERTEX, INPUT_LAYOUT_SHADER)};
    GE::ShaderInputLayout expected_layout;
};

class ShaderBindingDescriptionTest: public testing::Test
{
protected:
    GE::ResourceDescriptors fillExpectedBindDesc()
    {
        using Desc = GE::resource_descriptor_t;

        return {
            {"u_MVP", Desc::UNIFORM_BUFFER, 0, 0, 1},
            {"u_Elements", Desc::UNIFORM_BUFFER, 1, 0, 1},
            {"u_Texture", Desc::COMBINED_IMAGE_SAMPLER, 0, 1, 12},
        };
    }

    GE::ShaderReflection shader_reflection{
        GE::ShaderPrecompiler::compileFromFile(GE::Shader::Type::VERTEX, BIND_DESC_SHADER)};
    GE::ResourceDescriptors expected_binding_descriptions{fillExpectedBindDesc()};
};

TEST_F(ShaderInputLayoutTest, ShaderLayoutTest)
{
    const auto& shader_layout = shader_reflection.inputLayout();
    ASSERT_FALSE(shader_layout.attributes().empty());
    ASSERT_EQ(expected_layout.attributes(), shader_layout.attributes());
    ASSERT_EQ(expected_layout.stride(), shader_layout.stride());
}

TEST_F(ShaderBindingDescriptionTest, ShaderLayoutTest)
{
    const auto& shader_bind_desc = shader_reflection.resourceDescriptors();
    ASSERT_EQ(shader_bind_desc, expected_binding_descriptions);
}

} // namespace
