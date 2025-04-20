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

#include <genesis/core/asserts.h>

#include <functional>
#include <string_view>
#include <type_traits>

namespace GE::Dll {

class GE_API SharedLibrary
{
public:
    SharedLibrary() = default;
    SharedLibrary(const SharedLibrary&) = delete;
    SharedLibrary(SharedLibrary&&) = default;
    ~SharedLibrary();

    SharedLibrary& operator=(const SharedLibrary&) = delete;
    SharedLibrary& operator=(SharedLibrary&&) = default;

    bool open(std::string_view path);
    void close();

    template<typename Signature>
    bool loadFunction(std::function<Signature>* function, std::string_view name) const;

    bool isOpen() const { return m_library != nullptr; }

private:
    void* loadFunctionPtr(std::string_view name) const;

    void* m_library{nullptr};
};

template<typename Signature>
bool SharedLibrary::loadFunction(std::function<Signature>* function, std::string_view name) const
{
    GE_ASSERT(function != nullptr, "Function pointer is null");

    if (void* loaded_function = loadFunctionPtr(name); loaded_function != nullptr) {
        *function = std::add_pointer_t<Signature>(loaded_function);
        return true;
    }

    return false;
}

} // namespace GE::Dll
