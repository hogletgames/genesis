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
#include <genesis/script/class.h>
#include <genesis/script/type_traits.h>

#include <optional>

extern "C" {
typedef struct _MonoObject MonoObject;
typedef struct _MonoClass MonoClass;
}

namespace GE::Script {

class Class;
class InvokeResult;
class Method;
class ObjectAccessor;

class GE_API Object
{
public:
    template<typename T>
    explicit Object(T&& value);

    Object() = default;
    ~Object();

    bool isValid() const { return m_class.isValid() && m_object != nullptr; }

    Method method(std::string_view name, int param_count = -1) const;
    const Class& getClass() const { return m_class; }
    ClassType type() const { return m_class.type(); }
    void* unbox() const;
    MonoObject* nativeHandle() const { return m_object; }

    template<typename T>
    bool boxValue(T value);

    template<typename T>
    std::optional<T> as() const;

private:
    friend class ObjectAccessor;

    explicit Object(MonoObject* object, MonoClass* klass = nullptr);
    bool boxValue(void* value, ClassType class_type);

    MonoObject* m_object{nullptr};
    Class m_class;
    uint32_t m_gc_handle{0};
};

template<typename T>
Object::Object(T&& value)
{
    boxValue(std::forward<T>(value));
}

template<typename T>
bool Object::boxValue(T value)
{
    return boxValue(&value, CLASS_TYPE<T>);
}

template<typename T>
std::optional<T> Object::as() const
{
    return isValid() ? SCRIPT_TYPE<T>{*this}.value() : std::nullopt;
}

class ObjectAccessor
{
private:
    friend class Class;
    friend class InvokeResult;

    static Object createObject(MonoObject* object, MonoClass* klass = nullptr);
};

} // namespace GE::Script
