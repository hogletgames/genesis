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

#include <genesis/assets/yaml_convert.h>
#include <genesis/math/yaml_convert.h>
#include <genesis/scene/components.h>

#include <yaml-cpp/yaml.h>

namespace YAML {

template<>
struct convert<GE::Scene::CameraComponent> {
    using Component = GE::Scene::CameraComponent;
    static constexpr auto TYPE{Component ::NAME};

    static bool decode(const Node& node, Component& camera)
    {
        if (node["type"].as<std::string>() != TYPE) {
            return false;
        }

        camera.fixed_aspect_ratio = node["fixed_aspect_ratio"].as<bool>();
        auto projection_node = node["projection"];

        auto ortho_node = projection_node["orthographic_options"];
        GE::Scene::ProjectionCamera::ortho_options_t ortho_options{
            ortho_node["size"].as<float>(),
            ortho_node["near"].as<float>(),
            ortho_node["far"].as<float>(),
        };
        camera.camera.setOrthoOptions(ortho_options);

        auto perspective_node = projection_node["perspective_options"];
        GE::Scene::ProjectionCamera::perspective_options_t perspective_options{
            perspective_node["fov"].as<float>(),
            perspective_node["near"].as<float>(),
            perspective_node["far"].as<float>(),
        };
        camera.camera.setPerspectiveOptions(perspective_options);

        auto projection_type = projection_node["type"].as<std::string>();
        camera.camera.setType(GE::Scene::toProjectionType(projection_type));

        return true;
    }

    static Node encode(const Component& camera)
    {
        YAML::Node node;
        node["type"] = TYPE.data();
        node["fixed_aspect_ratio"] = camera.fixed_aspect_ratio;

        auto projection_node = node["projection"];
        projection_node["type"] = GE::toString(camera.camera.type());

        const auto& ortho_options = camera.camera.orthographicOptions();
        auto ortho_node = projection_node["orthographic_options"];
        ortho_node["size"] = ortho_options.size;
        ortho_node["near"] = ortho_options.near;
        ortho_node["far"] = ortho_options.far;

        const auto& perspective_options = camera.camera.perspectiveOptions();
        auto perspective_node = projection_node["perspective_options"];
        perspective_node["fov"] = perspective_options.fov;
        perspective_node["near"] = perspective_options.near;
        perspective_node["far"] = perspective_options.far;

        return node;
    }
};

template<>
struct convert<GE::Scene::MaterialComponent> {
    using Component = GE::Scene::MaterialComponent;
    static constexpr auto TYPE{Component ::NAME};

    static bool decode(const Node& node, Component& sprite)
    {
        if (node["type"].as<std::string>() != TYPE) {
            return false;
        }

        auto resources_node = node["resources"];
        sprite.setMaterialID(resources_node["material"].as<GE::Assets::ResourceID>());
        return true;
    }

    static Node encode(const Component& sprite)
    {
        YAML::Node node;
        node["type"] = TYPE.data();

        auto resources_node = node["resources"];
        resources_node["material"] = sprite.materialID();
        return node;
    }
};

template<>
struct convert<GE::Scene::RigidBody2DComponent> {
    using Component = GE::Scene::RigidBody2DComponent;
    static constexpr auto TYPE{Component ::NAME};

    static bool decode(const Node& node, Component& rigid_body)
    {
        if (node["type"].as<std::string>() != TYPE) {
            return false;
        }

        rigid_body.body_type = GE::P2D::toRigidBodyType(node["body_type"].as<std::string>());
        rigid_body.fixed_rotation = node["fixed_rotation"].as<bool>();
        return true;
    }

    static Node encode(const Component& rigid_body)
    {
        YAML::Node node;
        node["type"] = TYPE.data();
        node["body_type"] = GE::toString(rigid_body.body_type);
        node["fixed_rotation"] = rigid_body.fixed_rotation;
        return node;
    }
};

template<>
struct convert<GE::Scene::BoxCollider2DComponent> {
    using Component = GE::Scene::BoxCollider2DComponent;
    static constexpr auto TYPE{Component ::NAME};

    static bool decode(const Node& node, Component& collider)
    {
        if (node["type"].as<std::string>() != TYPE) {
            return false;
        }

        collider.size = node["size"].as<GE::Vec2>();
        collider.center = node["center"].as<GE::Vec2>();
        collider.angle = node["angle"].as<float>();
        return true;
    }

    static Node encode(const Component& collider)
    {
        YAML::Node node;
        node["type"] = TYPE.data();
        node["size"] = collider.size;
        node["center"] = collider.center;
        node["angle"] = collider.angle;
        return node;
    }
};

template<>
struct convert<GE::Scene::CircleCollider2DComponent> {
    using Component = GE::Scene::CircleCollider2DComponent;
    static constexpr auto TYPE{Component ::NAME};

    static bool decode(const Node& node, Component& collider)
    {
        if (node["type"].as<std::string>() != TYPE) {
            return false;
        }

        collider.offset = node["offset"].as<GE::Vec2>();
        collider.radius = node["radius"].as<float>();
        return true;
    }

    static Node encode(const Component& collider)
    {
        YAML::Node node;
        node["type"] = TYPE.data();
        node["offset"] = collider.offset;
        node["radius"] = collider.radius;
        return node;
    }
};

template<>
struct convert<GE::Scene::SpriteComponent> {
    using Component = GE::Scene::SpriteComponent;
    static constexpr auto TYPE{Component ::NAME};

    static bool decode(const Node& node, Component& sprite)
    {
        if (node["type"].as<std::string>() != TYPE) {
            return false;
        }

        sprite.color = node["color"].as<GE::Vec3>();

        auto resources_node = node["resources"];
        sprite.setTextureID(resources_node["texture"].as<GE::Assets::ResourceID>());
        sprite.setMeshID(resources_node["mesh"].as<GE::Assets::ResourceID>());
        return true;
    }

    static Node encode(const Component& sprite)
    {
        YAML::Node node;
        node["type"] = TYPE.data();
        node["color"] = sprite.color;

        auto resources_node = node["resources"];
        resources_node["texture"] = sprite.textureID();
        resources_node["mesh"] = sprite.meshID();

        return node;
    }
};

template<>
struct convert<GE::Scene::TagComponent> {
    using Component = GE::Scene::TagComponent;
    static constexpr auto TYPE{Component ::NAME};

    static bool decode(const Node& node, Component& tag)
    {
        if (node["type"].as<std::string>() != TYPE) {
            return false;
        }

        tag.tag = node["tag"].as<std::string>();
        return true;
    }

    static Node encode(const Component& tag)
    {
        YAML::Node node;
        node["type"] = TYPE.data();
        node["tag"] = tag.tag;
        return node;
    }
};

template<>
struct convert<GE::Scene::TransformComponent> {
    using Component = GE::Scene::TransformComponent;
    static constexpr auto TYPE{Component ::NAME};

    static bool decode(const Node& node, Component& transform)
    {
        if (node["type"].as<std::string>() != TYPE) {
            return false;
        }

        transform.translation = node["translation"].as<GE::Vec3>();
        transform.rotation = node["rotation"].as<GE::Vec3>();
        transform.scale = node["scale"].as<GE::Vec3>();
        return true;
    }

    static Node encode(const Component& transform)
    {
        YAML::Node node;
        node["type"] = TYPE.data();
        node["translation"] = transform.translation;
        node["rotation"] = transform.rotation;
        node["scale"] = transform.scale;
        return node;
    }
};

} // namespace YAML
