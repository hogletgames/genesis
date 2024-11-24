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

#pragma once

#include <genesis/core/export.h>
#include <genesis/script/class_type.h>
#include <genesis/script/object.h>

#include <optional>
#include <string>

extern "C" {
typedef struct _MonoObject MonoObject;
}

namespace GE::Script {

class InvokeResultAccessor;
class Method;

class GE_API InvokeResult
{
public:
    template<typename T>
    std::optional<T> as() const;

    ClassType type() const { return m_result.type(); }

    operator bool() const { return hasError(); }
    bool hasError() const { return !m_error_message.empty(); }
    std::string_view errorMessage() const { return m_error_message; }

private:
    friend class InvokeResultAccessor;

    InvokeResult(MonoObject* result, MonoObject* exception)
        : m_result{ObjectAccessor::createObject(result)}
        , m_error_message{asString(exception)}
    {}

    std::string asString(MonoObject* exception) const;

    Object m_result;
    std::string m_error_message;
};

template<typename T>
std::optional<T> InvokeResult::as() const
{
    return m_result.as<T>();
}

class InvokeResultAccessor
{
private:
    friend class Method;

    static InvokeResult createInvokeResult(MonoObject* result, MonoObject* exception);
};

} // namespace GE::Script
