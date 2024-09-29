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
        GE_CORE_ERR("Trying to get method '{}' from invalid object", name);
        return Method{};
    }

    auto* method = mono_class_get_method_from_name(m_class, name.data(), param_count);
    if (method == nullptr) {
        GE_CORE_ERR("Method '{}' not found", name);
        return Method{};
    }

    return Method{method, m_object};
}

Class Object::getClass() const
{
    return Class{m_class};
}

ClassType Object::type() const
{
    return Class{m_class}.type();
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
    , m_class{klass}
{
    if (m_object != nullptr && m_class == nullptr) {
        m_class = mono_object_get_class(m_object);
        m_gc_handle = mono_gchandle_new(m_object, 0);
    }
}

} // namespace GE::Script
