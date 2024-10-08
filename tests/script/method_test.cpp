/*
 * BSD 3-Clause License
 *
 * Copyright (c) 2024, Dmitry Shilnenkov
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
#include "genesis/filesystem/file.h"
#include "genesis/filesystem/filepath.h"
#include "genesis/script/assembly.h"
#include "genesis/script/class.h"
#include "genesis/script/invoke_result.h"
#include "genesis/script/method.h"
#include "genesis/script/object.h"
#include "genesis/script/scripting_engine.h"

#include <gtest/gtest.h>

using namespace GE::Script;
using namespace testing;
using namespace std::string_view_literals;

namespace {

class MethodTest: public Test
{
protected:
    void SetUp() override
    {
        ASSERT_TRUE(assembly.load(ASSEMBLY_PATH));

        object = assembly.getClass("Ge.Tests", "MethodTest").newObject();
        ASSERT_TRUE(object.isValid());
    }

    static constexpr std::string_view ASSEMBLY_PATH = "./GeScriptTests.dll";

    Assembly assembly{ScriptingEngine::createAssembly()};
    Object object;
};

TEST_F(MethodTest, UnknownMethod)
{
    auto method = object.method("UnknownMethod");
    ASSERT_FALSE(method.isValid());
}

// TEST_F(MethodTest, ExsitingMethod)
// {
//     auto method = object.method("ExistingMethod");
//     ASSERT_TRUE(method.isValid());
// }
//
// TEST_F(MethodTest, ReturnBoolean)
// {
//     constexpr bool EXPECTED_VALUE{true};
//
//     auto result = object.method("ReturnBoolean")();
//     ASSERT_TRUE(result) << result.errorMessage();
//
//     auto value = result.as<bool>();
//     ASSERT_TRUE(value.has_value());
//     EXPECT_EQ(value.value(), EXPECTED_VALUE);
// }
//
// TEST_F(MethodTest, SetBoolean)
// {
//     constexpr bool VALUE{true};
//
//     auto result = object.method("SetBoolean")(VALUE);
//     ASSERT_TRUE(result) << result.errorMessage();
//
//     auto value = result.as<bool>();
//     ASSERT_TRUE(value.has_value());
//     EXPECT_EQ(value.value(), VALUE);
// }
//
// TEST_F(MethodTest, ReturnString)
// {
//     constexpr std::string_view EXPECTED_VALUE{"42"};
//
//     auto result = object.method("ReturnString")();
//     ASSERT_TRUE(result) << result.errorMessage();
//
//     auto value = result.as<std::string>();
//     ASSERT_TRUE(value.has_value());
//     EXPECT_EQ(value.value(), EXPECTED_VALUE);
// }
//
// TEST_F(MethodTest, SetString)
// {
//     constexpr std::string_view VALUE{"42"};
//
//     auto result = object.method("SetString")(VALUE);
//     ASSERT_TRUE(result) << result.errorMessage();
//
//     auto value = result.as<std::string>();
//     ASSERT_TRUE(value.has_value());
//     EXPECT_EQ(value.value(), VALUE);
// }
//
// template<typename T>
// class BittableTypeTest: public MethodTest
// {};
//
// using BittableTypes =
//     Types<int8_t, uint8_t, int16_t, uint16_t, int32_t, uint32_t, int64_t, uint64_t, char>;
//
// TYPED_TEST_SUITE(BittableTypeTest, BittableTypes);
//
// TYPED_TEST(BittableTypeTest, ReturnValue)
// {
//     using BittableType = TypeParam;
//
//     constexpr BittableType EXPECTED_VALUE = 42;
//     const std::string METHOD_NAME = "Return" + std::string{CLASS_TYPE_NAME<BittableType>};
//
//     auto method = this->object.method(METHOD_NAME);
//     ASSERT_TRUE(method.isValid()) << "Method name='" << METHOD_NAME << "'";
//
//     auto result = method();
//     ASSERT_TRUE(result) << "Exception message=" << result.errorMessage();
//
//     auto value = result.template as<BittableType>();
//     ASSERT_TRUE(value.has_value());
//     EXPECT_EQ(value.value(), EXPECTED_VALUE);
// }
//
// TYPED_TEST(BittableTypeTest, SetValue)
// {
//     using BittableType = TypeParam;
//
//     constexpr BittableType VALUE = 42;
//     const std::string METHOD_NAME = "Set" + std::string{CLASS_TYPE_NAME<BittableType>};
//
//     auto method = this->object.method(METHOD_NAME);
//     ASSERT_TRUE(method.isValid()) << "Method name: '" << METHOD_NAME << "'";
//
//     auto result = method(VALUE);
//     ASSERT_TRUE(result) << "Exception message: '" << result.errorMessage() << "'";
//
//     auto value = result.template as<BittableType>();
//     ASSERT_TRUE(value.has_value());
//     EXPECT_EQ(value.value(), VALUE);
// }

} // namespace
