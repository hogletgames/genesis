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
#include "genesis/script/object.h"

#include <gmock/gmock.h>
#include <gtest/gtest.h>

using namespace testing;
using namespace GE::Script;

namespace {

class DomainTest: public Test
{
protected:
    void SetUp() override { ASSERT_TRUE(domain.isValid()); }

    static constexpr std::string_view DOMAIN_NAME = "DomainTest";
    static constexpr std::string_view SECOND_DOMAIN_NAME = "SecondDomainTest";

    static constexpr std::string_view FIRST_ASSEMBLY_PATH = "./GeFirstAssembly.dll";
    static constexpr std::string_view SECOND_ASSEMBLY_PATH = "./GeSecondAssemblyTest.dll";

    static constexpr std::string_view FIRST_ASSEMBLY_NAMESPACE = "Ge.FirstAssemblyTest";
    static constexpr std::string_view FIRST_CLASS_NAME = "FirstTestClass";

    static constexpr std::string_view SECOND_ASSEMBLY_NAMESPACE = "Ge.SecondAssemblyTest";
    static constexpr std::string_view SECOND_CLASS_NAME = "SecondTestClass";

    Domain domain{DOMAIN_NAME};
};

TEST_F(DomainTest, DefaultObject)
{
    EXPECT_TRUE(domain.isValid());
    EXPECT_TRUE(domain.isCurrent());
}

TEST_F(DomainTest, NewNonCurrentDomain)
{
    Domain non_current_domain{"NotCurrentDomain", Domain::CREATION_POLICY_DO_NOT_SET_AS_CURRENT};
    EXPECT_TRUE(non_current_domain.isValid());
    EXPECT_FALSE(non_current_domain.isCurrent());

    EXPECT_TRUE(domain.isCurrent());
}

TEST_F(DomainTest, NewCurrentDomain)
{
    Domain non_current_domain{"NotCurrentDomain", Domain::CREATION_POLICY_SET_AS_CURRENT};
    EXPECT_TRUE(non_current_domain.isValid());
    EXPECT_TRUE(non_current_domain.isCurrent());

    EXPECT_FALSE(domain.isCurrent());
}

TEST_F(DomainTest, RecreateDomainAndSetAsCurrent)
{
    ASSERT_TRUE(domain.recreate(DOMAIN_NAME));
    EXPECT_TRUE(domain.isValid());
    EXPECT_TRUE(domain.isCurrent());
    EXPECT_FALSE(Domain::rootDomain().isCurrent());
}

TEST_F(DomainTest, RecreateDomainAndDoNotSetAsCurrent)
{
    ASSERT_TRUE(domain.recreate(DOMAIN_NAME, Domain::CREATION_POLICY_DO_NOT_SET_AS_CURRENT));
    EXPECT_TRUE(domain.isValid());
    EXPECT_FALSE(domain.isCurrent());
    EXPECT_TRUE(Domain::rootDomain().isCurrent());
}

TEST_F(DomainTest, RecreateWithAnotherDomainName)
{
    ASSERT_TRUE(domain.recreate(SECOND_DOMAIN_NAME));
    EXPECT_TRUE(domain.isValid());
}

TEST_F(DomainTest, LoadAssembly)
{
    Assembly assembly{domain};
    ASSERT_TRUE(assembly.load(FIRST_ASSEMBLY_PATH));
}

class DomainWithAssemblyTest: public DomainTest
{
protected:
    void SetUp() override
    {
        DomainTest::SetUp();

        ASSERT_TRUE(assembly.load(FIRST_ASSEMBLY_PATH));
    }

