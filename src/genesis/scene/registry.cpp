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

#include "registry.h"
#include "entity.h"

namespace GE::Scene {

Entity Registry::create()
{
    return toEntity(m_registry.create());
}

Entity Registry::entity(EntityHandle entity_id)
{
    if (m_registry.valid(entity_id)) {
        return toEntity(entity_id);
    }

    return {};
}

void Registry::destroy(const Entity& entity)
{
    m_registry.destroy(entity.nativeHandle());
}

void Registry::clear()
{
    m_registry.clear();
}

void Registry::eachEntity(const ForeachCallback& callback)
{
    for (auto entity : m_registry.storage<EntityHandle>().each()) {
        callback(toEntity(std::get<0>(entity)));
    }
}

void Registry::eachEntity(const ForeachCallback& callback) const
{
    for (auto entity : m_registry.storage<EntityHandle>().each()) {
        callback(toEntity(std::get<0>(entity)));
    }
}

Entity Registry::toEntity(EntityHandle entity) const
{
    return Entity::Factory::create(entity, &m_registry);
}

} // namespace GE::Scene
