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

#include "input.h"

#include "genesis/core/log.h"

#include <SDL.h>

namespace {

template<typename T, typename U>
std::unordered_map<U, T> swapKeyAndValue(const std::unordered_map<T, U>& map)
{
    std::unordered_map<U, T> swapped_map;

    for (auto [key, value] : map) {
        swapped_map.emplace(value, key);
    }

    return swapped_map;
}

std::unordered_map<SDL_KeyCode, GE::KeyCode> getFromKeyMap()
{
    std::unordered_map<SDL_KeyCode, GE::KeyCode> key_to_sdl_key = {
        {SDLK_UNKNOWN, GE::KeyCode::UNKNOWN},

        {SDLK_BACKSPACE, GE::KeyCode::BACKSPACE},
        {SDLK_TAB, GE::KeyCode::TAB},
        {SDLK_RETURN, GE::KeyCode::RETURN},
        {SDLK_ESCAPE, GE::KeyCode::ESCAPE},

        {SDLK_SPACE, GE::KeyCode::SPACE},
        {SDLK_EXCLAIM, GE::KeyCode::EXCLAIM},
        {SDLK_QUOTEDBL, GE::KeyCode::QUOTE_DBL},
        {SDLK_HASH, GE::KeyCode::HASH},
        {SDLK_PERCENT, GE::KeyCode::PERCENT},
        {SDLK_DOLLAR, GE::KeyCode::DOLLAR},
        {SDLK_AMPERSAND, GE::KeyCode::AMPERSAND},
        {SDLK_QUOTE, GE::KeyCode::QUOTE},
        {SDLK_LEFTPAREN, GE::KeyCode::LEFT_PAREN},
        {SDLK_RIGHTPAREN, GE::KeyCode::RIGHT_PAREN},
        {SDLK_ASTERISK, GE::KeyCode::ASTERISK},
        {SDLK_PLUS, GE::KeyCode::PLUS},
        {SDLK_COMMA, GE::KeyCode::COMMA},
        {SDLK_MINUS, GE::KeyCode::MINUS},
        {SDLK_PERIOD, GE::KeyCode::PERIOD},
        {SDLK_SLASH, GE::KeyCode::SLASH},

        {SDLK_0, GE::KeyCode::D0},
        {SDLK_1, GE::KeyCode::D1},
        {SDLK_2, GE::KeyCode::D2},
        {SDLK_3, GE::KeyCode::D3},
        {SDLK_4, GE::KeyCode::D4},
        {SDLK_5, GE::KeyCode::D5},
        {SDLK_6, GE::KeyCode::D6},
        {SDLK_7, GE::KeyCode::D7},
        {SDLK_8, GE::KeyCode::D8},
        {SDLK_9, GE::KeyCode::D9},

        {SDLK_COLON, GE::KeyCode::COLON},
        {SDLK_SEMICOLON, GE::KeyCode::SEMICOLON},
        {SDLK_LESS, GE::KeyCode::LESS},
        {SDLK_EQUALS, GE::KeyCode::EQUALS},
        {SDLK_GREATER, GE::KeyCode::GREATER},
        {SDLK_QUESTION, GE::KeyCode::QUESTION},
        {SDLK_AT, GE::KeyCode::AT},

        {SDLK_a, GE::KeyCode::A},
        {SDLK_b, GE::KeyCode::B},
        {SDLK_c, GE::KeyCode::C},
        {SDLK_d, GE::KeyCode::D},
        {SDLK_e, GE::KeyCode::E},
        {SDLK_f, GE::KeyCode::F},
        {SDLK_g, GE::KeyCode::G},
        {SDLK_h, GE::KeyCode::H},
        {SDLK_i, GE::KeyCode::I},
        {SDLK_j, GE::KeyCode::J},
        {SDLK_k, GE::KeyCode::K},
        {SDLK_l, GE::KeyCode::L},
        {SDLK_m, GE::KeyCode::M},
        {SDLK_n, GE::KeyCode::N},
        {SDLK_o, GE::KeyCode::O},
        {SDLK_p, GE::KeyCode::P},
        {SDLK_q, GE::KeyCode::Q},
        {SDLK_r, GE::KeyCode::R},
        {SDLK_s, GE::KeyCode::S},
        {SDLK_t, GE::KeyCode::T},
        {SDLK_u, GE::KeyCode::U},
        {SDLK_v, GE::KeyCode::V},
        {SDLK_w, GE::KeyCode::W},
        {SDLK_x, GE::KeyCode::X},
        {SDLK_y, GE::KeyCode::Y},
        {SDLK_z, GE::KeyCode::Z},

        {SDLK_LEFTBRACKET, GE::KeyCode::LEFT_BRACKET},
        {SDLK_BACKSLASH, GE::KeyCode::BACKSLASH},
        {SDLK_RIGHTBRACKET, GE::KeyCode::RIGHT_BRACKET},
        {SDLK_CARET, GE::KeyCode::CARET},
        {SDLK_UNDERSCORE, GE::KeyCode::UNDERSCORE},
        {SDLK_BACKQUOTE, GE::KeyCode::BACK_QUOTE},

        {SDLK_ESCAPE, GE::KeyCode::ESCAPE},
        {SDLK_RETURN, GE::KeyCode::ENTER},
        {SDLK_INSERT, GE::KeyCode::INSERT},
        {SDLK_DELETE, GE::KeyCode::DELETE},
        {SDLK_RIGHT, GE::KeyCode::RIGHT},
        {SDLK_LEFT, GE::KeyCode::LEFT},
        {SDLK_DOWN, GE::KeyCode::DOWN},
        {SDLK_UP, GE::KeyCode::UP},
        {SDLK_PAGEUP, GE::KeyCode::PAGE_UP},
        {SDLK_PAGEDOWN, GE::KeyCode::PAGE_DOWN},
        {SDLK_HOME, GE::KeyCode::HOME},
        {SDLK_END, GE::KeyCode::END},
        {SDLK_CAPSLOCK, GE::KeyCode::CAPS_LOCK},
        {SDLK_SCROLLLOCK, GE::KeyCode::SCROLL_LOCK},
        {SDLK_NUMLOCKCLEAR, GE::KeyCode::NUM_LOCK},
        {SDLK_PRINTSCREEN, GE::KeyCode::PRINT_SCREEN},
        {SDLK_PAUSE, GE::KeyCode::PAUSE},

        {SDLK_F1, GE::KeyCode::F1},
        {SDLK_F2, GE::KeyCode::F2},
        {SDLK_F3, GE::KeyCode::F3},
        {SDLK_F4, GE::KeyCode::F4},
        {SDLK_F5, GE::KeyCode::F5},
        {SDLK_F6, GE::KeyCode::F6},
        {SDLK_F7, GE::KeyCode::F7},
        {SDLK_F8, GE::KeyCode::F8},
        {SDLK_F9, GE::KeyCode::F9},
        {SDLK_F10, GE::KeyCode::F10},
        {SDLK_F11, GE::KeyCode::F11},
        {SDLK_F12, GE::KeyCode::F12},
        {SDLK_F13, GE::KeyCode::F13},
        {SDLK_F14, GE::KeyCode::F14},
        {SDLK_F15, GE::KeyCode::F15},
        {SDLK_F16, GE::KeyCode::F16},
        {SDLK_F17, GE::KeyCode::F17},
        {SDLK_F18, GE::KeyCode::F18},
        {SDLK_F19, GE::KeyCode::F19},
        {SDLK_F20, GE::KeyCode::F20},
        {SDLK_F21, GE::KeyCode::F21},
        {SDLK_F22, GE::KeyCode::F22},
        {SDLK_F23, GE::KeyCode::F23},
        {SDLK_F24, GE::KeyCode::F24},

        {SDLK_KP_0, GE::KeyCode::KP0},
        {SDLK_KP_1, GE::KeyCode::KP1},
        {SDLK_KP_2, GE::KeyCode::KP2},
        {SDLK_KP_3, GE::KeyCode::KP3},
        {SDLK_KP_4, GE::KeyCode::KP4},
        {SDLK_KP_5, GE::KeyCode::KP5},
        {SDLK_KP_6, GE::KeyCode::KP6},
        {SDLK_KP_7, GE::KeyCode::KP7},
        {SDLK_KP_8, GE::KeyCode::KP8},
        {SDLK_KP_9, GE::KeyCode::KP9},
        {SDLK_KP_PERIOD, GE::KeyCode::KP_PERIOD},
        {SDLK_KP_DECIMAL, GE::KeyCode::KP_DECIMAL},
        {SDLK_KP_DIVIDE, GE::KeyCode::KP_DIVIDE},
        {SDLK_KP_MULTIPLY, GE::KeyCode::KP_MULTIPLY},
        {SDLK_KP_MINUS, GE::KeyCode::KP_MINUS},
        {SDLK_KP_PLUS, GE::KeyCode::KP_PLUS},
        {SDLK_KP_ENTER, GE::KeyCode::KP_ENTER},
        {SDLK_KP_EQUALS, GE::KeyCode::KP_EQUAL},

        {SDLK_LSHIFT, GE::KeyCode::LSHIFT},
        {SDLK_LCTRL, GE::KeyCode::LCTRL},
        {SDLK_LALT, GE::KeyCode::LALT},
        {SDLK_LGUI, GE::KeyCode::LSUPER},
        {SDLK_RSHIFT, GE::KeyCode::RSHIFT},
        {SDLK_RCTRL, GE::KeyCode::RCTRL},
        {SDLK_RALT, GE::KeyCode::RALT},
        {SDLK_RGUI, GE::KeyCode::RSUPER},
        {SDLK_MENU, GE::KeyCode::MENU}};

    return key_to_sdl_key;
}

std::unordered_map<GE::KeyCode, SDL_KeyCode> getToKeyMap()
{
    return swapKeyAndValue(getFromKeyMap());
}

std::unordered_map<uint8_t, GE::MouseButton> getFromButtonMap()
{
    std::unordered_map<uint8_t, GE::MouseButton> sdl_button_to_button = {
        {SDL_BUTTON_LEFT, GE::MouseButton::LEFT},
        {SDL_BUTTON_RIGHT, GE::MouseButton::RIGHT},
        {SDL_BUTTON_MIDDLE, GE::MouseButton::MIDDLE},
        {SDL_BUTTON_X1, GE::MouseButton::X1},
        {SDL_BUTTON_X2, GE::MouseButton::X2}};

    return sdl_button_to_button;
}

std::unordered_map<GE::MouseButton, uint8_t> getToButtonMap()
{
    return swapKeyAndValue(getFromButtonMap());
}

std::unordered_map<SDL_Keymod, GE::KeyModFlags> getFromKeyModMap()
{
    std::unordered_map<SDL_Keymod, GE::KeyModFlags> sdl_keymod_to_keymod = {
        {KMOD_SHIFT, GE::KeyModFlags::SHIFT_BIT},
        {KMOD_CTRL, GE::KeyModFlags::CTRL_BIT},
        {KMOD_ALT, GE::KeyModFlags::ALT_BIT},
        {KMOD_GUI, GE::KeyModFlags::SUPER_BIT},
        {KMOD_NUM, GE::KeyModFlags::NUM_LOCK_BIT},
        {KMOD_CAPS, GE::KeyModFlags::CAPS_LOCK_BIT}};

    return sdl_keymod_to_keymod;
}

std::unordered_map<GE::KeyModFlags, SDL_Keymod> getToKeyModMap()
{
    return swapKeyAndValue(getFromKeyModMap());
}

template<typename DstFlags, typename Map, typename SrsFlags>
DstFlags convertBits(const Map& map, SrsFlags bits)
{
    DstFlags ret{};

    for (auto [from_bit, to_bit] : map) {
        if ((bits & from_bit) != 0) {
            ret |= to_bit;
        }
    }

    return ret;
}

} // namespace

