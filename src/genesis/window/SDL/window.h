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

// NOLINTNEXTLINE(llvm-header-guard)
#ifndef GENESIS_WINDOW_SDL_WINDOW_H_
#define GENESIS_WINDOW_SDL_WINDOW_H_

#include <genesis/window/window.h>

#include <list>

struct SDL_Window;
union SDL_Event;

namespace GE {
class RenderContext;
} // namespace GE

namespace GE::SDL {

class Window: public GE::Window
{
public:
    explicit Window(settings_t settings);
    ~Window();

    static bool initialize();
    static void shutdown();

    void pollEvents() override;
    void onUpdate() override;

    void attachEventListener(EventListener* listener) override;
    void detachEventListener(EventListener* listener) override;

    const Vec2& windowSize() const override { return m_settings.size; }
    void setVSync(bool enabled) override;
    const settings_t& settings() const override { return m_settings; }

    void* nativeWindow() override { return m_window; }
    Shared<RenderContext> renderContext() const override { return m_context; }

private:
    void emitEvent(Event* event);

    void onMouseEvent(const SDL_Event& sdl_event);
    void onKeyboardEvent(const SDL_Event& sdl_event);
    void onWindowEvent(const SDL_Event& sdl_event);

    settings_t m_settings;
    SDL_Window* m_window{nullptr};
    Shared<RenderContext> m_context;

    std::list<EventListener*> m_event_listeners;
};

} // namespace GE::SDL

#endif // GENESIS_WINDOW_SDL_WINDOW_H_
