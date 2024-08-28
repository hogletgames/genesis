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

#include <genesis/assets/registry.h>
#include <genesis/assets/resource_id.h>
#include <genesis/core/deferred_commands.h>
#include <genesis/gui/window/window_base.h>

#include <boost/signals2/signal.hpp>

namespace GE::Assets {
class MeshResource;
class Package;
class PipelineResource;
class Registry;
class TextureResource;
} // namespace GE::Assets

namespace GE::GUI {
class WidgetNode;
} // namespace GE::GUI

namespace LE {

class DeferredAssetsPanelCommands: public GE::DeferredCommands<void()>
{
public:
    explicit DeferredAssetsPanelCommands(GE::Assets::Registry* assets)
        : m_assets{assets}
    {}

    void removePackage(const std::string& name);
    void removeResource(const GE::Assets::ResourceID& id);

private:
    GE::Assets::Registry* m_assets{nullptr};
};

class GE_API AssetsPanel: public GE::GUI::WindowBase
{
public:
    using AddResourceSignal = boost::signals2::signal<void()>;

    explicit AssetsPanel(GE::Assets::Registry* assets);

    void onRender() override;

    AddResourceSignal* addMeshResourceSignal() { return &m_add_mesh_resource_signal; }
    AddResourceSignal* addPipelineResourceSignal() { return &m_add_pipeline_resource_signal; }
    AddResourceSignal* addTextureResourceSignal() { return &m_add_texture_resource_signal; }

    static constexpr auto NAME{"Assets"};

private:
    void updateWindowParameters();

    void drawContextMenu(GE::GUI::WidgetNode* node);
    void drawAssets(GE::GUI::WidgetNode* node);
    void drawPackage(GE::GUI::WidgetNode* node, const GE::Assets::Package& package);
    void drawResource(GE::GUI::WidgetNode* node,
                      const GE::Shared<GE::Assets::MeshResource>& resource);
    void drawResource(GE::GUI::WidgetNode* node,
                      const GE::Shared<GE::Assets::PipelineResource>& resource);
    void drawResource(GE::GUI::WidgetNode* node,
                      const GE::Shared<GE::Assets::TextureResource>& resource);

    template<typename T>
    void drawResources(GE::GUI::WidgetNode* node, const GE::Assets::Package& package);

    GE::Assets::Registry* m_assets{nullptr};
    GE::Vec2 m_window_size{0.0f, 0.0f};

    DeferredAssetsPanelCommands m_commands;

    AddResourceSignal m_add_mesh_resource_signal;
    AddResourceSignal m_add_pipeline_resource_signal;
    AddResourceSignal m_add_texture_resource_signal;
};

} // namespace LE
