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

#include "drawable/drawable.h"

#include <genesis/core/timestamp.h>
#include <genesis/graphics/framebuffer.h>
#include <genesis/gui/widgets.h>

namespace GE::Scene {
class ViewProjectionCamera;
class VPCameraController;
} // namespace GE::Scene

namespace GE::Examples {

class GE_API GuiLayerWindow
{
public:
    ~GuiLayerWindow();

    void onUpdate(Timestamp ts);
    void onEvent(Event* event);
    void draw();

    std::string_view name() const { return m_name; }
    bool* isOpenFlag() { return &m_is_open; }
    Shared<Scene::ViewProjectionCamera> camera() const { return m_camera; }

    template<typename T, typename... Args>
    static Scoped<GuiLayerWindow> create(const std::string& name, Args&&... args)
    {
        auto window = Scoped<GuiLayerWindow>(new GuiLayerWindow{name});
        auto* renderer = window->m_fbo->renderer();
        window->m_draw_object = makeScoped<T>(renderer, std::forward<Args>(args)...);
        return window;
    }

private:
    explicit GuiLayerWindow(std::string name);

    void updateWindowParameters();

    bool m_is_open{false};
    bool m_is_focused{false};
    std::string m_name;
    GUI::Window m_window{m_name, &m_is_open};

    Scoped<Framebuffer> m_fbo;
    Scoped<Drawable> m_draw_object;

    Shared<Scene::ViewProjectionCamera> m_camera;
    Scoped<Scene::VPCameraController> m_camera_controller;
};

} // namespace GE::Examples
