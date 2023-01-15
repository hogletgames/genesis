/*
 * BSD 3-Clause License
 *
 * Copyright (c) 2022, Dmitry Shilnenkov
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

#include "texture_loader.h"
#include "texture.h"

#include "genesis/core/defer.h"
#include "genesis/core/filesystem.h"
#include "genesis/core/log.h"

#include <stb_image.h>

namespace {

GE::TextureFormat toTextureFormatFromHDR(int channel_count)
{
    switch (channel_count) {
        case 1: return GE::TextureFormat::R32F;
        case 3:
        case 4: return GE::TextureFormat::RGBA32F;
        case 2:
        default: break;
    }

    return GE::TextureFormat::UNKNOWN;
}

GE::TextureFormat toTextureFormat(int channel_count, bool is_hdr)
{
    if (is_hdr) {
        return toTextureFormatFromHDR(channel_count);
    }

    switch (channel_count) {
        case 1: return GE::TextureFormat::R8;
        case 3:
        case 4: return GE::TextureFormat::RGBA8;
        case 2:
        default: break;
    }

    return GE::TextureFormat::UNKNOWN;
}

GE::texture_config_t toTextureConfig(int width, int height, int channel_count, bool is_hdr)
{
    GE::texture_config_t config{};
    config.type = GE::TextureType::TEXTURE_2D;
    config.format = toTextureFormat(channel_count, is_hdr);
    config.width = static_cast<uint32_t>(width);
    config.height = static_cast<uint32_t>(height);
    config.depth = 1;
    config.layers = 1;

    return config;
}

uint32_t toTextureSize(const GE::texture_config_t& config)
{
    return config.width * config.height * GE::toTextureBPP(config.format);
}

void* stbiLoadWrapper(const std::vector<uint8_t>& memory, int* width, int* height,
                      int* channel_count, int desired_channels)
{
    int memory_size = static_cast<int>(memory.size());
    bool is_hdr = ::stbi_is_hdr_from_memory(memory.data(), memory_size) != 0;

    if (!is_hdr) {
        return ::stbi_load_from_memory(memory.data(), memory_size, width, height, channel_count,
                                       desired_channels);
    }

    return ::stbi_loadf_from_memory(memory.data(), memory_size, width, height, channel_count,
                                    desired_channels);
}

bool isNullTextureData(const void* texture_data)
{
    if (texture_data == nullptr) {
        GE_CORE_ERR("STB error: '{}'", ::stbi_failure_reason());
        return true;
    }

    return false;
}

std::pair<void*, GE::texture_config_t> loadStbiTexture(const std::vector<uint8_t>& data)
{
    int width{};
    int height{};
    int channel_count{};
    void* texture_data{nullptr};

    texture_data = stbiLoadWrapper(data, &width, &height, &channel_count, 0);

    if (isNullTextureData(texture_data)) {
        return {};
    }

    if (channel_count == 2) {
        GE_CORE_ERR("Unsupported texture channel count: 2");
        ::stbi_image_free(texture_data);
        return {};
    }

    // Convert 3 to 4 channels
    if (channel_count == 3) {
        ::stbi_image_free(texture_data);
        texture_data = stbiLoadWrapper(data, &width, &height, &channel_count, ::STBI_rgb_alpha);
    }

    if (isNullTextureData(texture_data)) {
        return {};
    }

    int data_size = static_cast<int>(data.size());
    bool is_hdr = ::stbi_is_hdr_from_memory(data.data(), data_size) != 0;

    return {texture_data, toTextureConfig(width, height, channel_count, is_hdr)};
}

} // namespace

namespace GE {

TextureLoader::TextureLoader(std::string filepath)
    : m_filepath{std::move(filepath)}
{}

Scoped<Texture> TextureLoader::load()
{
    ::stbi_set_flip_vertically_on_load(1);
    auto file_data = readFile<uint8_t>(m_filepath);

    if (file_data.empty()) {
        GE_CORE_ERR("Texture '{}' data is empty", m_filepath);
        return nullptr;
    }

    auto [texture_data, config] = loadStbiTexture(file_data);
    Defer texture_data_defer{[data = texture_data] { ::stbi_image_free(data); }};

    if (texture_data == nullptr) {
        // NOLINTNEXTLINE(clang-analyzer-unix.Malloc)
        GE_CORE_ERR("Failed to load texture '{}'", m_filepath);
        return nullptr;
    }

    if (config.format == TextureFormat::UNKNOWN) {
        GE_CORE_ERR("Unsupported format for texture '{}'", m_filepath);
        return nullptr;
    }

    auto texture = Texture::create(config);

    if (!texture) {
        GE_CORE_ERR("Failed to create Genesis Texture object");
        return nullptr;
    }

    uint32_t texture_data_size = toTextureSize(config);
    texture->setData(texture_data, texture_data_size);

    return texture;
}

} // namespace GE
