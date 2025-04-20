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

#include <gtest/gtest.h>

#include "genesis/core/log.h"
#include "genesis/dll/path.h"
#include "genesis/dll/shared_library.h"
#include "genesis/filesystem/filepath.h"

using namespace GE;
using namespace Dll;
using namespace testing;

namespace {

class SharedLibraryTest: public Test
{
protected:
    static void SetUpTestCase()
    {
        Log::settings_t settings{
            .core_log_level = Logger::Level::ERROR,
            .client_log_level = Logger::Level::ERROR,
        };

        Log::initialize(settings);
    }

    static void TearDownTestCase() { Log::shutdown(); }

    static constexpr std::string_view TEST_LIBRARY_NAME{"ge_shared_library_test"};
    static constexpr std::string_view TEST_LIBRARY_DIR{"shared_library_test"};

    SharedLibrary shared_library;
};

TEST_F(SharedLibraryTest, IsOpen_DeafultObject)
{
    ASSERT_FALSE(shared_library.isOpen());
}

TEST_F(SharedLibraryTest, Open_NonExistingPath)
{
    const std::string NON_EXISTING_LIBRARY_PATH = libraryPath("no_such_lib");

    ASSERT_FALSE(shared_library.open(NON_EXISTING_LIBRARY_PATH));
}

TEST_F(SharedLibraryTest, Open_ExistingRelativePath)
{
    const std::string LIBRARY_PATH = libraryPath(TEST_LIBRARY_NAME, TEST_LIBRARY_DIR);

    ASSERT_TRUE(shared_library.open(LIBRARY_PATH));
}

TEST_F(SharedLibraryTest, Open_ExistingAbsolutePath)
{
    const std::string ABSOLUTE_LIB_DIR_PATH = FS::joinPath(FS::currentDir(), TEST_LIBRARY_DIR);
    const std::string LIBRARY_PATH = libraryPath(TEST_LIBRARY_NAME, ABSOLUTE_LIB_DIR_PATH);

    ASSERT_TRUE(shared_library.open(LIBRARY_PATH));
}

class SharedLibraryTest_GivenOpenLibrary: public SharedLibraryTest
{
protected:
    using ReturnIntFunctionType = std::function<int(int)>;

    void SetUp() override
    {
        SharedLibraryTest::SetUp();

        ASSERT_TRUE(shared_library.open(libraryPath(TEST_LIBRARY_NAME, TEST_LIBRARY_DIR)));
    }

    static constexpr std::string_view RETURN_INT_FUN_NAME{"return_int"};

    ReturnIntFunctionType return_int_function;
};

TEST_F(SharedLibraryTest_GivenOpenLibrary, IsOpen)
{
    ASSERT_TRUE(shared_library.isOpen());
}

TEST_F(SharedLibraryTest_GivenOpenLibrary, Close)
{
    shared_library.close();
    ASSERT_FALSE(shared_library.isOpen());
}

TEST_F(SharedLibraryTest_GivenOpenLibrary, CloseTwice)
{
    shared_library.close();
    ASSERT_FALSE(shared_library.isOpen());

    shared_library.close();
    ASSERT_FALSE(shared_library.isOpen());
}

TEST_F(SharedLibraryTest_GivenOpenLibrary, LoadFunction_NonExists)
{
    constexpr std::string_view FUNCTION_NAME{"non_existing_function"};

    std::function<void()> non_exiting_func;
    ASSERT_FALSE(shared_library.loadFunction(&non_exiting_func, FUNCTION_NAME));
}

TEST_F(SharedLibraryTest_GivenOpenLibrary, LoadFunction_Exists)
{
    ASSERT_TRUE(shared_library.loadFunction(&return_int_function, RETURN_INT_FUN_NAME));
}

TEST_F(SharedLibraryTest_GivenOpenLibrary, LoadFunction_MalformedFunctionSignature)
{
    // We don't check the signature, just cast the returned function pointer into passed function

    std::function<void()> invalid_function;
    ASSERT_TRUE(shared_library.loadFunction(&invalid_function, RETURN_INT_FUN_NAME));
}

class SharedLibraryTest_GiveLoadedFunction: public SharedLibraryTest_GivenOpenLibrary
{
protected:
    void SetUp() override
    {
        SharedLibraryTest_GivenOpenLibrary::SetUp();

        ASSERT_TRUE(shared_library.loadFunction(&return_int_function, RETURN_INT_FUN_NAME));
    }
};

TEST_F(SharedLibraryTest_GiveLoadedFunction, InvokeLoadedFunction)
{
    constexpr int EXPECTED_VALUE{42};

    ASSERT_EQ(return_int_function(EXPECTED_VALUE), EXPECTED_VALUE);
}

} // namespace
