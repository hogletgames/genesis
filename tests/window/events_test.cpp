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
    static constexpr auto CODE{GE::KeyCode::LALT};
    static constexpr auto MOD{GE::KeyModFlags::CAPS_LOCK_BIT};
    static constexpr uint32_t REPEAT_COUNT{49};

    GE::KeyPressedEvent event{CODE, MOD, REPEAT_COUNT};

    EXPECT_EQ(event.getDescriptor(), GE::KeyPressedEvent::getStaticDescriptor());
    EXPECT_EQ(event.getCode(), CODE);
    EXPECT_EQ(event.getMod(), MOD);
    EXPECT_EQ(event.getRepeatCount(), REPEAT_COUNT);
}

TEST_F(KeyEventsTest, ReleasedEvent)
{
    static constexpr auto CODE{GE::KeyCode::S};
    static constexpr auto MOD{GE::KeyModFlags::SUPER_BIT};

    GE::KeyReleasedEvent event{CODE, MOD};

    EXPECT_EQ(event.getDescriptor(), GE::KeyReleasedEvent::getStaticDescriptor());
    EXPECT_EQ(event.getCode(), CODE);
    EXPECT_EQ(event.getMod(), MOD);
}

TEST_F(KeyEventsTest, KeyTypedEvent)
{
    static constexpr auto TYPED_TEXT{"key typed event test"};

    GE::KeyTypedEvent event{TYPED_TEXT};

    EXPECT_EQ(event.getDescriptor(), GE::KeyTypedEvent::getStaticDescriptor());
    EXPECT_STREQ(event.getText(), TYPED_TEXT);
}

class MouseEventsTest: public testing::Test
{};

TEST_F(MouseEventsTest, MouseMovedEvent)
{
    static constexpr GE::Vec2 POSITION{78.0f, 46.0f};
    static constexpr uint32_t WINDOW_ID{431};

    GE::MouseMovedEvent event{POSITION, WINDOW_ID};

    EXPECT_EQ(event.getDescriptor(), GE::MouseMovedEvent::getStaticDescriptor());
    EXPECT_EQ(event.getPosition(), POSITION);
}

TEST_F(MouseEventsTest, MouseScrolledEvent)
{
    static constexpr GE::Vec2 OFFSET{53.0f, 90.0f};

    GE::MouseScrolledEvent event{OFFSET};

    EXPECT_EQ(event.getDescriptor(), GE::MouseScrolledEvent::getStaticDescriptor());
    EXPECT_EQ(event.getOffset(), OFFSET);
}

TEST_F(MouseEventsTest, MouseButtonPressedEvent)
{
    static constexpr auto PRESSED_BUTTON{GE::MouseButton::LEFT};

    GE::MouseButtonPressedEvent event{PRESSED_BUTTON};

    EXPECT_EQ(event.getDescriptor(), GE::MouseButtonPressedEvent::getStaticDescriptor());
    EXPECT_EQ(event.getMouseButton(), PRESSED_BUTTON);
}

TEST_F(MouseEventsTest, MouseButtonReleasedEvent)
{
    static constexpr auto released_button{GE::MouseButton::MIDDLE};

    GE::MouseButtonReleasedEvent event{released_button};

    EXPECT_EQ(event.getDescriptor(), GE::MouseButtonReleasedEvent::getStaticDescriptor());
    EXPECT_EQ(event.getMouseButton(), released_button);
}

class WindowEventsTest: public testing::Test
{};

TEST_F(WindowEventsTest, WindowMovedEvent)
{
    static constexpr uint32_t WINDOW_ID{42};
    static constexpr GE::Vec2 POSITION{320.0, 240.0};

    GE::WindowMovedEvent event{WINDOW_ID, POSITION};

    EXPECT_EQ(event.getDescriptor(), GE::WindowMovedEvent::getStaticDescriptor());
    EXPECT_EQ(event.id(), WINDOW_ID);
    EXPECT_EQ(event.position(), POSITION);
}

TEST_F(WindowEventsTest, WindowResizedEvent)
{
    static constexpr uint32_t WINDOW_ID{89};
    static constexpr GE::Vec2 SIZE{320.0f, 240.0f};

    GE::WindowResizedEvent event{WINDOW_ID, SIZE};

    EXPECT_EQ(event.getDescriptor(), GE::WindowResizedEvent::getStaticDescriptor());
    EXPECT_EQ(event.id(), WINDOW_ID);
    EXPECT_EQ(event.size(), SIZE);
}

