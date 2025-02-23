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

#include <genesis/core/enum.h>
#include <genesis/core/export.h>

typedef struct _MonoClass MonoClass;

namespace GE::Script {

enum class ClassType
{
    UNKNOWN = 0,
    VOID,
    BOOLEAN,
    CHAR,
    I1,
    U1,
    I2,
    U2,
    I4,
    U4,
    I8,
    U8,
    R4,
    R8,
    STRING,
    PTR,
    BYREF,
    VALUETYPE,
    CLASS,
    VAR,
    ARRAY,
    GENERICINST,
    TYPEDBYREF,
    I,
    U,
    FNPTR,
    OBJECT,
    SZARRAY,
    MVAR,
    CMOD_REQD,
    CMOD_OPT,
    INTERNAL,
    MODIFIER,
    SENTINEL,
    PINNED,
    ENUM
};

GE_API ClassType toClassType(int mono_type);
GE_API MonoClass* toNativeClass(ClassType class_type);

} // namespace GE::Script

template<>
struct fmt::formatter<GE::Script::ClassType> {
    constexpr auto parse(format_parse_context& ctx) const { return ctx.begin(); }

    template<typename FmtContext>
    constexpr auto format(const GE::Script::ClassType& class_type, FmtContext& ctx) const
    {
        return format_to(ctx.out(), "{}", GE::toString(class_type));
    }
};
