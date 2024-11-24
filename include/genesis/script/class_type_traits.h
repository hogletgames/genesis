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

#include <genesis/script/class_type.h>

#include <string>
#include <type_traits>

namespace GE::Script {

template<typename T>
struct class_type_traits_t;

template<>
struct class_type_traits_t<int8_t> {
    static constexpr ClassType CLASS_TYPE{ClassType::I1};
    static constexpr std::string_view TYPE_NAME{"SByte"};
};

template<>
struct class_type_traits_t<uint8_t> {
    static constexpr ClassType CLASS_TYPE{ClassType::U1};
    static constexpr std::string_view TYPE_NAME{"Byte"};
};

template<>
struct class_type_traits_t<int16_t> {
    static constexpr ClassType CLASS_TYPE{ClassType::I2};
    static constexpr std::string_view TYPE_NAME{"Int16"};
};

template<>
struct class_type_traits_t<uint16_t> {
    static constexpr ClassType CLASS_TYPE{ClassType::U2};
    static constexpr std::string_view TYPE_NAME{"UInt16"};
};

template<>
struct class_type_traits_t<int32_t> {
    static constexpr ClassType CLASS_TYPE{ClassType::I4};
    static constexpr std::string_view TYPE_NAME{"Int32"};
};

template<>
struct class_type_traits_t<uint32_t> {
    static constexpr ClassType CLASS_TYPE{ClassType::U4};
    static constexpr std::string_view TYPE_NAME{"UInt32"};
};

template<>
struct class_type_traits_t<int64_t> {
    static constexpr ClassType CLASS_TYPE{ClassType::I8};
    static constexpr std::string_view TYPE_NAME{"Int64"};
};

template<>
struct class_type_traits_t<uint64_t> {
    static constexpr ClassType CLASS_TYPE{ClassType::U8};
    static constexpr std::string_view TYPE_NAME{"UInt64"};
};

template<>
struct class_type_traits_t<float> {
    static constexpr ClassType CLASS_TYPE{ClassType::R4};
    static constexpr std::string_view TYPE_NAME{"Single"};
};

template<>
struct class_type_traits_t<double> {
    static constexpr ClassType CLASS_TYPE{ClassType::R8};
    static constexpr std::string_view TYPE_NAME{"Double"};
};

template<>
struct class_type_traits_t<bool> {
    static constexpr ClassType CLASS_TYPE{ClassType::BOOLEAN};
    static constexpr std::string_view TYPE_NAME{"Boolean"};
};

template<>
struct class_type_traits_t<char> {
    static constexpr ClassType CLASS_TYPE{ClassType::CHAR};
    static constexpr std::string_view TYPE_NAME{"Char"};
};

template<>
struct class_type_traits_t<std::string> {
    static constexpr ClassType CLASS_TYPE{ClassType::STRING};
};

template<>
struct class_type_traits_t<std::string_view> {
    static constexpr ClassType CLASS_TYPE{ClassType::STRING};
};

template<>
struct class_type_traits_t<const char*> {
    static constexpr ClassType CLASS_TYPE{ClassType::STRING};
};

template<typename T>
constexpr ClassType CLASS_TYPE{class_type_traits_t<std::decay_t<T>>::CLASS_TYPE};

template<typename T>
constexpr std::string_view CLASS_TYPE_NAME{class_type_traits_t<std::decay_t<T>>::TYPE_NAME};

} // namespace GE::Script
