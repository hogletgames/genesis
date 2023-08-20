/*
 * BSD 3-Clause License
 *
 * Copyright (c) 2023, Dmitry Shilnenkov
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

#include "genesis/assets/resource_id.h"
#include "genesis/core/format.h"

#include <gtest/gtest.h>

using testing::Combine;
using testing::Values;
using ID = GE::Assets::ResourceID;

namespace {

class ResourceIDTest: public testing::Test
{};

class ResourceIDIsNotEqualTest: public testing::TestWithParam<ID>
{};

INSTANTIATE_TEST_SUITE_P(Parameterized, ResourceIDIsNotEqualTest,
                         Values(ID{"a", "a", "b"}, ID{"a", "b", "a"}, ID{"b", "a", "a"}));

class ResourceIDIsLessTest: public testing::TestWithParam<std::pair<ID, ID>>
{};

INSTANTIATE_TEST_SUITE_P(Parameterazed, ResourceIDIsLessTest,
                         Values(std::make_pair(ID{"a", "a", "a"}, ID{"a", "a", "b"}),
                                std::make_pair(ID{"a", "a", "a"}, ID{"a", "b", "a"}),
                                std::make_pair(ID{"a", "a", "a"}, ID{"b", "a", "a"})));

class ResourceIDHashNotEqual: public testing::TestWithParam<std::pair<ID, ID>>
{};

INSTANTIATE_TEST_SUITE_P(Parameterazed, ResourceIDHashNotEqual,
                         Values(std::make_pair(ID{"a", "a", "a"}, ID{"a", "a", "b"}),
                                std::make_pair(ID{"a", "a", "a"}, ID{"a", "b", "a"}),
                                std::make_pair(ID{"a", "a", "a"}, ID{"b", "a", "a"})));

TEST_F(ResourceIDTest, PackageGrouptName)
{
    static constexpr auto PACKAGE{"package"};
    static constexpr auto GROUP{"group"};
    static constexpr auto NAME{"name"};
    static const auto EXPECTED_ID = GE_FMTSTR("{}.{}.{}", PACKAGE, GROUP, NAME);

    ID id{PACKAGE, GROUP, NAME};

    EXPECT_EQ(id.package(), PACKAGE);
    EXPECT_EQ(id.group(), GROUP);
    EXPECT_EQ(id.name(), NAME);
    EXPECT_EQ(id.id(), EXPECTED_ID);
}

TEST_F(ResourceIDTest, isEqual)
{
    ID id_lhs{"a", "b", "b"};
    ID id_rhs{"a", "b", "b"};

    EXPECT_EQ(id_lhs, id_rhs);
}

TEST_F(ResourceIDTest, hashIsEqual)
{
    auto hasher = std::hash<ID>{};

    ID id_lhs{"a", "b", "b"};
    ID id_rhs{"a", "b", "b"};

    EXPECT_EQ(hasher(id_lhs), hasher(id_rhs));
}

TEST_P(ResourceIDIsNotEqualTest, isNotEqual)
{
    ID id_lhs{"a", "a", "a"};
    auto id_rhs = GetParam();

    EXPECT_NE(id_lhs, id_rhs);
}

TEST_P(ResourceIDIsLessTest, isLess)
{
    const auto& [lhs, rhs] = GetParam();

    EXPECT_LT(lhs, rhs);
}

TEST_P(ResourceIDHashNotEqual, NotEqual)
{
    auto hasher = std::hash<ID>{};
    const auto& [lhs, rhs] = GetParam();

    EXPECT_NE(hasher(lhs), hasher(rhs));
}

} // namespace
