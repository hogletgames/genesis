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

#include <yaml-cpp/yaml.h>

namespace YAML {

template<>
struct convert<GE::Assets::ResourceID> {
    static bool decode(const Node& node, GE::Assets::ResourceID& id)
    {
        id = GE::Assets::ResourceID{
            node["package"].as<std::string>(),
            node["group"].as<std::string>(),
            node["name"].as<std::string>(),
        };

        return true;
    }

    static Node encode(const GE::Assets::ResourceID& id)
    {
        Node node;
        node.SetStyle(YAML::EmitterStyle::Flow);
        node["package"] = id.package();
        node["group"] = id.group();
        node["name"] = id.name();
        return node;
    }
};

template<>
struct convert<GE::Scoped<GE::Assets::MeshResource>> {
    static bool decode(const Node& node, GE::Scoped<GE::Assets::MeshResource>& resource)
    {
        using GE::Assets::MeshResource;
        using GE::Assets::ResourceID;

        auto id = node["id"].as<ResourceID>();
        auto filepath = node["filepath"].as<std::string>();

        resource = MeshResource::create(id, filepath);
        return resource != nullptr;
    }
};

template<>
struct convert<GE::Assets::MeshResource> {
    static Node encode(const GE::Assets::MeshResource& resource)
    {
        Node node;
        node["id"] = resource.id();
        node["filepath"] = resource.filepath();

        return node;
    }
};

template<>
struct convert<GE::Scoped<GE::Assets::PipelineResource>> {
    static bool decode(const Node& node, GE::Scoped<GE::Assets::PipelineResource>& resource)
    {
        using GE::Assets::PipelineResource;
        using GE::Assets::ResourceID;

        auto id = node["id"].as<ResourceID>();
        auto vertex_shader = node["vertex_shader"].as<std::string>();
        auto fragment_shader = node["fragment_shader"].as<std::string>();

        resource = PipelineResource::create(id, vertex_shader, fragment_shader);
        return resource != nullptr;
    }
};

template<>
struct convert<GE::Assets::PipelineResource> {
    static Node encode(const GE::Assets::PipelineResource& resource)
    {
        Node node;
        node["id"] = resource.id();
        node["vertex_shader"] = resource.vertexShader();
        node["fragment_shader"] = resource.fragmentShader();

        return node;
    }
};

template<>
struct convert<GE::Scoped<GE::Assets::TextureResource>> {
    static bool decode(const Node& node, GE::Scoped<GE::Assets::TextureResource>& resource)
    {
        using GE::Assets::ResourceID;
        using GE::Assets::TextureResource;

        auto id = node["id"].as<ResourceID>();
        auto filepath = node["filepath"].as<std::string>();

        resource = TextureResource::create(id, filepath);
        return resource != nullptr;
    }
};

template<>
struct convert<GE::Assets::TextureResource> {
    static Node encode(const GE::Assets::TextureResource& resource)
    {
        Node node;
        node["id"] = resource.id();
        node["filepath"] = resource.filepath();

        return node;
    }
};

} // namespace YAML
