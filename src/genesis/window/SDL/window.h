/*
 * BSD 3-Clause License
 *
 * Copyright (c) 2021, Dmitry Shilnenkov
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

#include <genesis/graphics/graphics.h>
#include <genesis/window/window.h>

#include <list>

struct SDL_Window;
union SDL_Event;

namespace GE {
class GraphicsContext;
} // namespace GE

namespace GE::SDL {

class Window: public GE::Window
{
public:
    Window(settings_t settings, Graphics::API api);
    ~Window();

    static bool initialize();
    static void shutdown();

    void pollEvents() override;

    void attachEventListener(EventListener* listener) override;
    void detachEventListener(EventListener* listener) override;

    void* nativeWindow() override { return m_window; }
    std::string title() override;
    Vec2 size() const override;
    Vec2 position() const override;

    void setVSync(bool enabled) override;
    void setTitle(std::string_view title) override;

private:
    void emitEvent(Event* event);

    void onMouseEvent(const SDL_Event& sdl_event);
    void onKeyboardEvent(const SDL_Event& sdl_event);
    void onWindowEvent(const SDL_Event& sdl_event);

    SDL_Window*               m_window{nullptr};
    std::list<EventListener*> m_event_listeners;
};

} // namespace GE::SDL
