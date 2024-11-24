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

#include <gmock/gmock.h>
#include <gtest/gtest.h>

using namespace GE::Script;
using namespace testing;

namespace {

class MethodTest: public Test
{
protected:
    void SetUp() override
    {
        ASSERT_TRUE(assembly.load(ASSEMBLY_PATH));

        klass = assembly.getClass("Ge.Tests", "ClassMethodsTest");
        ASSERT_TRUE(klass.isValid());

        object = assembly.getClass("Ge.Tests", "ObjectMethodsTest").newObject();
        ASSERT_TRUE(object.isValid());
    }

    static constexpr std::string_view ASSEMBLY_PATH = "./GeScriptTests.dll";

    Assembly assembly{ScriptingEngine::createAssembly()};
    Class klass;
    Object object;
};

TEST_F(MethodTest, DefaultObject)
{
    Method method;
    EXPECT_FALSE(method.isValid());
    EXPECT_FALSE(method.isInstance());
    EXPECT_EQ(method.paramCount(), 0);
    EXPECT_THAT(method.name(), IsEmpty());

    auto result = method();
    EXPECT_TRUE(result.hasError());
}

TEST_F(MethodTest, UnknownMethod)
{
    ASSERT_FALSE(klass.method("UnknownMethod").isValid());
    ASSERT_FALSE(object.method("UnknownMethod").isValid());
}

TEST_F(MethodTest, ExisttingMethod)
{
    ASSERT_TRUE(klass.method("ExistingMethod").isValid());
    ASSERT_TRUE(object.method("ExistingMethod").isValid());
}

TEST_F(MethodTest, ReturnBoolean)
{
    constexpr bool CLASS_VALUE{false};
    constexpr bool OBJECT_VALUE{true};

    auto class_value = klass.method("ReturnBoolean")().as<bool>();
    ASSERT_TRUE(class_value.has_value());
    EXPECT_EQ(class_value.value(), CLASS_VALUE);

    auto object_value = object.method("ReturnBoolean")().as<bool>();
    ASSERT_TRUE(object_value.has_value());
    EXPECT_EQ(object_value.value(), OBJECT_VALUE);
}

TEST_F(MethodTest, SetBoolean)
{
    constexpr bool CLASS_VALUE{true};
    constexpr bool OBJECT_VALUE{false};

    auto class_value = klass.method("SetBoolean")(CLASS_VALUE).as<bool>();
    ASSERT_TRUE(class_value.has_value());
    EXPECT_EQ(class_value.value(), CLASS_VALUE);

    auto object_value = object.method("SetBoolean")(OBJECT_VALUE).as<bool>();
    ASSERT_TRUE(object_value.has_value());
    EXPECT_EQ(object_value.value(), OBJECT_VALUE);
}

TEST_F(MethodTest, ReturnString)
{
    constexpr std::string_view CLASS_VALUE{"43"};
    constexpr std::string_view OBJECT_VALUE{"42"};

    auto class_value = klass.method("ReturnString")().as<std::string>();
    ASSERT_TRUE(class_value.has_value());
    EXPECT_EQ(class_value.value(), CLASS_VALUE);

    auto object_value = object.method("ReturnString")().as<std::string>();
    ASSERT_TRUE(object_value.has_value());
    EXPECT_EQ(object_value.value(), OBJECT_VALUE);
}

TEST_F(MethodTest, SetString)
{
    constexpr std::string_view CLASS_VALUE{"43"};
    constexpr std::string_view OBJECT_VALUE{"42"};

    auto class_value = klass.method("SetString")(CLASS_VALUE).as<std::string>();
    ASSERT_TRUE(class_value.has_value());
    EXPECT_EQ(class_value.value(), CLASS_VALUE);

    auto object_value = object.method("SetString")(OBJECT_VALUE).as<std::string>();
    ASSERT_TRUE(object_value.has_value());
    EXPECT_EQ(object_value.value(), OBJECT_VALUE);
}

template<typename T>
class BittableTypeTest: public MethodTest
{};

using BittableTypes =
    Types<int8_t, uint8_t, int16_t, uint16_t, int32_t, uint32_t, int64_t, uint64_t, char>;

TYPED_TEST_SUITE(BittableTypeTest, BittableTypes);

TYPED_TEST(BittableTypeTest, ReturnValue)
{
    using BittableType = TypeParam;

    constexpr BittableType CLASS_VALUE = 43;
    constexpr BittableType OBJECT_VALUE = 42;

    const std::string METHOD_NAME = "Return" + std::string{CLASS_TYPE_NAME<BittableType>};

    auto class_value = this->klass.method(METHOD_NAME)().template as<BittableType>();
    ASSERT_TRUE(class_value.has_value());
    EXPECT_EQ(class_value.value(), CLASS_VALUE);

    auto object_value = this->object.method(METHOD_NAME)().template as<BittableType>();
    ASSERT_TRUE(object_value.has_value());
    EXPECT_EQ(object_value.value(), OBJECT_VALUE);
}

TYPED_TEST(BittableTypeTest, SetValue)
{
    using BittableType = TypeParam;

    constexpr BittableType CLASS_VALUE = 43;
    constexpr BittableType OBJECT_VALUE = 42;

    const std::string METHOD_NAME = "Set" + std::string{CLASS_TYPE_NAME<BittableType>};

    auto class_value = this->klass.method(METHOD_NAME)(CLASS_VALUE).template as<BittableType>();
    ASSERT_TRUE(class_value.has_value());
    EXPECT_EQ(class_value.value(), CLASS_VALUE);

    auto object_value = this->object.method(METHOD_NAME)(OBJECT_VALUE).template as<BittableType>();
    ASSERT_TRUE(object_value.has_value());
    EXPECT_EQ(object_value.value(), OBJECT_VALUE);
}

TEST_F(MethodTest, paramCount)
{
    EXPECT_EQ(klass.method("zeroArgs").paramCount(), 0);
    EXPECT_EQ(klass.method("oneArg").paramCount(), 1);
    EXPECT_EQ(klass.method("twoArgs").paramCount(), 2);

    EXPECT_FALSE(klass.method("zeroArgs")().hasError());
    EXPECT_FALSE(klass.method("oneArg")(1).hasError());
    EXPECT_FALSE(klass.method("twoArgs")(1, 2).hasError());

    EXPECT_EQ(object.method("zeroArgs").paramCount(), 0);
    EXPECT_EQ(object.method("oneArg").paramCount(), 1);
    EXPECT_EQ(object.method("twoArgs").paramCount(), 2);

    EXPECT_FALSE(object.method("zeroArgs")().hasError());
    EXPECT_FALSE(object.method("oneArg")(1).hasError());
    EXPECT_FALSE(object.method("twoArgs")(1, 2).hasError());
}

TEST_F(MethodTest, invalidArgCount)
{
    auto zero_args_method = object.method("zeroArgs");
    auto one_arg_method = object.method("oneArg");
    auto two_args_method = object.method("twoArgs");

    ASSERT_TRUE(zero_args_method.isValid());
    ASSERT_TRUE(one_arg_method.isValid());
    ASSERT_TRUE(two_args_method.isValid());

    EXPECT_TRUE(zero_args_method(1, 2, 3).hasError());
    EXPECT_TRUE(one_arg_method(1, 2, 3).hasError());
    EXPECT_TRUE(two_args_method(1, 2, 3).hasError());
}

TEST_F(MethodTest, invalidArgTypes)
{
    auto one_arg_method = object.method("oneArg");
    auto two_args_method = object.method("twoArgs");

    ASSERT_TRUE(one_arg_method.isValid());
    ASSERT_TRUE(two_args_method.isValid());

    EXPECT_FALSE(one_arg_method(1).hasError());
    EXPECT_FALSE(two_args_method(1, 2).hasError());

    EXPECT_TRUE(one_arg_method("hello world").hasError());
    EXPECT_TRUE(two_args_method("hello world", "hello world").hasError());
}

TEST_F(MethodTest, instanceMethod)
{
    constexpr std::string_view INSTANCE_METHOD_VALUE = "ClassMethodsTest::instanceMethod";

    auto method = klass.method("instanceMethod");
    ASSERT_TRUE(method.isValid());
    EXPECT_TRUE(method.isInstance());

    auto result = method();
    EXPECT_TRUE(result.hasError());

    auto class_object = klass.newObject();
    auto object_method = class_object.method("instanceMethod");
    ASSERT_TRUE(object_method.isValid());
    EXPECT_TRUE(method.isInstance());

    auto value = object_method().as<std::string>();
    ASSERT_TRUE(value.has_value());
    EXPECT_EQ(value.value(), INSTANCE_METHOD_VALUE);
}

TEST_F(MethodTest, staticMethod)
{
    constexpr std::string_view CLASS_VALUE = "ClassMethodsTest::staticMethod";
    constexpr std::string_view OBJECT_VALUE = "ObjectMethodsTest::staticMethod";

    auto class_method_value = klass.method("staticMethod")().as<std::string>();
    ASSERT_TRUE(class_method_value.has_value());
    EXPECT_EQ(class_method_value.value(), CLASS_VALUE);

    auto method = object.method("staticMethod");
    ASSERT_TRUE(method.isValid());
    EXPECT_FALSE(method.isInstance());

    auto value = method().as<std::string>();
    ASSERT_TRUE(value.has_value());
    EXPECT_EQ(value.value(), OBJECT_VALUE);
}

TEST_F(MethodTest, implicitArgsConversion)
{
    constexpr int16_t FIRST_ARG{1};
    constexpr int32_t SECOND_ARG{3};
    constexpr double THIRD_ARG{5.0f};

    constexpr double ARGS_SUM{9.0};

    auto result = object.method("shortIntDouble")(FIRST_ARG, SECOND_ARG, THIRD_ARG);
    ASSERT_FALSE(result.hasError()) << result.errorMessage();

    auto value = result.as<double>();
    ASSERT_TRUE(value.has_value());
    EXPECT_DOUBLE_EQ(value.value(), ARGS_SUM);
}

} // namespace
