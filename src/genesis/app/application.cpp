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

#include "application.h"

#include "genesis/core/utils.h"
#include "genesis/window/events/event_dispatcher.h"
#include "genesis/window/events/window_events.h"
#include "genesis/window/input.h"

namespace GE {

bool Application::initialize(const settings_t& settings)
{
    if (!Log::initialize(settings.log) || !Window::initialize() || !Input::initialize() ||
        !initializeApp(settings)) {
        GE_CORE_ERR("Failed to initialize Application");
        shutdown();
        return false;
    }

    GE_CORE_INFO("Application has been initialized");
    return true;
}

void Application::shutdown()
{
    shutdownApp();
    Input::shutdown();
    Window::shutdown();
    Log::shutdown();
}

void Application::run()
{
    get()->m_running = true;
    get()->mainLoop();
}

void Application::attachLayer(Shared<Layer> layer)
{
    layer->onAttached();
    get()->m_layers.push_back(std::move(layer));
}

void Application::detachLayer(const Shared<Layer>& layer)
{
    auto& layers = get()->m_layers;

    if (auto it = std::find(layers.begin(), layers.end(), layer); it != layers.end()) {
        (*it)->onDetached();
        layers.erase(it);
    }
}

void Application::close()
{
    get()->m_running = false;
}

bool Application::initializeApp(const settings_t& settings)
{
    GE_CORE_INFO("Initializing Application...");

    auto& window = get()->m_window;

    if (window = Window::create(settings.window); window == nullptr) {
        GE_CORE_ERR("Failed to create Window");
        return false;
    }

    Renderer::setContext(window->renderContext());

    window->attachEventListener(get());
    return true;
}

void Application::shutdownApp()
{
    GE_CORE_INFO("Shutdown Application");
    close();
    get()->clearLayers();
    get()->m_window.reset();
    Renderer::setContext(nullptr);
    get()->m_window_state = WindowState::NONE;
}

void Application::mainLoop()
{
    m_prev_frame_ts = Timestamp::now();

    while (m_running) {
        auto now = Timestamp::now();
        auto time_delta = now - m_prev_frame_ts;
        m_prev_frame_ts = now;

        m_window->pollEvents();
        updateLayers(time_delta);

        if (m_window_state != WindowState::MINIMIZED) {
            renderLayers();
        }

        m_window->onUpdate();
    }
}

void Application::onEvent(Event* event)
{
    EventDispatcher dispatcher{event};
    dispatcher.dispatch<WindowClosedEvent>(GE_EVENT_MEM_FN(onWindowClosed));
    dispatcher.dispatch<WindowMaximizedEvent>(GE_EVENT_MEM_FN(onWindowMaximized));
    dispatcher.dispatch<WindowMinimizedEvent>(GE_EVENT_MEM_FN(onWindowMinimized));
    dispatcher.dispatch<WindowRestoredEvent>(GE_EVENT_MEM_FN(onWindowRestored));

    sendEventToLayers(event);
}

bool Application::onWindowClosed([[maybe_unused]] const WindowClosedEvent& event)
{
    close();
    return true;
}

bool Application::onWindowMaximized([[maybe_unused]] const WindowMaximizedEvent& event)
{
    m_window_state = WindowState::MAXIMIZED;
    return false;
}

bool Application::onWindowMinimized([[maybe_unused]] const WindowMinimizedEvent& event)
{
    m_window_state = WindowState::MINIMIZED;
    return false;
}

bool Application::onWindowRestored([[maybe_unused]] const WindowRestoredEvent& event)
{
    m_window_state = WindowState::NONE;
    return false;
}

void Application::sendEventToLayers(Event* event)
{
    for (auto& layer : m_layers) {
        if (event->handled()) {
            break;
        }

        layer->onEvent(event);
    }
}

void Application::updateLayers(Timestamp ts)
{
    for (auto& layer : get()->m_layers) {
        layer->onUpdate(ts);
    }
}

void Application::renderLayers()
{
    for (auto& layer : m_layers) {
        layer->onRender();
    }
}

void Application::clearLayers()
{
    for (auto& layer : get()->m_layers) {
        layer->onDetached();
    }

    get()->m_layers.clear();
}

} // namespace GE
