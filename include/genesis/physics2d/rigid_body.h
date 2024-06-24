/*
 * BSD 3-Clause License
 *
 * Copyright (c) 2024, Dmitry Shilnenkov
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

#include <genesis/core/enum.h>
#include <genesis/core/interface.h>
#include <genesis/physics2d/rigid_body_shape.h>

#include <cstdint>
#include <string_view>

namespace GE::P2D {

class GE_API RigidBody: public NonCopyable
{
public:
    enum class Type : uint8_t
    {
        STATIC,
        DYNAMIC,
        KINEMATIC,
    };

    virtual void createFixure(const box_body_shape_config_t& shape_config) = 0;
    virtual void createFixure(const circle_body_shape_config_t& shape_config) = 0;

    virtual void setFixedRotation(bool flag) = 0;

    virtual bool isFixedRotation() const = 0;
    virtual Vec2 position() const = 0;
    virtual float angle() const = 0;
};

inline RigidBody::Type toRigidBodyType(std::string_view type_string)
{
    if (auto type = toEnum<RigidBody::Type>(type_string); type.has_value()) {
        return type.value();
    }

    return RigidBody::Type::STATIC;
}

} // namespace GE::P2D

namespace GE {

inline std::string toString(P2D::RigidBody::Type type)
{
    return toString<P2D::RigidBody::Type>(type);
}

} // namespace GE
