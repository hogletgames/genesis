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

#include "options.h"

#include "genesis/core/exception.h"
#include "genesis/core/format.h"
#include "genesis/core/log.h"

#include <nlohmann/json.hpp>

#include <fstream>
#include <iostream>

namespace glm {

// NOLINTNEXTLINE(misc-use-anonymous-namespace)
static void from_json(const nlohmann::json& json, vec2& vec)
{
    json.at("x").get_to(vec.x);
    json.at("y").get_to(vec.y);
}

} // namespace glm

namespace GE {

// NOLINTNEXTLINE(misc-use-anonymous-namespace)
static void from_json(const nlohmann::json& json, Logger::Level& level)
{
    auto level_string = json.get<std::string>();

    if (level = toLoggerLevel(level_string); level == Logger::Level::UNKNOWN) {
        throw Exception(GE_FMTSTR("Failed convert '{}' to logging level", level_string));
    }
}

// NOLINTNEXTLINE(misc-use-anonymous-namespace)
static void from_json(const nlohmann::json& json, Graphics::API& api)
{
    auto api_string = json.get<std::string>();

    if (api = toRendererAPI(api_string); api == Graphics::API::NONE) {
        throw Exception(GE_FMTSTR("Failed convert '{}' to Graphics API", api_string));
    }
}

// NOLINTNEXTLINE(misc-use-anonymous-namespace)
static void from_json(const nlohmann::json& json, Log::settings_t& settings)
{
    json.at("core_log_level").get_to(settings.core_log_level);
    json.at("client_log_level").get_to(settings.client_log_level);
}

// NOLINTNEXTLINE(misc-use-anonymous-namespace)
static void from_json(const nlohmann::json& json, Window::settings_t& settings)
{
    json.at("title").get_to(settings.title);
    json.at("size").get_to(settings.size);
    json.at("vsync").get_to(settings.vsync);
}

// NOLINTNEXTLINE(misc-use-anonymous-namespace)
static void from_json(const nlohmann::json& json, Graphics::settings_t& settings)
{
    json.at("api").get_to(settings.api);
    json.at("app_name").get_to(settings.app_name);
    json.at("msaa_samples").get_to(settings.msaa_samples);
}

// NOLINTNEXTLINE(misc-use-anonymous-namespace)
static void from_json(const nlohmann::json& json, Application::settings_t& settings)
{
    json.at("log").get_to(settings.log);
    json.at("window").get_to(settings.window);
    json.at("graphics").get_to(settings.graphics);
}

} // namespace GE

namespace LE {

std::optional<GE::Application::settings_t> Options::load(std::string_view options_filepath)
{
    std::ifstream fin(options_filepath.data());

    if (!fin) {
        std::cerr << "Failed to open configuration file: '" << options_filepath << "'" << std::endl;
        return {};
    }

    nlohmann::json json;
    GE::Application::settings_t settings;

    try {
        fin >> json;
        return json.get<GE::Application::settings_t>();
    } catch (const std::exception& e) {
        std::cerr << "Failed to parse configuration '" << options_filepath << "': '" << e.what()
                  << "'" << std::endl;
    }

    return {};
}

} // namespace LE
