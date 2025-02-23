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

#include "tested_value.h"

#include "genesis/core/log.h"
#include "genesis/filesystem/file.h"
#include "genesis/filesystem/filepath.h"
#include "genesis/script/assembly.h"
#include "genesis/script/class.h"
#include "genesis/script/class_type.h"
#include "genesis/script/invoke_result.h"
#include "genesis/script/method.h"
#include "genesis/script/object.h"
#include "genesis/script/scripting_engine.h"

#include <gmock/gmock.h>
#include <gtest/gtest.h>

using namespace GE::Script;
using namespace GE::Tests;
using namespace testing;

namespace GE::Script {

using ScriptTypes = Types<int8_t, uint8_t, int16_t, uint16_t, int32_t, uint32_t, int64_t, uint64_t,
                          char, float, double, bool, std::string_view>;

using BittableTypes = Types<int8_t, uint8_t, int16_t, uint16_t, int32_t, uint32_t, int64_t,
                            uint64_t, char, float, double, bool>;

void PrintTo(const ClassType& type, std::ostream* os)
{
    *os << GE::toString(type);
}

} // namespace GE::Script

namespace {

class MethodTest: public Test
{
protected:
    void SetUp() override { ASSERT_TRUE(assembly.load(ASSEMBLY_PATH)); }

    static constexpr std::string_view ASSEMBLY_PATH{"./GeMethodTests.dll"};

    Assembly assembly{Domain::rootDomain()};
};

TEST_F(MethodTest, DefaultObject)
{
    Method method;
    EXPECT_FALSE(method.isValid());
    EXPECT_FALSE(method.isInstance());
    EXPECT_EQ(method.paramCount(), 0);
    EXPECT_THAT(method.paramTypes(), IsEmpty());
    EXPECT_THAT(method.name(), IsEmpty());
    EXPECT_FALSE(method.klass().isValid());

    auto result = method();
    EXPECT_TRUE(result.hasError());
}

class ClassMethodTest: public MethodTest
{
protected:
    void SetUp() override
    {
        MethodTest::SetUp();

        klass = assembly.createClass("Ge.Tests", "StaticClass");
        ASSERT_TRUE(klass.isValid());

        zero_args_method = klass.method("ZeroArgs");
        one_int_arg_method = klass.method("OneIntArg");
        two_int_args_method = klass.method("TwoIntArgs");
        int_string_bool_args_method = klass.method("IntStringBoolArgs");
        static_method = klass.method("StaticMethod");
        test_exception_method = klass.method("TestException");

        ASSERT_TRUE(zero_args_method.isValid());
        ASSERT_TRUE(one_int_arg_method.isValid());
        ASSERT_TRUE(two_int_args_method.isValid());
        ASSERT_TRUE(int_string_bool_args_method.isValid());
        ASSERT_TRUE(static_method.isValid());
        ASSERT_TRUE(test_exception_method.isValid());
    }

