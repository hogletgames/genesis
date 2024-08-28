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

#include <genesis/core/export.h>
#include <genesis/core/memory.h>
#include <genesis/math/types.h>

namespace GE {
class Framebuffer;
class Pipeline;
class StagingBuffer;
class Texture;
} // namespace GE

namespace GE::Assets {
class Registry;
} // namespace GE::Assets

namespace GE::Scene {

class Entity;
class Scene;
class ViewProjectionCamera;

class GE_API EntityPicker
{
public:
    EntityPicker(Scene* scene, const Assets::Registry& assets, const ViewProjectionCamera* camera);
    ~EntityPicker();

    void onRender();
    void onViewportUpdate(const Vec2& viewport);

    Entity getEntityByPosition(const Vec2& position);

    static constexpr int32_t ENTITY_ID_NONE{-1};

private:
    void recreateEntityIdFramebuffer(const Vec2& size);
    void createEntityIdPipeline(const Assets::Registry& assets);

    void renderEntityId(const Entity& entity);

    Scene* m_scene{nullptr};
    const ViewProjectionCamera* m_camera{nullptr};
    Scoped<Framebuffer> m_entity_id_fbo;
    Scoped<Pipeline> m_entity_id_pipeline;
    Scoped<StagingBuffer> m_entity_id_buffer;
    bool m_is_buffer_updated{false};
};

} // namespace GE::Scene
