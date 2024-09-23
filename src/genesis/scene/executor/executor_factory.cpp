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

#include "executor/executor_factory.h"
#include "executor/dummy_executor.h"
#include "executor/runtime2d_executor.h"
#include "scene.h"
#include "scene_deserializer.h"
#include "scene_serializer.h"

#include "genesis/core/log.h"
#include "genesis/filesystem/file.h"
#include "genesis/filesystem/file_content.h"
#include "genesis/filesystem/filepath.h"
#include "genesis/filesystem/known_folders.h"

#include <unordered_map>

namespace GE::Scene {
namespace {

std::string serializedScenePath(const Scene& scene)
{
    return FS::joinPath(FS::cacheDir("genesis"), scene.name()) + ".scene";
}

} // namespace

ExecutorFactory::ExecutorFactory(Scene* scene, Assets::Registry* assets, P2D::World* world)
    : m_scene{scene}
    , m_assets{assets}
    , m_world(world)
{
    m_factory_methods = {
        {DummyExecutor::TYPE, &makeScoped<DummyExecutor>},
        {Runtime2DExecutor::TYPE,
         [this] { return makeScoped<Runtime2DExecutor>(m_scene, m_world); }},
    };
}

Scoped<IExecutor> ExecutorFactory::create(std::string_view type)
{
    if (const auto& factory = m_factory_methods.find(type); factory != m_factory_methods.end()) {
        return factory->second();
    }

    GE_CORE_WARN("Failed to get a factory for '{}' scene executor type", type);
    return makeScoped<DummyExecutor>();
}

bool ExecutorFactory::saveScene()
{
    m_saved_scene_path = serializedScenePath(*m_scene);
    FS::createDir(FS::parentPath(m_saved_scene_path));

    SceneSerializer serializer{m_scene};
    return serializer.serialize(m_saved_scene_path);
}

bool ExecutorFactory::restoreScene()
{
    if (m_saved_scene_path.empty()) {
        return false;
    }

    if (SceneDeserializer deserializer{m_scene, m_assets};
        deserializer.deserialize(m_saved_scene_path)) {
        return false;
    }

    FS::removeAll(m_saved_scene_path);
    m_saved_scene_path.clear();
    return true;
}

} // namespace GE::Scene
