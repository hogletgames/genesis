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

#include <genesis/core/export.h>
#include <genesis/core/format.h>
#include <genesis/core/hash.h>

#include <functional>
#include <string>

namespace GE::Assets {

class GE_API ResourceID
{
public:
    ResourceID() = default;
    ResourceID(const ResourceID& other) { copy(other); }
    ResourceID(ResourceID&& other) noexcept { move(other); }
    ResourceID(std::string package, std::string group, std::string name);

    ~ResourceID() = default;

    ResourceID& operator=(const ResourceID& other);
    ResourceID& operator=(ResourceID&& other) noexcept;

    const std::string& package() const { return m_package; }
    const std::string& group() const { return m_group; }
    const std::string& name() const { return m_name; }

    std::string* package() { return &m_package; }
    std::string* group() { return &m_group; }
    std::string* name() { return &m_name; }

    std::string id() const { return GE_FMTSTR("{}.{}.{}", m_package, m_group, m_name); }

private:
    void copy(const ResourceID& other);
    void move(ResourceID& other);

    std::string m_package;
    std::string m_group;
    std::string m_name;
};

inline ResourceID::ResourceID(std::string package, std::string group, std::string name)
    : m_package{std::move(package)}
    , m_group{std::move(group)}
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

inline void ResourceID::copy(const ResourceID& other)
{
    m_package = other.m_package;
    m_group = other.m_group;
    m_name = other.m_name;
}

inline void ResourceID::move(ResourceID& other)
{
    m_package = std::move(other.m_package);
    m_group = std::move(other.m_group);
    m_name = std::move(other.m_name);
}

inline bool operator<(const ResourceID& lhs, const ResourceID& rhs)
{
    auto to_tuple = [](const ResourceID& id) {
        return std::tie(id.package(), id.group(), id.name());
    };
    return to_tuple(lhs) < to_tuple(rhs);
}

inline bool operator==(const ResourceID& lhs, const ResourceID& rhs)
{
    return !(lhs < rhs) && !(rhs < lhs);
}

inline bool operator!=(const ResourceID& lhs, const ResourceID& rhs)
{
    return !(lhs == rhs);
}

} // namespace GE::Assets

namespace std {

template<>
struct hash<GE::Assets::ResourceID> {
    size_t operator()(const GE::Assets::ResourceID& id) const
    {
        return GE::combinedHash(id.package(), id.group(), id.name());
    }
};

} // namespace std
