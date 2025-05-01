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

#include <functional>
#include <string_view>

namespace GE::Script::Bindings {

class GE_API AssemblyManager
{
public:
    bool initialize();

    bool loadAssembly(std::string_view path) const;
    bool unloadAssembly(std::string_view name) const;

    template<typename Signature>
    bool getDelegate(std::function<Signature>* delegate, std::string_view assembly_name,
                     std::string_view type_name, std::string_view method_name) const;

private:
    using LoadAssemblyFn = std::function<bool(const char* path)>;
    using UnloadAssemblyFn = std::function<bool(const char* name)>;
    using GetFunctionPointerFn = std::function<void*(
        const char* assembly_name, const char* type_name, const char* method_name)>;

    void* getFunctionPointer(std::string_view assembly_name, std::string_view type_name,
                             std::string_view method_name) const;

    LoadAssemblyFn m_load_assembly_fn;
    UnloadAssemblyFn m_unload_assembly_fn;
    GetFunctionPointerFn m_get_function_pointer_fn;
}

template<typename Signature>
bool AssemblyManager::getDelegate(std::function<Signature>* delegate,
                                  std::string_view assembly_name, std::string_view type_name,
                                  std::string_view method_name) const
{}

} // namespace GE::Script::Bindings
