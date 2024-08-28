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

#include "package.h"

namespace GE::Assets {
namespace {

template<typename T>
void appendIds(std::vector<ResourceID>* resource_ids,
               const std::unordered_map<std::string, Shared<T>>& resources)
{
    resource_ids->reserve(resource_ids->size() + resources.size());
    std::transform(resources.cbegin(), resources.cend(), std::back_inserter(*resource_ids),
                   [](const auto& item) { return item.second->id(); });
}

} // namespace

Package::Package(Package&& other) noexcept
    : m_name{std::move(other.m_name)}
    , m_filepath{std::move(other.m_filepath)}
    , m_pipelines{std::move(other.m_pipelines)}
    , m_meshes{std::move(other.m_meshes)}
    , m_textures{std::move(other.m_textures)}
{}

Package& Package::operator=(Package&& other) noexcept
{
    if (this != &other) {
        m_name = std::move(other.m_name);
        m_filepath = std::move(other.m_filepath);
        m_pipelines = std::move(other.m_pipelines);
        m_meshes = std::move(other.m_meshes);
        m_textures = std::move(other.m_textures);
    }

    return *this;
}

void Package::removeResource(const ResourceID& id)
{
    m_pipelines.erase(id.name());
    m_meshes.erase(id.name());
    m_textures.erase(id.name());
}

std::vector<ResourceID> Package::allResourceIDs() const
{
    std::vector<ResourceID> resource_ids;
    appendIds(&resource_ids, m_pipelines);
    appendIds(&resource_ids, m_meshes);
    appendIds(&resource_ids, m_textures);

    return resource_ids;
}

} // namespace GE::Assets
