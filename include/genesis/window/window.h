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

#ifndef GENESIS_WINDOW_WINDOW_H_
#define GENESIS_WINDOW_WINDOW_H_

#include <genesis/core/interface.h>
#include <genesis/core/memory.h>
#include <genesis/math/types.h>
#include <genesis/renderer/renderer.h>
#include <genesis/window/event_listener.h>

#include <string>

namespace GE {

class RenderContext;

class GE_API Window: public Interface
{
public:
    struct settings_t {
        std::string title{TITLE_DEFAULT};
        Vec2 size{SIZE_DEFAULT};
        bool vsync{VSYNC_DEFAULT};
        Renderer::API render_api{RENDERER_API_DEFAULT};

        static constexpr auto TITLE_DEFAULT = "Genesis";
        static constexpr Vec2 SIZE_DEFAULT{1280.0f, 720.0f};
        static constexpr bool VSYNC_DEFAULT{true};
        static constexpr Renderer::API RENDERER_API_DEFAULT{Renderer::API::VULKAN};
    };

    static bool initialize();
    static void shutdown();

    static Scoped<Window> create(const settings_t& settings);

    virtual void attachEventListener(EventListener* listener) = 0;
    virtual void detachEventListener(EventListener* listener) = 0;

    virtual const Vec2& getSize() const = 0;
    virtual void setVSync(bool enabled) = 0;
    virtual const settings_t& getSettings() const = 0;

    virtual void* getNativeWindow() = 0;
    virtual void* getNativeContext() = 0;
};

} // namespace GE

#endif // GENESIS_WINDOW_WINDOW_H_
