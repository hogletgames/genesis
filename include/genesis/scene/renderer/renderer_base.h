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

#include <genesis/graphics/pipeline.h>
#include <genesis/graphics/primitives_renderer.h>
#include <genesis/scene/pipeline_library.h>
#include <genesis/scene/renderer/irenderer.h>

namespace GE {
class Mesh;
class Pipeline;
class Texture;
} // namespace GE

namespace GE::Scene {

class Entity;
class ViewProjectionCamera;

class RendererBase: public IRenderer
{
public:
    RendererBase(GE::Renderer* renderer, const ViewProjectionCamera* camera);

protected:
    void renderEntity(GE::Renderer* renderer, Pipeline* pipeline, const Entity& entity);

    void renderPhysics2DColliders(const Entity& entity);
    void renderCircleCollider2D(const Entity& entity);
    void renderBoxCollider2D(const Entity& entit);

    bool isValid(std::string_view entity_name, Pipeline* material, Texture* texture,
                 Mesh* mesh) const;

    GE::Renderer* m_renderer{nullptr};
    PipelineLibrary m_pipeline_library;
    PrimitivesRenderer m_primitives_renderer;
    const ViewProjectionCamera* m_camera{nullptr};
};

Mat4 parentTransform(const Entity& entity);

} // namespace GE::Scene
