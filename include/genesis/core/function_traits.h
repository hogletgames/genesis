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

#include <genesis/core/concepts.h>

#include <functional>

namespace GE {

// Traits
template<typename T>
struct raw_function_signature_t;

template<typename ResultType, typename... Args>
struct raw_function_signature_t<ResultType (*)(Args...)> {
    using Type = ResultType(Args...);
};

template<typename T>
struct raw_function_type_t;

template<typename ResultType, typename... Args>
struct raw_function_type_t<std::function<ResultType(Args...)>> {
    using Type = ResultType (*)(Args...);
};

template<typename T>
struct function_signature_t;

template<typename Signature>
struct function_signature_t<std::function<Signature>> {
    using Type = Signature;
};

// Aliases
template<IsRawFunction RawFunction>
using RawFunctionSignature = typename raw_function_signature_t<RawFunction>::Type;

template<IsStdFunction Function>
using RawFunctionType = typename raw_function_type_t<Function>::Type;

template<IsStdFunction Function>
using FunctionSignature = typename function_signature_t<Function>::Type;

template<IsRawFunction RawFunction>
using FunctionType = std::function<RawFunctionSignature<RawFunction>>;

} // namespace GE
