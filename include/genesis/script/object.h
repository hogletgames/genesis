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

#include <genesis/core/export.h>
#include <genesis/script/type_traits.h>

#include <optional>

extern "C" {
typedef struct _MonoObject MonoObject;
typedef struct _MonoClass MonoClass;
}

namespace GE::Script {

class Class;
class Method;

class GE_API Object
{
public:
    Object() = default;
    ~Object();

    bool isValid() const { return m_object != nullptr; }

    Class getClass() const;
    ClassType type() const;
    void* unbox() const;
    MonoObject* nativeHandle() const { return m_object; }

    template<typename T>
    std::optional<T> as() const;

private:
    friend class Class;
    friend class InvokeResult;

    explicit Object(MonoObject* object, MonoClass* klass = nullptr);

    MonoObject* m_object{nullptr};
    MonoClass* m_class{nullptr};
    uint32_t m_gc_handle{0};
};

template<typename T>
std::optional<T> Object::as() const
{
    return isValid() ? ToScriptType<T>{*this}.value() : std::nullopt;
}

} // namespace GE::Script
