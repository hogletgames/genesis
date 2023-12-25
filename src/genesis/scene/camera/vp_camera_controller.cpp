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

#include "camera/vp_camera_controller.h"
#include "camera/view_projection_camera.h"

#include "genesis/core/utils.h"
#include "genesis/math/trigonometric.h"
#include "genesis/window/events/event_dispatcher.h"
#include "genesis/window/events/key_events.h"
#include "genesis/window/events/mouse_events.h"

namespace {

GE::Scene::VPCameraController::Mode toCameraControlModeBit(GE::MouseButton mouse_button)
{
    using CameraMode = GE::Scene::VPCameraController::Mode;

    static const std::unordered_map<GE::MouseButton, CameraMode> TO_MODE = {
        {GE::MouseButton::RIGHT, CameraMode::MODE_ROTATE_BIT},
    };

    return getValue(TO_MODE, mouse_button, CameraMode::MODE_NONE);
}

} // namespace

namespace GE::Scene {

VPCameraController::VPCameraController(VPCameraPtr camera)
    : m_camera{std::move(camera)}
{}

VPCameraController::~VPCameraController() = default;

void VPCameraController::onUpdate([[maybe_unused]] Timestamp ts)
{
    if (checkBits(m_mode, MODE_ROTATE_BIT)) {
        rotate();
    } else if (checkBits(m_mode, MODE_SCROLL_ZOOM_BIT)) {
        zoom(m_mouse_offset.y);
        m_mode = clearBits(m_mode, MODE_SCROLL_ZOOM_BIT);
    }

    m_mouse_offset = {};
}

void VPCameraController::onEvent(Event* event)
{
    EventDispatcher dispatcher{event};
    dispatcher.dispatch<MouseButtonPressedEvent>(
        toEventHandler(&VPCameraController::onMouseButtonPressed, this));
    dispatcher.dispatch<MouseButtonReleasedEvent>(
        toEventHandler(&VPCameraController::onMouseButtonReleased, this));
    dispatcher.dispatch<MouseMovedEvent>(toEventHandler(&VPCameraController::onMouseMoved, this));
    dispatcher.dispatch<MouseScrolledEvent>(
        toEventHandler(&VPCameraController::onMouseScrolled, this));
}

void VPCameraController::setCamera(VPCameraPtr camera)
{
    m_camera = std::move(camera);
}

void VPCameraController::setViewport(const Vec2& viewport)
{
    m_camera->setViewport(viewport);
}

bool VPCameraController::onMouseButtonPressed(const MouseButtonPressedEvent& event)
{
    m_mode = setBits(m_mode, toCameraControlModeBit(event.getMouseButton()));
    return false;
}

bool VPCameraController::onMouseButtonReleased(const MouseButtonReleasedEvent& event)
{
    m_mode = clearBits(m_mode, toCameraControlModeBit(event.getMouseButton()));
    return false;
}

bool VPCameraController::onMouseMoved(const MouseMovedEvent& event)
{
    m_mouse_offset = event.getOffset();
    return false;
}

bool VPCameraController::onMouseScrolled(const MouseScrolledEvent& event)
{
    m_mouse_offset = event.getOffset();
    m_mode = setBits(m_mode, MODE_SCROLL_ZOOM_BIT);
    return false;
}

void VPCameraController::rotate()
{
    static constexpr float MOUSE_ROTATION_SPEED{0.2f};

    Vec3 rotation{m_mouse_offset.y, m_mouse_offset.x, 0.0f};
    m_camera->rotate(radians(rotation * MOUSE_ROTATION_SPEED));
}

void VPCameraController::zoom(float delta)
{
    static constexpr float ZOOM_DISTANCE_FACTOR{0.2f};

    auto new_position = m_camera->position() + m_camera->direction() * delta * ZOOM_DISTANCE_FACTOR;
    m_camera->setPosition(new_position);
}

} // namespace GE::Scene
