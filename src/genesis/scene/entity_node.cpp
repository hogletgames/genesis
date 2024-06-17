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

#include "entity_node.h"

#include "components/relationship_components.h"
#include "scene.h"

#include "genesis/core/asserts.h"

namespace GE::Scene {
namespace {

// NOLINTNEXTLINE(misc-no-recursion)
void destroyChildren(Scene* scene, const EntityNode& entity)
{
    if (entity.isNull()) {
        return;
    }

    GE_CORE_ASSERT(!entity.parentNode().isNull(), "Child entity must have a parent");

    destroyChildren(scene, entity.childNode());
    destroyChildren(scene, entity.nextNode());
    scene->destroyEntity(entity.entity());
}

} // namespace

EntityNode::EntityNode(const Entity& entity)
    : m_entity{entity}
{}

EntityNode& EntityNode::insert(const Entity& entity)
{
    GE_CORE_ASSERT(!isNull(), "Entity cannot be Null");
    GE_CORE_ASSERT(m_entity != entity, "Cannot append entity to itself");

    EntityNode inserted_entity{entity};
    inserted_entity.eject();

    inserted_entity.node().prev_node = m_entity.nativeHandle();
    inserted_entity.node().next_node = node().next_node;
    inserted_entity.node().parent_node = node().parent_node;

    if (hasNextNode()) {
        nextNode().node().prev_node = entity.nativeHandle();
    }

    node().next_node = entity.nativeHandle();
    moveTailToNextNode();

    return *this;
}

EntityNode& EntityNode::appendChild(const Entity& child_entity)
{
    GE_CORE_ASSERT(!isNull(), "Parent entity cannot be Null");
    GE_CORE_ASSERT(!child_entity.isNull(), "Child entity cannot be Null");
    GE_CORE_ASSERT(m_entity != child_entity, "Cannot make an entity a child of itself");

    if (hasChildNode()) {
        lastChild().insert(child_entity);
        return *this;
    }

    EntityNode child_node{child_entity};
    child_node.eject();
    child_node.node().parent_node = m_entity.nativeHandle();
    node().child_node = child_entity.nativeHandle();
    return *this;
}

EntityNode EntityNode::prevNode() const
{
    return !isNull() ? makeEntity(node().prev_node) : EntityNode{};
}

EntityNode EntityNode::nextNode() const
{
    return !isNull() ? makeEntity(node().next_node) : EntityNode{};
}

EntityNode EntityNode::childNode() const
{
    return !isNull() ? makeEntity(node().child_node) : EntityNode{};
}

EntityNode EntityNode::parentNode() const
{
    return !isNull() ? makeEntity(node().parent_node) : EntityNode{};
}

EntityNode EntityNode::lastChild() const
{
    auto child_node = childNode();

    while (child_node.hasNextNode()) {
        child_node = child_node.nextNode();
    }

    return child_node;
}

bool EntityNode::isNull() const
{
    return m_entity.isNull();
}

bool EntityNode::isHead() const
{
    return !isNull() ? m_entity.has<HeadNodeComponent>() : false;
}

bool EntityNode::isTail() const
{
    return !isNull() ? m_entity.has<TailNodeComponent>() : false;
}

bool EntityNode::hasPrevNode() const
{
    return !isNull() ? node().prev_node != Entity::NULL_ID : false;
}

bool EntityNode::hasNextNode() const
{
    return !isNull() ? node().next_node != Entity::NULL_ID : false;
}

bool EntityNode::hasChildNode() const
{
    return !isNull() ? node().child_node != Entity::NULL_ID : false;
}

bool EntityNode::hasParentNode() const
{
    return !isNull() ? node().parent_node != Entity::NULL_ID : false;
}

bool EntityNode::hasChild(const Entity& child) const
{
    if (isNull()) {
        return false;
    }

    for (auto parent_node = EntityNode{child}.parentNode(); !parent_node.isNull();
         parent_node = parent_node.parentNode()) {
        if (parent_node.entity() == m_entity) {
            return true;
        }
    }

    return false;
}

void EntityNode::destoryEntityWithChildren(Scene* scene)
{
    GE_CORE_ASSERT(!isNull(), "Entity cannot be Null");

    eject();
    destroyChildren(scene, childNode());
    scene->destroyEntity(m_entity);
    m_entity = Entity();
}

void EntityNode::moveHeadToNextNode()
{
    GE_CORE_ASSERT(!isNull(), "Entity cannot be Null");

    if (isHead() && hasNextNode()) {
        nextNode().entity().add<HeadNodeComponent>();
        m_entity.remove<HeadNodeComponent>();
    }
}

void EntityNode::moveTailToPrevNode()
{
    GE_CORE_ASSERT(!isNull(), "Entity cannot be Null");

    if (isTail() && hasPrevNode()) {
        prevNode().entity().add<TailNodeComponent>();
        m_entity.remove<TailNodeComponent>();
    }
}

void EntityNode::moveTailToNextNode()
{
    GE_CORE_ASSERT(!isNull(), "Entity cannot be Null");

    if (isTail() && hasNextNode()) {
        nextNode().entity().add<TailNodeComponent>();
        m_entity.remove<TailNodeComponent>();
    }
}

void EntityNode::eject()
{
    GE_CORE_ASSERT(!isNull(), "Entity cannot be Null");

    moveHeadToNextNode();
    moveTailToPrevNode();

    if (hasParentNode() && parentNode().node().child_node == m_entity.nativeHandle()) {
        parentNode().node().child_node = node().next_node;
    }

    if (hasPrevNode()) {
        prevNode().node().next_node = node().next_node;
    }

    if (hasNextNode()) {
        nextNode().node().prev_node = node().prev_node;
    }

    node().prev_node = Entity::NULL_ID;
    node().next_node = Entity::NULL_ID;
    node().parent_node = Entity::NULL_ID;
}

NodeComponent& EntityNode::node()
{
    GE_CORE_ASSERT(!isNull(), "Entity cannot be Null");
    return m_entity.get<NodeComponent>();
}

const NodeComponent& EntityNode::node() const
{
    GE_CORE_ASSERT(!isNull(), "Entity cannot be Null");
    return m_entity.get<NodeComponent>();
}

EntityNode EntityNode::makeEntity(Entity::NativeHandle entity_handle) const
{
    return EntityNode{Entity::Factory::createWithRegistryOfEntity(m_entity, entity_handle)};
}

} // namespace GE::Scene
