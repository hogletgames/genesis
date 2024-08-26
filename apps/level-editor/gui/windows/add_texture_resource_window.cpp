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

#include "add_texture_resource_window.h"
#include "level_editor_context.h"

#include "genesis/assets/texture_resource.h"
#include "genesis/filesystem/filepath.h"
#include "genesis/gui/file_dialog.h"
#include "genesis/gui/widgets.h"

using namespace GE::GUI;
using namespace GE::Assets;

namespace LE {

AddTextureResourceWindow::AddTextureResourceWindow(LevelEditorContext* ctx)
    : AddResourceWindowBase{NAME, ctx}
{}

void AddTextureResourceWindow::onRender()
{
    WidgetNode node{&m_window};
    renderPackageCombobox(&node);
    node.call<InputText>("Name", &m_resource_name);
    if (node.call<Button>("Texture path")) {
        m_texture_path = openSingleFile("png");
        m_resource_name = GE::FS::stem(m_texture_path);
    }
    node.call<SameLine>();
    node.call<InputText>("", &m_texture_path);
    if (node.call<Button>("Add")) {
        addResource();
    }
}

void AddTextureResourceWindow::addResource()
{
    auto* package = m_ctx->assets()->package(m_package_name);
    if (package == nullptr) {
        m_error_signal(GE_FMTSTR("Package '{}' not found", m_package_name));
        return;
    }

    auto resource = package->createResource<TextureResource>({m_resource_name, m_texture_path});
    if (resource == nullptr) {
        m_error_signal(GE_FMTSTR("Failed to create texture resource '{}'", m_resource_name));
        return;
    }

    close();
}

} // namespace LE
