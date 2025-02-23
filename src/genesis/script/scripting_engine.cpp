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
#include "domain.h"

#include "genesis/core/asserts.h"
#include "genesis/core/log.h"

#include <mono/jit/jit.h>
#include <mono/metadata/loader.h>

namespace GE::Script {

bool ScriptingEngine::initialize(std::string_view domain_name, std::string_view runtime_version)
{
    GE_CORE_ASSERT(s_root_domain == nullptr, "Mono runtime has already been initialized");
    GE_CORE_INFO("Initializing '{}' Mono runtime with version '{}'", domain_name, runtime_version);

    s_root_domain = mono_jit_init_version(domain_name.data(), runtime_version.data());
    return s_root_domain != nullptr;
}

void ScriptingEngine::shutdown()
{
    GE_CORE_ASSERT(s_root_domain != nullptr, "Shutting down uninitialized Mono runtime");
    GE_CORE_INFO("Shutting down Mono runtime");

    mono_jit_cleanup(s_root_domain);
    s_root_domain = nullptr;
}

void ScriptingEngine::addInternalCall(std::string_view method_name, const void* method)
{
    GE_CORE_DBG("Registering internal call: {}", method_name);
    mono_add_internal_call(method_name.data(), method);
}

} // namespace GE::Script
