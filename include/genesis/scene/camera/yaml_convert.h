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

#include <genesis/math/yaml_convert.h>
#include <genesis/scene/camera/projection_camera.h>
#include <genesis/scene/camera/view_projection_camera.h>

#include <yaml-cpp/node/convert.h>
#include <yaml-cpp/node/node.h>

namespace YAML {

template<>
struct convert<GE::Scene::ProjectionCamera::Type> {
    static bool decode(const Node& node, GE::Scene::ProjectionCamera::Type& type)
    {
        type = GE::Scene::toProjectionType(node.as<std::string>());
        return true;
    }

    static Node encode(const GE::Scene::ProjectionCamera::Type& type)
    {
        Node node;
        node = GE::toString(type);
        return node;
    }
};

template<>
struct convert<GE::Scene::ProjectionCamera::ortho_options_t> {
    static bool decode(const Node& node,
                       GE::Scene::ProjectionCamera::ortho_options_t& ortho_options)
    {
        ortho_options = {
            .size = node["size"].as<float>(),
            .near = node["near"].as<float>(),
            .far = node["far"].as<float>(),
        };

        return true;
    }

    static Node encode(const GE::Scene::ProjectionCamera::ortho_options_t& ortho_options)
    {
        Node node;
        node["size"] = ortho_options.size;
        node["near"] = ortho_options.near;
        node["far"] = ortho_options.far;

        return node;
    }
};

template<>
struct convert<GE::Scene::ProjectionCamera::perspective_options_t> {
    static bool decode(const Node& node,
                       GE::Scene::ProjectionCamera::perspective_options_t& perspective_options)
    {
        perspective_options = {
            .fov = node["fov"].as<float>(),
            .near = node["near"].as<float>(),
            .far = node["far"].as<float>(),
        };

        return true;
    }

    static Node
    encode(const GE::Scene::ProjectionCamera::perspective_options_t& perspective_options)
    {
        Node node;
        node["fov"] = perspective_options.fov;
        node["near"] = perspective_options.near;
        node["far"] = perspective_options.far;

        return node;
    }
};

template<>
struct convert<GE::Scene::ProjectionCamera> {
    static bool decode(const Node& node, GE::Scene::ProjectionCamera& camera)
    {
        using OrthoOptions = GE::Scene::ProjectionCamera::ortho_options_t;
        using PerspectiveOptions = GE::Scene::ProjectionCamera::perspective_options_t;
        using ProjectionType = GE::Scene::ProjectionCamera::Type;

        auto projection_node = node["projection"];
        camera.setOrthoOptions(projection_node["orthographic_options"].as<OrthoOptions>());
        camera.setPerspectiveOptions(
            projection_node["perspective_options"].as<PerspectiveOptions>());
        camera.setType(projection_node["type"].as<ProjectionType>());

        return true;
    }

    static Node encode(const GE::Scene::ProjectionCamera& camera)
    {
        Node node;
        auto projection_node = node["projection"];
        projection_node["type"] = camera.type();
        projection_node["orthographic_options"] = camera.orthographicOptions();
        projection_node["perspective_options"] = camera.perspectiveOptions();

        return node;
    }
};

template<>
struct convert<GE::Scene::ViewProjectionCamera> {
    static bool decode(const Node& node, GE::Scene::ViewProjectionCamera& camera)
    {
        GE::Scene::ProjectionCamera& projection_camera = camera;
        projection_camera = node.as<GE::Scene::ProjectionCamera>();

        camera.setPosition(node["position"].as<GE::Vec3>());
        camera.setRotation(node["rotation"].as<GE::Vec3>());

        return true;
    }

    static Node encode(const GE::Scene::ViewProjectionCamera& camera)
    {
        Node node;

        const GE::Scene::ProjectionCamera& projection_camera = camera;
        node = projection_camera;

        node["position"] = camera.position();
        node["rotation"] = camera.rotation();

        return node;
    }
};

} // namespace YAML
