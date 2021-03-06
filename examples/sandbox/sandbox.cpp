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

#include "render_layer.h"

#include <genesis/genesis.h>

namespace {

constexpr GE::Logger::Level LOG_LEVEL{GE::Logger::Level::TRACE};
constexpr GE::Renderer::API RENDER_API{GE::Renderer::API::VULKAN};

} // namespace

int main()
{
    GE::Log::settings_t log_settings{};
    log_settings.core_log_level = LOG_LEVEL;
    log_settings.client_log_level = LOG_LEVEL;

    GE::Window::settings_t window_settings{};
    window_settings.render_api = RENDER_API;

    GE::Renderer::settings_t renderer_settings{};
    renderer_settings.api = RENDER_API;

    GE::Application::settings_t app_settings{};
    app_settings.log = log_settings;
    app_settings.window = window_settings;
    app_settings.renderer = renderer_settings;

    if (!GE::Application::initialize(app_settings)) {
        GE_ERR("Failed to initialize Engine");
        return EXIT_FAILURE;
    }

    GE::Application::attachLayer(GE::makeShared<GE::Examples::RenderLayer>());
    GE::Application::run();
    GE::Application::shutdown();

    return EXIT_SUCCESS;
}
