/*
 * BSD 3-Clause License
 *
 * Copyright (c) 2022, Dmitry Shilnenkov
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

#include "camera.h"
#include "drawable/drawable.h"

#include <genesis/graphics/framebuffer.h>
#include <genesis/gui/widgets.h>

namespace GE::Examples {

class GE_API GuiLayerWindow
{
public:
    void draw();
    void update();

    std::string_view name() const { return m_name; }
    bool* isOpenFlag() { return &m_is_open; }
    Vec3* objectTranslation() { return &m_translation; }
    Vec3* objectRotation() { return &m_rotation; }
    Vec3* objectScale() { return &m_scale; }

    Mat4 transform() const;

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

    bool m_is_open{false};
    std::string m_name;
    GUI::Window m_window{m_name, &m_is_open};

    Camera m_camera;
    Vec3 m_translation{0.0f, 0.0f, 0.0f};
    Vec3 m_rotation{0.0f, 0.0f, 0.0f};
    Vec3 m_scale{1.0f, 1.0f, 1.0f};

    Scoped<Framebuffer> m_fbo;
    Scoped<Drawable> m_draw_object;
};

} // namespace GE::Examples
