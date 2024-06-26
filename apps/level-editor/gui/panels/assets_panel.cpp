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

using namespace GE::GUI;

namespace LE {
namespace {

template<typename ForwardIt>
ForwardIt nextPackage(ForwardIt begin, ForwardIt end, std::string_view current_package)
{
    return std::find_if(
        begin, end, [current_package](const auto &id) { return id.package() != current_package; });
}

template<typename ForwardIt>
ForwardIt nextGroup(ForwardIt begin, ForwardIt end, std::string_view current_package,
                    std::string_view current_group)
{
    return std::find_if(begin, end, [current_package, current_group](const auto &id) {
        return id.package() != current_package || id.group() != current_group;
    });
}

GE::Vec2 scaledTextureSize(const GE::Vec2 &texture_size, float window_width)
{
    float scale = window_width / texture_size.x;
    return texture_size * scale;
}

} // namespace

AssetsPanel::AssetsPanel(GE::Assets::Registry *registry)
    : WindowBase{NAME}
    , m_registry{registry}
{}

void AssetsPanel::onRender()
{
    WidgetNode window_node{&m_window};
    window_node.call(&AssetsPanel::updateWindowParameters, this);
    drawAssets(&window_node);
    drawContextMenu(&window_node);
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
    auto ids = m_registry->ids();
    std::sort(ids.begin(), ids.end());

    for (auto it = ids.cbegin(); it < ids.cend();) {
        std::string_view package = it->package();
        auto package_tree_node = node->makeSubNode<TreeNode>(package);

        if (package_tree_node.isOpened()) {
            it = drawPackage(it, ids.cend(), package);
        } else {
            it = nextPackage(it, ids.cend(), package);
        }
    }
}

AssetsPanel::ResourceIDIt AssetsPanel::drawPackage(ResourceIDIt begin, ResourceIDIt end,
                                                   std::string_view package)
{
    auto it{begin};

    while (it < end) {
        if (it->package() != package) {
            break;
        }

        std::string_view group = it->group();
        auto group_tree_node = WidgetNode::create<TreeNode>(group);

        if (group_tree_node.isOpened()) {
            it = drawGroup(it, end, package, group);
        } else {
            it = nextGroup(it, end, package, group);
        }
    }

    return it;
}

AssetsPanel::ResourceIDIt AssetsPanel::drawGroup(ResourceIDIt begin, ResourceIDIt end,
                                                 std::string_view package, std::string_view group)
{
    auto it{begin};

    for (; it < end; ++it) {
        if (it->package() != package || it->group() != group) {
            break;
        }

        std::string_view name = it->name();
        auto name_tree_node = WidgetNode::create<TreeNode>(name);

        if (name_tree_node.isOpened()) {
            m_registry->visit(*it, this);
        }

        auto popup_context = WidgetNode::create<PopupContextItem>(name);
        popup_context.call<MenuItem>(GE_FMTSTR("Remove '{}'", it->id()));
    }

    return it;
}

void AssetsPanel::visit(GE::Assets::MeshResource *resource)
{
    Text::call("Use count: %d", resource->mesh().use_count());
    Text::call("Filepath: %s", resource->filepath().c_str());
}

void AssetsPanel::visit(GE::Assets::PipelineResource *resource)
{
    Text::call("Use count: %d", resource->pipeline().use_count());

    {
        Text::call("Vertex shader: %s", resource->vertexShader().c_str());
        auto code_node = WidgetNode::create<TreeNode>("Code:");
        if (code_node.isOpened()) {
            auto code = GE::FS::readFile<char>(resource->vertexShader());
            Text::call(std::string_view{code.data(), code.size()});
        }
    }

    {
        Text::call("Fragment shader: %s", resource->fragmentShader().c_str());
        auto code_node = WidgetNode::create<TreeNode>("Code:##2");
        if (code_node.isOpened()) {
            auto code = GE::FS::readFile<char>(resource->fragmentShader());
            Text::call(std::string_view{code.data(), code.size()});
        }
    }
}

void AssetsPanel::visit(GE::Assets::TextureResource *resource)
{
    auto *texture = resource->texture().get();

    Text::call("Use count: %d", resource->texture().use_count());
    Text::call("Filepath: %s", resource->filepath().c_str());
    Text::call("Is opaque: %s", toString(resource->texture()->isOpaque()).data());
    Image::call(texture->nativeID(), scaledTextureSize(texture->size(), m_window_size.x));
}

} // namespace LE
