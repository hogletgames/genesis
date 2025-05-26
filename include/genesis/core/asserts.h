/*
 * BSD 3-Clause License
 *
 * Copyright (c) 2021, Dmitry Shilnenkov
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

#include <genesis/core/log.h>
#include <genesis/core/utils.h>

#ifndef GE_DISABLE_ASSERTS
#define GE_CORE_ASSERT(expr, ...)                                                                  \
    static_cast<bool>(expr) ? static_cast<void>(0)                                                 \
                            : ::GE::Details::coreAssert(__FILE__, __LINE__, #expr, __VA_ARGS__)

#define GE_ASSERT(expr, ...)                                                                       \
    static_cast<bool>(expr) ? static_cast<void>(0)                                                 \
                            : ::GE::Details::clientAssert(__FILE__, __LINE__, #expr, __VA_ARGS__)

namespace GE::Details {

template<typename... Args>
inline void coreAssert(const char* file, int line, const char* expr, Args&&... args)
{
    GE_CORE_CRIT("assert failed: {}:{} '{}'", file, line, expr);

    if constexpr (sizeof...(Args) > 0) {
        GE_CORE_CRIT(std::forward<Args>(args)...);
    }

    GE_DBGBREAK();
}

template<typename... Args>
inline void clientAssert(const char* file, int line, const char* expr, Args&&... args)
{
    GE_CRIT("assert failed: {}:{} '{}'", file, line, expr);

    if constexpr (sizeof...(Args) > 0) {
        GE_CRIT(std::forward<Args>(args)...);
    }

    GE_DBGBREAK();
}

} // namespace GE::Details
#else
#define GE_CORE_ASSERT(expr, ...) static_cast<void>(expr)
#define GE_ASSERT(expr, ...)      static_cast<void>(expr)
#endif // GE_DISABLE_ASSERTS
