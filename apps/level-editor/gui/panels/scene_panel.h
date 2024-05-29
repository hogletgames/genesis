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

#include <genesis/core/deferred_commands.h>
#include <genesis/gui/window/window_base.h>
#include <genesis/scene/entity.h>

namespace GE::GUI {
class WidgetNode;
} // namespace GE::GUI

namespace GE::Scene {
class EntityNode;
} // namespace GE::Scene

namespace LE {

class LevelEditorContext;

class DeferredScenePanelCommands: public GE::DeferredCommands<void()>
{
public:
    DeferredScenePanelCommands(LevelEditorContext* ctx)
        : m_ctx{ctx}
    {}

    void selectEntity(const GE::Scene::Entity& entity);
    void removeEntity(const GE::Scene::Entity& entity);
    void appendEntity(const GE::Scene::Entity& dst, const GE::Scene::Entity& src);
    void appendToChildren(const GE::Scene::Entity& dst, const GE::Scene::Entity& src);

private:
    LevelEditorContext* m_ctx{nullptr};
};

class GE_API ScenePanel: public GE::GUI::WindowBase
{
public:
    using DeferredCmdQueue = std::deque<std::function<void()>>;

    explicit ScenePanel(LevelEditorContext* ctx);

    void onRender() override;

    static constexpr auto NAME{"Scene"};

private:
    void drawScene(GE::GUI::WidgetNode* node);

    void drawEntities(GE::GUI::WidgetNode* node, const GE::Scene::EntityNode& entity);
    void drawEntity(GE::GUI::WidgetNode* node, const GE::Scene::Entity& entity);
    void drawEntityDragDrop(const GE::Scene::Entity& entity);
    void drawContextMenu(GE::GUI::WidgetNode* node);

    void resetDragDropEntities();

    LevelEditorContext* m_ctx{nullptr};
    DeferredScenePanelCommands m_commands;

    bool m_is_select_entity_handled{false};
    GE::Scene::Entity m_drag_drop_src_entity;
    GE::Scene::Entity m_drag_drop_dst_entity;
};

} // namespace LE