    Class klass;
    Method zero_args_method;
    Method one_int_arg_method;
    Method two_int_args_method;
    Method int_string_bool_args_method;
    Method static_method;
    Method test_exception_method;
};

TEST_F(ClassMethodTest, UnknownMethod)
{
    ASSERT_FALSE(klass.method("UnknownMethod").isValid());
}

TEST_F(ClassMethodTest, ExisttingMethod)
{
    ASSERT_TRUE(klass.method("ExistingMethod").isValid());
}

TEST_F(ClassMethodTest, ParamCount)
{
    EXPECT_EQ(zero_args_method.paramCount(), 0);
    EXPECT_EQ(one_int_arg_method.paramCount(), 1);
    EXPECT_EQ(two_int_args_method.paramCount(), 2);
    EXPECT_EQ(int_string_bool_args_method.paramCount(), 3);
}

TEST_F(ClassMethodTest, InvalidArgCount)
{
    EXPECT_FALSE(zero_args_method().hasError());
    EXPECT_TRUE(zero_args_method(1).hasError());

    EXPECT_TRUE(one_int_arg_method().hasError());
    EXPECT_FALSE(one_int_arg_method(1).hasError());
    EXPECT_TRUE(one_int_arg_method(1, 2).hasError());

    EXPECT_TRUE(two_int_args_method(1).hasError());
    EXPECT_FALSE(two_int_args_method(1, 2).hasError());
    EXPECT_TRUE(two_int_args_method(1, 2, 3).hasError());
}

TEST_F(ClassMethodTest, InvalidArgTypes)
{
    EXPECT_FALSE(one_int_arg_method(1).hasError());
    EXPECT_TRUE(one_int_arg_method("hello world").hasError());

    EXPECT_FALSE(two_int_args_method(1, 2).hasError());
    EXPECT_TRUE(two_int_args_method(1, "hello world").hasError());
    EXPECT_TRUE(two_int_args_method("hello world", 1).hasError());
    EXPECT_TRUE(two_int_args_method("hello world", "hello world").hasError());

    EXPECT_FALSE(int_string_bool_args_method(1, "hello world", true).hasError());
    EXPECT_TRUE(int_string_bool_args_method(true, "hello world", true).hasError());
    EXPECT_TRUE(int_string_bool_args_method(1, true, true).hasError());
    EXPECT_TRUE(int_string_bool_args_method(1, "hello world", 1).hasError());
}

TEST_F(ClassMethodTest, ParamTypes)
{
    EXPECT_THAT(zero_args_method.paramTypes(), IsEmpty());
    EXPECT_THAT(one_int_arg_method.paramTypes(), ElementsAre(ClassType::I4));
    EXPECT_THAT(two_int_args_method.paramTypes(), ElementsAre(ClassType::I4, ClassType::I4));
    EXPECT_THAT(int_string_bool_args_method.paramTypes(),
                ElementsAre(ClassType::I4, ClassType::STRING, ClassType::BOOLEAN));
}

TEST_F(ClassMethodTest, MethodName)
{
    EXPECT_EQ(zero_args_method.name(), "ZeroArgs");
    EXPECT_EQ(one_int_arg_method.name(), "OneIntArg");
    EXPECT_EQ(two_int_args_method.name(), "TwoIntArgs");
    EXPECT_EQ(int_string_bool_args_method.name(), "IntStringBoolArgs");
    EXPECT_EQ(static_method.name(), "StaticMethod");
    EXPECT_EQ(test_exception_method.name(), "TestException");
}

TEST_F(ClassMethodTest, ClassIsValid)
{
    EXPECT_TRUE(zero_args_method.klass().isValid());
    EXPECT_TRUE(one_int_arg_method.klass().isValid());
    EXPECT_TRUE(two_int_args_method.klass().isValid());
    EXPECT_TRUE(int_string_bool_args_method.klass().isValid());
    EXPECT_TRUE(static_method.klass().isValid());
    EXPECT_TRUE(test_exception_method.klass().isValid());
}

TEST_F(ClassMethodTest, StaticMethod)
{
    constexpr std::string_view RETURN_VALUE = "ClassMethodsTest::staticMethod";

    EXPECT_FALSE(static_method.isInstance());
    EXPECT_EQ(static_method().as<std::string>(), RETURN_VALUE);
}

TEST_F(ClassMethodTest, Exception)
{
    constexpr bool THROW_EXCEPTION{true};
    constexpr std::string_view MESSAGE = "I'm an exception";

    auto result_with_exception = test_exception_method(THROW_EXCEPTION, MESSAGE);
    ASSERT_TRUE(result_with_exception.hasError());
    EXPECT_THAT(result_with_exception.errorMessage(), HasSubstr(MESSAGE));

    auto result_with_message = test_exception_method(!THROW_EXCEPTION, MESSAGE);
    ASSERT_FALSE(result_with_message.hasError());
    EXPECT_EQ(result_with_message.as<std::string>(), MESSAGE);
}

class ObjectMethodTest: public MethodTest
{
protected:
    void SetUp() override
    {
        MethodTest::SetUp();

        object = assembly.createClass("Ge.Tests", "ObjectMethodsTest").newObject();
        ASSERT_TRUE(object.isValid());

        zero_args_method = object.method("ZeroArgs");
        one_int_arg_method = object.method("OneIntArg");
        two_int_args_method = object.method("TwoIntArgs");
        int_string_bool_args_method = object.method("IntStringBoolArgs");
        static_method = object.method("StaticMethod");
        instance_method = object.method("InstanceMethod");
        test_exception_method = object.method("TestException");

        ASSERT_TRUE(zero_args_method.isValid());
        ASSERT_TRUE(one_int_arg_method.isValid());
        ASSERT_TRUE(two_int_args_method.isValid());
        ASSERT_TRUE(int_string_bool_args_method.isValid());
        ASSERT_TRUE(static_method.isValid());
        ASSERT_TRUE(instance_method.isValid());
        ASSERT_TRUE(test_exception_method.isValid());
    }

