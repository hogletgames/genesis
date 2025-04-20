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

#pragma once

#include <genesis/core/export.h>
#include <genesis/core/function_traits.h>
#include <genesis/core/memory.h>

#include <string_view>

#include <coreclr_delegates.h>

namespace GE::Script {

using LoadAssemblyAndGetFunctionPointerFn = FunctionType<load_assembly_and_get_function_pointer_fn>;
using GetFunctionPointerFn = FunctionType<get_function_pointer_fn>;
using LoadAssemblyFn = FunctionType<load_assembly_fn>;
using LoadAssemblyBytesFn = FunctionType<load_assembly_bytes_fn>;

class GE_API HostFramework
{
public:
    static bool initialize(std::string_view runtime_config_path);
    static void shutdown();

    static LoadAssemblyAndGetFunctionPointerFn loadAssemblyAndGetFunctionPointerFn();
    static GetFunctionPointerFn getFunctionPointerFn();
    static LoadAssemblyFn loadAssemblyFn();
    static LoadAssemblyBytesFn loadAssemblyBytesFn();

    static bool isInitialized();

private:
    class Context;

    HostFramework();
    ~HostFramework();

    static HostFramework* get()
    {
        static HostFramework instance;
        return &instance;
    }

    Scoped<Context> m_context;
};

} // namespace GE::Script
