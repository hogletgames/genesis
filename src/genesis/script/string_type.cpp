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

#include "string_type.h"
#include "class.h"
#include "object.h"

#include "genesis/core/enum.h"
#include "genesis/core/log.h"

#include <mono/metadata/appdomain.h>
#include <mono/metadata/object.h>

namespace GE::Script {

StringType::StringType(const Object& object)
{
    if (!object.isValid()) {
        GE_CORE_ERR("Trying to create a string from an invalid object");
        return;
    }

    if (auto type = object.type(); type != ClassType::STRING) {
        GE_CORE_ERR("Trying to create a string from object with type={}", toString(type));
        return;
    }

    m_string = reinterpret_cast<MonoString*>(object.nativeHandle());
    updateGCHandle(m_string);
}

StringType::StringType(std::string_view string)
{
    m_string = mono_string_new(mono_domain_get(), string.data());
    updateGCHandle(m_string);
}

StringType::~StringType()
{
    if (m_string != nullptr) {
        mono_gchandle_free(m_gc_handle);
    }
}

std::optional<std::string> StringType::value() const
{
    char* string = mono_string_to_utf8(m_string);
    if (string == nullptr) {
        GE_CORE_ERR("Failed to convert string to UTF-8");
        return {};
    }

    size_t string_length = mono_string_length(m_string);
    std::string result{string, string_length};
    mono_free(string);

    return result;
}

void StringType::updateGCHandle(MonoString* mono_string)
{
    m_gc_handle = mono_gchandle_new(reinterpret_cast<MonoObject*>(mono_string), 0);
}

} // namespace GE::Script
