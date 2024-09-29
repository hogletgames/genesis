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

#include "method.h"
#include "invoke_result.h"

#include "genesis/core/format.h"
#include "genesis/core/log.h"

#include <mono/metadata/appdomain.h>
#include <mono/metadata/object.h>

#include <string_view>

namespace GE::Script {
namespace {

MonoObject* makeException(std::string_view message)
{
    auto* exception = mono_string_new(mono_domain_get(), message.data());
    return reinterpret_cast<MonoObject*>(exception);
}

} // namespace

int Method::paramCount() const
{
    if (!isValid()) {
        GE_CORE_ERR("Trying to get param count for an invalid method");
        return 0;
    }

    auto* signature = mono_method_signature(m_method);
    if (signature == nullptr) {
        GE_CORE_ERR("Failed to get signature for method: {}", mono_method_get_name(m_method));
        return 0;
    }

    return mono_signature_get_param_count(signature);
}

InvokeResult Method::invoke(void** args, int args_count)
{
    if (!isValid()) {
        GE_CORE_ERR("Trying to get param count for invalid method");
        return {nullptr, makeException("Invalid method")};
    }

    if (args_count != paramCount()) {
        auto error_string =
            GE_FMTSTR("Invalid args count: expected={}, got={}", paramCount(), args_count);
        GE_CORE_ERR(error_string);
        return {nullptr, makeException(error_string)};
    }

    MonoObject* exception{nullptr};
    auto* result = mono_runtime_invoke(m_method, m_object, args, &exception);
    return InvokeResult{result, exception};
}

} // namespace GE::Script
