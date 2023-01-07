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

#include "genesis/window/events/event.h"
#include "genesis/window/events/event_dispatcher.h"
#include "genesis/window/events/key_events.h"
#include "genesis/window/events/mouse_events.h"
#include "genesis/window/events/window_events.h"

#include <gtest/gtest.h>

class KeyEventsTest: public testing::Test
{};

TEST_F(KeyEventsTest, KeyPressedEvent)
{
    static constexpr GE::KeyCode code{GE::KeyCode::LALT};
    static constexpr GE::KeyModFlags mod{GE::KeyModFlags::CAPS_LOCK_BIT};
    static constexpr uint32_t repeat_count{49};

    GE::KeyPressedEvent key_pressed{code, mod, repeat_count};

    EXPECT_EQ(key_pressed.getDescriptor(), GE::KeyPressedEvent::getStaticDescriptor());
    EXPECT_EQ(key_pressed.getCode(), code);
    EXPECT_EQ(key_pressed.getMod(), mod);
    EXPECT_EQ(key_pressed.getRepeatCount(), repeat_count);
}

TEST_F(KeyEventsTest, ReleasedEvent)
{
    static constexpr GE::KeyCode code{GE::KeyCode::S};
    static constexpr GE::KeyModFlags mod{GE::KeyModFlags::SUPER_BIT};

    GE::KeyReleasedEvent key_released{code, mod};

    EXPECT_EQ(key_released.getDescriptor(), GE::KeyReleasedEvent::getStaticDescriptor());
    EXPECT_EQ(key_released.getCode(), code);
    EXPECT_EQ(key_released.getMod(), mod);
}

TEST_F(KeyEventsTest, KeyTypedEvent)
{
    const char* typed_text = "key typed event test";

    GE::KeyTypedEvent key_typed{typed_text};

    EXPECT_EQ(key_typed.getDescriptor(), GE::KeyTypedEvent::getStaticDescriptor());
    EXPECT_STREQ(key_typed.getText(), typed_text);
}

class MouseEventsTest: public testing::Test
{};

TEST_F(MouseEventsTest, MouseMovedEvent)
{
    static constexpr GE::Vec2 position{78.0f, 46.0f};

    GE::MouseMovedEvent mouse_moved{position};

    EXPECT_EQ(mouse_moved.getDescriptor(), GE::MouseMovedEvent::getStaticDescriptor());
    EXPECT_EQ(mouse_moved.getPosition(), position);
}

TEST_F(MouseEventsTest, MouseScrolledEvent)
{
    static constexpr GE::Vec2 offset{53.0f, 90.0f};

    GE::MouseScrolledEvent mouse_scrolled{offset};

    EXPECT_EQ(mouse_scrolled.getDescriptor(),
              GE::MouseScrolledEvent::getStaticDescriptor());
    EXPECT_EQ(mouse_scrolled.getOffset(), offset);
}

TEST_F(MouseEventsTest, MouseButtonPressedEvent)
{
    static constexpr GE::MouseButton pressed_button{GE::MouseButton::LEFT};

    GE::MouseButtonPressedEvent mouse_pressed{pressed_button};

    EXPECT_EQ(mouse_pressed.getDescriptor(),
              GE::MouseButtonPressedEvent::getStaticDescriptor());
    EXPECT_EQ(mouse_pressed.getMouseButton(), pressed_button);
}

TEST_F(MouseEventsTest, MouseButtonReleasedEvent)
{
    static constexpr GE::MouseButton released_button{GE::MouseButton::MIDDLE};

    GE::MouseButtonReleasedEvent mouse_released{released_button};

    EXPECT_EQ(mouse_released.getDescriptor(),
              GE::MouseButtonReleasedEvent::getStaticDescriptor());
    EXPECT_EQ(mouse_released.getMouseButton(), released_button);
}

class WindowEventsTest: public testing::Test
{};

TEST_F(WindowEventsTest, WindowResizedEvent)
{
    static constexpr GE::Vec2 size{320.0f, 240.0f};

    GE::WindowResizedEvent win_resized{size};

    EXPECT_EQ(win_resized.getDescriptor(), GE::WindowResizedEvent::getStaticDescriptor());
    EXPECT_EQ(win_resized.size(), size);
}

TEST_F(WindowEventsTest, WindowClosedEvent)
{
    GE::WindowClosedEvent win_closed{};

    EXPECT_EQ(win_closed.getDescriptor(), GE::WindowClosedEvent::getStaticDescriptor());
}

TEST_F(WindowEventsTest, WindowMaximizedEvent)
{
    GE::WindowMaximizedEvent win_maximized{};

    EXPECT_EQ(win_maximized.getDescriptor(),
              GE::WindowMaximizedEvent::getStaticDescriptor());
}

TEST_F(WindowEventsTest, WindowMinimizedEvent)
{
    GE::WindowMinimizedEvent win_minimazed{};

    EXPECT_EQ(win_minimazed.getDescriptor(),
              GE::WindowMinimizedEvent::getStaticDescriptor());
}

TEST_F(WindowEventsTest, WindowRestoredEvent)
{
    GE::WindowRestoredEvent win_restored{};

    EXPECT_EQ(win_restored.getDescriptor(),
              GE::WindowRestoredEvent::getStaticDescriptor());
}
template<typename EventType>
class EventDispatcherTest: public ::testing::Test
{};

using EventTypeList = ::testing::Types<
    // Key events
    GE::KeyPressedEvent, GE::KeyReleasedEvent, GE::KeyTypedEvent,
    // Mouse events
    GE::MouseMovedEvent, GE::MouseScrolledEvent, GE::MouseButtonPressedEvent,
    GE::MouseButtonReleasedEvent,
    // Window events
    GE::WindowResizedEvent, GE::WindowClosedEvent, GE::WindowMaximizedEvent,
    GE::WindowMinimizedEvent, GE::WindowRestoredEvent>;

TYPED_TEST_SUITE(EventDispatcherTest, EventTypeList);

TYPED_TEST(EventDispatcherTest, SuccessfulDispatch)
{
    TypeParam event{};

    GE::EventDispatcher::Callback<TypeParam> callback = [](const TypeParam&) {
        return true;
    };

    GE::EventDispatcher dispatcher{&event};

    EXPECT_FALSE(event.handled());
    EXPECT_TRUE(dispatcher.dispatch(callback));
    EXPECT_TRUE(event.handled());
}
