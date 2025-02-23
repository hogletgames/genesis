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

#include <genesis/script/concepts.h>

#include <type_traits>

namespace GE::Tests {

template<typename T>
struct test_values_t;

template<std::signed_integral T>
struct test_values_t<T> {
    static constexpr T VALUE{-42};
    static constexpr T ALTERNATIVE_VALUE{-43};
};

template<std::unsigned_integral T>
struct test_values_t<T> {
    static constexpr T VALUE{42};
    static constexpr T ALTERNATIVE_VALUE{43};
};

template<std::floating_point T>
struct test_values_t<T> {
    static constexpr T VALUE{4.2};
    static constexpr T ALTERNATIVE_VALUE{4.2};
};

template<>
struct test_values_t<char> {
    static constexpr char VALUE{'*'};             // 42
    static constexpr char ALTERNATIVE_VALUE{'+'}; // 43
};

template<>
struct test_values_t<bool> {
    static constexpr bool VALUE{true};
    static constexpr bool ALTERNATIVE_VALUE{false};
};

template<Script::IsStringType T>
struct test_values_t<T> {
    static constexpr T VALUE{"42"};
    static constexpr T ALTERNATIVE_VALUE{"43"};
};

template<typename T>
constexpr T TESTED_VALUE{test_values_t<std::decay_t<T>>::VALUE};

template<typename T>
constexpr T TESTED_ALT_VALUE{test_values_t<std::decay_t<T>>::ALTERNATIVE_VALUE};

} // namespace GE::Tests
