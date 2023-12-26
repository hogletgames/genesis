/*
 * BSD 3-Clause License
 *
 * Copyright (c) 2022, Dmitry Shilnenkov
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

#include <string>
#include <tuple>
#include <vector>

namespace GE {

struct GE_API resource_descriptor_t {
    enum Type : uint8_t
    {
        UNKNOWN = 0,
        UNIFORM_BUFFER,
        COMBINED_IMAGE_SAMPLER
    };

    std::string name;
    Type type{UNKNOWN};
    uint32_t set{0};
    uint32_t binding{0};
    uint32_t count{0};
};

struct GE_API push_constant_t {
    std::string name;
    uint32_t offset{0};
    uint32_t size{0};
    uint32_t pipeline_stages{0};
};

constexpr bool operator==(const resource_descriptor_t& lhs, const resource_descriptor_t& rhs)
{
    return std::tie(lhs.name, lhs.type, lhs.set, lhs.binding, lhs.count) ==
           std::tie(rhs.name, rhs.type, rhs.set, rhs.binding, rhs.count);
}

constexpr bool operator==(const push_constant_t& lhs, const push_constant_t& rhs)
{
    return std::tie(lhs.name, lhs.offset, lhs.size) == std::tie(rhs.name, rhs.offset, rhs.size);
}

using ResourceDescriptors = std::vector<resource_descriptor_t>;
using PushConstants = std::vector<push_constant_t>;

} // namespace GE
