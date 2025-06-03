/*
 * BSD 3-Clause License
 *
 * Copyright (c) 2025, Dmitry Shilnenkov
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

#include "genesis/core/result.h"

#include <gmock/gmock.h>
#include <gtest/gtest.h>

using namespace GE;
using namespace testing;

namespace {

class MoveOnlyClass
{
public:
    constexpr explicit MoveOnlyClass(int value)
        : m_value{value} {};

    ~MoveOnlyClass() = default;

    MoveOnlyClass(MoveOnlyClass&& other) noexcept = default;
    MoveOnlyClass& operator=(MoveOnlyClass&& other) noexcept = default;

    MoveOnlyClass(const MoveOnlyClass& other) = delete;
    MoveOnlyClass& operator=(const MoveOnlyClass& other) = delete;

    auto operator<=>(const MoveOnlyClass& other) const = default;

private:
    int m_value{-1};
};

TEST(ResultTest, Failure_SimpleMessage)
{
    constexpr std::string_view ERROR_MESSAGE = "An error message";
    constexpr std::string_view FULL_ERROR_MESSAGE = "#0 result_test.cpp:68: An error message";

    auto error_result = Result<int>::makeFailure(ERROR_MESSAGE);
    ASSERT_FALSE(error_result);
    EXPECT_EQ(error_result.errorMessage(), FULL_ERROR_MESSAGE);
}

TEST(ResultTest, Failure_FormattedMessage)
{
    constexpr std::string_view FULL_ERROR_MESSAGE =
        "#0 result_test.cpp:78: An error message with value: 42";

    auto error_result = Result<int>::makeFailure("An error message with value: {}", 42);
    ASSERT_FALSE(error_result);
    EXPECT_EQ(error_result.errorMessage(), FULL_ERROR_MESSAGE);
}

TEST(ResultTest, TwoNestedFailure)
{
    constexpr std::string_view FULL_ERROR_MESSAGE_1ST = "#0 result_test.cpp:90: First error: 42";
    constexpr std::string_view FULL_ERROR_MESSAGE_2ND =
        "#0 result_test.cpp:92: Second error: Hello world!\n"
        "#1 result_test.cpp:90: First error: 42";

    auto first_error_result = Result<int>::makeFailure("First error: {}", 42);
    auto second_error_result =
        Result<int>::makeFailure(first_error_result, "Second error: {}", "Hello world!");

    ASSERT_FALSE(first_error_result);
    EXPECT_EQ(first_error_result.errorMessage(), FULL_ERROR_MESSAGE_1ST);

    ASSERT_FALSE(second_error_result);
    EXPECT_EQ(second_error_result.errorMessage(), FULL_ERROR_MESSAGE_2ND);
}

TEST(ResultTest, ThreeNestedFailuresWithDifferentTypes)
{
    constexpr std::string_view FIRST_ERROR_MESSAGE = "First error message";
    constexpr std::string_view SECOND_ERROR_MESSAGE = "Second error message";
    constexpr std::string_view THIRD_ERROR_MESSAGE = "Third error message";

    constexpr std::string_view FULL_ERROR_MESSAGE_1ST =
        "#0 result_test.cpp:117: First error message";
    constexpr std::string_view FULL_ERROR_MESSAGE_2ND =
        "#0 result_test.cpp:119: Second error message\n"
        "#1 result_test.cpp:117: First error message";
    constexpr std::string_view FULL_ERROR_MESSAGE_3RD =
        "#0 result_test.cpp:121: Third error message\n"
        "#1 result_test.cpp:119: Second error message\n"
        "#2 result_test.cpp:117: First error message";

    auto first_error_result = Result<int>::makeFailure(FIRST_ERROR_MESSAGE);
    auto second_error_result =
        Result<double>::makeFailure(first_error_result, SECOND_ERROR_MESSAGE);
    auto third_error_result =
        Result<std::string>::makeFailure(second_error_result, THIRD_ERROR_MESSAGE);

    ASSERT_FALSE(first_error_result);
    EXPECT_EQ(first_error_result.errorMessage(), FULL_ERROR_MESSAGE_1ST);

    ASSERT_FALSE(second_error_result);
    EXPECT_EQ(second_error_result.errorMessage(), FULL_ERROR_MESSAGE_2ND);

    ASSERT_FALSE(third_error_result);
    EXPECT_EQ(third_error_result.errorMessage(), FULL_ERROR_MESSAGE_3RD);
}

TEST(ResultTest, Success)
{
    constexpr int SUCCESS_RESULT = 42;
    Result<int>   result(SUCCESS_RESULT);

    ASSERT_TRUE(result);
    EXPECT_EQ(result.value(), SUCCESS_RESULT);
}

TEST(ResultTest, Success_MoveOnlyType)
{
    constexpr int         EXPECTED_VALUE{42};
    Result<MoveOnlyClass> result{MoveOnlyClass{EXPECTED_VALUE}};

    ASSERT_TRUE(result);
    EXPECT_EQ(result.value(), MoveOnlyClass{EXPECTED_VALUE});
}

TEST(ResultTest, Success_MoveValueFromResult)
{
    constexpr int         EXPECTED_VALUE{42};
    Result<MoveOnlyClass> result{MoveOnlyClass{EXPECTED_VALUE}};

    ASSERT_TRUE(result);

    MoveOnlyClass move_result = std::move(result.value());
    EXPECT_EQ(move_result, MoveOnlyClass{EXPECTED_VALUE});
}

} // namespace
