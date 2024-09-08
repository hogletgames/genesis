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

#include <genesis/assets/mesh_resource.h>
#include <genesis/assets/pipeline_resource.h>
#include <genesis/assets/resource_id.h>
#include <genesis/assets/texture_resource.h>

#include <yaml-cpp/node/convert.h>
#include <yaml-cpp/node/node.h>

namespace YAML {

template<>
struct convert<GE::Assets::ResourceID> {
    static bool decode(const Node& node, GE::Assets::ResourceID& id)
    {
        auto group = GE::toEnum<GE::Assets::Group>(node["group"].as<std::string>());
        if (!group.has_value()) {
            return false;
        }

        id = GE::Assets::ResourceID{
            node["package"].as<std::string>(),
            group.value(),
            node["name"].as<std::string>(),
        };

        return true;
    }

    static Node encode(const GE::Assets::ResourceID& id)
    {
        Node node;
        node.SetStyle(YAML::EmitterStyle::Flow);
        node["package"] = id.package();
        node["group"] = GE::toString(id.group());
        node["name"] = id.name();
        return node;
    }
};

template<>
struct convert<GE::Assets::MeshResource::config_t> {
    static bool decode(const Node& node, GE::Assets::MeshResource::config_t& resource_data)
    {
        resource_data.name = node["name"].as<std::string>();
        resource_data.filepath = node["filepath"].as<std::string>();
        return true;
    }
};

template<>
struct convert<GE::Assets::MeshResource> {
    static Node encode(const GE::Assets::MeshResource& resource)
    {
        Node node;
        node["name"] = resource.id().name();
        node["filepath"] = resource.filepath();
        return node;
    }
};

template<>
struct convert<GE::Assets::PipelineResource::config_t> {
    static bool decode(const Node& node, GE::Assets::PipelineResource::config_t& config)
    {
        config.name = node["name"].as<std::string>();
        config.vertex_shader_path = node["vertex_shader_path"].as<std::string>();
        config.fragment_shader_path = node["fragment_shader_path"].as<std::string>();
        return true;
    }
};

template<>
struct convert<GE::Assets::PipelineResource> {
    static Node encode(const GE::Assets::PipelineResource& resource)
    {
        Node node;
        node["name"] = resource.id().name();
        node["vertex_shader_path"] = resource.vertexShaderPath();
        node["fragment_shader_path"] = resource.fragmentShaderPath();
        return node;
    }
};

template<>
struct convert<GE::Assets::TextureResource::config_t> {
    static bool decode(const Node& node, GE::Assets::TextureResource::config_t& config)
    {
        config.name = node["name"].as<std::string>();
        config.filepath = node["filepath"].as<std::string>();
        return true;
    }
};

template<>
struct convert<GE::Assets::TextureResource> {
    static Node encode(const GE::Assets::TextureResource& resource)
    {
        Node node;
        node["name"] = resource.id().name();
        node["filepath"] = resource.filepath();
        return node;
    }
};

} // namespace YAML
