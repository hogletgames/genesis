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

#include "bindings/assembly_manager.h"
#include "delegate_loader.h"

#include "genesis/core/asserts.h"
#include "genesis/core/log.h"

namespace GE::Script::Bindings {

bool AssemblyManager::initialize()
{
    constexpr std::string_view ASSEMBLY_NAME = "Ge.Framework";
    constexpr std::string_view NATIVE_EXPORT_CLASS = "NativeExport";

    constexpr std::string_view LOAD_ASSEMBLY_FN_NAME = "LoadAssembly";
    constexpr std::string_view UNLOAD_ASSEMBLY_FN_NAME = "UnloadAssembly";
    constexpr std::string_view GET_FUNCTION_POINTER_FN_NAME = "GetFunctionPointer";

    if (!DelegateLoader::getDelegate(&m_load_assembly_fn, ASSEMBLY_NAME, NATIVE_EXPORT_CLASS,
                                     LOAD_ASSEMBLY_FN_NAME) ||
        !DelegateLoader::getDelegate(&m_unload_assembly_fn, ASSEMBLY_NAME, NATIVE_EXPORT_CLASS,
                                     UNLOAD_ASSEMBLY_FN_NAME) ||
        !DelegateLoader::getDelegate(&m_get_function_pointer_fn, ASSEMBLY_NAME, NATIVE_EXPORT_CLASS,
                                     GET_FUNCTION_POINTER_FN_NAME)) {
        GE_CORE_ERR("Failed to initialize Assembly Manager");
        return false;
    }

    return true;
}

bool AssemblyManager::loadAssembly(std::string_view path) const
{
    GE_CORE_ASSERT(m_load_assembly_fn, "Assembly Manager hasn't been initialized");
    return m_load_assembly_fn(path.data());
}

bool AssemblyManager::unloadAssembly(std::string_view name) const
{
    GE_CORE_ASSERT(m_unload_assembly_fn, "Assembly Manager hasn't been initialized");
    return m_unload_assembly_fn(name.data());
}

void* AssemblyManager::getFunctionPointer(std::string_view assembly_name,
                                          std::string_view type_name,
                                          std::string_view method_name) const
{
    GE_CORE_ASSERT(m_get_function_pointer_fn, "Assembly Manager has no function pointer");
    return m_get_function_pointer_fn(assembly_name.data(), type_name.data(), method_name.data());
}

} // namespace GE::Script::Bindings
