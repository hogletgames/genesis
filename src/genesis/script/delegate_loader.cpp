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

#include "delegate_loader.h"
#include "host_framework.h"

#include "genesis/core/asserts.h"
#include "genesis/core/format.h"
#include "genesis/core/log.h"

#include <string>

namespace GE::Script {

bool DelegateLoader::loadAssembly(std::string_view assembly_path)
{
    GE_CORE_ASSERT(HostFramework::isInitialized(), "Host Framework hasn't been initialized");
    GE_CORE_INFO("Loading '{}' assembly", assembly_path);

    int rc = HostFramework::loadAssemblyFn()(assembly_path.data(), nullptr, nullptr);
    if (rc != 0) {
        GE_CORE_ERR("Failed to load assembly: {:#010x}", static_cast<uint32_t>(rc));
        return false;
    }

    return true;
}

void* DelegateLoader::getFunctionPointer(std::string_view assembly_name,
                                         std::string_view managed_class_name,
                                         std::string_view method_name,
                                         std::string_view delegate_type_name)
{
    GE_CORE_ASSERT(HostFramework::isInitialized(), "Host Framework hasn't been initialized");
    GE_CORE_DBG("Loading '{}.{}, {}' method", managed_class_name, method_name, assembly_name);

    // Create an assembly qualified delegate type name if the passed value is not empty, otherwise
    // use 'UNMANAGEDCALLERSONLY_METHOD'.

    std::string full_delegate_type_name;
    const char_t* full_delegate_name_ptr = UNMANAGEDCALLERSONLY_METHOD;
    if (!delegate_type_name.empty()) {
        full_delegate_type_name = GE_FMTSTR("{}, {}", delegate_type_name, assembly_name);
        full_delegate_name_ptr = full_delegate_type_name.data();
    }

    // Create an assembly qualified type name

    auto type_name = GE_FMTSTR("{}, {}", managed_class_name, assembly_name);

    void* function{nullptr};
    int rc = HostFramework::getFunctionPointerFn()(
        type_name.c_str(), method_name.data(), full_delegate_name_ptr, nullptr, nullptr, &function);
    if (rc != 0 || function == nullptr) {
        GE_CORE_ERR("Failed to load '{}' method of '{}' class from '{}' assembly: "
                    "hresult={:#010x}, function={:p}",
                    method_name, managed_class_name, assembly_name, static_cast<uint32_t>(rc),
                    function);
        return nullptr;
    }

    return function;
}

} // namespace GE::Script
