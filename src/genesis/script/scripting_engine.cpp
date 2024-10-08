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

#include "scripting_engine.h"
#include "assembly.h"

#include "genesis/core/log.h"
#include "genesis/core/timestamp.h"

#include <mono/jit/jit.h>

namespace GE::Script {
namespace {

int memory_counter{0};

void* malloc(size_t bytes)
{
    void* memory = std::malloc(bytes);
    GE_CORE_INFO("[{:03d}] Allocating: malloc(), bytes={}, memory=0x{:p}", ++memory_counter, bytes,
                 memory);
    return memory;
}

void* realloc(void* buff, size_t bytes)
{
    void* memory = std::realloc(buff, bytes);
    GE_CORE_INFO("[{:03d}] Allocating: realloc(), buff={}, bytes={}, memory=0x{:p}", memory_counter,
                 buff, bytes, memory);
    return memory;
}

void* calloc(size_t count, size_t size)
{
    void* memory = std::calloc(count, size);
    GE_CORE_INFO("[{:03d}] Allocating: calloc() count={}, size={}, memory=0x{:p}", ++memory_counter,
                 count, size, memory);
    return memory;
}

void free(void* memory)
{
    GE_CORE_INFO("[{:03d}] Free memory=0x{:p}", --memory_counter, memory);
    std::free(memory);
}

} // namespace

bool ScriptingEngine::initialize(std::string_view domain_name, std::string_view runtime_version)
{
    MonoAllocatorVTable allocator_vtable{MONO_ALLOCATOR_VTABLE_VERSION, malloc, realloc, free,
                                         calloc};
    mono_set_allocator_vtable(&allocator_vtable);
    new char[1]; // To trigger the memory leak detection

    s_domain = mono_jit_init_version(domain_name.data(), runtime_version.data());
    return s_domain != nullptr;
}

void ScriptingEngine::shutdown()
{
    constexpr Timestamp TIMEOUT{1.0};

    mono_domain_finalize(s_domain, TIMEOUT.ms());
    mono_jit_cleanup(s_domain);
    s_domain = nullptr;
}

Assembly ScriptingEngine::createAssembly()
{
    return Assembly{s_domain};
}

} // namespace GE::Script
