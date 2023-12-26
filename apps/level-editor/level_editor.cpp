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

#include "level_editor.h"
#include "application_info.h"
#include "gui/level_editor_gui.h"
#include "settings.h"

#include "genesis/app.h"
#include "genesis/assets.h"
#include "genesis/core.h"
#include "genesis/filesystem.h"
#include "genesis/graphics.h"
#include "genesis/gui.h"
#include "genesis/scene.h"

using namespace GE::GUI;

namespace LE {
namespace {

const auto SETTINGS_FILE =
    GE::FS::joinPath(GE::FS::cacheDir(LEVEL_EDITOR_APP_NAME), "settings.yaml");

} // namespace

LevelEditor::LevelEditor() = default;

LevelEditor::~LevelEditor()
{
    shutdown();
}

bool LevelEditor::initialize()
{
    loadSettings();

    if (!createFramebuffer()) {
        GE_ERR("Failed to create scene framebuffer");
        return false;
    }

    m_scene_renderer =
        GE::makeScoped<GE::Scene::Renderer>(m_ctx.cameraController()->camera().get());
    m_gui = GE::makeScoped<LevelEditorGUI>(&m_ctx);
    connectSignals();
    initializeProject();
    return true;
}

void LevelEditor::shutdown()
{
    saveSettings();
}

void LevelEditor::onUpdate(GE::Timestamp ts)
{
    m_gui->onUpdate(ts);
}

void LevelEditor::onEvent(GE::Event* event)
{
    m_gui->onEvent(event);
}

void LevelEditor::onRender()
{
    auto* renderer = m_ctx.sceneFbo()->renderer();
    auto* scene = m_ctx.scene();

    updateParameters();

    renderer->beginFrame();
    m_scene_renderer->render(renderer, *scene);
    renderer->endFrame();

    m_gui->onRender();
}

bool LevelEditor::createFramebuffer()
{
    GE::Framebuffer::config_t model_fbo_config{};
    model_fbo_config.clear_color = {0.3f, 0.3f, 0.3f, 1.0f};
    model_fbo_config.size = {720.0f, 480.0f};
    model_fbo_config.msaa_samples = GE::Graphics::limits().max_msaa;

    m_ctx.sceneFbo() = GE::Framebuffer::create(model_fbo_config);
    return m_ctx.sceneFbo() != nullptr;
}

void LevelEditor::connectSignals()
{
    m_gui->viewportSizeSignal()->connect([this](const auto& viewport) { m_viewport = viewport; });
    m_gui->loadAssetSignal()->connect([this] { onLoadAssets(); });
    m_gui->saveAssetSignal()->connect([this] { onSaveAssets(); });
    m_gui->loadSceneSignal()->connect([this] { onLoadScene(); });
    m_gui->saveSceneSignal()->connect([this] { onSaveScene(); });
    m_gui->loadProjectSignal()->connect([this] { onLoadProject(); });
    m_gui->loadRecentProjectSignal()->connect(
        [this](const auto& filepath) { loadProject(filepath); });
    m_gui->saveProjectSignal()->connect([this](const auto& filepath) { saveProject(filepath); });
    m_gui->saveProjectAsSignal()->connect([this] { onSaveProject(); });
}

void LevelEditor::initializeProject()
{
    const auto* project = m_ctx.settings()->currentProject();

    if (const auto& assets = project->assetsPath(); !assets.empty()) {
        loadAssets(assets);
    }

    GE::Assets::PipelineInitializer initializer{m_ctx.sceneFbo()->renderer()};
    m_ctx.assets()->visitAll(&initializer);

    if (const auto& scene = project->scenePath(); !scene.empty()) {
        loadScene(scene);
    }

    GE::Application::window()->setTitle(project->name());
}

void LevelEditor::updateParameters()
{
    if (m_ctx.sceneFbo()->size() != m_viewport) {
        m_ctx.sceneFbo()->resize(m_viewport);
        m_ctx.cameraController()->camera()->setViewport(m_viewport);
    }
}

void LevelEditor::loadSettings()
{
    if (!GE::FS::exists(SETTINGS_FILE)) {
        return;
    }

    if (SettingsDeserializer deserializer{m_ctx.settings()};
        !deserializer.deserialize(SETTINGS_FILE)) {
        GE_ERR("Failed to deserialize settings from the file '{}'", SETTINGS_FILE);
        return;
    }

    if (m_ctx.settings()->currentProject()->name().empty()) {
        initializeProject();
    }
}

bool LevelEditor::saveSettings()
{
    if (auto settings_dir = GE::FS::parentPath(SETTINGS_FILE); !GE::FS::exists(settings_dir)) {
        GE::FS::createDir(settings_dir);
    }

    SettingsSerializer serializer{m_ctx.settings()};
    return serializer.serialize(SETTINGS_FILE);
}

bool LevelEditor::loadAssets(std::string_view filepath)
{
    GE::Assets::ResourceDeserializer deserializer{m_ctx.assets()};
    deserializer.deserialize(filepath.data());
    m_ctx.settings()->currentProject()->setAssetsPath(filepath.data());
    return true;
}

bool LevelEditor::saveAssets(std::string_view filepath)
{
    GE::Assets::ResourceSerializer serializer{m_ctx.assets()};
    serializer.serialize(filepath.data());
    m_ctx.settings()->currentProject()->setAssetsPath(filepath.data());
    return true;
}

bool LevelEditor::loadScene(std::string_view filepath)
{
    GE::Scene::SceneDeserializer deserializer{m_ctx.scene(), m_ctx.assets()};
    deserializer.deserialize(filepath.data());
    m_ctx.settings()->currentProject()->setScenePath(filepath.data());
    return true;
}

bool LevelEditor::saveScene(std::string_view filepath)
{
    GE::Scene::SceneSerializer serializer{m_ctx.scene()};
    serializer.serialize(filepath.data());
    m_ctx.settings()->currentProject()->setScenePath(filepath);
    return true;
}

bool LevelEditor::loadProject(std::string_view filepath)
{
    auto project = ProjectSettings::fromFile(filepath.data());

    if (!project.has_value()) {
        return false;
    }

    m_ctx.settings()->setProjectPath(project->name(), filepath.data());
    m_ctx.settings()->setCurrentProject(project->name());
    initializeProject();
    return true;
}

bool LevelEditor::saveProject(std::string_view filepath)
{
    auto* project = m_ctx.settings()->currentProject();
    project->setName(GE::FS::stem(filepath));
    project->setProjectPath(filepath);
    m_ctx.settings()->setProjectPath(project->name(), project->projectPath());

    return project->saveToFile(filepath.data()) && saveAssets(project->assetsPath()) &&
           saveScene(project->scenePath());
}

void LevelEditor::onLoadAssets()
{
    if (auto filepath = openSingleFile("assets"); !filepath.empty()) {
        loadAssets(filepath);
        initializeProject();
    }
}

void LevelEditor::onSaveAssets()
{
    if (auto filepath = saveFile("assets"); !filepath.empty()) {
        saveAssets(filepath);
    }
}

void LevelEditor::onLoadScene()
{
    if (auto filepath = openSingleFile("scene"); !filepath.empty()) {
        loadScene(filepath);
    }
}

void LevelEditor::onSaveScene()
{
    if (auto filepath = saveFile("scene"); !filepath.empty()) {
        saveScene(filepath);
    }
}

void LevelEditor::onLoadProject()
{
    if (auto filepath = openSingleFile("project"); !filepath.empty()) {
        loadProject(filepath);
    }
}

void LevelEditor::onSaveProject()
{
    if (auto filepath = saveFile("project"); !filepath.empty()) {
        saveProject(filepath);
    }
}

} // namespace LE
