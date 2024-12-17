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

#include "assets_panel.h"

#include "genesis/assets/mesh_resource.h"
#include "genesis/assets/pipeline_resource.h"
#include "genesis/assets/texture_resource.h"
#include "genesis/core/format.h"
#include "genesis/filesystem/file_content.h"
#include "genesis/gui/widgets.h"

using namespace GE::Assets;
using namespace GE::GUI;

namespace LE {
namespace {

constexpr TreeNode::Flags TREE_NODE_FLAGS{TreeNode::SPAN_AVAIL_WIDTH | TreeNode::FRAME_PADDING};

GE::Vec2 scaledTextureSize(const GE::Vec2 &texture_size, float window_width)
{
    float scale = window_width / texture_size.x;
    return texture_size * scale;
}

} // namespace

void DeferredAssetsPanelCommands::removePackage(const std::string &name)
{
    enqueue([this, name] { m_assets->removePackage(name); });
}

void DeferredAssetsPanelCommands::removeResource(const ResourceID &id)
{
    enqueue([this, id] { m_assets->removeResource(id); });
}

AssetsPanel::AssetsPanel(Registry *assets)
    : WindowBase{NAME}
    , m_assets{assets}
    , m_commands(assets)
{}

void AssetsPanel::onRender()
{
    WidgetNode window_node{&m_window};
    window_node.call(&AssetsPanel::updateWindowParameters, this);
    drawAssets(&window_node);
    drawContextMenu(&window_node);

    m_commands.submit();
}

void AssetsPanel::updateWindowParameters()
{
    m_window_size = m_window.availableRegion();
}

void AssetsPanel::drawContextMenu(WidgetNode *node)
{
    auto flags = PopupFlag::MOUSE_BUTTON_RIGHT | PopupFlag::NO_OPEN_OVER_ITEMS |
                 PopupFlag::NO_OPEN_OVER_EXISTING_POPUP;

    auto popup_context = node->makeSubNode<PopupContextWindow>(std::string_view{}, flags);
    auto add_resource_menu = popup_context.makeSubNode<Menu>("Add resource");

    if (add_resource_menu.call<MenuItem>("Mesh")) {
        m_add_mesh_resource_signal();
    }
    if (add_resource_menu.call<MenuItem>("Pipeline")) {
        m_add_pipeline_resource_signal();
    }
    if (add_resource_menu.call<MenuItem>("Texture")) {
        m_add_texture_resource_signal();
    }
}

void AssetsPanel::drawAssets(WidgetNode *node)
{
    if (!node->isOpened()) {
        return;
    }

    auto packages = m_assets->allPackages();
    std::ranges::sort(packages,
                      [](const auto *lhs, const auto *rhs) { return lhs->name() < rhs->name(); });

    for (const auto *package : packages) {
        drawPackage(node, *package);
    }
}

void AssetsPanel::drawPackage(WidgetNode *node, const Package &package)
{
    auto package_node = node->makeSubNode<TreeNode>(package.name(), TREE_NODE_FLAGS);

    if (auto popup_context = WidgetNode::create<PopupContextItem>();
        popup_context.call<MenuItem>(GE_FMTSTR("Remove '{}'", package.name()))) {
        m_commands.removePackage(package.name());
    }

    if (package_node.isOpened()) {
        drawResources<MeshResource>(&package_node, package);
        drawResources<PipelineResource>(&package_node, package);
        drawResources<TextureResource>(&package_node, package);
    }
}

void AssetsPanel::drawResource(WidgetNode *node, const GE::Shared<MeshResource> &resource)
{
    auto mesh_node = node->makeSubNode<TreeNode>(resource->id().name(), TREE_NODE_FLAGS);
    mesh_node.call<Text>("Use count: %d", resource->mesh().use_count());
    mesh_node.call<Text>("Filepath: %s", resource->filepath().c_str());

    if (auto popup_context = WidgetNode::create<PopupContextItem>();
        popup_context.call<MenuItem>(GE_FMTSTR("Remove '{}'", resource->id().asString()))) {
        m_commands.removeResource(resource->id());
    }
}

void AssetsPanel::drawResource(WidgetNode *node, const GE::Shared<PipelineResource> &resource)
{
    auto pipeline_node = node->makeSubNode<TreeNode>(resource->id().name(), TREE_NODE_FLAGS);

    pipeline_node.call<Text>("Vertex shader: %s", resource->vertexShaderPath().c_str());
    auto vertex_node = pipeline_node.makeSubNode<TreeNode>("Code:##1");
    if (vertex_node.isOpened()) {
        auto code = GE::FS::readFile<char>(resource->vertexShaderPath());
        code.push_back('\0');
        vertex_node.call<Text>(std::string_view{code.data(), code.size()});
    }

    pipeline_node.call<Text>("Fragment shader: %s", resource->fragmentShaderPath().c_str());
    auto fragment_node = pipeline_node.makeSubNode<TreeNode>("Code:##2");
    if (fragment_node.isOpened()) {
        auto code = GE::FS::readFile<char>(resource->fragmentShaderPath());
        code.push_back('\0');
        fragment_node.call<Text>(std::string_view{code.data(), code.size()});
    }

    if (auto popup_context = WidgetNode::create<PopupContextItem>();
        popup_context.call<MenuItem>(GE_FMTSTR("Remove '{}'", resource->id().asString()))) {
        m_commands.removeResource(resource->id());
    }
}

void AssetsPanel::drawResource(WidgetNode *node, const GE::Shared<TextureResource> &resource)
{
    auto *texture = resource->texture().get();

    auto texture_node = node->makeSubNode<TreeNode>(resource->id().name(), TREE_NODE_FLAGS);
    texture_node.call<Text>("Use count: %d", resource->texture().use_count());
    texture_node.call<Text>("Filepath: %s", resource->filepath().c_str());
    texture_node.call<Text>("Is opaque: %s", toString(resource->texture()->isOpaque()).data());
    texture_node.call<Image>(texture->nativeID(),
                             scaledTextureSize(texture->size(), m_window_size.x));

    if (auto popup_context = WidgetNode::create<PopupContextItem>();
        popup_context.call<MenuItem>(GE_FMTSTR("Remove '{}'", resource->id().asString()))) {
        m_commands.removeResource(resource->id());
    }
}

template<typename T>
void AssetsPanel::drawResources(WidgetNode *node, const Package &package)
{
    auto resources = package.getAllOf<T>();
    if (resources.empty()) {
        return;
    }

    auto group_name = GE::toString(T::GROUP);
    auto resources_node = node->makeSubNode<TreeNode>(group_name, TREE_NODE_FLAGS);
    if (!resources_node.isOpened()) {
        return;
    }

    for (const auto &resource : resources) {
        drawResource(&resources_node, resource);
    }
}

} // namespace LE
