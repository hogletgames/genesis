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

#include <genesis/scene/components/relationship_components.h>
#include <genesis/scene/entity.h>

namespace GE::Scene {

class Scene;

class GE_API EntityNode
{
public:
    EntityNode() = default;
    explicit EntityNode(const Entity& entity);

    EntityNode insert(const Entity& entity);
    EntityNode appendChild(const Entity& child_entity);

    EntityNode prevNode() const;
    EntityNode nextNode() const;
    EntityNode childNode() const;
    EntityNode parentNode() const;

    EntityNode lastChild() const;

    bool isNull() const;
    bool isHead() const;
    bool isTail() const;

    bool hasPrevNode() const;
    bool hasNextNode() const;
    bool hasChildNode() const;
    bool hasParentNode() const;

    bool hasChild(const Entity& child) const;

    Entity& entity() { return m_entity; }
    const Entity& entity() const { return m_entity; }

    void destoryEntityWithChildren(Scene* scene);

private:
    void moveHeadToNextNode();
    void moveTailToPrevNode();
    void moveTailToNextNode();

    void eject();

    NodeComponent& node();
    const NodeComponent& node() const;

    EntityNode makeEntity(Entity::NativeHandle entity_handle) const;

    Entity m_entity;
};

} // namespace GE::Scene
