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

#include <gtest/gtest.h>
#include <gmock/gmock.h>

using namespace GE;
using namespace testing;

namespace {

TEST(ResultTest, Success)
{
    constexpr int SUCCESS_RESULT = 42;

    auto result = Success::make(SUCCESS_RESULT);
    EXPECT_TRUE(result);
}

TEST(ResultTest, Failure)
{
    constexpr std::string_view ERROR_MESSAGE = "An error message";
    constexpr std::string_view FULL_ERROR_MESSAGE = "#0 result_test.cpp:54: An error message";

    auto error_result = Failure::make<int>(ERROR_MESSAGE);
    EXPECT_FALSE(error_result);
    EXPECT_EQ(error_result.errorMessage(), FULL_ERROR_MESSAGE);
}

TEST(ResultTest, NestedFailure)
{
    constexpr std::string_view FIRST_ERROR_MESSAGE = "First error message";
    constexpr std::string_view SECOND_ERROR_MESSAGE = "Second error message";
    constexpr std::string_view FULL_ERROR_MESSAGE = "#0 result_test.cpp:67: Second error message\n"
                                                    "#1 result_test.cpp:66: First error message";

    auto first_error_result = Failure::make<int>(FIRST_ERROR_MESSAGE);
    auto second_error_result = Failure::make<int>(SECOND_ERROR_MESSAGE, first_error_result);

    EXPECT_FALSE(first_error_result);
    EXPECT_FALSE(second_error_result);
    EXPECT_EQ(second_error_result.errorMessage(), FULL_ERROR_MESSAGE);
}

TEST(ResultTest, SuccessResultASNestedError)
{
    constexpr std::string_view ERROR_MESSAGE = "An error message";
    EXPECT_EXIT(Failure::make<int>(ERROR_MESSAGE, Success::make(1)), KilledBySignal(SIGTRAP), "");
}

} // namespace