    Object object;
    Method zero_args_method;
    Method one_int_arg_method;
    Method two_int_args_method;
    Method int_string_bool_args_method;
    Method static_method;
    Method instance_method;
    Method test_exception_method;
};

TEST_F(ObjectMethodTest, UnknownMethod)
{
    ASSERT_FALSE(object.method("UnknownMethod").isValid());
}

TEST_F(ObjectMethodTest, ExisttingMethod)
{
    ASSERT_TRUE(object.method("ExistingMethod").isValid());
}

TEST_F(ObjectMethodTest, ParamCount)
{
    EXPECT_EQ(zero_args_method.paramCount(), 0);
    EXPECT_EQ(one_int_arg_method.paramCount(), 1);
    EXPECT_EQ(two_int_args_method.paramCount(), 2);
    EXPECT_EQ(int_string_bool_args_method.paramCount(), 3);
}

TEST_F(ObjectMethodTest, InvalidArgCount)
{
    EXPECT_FALSE(zero_args_method().hasError());
    EXPECT_TRUE(zero_args_method(1).hasError());

    EXPECT_TRUE(one_int_arg_method().hasError());
    EXPECT_FALSE(one_int_arg_method(1).hasError());
    EXPECT_TRUE(one_int_arg_method(1, 2).hasError());

    EXPECT_TRUE(two_int_args_method(1).hasError());
    EXPECT_FALSE(two_int_args_method(1, 2).hasError());
    EXPECT_TRUE(two_int_args_method(1, 2, 3).hasError());
}

TEST_F(ObjectMethodTest, InvalidArgTypes)
{
    EXPECT_FALSE(one_int_arg_method(1).hasError());
    EXPECT_TRUE(one_int_arg_method("hello world").hasError());

    EXPECT_FALSE(two_int_args_method(1, 2).hasError());
    EXPECT_TRUE(two_int_args_method(1, "hello world").hasError());
    EXPECT_TRUE(two_int_args_method("hello world", 1).hasError());
    EXPECT_TRUE(two_int_args_method("hello world", "hello world").hasError());

    EXPECT_FALSE(int_string_bool_args_method(1, "hello world", true).hasError());
    EXPECT_TRUE(int_string_bool_args_method(true, "hello world", true).hasError());
    EXPECT_TRUE(int_string_bool_args_method(1, true, true).hasError());
    EXPECT_TRUE(int_string_bool_args_method(1, "hello world", 1).hasError());
}

TEST_F(ObjectMethodTest, ParamTypes)
{
    EXPECT_THAT(zero_args_method.paramTypes(), IsEmpty());
    EXPECT_THAT(one_int_arg_method.paramTypes(), ElementsAre(ClassType::I4));
    EXPECT_THAT(two_int_args_method.paramTypes(), ElementsAre(ClassType::I4, ClassType::I4));
    EXPECT_THAT(int_string_bool_args_method.paramTypes(),
                ElementsAre(ClassType::I4, ClassType::STRING, ClassType::BOOLEAN));
}

TEST_F(ObjectMethodTest, MethodName)
{
    EXPECT_EQ(zero_args_method.name(), "ZeroArgs");
    EXPECT_EQ(one_int_arg_method.name(), "OneIntArg");
    EXPECT_EQ(two_int_args_method.name(), "TwoIntArgs");
    EXPECT_EQ(int_string_bool_args_method.name(), "IntStringBoolArgs");
    EXPECT_EQ(static_method.name(), "StaticMethod");
    EXPECT_EQ(test_exception_method.name(), "TestException");
}

TEST_F(ObjectMethodTest, ClassIsValid)
{
    EXPECT_TRUE(zero_args_method.klass().isValid());
    EXPECT_TRUE(one_int_arg_method.klass().isValid());
    EXPECT_TRUE(two_int_args_method.klass().isValid());
    EXPECT_TRUE(int_string_bool_args_method.klass().isValid());
    EXPECT_TRUE(static_method.klass().isValid());
    EXPECT_TRUE(test_exception_method.klass().isValid());
}

TEST_F(ObjectMethodTest, StaticMethod)
{
    constexpr std::string_view RETURN_VALUE = "ObjectMethodsTest::staticMethod";

    EXPECT_FALSE(static_method.isInstance());
    EXPECT_EQ(static_method().as<std::string>(), RETURN_VALUE);
}

TEST_F(ObjectMethodTest, InstanceMethod)
{
    constexpr std::string_view RETURN_VALUE = "ObjectMethodsTest::instanceMethod";

    EXPECT_TRUE(instance_method.isInstance());
    EXPECT_EQ(instance_method().as<std::string>(), RETURN_VALUE);
}

TEST_F(ObjectMethodTest, Exception)
{
    constexpr bool THROW_EXCEPTION{true};
    constexpr std::string_view MESSAGE = "I'm an exception";

    auto result_with_exception = test_exception_method(THROW_EXCEPTION, MESSAGE);
    ASSERT_TRUE(result_with_exception.hasError());
    EXPECT_THAT(result_with_exception.errorMessage(), HasSubstr(MESSAGE));

    auto result_with_message = test_exception_method(!THROW_EXCEPTION, MESSAGE);
    ASSERT_FALSE(result_with_message.hasError());
    EXPECT_EQ(result_with_message.as<std::string>(), MESSAGE);
}

template<typename T>
class ClassTypedTest: public ClassMethodTest
{};

TYPED_TEST_SUITE(ClassTypedTest, ScriptTypes);

TYPED_TEST(ClassTypedTest, ReturnValue)
{
    const std::string METHOD_NAME = "Return" + std::string{CLASS_TYPE_NAME<TypeParam>};

    auto result = this->klass.method(METHOD_NAME)();
    ASSERT_TRUE(result.template is<TypeParam>()) << GE::toString(result.type());
    EXPECT_EQ(result.template as<TypeParam>(), TESTED_VALUE<TypeParam>);
}

TYPED_TEST(ClassTypedTest, SetValue)
{
    const std::string METHOD_NAME = "Set" + std::string{CLASS_TYPE_NAME<TypeParam>};

    auto object_value = this->klass.method(METHOD_NAME)(TESTED_VALUE<TypeParam>);
    ASSERT_TRUE(object_value.template is<TypeParam>()) << GE::toString(object_value.type());
    EXPECT_EQ(object_value.template as<TypeParam>(), TESTED_VALUE<TypeParam>);
}

template<typename T>
class ObjectTypedTest: public ObjectMethodTest
{};

TYPED_TEST_SUITE(ObjectTypedTest, ScriptTypes);

TYPED_TEST(ObjectTypedTest, ReturnValue)
{
    const std::string METHOD_NAME = "Return" + std::string{CLASS_TYPE_NAME<TypeParam>};

    auto result = this->object.method(METHOD_NAME)();
    ASSERT_TRUE(result.template is<TypeParam>()) << GE::toString(result.type());
    EXPECT_EQ(result.template as<TypeParam>(), TESTED_VALUE<TypeParam>);
}

TYPED_TEST(ObjectTypedTest, SetValue)
{
    const std::string METHOD_NAME = "Set" + std::string{CLASS_TYPE_NAME<TypeParam>};

    auto object_value = this->object.method(METHOD_NAME)(TESTED_VALUE<TypeParam>);
    ASSERT_TRUE(object_value.template is<TypeParam>()) << GE::toString(object_value.type());
    EXPECT_EQ(object_value.template as<TypeParam>(), TESTED_VALUE<TypeParam>);
}

template<typename T>
class UnmanagedClassMethodTest: public MethodTest
{
protected:
    using GetValueMock = MockFunction<T()>;
    using SetValueMock = MockFunction<void(T)>;

