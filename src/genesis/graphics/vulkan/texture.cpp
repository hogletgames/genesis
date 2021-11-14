/*
 * BSD 3-Clause License
 *
 * Copyright (c) 2021, Dmitry Shilnenkov
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

#include "texture.h"

#include "genesis/core/utils.h"

namespace {

std::unordered_map<GE::TextureFormat, VkFormat> toVkFormatMap()
{
    return {
        {GE::TextureFormat::RGB8, VK_FORMAT_R8G8B8_UNORM},
        {GE::TextureFormat::RGBA8, VK_FORMAT_R8G8B8A8_UNORM},
        {GE::TextureFormat::SRGB8, VK_FORMAT_R8G8B8_SRGB},
        {GE::TextureFormat::SRGBA8, VK_FORMAT_R8G8B8A8_SRGB},
        {GE::TextureFormat::R32F, VK_FORMAT_R32G32B32A32_SFLOAT},
        {GE::TextureFormat::D32F, VK_FORMAT_D32_SFLOAT},
        {GE::TextureFormat::D24S8, VK_FORMAT_D24_UNORM_S8_UINT},
    };
}

} // namespace

namespace GE::Vulkan {

VkFormat toVkFormat(TextureFormat format)
{
    static const auto to_format = toVkFormatMap();
    return toType(to_format, format, VK_FORMAT_UNDEFINED);
}

TextureFormat toTextureFormat(VkFormat format)
{
    static const auto to_format = swapKeyAndValue(toVkFormatMap());
    return toType(to_format, format, TextureFormat::UNKNOWN);
}

} // namespace GE::Vulkan
