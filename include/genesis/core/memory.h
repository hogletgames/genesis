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

#include <genesis/core/exception.h>
#include <genesis/core/log.h>

#include <memory>

namespace GE {

template<typename T, typename D = std::default_delete<T>>
using Scoped = std::unique_ptr<T, D>;

template<typename T>
using Shared = std::shared_ptr<T>;

template<typename T, typename... Args>
constexpr Scoped<T> makeScoped(Args&&... args)
{
    return std::make_unique<T>(std::forward<Args>(args)...);
}

template<typename T, typename... Args>
constexpr Shared<T> makeShared(Args&&... args)
{
    return std::make_shared<T>(std::forward<Args>(args)...);
}

template<typename T, typename... Args>
Scoped<T> tryMakeScoped(Args&&... args)
{
    try {
        return makeScoped<T>(std::forward<Args>(args)...);
    } catch (const std::exception& e) {
        GE_CORE_ERR("Exception: {}", e.what());
        return nullptr;
    }
}

template<typename T, typename... Args>
Shared<T> tryMakeShared(Args&&... args)
{
    try {
        return makeScoped<T>(std::forward<Args>(args)...);
    } catch (const std::exception& e) {
        GE_CORE_ERR("Exception: {}", e.what());
        return nullptr;
    }
}

template<class T, class U>
Shared<T> staticPtrCast(const Shared<U>& ptr)
{
    return std::static_pointer_cast<T>(ptr);
}

template<class T, class U>
Shared<T> dynamicPtrCast(const Shared<U>& ptr)
{
    return std::dynamic_pointer_cast<T>(ptr);
}

} // namespace GE
