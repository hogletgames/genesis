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

#ifndef GENESIS_GRAPHICS_GRAPHICS_H_
#define GENESIS_GRAPHICS_GRAPHICS_H_

#include <genesis/core/export.h>
#include <genesis/core/memory.h>
#include <genesis/graphics/graphics_factory.h>

namespace GE {

class GraphicsContext;

class GE_API Graphics
{
public:
    enum class API
    {
        NONE = 0,
        VULKAN
    };

    struct settings_t {
        API api{API_DEFAULT};

        static constexpr API API_DEFAULT{API::VULKAN};
    };

    static API renderAPI();
    static void setContext(Shared<GraphicsContext> context);
    static Shared<GraphicsContext> context();
    static const Scoped<GraphicsFactory>& factory();

private:
    Graphics() = default;

    static Graphics* get()
    {
        static Graphics instance;
        return &instance;
    }

    Shared<GraphicsContext> m_context;
};

Graphics::API toRendererAPI(const std::string& api_str);

} // namespace GE

#endif // GENESIS_GRAPHICS_GRAPHICS_H_
