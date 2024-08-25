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

#include <genesis/assets/mesh_resource.h>
#include <genesis/assets/pipeline_resource.h>
#include <genesis/assets/resource_id.h>
#include <genesis/assets/texture_resource.h>
#include <genesis/core/export.h>
#include <genesis/core/memory.h>

#include <string>
#include <vector>

namespace GE::Assets {

class GE_API Package
{
public:
    Package(std::string name, std::string filepath)
        : m_name{std::move(name)}
        , m_filepath{std::move(filepath)}
    {}

    ~Package() = default;
    Package(const Package& other) = delete;
    Package& operator=(const Package& other) = delete;

    Package(Package&& other) noexcept;
    Package& operator=(Package&& other) noexcept;

    const std::string& name() const { return m_name; }
    const std::string& filepath() const { return m_filepath; }

    template<typename T>
    Shared<T> get(const ResourceID& id) const;

    void removeResource(const ResourceID& id);

    template<typename T>
    std::vector<Shared<T>> getAllOf() const;

    std::vector<ResourceID> allResourceIDs() const;

    template<typename T>
    Shared<T> createResource(const typename T::config_t& config);

private:
    std::string m_name;
    std::string m_filepath;

    std::unordered_map<std::string, Shared<PipelineResource>> m_pipelines;
    std::unordered_map<std::string, Shared<MeshResource>> m_meshes;
    std::unordered_map<std::string, Shared<TextureResource>> m_textures;
};

template<typename T>
Shared<T> Package::get(const ResourceID& id) const
{
    if (m_name != id.package() || T::GROUP != id.group()) {
        return nullptr;
    }

    if constexpr (T::GROUP == Group::PIPELINES) {
        return m_pipelines.count(id.name()) ? m_pipelines.at(id.name()) : nullptr;
    } else if constexpr (T::GROUP == Group::MESHES) {
        return m_meshes.count(id.name()) ? m_meshes.at(id.name()) : nullptr;
    } else if constexpr (T::GROUP == Group::TEXTURES) {
        return m_textures.count(id.name()) ? m_textures.at(id.name()) : nullptr;
    } else {
        static_assert(!std::is_same_v<T, T>, "Invalid resource group");
    }

    return nullptr;
}

template<typename T>
std::vector<Shared<T>> Package::getAllOf() const
{
    std::vector<Shared<T>> resources;
    auto get_value = [](const auto& item) { return item.second; };

    if constexpr (T::GROUP == Group::PIPELINES) {
        resources.resize(m_pipelines.size());
        std::transform(m_pipelines.cbegin(), m_pipelines.cend(), resources.begin(), get_value);
    } else if constexpr (T::GROUP == Group::MESHES) {
        resources.resize(m_meshes.size());
        std::transform(m_meshes.cbegin(), m_meshes.cend(), resources.begin(), get_value);
    } else if constexpr (T::GROUP == Group::TEXTURES) {
        resources.resize(m_textures.size());
        std::transform(m_textures.cbegin(), m_textures.cend(), resources.begin(), get_value);
    } else {
        static_assert(!std::is_same_v<T, T>, "Invalid resource group");
    }

    return resources;
}

template<typename T>
Shared<T> Package::createResource(const typename T::config_t& config)
{
    if (config.name.empty()) {
        return nullptr;
    }

    auto resource = T::Factory::create(m_name, config);
    if (!resource) {
        return nullptr;
    }

    if constexpr (T::GROUP == Group::PIPELINES) {
        return m_pipelines.emplace(config.name, resource).first->second;
    } else if constexpr (T::GROUP == Group::MESHES) {
        return m_meshes.emplace(config.name, resource).first->second;
    } else if constexpr (T::GROUP == Group::TEXTURES) {
        return m_textures.emplace(config.name, resource).first->second;
    } else {
        static_assert(!std::is_same_v<T, T>, "Invalid resource group");
    }

    return nullptr;
}

} // namespace GE::Assets
