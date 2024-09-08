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

#include <genesis/math/types.h>

#include <yaml-cpp/node/convert.h>
#include <yaml-cpp/node/node.h>

namespace YAML {

template<>
struct convert<GE::Vec2> {
    static bool decode(const Node& node, GE::Vec2& vec)
    {
        vec = {
            node[0].as<float>(),
            node[1].as<float>(),
        };

        return true;
    }

    static Node encode(const GE::Vec2& vec)
    {
        Node node;
        node.SetStyle(EmitterStyle::Flow);
        node.push_back(vec.x);
        node.push_back(vec.y);

        return node;
    }
};

template<>
struct convert<GE::Vec3> {
    static bool decode(const Node& node, GE::Vec3& vec)
    {
        vec = {
            node[0].as<float>(),
            node[1].as<float>(),
            node[2].as<float>(),
        };

        return true;
    }

    static Node encode(const GE::Vec3& vec)
    {
        Node node;
        node.SetStyle(EmitterStyle::Flow);
        node.push_back(vec.x);
        node.push_back(vec.y);
        node.push_back(vec.z);

        return node;
    }
};

template<>
struct convert<GE::Vec4> {
    static bool decode(const Node& node, GE::Vec4& vec)
    {
        vec = {
            node[0].as<float>(),
            node[1].as<float>(),
            node[2].as<float>(),
            node[3].as<float>(),
        };

        return true;
    }

    static Node encode(const GE::Vec4& vec)
    {
        Node node;
        node.SetStyle(EmitterStyle::Flow);
        node.push_back(vec.x);
        node.push_back(vec.y);
        node.push_back(vec.z);
        node.push_back(vec.w);

        return node;
    }
};

template<>
struct convert<GE::Mat4> {
    static bool decode(const Node& node, GE::Mat4& mat)
    {
        mat = {
            {node[0].as<GE::Vec4>()},
            {node[1].as<GE::Vec4>()},
            {node[2].as<GE::Vec4>()},
            {node[3].as<GE::Vec4>()},
        };

        return true;
    }

    static Node encode(const GE::Mat4& mat)
    {
        Node node;
        node.push_back(mat[0]);
        node.push_back(mat[1]);
        node.push_back(mat[2]);
        node.push_back(mat[3]);

        return node;
    }
};

template<>
struct convert<GE::Quat> {
    static bool decode(const Node& node, GE::Quat& quat)
    {
        quat = {
            node.as<GE::Mat4>(),
        };

        return true;
    }

    static Node encode(const GE::Quat& quat)
    {
        Node node;
        node = toMat4(quat);
        return node;
    }
};

}; // namespace YAML
