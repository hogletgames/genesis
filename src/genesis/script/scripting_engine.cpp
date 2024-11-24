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
#include "genesis/core/memory.h"

#include <mono/jit/jit.h>

#include <mutex>
#include <unordered_set>

namespace GE::Script {
namespace {

// A temporary solution to allocate memory for Mono runtime and gracefully free it at shutdown

class AllocatorVTable
{
public:
    static void initialize()
    {
        s_instance = Scoped<AllocatorVTable>{new AllocatorVTable{}};
        s_instance->setMonoAllocatorVTable();
    }

    static void shutdown()
    {
        if (s_instance) {
            s_instance->freeAllocatedMemory();
            s_instance.reset();
        }
    }

private:
    AllocatorVTable() = default;

    void setMonoAllocatorVTable()
    {
        MonoAllocatorVTable allocator_vtable{MONO_ALLOCATOR_VTABLE_VERSION,
                                             &AllocatorVTable::malloc, &AllocatorVTable::realloc,
                                             &AllocatorVTable::free, &AllocatorVTable::calloc};
        mono_set_allocator_vtable(&allocator_vtable);
    }

    void freeAllocatedMemory()
    {
        for (auto* memory : m_allocated_memory) {
            std::free(memory);
        }

        m_allocated_memory.clear();
    }

    void insertMemory(void* memory)
    {
        std::lock_guard lock(m_allocated_memory_mutex);
        m_allocated_memory.insert(memory);
    }

    void eraseMemory(void* memory)
    {
        std::lock_guard lock(m_allocated_memory_mutex);
        m_allocated_memory.erase(memory);
    }

    static void* malloc(size_t bytes)
    {
        void* memory = std::malloc(bytes);
        s_instance->insertMemory(memory);
        return memory;
    }

    static void* calloc(size_t count, size_t size)
    {
        void* memory = std::calloc(count, size);
        s_instance->insertMemory(memory);
        return memory;
    }

    static void* realloc(void* buff, size_t bytes)
    {
        void* memory = std::realloc(buff, bytes);
        s_instance->eraseMemory(buff);
        s_instance->insertMemory(memory);
        return memory;
    }

    static void free(void* memory)
    {
        s_instance->eraseMemory(memory);
        std::free(memory);
    }

    static inline Scoped<AllocatorVTable> s_instance;

    std::mutex m_allocated_memory_mutex;
    std::unordered_set<void*> m_allocated_memory;
};

} // namespace

bool ScriptingEngine::initialize(std::string_view domain_name, std::string_view runtime_version)
{
    GE_CORE_INFO("Initializing '{}' Mono runtime with version '{}'", domain_name, runtime_version);

    AllocatorVTable::initialize();
    s_domain = mono_jit_init_version(domain_name.data(), runtime_version.data());
    return s_domain != nullptr;
}

void ScriptingEngine::shutdown()
{
    GE_CORE_INFO("Shutting down Mono runtime");

    mono_jit_cleanup(s_domain);
    s_domain = nullptr;
    AllocatorVTable::shutdown();
}

Assembly ScriptingEngine::createAssembly()
{
    return Assembly{s_domain};
}

} // namespace GE::Script
