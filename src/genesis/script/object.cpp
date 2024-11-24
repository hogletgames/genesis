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

#include "object.h"
#include "class.h"
#include "method.h"

#include "genesis/core/log.h"

#include <mono/metadata/appdomain.h>
#include <mono/metadata/object.h>

namespace GE::Script {

Object::~Object()
{
    if (m_object != nullptr) {
        mono_gchandle_free(m_gc_handle);
    }
}

Method Object::method(std::string_view name, int param_count) const
{
    if (!isValid()) {
        GE_CORE_ERR("Trying to get method '{}' for invalid object", name);
        return {};
    }

    auto method = m_class.method(name, param_count);
    if (method.isValid() && method.isInstance()) {
        MethodAccessor::setMethodInstance(&method, m_object);
    }

    return method;
}

void* Object::unbox() const
{
    if (isValid()) {
        return mono_object_unbox(m_object);
    }

    GE_CORE_ERR("Trying to unbox invalid object");
    return nullptr;
}

Object::Object(MonoObject* object, MonoClass* klass)
    : m_object{object}
    , m_class{ClassAccessor::createClass(klass)}
{
    if (m_object == nullptr) {
        return;
    }

    if (!m_class.isValid()) {
        m_class = ClassAccessor::createClass(mono_object_get_class(m_object));
    }

    m_gc_handle = mono_gchandle_new(m_object, 0);
}

void Object::boxValue(void* value, ClassType class_type)
{
    auto* domain = mono_get_root_domain();
    if (domain == nullptr) {
        GE_CORE_ERR("Failed get root domain to box value");
        return;
    }

    auto* mono_class = toNativeClass(class_type);
    if (mono_class == nullptr) {
        GE_CORE_ERR("Failed to get Mono Class to box value of type {}", toString(class_type));
        return;
    }

    m_object = mono_value_box(mono_domain_get(), mono_class, value);
    m_class = ClassAccessor::createClass(mono_class);
}

Object ObjectAccessor::createObject(MonoObject* object, MonoClass* klass)
{
    return Object{object, klass};
}

} // namespace GE::Script
