/*
 * BSD 3-Clause License
 *
 * Copyright (c) 2023, Dmitry Shilnenkov
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
#include <genesis/core/format.h>
#include <genesis/core/hash.h>

#include <functional>
#include <string>

namespace GE::Assets {

enum class Group : uint8_t
{
    UNKNOWN,
    PIPELINES,
    MESHES,
    TEXTURES
};

class GE_API ResourceID
{
public:
    ResourceID() = default;
    ResourceID(const ResourceID& other) { copy(other); }
    ResourceID(ResourceID&& other) noexcept { move(other); }
    ResourceID(std::string package, Group group, std::string name);

    ~ResourceID() = default;

    ResourceID& operator=(const ResourceID& other);
    ResourceID& operator=(ResourceID&& other) noexcept;
    auto operator<=>(const ResourceID& other) const = default;

    const std::string& package() const { return m_package; }
    Group group() const { return m_group; }
    const std::string& name() const { return m_name; }

    std::string* package() { return &m_package; }
    Group* group() { return &m_group; }
    std::string* name() { return &m_name; }

    std::string asString() const;

private:
    void copy(const ResourceID& other);
    void move(ResourceID& other);

    std::string m_package;
    Group m_group{Group::UNKNOWN};
    std::string m_name;
};

inline ResourceID::ResourceID(std::string package, Group group, std::string name)
    : m_package{std::move(package)}
    , m_group{group}
    , m_name{std::move(name)}
{}

inline ResourceID& ResourceID::operator=(const ResourceID& other)
{
    if (this != &other) {
        copy(other);
    }

    return *this;
}

inline ResourceID& ResourceID::operator=(ResourceID&& other) noexcept
{
    if (this != &other) {
        move(other);
    }

    return *this;
}

inline std::string ResourceID::asString() const
{
    return GE_FMTSTR("{}.{}.{}", m_package, GE::toString(m_group), m_name);
}

inline void ResourceID::copy(const ResourceID& other)
{
    m_package = other.m_package;
    m_group = other.m_group;
    m_name = other.m_name;
}

inline void ResourceID::move(ResourceID& other)
{
    m_package = std::move(other.m_package);
    m_group = other.m_group;
    m_name = std::move(other.m_name);
}

} // namespace GE::Assets

template<>
struct std::hash<GE::Assets::ResourceID> {
    size_t operator()(const GE::Assets::ResourceID& id) const noexcept
    {
        return GE::combinedHash(id.package(), id.group(), id.name());
    }
};
