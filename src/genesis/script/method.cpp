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

#include "genesis/core/asserts.h"
#include "genesis/core/format.h"
#include "genesis/core/log.h"

#include <mono/metadata/appdomain.h>
#include <mono/metadata/object.h>

namespace GE::Script {
namespace {

MonoObject* makeExceptionMessage(std::string_view message)
{
    auto* exception = mono_string_new(mono_domain_get(), message.data());
    return reinterpret_cast<MonoObject*>(exception);
}

class ParamTypeIterator
{
public:
    explicit ParamTypeIterator(MonoMethodSignature* signature)
        : m_signature{signature}
    {
        if (isValid()) {
            getNextType();
        }
    }

    bool isValid() const { return m_signature != nullptr; }
    bool hasCurrentType() const { return m_current_type != nullptr; }

    bool hasNext() const { return isValid() && hasCurrentType(); }

    ClassType next()
    {
        if (!hasCurrentType()) {
            return ClassType::UNKNOWN;
        }

        ClassType type{toClassType(mono_type_get_type(m_current_type))};
        getNextType();
        return type;
    }

private:
    void getNextType() { m_current_type = mono_signature_get_params(m_signature, &m_iterator); }

    MonoMethodSignature* m_signature{nullptr};
    MonoType* m_current_type{nullptr};
    void* m_iterator{nullptr};
};

class MethodSignature
{
public:
    explicit MethodSignature(MonoMethod* method)
    {
        if (method == nullptr) {
            GE_CORE_ERR("Failed to create Method Signature NULL method");
            return;
        }

        m_signature = mono_method_signature(method);

        if (m_signature == nullptr) {
            GE_CORE_ERR("Failed to create Method Signature for '{}' method",
                        mono_method_get_name(method));
        }
    }

    bool isValid() const { return m_signature != nullptr; }
    bool isInstance() const { return isValid() ? mono_signature_is_instance(m_signature) : false; }
    int paramCount() const { return isValid() ? mono_signature_get_param_count(m_signature) : 0; }

    std::vector<ClassType> paramTypes() const
    {
        if (!isValid()) {
            return {};
        }

        std::vector<ClassType> param_types;
        ParamTypeIterator iterator{m_signature};

        while (iterator.hasNext()) {
            param_types.push_back(iterator.next());
        }

        return param_types;
    }

private:
    MonoMethodSignature* m_signature{nullptr};
};

} // namespace

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
        GE_CORE_ERR("Failed to get param count for '{}' method", name());
        return 0;
    }

    return signature.paramCount();
}

std::vector<ClassType> Method::paramTypes() const
{
    return MethodSignature{m_method}.paramTypes();
}

std::string_view Method::name() const
{
    return isValid() ? mono_method_get_name(m_method) : std::string_view{};
}

bool Method::validateArguments(const ClassType* arg_types, int arg_types_count) const
{
    GE_CORE_ASSERT(isValid(), "Trying to validate args using invalid method");

    auto param_types = paramTypes();

    if (arg_types_count != static_cast<int>(param_types.size())) {
        GE_CORE_ERR("Invalid args count for '{}' method: expected={}, got={}", name(),
                    param_types.size(), arg_types_count);
        return false;
    }

    for (int i{0}; i < arg_types_count; i++) {
        if (arg_types[i] != param_types[i]) {
            GE_CORE_ERR("Argument of type '{}' doesn't match the parameter arg of '{}' type, "
                        "argument index: {}",
                        toString(arg_types[i]), toString(param_types[i]), i);
            return false;
        }
    }

    return true;
}

InvokeResult Method::invoke(void** args, int args_count, const ClassType* arg_types,
                            int arg_types_count) const
{
    GE_CORE_ASSERT(arg_types_count == args_count, "Argument count must match argument type count");

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

    if (!validateArguments(arg_types, arg_types_count)) {
        auto error_message = GE_FMTSTR("Passed invalid arguments to '{}' method", name());
        GE_CORE_ERR(error_message);
        return createInvalidInvokeResult(error_message);
    }

    MonoObject* exception{nullptr};
    auto* result = mono_runtime_invoke(m_method, m_object, args, &exception);
    return InvokeResultAccessor::createInvokeResult(result, exception);
}

InvokeResult Method::createInvalidInvokeResult(std::string_view error_message)
{
    return InvokeResultAccessor::createInvokeResult(nullptr, makeExceptionMessage(error_message));
}

Method MethodAccessor::createMethod(MonoMethod* method)
{
    return Method{method};
}

void MethodAccessor::setMethodInstance(Method* method, MonoObject* object)
{
    method->setObject(object);
}

} // namespace GE::Script
