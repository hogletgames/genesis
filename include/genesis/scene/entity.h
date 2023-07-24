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

#include <genesis/core/asserts.h>
#include <genesis/core/export.h>

#include <entt/entity/registry.hpp>

namespace GE::Scene {

class Registry;

class GE_API Entity
{
public:
    using NativeHandle = entt::entity;

    Entity() = default;

    template<typename T>
    bool has() const
    {
        return m_registry->all_of<T>(m_handle);
    }

    template<typename T, typename... Args>
    T& add(Args&&... args)
    {
        GE_CORE_ASSERT(!has<T>(), "Entity already has this component");
        return m_registry->emplace<T>(m_handle, std::forward<Args>(args)...);
    }

    template<typename T>
    void remove()
    {
        GE_CORE_ASSERT(has<T>(), "Unable to remove non-existent component");
        m_registry->remove<T>(m_handle);
    }

    template<typename T>
    T& get()
    {
        GE_CORE_ASSERT(has<T>(), "Unable to get non-existent component");
        return m_registry->get<T>(m_handle);
    }

    template<typename T>
    const T& get() const
    {
        GE_CORE_ASSERT(has<T>(), "Unable to get non-existent component");
        return m_registry->get<T>(m_handle);
    }

    bool isNull() const { return m_handle == NULL_ID; }
    NativeHandle nativeHandle() const { return m_handle; }

    friend constexpr bool operator==(const Entity& lhs, const Entity& rhs);
    friend constexpr bool operator!=(const Entity& lhs, const Entity& rhs);

private:
    friend Registry;

    Entity(NativeHandle native_handle, entt::registry* registry);

    static constexpr auto NULL_ID{entt::null};

    NativeHandle m_handle{NULL_ID};
    entt::registry* m_registry{nullptr};
};

constexpr bool operator==(const Entity& lhs, const Entity& rhs)
{
    return lhs.m_handle == rhs.m_handle && lhs.m_registry == rhs.m_registry;
}

constexpr bool operator!=(const Entity& lhs, const Entity& rhs)
{
    return !(lhs == rhs);
}

} // namespace GE::Scene
