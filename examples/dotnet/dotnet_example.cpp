/*
 * BSD 3-Clause License
 *
 * Copyright (c) 2024, Dmitry Shilnenkov
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

#include "genesis/core/log.h"

#include <coreclr_delegates.h>
#include <hostfxr.h>
#include <nethost.h>

#include <array>
#include <filesystem>
#include <iostream>
#include <string>

extern "C" {

void hostfxr_set_error_writer(hostfxr_error_writer_fn error_writer);

int32_t hostfxr_initialize_for_runtime_config(const char_t *runtime_config_path,
                                              const hostfxr_initialize_parameters *parameters,
                                              hostfxr_handle *host_context_handle);

int32_t hostfxr_close(const hostfxr_handle host_context_handle);

int32_t hostfxr_get_runtime_delegate(const hostfxr_handle host_context_handle,
                                     hostfxr_delegate_type type, void **delegate);
}

namespace {

std::string_view RUNTIMECONFIG_JSON{"examples/dotnet/configs/dotnet_example.runtimeconfig.json"};

void dotNetErrorWriter(const char *error)
{
    GE_ERR("[.NET]: {}", error);
}

} // namespace

int main()
{
    // Initialize logger

    GE::Log::settings_t log_settings;
    log_settings.core_log_level = GE::Logger::Level::INFO;
    log_settings.client_log_level = GE::Logger::Level::TRACE;

    if (!GE::Log::initialize(log_settings)) {
        std::cerr << "Failed to initialize logger\n";
        return EXIT_FAILURE;
    }

    // Load library

    // constexpr size_t PATH_BUFFER_SIZE{256};
    // std::array<char, PATH_BUFFER_SIZE> path_buffer{};
    // size_t path_size{0};

    // if (::get_hostfxr_path(path_buffer.data(), &path_size, nullptr) != 0) {
    //     GE_ERR("Failed to get 'hostfxr' path");
    //     return EXIT_FAILURE;
    // }

    // Set a callback for error messages

    hostfxr_set_error_writer(&dotNetErrorWriter);

    // Initialize hostfxr

    hostfxr_initialize_parameters init_params{};
    init_params.size = sizeof(hostfxr_initialize_parameters);
    init_params.host_path = nullptr;
    init_params.dotnet_root = nullptr;

    hostfxr_handle hostfxr_ctx{nullptr};

    if (int rc =
            hostfxr_initialize_for_runtime_config(RUNTIMECONFIG_JSON.data(), nullptr, &hostfxr_ctx);
        rc != 0 || hostfxr_ctx == nullptr) {
        GE_ERR("Failed to initialize 'hostfxr' context: {:x}", rc);
        return EXIT_FAILURE;
    }

    // Get the "load assembly" delegate

    load_assembly_and_get_function_pointer_fn loadAssemblyAndGetFunctionPointer{};
    if (int rc = hostfxr_get_runtime_delegate(
            hostfxr_ctx, hdt_load_assembly_and_get_function_pointer,
            reinterpret_cast<void **>(&loadAssemblyAndGetFunctionPointer));
        rc != 0 || loadAssemblyAndGetFunctionPointer == nullptr) {
        GE_ERR("Failed to get 'load_assembly_and_get_function_pointer' delegate: {:#08x}", rc);
        hostfxr_close(hostfxr_ctx);
        return EXIT_FAILURE;
    }

    // Load the assembly and get the function pointer

    constexpr std::string_view ASSEMBLY_PATH =
        "/Users/dmitryshilnenkov/Documents/projects/hogletgames/genesis/build/examples/dotnet/"
        "DotNetExample/DotNetExample.dll";
    constexpr std::string_view TYPE_NAME = "GeExamples.UnmanagedCode";
    constexpr std::string_view METHOD_NAME = "HelloWorld";
    // constexpr std::string_view DELEGATE_TYPE_NAME = "Ge.Examples+HelloWorldDelegate";

    using HelloWorldFn = int (*)();
    HelloWorldFn helloWorld{nullptr};

    if (int rc = loadAssemblyAndGetFunctionPointer(ASSEMBLY_PATH.data(), TYPE_NAME.data(),
                                                   METHOD_NAME.data(), UNMANAGEDCALLERSONLY_METHOD,
                                                   nullptr, reinterpret_cast<void **>(&helloWorld));
        rc != 0) {
        GE_ERR("Failed to load 'HelloWorld' function: {:#010x}", static_cast<uint32_t>(rc));
        hostfxr_close(hostfxr_ctx);
        return EXIT_FAILURE;
    }

    // Invoke 'HelloWorld' function

    int helloWorldResult = helloWorld();
    GE_INFO("HelloWorld() returned: {}", helloWorldResult);

    // Cleanup and exit

    hostfxr_close(hostfxr_ctx);
    return EXIT_SUCCESS;
}
