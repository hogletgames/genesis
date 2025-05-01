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
#include "genesis/script/bindings/assembly_manager.h"
#include "genesis/script/host_framework.h"

#include "gtest/gtest.h"

using namespace GE;
using namespace GE::Script;
using namespace testing;

namespace {

class AssemblyManagerTest: public Test
{
protected:
    static void SetUpTestSuite()
    {
        ASSERT_TRUE(HostFramework::isInitialized());
        ASSERT_TRUE(HostFramework::loadAssembly(GE_FRAMEWORK_ASSEMBLY_PATH));
    }

    inline static const std::string GE_FRAMEWORK_ASSEMBLY_PATH{
        FS::joinPath(GE_SCRIPTING_DIR, "Ge.Framework.dll")};
};

TEST_F(AssemblyManagerTest, InitializeInitialiation)
{
    Bindings::AssemblyManager m_assembly_manager;
    EXPECT_TRUE(m_assembly_manager.initialize());
}

class AssemblyManagerTest_GivenInitializedAssemblyManager: public AssemblyManagerTest
{
protected:
    void SetUp() override
    {
        AssemblyManagerTest::SetUp();
        ASSERT_TRUE(assembly_manager.initialize());
    }

    inline static const std::string TEST_ASSEMBLY_PATH{
        FS::joinPath(TEST_ASSEMBLY_DIR, "TestAssembly.dll")};

    Bindings::AssemblyManager assembly_manager;
};

TEST_F(AssemblyManagerTest_GivenInitializedAssemblyManager, LaodAssembly)
{
    EXPECT_TRUE(assembly_manager.loadAssembly(TEST_ASSEMBLY_PATH));
}

class AssemblyManagerTest_GivenLoadedAssembly
    : public AssemblyManagerTest_GivenInitializedAssemblyManager
{
protected:
    using ReturnIntFunc = std::function<int(int)>;

    void SetUp() override
    {
        AssemblyManagerTest_GivenInitializedAssemblyManager::SetUp();
        ASSERT_TRUE(assembly_manager.loadAssembly(TEST_ASSEMBLY_PATH));
    }

    ReturnIntFunc return_int_func;
};

TEST_F(AssemblyManagerTest_GivenLoadedAssembly, GetDelegate)
{
    ASSERT_TRUE(assembly_manager.getDelegate(&return_int_func, "TestAssembly",
                                             "TestAssembly.TestClass", "returnInt",
                                             "TestAssembly.TestClass+ReturnIntDelegate"));

    EXPECT_EQ(42, return_int_func(42));
}

TEST_F(AssemblyManagerTest_GivenLoadedAssembly, GetUnmanagedCallersOnlyDelegate)
{
    ASSERT_TRUE(assembly_manager.getDelegate(&return_int_func, "TestAssembly",
                                             "TestAssembly.TestClass", "returnIntUnmanagedOnly"));

    EXPECT_EQ(42, return_int_func(42));
}

} // namespace
