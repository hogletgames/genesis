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

#include <genesis/assets/mesh_resource.h>
#include <genesis/assets/registry.h>
#include <genesis/assets/texture_resource.h>

namespace GE::Scene {

struct GE_API SpriteComponent {
    Shared<Texture> texture;
    Shared<Mesh> mesh;
    Vec3 color{0.0f, 0.0f, 0.0f};

    static constexpr std::string_view NAME{"Sprite"};

    bool isValid() const { return texture && mesh; }

    const Assets::ResourceID &textureID() const { return m_texture_id; }
    const Assets::ResourceID &meshID() const { return m_mesh_id; }

    void setTextureID(Assets::ResourceID id) { m_texture_id = std::move(id); }
    void setMeshID(Assets::ResourceID id) { m_mesh_id = std::move(id); }

    bool loadTexture(Assets::Registry *assets);
    bool loadMesh(Assets::Registry *assets);
    bool loadAll(Assets::Registry *assets);

private:
    Assets::ResourceID m_texture_id;
    Assets::ResourceID m_mesh_id;
};

inline bool SpriteComponent::loadTexture(Assets::Registry *assets)
{
    if (const auto &resource = assets->get<Assets::TextureResource>(m_texture_id);
        resource != nullptr) {
        texture = resource->texture();
        return true;
    }

    return false;
}

inline bool SpriteComponent::loadMesh(Assets::Registry *assets)
{
    if (const auto &resource = assets->get<Assets::MeshResource>(m_mesh_id); resource != nullptr) {
        mesh = resource->mesh();
        return true;
    }

    return false;
}

inline bool SpriteComponent::loadAll(Assets::Registry *assets)
{
    return loadTexture(assets) && loadMesh(assets);
}

} // namespace GE::Scene
