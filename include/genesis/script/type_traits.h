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

#include <genesis/script/bittable_type.h>
#include <genesis/script/string_type.h>

#include <string>
#include <type_traits>

namespace GE::Script {

template<typename T>
struct type_traits_t;

template<>
struct type_traits_t<int8_t> {
    using ScriptType = BittableType<int8_t>;
};

template<>
struct type_traits_t<uint8_t> {
    using ScriptType = BittableType<uint8_t>;
};

template<>
struct type_traits_t<int16_t> {
    using ScriptType = BittableType<int16_t>;
};

template<>
struct type_traits_t<uint16_t> {
    using ScriptType = BittableType<uint16_t>;
};

template<>
struct type_traits_t<int32_t> {
    using ScriptType = BittableType<int32_t>;
};

template<>
struct type_traits_t<uint32_t> {
    using ScriptType = BittableType<uint32_t>;
};

template<>
struct type_traits_t<int64_t> {
    using ScriptType = BittableType<int64_t>;
};

template<>
struct type_traits_t<uint64_t> {
    using ScriptType = BittableType<uint64_t>;
};

template<>
struct type_traits_t<float> {
    using ScriptType = BittableType<float>;
};

template<>
struct type_traits_t<double> {
    using ScriptType = BittableType<double>;
};

template<>
struct type_traits_t<bool> {
    using ScriptType = BittableType<bool>;
};

template<>
struct type_traits_t<char> {
    using ScriptType = BittableType<char>;
};

template<>
struct type_traits_t<std::string> {
    using ScriptType = StringType;
};

template<>
struct type_traits_t<std::string_view> {
    using ScriptType = StringType;
};

template<>
struct type_traits_t<const char*> {
    using ScriptType = StringType;
};

template<typename T>
using SCRIPT_TYPE = typename type_traits_t<std::decay_t<T>>::ScriptType;

} // namespace GE::Script
