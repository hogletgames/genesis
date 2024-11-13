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

#include "genesis/core/format.h"
#include "genesis/core/log.h"
#include "genesis/filesystem/filepath.h"

#include <coreclr_delegates.h>
#include <hostfxr.h>
#include <nethost.h>

#include <dlfcn.h>
#include <iostream>
#include <string>

namespace {

void *loadLibrary(std::string_view library_path)
{
    return dlopen(library_path.data(), RTLD_LAZY);
}

template<typename Func>
bool loadFunction(Func *func, void *library, std::string_view function_name)
{
    if (auto *symbol = dlsym(library, function_name.data()); symbol != nullptr) {
        *func = reinterpret_cast<Func>(symbol);
        return true;
    }

    return false;
}

class ScriptingRuntimeContext
{
public:
    ScriptingRuntimeContext() {}

    ~ScriptingRuntimeContext() { shutdown(); }

    bool initialize(std::string_view runtime_config)
    {
        GE_INFO("Initializing 'hostfxr' context");

        auto hostfxr_path = loadHostLibrary();
        if (hostfxr_path.empty()) {
            GE_ERR("Failed to load 'hostfxr' path");
            return false;
        }

        GE_INFO("Loaded 'hostfxr' path: '{}'", hostfxr_path);

        if (!loadHostfxrFunctions(hostfxr_path)) {
            GE_ERR("Failed to load 'hostfxr' functions");
            return false;
        }

        GE_INFO("Configure the error writer");
        m_hostfxr_set_error_writer(&ScriptingRuntimeContext::errorWriter);

        if (int rc =
                m_hostfxr_initialize_for_runtime_config(runtime_config.data(), nullptr, &m_context);
            rc != 0 || m_context == nullptr) {
            GE_ERR("Failed to initialize 'hostfxr' context: {:#08x}", static_cast<uint32_t>(rc));
            return false;
        }

        GE_INFO("'hostfxr' context initialized");
        return loadAssemblyFnDelegate() && false;// && loadGetFunctionPointerFnDelegate();
    }

    void shutdown()
    {
        if (m_context != nullptr) {
            GE_INFO("Close 'hostfxr' context");
            m_hostfxr_close(m_context);
            m_context = nullptr;
        }
    }

    bool loadAssembly(std::string_view assembly_path)
    {
        GE_INFO("Loading assembly: '{}'", assembly_path);

        if (int rc = m_load_assembly(assembly_path.data(), nullptr, nullptr); rc != 0) {
            GE_ERR("Failed to load assembly: {:#08x}", static_cast<uint32_t>(rc));
            return false;
        }

        GE_INFO("Assembly loaded");
        return true;
    }

    template<typename Func>
    bool getFunctionPointer(std::string_view type_name, std::string_view method_name, Func **func,
                            std::string_view delegate_type_name = {})
    {
        const char_t *method_type = delegate_type_name.empty() ? UNMANAGEDCALLERSONLY_METHOD
                                                               : delegate_type_name.data();

        GE_INFO("Loading function pointer: '{}'", method_name);

        if (int rc = m_get_function_pointer(type_name.data(), method_name.data(), method_type,
                                            nullptr, nullptr, reinterpret_cast<void **>(func));
            rc != 0 || *func == nullptr) {
            GE_ERR("Failed to load '{}' pointer: {:#08x}", method_name, static_cast<uint32_t>(rc));
            return false;
        }

        GE_INFO("'{}' function loaded", method_name);
        return true;
    }

private:
    static void errorWriter(const char_t *error) { GE_ERR("[.NET]: {}", error); }

    std::string loadHostLibrary() const
    {
        std::array<char, 256> buffer{};
        size_t buffer_size = buffer.size();

        if (int rc = get_hostfxr_path(buffer.data(), &buffer_size, nullptr); rc != 0) {
            GE_ERR("Failed to get hostfxr path: {:#08x}", static_cast<uint32_t>(rc));
            return {};
        }

        return {buffer.data(), buffer_size};
    }

    bool loadHostfxrFunctions(std::string_view hostfxr_path)
    {
        auto *library = loadLibrary(hostfxr_path);
        if (library == nullptr) {
            GE_ERR("Failed to load 'hostfxr' library: {}", hostfxr_path);
            return false;
        }

        if (!loadFunction(&m_hostfxr_initialize_for_runtime_config, library,
                          "hostfxr_initialize_for_runtime_config") ||
            !loadFunction(&m_hostfxr_close, library, "hostfxr_close") ||
            !loadFunction(&m_hostfxr_set_error_writer, library, "hostfxr_set_error_writer") ||
            !loadFunction(&m_hostfxr_get_runtime_delegate, library,
                          "hostfxr_get_runtime_delegate")) {
            GE_ERR("Failed to load 'hostfxr' functions");
            return false;
        }

        return true;
    }

    bool loadAssemblyFnDelegate()
    {
        GE_INFO("Loading 'load_assembly_fn' delegate");

        if (int rc = m_hostfxr_get_runtime_delegate(m_context, hdt_load_assembly,
                                                    reinterpret_cast<void **>(&m_load_assembly));
            rc != 0 || m_load_assembly == nullptr) {
            GE_ERR("Failed to get 'load_assembly' delegate: {:#08x}", static_cast<uint32_t>(rc));
            return false;
        }

        GE_INFO("'load_assembly_fn' delegate loaded");
        return true;
    }

