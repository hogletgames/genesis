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

#include <genesis/assets/package.h>
#include <genesis/assets/resource_id.h>
#include <genesis/core/memory.h>

#include <unordered_map>

namespace GE::Assets {

class ResourceVisitor;
class IResource;

class GE_API Registry
{
public:
    Registry();
    ~Registry();

    Registry(const Registry& other) = delete;
    Registry& operator=(const Registry& other) = delete;

    Registry(Registry&& other) noexcept;
    Registry& operator=(Registry&& other) noexcept;

    Package* emplacePackage(const std::string& name, const std::string& filepath);
    void insertPackage(Package&& package);

    void removePackage(const std::string& name);
    void removeResource(const ResourceID& id);

    Package* package(const std::string& name);
    const Package* package(const std::string& name) const;

    template<typename T>
    Shared<T> get(const ResourceID& id) const;

    template<typename T>
    std::vector<Shared<T>> getAllOf() const;

    std::vector<const Package*> allPackages() const;
    std::vector<ResourceID> allResourceIDs();

private:
    std::unordered_map<std::string, Package> m_packages;
};

template<typename T>
Shared<T> Registry::get(const ResourceID& id) const
{
    if (auto package = m_packages.find(id.package()); package != m_packages.end()) {
        return package->second.template get<T>(id);
    }

    return {};
}

template<typename T>
std::vector<Shared<T>> Registry::getAllOf() const
{
    std::vector<Shared<T>> all_resources;

    for (const auto& [package_name, package] : m_packages) {
        auto resources = package.template getAllOf<T>();
        std::copy(resources.cbegin(), resources.cend(), std::back_inserter(all_resources));
    }

    return all_resources;
}

} // namespace GE::Assets
