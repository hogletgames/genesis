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

#include "genesis/script/assembly.h"
#include "genesis/script/class.h"
#include "genesis/script/domain.h"
#include "genesis/script/method.h"

#include <gmock/gmock.h>
#include <gtest/gtest.h>

using namespace GE::Script;
using namespace testing;

namespace {

class AssemblyTest: public Test
{
protected:
    static constexpr std::string_view DOMAIN_NAME = "AssemblyTest";
    static constexpr std::string_view FIRST_ASSEMBLY_PATH{"./GeFirstAssembly.dll"};
    static constexpr std::string_view SECOND_ASSEMBLY_PATH{"./GeSecondAssemblyTest.dll"};

    Assembly assembly{Domain::rootDomain()};
};

TEST_F(AssemblyTest, DefaultAssembly)
{
    EXPECT_FALSE(assembly.isValid());
}

TEST_F(AssemblyTest, LoadAssembly)
{
    ASSERT_TRUE(assembly.load(FIRST_ASSEMBLY_PATH));
    EXPECT_TRUE(assembly.isValid());

    Assembly second_assembly{Domain::rootDomain()};
    ASSERT_TRUE(second_assembly.load(SECOND_ASSEMBLY_PATH));
    EXPECT_TRUE(second_assembly.isValid());
}

class OneAssemblyTest: public AssemblyTest
{
protected:
    void SetUp() override
    {
        AssemblyTest::SetUp();

        ASSERT_TRUE(assembly.load(FIRST_ASSEMBLY_PATH));
        ASSERT_TRUE(assembly.isValid());
    }
};

// TEST_F(OneAssemblyTest, CreateClass)
// {
//     constexpr int EXPECTED_VALUE{42};
//
//     auto klass = assembly.createClass("Ge.Tests", "ClassMethodsTest");
//     ASSERT_TRUE(klass.isValid());
//     EXPECT_EQ(EXPECTED_VALUE, klass.method("SetInt32")(EXPECTED_VALUE).as<int>());
// }
//
// TEST_F(OneAssemblyTest, LoadSecondAssembly)
// {
//     Assembly second_assembly{ScriptingEngine::createAssembly()};
//     ASSERT_TRUE(second_assembly.load(SECOND_ASSEMBLY_PATH));
//     EXPECT_TRUE(second_assembly.isValid());
// }
//
// TEST_F(OneAssemblyTest, ReloadAssembly)
// {
//     constexpr std::string_view EXPECTED_VALUE{"Hello World!"};
//
//     ASSERT_TRUE(assembly.load(SECOND_ASSEMBLY_PATH));
//     EXPECT_TRUE(assembly.isValid());
//
//     auto klass = assembly.createClass("Ge.SecondAssemblyTest", "TestClass");
//     ASSERT_TRUE(klass.isValid());
//     EXPECT_EQ(EXPECTED_VALUE,
//     klass.method("ReturnStringValue")(EXPECTED_VALUE).as<std::string>());
// }
//
// class TwoAssembliesTest: public OneAssemblyTest
// {
// protected:
//     void SetUp() override
//     {
//         OneAssemblyTest::SetUp();
//
//         ASSERT_TRUE(second_assembly.load(SECOND_ASSEMBLY_PATH));
//         ASSERT_TRUE(second_assembly.isValid());
//     }
//
//     Assembly second_assembly{ScriptingEngine::createAssembly()};
// };
//
// TEST_F(TwoAssembliesTest, CloseAssemblies)
// {
//     assembly.close();
//     EXPECT_FALSE(assembly.isValid());
//
//     second_assembly.close();
//     EXPECT_FALSE(second_assembly.isValid());
// }

} // namespace
