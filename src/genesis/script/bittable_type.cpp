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

#include "bittable_type.h"
#include "domain.h"
#include "object.h"

#include "genesis/core/enum.h"
#include "genesis/core/log.h"

#include <mono/metadata/object.h>

namespace GE::Script {

BaseBittableType::~BaseBittableType()
{
    if (isValid()) {
        mono_gchandle_free(m_gc_handle);
    }
}

Object BaseBittableType::asObject() const
{
    return Object{m_object};
}

BaseBittableType::BaseBittableType(const Object& object)
{
    if (!object.isValid()) {
        GE_CORE_ERR("Trying to create a bittalbe type from an invalid object");
        return;
    }

    m_object = mono_object_clone(object.nativeHandle());
    m_gc_handle = mono_gchandle_new(m_object, false);
}

BaseBittableType::BaseBittableType(void* value, ClassType type)
{
    auto* mono_class = toNativeClass(type);
    if (mono_class == nullptr) {
        GE_CORE_ERR("Failed to get MonoClass to box value of '{}' type", toString(type));
        return;
    }

    m_object = mono_value_box(Domain::currentDomain().nativeHandle(), mono_class, value);
    m_gc_handle = mono_gchandle_new(m_object, false);
}

void* BaseBittableType::unboxObject(ClassType type) const
{
    auto object = asObject();

    if (!object.isValid()) {
        GE_CORE_ERR("Unboxing invalid object");
        return nullptr;
    }

    if (object.type() != type) {
        GE_CORE_ERR("Incorrect type of object, expected: '{}', actual: '{}'", toString(type),
                    toString(object.type()));
        return nullptr;
    }

    return object.unbox();
}

} // namespace GE::Script
