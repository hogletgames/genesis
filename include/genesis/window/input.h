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

#ifndef GENESIS_WINDOW_INPUT_H_
#define GENESIS_WINDOW_INPUT_H_

#include <genesis/core/interface.h>
#include <genesis/core/memory.h>
#include <genesis/math/math.h>
#include <genesis/window/key_codes.h>
#include <genesis/window/mouse_button_codes.h>

namespace GE {

class GE_API InputController: public Interface
{
public:
    virtual int32_t toNativeKeyCode(KeyCode key_code) const = 0;
    virtual KeyCode fromNativeKeyCode(int32_t key_code) const = 0;
    virtual uint32_t toNativeKeyMod(KeyModFlags key_mod) const = 0;
    virtual KeyModFlags fromNativeKeyMod(uint32_t key_mod) const = 0;
    virtual bool isKeyPressed(KeyCode key_code) const = 0;

    virtual uint8_t toNativeButton(MouseButton button) const = 0;
    virtual MouseButton fromNativeButton(uint8_t button) const = 0;
    virtual bool isButtonPressed(MouseButton button) const = 0;
    virtual Vec2 getMousePosition() const = 0;
};

class GE_API Input
{
public:
    static bool initialize();
    static void shutdown();

    static int32_t toNativeKeyCode(KeyCode key_code)
    {
        return get()->m_pimpl->toNativeKeyCode(key_code);
    }

    static KeyCode fromNativeKeyCode(int32_t key_code)
    {
        return get()->m_pimpl->fromNativeKeyCode(key_code);
    }

    static uint32_t toNativeKeyMod(KeyModFlags mod)
    {
        return get()->m_pimpl->toNativeKeyMod(mod);
    }

    static KeyModFlags fromNativeKeyMod(uint32_t mod)
    {
        return get()->m_pimpl->fromNativeKeyMod(mod);
    }

    static bool isKeyPressed(KeyCode key_code)
    {
        return get()->m_pimpl->isKeyPressed(key_code);
    }

    static uint8_t toNativeButton(MouseButton button)
    {
        return get()->m_pimpl->toNativeButton(button);
    }

    static MouseButton fromNativeButton(uint8_t button)
    {
        return get()->m_pimpl->fromNativeButton(button);
    }

    static bool isButtonPressed(MouseButton button)
    {
        return get()->m_pimpl->isButtonPressed(button);
    }

    static glm::vec2 getMousePosition() { return get()->m_pimpl->getMousePosition(); }

private:
    static Input* get()
    {
        static Input instance;
        return &instance;
    }

    Scoped<InputController> m_pimpl;
};

} // namespace GE

#endif // GENESIS_WINDOW_INPUT_H_
