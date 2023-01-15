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

#pragma once

#include <genesis/core/interface.h>
#include <genesis/core/memory.h>
#include <genesis/math/types.h>

#include <cstdint>

namespace GE {

enum class TextureType : uint8_t
{
    UNKNOWN = 0,
    TEXTURE_2D
};

enum class TextureFormat : uint8_t
{
    UNKNOWN = 0,
    R8,
    RGB8,
    RGBA8,
    SRGB8,
    SRGBA8,
    R32F,
    RGBA32F,
    D32F,
    D24S8,
    D32S8
};

enum class TextureFilter : uint8_t
{
    AUTO = 0,
    NEAREST,
    NEAREST_MIPMAP_NEAREST,
    NEAREST_MIPMAP_LINEAR,
    LINEAR,
    LINEAR_MIPMAP_NEAREST,
    LINEAR_MIPMAP_LINEAR
};

enum class TextureWrap : uint32_t
{
    AUTO = 0,
    CLAMP_TO_EDGE,
    CLAMP_TO_BORDER,
    REPEAT,
    MIRROR_REPEAT
};

struct texture_config_t {
    TextureType type{TextureType::UNKNOWN};
    TextureFormat format{TextureFormat::SRGB8};
    uint32_t width{1};
    uint32_t height{1};
    uint8_t depth{1};
    uint8_t layers{1};
    TextureFilter min_filter{TextureFilter::LINEAR_MIPMAP_LINEAR};
    TextureFilter mag_filter{TextureFilter::LINEAR};
    TextureWrap wrap_u{TextureWrap::REPEAT};
    TextureWrap wrap_v{TextureWrap::REPEAT};
    TextureWrap wrap_w{TextureWrap::REPEAT};
    uint32_t mip_levels{MIP_LEVELS_AUTO};
    bool image_storage{false};

    static constexpr uint32_t MIP_LEVELS_AUTO{0};
};

class Texture: public Interface
{
public:
    using NativeID = void*;

    virtual bool setData(const void* data, uint32_t size) = 0;
    virtual const Vec2& size() const = 0;
    virtual TextureFormat format() const = 0;
    virtual NativeID nativeID() const = 0;

    static Scoped<Texture> create(const texture_config_t& config);

    static uint32_t calculateMipLevels(uint32_t width, uint32_t height);
};

inline constexpr bool isDepthFormat(TextureFormat format)
{
    switch (format) {
        case TextureFormat::D32F:
        case TextureFormat::D24S8:
        case TextureFormat::D32S8: return true;
        default: return false;
    }
}

inline constexpr bool isColorFormat(TextureFormat format)
{
    return !isDepthFormat(format);
}

inline constexpr uint32_t toTextureBPP(TextureFormat format)
{
    switch (format) {
        case TextureFormat::R8: return 1;
        case TextureFormat::RGB8:
        case TextureFormat::SRGB8: return 3;
        case TextureFormat::RGBA8:
        case TextureFormat::SRGBA8:
        case TextureFormat::R32F: return 4;
        case TextureFormat::RGBA32F: return 16;
        default: return 0;
    }
}

} // namespace GE
