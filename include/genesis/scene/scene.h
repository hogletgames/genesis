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

#include <genesis/core/export.h>
#include <genesis/scene/registry.h>

#include <string>

namespace GE::Scene {

class EntityNode;

class GE_API Scene
{
public:
    using ForeachCallback = Registry::ForeachCallback;
    using ForeachConstCallback = Registry::ForeachConstCallback;

    Scene() = default;
    ~Scene() = default;

    Scene(const Scene& other) = delete;
    Scene& operator=(const Scene& other) = delete;

    Scene(Scene&& other) noexcept;
    Scene& operator=(Scene&& other) noexcept;

    Entity createEntity(std::string_view name = {});
    Entity entity(Entity::NativeHandle entity_handle);
    void destroyEntity(const Entity& entity);
    void destroyEntity(Entity::NativeHandle entity_handle);
    void clear();

    Entity headEntity() const;
    Entity tailEnity() const;

    const Entity& mainCamera() const { return m_main_camera; }
    void setMainCamera(const Entity& camera) { m_main_camera = camera; }

    const std::string& name() const { return m_name; }
    void setName(std::string_view name) { m_name = name; }

    template<typename... Args>
    void forEach(const ForeachCallback& callback);
    void forEachEntity(const ForeachCallback& callback);

    template<typename... Args>
    void forEach(const ForeachConstCallback& callback) const;
    void forEachEntity(const ForeachConstCallback& callback) const;

    static constexpr uint32_t SERIALIZATION_VERSION{1};

private:
    std::string m_name;
    Registry m_registry;
    Entity m_main_camera;
};

template<typename... Args>
void Scene::forEach(const Scene::ForeachCallback& callback)
{
    m_registry.eachEntityWith<Args...>(callback);
}

template<typename... Args>
void Scene::forEach(const Scene::ForeachConstCallback& callback) const
{
    m_registry.eachEntityWith<Args...>(callback);
}

} // namespace GE::Scene
