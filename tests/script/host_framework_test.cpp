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

#include "genesis/filesystem/filepath.h"
#include "genesis/script/host_framework.h"

#include <gtest/gtest.h>

using namespace GE;
using namespace Script;
using namespace testing;

namespace {

// The .NET runtime can only be initialized once per process and cannot be reinitialized after
// shutdown. Therefore, it is initialized during environment setup to ensure availability for all
// components. For details, see main.cpp.

class HostFrameworkTest: public Test
{
protected:
    static constexpr std::string_view TEST_ASSEMBLY_RELATIVE_PATH{"TestAssembly/TestAssembly.dll"};
    inline static const std::string   TEST_ASSEMBLY_ABSOLUTE_PATH{
        FS::joinPath(FS::currentDir(), TEST_ASSEMBLY_RELATIVE_PATH)};

    static constexpr std::string_view TEST_ASSEMBLY_UPDATED_RELATIVE_PATH{
        "TestAssemblyUpdated/TestAssembly.dll"};
    inline static const std::string TEST_ASSEMBLY_UPDATED_ABSOLUTE_PATH{
        FS::joinPath(FS::currentDir(), TEST_ASSEMBLY_UPDATED_RELATIVE_PATH)};
};

TEST_F(HostFrameworkTest, IsInitialzed)
{
    EXPECT_TRUE(HostFramework::isInitialized());
}

TEST_F(HostFrameworkTest, LoadAssembly_RelativePath)
{
    EXPECT_FALSE(HostFramework::loadAssembly(TEST_ASSEMBLY_RELATIVE_PATH));
}

TEST_F(HostFrameworkTest, LoadAssembly_AbsolutePath)
{
    EXPECT_TRUE(HostFramework::loadAssembly(TEST_ASSEMBLY_ABSOLUTE_PATH));
}

TEST_F(HostFrameworkTest, LoadAssembly_MultipleTimes)
{
    EXPECT_TRUE(HostFramework::loadAssembly(TEST_ASSEMBLY_ABSOLUTE_PATH));
    EXPECT_TRUE(HostFramework::loadAssembly(TEST_ASSEMBLY_ABSOLUTE_PATH));
}

class HostFrameworkTest_GivenLoadedTestAssembly: public HostFrameworkTest
{
protected:
    using ReturnIntFunc = std::function<int(int)>;

    void SetUp() override
    {
        HostFrameworkTest::SetUp();
        EXPECT_TRUE(HostFramework::loadAssembly(TEST_ASSEMBLY_ABSOLUTE_PATH));
    }

    static constexpr std::string_view ASSEMBLY_NAME{"TestAssembly"};
    static constexpr std::string_view MANAGED_CLASS_NAME{"TestAssembly.TestClass"};
    static constexpr std::string_view RETURN_INT_METHOD_NAME{"returnInt"};
    static constexpr std::string_view RETURN_INT_METHOD_UNMANAGED_ONLY_NAME{
        "returnIntUnmanagedOnly"};
    static constexpr std::string_view RETURN_INT_DELEGATE_TYPE_NAME{
        "TestAssembly.TestClass+ReturnIntDelegate"};
};

TEST_F(HostFrameworkTest_GivenLoadedTestAssembly, GetDelegate_ByDelegateName)
{
    ReturnIntFunc return_int_fn;
    ASSERT_TRUE(HostFramework::getFunctionPointer(&return_int_fn, ASSEMBLY_NAME, MANAGED_CLASS_NAME,
                                                  RETURN_INT_METHOD_NAME,
                                                  RETURN_INT_DELEGATE_TYPE_NAME));

    constexpr int EXPECTED_VALUE{42};
    EXPECT_EQ(return_int_fn(EXPECTED_VALUE), EXPECTED_VALUE);
}

TEST_F(HostFrameworkTest_GivenLoadedTestAssembly,
       GetDelegate_DoNotPassDelegateNameWuthoutManagerCallersOnly)
{
    ReturnIntFunc return_int_fn;
    EXPECT_FALSE(HostFramework::getFunctionPointer(&return_int_fn, ASSEMBLY_NAME,
                                                   MANAGED_CLASS_NAME, RETURN_INT_METHOD_NAME));
}

TEST_F(HostFrameworkTest_GivenLoadedTestAssembly, GetDelegate_UnamagedCallersOnly)
{
    ReturnIntFunc return_int_fn;
    ASSERT_TRUE(HostFramework::getFunctionPointer(&return_int_fn, ASSEMBLY_NAME, MANAGED_CLASS_NAME,
                                                  RETURN_INT_METHOD_UNMANAGED_ONLY_NAME));

    constexpr int EXPECTED_VALUE{42};
    EXPECT_EQ(return_int_fn(EXPECTED_VALUE), EXPECTED_VALUE);
}

} // namespace