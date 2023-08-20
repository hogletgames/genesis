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
#include "iresource.h"

namespace GE::Assets {

Registry::Registry() = default;

Registry::~Registry() = default;

void Registry::add(Scoped<IResource> resource)
{
    auto uuid = resource->id();
    m_resources.emplace(uuid, std::move(resource));
}

void Registry::remove(const ResourceID &uuid)
{
    m_resources.erase(uuid);
}

void Registry::visit(const ResourceID &id, ResourceVisitor *visitor)
{
    if (auto it = m_resources.find(id); it != m_resources.end()) {
        it->second->accept(visitor);
    }
}

void Registry::visitAll(ResourceVisitor *visitor)
{
    for (auto &[_, resource] : m_resources) {
        resource->accept(visitor);
    }
}

Registry::ResourceIDs Registry::ids() const
{
    ResourceIDs ids(m_resources.size());
    std::transform(m_resources.begin(), m_resources.end(), ids.begin(),
                   [](const auto &resource) { return resource.first; });
    return ids;
}

} // namespace GE::Assets
