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

namespace GE::Assets {

Registry::Registry() = default;

Registry::~Registry() = default;

Registry::Registry(Registry&& other) noexcept
    : m_packages{std::move(other.m_packages)}
{}

Registry& Registry::operator=(Registry&& other) noexcept
{
    if (this != &other) {
        m_packages = std::move(other.m_packages);
    }

    return *this;
}

Package* Registry::emplacePackage(const std::string& name, const std::string& filepath)
{
    return &m_packages.emplace(name, Package{name, filepath}).first->second;
}

void Registry::insertPackage(Package&& package)
{
    auto name = package.name();
    m_packages.insert({std::move(name), std::move(package)});
}

void Registry::removePackage(const std::string& name)
{
    m_packages.erase(name);
}

void Registry::removeResource(const ResourceID& id)
{
    if (auto it = m_packages.find(id.package()); it != m_packages.end()) {
        it->second.removeResource(id);
    }
}

Package* Registry::package(const std::string& name)
{
    if (auto it = m_packages.find(name); it != m_packages.end()) {
        return &it->second;
    }

    return nullptr;
}

const Package* Registry::package(const std::string& name) const
{
    if (auto it = m_packages.find(name); it != m_packages.end()) {
        return &it->second;
    }

    return nullptr;
}

std::vector<const Package*> Registry::allPackages() const
{
    std::vector<const Package*> packages(m_packages.size());
    std::transform(m_packages.cbegin(), m_packages.cend(), packages.begin(),
                   [](const auto& package) { return &package.second; });
    return packages;
}

std::vector<ResourceID> Registry::allResourceIDs()
{
    std::vector<ResourceID> all_resource_ids;

    for (const auto& [package_name, package] : m_packages) {
        auto resource_ids = package.allResourceIDs();
        std::copy(all_resource_ids.begin(), all_resource_ids.end(),
                  std::back_inserter(all_resource_ids));
    }

    return all_resource_ids;
}

} // namespace GE::Assets
