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

#include "gui_layer_window.h"
#include "drawable/drawable.h"

#include "genesis/core.h"
#include "genesis/graphics.h"
#include "genesis/gui/widgets.h"
#include "genesis/math.h"
#include "genesis/scene.h"

namespace GE::Examples {

GuiLayerWindow::~GuiLayerWindow() = default;

void GuiLayerWindow::onUpdate(GE::Timestamp ts)
{
    if (m_is_focused) {
        m_camera_controller->onUpdate(ts);
    }
}

void GuiLayerWindow::onEvent(Event* event)
{
    if (m_is_focused) {
        m_camera_controller->onEvent(event);
    }
}

void GuiLayerWindow::draw()
{
    if (!m_is_open) {
        return;
    }

    GUI::StyleVar padding{GUI::StyleVar::WINDOW_PADDING, {0.0f, 0.0f}};
    GUI::WidgetNode node{&m_window};
    node.call(&GuiLayerWindow::updateWindowParameters, this);

    Drawable::mvp_t mvp{};
    mvp.model = translate(Mat4{1.0f}, m_camera->position());
    mvp.view = m_camera->view();
    mvp.projection = m_camera->projection();

    m_fbo->renderer()->beginFrame();
    m_draw_object->draw(m_fbo->renderer(), mvp);
    m_fbo->renderer()->endFrame();
    m_fbo->renderer()->swapBuffers();

    const auto& texture = m_fbo->colorTexture();
    node.call<GUI::Image>(texture.nativeID(), texture.size());
}

GuiLayerWindow::GuiLayerWindow(std::string name)
    : m_name{std::move(name)}
    , m_camera{makeShared<Scene::ViewProjectionCamera>()}
    , m_camera_controller{makeScoped<Scene::VPCameraController>(m_camera)}
{
    Framebuffer::config_t model_fbo_config{};
    model_fbo_config.attachments[0].clear_color = Vec4{0.3f, 0.3f, 0.3f, 1.0f};
    model_fbo_config.size = {720.0f, 480.0f};
    model_fbo_config.msaa_samples = GE::Graphics::limits().max_msaa;

    m_fbo = Framebuffer::create(model_fbo_config);
    GE_ASSERT(m_fbo, "Failed to create framebuffer");

    m_camera_controller->setViewport(model_fbo_config.size);
}

void GuiLayerWindow::updateWindowParameters()
{
    m_is_focused = m_window.isFocused();

    if (auto viewport = m_window.availableRegion(); viewport != m_fbo->size()) {
        m_fbo->resize(viewport);
        m_camera_controller->setViewport(viewport);
    }
}

} // namespace GE::Examples