namespace GE::SDL {

int32_t InputController::toNativeKeyCode(KeyCode key_code) const
{
    static const auto sdl_key_to_key = getToKeyMap();
    return toType(sdl_key_to_key, key_code, SDLK_UNKNOWN);
}

KeyCode InputController::fromNativeKeyCode(int32_t key_code) const
{
    static const auto key_to_sdl_key = getFromKeyMap();
    return toType(key_to_sdl_key, static_cast<SDL_KeyCode>(key_code), KeyCode::UNKNOWN);
}

uint32_t InputController::toNativeKeyMod(KeyModFlags key_mod) const
{
    static const auto key_mod_to_sdl_key_mod = getToKeyModMap();
    return convertBits<uint32_t>(key_mod_to_sdl_key_mod, key_mod);
}

KeyModFlags InputController::fromNativeKeyMod(uint32_t key_mod) const
{
    static const auto sdl_key_mod_to_key_mod = getFromKeyModMap();
    return convertBits<KeyModFlags>(sdl_key_mod_to_key_mod, key_mod);
}

bool InputController::isKeyPressed(KeyCode key_code) const
{
    const Uint8* keyboard_state{nullptr};

    if (keyboard_state = SDL_GetKeyboardState(nullptr); keyboard_state == nullptr) {
        return false;
    }

    auto sdl_keycode = toNativeKeyCode(key_code);
    auto sdl_scancode = SDL_GetScancodeFromKey(sdl_keycode);

    return keyboard_state[sdl_scancode] != 0;
}

uint8_t InputController::toNativeButton(MouseButton button) const
{
    static const auto button_to_sdl_button = getToButtonMap();
    return toType(button_to_sdl_button, button, uint8_t{0});
}

MouseButton InputController::fromNativeButton(uint8_t button) const
{
    static const auto sdl_button_to_button = getFromButtonMap();
    return toType(sdl_button_to_button, button, MouseButton::UNKNOWN);
}

bool InputController::isButtonPressed(MouseButton button) const
{
    auto mouse_state = SDL_GetMouseState(nullptr, nullptr);
    // NOLINTNEXTLINE(clang-analyzer-core.UndefinedBinaryOperatorResult)
    auto sdl_mouse_button = SDL_BUTTON(toNativeButton(button));
    return (mouse_state & sdl_mouse_button) != 0;
}

Vec2 InputController::getMousePosition() const
{
    int x{};
    int y{};

    SDL_GetMouseState(&x, &y);

    return {x, y};
}

} // namespace GE::SDL