    UnmanagedClassMethodTest()
    {
        get_value_mock = &get_value;
        get_alt_value_mock = &get_alt_value;
        set_value_mock = &set_value;
    }

    ~UnmanagedClassMethodTest()
    {
        get_value_mock = nullptr;
        get_alt_value_mock = nullptr;
        set_value_mock = nullptr;
    }

    void SetUp() override
    {
        MethodTest::SetUp();

        klass = assembly.createClass("Ge.Tests", "UnmanagedClassMethods");
        ASSERT_TRUE(klass.isValid());

        const std::string SET_METHOD_NAME =
            "Ge.Tests.InternalCalls::Set" + std::string{CLASS_TYPE_NAME<T>};
        ScriptingEngine::registerInternalCall(SET_METHOD_NAME,
                                              &UnmanagedClassMethodTest::setUnmanagedValue);

        const std::string GET_METHOD_NAME =
            "Ge.Tests.InternalCalls::Get" + std::string{CLASS_TYPE_NAME<T>};
        ScriptingEngine::registerInternalCall(GET_METHOD_NAME,
                                              &UnmanagedClassMethodTest::getUnmanagedValue);
    }

    void TearDown() override
    {
        Mock::VerifyAndClearExpectations(&get_value);
        Mock::VerifyAndClearExpectations(&get_alt_value);
        Mock::VerifyAndClearExpectations(&set_value);
    }

