/*
 * BSD 3-Clause License
 *
 * Copyright (c) 2025, Dmitry Shilnenkov
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

#include "host_framework.h"

#include "genesis/core/asserts.h"
#include "genesis/core/enum.h"
#include "genesis/core/format.h"
#include "genesis/core/log.h"
#include "genesis/dll/shared_library.h"

#include <array>
#include <hostfxr.h>
#include <nethost.h>
#include <type_traits>

namespace GE::Script {
namespace {

void hostfxrErrorWriter(const char_t* message)
{
    GE_CORE_ERR("[.NET]: {}", message);
}

std::string getHostfxrLibraryPath()
{
    std::array<char, 1024> buffer{};
    size_t                 buffer_size = buffer.size();

    if (int rc = ::get_hostfxr_path(buffer.data(), &buffer_size, nullptr); rc != 0) {
        GE_CORE_ERR("Failed to get hostfxr path: {:#010x}", static_cast<uint32_t>(rc));
        return {};
    }

    return {buffer.data(), buffer_size};
}

} // namespace

class HostFramework::Context
{
public:
    ~Context();

    bool initialize(std::string_view runtime_config_path);
    void shutdown();

    LoadAssemblyAndGetFunctionPointerFn loadAssemblyAndGetFunctionPointerFn() const;
    GetFunctionPointerFn getFunctionPointerFn() const;
    LoadAssemblyFn loadAssemblyFn() const;
    LoadAssemblyBytesFn loadAssemblyBytesFn() const;

    bool isInitialized() const { return m_context != nullptr; }

private:
    using HostfxrInitializeForRuntimeConfigFn =
        FunctionType<hostfxr_initialize_for_runtime_config_fn>;
    using HostfxrCloseFn = FunctionType<hostfxr_close_fn>;
    using HostfxrSetErrorWriterFn = FunctionType<hostfxr_set_error_writer_fn>;
    using HostfxrGetRuntimeDelegateFn = FunctionType<hostfxr_get_runtime_delegate_fn>;

    bool initializeHostfxr(std::string_view runtime_config_path);
    void closeHostfxr();
    bool loadHostfxrFunctions();
    bool getDelegates();

    template<typename Signature>
    bool loadHostfxrFunction(std::function<Signature>* function, std::string_view function_name);

    template<typename Signature>
    bool getRuntimeDelegate(std::function<Signature>* delegate, hostfxr_delegate_type type);

    Dll::SharedLibrary m_library;
    hostfxr_handle     m_context{nullptr};

    HostfxrInitializeForRuntimeConfigFn m_hostfxr_initialize_for_runtime_config;
    HostfxrCloseFn                      m_hostfxr_close;
    HostfxrSetErrorWriterFn             m_hostfxr_set_error_writer;
    HostfxrGetRuntimeDelegateFn         m_hostfxr_get_runtime_delegate;

    LoadAssemblyAndGetFunctionPointerFn m_load_assembly_and_get_function_pointer;
    GetFunctionPointerFn                m_get_function_pointer;
    LoadAssemblyFn                      m_load_assembly;
    LoadAssemblyBytesFn                 m_load_assembly_bytes;
};

HostFramework::Context::~Context()
{
    if (isInitialized()) {
        shutdown();
    }
}

bool HostFramework::Context::initialize(std::string_view runtime_config_path)
{
    GE_CORE_ASSERT(!isInitialized(), "'hostfxr' has already been initialized");
    GE_CORE_INFO("Initializing 'hostfxr' library");

    auto hostfxr_path = getHostfxrLibraryPath();
    if (hostfxr_path.empty()) {
        GE_CORE_ERR("Failed to get 'hostfxr' library path");
        return false;
    }

    GE_CORE_INFO("'hostfxr' library path: '{}'", hostfxr_path);

    if (!m_library.open(hostfxr_path)) {
        GE_CORE_ERR("Failed to open 'hostfxr' library: '{}'", hostfxr_path);
        return false;
    }

    if (!loadHostfxrFunctions()) {
        shutdown();
        return false;
    }

    m_hostfxr_set_error_writer(&hostfxrErrorWriter);

    if (!initializeHostfxr(runtime_config_path) || !getDelegates()) {
        shutdown();
        return false;
    }

    GE_CORE_INFO("'hostfxr' library initialized successfully");
    return true;
}

void HostFramework::Context::shutdown()
{
    GE_CORE_INFO("Shutting down 'hostfxr'");
    closeHostfxr();
    m_library.close();
}

LoadAssemblyAndGetFunctionPointerFn
HostFramework::Context::loadAssemblyAndGetFunctionPointerFn() const
{
    GE_CORE_ASSERT(isInitialized(), "'hostfxr' hasn't been initialized");
    return m_load_assembly_and_get_function_pointer;
}

GetFunctionPointerFn HostFramework::Context::getFunctionPointerFn() const
{
    GE_CORE_ASSERT(isInitialized(), "'hostfxr' hasn't been initialized");
    return m_get_function_pointer;
}

LoadAssemblyFn HostFramework::Context::loadAssemblyFn() const
{
    GE_CORE_ASSERT(isInitialized(), "'hostfxr' hasn't been initialized");
    return m_load_assembly;
}

LoadAssemblyBytesFn HostFramework::Context::loadAssemblyBytesFn() const
{
    GE_CORE_ASSERT(isInitialized(), "'hostfxr' hasn't been initialized");
    return m_load_assembly_bytes;
}

bool HostFramework::Context::initializeHostfxr(std::string_view runtime_config_path)
{
    GE_CORE_ASSERT(!isInitialized(), "'hostfxr' has already been initialized");
    GE_CORE_DBG("Initializing 'hostfxr' context");

    if (int rc = m_hostfxr_initialize_for_runtime_config(runtime_config_path.data(), nullptr,
                                                         &m_context);
        rc != 0 || m_context == nullptr) {
        GE_CORE_ERR("Failed to intialize 'hostfxr' context: hresult={:#010x}, context={:p}",
                    static_cast<uint32_t>(rc), m_context);
        return false;
    }

    return true;
}

void HostFramework::Context::closeHostfxr()
{
    if (m_context != nullptr && m_hostfxr_close) {
        GE_CORE_DBG("Closing 'hostfxr' context");
        m_hostfxr_close(m_context);
    }

    m_context = nullptr;
}

bool HostFramework::Context::loadHostfxrFunctions()
{
    GE_CORE_ASSERT(m_library.isOpen(), "'hostfxr' library hasn't been opened");
    GE_CORE_DBG("Loading 'hostfxr' functions");

    return loadHostfxrFunction(&m_hostfxr_initialize_for_runtime_config,
                               "hostfxr_initialize_for_runtime_config") &&
           loadHostfxrFunction(&m_hostfxr_close, "hostfxr_close") &&
           loadHostfxrFunction(&m_hostfxr_set_error_writer, "hostfxr_set_error_writer") &&
           loadHostfxrFunction(&m_hostfxr_get_runtime_delegate, "hostfxr_get_runtime_delegate");
}

bool HostFramework::Context::getDelegates()
{
    GE_CORE_DBG("Loading 'hostfxr' delegates");

    return getRuntimeDelegate(&m_load_assembly_and_get_function_pointer,
                              hdt_load_assembly_and_get_function_pointer) &&
           getRuntimeDelegate(&m_get_function_pointer, hdt_get_function_pointer) &&
           getRuntimeDelegate(&m_load_assembly, hdt_load_assembly) &&
           getRuntimeDelegate(&m_load_assembly_bytes, hdt_load_assembly_bytes);
}

template<typename Signature>
bool HostFramework::Context::loadHostfxrFunction(std::function<Signature>* function,
                                                 std::string_view          function_name)
{
    GE_CORE_ASSERT(function, "'function' is nullptr");
    GE_CORE_ASSERT(m_library.isOpen(), "'hostfxr' library hasn't been opened");

    if (!m_library.loadFunction(function, function_name)) {
        GE_CORE_ERR("Failed to load '{}' function", function_name);
        return false;
    }

    return true;
}

template<typename Signature>
bool HostFramework::Context::getRuntimeDelegate(std::function<Signature>* delegate,
                                                hostfxr_delegate_type     type)
{
    GE_CORE_ASSERT(delegate, "'delegate' is nullptr");
    GE_CORE_ASSERT(isInitialized(), "'hostfxr' hasn't been initialized");

    void* loaded_delegate{nullptr};

    if (int rc = m_hostfxr_get_runtime_delegate(m_context, type, &loaded_delegate);
        rc != 0 || loaded_delegate == nullptr) {
        GE_CORE_ERR("Failed to get '{}' delegate: hresult={:#010x}, delegate={:p}",
                    GE::toString(type), static_cast<uint32_t>(rc), loaded_delegate);
        return false;
    }

    *delegate = std::add_pointer_t<Signature>(loaded_delegate);
    return true;
}

HostFramework::HostFramework() = default;

HostFramework::~HostFramework() = default;

void* HostFramework::getFunctionPointer(std::string_view assembly_name,
                                        std::string_view managed_class_name,
                                        std::string_view method_name,
                                        std::string_view delegate_type_name)
{
    GE_CORE_ASSERT(isInitialized(), "Host Framework hasn't been initialized");
    GE_CORE_DBG("Loading '{}.{}, {}' method", managed_class_name, method_name, assembly_name);

    // Create an assembly qualified delegate type name if the passed value is not empty, otherwise
    // use 'UNMANAGEDCALLERSONLY_METHOD'.

    std::string   full_delegate_type_name;
    const char_t* full_delegate_name_ptr = UNMANAGEDCALLERSONLY_METHOD;
    if (!delegate_type_name.empty()) {
        full_delegate_type_name = GE_FMTSTR("{}, {}", delegate_type_name, assembly_name);
        full_delegate_name_ptr = full_delegate_type_name.data();
    }

    // Create an assembly qualified type name

    auto type_name = GE_FMTSTR("{}, {}", managed_class_name, assembly_name);

    void* function{nullptr};
    int   rc = m_context->getFunctionPointerFn()(type_name.c_str(), method_name.data(),
                                               full_delegate_name_ptr, nullptr, nullptr, &function);
    if (rc != 0 || function == nullptr) {
        GE_CORE_ERR("Failed to load '{}' method of '{}' class from '{}' assembly: "
                    "hresult={:#010x}, function={:p}",
                    method_name, managed_class_name, assembly_name, static_cast<uint32_t>(rc),
                    function);
        return nullptr;
    }

    return function;
}

bool HostFramework::initialize(std::string_view runtime_config_path)
{
    get()->m_context = std::make_unique<Context>();
    if (!get()->m_context->initialize(runtime_config_path)) {
        shutdown();
        return false;
    }

    return true;
}

void HostFramework::shutdown()
{
    get()->m_context.reset();
}

bool HostFramework::loadAssembly(std::string_view assembly_path)
{
    GE_CORE_ASSERT(isInitialized(), "Host Framework hasn't been initialized");
    GE_CORE_INFO("Loading '{}' assembly", assembly_path);

    int rc = get()->m_context->loadAssemblyFn()(assembly_path.data(), nullptr, nullptr);
    if (rc != 0) {
        GE_CORE_ERR("Failed to load assembly: {:#010x}", static_cast<uint32_t>(rc));
        return false;
    }

    return true;
}

bool HostFramework::isInitialized()
{
    return get()->m_context != nullptr;
}

} // namespace GE::Script
