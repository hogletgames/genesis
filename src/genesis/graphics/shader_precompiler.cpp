/*
 * BSD 3-Clause License
 *
 * Copyright (c) 2021-2022, Dmitry Shilnenkov
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

#include "shader_precompiler.h"

#include "genesis/core/enum.h"
#include "genesis/core/log.h"
#include "genesis/filesystem/file_content.h"

#include <shaderc/shaderc.hpp>

#include <filesystem>

namespace {

std::optional<shaderc_shader_kind> toShaderKind(GE::Shader::Type type)
{
    using Type = GE::Shader::Type;

    switch (type) {
        case Type::VERTEX: return shaderc_vertex_shader;
        case Type::FRAGMENT: return shaderc_fragment_shader;
        default: return {};
    }
}

std::string readShaderCode(const std::string &filepath)
{
    if (auto file = GE::FS::readFile<char>(filepath); !file.empty()) {
        return {file.begin(), file.end()};
    }

    GE_CORE_ERR("Failed to open Shader: '{}'", filepath);
    return {};
}

} // namespace

namespace GE {

ShaderCache ShaderPrecompiler::compileFromFile(Shader::Type shader_type,
                                               const std::string &filepath)
{
    return compileShader(shader_type, readShaderCode(filepath), filepath);
}

ShaderCache ShaderPrecompiler::compileFromSource(Shader::Type shader_type,
                                                 const std::string &source_code)
{
    return compileShader(shader_type, source_code, "<no-filename>");
}

ShaderCache ShaderPrecompiler::compileShader(Shader::Type type, const std::string &source_code,
                                             const std::string &filepath)
{
    if (source_code.empty()) {
        GE_CORE_ERR("Shader Source code is empty");
        return {};
    }

    auto kind = toShaderKind(type);

    if (!kind.has_value()) {
        GE_CORE_ERR("Unsupported Shader Type: {}", toString(type));
        return {};
    }

    shaderc::Compiler compiler;
    auto result = compiler.CompileGlslToSpv(source_code, kind.value(), filepath.c_str());

    if (result.GetCompilationStatus() != shaderc_compilation_status_success) {
        GE_CORE_ERR("Failed to compile '{}': {}", filepath, result.GetErrorMessage());
        return {};
    }

    return {result.begin(), result.end()};
}

ShaderCache ShaderPrecompiler::loadShaderCache(const std::string &filepath)
{
    if (std::ifstream file{filepath, std::ios::binary}; file) {
        file >> std::noskipws;
        return {std::istream_iterator<uint32_t>{file}, std::istream_iterator<uint32_t>{}};
    }

    GE_CORE_ERR("Failed to open Shader Cache: '{}'", filepath);
    return {};
}

bool ShaderPrecompiler::saveShaderCache(const ShaderCache &shader_cache,
                                        const std::string &filepath)
{
    auto cache_dir = std::filesystem::path{filepath}.parent_path();
    std::error_code error_code;

    if (!std::filesystem::exists(cache_dir) &&
        !std::filesystem::create_directories(cache_dir, error_code)) {
        GE_CORE_ERR("Failed to create Shader Cache directory '{}': {}", cache_dir.string(),
                    error_code.message());
        return false;
    }

    if (std::ofstream file{filepath, std::ios::binary}; file) {
        file << std::noskipws;
        std::ranges::copy(shader_cache, std::ostream_iterator<uint32_t>{file});
        return true;
    }

    GE_CORE_ERR("Failed to open Shader Cache file: '{}'", filepath);
    return false;
}

} // namespace GE