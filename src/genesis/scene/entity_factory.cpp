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

#include "entity_factory.h"
#include "components.h"
#include "entity_node.h"
#include "scene.h"

#include "genesis/assets/registry.h"

namespace GE::Scene {

EntityFactory::EntityFactory(Scene* scene, Assets::Registry* assets)
    : m_scene{scene}
    , m_assets{assets}
{}

Entity EntityFactory::createCamera(std::string_view name)
{
    auto entity = m_scene->createEntity(name);
    entity.add<CameraComponent>();

    appentToTail(entity);
    return entity;
}

Entity EntityFactory::createSquare(std::string_view name)
{
    auto entity = m_scene->createEntity(name);

    auto& sprite = entity.add<SpriteComponent>();
    sprite.setMeshID({"genesis", "meshes", "square"});
    sprite.setTextureID({"genesis", "textures", "square"});
    sprite.loadAll(m_assets);

    auto& material = entity.add<MaterialComponent>();
    material.setMaterialID({"genesis", "materials", "sprite"});
    material.loadMaterial(m_assets);

    appentToTail(entity);
    return entity;
}

Entity EntityFactory::createCircle(std::string_view name)
{
    auto entity = m_scene->createEntity(name);

    auto& sprite = entity.add<SpriteComponent>();
    sprite.setMeshID({"genesis", "meshes", "circle"});
    sprite.setTextureID({"genesis", "textures", "circle"});
    sprite.loadAll(m_assets);

    auto& material = entity.add<MaterialComponent>();
    material.setMaterialID({"genesis", "materials", "sprite"});
    material.loadMaterial(m_assets);

    appentToTail(entity);
    return entity;
}

Entity EntityFactory::createEmptyEntity(std::string_view name)
{
    auto entity = m_scene->createEntity(name);
    appentToTail(entity);
    return entity;
}

void EntityFactory::appentToTail(const Entity& entity)
{
    if (auto tail_entity = m_scene->tailEnity(); tail_entity != entity) {
        EntityNode{tail_entity}.insert(entity);
    }
}

} // namespace GE::Scene
