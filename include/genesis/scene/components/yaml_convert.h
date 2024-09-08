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
#include <genesis/scene/camera/yaml_convert.h>
#include <genesis/scene/components.h>

#include <yaml-cpp/node/convert.h>
#include <yaml-cpp/node/node.h>

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
        camera.camera = node.as<GE::Scene::ProjectionCamera>();
        return true;
    }

    static Node encode(const Component& camera)
    {
        YAML::Node node;
        node["type"] = TYPE.data();
        node["fixed_aspect_ratio"] = camera.fixed_aspect_ratio;
        node["camera"] = camera.camera;

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
