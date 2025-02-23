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
#include "domain.h"
#include "invoke_result.h"

#include "genesis/core/asserts.h"
#include "genesis/core/format.h"
#include "genesis/core/log.h"

#include <mono/metadata/appdomain.h>
#include <mono/metadata/object.h>

#include <algorithm>
#include <ranges>
#include <vector>

namespace GE::Script {
namespace {

template<std::ranges::view View>
std::string typesAsString(View types)
{
    std::string types_string = "[";

    for (uint32_t i{0}; i < types.size(); i++) {
        types_string += GE_FMTSTR("{}:{}", i, GE::toString(types[i]));

        if (i + 1 < types.size()) {
            types_string += ", ";
        }
    }

    return types_string + "]";
}

MonoObject* makeExceptionMessage(std::string_view message)
{
    auto* exception = mono_string_new(Domain::currentDomain().nativeHandle(), message.data());
    return reinterpret_cast<MonoObject*>(exception);
}

InvokeResult createInvalidInvokeResult(std::string_view error_message)
{
    return InvokeResult{nullptr, makeExceptionMessage(error_message)};
}

class MethodSignature
{
public:
    explicit MethodSignature(MonoMethod* method)
    {
        if (method == nullptr) {
            GE_CORE_ERR("Failed to create Method Signature: NULL method");
            return;
        }

        if (m_signature = mono_method_signature(method); m_signature == nullptr) {
            GE_CORE_ERR("Failed to create Method Signature for '{}' method",
                        mono_method_get_name(method));
        }
    }

    bool isValid() const { return m_signature != nullptr; }

    bool isInstance() const
    {
        GE_CORE_ASSERT(isValid(), "Invalid signature");
        return mono_signature_is_instance(m_signature);
    }

    uint32_t paramCount() const
    {
        GE_CORE_ASSERT(isValid(), "Invalid signature");
        return mono_signature_get_param_count(m_signature);
    }

    std::vector<ClassType> paramTypes() const
    {
        GE_CORE_ASSERT(isValid(), "Invalid signature");

        std::vector<ClassType> param_types{};
        param_types.reserve(paramCount());

        void* iterator{nullptr};
        MonoType* current_type = mono_signature_get_params(m_signature, &iterator);

        while (current_type != nullptr) {
            param_types.push_back(toClassType(mono_type_get_type(current_type)));
            current_type = mono_signature_get_params(m_signature, &iterator);
        }

        return param_types;
    }

private:
    MonoMethodSignature* m_signature{nullptr};
};

} // namespace

Method::Method(MonoMethod* method)
    : m_method{method}
{}

Method::Method(MonoMethod* method, const Object& object)
    : Method{method}
{
    if (isValid() && isInstance()) {
        GE_ASSERT(klass().type() == object.type(), "Method and object types don't match: {} != {}",
                  klass().type(), object.type());
        m_object = object.nativeHandle();
    }
}

bool Method::isInstance() const
{
    MethodSignature signature{m_method};
    if (!signature.isValid()) {
        GE_CORE_ERR("Failed to check if '{}' is an instance method", name());
        return false;
    }

    return signature.isInstance();
}

int Method::paramCount() const
{
    MethodSignature signature{m_method};
    if (!signature.isValid()) {
        GE_CORE_ERR("Failed to get parameter count for '{}' method", name());
        return 0;
    }

    return signature.paramCount();
}

std::vector<ClassType> Method::paramTypes() const
{
    MethodSignature signature{m_method};
    if (!signature.isValid()) {
        GE_CORE_ERR("Failed to get parameter types for '{}' method", name());
        return {};
    }

    return signature.paramTypes();
}

std::string_view Method::name() const
{
    return isValid() ? mono_method_get_name(m_method) : std::string_view{};
}

Class Method::klass() const
{
    return isValid() ? Class{mono_method_get_class(m_method)} : Class{};
}

bool Method::validateArguments(std::span<Object> args) const
{
    GE_CORE_ASSERT(isValid(), "Trying to validate args using invalid method");

    auto param_types = paramTypes();
    auto arg_types = std::views::transform(args, [](const auto& object) { return object.type(); });

    if (!std::ranges::equal(param_types, arg_types)) {
        GE_CORE_ERR("Expected args: {}, got: {}", typesAsString(std::views::all(param_types)),
                    typesAsString(arg_types));
        return false;
    }

    return true;
}

InvokeResult Method::invoke(std::span<Object> args) const
{
    if (!isValid()) {
        std::string_view error_message = "Invoking invalid method";
        GE_CORE_ERR(error_message);
        return createInvalidInvokeResult(error_message);
    }

    if (isInstance() && !m_object) {
        auto error_message =
            GE_FMTSTR("Trying to invoke '{}' instance method without an instance", name());
        GE_CORE_ERR(error_message);
        return createInvalidInvokeResult(error_message);
    }

    if (!validateArguments(args)) {
        auto error_message = GE_FMTSTR("Passed invalid arguments to '{}' method", name());
        GE_CORE_ERR(error_message);
        return createInvalidInvokeResult(error_message);
    }

    auto* mono_args = mono_array_new(Domain::currentDomain().nativeHandle(),
                                     mono_get_object_class(), args.size());

    for (uint32_t i{0}; i < args.size(); i++) {
        void* element = mono_array_addr_with_size(mono_args, sizeof(MonoObject*), i);
        mono_gc_wbarrier_set_arrayref(mono_args, element, args[i].nativeHandle());
    }

    MonoObject* exception{nullptr};
    auto* result = mono_runtime_invoke_array(m_method, m_object, mono_args, &exception);
    return InvokeResult{result, exception};
}

} // namespace GE::Script
