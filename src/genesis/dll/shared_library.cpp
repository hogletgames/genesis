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

#include "shared_library.h"
#include "platform/shared_library.h"

#include "genesis/core/asserts.h"
#include "genesis/core/log.h"

namespace GE::Dll {
namespace {

std::string_view lastError()
{
    return Platform::getLastError();
}

} // namespace

SharedLibrary::~SharedLibrary()
{
    close();
}

bool SharedLibrary::open(std::string_view path)
{
    GE_ASSERT(!isOpen(), "Library is already opened");

    if (m_library = Platform::openLibrary(path); m_library == nullptr) {
        GE_CORE_ERR("Failed to open library '{}': '{}'", path, lastError());
        return false;
    }

    return true;
}

void SharedLibrary::close()
{
    if (!isOpen()) {
        return;
    }

    if (Platform::closeLibrary(m_library) != 0) {
        GE_CORE_ERR("Failed to close library: '{}'", lastError());
    }

    m_library = nullptr;
}

void* SharedLibrary::loadFunctionPtr(std::string_view name) const
{
    GE_ASSERT(isOpen(), "Library has not been opened");

    if (void* function = Platform::getSymbol(m_library, name); function != nullptr) {
        return function;
    }

    GE_CORE_ERR("Failed to load function '{}': '{}'", name, lastError());
    return nullptr;
}

} // namespace GE::Dll
