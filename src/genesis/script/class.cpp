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

#include "class.h"
#include "method.h"
#include "object.h"

#include "genesis/core/log.h"

#include <mono/metadata/appdomain.h>
#include <mono/metadata/class.h>
#include <mono/metadata/metadata.h>
#include <mono/metadata/object.h>

namespace GE::Script {
namespace {
ClassType toType(int mono_type)
{
    switch (mono_type) {
        case MONO_TYPE_END: return ClassType::END;
        case MONO_TYPE_VOID: return ClassType::VOID;
        case MONO_TYPE_BOOLEAN: return ClassType::BOOLEAN;
        case MONO_TYPE_CHAR: return ClassType::CHAR;
        case MONO_TYPE_I1: return ClassType::I1;
        case MONO_TYPE_U1: return ClassType::U1;
        case MONO_TYPE_I2: return ClassType::I2;
        case MONO_TYPE_U2: return ClassType::U2;
        case MONO_TYPE_I4: return ClassType::I4;
        case MONO_TYPE_U4: return ClassType::U4;
        case MONO_TYPE_I8: return ClassType::I8;
        case MONO_TYPE_U8: return ClassType::U8;
        case MONO_TYPE_R4: return ClassType::R4;
        case MONO_TYPE_R8: return ClassType::R8;
        case MONO_TYPE_STRING: return ClassType::STRING;
        case MONO_TYPE_PTR: return ClassType::PTR;
        case MONO_TYPE_BYREF: return ClassType::BYREF;
        case MONO_TYPE_VALUETYPE: return ClassType::VALUETYPE;
        case MONO_TYPE_CLASS: return ClassType::CLASS;
        case MONO_TYPE_VAR: return ClassType::VAR;
        case MONO_TYPE_ARRAY: return ClassType::ARRAY;
        case MONO_TYPE_GENERICINST: return ClassType::GENERICINST;
        case MONO_TYPE_TYPEDBYREF: return ClassType::TYPEDBYREF;
        case MONO_TYPE_I: return ClassType::I;
        case MONO_TYPE_U: return ClassType::U;
        case MONO_TYPE_FNPTR: return ClassType::FNPTR;
        case MONO_TYPE_OBJECT: return ClassType::OBJECT;
        case MONO_TYPE_SZARRAY: return ClassType::SZARRAY;
        case MONO_TYPE_MVAR: return ClassType::MVAR;
        case MONO_TYPE_CMOD_REQD: return ClassType::CMOD_REQD;
        case MONO_TYPE_CMOD_OPT: return ClassType::CMOD_OPT;
        case MONO_TYPE_INTERNAL: return ClassType::INTERNAL;
        case MONO_TYPE_MODIFIER: return ClassType::MODIFIER;
        case MONO_TYPE_SENTINEL: return ClassType::SENTINEL;
        case MONO_TYPE_PINNED: return ClassType::PINNED;
        case MONO_TYPE_ENUM: return ClassType::ENUM;
        default: return ClassType::UNKNOWN;
    }
}

} // namespace

ClassType Class::type() const
{
    return toType(mono_type_get_type(mono_class_get_type(m_class)));
}

Method Class::method(std::string_view name, int param_count) const
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

    return Method{method};
}

Object Class::newObject() const
{
    auto* object = mono_object_new(mono_domain_get(), m_class);
    mono_runtime_object_init(object);
    return Object{object, m_class};
}

} // namespace GE::Script
