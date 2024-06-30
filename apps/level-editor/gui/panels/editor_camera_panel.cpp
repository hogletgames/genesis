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

#include "editor_camera_panel.h"

#include "genesis/filesystem/file.h"
#include "genesis/filesystem/filepath.h"
#include "genesis/filesystem/known_folders.h"
#include "genesis/gui/widgets.h"
#include "genesis/math/trigonometric.h"
#include "genesis/scene/camera/view_projection_camera.h"
#include "genesis/scene/executor.h"
#include "genesis/scene/scene_deserializer.h"
#include "genesis/scene/scene_serializer.h"

using namespace GE::GUI;

namespace LE {
namespace {

std::string serializedScenePath(const GE::Scene::Scene &scene)
{
    return GE::FS::joinPath(GE::FS::cacheDir("genesis"), scene.name()) + ".scene";
}

} // namespace

EditorCameraPanel::EditorCameraPanel(LevelEditorContext *ctx)
    : WindowBase{NAME}
    , m_ctx{ctx}
{}

void EditorCameraPanel::onRender()
{
    WidgetNode node{&m_window};
    drawView(&node);
    drawProjectionCombo(&node);
    drawProjectionOptions(&node);
    drawReadOnlyOptions(&node);
    drawSceneExecutor(&node);
}

void EditorCameraPanel::drawView(WidgetNode *node)
{
    if (auto position = camera()->position();
        node->call<::ValueEditor>("Position", &position, 0.1f, -100.0f, 100.0f)) {
        camera()->setPosition(position);
    }

    if (auto rotation = GE::degrees(camera()->rotation());
        node->call<::ValueEditor>("Rotation", &rotation, 0.1f, -360.0f, 360.0f)) {
        camera()->setRotation(GE::radians(rotation));
    }
}

void EditorCameraPanel::drawProjectionCombo(WidgetNode *node)
{
    static const std::vector<std::string> PROJECTIONS = {
        GE::toString(GE::Scene::ViewProjectionCamera::ORTHOGRAPHIC),
        GE::toString(GE::Scene::ViewProjectionCamera::PERSPECTIVE),
    };

    auto current_projection = GE::toString(camera()->type());
    ComboBox combo{"Projection type", PROJECTIONS, current_projection};
    node->subNode(&combo);

    if (combo.selectedItem() != current_projection) {
        camera()->setType(GE::Scene::toProjectionType(combo.selectedItem()));
    }
}

void EditorCameraPanel::drawPerspectiveProjection(WidgetNode *node)
{
    auto [fov, near, far] = camera()->perspectiveOptions();
    node->call<::ValueEditor>("Field of view", &fov, 0.1f, 0.0f, 180.0f);
    node->call<::ValueEditor>("Near", &near, 0.1f, 0.0f, 100.0f);
    node->call<::ValueEditor>("Far", &far, 0.1f, 0.0f, 100.0f);

    camera()->setPerspectiveOptions({fov, near, far});
}

void EditorCameraPanel::drawOrthoProjection(WidgetNode *node)
{
    auto [size, near, far] = camera()->orthographicOptions();
    node->call<::ValueEditor>("Size", &size, 0.1f, 0.0f, 10.0f);
    node->call<::ValueEditor>("Near", &near, 0.1f, 0.0f, 100.f);
    node->call<::ValueEditor>("Far", &far, 0.1f, 0.0f, 100.0f);

    camera()->setOrthoOptions({size, near, far});
}

void EditorCameraPanel::drawProjectionOptions(WidgetNode *node)
{
    switch (camera()->type()) {
        case GE::Scene::ViewProjectionCamera::PERSPECTIVE: drawPerspectiveProjection(node); break;
        case GE::Scene::ViewProjectionCamera::ORTHOGRAPHIC: drawOrthoProjection(node); break;
        default: break;
    }
}

void EditorCameraPanel::drawReadOnlyOptions(WidgetNode *node)
{
    node->call<Text>("Direction: %s", GE::toString(camera()->direction()).c_str());
}

void EditorCameraPanel::drawSceneExecutor(WidgetNode *node)
{
    static const std::vector<std::string> EXECUTORS = {
        GE::Scene::DummyExecutor::TYPE.data(),
        GE::Scene::Runtime2DExecutor::TYPE.data(),
    };

    auto &executor = m_ctx->sceneExecutor();

    node->call<Text>("Scene executor:");
    ComboBox executor_types{"Executor type", EXECUTORS, executor->type()};
    node->subNode(&executor_types);

    if (auto new_executor_type = executor_types.selectedItem();
        new_executor_type != executor->type()) {
        if (new_executor_type == GE::Scene::DummyExecutor::TYPE) {
            loadSerializedScene();
        } else if (new_executor_type == GE::Scene::Runtime2DExecutor::TYPE) {
            saveScene();
        }

        GE::Scene::ExecutorFactory factory{m_ctx->scene(), m_ctx->world().get()};
        executor = factory.create(new_executor_type);
    }

    if (bool is_paused = executor->isPaused(); node->call<Checkbox>("Paused", &is_paused)) {
        if (is_paused) {
            executor->pause();
        } else {
            executor->resume();
        }
    }
}

void EditorCameraPanel::saveScene()
{
    m_saved_scene_path = serializedScenePath(*m_ctx->scene());
    GE::FS::createDir(GE::FS::parentPath(m_saved_scene_path));

    GE::Scene::SceneSerializer serializer{m_ctx->scene()};
    serializer.serialize(m_saved_scene_path);
}

void EditorCameraPanel::loadSerializedScene()
{
    if (m_saved_scene_path.empty()) {
        return;
    }

    GE::Scene::SceneDeserializer deserializer{m_ctx->scene(), m_ctx->assets()};
    deserializer.deserialize(m_saved_scene_path);

    GE::FS::removeAll(m_saved_scene_path);
    m_saved_scene_path.clear();
}

} // namespace LE