    Assembly assembly{domain};
};

TEST_F(DomainWithAssemblyTest, InvokeObjectMethod)
{
    constexpr int EXPECTED_VALUE{42};

    auto first_class_object =
        assembly.createClass(FIRST_ASSEMBLY_NAMESPACE, FIRST_CLASS_NAME).newObject();
    ASSERT_TRUE(first_class_object.isValid());
    ASSERT_EQ(first_class_object.method("ReturnInt")(EXPECTED_VALUE).as<int>(), EXPECTED_VALUE);
}

TEST_F(DomainWithAssemblyTest, RecreateDomainAndReloadAssembly)
{
    constexpr int EXPECTED_VALUE{42};

    // Recreate the domain

    ASSERT_TRUE(domain.recreate(DOMAIN_NAME));
    EXPECT_TRUE(domain.isCurrent());

    // Load the assembly

    Assembly same_assembly{domain};
    ASSERT_TRUE(same_assembly.load(FIRST_ASSEMBLY_PATH));
    ASSERT_TRUE(same_assembly.isValid());

    // Invoke the object method

    auto first_class_object =
        same_assembly.createClass(FIRST_ASSEMBLY_NAMESPACE, FIRST_CLASS_NAME).newObject();
    ASSERT_TRUE(first_class_object.isValid());
    ASSERT_EQ(first_class_object.method("ReturnInt")(EXPECTED_VALUE).as<int>(), EXPECTED_VALUE);
}

TEST_F(DomainWithAssemblyTest, RecreateDomainAndLoadAnotherAssembly)
{
    constexpr std::string_view EXPECTED_VALUE{"Hello World!"};

    // Recreate the domain

    ASSERT_TRUE(domain.recreate(DOMAIN_NAME));
    EXPECT_TRUE(domain.isCurrent());

    // Load another assembly

    Assembly another_assembly{domain};
    ASSERT_TRUE(another_assembly.load(SECOND_ASSEMBLY_PATH));
    ASSERT_TRUE(another_assembly.isValid());

    // Invoke the object method

    auto second_class_object =
        another_assembly.createClass(SECOND_ASSEMBLY_NAMESPACE, SECOND_CLASS_NAME).newObject();
    ASSERT_TRUE(second_class_object.isValid());
    ASSERT_EQ(second_class_object.method("ReturnString")(EXPECTED_VALUE).as<std::string>(),
              EXPECTED_VALUE);
}

TEST_F(DomainWithAssemblyTest, LoadSecondAssembly)
{
    constexpr int EXPECTED_FIRST_OBJECT_VALUE{42};
    constexpr std::string_view EXPECTED_SECOND_OBJECT_VALUE{"Hello World!"};

    // Load the second assembly

    Assembly second_assembly{domain};
    ASSERT_TRUE(second_assembly.load(SECOND_ASSEMBLY_PATH));

    // Invoke a method of an object from the first assembly

    auto first_class_object =
        assembly.createClass(FIRST_ASSEMBLY_NAMESPACE, FIRST_CLASS_NAME).newObject();
    ASSERT_TRUE(first_class_object.isValid());
    ASSERT_EQ(first_class_object.method("ReturnInt")(EXPECTED_FIRST_OBJECT_VALUE).as<int>(),
              EXPECTED_FIRST_OBJECT_VALUE);

    // Invoke a method of an object from the second assembly

    auto second_class_object =
        second_assembly.createClass(SECOND_ASSEMBLY_NAMESPACE, SECOND_CLASS_NAME).newObject();
    ASSERT_TRUE(second_class_object.isValid());
    ASSERT_EQ(
        second_class_object.method("ReturnString")(EXPECTED_SECOND_OBJECT_VALUE).as<std::string>(),
        EXPECTED_SECOND_OBJECT_VALUE);
}

class DomainWithDifferentAssembliesTest: public DomainWithAssemblyTest
{
protected:
    void SetUp() override
    {
        DomainWithAssemblyTest::SetUp();

        ASSERT_TRUE(second_domain.isValid());
        ASSERT_TRUE(second_assembly.load(SECOND_ASSEMBLY_PATH));
    }

    Domain second_domain{SECOND_DOMAIN_NAME};
    Assembly second_assembly{second_domain};
};

TEST_F(DomainWithDifferentAssembliesTest, CreteObjects)
{
    constexpr int EXPECTED_FIRST_OBJECT_VALUE{42};
    constexpr std::string_view EXPECTED_SECOND_OBJECT_VALUE{"Hello World!"};

    // Invoke a method of an object from the first domain's assembly

    domain.setAsCurrent();

    auto first_class_object =
        assembly.createClass(FIRST_ASSEMBLY_NAMESPACE, FIRST_CLASS_NAME).newObject();
    ASSERT_TRUE(first_class_object.isValid());
    ASSERT_EQ(first_class_object.method("ReturnInt")(EXPECTED_FIRST_OBJECT_VALUE).as<int>(),
              EXPECTED_FIRST_OBJECT_VALUE);

    // Invoke a method of an object from the second domain's assembly

    second_domain.setAsCurrent();

    auto second_class_object =
        second_assembly.createClass(SECOND_ASSEMBLY_NAMESPACE, SECOND_CLASS_NAME).newObject();
    ASSERT_TRUE(second_class_object.isValid());
    ASSERT_EQ(
        second_class_object.method("ReturnString")(EXPECTED_SECOND_OBJECT_VALUE).as<std::string>(),
        EXPECTED_SECOND_OBJECT_VALUE);
}

TEST_F(DomainWithDifferentAssembliesTest, RecreateFirstDomainAndRelaodAssembly)
{
    constexpr int EXPECTED_FIRST_OBJECT_VALUE{42};
    constexpr std::string_view EXPECTED_SECOND_OBJECT_VALUE{"Hello World!"};

    // Recreate the first domain

    ASSERT_TRUE(domain.recreate(DOMAIN_NAME, Domain::CREATION_POLICY_SET_AS_CURRENT));
    ASSERT_TRUE(domain.isValid());

    // Reload the first domain's assembly

    Assembly new_assembly{domain};
    ASSERT_TRUE(new_assembly.load(FIRST_ASSEMBLY_PATH));

    // Invoke a method of an object from the first domain's assembly

    auto first_class_object =
        new_assembly.createClass(FIRST_ASSEMBLY_NAMESPACE, FIRST_CLASS_NAME).newObject();
    ASSERT_TRUE(first_class_object.isValid());
    ASSERT_EQ(first_class_object.method("ReturnInt")(EXPECTED_FIRST_OBJECT_VALUE).as<int>(),
              EXPECTED_FIRST_OBJECT_VALUE);

    // Invoke a method of an object from the second domain's assembly

    second_domain.setAsCurrent();

    auto second_class_object =
        second_assembly.createClass(SECOND_ASSEMBLY_NAMESPACE, SECOND_CLASS_NAME).newObject();
    ASSERT_TRUE(second_class_object.isValid());
    ASSERT_EQ(
        second_class_object.method("ReturnString")(EXPECTED_SECOND_OBJECT_VALUE).as<std::string>(),
        EXPECTED_SECOND_OBJECT_VALUE);
}

} // namespace
