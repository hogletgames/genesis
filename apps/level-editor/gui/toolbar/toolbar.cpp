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

#include "toolbar.h"
#include "level_editor_context.h"

#include "genesis/graphics/texture.h"
#include "genesis/graphics/texture_loader.h"
#include "genesis/gui/widgets/style_var.h"

using namespace GE::GUI;

namespace LE {

Toolbar::Toolbar(LevelEditorContext* ctx)
    : m_ctx{ctx}
{
    loadIcons(m_ctx->settings()->resourePaths());
}

Toolbar::~Toolbar() = default;

void Toolbar::onRender()
{
    auto colors = getStyleColors();
    auto button_hovered = colors[StyleColor::BUTTON_HOVERED];
    auto button_active = colors[StyleColor::BUTTON_ACTIVE];

    button_hovered.w = 0.5f;
    button_active.w = 0.5f;

    StyleVar window_padding_style{StyleVar::WINDOW_PADDING, GE::Vec2{0.0f, 2.0f}};
    StyleVar item_inner_spacing_style{StyleVar::ITEM_INNER_SPACING, GE::Vec2{0.0f, 0.0f}};
    StyleColor button_color{StyleColor::BUTTON, GE::Vec4{0.0f, 0.0f, 0.0f, 0.0f}};
    StyleColor button_hovered_style{StyleColor::BUTTON_HOVERED, button_hovered};
    StyleColor button_active_style{StyleColor::BUTTON_ACTIVE, button_active};
}

void Toolbar::loadIcons(const ResourcePaths& resources)
{
    m_play_button_icon = GE::TextureLoader{resources.playButtonIconPath()}.load();
    m_simulation_button_icon = GE::TextureLoader{resources.simulationButtonIconPath()}.load();
    m_step_button_icon = GE::TextureLoader{resources.stepButtonIconPath()}.load();
    m_pause_button_icon = GE::TextureLoader{resources.pauseButtonIconPath()}.load();
    m_stop_button_icon = GE::TextureLoader{resources.stopButtonIconPath()}.load();
}

} // namespace LE
