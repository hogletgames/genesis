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

namespace LE {

AddTextureResourceWindow::AddTextureResourceWindow(LevelEditorContext* ctx)
    : WindowBase{NAME}
    , m_ctx(ctx)
{}

void AddTextureResourceWindow::onRender()
{
    WidgetNodeGuard node{&m_window};
    node.call<InputText>("Package", m_id.package());
    node.call<InputText>("Group", m_id.group());
    node.call<InputText>("Name", m_id.name());
    if (node.call<Button>("Texture path")) {
        m_texture_path = openSingleFile("png");
        *m_id.name() = GE::FS::stem(m_texture_path);
    }
    node.call<SameLine>();
    node.call<InputText>("", &m_texture_path);
    if (node.call<Button>("Add")) {
        addResource();
    }
}

void AddTextureResourceWindow::addResource()
{
    if (auto resource = GE::Assets::TextureResource::create(m_id, m_texture_path); resource) {
        m_ctx->assets()->add(std::move(resource));
        close();
        return;
    }

    m_error_signal("Failed to create texture resource");
}

} // namespace LE