    static T getUnmanagedValue() { return get_value_mock->Call(); }
    static T getUnmanagedAlternativeValue() { return get_alt_value_mock->Call(); }
    static void setUnmanagedValue(T value) { set_value_mock->Call(value); }

    Class klass;
    GetValueMock get_value;
    GetValueMock get_alt_value;
    SetValueMock set_value;

private:
    inline static GetValueMock* get_value_mock{nullptr};
    inline static GetValueMock* get_alt_value_mock{nullptr};
    inline static SetValueMock* set_value_mock{nullptr};
};

TYPED_TEST_SUITE(UnmanagedClassMethodTest, BittableTypes);

TYPED_TEST(UnmanagedClassMethodTest, GetUnmanagedValue)
{
    EXPECT_CALL(this->get_value, Call()).WillOnce(Return(TESTED_VALUE<TypeParam>));

    const std::string METHOD_NAME = "Get" + std::string{CLASS_TYPE_NAME<TypeParam>};
    auto result = this->klass.method(METHOD_NAME)();

    ASSERT_TRUE(result.template is<TypeParam>()) << GE::toString(result.type());
    EXPECT_EQ(result.template as<TypeParam>(), TESTED_VALUE<TypeParam>);
}

TYPED_TEST(UnmanagedClassMethodTest, GetUnmanagedValue_Exception)
{
    EXPECT_CALL(this->get_value, Call()).WillOnce(Throw(std::runtime_error("error")));

    const std::string METHOD_NAME = "Get" + std::string{CLASS_TYPE_NAME<TypeParam>};
    auto result = this->klass.method(METHOD_NAME)();

    ASSERT_TRUE(result.template is<TypeParam>()) << GE::toString(result.type());
    EXPECT_EQ(result.template as<TypeParam>(), TESTED_VALUE<TypeParam>);
}

TYPED_TEST(UnmanagedClassMethodTest, DISABLED_OverrideInternalCall)
{
    // Override "InternalCalls.Get<Type>" method

    const std::string INTERNAL_CALLS_GET =
        "Ge.Tests.InternalCalls::Get" + std::string{CLASS_TYPE_NAME<TypeParam>};
    ScriptingEngine::registerInternalCall(
        INTERNAL_CALLS_GET, &UnmanagedClassMethodTest<TypeParam>::getUnmanagedAlternativeValue);

    // Test the method

    EXPECT_CALL(this->get_value, Call()).Times(0);
    EXPECT_CALL(this->get_alt_value, Call()).WillOnce(Return(TESTED_VALUE<TypeParam>));

    const std::string METHOD_NAME = "Get" + std::string{CLASS_TYPE_NAME<TypeParam>};
    auto result = this->klass.method(METHOD_NAME)();

    ASSERT_TRUE(result.template is<TypeParam>()) << GE::toString(result.type());
    EXPECT_EQ(result.template as<TypeParam>(), TESTED_VALUE<TypeParam>);
}

TYPED_TEST(UnmanagedClassMethodTest, DISABLED_SetUnmanagedValue)
{
    const std::string METHOD_NAME = "Set" + std::string{CLASS_TYPE_NAME<TypeParam>};

    auto result = this->klass.method(METHOD_NAME)(TESTED_VALUE<TypeParam>);
    ASSERT_FALSE(result.hasError()) << result.errorMessage();
}

template<typename T>
class DISABLED_UnmanagedObjectMethodTest: public MethodTest
{
protected:
    static void SetUpTestCase()
    {
        const std::string GET_METHOD_NAME =
            "Ge.Tests.InternalCalls::Get" + std::string{CLASS_TYPE_NAME<T>};
        const std::string SET_METHOD_NAME =
            "Ge.Tests.InternalCalls::Set" + std::string{CLASS_TYPE_NAME<T>};

        ScriptingEngine::registerInternalCall(
            GET_METHOD_NAME, &DISABLED_UnmanagedObjectMethodTest::getUnmanagedValue);
        ScriptingEngine::registerInternalCall(
            SET_METHOD_NAME, &DISABLED_UnmanagedObjectMethodTest::setUnmanagedValue);
    }

