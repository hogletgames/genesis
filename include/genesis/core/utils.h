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

#pragma once

#include <cctype>
#include <functional>
#include <string>
#include <unordered_map>

// Unused
#define GE_UNUSED(x) static_cast<void>(x)

// Breakpoint
#if defined(GE_PLATFORM_UNIX)
#include <csignal>
#define GE_DBGBREAK() GE_UNUSED(::raise(SIGTRAP))
#elif defined(GE_PLATFORM_WINDOWS)
#define GE_DBGBREAK() __debugbreak()
#else
#error "Platform is not defined!"
#endif

// Bind member function
#define GE_EVENT_MEM_FN(mem_func) \
    [this](auto&&... args) { return mem_func(std::forward<decltype(args)>(args)...); }

// Concat names
#define GE_CONCAT_IMPL(lhs, rhs) lhs##rhs
#define GE_CONCAT(lhs, rhs)      GE_CONCAT_IMPL(lhs, rhs)

// Unique name with a given prefix
#define GE_UNIQUE_NAME(prefix) GE_CONCAT(prefix##_, __COUNTER__)

namespace GE {

template<typename Func, typename Type>
auto toEventHandler(Func&& f, Type* instance)
{
    return [&f, instance](const auto& event) {
        return std::invoke(std::forward<Func>(f), instance, event);
    };
}

template<typename Key, typename Value>
Value getValue(const std::unordered_map<Key, Value>& map, const Key& key,
               const Value& default_value = {})
{
    if (auto it = map.find(key); it != map.end()) {
        return it->second;
    }

    return default_value;
}

template<typename T, typename U>
std::unordered_map<U, T> swapKeyAndValue(const std::unordered_map<T, U>& map)
{
    std::unordered_map<U, T> swapped_map;

    for (auto [key, value] : map) {
        swapped_map.emplace(value, key);
    }

    return swapped_map;
}

template<typename Container, typename... Args>
Container jointContainers(Container&& first, const Args&... others)
{
    Container result{std::forward<Container>(first)};
    (result.insert(result.end(), others.begin(), others.end()), ...);
    return result;
}

} // namespace GE
