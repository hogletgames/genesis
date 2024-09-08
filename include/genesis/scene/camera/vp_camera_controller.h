/*
 * BSD 3-Clause License
 *
 * Copyright (c) 2023, Dmitry Shilnenkov
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

#include <genesis/core/bit.h>
#include <genesis/core/export.h>
#include <genesis/core/memory.h>
#include <genesis/core/timestamp.h>
#include <genesis/math/types.h>

namespace GE {

class Event;
class KeyPressedEvent;
class KeyReleasedEvent;
class MouseButtonPressedEvent;
class MouseButtonReleasedEvent;
class MouseMovedEvent;
class MouseScrolledEvent;

} // namespace GE

namespace GE::Scene {

class ViewProjectionCamera;

class GE_API VPCameraController
{
public:
    using VPCamera = ViewProjectionCamera;

    enum Mode : uint8_t
    {
        MODE_NONE = 0,
        MODE_ROTATE_BIT = bit(1),
        MODE_SCROLL_ZOOM_BIT = bit(2),
    };

    explicit VPCameraController(Shared<VPCamera> camera = makeShared<VPCamera>());
    ~VPCameraController();

    void onUpdate(Timestamp ts);
    void onEvent(Event* event);

    void setCamera(Shared<VPCamera> camera);
    void setViewport(const Vec2& viewport);

    Shared<VPCamera>& camera() { return m_camera; }
    const Shared<VPCamera>& camera() const { return m_camera; }
    Mode mode() const { return m_mode; }

private:
    bool onMouseButtonPressed(const MouseButtonPressedEvent& event);
    bool onMouseButtonReleased(const MouseButtonReleasedEvent& event);
    bool onMouseMoved(const MouseMovedEvent& event);
    bool onMouseScrolled(const MouseScrolledEvent& event);

    void rotate();
    void zoom(float delta);

    Shared<VPCamera> m_camera;
    Mode m_mode{MODE_NONE};
    Vec2 m_mouse_offset{0.0f};
};

} // namespace GE::Scene
