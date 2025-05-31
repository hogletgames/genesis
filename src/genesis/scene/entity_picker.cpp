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

#include "entity_picker.h"
#include "camera/view_projection_camera.h"
#include "components.h"
#include "entity.h"
#include "renderer/renderer_base.h"
#include "scene.h"

#include "genesis/assets/resource_id.h"
#include "genesis/graphics/framebuffer.h"
#include "genesis/graphics/pipeline_config.h"
#include "genesis/graphics/shader.h"
#include "genesis/graphics/staging_buffer.h"

namespace GE::Scene {
namespace {

const Assets::ResourceID ENTITY_ID_RESOURCE_ID{"genesis", Assets::Group::PIPELINES, "entity_id"};

constexpr int32_t toInt32(Entity::NativeHandle entityHandle)
{
    return static_cast<int32_t>(entityHandle);
}

inline bool isPositionInBounds(const Vec2& position, const Vec2& bounds)
{
    return position.x >= 0 && position.x <= bounds.x && position.y >= 0 && position.y <= bounds.y;
}

} // namespace

EntityPicker::EntityPicker(Scene*                      scene,
                           const Assets::Registry&     assets,
                           const ViewProjectionCamera* camera)
    : m_scene{scene}
    , m_camera{camera}
    , m_entity_id_buffer{StagingBuffer::create()}
{
    recreateEntityIdFramebuffer(camera->viewport());
    createEntityIdPipeline(assets);
}

EntityPicker::~EntityPicker() = default;

void EntityPicker::onRender()
{
    auto* renderer = m_entity_id_fbo->renderer();
    renderer->beginFrame(Renderer::CLEAR_ALL);
    m_scene->forEach<SpriteComponent>([this](const auto& entity) { renderEntityId(entity); });
    renderer->endFrame();
    renderer->swapBuffers();

    m_is_buffer_updated = false;
}

void EntityPicker::onViewportUpdate(const Vec2& viewport)
{
    if (m_entity_id_fbo->size() != viewport) {
        recreateEntityIdFramebuffer(viewport);
    }
}

Entity EntityPicker::getEntityByPosition(const Vec2& position)
{
    auto texture_size = m_entity_id_fbo->colorTexture(0).size();

    if (!isPositionInBounds(position, texture_size)) {
        GE_CORE_ERR("Incorrect mouse position, mouse position={}, texture size={}",
                    GE::toString(position), GE::toString(texture_size));
        return {};
    }

    if (!m_is_buffer_updated) {
        m_entity_id_fbo->colorTexture(0).copyTo(m_entity_id_buffer.get());
        m_is_buffer_updated = true;
    }

    void* texture = m_entity_id_buffer->data();
    if (texture == nullptr) {
        GE_CORE_ERR("Failed to get texture data");
        return {};
    }

    Vec2 inverted_position = {position.x, texture_size.y - position.y - 1};

    const auto* pixels = static_cast<const int32_t*>(texture);
    size_t      index = (inverted_position.y * texture_size.x) + inverted_position.x;
    int32_t     entity_id = pixels[index];

    if (entity_id == ENTITY_ID_NONE) {
        return {};
    }

    return m_scene->entity(static_cast<Entity::NativeHandle>(entity_id));
}

void EntityPicker::recreateEntityIdFramebuffer(const Vec2& size)
{
    Framebuffer::config_t config{};
    config.size = size;
    config.msaa_samples = 1;
    config.attachments = {
        // Entity ID attachment
        {.type = fb_attachment_t::Type::COLOR,
         .texture_type = TextureType::TEXTURE_2D,
         .texture_format = TextureFormat::R32_INT,
         .clear_color = IVec4{-1}},
        // Depth attachment
        {.type = fb_attachment_t::Type::DEPTH,
         .texture_type = TextureType::TEXTURE_2D,
         .texture_format = TextureFormat::D32F,
         .clear_depth = 1.0f},
    };

    m_entity_id_fbo = Framebuffer::create(config);
}

void EntityPicker::createEntityIdPipeline(const Assets::Registry& assets)
{
    auto pipeline_resource = assets.get<Assets::PipelineResource>(ENTITY_ID_RESOURCE_ID);
    GE_CORE_ASSERT(pipeline_resource, "Failed to find entity ID pipeline resource '{}'",
                   ENTITY_ID_RESOURCE_ID.asString());

    blending_t color_blending{};
    color_blending.enabled = false;

    pipeline_config_t config{};
    config.blending = color_blending;
    config.depth_test_enable = true;
    config.depth_write_enable = true;

    m_entity_id_pipeline = pipeline_resource->createPipeline(m_entity_id_fbo->renderer(), config);
    GE_CORE_ASSERT(m_entity_id_pipeline, "Failed to create entity ID pipeline");
}

void EntityPicker::renderEntityId(const Entity& entity)
{
    auto* pipeline = m_entity_id_pipeline.get();
    auto* mesh = entity.get<SpriteComponent>().mesh.get();
    auto  mvp = m_camera->viewProjection() * parentTransform(entity) *
               entity.get<TransformComponent>().transform();

    auto* cmd = m_entity_id_fbo->renderer()->command();
    cmd->bind(pipeline);
    cmd->pushConstant(pipeline, "pc.mvp", mvp);
    cmd->pushConstant(pipeline, "pc.entityId", toInt32(entity.nativeHandle()));
    cmd->draw(*mesh);
}

} // namespace GE::Scene
