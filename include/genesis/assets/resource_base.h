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

#include <genesis/assets/iresource.h>

namespace GE::Assets {

class GE_API ResourceBase: public IResource
{
public:
    ResourceBase(const ResourceBase& other);
    ResourceBase(ResourceBase&& other) noexcept;
    ResourceBase& operator=(const ResourceBase& other);
    ResourceBase& operator=(ResourceBase&& other) noexcept;

    const ResourceID& id() const override { return m_id; };

protected:
    ResourceBase() = default;
    ~ResourceBase() = default;

    explicit ResourceBase(ResourceID id);

private:
    ResourceID m_id;
};

inline ResourceBase::ResourceBase(const ResourceBase& other)
    : m_id{other.m_id}
{}

inline ResourceBase::ResourceBase(ResourceBase&& other) noexcept
    : m_id{std::move(other.m_id)}
{}

inline ResourceBase& ResourceBase::operator=(const ResourceBase& other)
{
    if (this != &other) {
        m_id = other.m_id;
    }

    return *this;
}

inline ResourceBase& ResourceBase::operator=(ResourceBase&& other) noexcept
{
    if (this != &other) {
        m_id = std::move(other.m_id);
    }

    return *this;
}

inline ResourceBase::ResourceBase(ResourceID id)
    : m_id{std::move(id)}
{}

} // namespace GE::Assets
