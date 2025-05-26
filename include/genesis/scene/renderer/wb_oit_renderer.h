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

#include <genesis/core/memory.h>
#include <genesis/math/types.h>
#include <genesis/scene/renderer/renderer_base.h>

namespace GE {
class Framebuffer;
} // namespace GE

namespace GE::Assets {
class Registry;
} // namespace GE::Assets

namespace GE::Scene {

class Entity;

class WeightedBlendedOITRenderer: public RendererBase
{
public:
    WeightedBlendedOITRenderer(GE::Renderer*               renderer,
                               const Assets::Registry&     assets,
                               const ViewProjectionCamera* camera);

    void render(const Scene& scene) override;
    std::string_view type() const override { return TYPE; }

    static constexpr std::string_view TYPE = "Weighted-Blended OIT Scene Renderer";

private:
    void recreateWbOitFramebuffer(const Vec2& size);
    void createOpaqueColorPipeline(GE::Renderer* renderer, const Assets::Registry& assets);
    void createAccumulationPipeline(GE::Renderer* renderer, const Assets::Registry& assets);
    void createComposingPipeline(GE::Renderer* renderer, const Assets::Registry& assets);

    void renderOpaqueEntities(GE::Renderer* renderer, const Scene& scene);
    void renderTransparentEntities(GE::Renderer* renderer, const Scene& scene);
    void renderPhysicsColliders(const Scene& scene);
    void composeScene(GE::Renderer* renderer);

    Scoped<Framebuffer> m_wb_oit_fbo;
    Shared<Pipeline>    m_color_pipeline;
    Shared<Pipeline>    m_accumulation_pipeline;
    Shared<Pipeline>    m_composing_pipeline;
};

} // namespace GE::Scene
