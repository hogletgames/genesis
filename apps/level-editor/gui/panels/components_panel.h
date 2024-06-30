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

#include <genesis/gui/window/window_base.h>

namespace GE::GUI {
class WidgetNode;
} // namespace GE::GUI

namespace GE::P2D {
struct body_shape_config_base_t;
} // namespace GE::P2D

namespace GE::Scene {
class Entity;
struct BoxCollider2DComponent;
struct CameraComponent;
struct MaterialComponent;
struct CircleCollider2DComponent;
struct RigidBody2DComponent;
struct SpriteComponent;
struct TagComponent;
struct TransformComponent;
} // namespace GE::Scene

namespace LE {

class LevelEditorContext;

class GE_API ComponentsPanel: public GE::GUI::WindowBase
{
public:
    explicit ComponentsPanel(LevelEditorContext* ctx);

    void onRender() override;

    static constexpr auto NAME{"Components"};

private:
    void drawEntityComponents(GE::Scene::Entity* entity);
    void drawAddNewComponents(GE::GUI::WidgetNode* node, GE::Scene::Entity* entity);

    template<typename Component>
    void draw(GE::Scene::Entity* entity);
    void draw(GE::GUI::WidgetNode* node, GE::Scene::CameraComponent* camera);
    void draw(GE::GUI::WidgetNode* node, GE::Scene::MaterialComponent* material);
    void draw(GE::GUI::WidgetNode* node, GE::Scene::TagComponent* tag);
    void draw(GE::GUI::WidgetNode* node, GE::Scene::TransformComponent* transform);
    void draw(GE::GUI::WidgetNode* node, GE::Scene::SpriteComponent* sprite);
    void draw(GE::GUI::WidgetNode* node, GE::Scene::RigidBody2DComponent* rigid_body);
    void draw(GE::GUI::WidgetNode* node, GE::P2D::body_shape_config_base_t* shape_config);
    void draw(GE::GUI::WidgetNode* node, GE::Scene::BoxCollider2DComponent* collider);
    void draw(GE::GUI::WidgetNode* node, GE::Scene::CircleCollider2DComponent* collider);

    LevelEditorContext* m_ctx{nullptr};
};

} // namespace LE
