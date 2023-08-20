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

#include <genesis/assets/resource_id.h>
#include <genesis/assets/resource_pointer_visitor.h>
#include <genesis/core/memory.h>

#include <unordered_map>

namespace GE::Assets {

class ResourceVisitor;
class IResource;

class GE_API Registry
{
public:
    using ResourceIDs = std::vector<ResourceID>;

    Registry();
    ~Registry();

    void add(Scoped<IResource> resource);
    void remove(const ResourceID& uuid);

    template<typename T>
    const T* get(const ResourceID& id) const;
    template<typename T>
    T* get(const ResourceID& id);
    template<typename T>
    ResourceIDs getAll();

    void visit(const ResourceID& id, ResourceVisitor* visitor);
    void visitAll(ResourceVisitor* visitor);

    ResourceIDs ids() const;

private:
    std::unordered_map<ResourceID, Scoped<IResource>> m_resources;
};

template<typename T>
const T* Registry::get(const ResourceID& id) const
{
    ResourcePointerVisitor<T> visitor;
    visit(id, &visitor);
    return visitor.get();
}

template<typename T>
T* Registry::get(const ResourceID& id)
{
    ResourcePointerVisitor<T> visitor;
    visit(id, &visitor);
    return visitor.get();
}

template<typename T>
Registry::ResourceIDs Registry::getAll()
{
    struct Visitor: ResourceVisitor {
        ResourceIDs resources;
        void visit(T* resource) { resources.push_back(resource->id()); }
    } visitor;

    visitAll(&visitor);
    return visitor.resources;
}

} // namespace GE::Assets