    bool loadGetFunctionPointerFnDelegate()
    {
        GE_INFO("Loading 'get_function_pointer' delegate");

        if (int rc =
                m_hostfxr_get_runtime_delegate(m_context, hdt_get_function_pointer,
                                               reinterpret_cast<void **>(&m_get_function_pointer));
            rc != 0 || m_get_function_pointer == nullptr) {
            GE_ERR("Failed to get 'get_function_pointer' delegate: {:#08x}",
                   static_cast<uint32_t>(rc));
            return false;
        }

        GE_INFO("'get_function_pointer' delegate loaded");
        return true;
    }

    hostfxr_initialize_for_runtime_config_fn m_hostfxr_initialize_for_runtime_config{nullptr};
    hostfxr_close_fn m_hostfxr_close{nullptr};
    hostfxr_set_error_writer_fn m_hostfxr_set_error_writer{nullptr};
    hostfxr_get_runtime_delegate_fn m_hostfxr_get_runtime_delegate{nullptr};

    hostfxr_handle m_context{nullptr};
    load_assembly_fn m_load_assembly{nullptr};
    get_function_pointer_fn m_get_function_pointer{nullptr};
};

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

    // Initialize the context

    constexpr std::string_view RUNTIMECONFIG_JSON{
        "examples/dotnet/configs/dotnet_example.runtimeconfig.json"};
    const std::string ASSEMBLY_PATH{GE::FS::joinPath(DOTNET_EXAMPLE_DLL_DIR, "DotNetExample.dll")};

    ScriptingRuntimeContext context;
    if (!context.initialize(RUNTIMECONFIG_JSON)
        // || !context.loadAssembly(ASSEMBLY_PATH)
    ) {
        return EXIT_FAILURE;
    }

    // Load and invoke 'HelloWorld()' method

    // constexpr std::string_view UNMANAGED_CODE_TYPE{"Ge.Examples.UnmanagedCode, DotNetExample"};
    // constexpr std::string_view HELLO_WORLD_NAME{"HelloWorld"};
    //
    // void (*helloWorld)(){nullptr};
    // if (!context.getFunctionPointer(UNMANAGED_CODE_TYPE, HELLO_WORLD_NAME, &helloWorld)) {
    //     return EXIT_FAILURE;
    // }
    //
    // helloWorld();
    //
    // // Load and invoke 'Print(string)' method
    //
    // constexpr std::string_view PRINT_NAME{"Print"};
    // constexpr std::string_view PRINT_DELEGATE_NAME{
    //     "Ge.Examples.UnmanagedCode+PrintDelegate, DotNetExample"};
    //
    // void (*print)(const char_t *message){nullptr};
    // if (!context.getFunctionPointer(UNMANAGED_CODE_TYPE, PRINT_NAME, &print,
    // PRINT_DELEGATE_NAME)) {
    //     return EXIT_FAILURE;
    // }
    //
    // print("Hello from C++!");
    //
    // Creat an object

    // constexpr std::string_view UNMANAGED_OBJECT_TYPE{"Ge.Examples.UnmanagedObject,
    // DotNetExample"}; constexpr std::string_view OBJECT_CREATE_NAME{"Create"}; constexpr
    // std::string_view OBJECT_CREATE_DELEGATE_NAME{
    //     "Ge.Examples.UnmanagedObject+CreateDelegate, DotNetExample"};
    //
    // void *(*ObjectCreate)(const char_t *name){nullptr};
    // if (!context.getFunctionPointer(UNMANAGED_OBJECT_TYPE, OBJECT_CREATE_NAME, &ObjectCreate,
    //                                 OBJECT_CREATE_DELEGATE_NAME)) {
    //     return EXIT_FAILURE;
    // }
    //
    // void *object = ObjectCreate("MyObject");
    //
    // // Print the object's name
    //
    // constexpr std::string_view OBJECT_PRINT_NAME_NAME{"PrintName"};
    // constexpr std::string_view OBJECT_PRINT_NAME_DELEGATE_NAME{
    //     "Ge.Examples.UnmanagedObject+PrintNameDelegate, DotNetExample"};
    //
    // void (*ObjectPrintName)(const void *object){nullptr};
    // if (!context.getFunctionPointer(UNMANAGED_OBJECT_TYPE, OBJECT_PRINT_NAME_NAME,
    // &ObjectPrintName,
    //                                 OBJECT_PRINT_NAME_DELEGATE_NAME)) {
    //     return EXIT_FAILURE;
    // }
    //
    // ObjectPrintName(object);
    //
    // // Destroy object
    //
    // constexpr std::string_view OBJECT_DESTROY_NAME{"Destroy"};
    // constexpr std::string_view OBJECT_DESTROY_DELEGATE_NAME{
    //     "Ge.Examples.UnmanagedObject+DestroyDelegate, DotNetExample"};
    //
    // void (*ObjectDestroy)(void *object){nullptr};
    // if (!context.getFunctionPointer(UNMANAGED_OBJECT_TYPE, OBJECT_DESTROY_NAME, &ObjectDestroy,
    //                                 OBJECT_DESTROY_DELEGATE_NAME)) {
    //     return EXIT_FAILURE;
    // }
    //
    // ObjectDestroy(object);

    return EXIT_SUCCESS;
}
