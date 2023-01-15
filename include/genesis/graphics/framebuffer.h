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
#include <genesis/graphics/renderer.h>
#include <genesis/graphics/texture.h>
#include <genesis/math/types.h>

#include <vector>

namespace GE {

class Texture;

struct fb_attachment_t {
    enum class Type : uint8_t
    {
        UNKNOWN = 0,
        COLOR,
        DEPTH,
        DEPTH_STENCIL
    };

    Type type{Type::UNKNOWN};
    TextureType texture_type{TextureType::UNKNOWN};
    TextureFormat texture_format{TextureFormat::UNKNOWN};
};

class GE_API Framebuffer: public NonCopyable
{
public:
    struct config_t {
        Vec2 size{1920.0f, 1080.0f};
        uint32_t layers{1};
        uint32_t msaa_samples{1};
        Vec4 clear_color{1.0f, 1.0f, 1.0f, 1.0f};
        float clear_depth{1.0f};
        std::vector<fb_attachment_t> attachments = {
            {fb_attachment_t::Type::COLOR, TextureType::TEXTURE_2D,
             TextureFormat::SRGBA8},
            {fb_attachment_t::Type::DEPTH, TextureType::TEXTURE_2D, TextureFormat::D32F},
        };
    };

    virtual void resize(const Vec2& size) = 0;

    virtual Renderer* renderer() = 0;
    virtual const Vec2& size() const = 0;
    virtual uint32_t MSAASamples() const = 0;
    virtual const Vec4& clearColor() const = 0;
    virtual float clearDepth() const = 0;

    virtual const Texture& colorTexture(size_t i = 0) const = 0;
    virtual const Texture& depthTexture() const = 0;
    virtual uint32_t colorAttachmentCount() const = 0;
    virtual bool hasDepthAttachment() const = 0;

    static Scoped<Framebuffer> create(const config_t& config);
};

} // namespace GE
