/*
 * BSD 3-Clause License
 *
 * Copyright (c) 2021-2022, Dmitry Shilnenkov
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

#ifndef GENESIS_CORE_UTILS_H_
#define GENESIS_CORE_UTILS_H_

#include <functional>
#include <unordered_map>

// Breakpoint
#if defined(GE_PLATFORM_UNIX)
    #include <csignal>
    #define GE_DBGBREAK() ::raise(SIGTRAP)
#elif defined(GE_PLATFORM_WINDOWS)
    #define GE_DBGBREAK() __debugbreak()
#else
    #error "Platform is not defined!"
#endif

// Set bit to position
#define GE_BIT(bit) (1 << (bit))

// Bind member function
#define GE_EVENT_MEM_FN(mem_func) \
    [this](auto&&... args) { return mem_func(std::forward<decltype(args)>(args)...); }

// Concat names
#define GE_CONCAT_IMPL(lhs, rhs) lhs##rhs
#define GE_CONCAT(lhs, rhs)      GE_CONCAT_IMPL(lhs, rhs)

// Unique name with a given prefix
#define GE_UNIQ_NAME(prefix) GE_CONCAT(prefix##_, __COUNTER__)

namespace GE {

template<typename Func, typename Type>
auto toEventHandler(Func&& f, Type* instance)
{
    return [&f, instance](const auto& event) {
        return std::invoke(std::forward<Func>(f), instance, event);
    };
}

template<typename FromType, typename ToType>
inline ToType toType(const std::unordered_map<FromType, ToType>& container,
                     const FromType& from_value, const ToType& def_ret = {})
{
    if (auto it = container.find(from_value); it != container.end()) {
        return it->second;
    }

    return def_ret;
}

template<typename T, typename U>
inline std::unordered_map<U, T> swapKeyAndValue(const std::unordered_map<T, U>& map)
{
    std::unordered_map<U, T> swapped_map;

    for (auto [key, value] : map) {
        swapped_map.emplace(value, key);
    }

    return swapped_map;
}

} // namespace GE

#endif // GENESIS_CORE_UTILS_H_
