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

#include "settings/settings.h"

#include <genesis/assets/registry.h>
#include <genesis/core/memory.h>
#include <genesis/scene/camera/view_projection_camera.h>
#include <genesis/scene/camera/vp_camera_controller.h>
#include <genesis/scene/entity_picker.h>
#include <genesis/scene/renderer.h>
#include <genesis/scene/scene.h>

namespace GE {
class Framebuffer;
} // namespace GE

namespace LE {

class GE_API LevelEditorContext
{
public:
    Settings* settings() { return m_settings.get(); }
    GE::Scoped<GE::Framebuffer>& sceneFbo() { return m_scene_fbo; }
    GE::Scoped<GE::Scene::IRenderer>& sceneRenderer() { return m_scene_renderer; }
    GE::Assets::Registry* assets() { return &m_assets; }
    GE::Scene::Scene* scene() { return &m_scene; }
    GE::Scene::VPCameraController* cameraController() { return &m_camera_controller; }
    GE::Scene::Entity* selectedEntity() { return &m_selected_entity; }
    GE::Scene::EntityPicker* entityPicker() { return &m_entity_picker; }

    void resetSelectedEntity() { m_selected_entity = {}; }

private:
    GE::Scoped<Settings> m_settings{GE::makeScoped<Settings>()};
    GE::Scoped<GE::Framebuffer> m_scene_fbo;
    GE::Scoped<GE::Scene::IRenderer> m_scene_renderer;
    GE::Assets::Registry m_assets;
    GE::Scene::Scene m_scene;
    GE::Shared<GE::Scene::ViewProjectionCamera> m_camera{
        GE::makeShared<GE::Scene::ViewProjectionCamera>()};
    GE::Scene::VPCameraController m_camera_controller{m_camera};
    GE::Scene::Entity m_selected_entity;
    GE::Scene::EntityPicker m_entity_picker{&m_scene, m_camera.get()};
};

} // namespace LE