    void SetUp() override
    {
        MethodTest::SetUp();

        object = assembly.createClass("Ge.Tests", "UnmanagedObjectMethods").newObject();
        ASSERT_TRUE(object.isValid());
    }

    static T getUnmanagedValue() { return EXPECTED_VALUE; }
    static void setUnmanagedValue(T value) { last_set_value = value; }

    inline static const T EXPECTED_VALUE{TESTED_VALUE<T>};

    inline static std::optional<T> last_set_value;

    Object object;
};

TYPED_TEST_SUITE(DISABLED_UnmanagedObjectMethodTest, BittableTypes);

TYPED_TEST(DISABLED_UnmanagedObjectMethodTest, GetUnmanagedValue)
{
    const std::string METHOD_NAME = "Get" + std::string{CLASS_TYPE_NAME<TypeParam>};

    auto result = this->object.method(METHOD_NAME)();
    ASSERT_TRUE(result.template is<TypeParam>()) << GE::toString(result.type());
    EXPECT_EQ(result.template as<TypeParam>(), this->EXPECTED_VALUE);
}

TYPED_TEST(DISABLED_UnmanagedObjectMethodTest, SetUnmanagedValue)
{
    const std::string METHOD_NAME = "Set" + std::string{CLASS_TYPE_NAME<TypeParam>};

    auto result = this->object.method(METHOD_NAME)(this->EXPECTED_VALUE);
    ASSERT_FALSE(result.hasError()) << result.errorMessage();
    EXPECT_EQ(this->last_set_value, this->EXPECTED_VALUE);
}

} // namespace
