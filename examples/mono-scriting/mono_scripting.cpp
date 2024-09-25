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

#include "genesis/core/asserts.h"
#include "genesis/core/log.h"

#include <mono/jit/jit.h>
#include <mono/metadata/assembly.h>
#include <mono/metadata/class.h>
#include <mono/utils/mono-logger.h>

#include <string>

namespace {

constexpr std::string_view DOMAIN_NAME{"GeExamplesRuntime"};
constexpr std::string_view VERSION{"v4.0.30319"};
constexpr std::string_view GE_EXAMPLES_ASSEMBLY_PATH{GE_EXAMPLES_DLL};

// Scripting types

class String
{
public:
    explicit String(MonoString* string)
        : m_string{string}
    {}

    explicit String(MonoObject* string_object)
        : m_string{reinterpret_cast<MonoString*>(string_object)}
    {}

    explicit String(std::string_view string)
        : m_string{mono_string_new(mono_domain_get(), string.data())}
    {}

    bool isValid() const { return m_string != nullptr; }

    MonoString* nativeHandle() const { return m_string; }

    std::string value() const
    {
        if (m_string == nullptr) {
            return {};
        }

        auto* string_utf8 = mono_string_to_utf8(m_string);
        std::string result{string_utf8};
        mono_free(string_utf8);
        return result;
    }

private:
    MonoString* m_string{nullptr};
};

// Scripting type traits

template<typename T>
struct scripting_type_traits_t;

template<>
struct scripting_type_traits_t<std::string_view> {
    using Type = String;
};

template<>
struct scripting_type_traits_t<std::string> {
    using Type = String;
};

template<>
struct scripting_type_traits_t<const char*> {
    using Type = String;
};

template<typename T>
using ScriptingType = typename scripting_type_traits_t<std::decay_t<T>>::Type;

template<typename T>
constexpr void* convertArg(T&& arg)
{
    return ScriptingType<T>{std::forward<T>(arg)}.nativeHandle();
}

// Scripting engine

class ScriptingEngineContext
{
public:
    ~ScriptingEngineContext() { shutdown(); }

    bool initialize(std::string_view domain_name, std::string_view runtime_version)
    {
        m_domain = mono_jit_init_version(domain_name.data(), runtime_version.data());
        return m_domain != nullptr;
    }

    void shutdown()
    {
        if (m_domain != nullptr) {
            mono_jit_cleanup(m_domain);
            m_domain = nullptr;
        }
    }

private:
    MonoDomain* m_domain{nullptr};
};

class Method
{
public:
    Method(MonoObject* object, MonoMethod* method)
        : m_object{object}
        , m_method{method}
    {}

    bool isValid() const { return m_method != nullptr; }

    template<typename... Args>
    std::pair<MonoObject*, bool> invoke(Args&&... args)
    {
        std::array<void*, sizeof...(args)> method_args{convertArg(std::forward<Args>(args))...};

        MonoObject* exception{nullptr};
        auto* return_object =
            mono_runtime_invoke(m_method, m_object, method_args.data(), &exception);
        return {return_object, exception == nullptr};
    }

private:
    MonoObject* m_object{nullptr};
    MonoMethod* m_method{nullptr};
};

class Property
{
public:
    Property(MonoObject* object, MonoProperty* mono_property)
        : m_object{object}
        , m_property{mono_property}
    {}

    bool isValid() const { return m_property != nullptr; }

    template<typename T>
    std::pair<MonoObject*, bool> set(T&& value)
    {
        Method setMethod{m_object, mono_property_get_set_method(m_property)};
        return setMethod.invoke(std::forward<T>(value));
    }

    template<typename T>
    std::optional<T> get()
    {
        Method setMethod{m_object, mono_property_get_get_method(m_property)};
        if (auto result = setMethod.invoke(); result.second) {
            return ScriptingType<T>{result.first}.value();
        }

        return {};
    }

private:
    MonoObject* m_object{nullptr};
    MonoProperty* m_property{nullptr};
};

class Object
{
public:
    explicit Object(MonoObject* mono_object)
        : m_object{mono_object}
    {
        if (m_object != nullptr) {
            mono_runtime_object_init(m_object);
            m_gc_handle = mono_gchandle_new(m_object, 0);
        }
    }

    ~Object()
    {
        if (m_object != nullptr) {
            mono_gchandle_free(m_gc_handle);
        }
    }

    Property getProperty(std::string_view name)
    {
        auto* object_class = mono_object_get_class(m_object);
        return Property{m_object, mono_class_get_property_from_name(object_class, name.data())};
    }

    Method getMethod(std::string_view name, int param_count = -1)
    {
        auto* object_class = mono_object_get_class(m_object);
        return Method{m_object,
                      mono_class_get_method_from_name(object_class, name.data(), param_count)};
    }

    String toString() const { return String{m_object}; }

private:
    MonoObject* m_object{nullptr};
    uint32_t m_gc_handle{0};
};

class Class
{
public:
    explicit Class(MonoClass* mono_class)
        : m_class{mono_class}
    {}

    bool isValid() const { return m_class != nullptr; }

    Object newObject() const { return Object{mono_object_new(mono_domain_get(), m_class)}; }

private:
    MonoClass* m_class{nullptr};
};

class Assembly
{
public:
    bool load(std::string_view assembly_path)
    {
        if (m_assembly = mono_domain_assembly_open(mono_domain_get(), assembly_path.data());
            m_assembly == nullptr) {
            return false;
        }

        m_image = mono_assembly_get_image(m_assembly);
        return true;
    }

    Class getClass(std::string_view class_namespace, std::string_view class_name)
    {
        return Class{mono_class_from_name(m_image, class_namespace.data(), class_name.data())};
    }

private:
    MonoAssembly* m_assembly{nullptr};
    MonoImage* m_image{nullptr};
};

template<typename ReturnType, typename... Signature>
void registerInternalCall(std::string_view name, ReturnType (*method)(Signature...))
{
    mono_add_internal_call(name.data(), reinterpret_cast<const void*>(method));
}

void printMessage(MonoString* message)
{
    String message_string{message};
    GE_ASSERT(message_string.isValid(), "Got invalid message");
    GE_INFO("Got message: '{}'", message_string.value());
}

} // namespace

int main()
{
    GE::Log::initialize({});

    ScriptingEngineContext engine;
    GE_ASSERT(engine.initialize(DOMAIN_NAME, VERSION), "Failed to initialize scripting engine");

    Assembly assembly;
    GE_ASSERT(assembly.load(GE_EXAMPLES_ASSEMBLY_PATH), "Failed to load assembly: '{}'",
              GE_EXAMPLES_ASSEMBLY_PATH);

    auto printer_class = assembly.getClass("Ge.Examples", "MessagePrinter");
    GE_ASSERT(printer_class.isValid(), "Failed to load class: 'Ge.Examples.MessagePrinter'");

    auto printer_object = printer_class.newObject();
    auto message_property = printer_object.getProperty("Message");

    message_property.set("Hello World!");

    auto message = message_property.get<std::string>();
    GE_ASSERT(message.has_value(), "Failed to get 'Message'");
    GE_INFO("Message: '{}'", message.value());

    registerInternalCall("Ge.Examples.MessagePrinter::PrintMessage", &printMessage);
    auto print_method = printer_object.getMethod("Print", 0);
    GE_ASSERT(print_method.isValid(), "Failed to get 'Print' method");

    print_method.invoke();

    return EXIT_SUCCESS;
}
