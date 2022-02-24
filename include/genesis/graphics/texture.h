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

#ifndef GENESIS_GRAPHICS_TEXTURE_H_
#define GENESIS_GRAPHICS_TEXTURE_H_

#include <genesis/core/interface.h>

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

#endif // GENESIS_GRAPHICS_TEXTURE_H_
