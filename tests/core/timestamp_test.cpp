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

#include "genesis/core/timestamp.h"

#include <gtest/gtest.h>

namespace {

class TimestampTest: public testing::Test
{};

TEST_F(TimestampTest, Conversion)
{
    GE::Timestamp ts{0.123456789};

    EXPECT_DOUBLE_EQ(ts.sec(), 0.123456789);
    EXPECT_DOUBLE_EQ(ts.ms(), 123.456789);
    EXPECT_DOUBLE_EQ(ts.us(), 123456.789);
    EXPECT_DOUBLE_EQ(ts.ns(), 123456789.0);
}

TEST_F(TimestampTest, Addition)
{
    GE::Timestamp time_one{0.123456789};
    GE::Timestamp time_two{0.987654321};
    GE::Timestamp sum = time_one + time_two;

    EXPECT_DOUBLE_EQ(sum.sec(), 1.11111111);
    EXPECT_DOUBLE_EQ(sum.ms(), 1111.11111);
    EXPECT_DOUBLE_EQ(sum.us(), 1111111.11);
    EXPECT_DOUBLE_EQ(sum.ns(), 1111111110.0);
}

TEST_F(TimestampTest, FetchAdd)
{
    GE::Timestamp timestamp{0.123456789};
    timestamp += 0.987654321;

    EXPECT_DOUBLE_EQ(timestamp.sec(), 1.11111111);
    EXPECT_DOUBLE_EQ(timestamp.ms(), 1111.11111);
    EXPECT_DOUBLE_EQ(timestamp.us(), 1111111.11);
    EXPECT_DOUBLE_EQ(timestamp.ns(), 1111111110.0);
}

TEST_F(TimestampTest, Subtraction)
{
    GE::Timestamp start{0.987654321};
    GE::Timestamp end{1.11111111};
    GE::Timestamp duration = end - start;

    EXPECT_DOUBLE_EQ(duration.sec(), 0.123456789);
    EXPECT_DOUBLE_EQ(duration.ms(), 123.456789);
    EXPECT_DOUBLE_EQ(duration.us(), 123456.789);
    EXPECT_DOUBLE_EQ(duration.ns(), 123456789.0);
}

TEST_F(TimestampTest, FetchSub)
{
    GE::Timestamp timestamp{1.11111111};
    timestamp -= 0.987654321;

    EXPECT_DOUBLE_EQ(timestamp.sec(), 0.123456789);
    EXPECT_DOUBLE_EQ(timestamp.ms(), 123.456789);
    EXPECT_DOUBLE_EQ(timestamp.us(), 123456.789);
    EXPECT_DOUBLE_EQ(timestamp.ns(), 123456789.0);
}

} // namespace