TEST_F(WindowEventsTest, WindowMinimizedEvent)
{
    static constexpr uint32_t WINDOW_ID{45};

    GE::WindowMinimizedEvent event{WINDOW_ID};

    EXPECT_EQ(event.id(), WINDOW_ID);
    EXPECT_EQ(event.getDescriptor(), GE::WindowMinimizedEvent::getStaticDescriptor());
}

TEST_F(WindowEventsTest, WindowMaximizedEvent)
{
    static constexpr uint32_t WINDOW_ID{63};

    GE::WindowMaximizedEvent event{WINDOW_ID};

    EXPECT_EQ(event.getDescriptor(), GE::WindowMaximizedEvent::getStaticDescriptor());
    EXPECT_EQ(event.id(), WINDOW_ID);
}

TEST_F(WindowEventsTest, WindowRestoredEvent)
{
    static constexpr uint32_t WINDOW_ID{72};

    GE::WindowRestoredEvent event{WINDOW_ID};

    EXPECT_EQ(event.getDescriptor(), GE::WindowRestoredEvent::getStaticDescriptor());
    EXPECT_EQ(event.id(), WINDOW_ID);
}

TEST_F(WindowEventsTest, WindowEnteredEvent)
{
    static constexpr uint32_t WINDOW_ID{12};

    GE::WindowRestoredEvent event{WINDOW_ID};

    EXPECT_EQ(event.getDescriptor(), GE::WindowRestoredEvent::getStaticDescriptor());
    EXPECT_EQ(event.id(), WINDOW_ID);
}

TEST_F(WindowEventsTest, WindowLeftEvent)
{
    static constexpr uint32_t WINDOW_ID{94};

    GE::WindowLeftEvent envent{WINDOW_ID};

    EXPECT_EQ(envent.getDescriptor(), GE::WindowLeftEvent::getStaticDescriptor());
    EXPECT_EQ(envent.id(), WINDOW_ID);
}

TEST_F(WindowEventsTest, WindowFocusGainedEvent)
{
    static constexpr uint32_t WINDOW_ID{82};

    GE::WindowFocusGainedEvent event{WINDOW_ID};

    EXPECT_EQ(event.getDescriptor(), GE::WindowFocusGainedEvent::getStaticDescriptor());
    EXPECT_EQ(event.id(), WINDOW_ID);
}

TEST_F(WindowEventsTest, WindowFocusLostEvent)
{
    static constexpr uint32_t WINDOW_ID{51};

    GE::WindowFocusLostEvent event{WINDOW_ID};

    EXPECT_EQ(event.getDescriptor(), GE::WindowFocusLostEvent::getStaticDescriptor());
    EXPECT_EQ(event.id(), WINDOW_ID);
}

TEST_F(WindowEventsTest, WindowClosedEvent)
{
    static constexpr uint32_t WINDOW_ID{39};

    GE::WindowClosedEvent event{WINDOW_ID};

    EXPECT_EQ(event.getDescriptor(), GE::WindowClosedEvent::getStaticDescriptor());
    EXPECT_EQ(event.id(), WINDOW_ID);
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
    GE::WindowMovedEvent, GE::WindowResizedEvent, GE::WindowMinimizedEvent,
    GE::WindowMaximizedEvent, GE::WindowRestoredEvent, GE::WindowEnteredEvent, GE::WindowLeftEvent,
    GE::WindowFocusGainedEvent, GE::WindowFocusLostEvent, GE::WindowClosedEvent>;

TYPED_TEST_SUITE(EventDispatcherTest, EventTypeList);

TYPED_TEST(EventDispatcherTest, SuccessfulDispatch)
{
    TypeParam event{};

    GE::EventDispatcher::Callback<TypeParam> callback = [](const TypeParam&) { return true; };

    GE::EventDispatcher dispatcher{&event};

    EXPECT_FALSE(event.handled());
    EXPECT_TRUE(dispatcher.dispatch(callback));
    EXPECT_TRUE(event.handled());
}
