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

#ifndef GENESIS_APP_APPLICATION_H_
#define GENESIS_APP_APPLICATION_H_

#include <genesis/app/layer.h>
#include <genesis/core/log.h>
#include <genesis/core/memory.h>
#include <genesis/graphics/graphics.h>
#include <genesis/window/event_listener.h>
#include <genesis/window/window.h>

#include <deque>

namespace GE {

class WindowClosedEvent;
class WindowMaximizedEvent;
class WindowMinimizedEvent;
class WindowRestoredEvent;

class GE_API Application: public EventListener
{
public:
    struct settings_t {
        Log::settings_t log{};
        Window::settings_t window{};
    };

    static bool initialize(const settings_t& settings);
    static void shutdown();

    static void run();
    static void close();

    static void attachLayer(Shared<Layer> layer);
    static void detachLayer(const Shared<Layer>& layer);

    static Shared<Window> window() { return get()->m_window; }

private:
    enum class WindowState : uint8_t
    {
        NONE = 0,
        MAXIMIZED,
        MINIMIZED
    };

    Application() = default;

    static Application* get()
    {
        static Application instance;
        return &instance;
    }

    static bool initializeApp(const settings_t& settings);
    static void shutdownApp();

    void mainLoop();

    void onEvent(Event* event) override;
    bool onWindowClosed(const WindowClosedEvent& event);
    bool onWindowMaximized(const WindowMaximizedEvent& event);
    bool onWindowMinimized(const WindowMinimizedEvent& event);
    bool onWindowRestored(const WindowRestoredEvent& event);

    void sendEventToLayers(Event* event);
    void updateLayers(Timestamp ts);
    void renderLayers();
    void clearLayers();

    Shared<Window> m_window;
    std::deque<Shared<Layer>> m_layers;
    WindowState m_window_state{WindowState::NONE};

    bool m_running{true};
    Timestamp m_prev_frame_ts;
};

} // namespace GE

#endif // GENESIS_APP_APPLICATION_H_
