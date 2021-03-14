/*
 * BSD 3-Clause License
 *
 * Copyright (c) 2021, Dmitry Shilnenkov
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

#ifndef GENESIS_CORE_ENUM_H_
#define GENESIS_CORE_ENUM_H_

#include <magic_enum.hpp>

#define GE_EXTEND_ENUM_RANGE(enum_type, min_value, max_value)  \
    namespace magic_enum::customize {                          \
    template<>                                                 \
    struct enum_range<enum_type> {                             \
        static constexpr int min{static_cast<int>(min_value)}; \
        static constexpr int max{static_cast<int>(max_value)}; \
    };                                                         \
    } // namespace magic_enum::customize

namespace GE {

using magic_enum::bitwise_operators::operator~;
using magic_enum::bitwise_operators::operator|;
using magic_enum::bitwise_operators::operator&;
using magic_enum::bitwise_operators::operator^;
using magic_enum::bitwise_operators::operator|=;
using magic_enum::bitwise_operators::operator&=;
using magic_enum::bitwise_operators::operator^=;

template<typename EnumType,
         typename = std::enable_if<std::is_enum<EnumType>::value, bool>>
std::string toString(EnumType value)
{
    return std::string{magic_enum::enum_name(value)};
}

template<typename EnumType>
std::optional<EnumType> toEnum(const std::string& string)
{
    return magic_enum::enum_cast<EnumType>(string);
}

template<typename OStream, typename EnumType,
         typename = std::enable_if<std::is_enum<EnumType>::value, bool>>
OStream& operator<<(OStream& os, EnumType value)
{
    return os << toString(value);
}

} // namespace GE

#endif // GENESIS_CORE_ENUM_H_
