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
#include <genesis/script/invoke_result.h>

#include <array>
#include <tuple>

extern "C" {
typedef struct _MonoObject MonoObject;
typedef struct _MonoMethod MonoMethod;
}

namespace GE::Script {

class GE_API Method
{
public:
    Method() = default;

    bool isValid() const { return m_method != nullptr; }

    int paramCount() const;

    template<typename... Args>
    InvokeResult operator()(Args&&... args);

private:
    friend class Class;
    friend class Object;

    explicit Method(MonoMethod* method, MonoObject* object = nullptr)
        : m_method{method}
        , m_object{object}
    {}

    InvokeResult invoke(void** args, int args_count);

    MonoMethod* m_method{nullptr};
    MonoObject* m_object{nullptr};
};

template<typename... Args>
InvokeResult Method::operator()(Args&&... args)
{
    auto get_arg_pointers = [](auto&&... arguments) {
        return std::array<void*, sizeof...(arguments)>{arguments.asMethodArg()...};
    };

    std::tuple arg_tuple{ToScriptType<Args>{args}...};
    auto method_args = std::apply(get_arg_pointers, arg_tuple);
    return invoke(method_args.data(), method_args.size());
}

} // namespace GE::Script
