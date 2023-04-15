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
#include "genesis/window/events/event_dispatcher.h"
#include "genesis/window/events/key_events.h"
#include "genesis/window/events/mouse_events.h"

namespace {

GE::Scene::VPCameraController::Mode toCameraControlModeBit(GE::MouseButton mouse_button)
{
    using CameraMode = GE::Scene::VPCameraController::Mode;

    static const std::unordered_map<GE::MouseButton, CameraMode> TO_MODE = {
        {GE::MouseButton::LEFT, CameraMode::MODE_ROTATE_BIT},
        {GE::MouseButton::RIGHT, CameraMode::MODE_ZOOM_BIT},
        {GE::MouseButton::MIDDLE, CameraMode::MODE_PAN_BIT},
    };

    return getValue(TO_MODE, mouse_button, CameraMode::MODE_NONE);
}

GE::Scene::VPCameraController::Mode toCameraControlModeBit(GE::KeyCode code)
{
    using CameraMode = GE::Scene::VPCameraController::Mode;

    static const std::unordered_map<GE::KeyCode, CameraMode> TO_MODE = {
        {GE::KeyCode::LALT, CameraMode::MODE_ENABLED_BIT},
        {GE::KeyCode::RALT, CameraMode::MODE_ENABLED_BIT},
        {GE::KeyCode::LSUPER, CameraMode::MODE_PAN_BIT},
        {GE::KeyCode::RSUPER, CameraMode::MODE_PAN_BIT},
    };

    return getValue(TO_MODE, code, CameraMode::MODE_NONE);
}

float panSpeedFactor(float viewport)
{
    static constexpr float PAN_MAX{2400.f};
    static constexpr float PAN_COORD_FACTOR{1e-3};

    static constexpr float A{0.0366f};
    static constexpr float B{0.1778f};
    static constexpr float C{0.2021f};

    float x = std::min(viewport, PAN_MAX) * PAN_COORD_FACTOR;
    return A * (x * x) - B * x + C;
}

} // namespace

namespace GE::Scene {

VPCameraController::VPCameraController(VPCameraPtr camera)
    : m_camera{std::move(camera)}
{}

VPCameraController::~VPCameraController() = default;

void VPCameraController::onUpdate([[maybe_unused]] Timestamp ts)
{
    static constexpr float MOUSE_SCROLL_FACTOR{0.1f};

    if (checkBits(m_mode, MODE_PAN_FLAGS)) {
        pan();
    } else if (checkBits(m_mode, MODE_ROTATE_FLAGS)) {
        rotate();
    } else if (checkBits(m_mode, MODE_ZOOM_FLAGS)) {
        zoom(m_mouse_offset.y);
    } else if (checkBits(m_mode, MODE_SCROLL_ZOOM_FLAGS)) {
        zoom(m_mouse_offset.y * MOUSE_SCROLL_FACTOR);
        m_mode = clearBits(m_mode, MODE_SCROLL_ZOOM_BIT);
    }

    m_mouse_offset = {};
}

void VPCameraController::onEvent(Event* event)
{
    EventDispatcher dispatcher{event};
    dispatcher.dispatch<KeyPressedEvent>(toEventHandler(&VPCameraController::onKeyPressed, this));
    dispatcher.dispatch<KeyReleasedEvent>(toEventHandler(&VPCameraController::onKeyReleased, this));
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

bool VPCameraController::onKeyPressed(const KeyPressedEvent& event)
{
    m_mode = setBits(m_mode, toCameraControlModeBit(event.getCode()));
    return false;
}

bool VPCameraController::onKeyReleased(const KeyReleasedEvent& event)
{
    m_mode = clearBits(m_mode, toCameraControlModeBit(event.getCode()));
    return false;
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
    static constexpr float MOUSE_ROTATION_SPEED{0.05f};

    float sign = m_camera->upDirection().y < 0.0f ? -1.0f : 1.0f;
    float pitch = m_camera->pitch() + (m_mouse_offset.x * MOUSE_ROTATION_SPEED);
    float yaw = m_camera->yaw() + (sign * m_mouse_offset.y * MOUSE_ROTATION_SPEED);

    m_camera->setRotationAngles(pitch, yaw);
}

void VPCameraController::zoom(float delta)
{
    float distance = m_camera->distance() - (delta * zoomSpeed());

    if (distance < 1.0f) {
        m_camera->setFocalPoint(m_camera->focalPoint() + m_camera->forwardDirection());
        distance = 1.0f;
    }

    m_camera->setDistance(distance);
}

void VPCameraController::pan()
{
    auto speed = panSpeed();
    auto focal_point = m_camera->focalPoint();
    float distance = m_camera->distance();

    focal_point += m_camera->rightDirection() * m_mouse_offset.x * speed.x * distance;
    focal_point += -m_camera->upDirection() * m_mouse_offset.y * speed.y * distance;
    m_camera->setFocalPoint(focal_point);
}

float VPCameraController::zoomSpeed() const
{
    static constexpr float ZOOM_DISTANCE_FACTOR{0.2f};
    static constexpr float DISTANCE_MIN{0.0f};
    static constexpr float MOUSE_SPEED_MAX{100.0f};

    float distance = std::max(m_camera->distance() * ZOOM_DISTANCE_FACTOR, DISTANCE_MIN);
    float speed = std::min(distance * distance, MOUSE_SPEED_MAX);
    return speed;
}

Vec2 VPCameraController::panSpeed() const
{
    const auto& viewport = m_camera->viewport();
    return {panSpeedFactor(viewport.x), panSpeedFactor(viewport.y)};
}

} // namespace GE::Scene
