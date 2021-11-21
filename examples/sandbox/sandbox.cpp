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

#include "gui_layer.h"
#include "triangle_layer.h"

#include <genesis/genesis.h>

#include <docopt.h>

#include <iostream>

namespace {

struct args_t {
    std::string layer;
};

constexpr auto USAGE = R"(Sandbox
Examples:
  - triangle
  - gui

Usage:
    sandbox (-h|--help)
    sandbox [-e <example>]

Options:
    -h, --help                  Show this help
    -e, --example <example>     A name of an example [default: triangle])";

constexpr GE::Logger::Level LOG_LEVEL{GE::Logger::Level::TRACE};
constexpr GE::Graphics::API RENDER_API{GE::Graphics::API::VULKAN};
constexpr auto APP_NAME = "Sandbox";
constexpr uint8_t MSAA_SAMPLES{4};

args_t parseArgs(int argc, char** argv)
{
    std::map<std::string, docopt::value> parsed_args;
    args_t args{};

    try {
        parsed_args = docopt::docopt_parse(USAGE, {argv + 1, argv + argc}, true, false);
        args.layer = parsed_args["--example"].asString();
    } catch (const docopt::DocoptExitHelp& e) {
        std::cout << USAGE << std::endl;
        exit(EXIT_SUCCESS); // NOLINT(concurrency-mt-unsafe)
    } catch (const std::exception& e) {
        std::cerr << "Failed to parse arguments: " << e.what() << std::endl;
        exit(EXIT_FAILURE); // NOLINT(concurrency-mt-unsafe)
    }

    return args;
}

GE::Shared<GE::Layer> layerFactory(const std::string& layer)
{
    using LayerCreator = std::function<GE::Shared<GE::Layer>()>;

    static const std::unordered_map<std::string, LayerCreator> TO_LAYER = {
        {"triangle", GE::makeShared<GE::Examples::TriangleLayer>},
        {"gui", GE::makeShared<GE::Examples::GUILayer>},
    };

    if (auto layer_creator = GE::getValue(TO_LAYER, layer); layer_creator) {
        return layer_creator();
    }

    return nullptr;
}

} // namespace

int main(int argc, char** argv)
{
    auto args = parseArgs(argc, argv);

    GE::Log::settings_t log_settings{};
    log_settings.core_log_level = LOG_LEVEL;
    log_settings.client_log_level = LOG_LEVEL;

    GE::Graphics::settings_t graphics_settings{};
    graphics_settings.api = RENDER_API;
    graphics_settings.app_name = APP_NAME;
    graphics_settings.msaa_samples = MSAA_SAMPLES;

    GE::Window::settings_t window_settings{};
    window_settings.title = APP_NAME;

    GE::Application::settings_t app_settings{};
    app_settings.log = log_settings;
    app_settings.window = window_settings;
    app_settings.graphics = graphics_settings;

    if (!GE::Application::initialize(app_settings)) {
        return EXIT_FAILURE;
    }

    if (auto layer = layerFactory(args.layer); layer != nullptr) {
        GE::Application::attachLayer(layer);
        GE::Application::run();
        return EXIT_SUCCESS;
    }

    GE_ERR("Unknown layer: '{}'", args.layer);
    GE::Application::shutdown();
    return EXIT_FAILURE;
}
