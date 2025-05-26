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

#include "level_editor_layer.h"
#include "options.h"

#include <genesis/genesis.h>

#include <docopt.h>

#include <iostream>

namespace {

constexpr auto USAGE = R"(Level Editor

Usage:
    level_editor -c <config-file>
    level_editor (-h | --help)

Options:
    -h, --help           Show this help.
    -c, --config <path>  The path to a config file.
)";

struct args_t {
    bool        show_help{false};
    std::string config;
};

std::optional<args_t> parseArgs(int argc, char** argv)
{
    std::map<std::string, docopt::value> parsed_args;
    args_t                               args{};

    try {
        parsed_args = docopt::docopt_parse(USAGE, {argv + 1, argv + argc}, true, false);
        args.config = parsed_args["--config"].asString();
    } catch (const docopt::DocoptExitHelp& e) {
        args.show_help = true;
    } catch (const std::exception& e) {
        std::cerr << "Failed to parse arguments: '" << e.what() << "'\n";
        return {};
    }

    return args;
}

GE::Shared<GE::Layer> makeLevelEditorLayer()
{
    return GE::makeShared<LE::LevelEditorLayer>();
}

} // namespace

int main(int argc, char** argv)
{
    auto args = parseArgs(argc, argv);

    if (!args.has_value()) {
        std::cerr << USAGE;
        return EXIT_FAILURE;
    }

    if (args->show_help) {
        std::cout << USAGE;
        return EXIT_SUCCESS;
    }

    auto app_settings = LE::Options::load(args->config);

    if (!app_settings.has_value() || !GE::Application::initialize(app_settings.value())) {
        return EXIT_FAILURE;
    }

    GE::Application::attachLayer(makeLevelEditorLayer());
    GE::Application::run();
    return EXIT_SUCCESS;
}
