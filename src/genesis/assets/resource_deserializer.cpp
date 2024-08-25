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

#include "resource_deserializer.h"
#include "mesh_resource.h"
#include "pipeline_resource.h"
#include "registry.h"
#include "texture_resource.h"
#include "yaml_convert.h"

namespace GE::Assets {

ResourceDeserializer::ResourceDeserializer(Registry* registry)
    : m_assets{registry}
{}

bool ResourceDeserializer::deserialize(const std::string& config_filepath)
{
    try {
        auto assets_node = YAML::LoadFile(config_filepath);

        for (const auto& package_filepath_node : assets_node["packages"]) {
            deserializePackage(package_filepath_node.as<std::string>());
        }
    } catch (const std::exception& e) {
        GE_CORE_ERR("Failed to load assets '{}': '{}'", config_filepath, e.what());
        return false;
    }

    return true;
}

void ResourceDeserializer::deserializePackage(const std::string& package_filepath)
{
    auto package_node = YAML::LoadFile(package_filepath);
    auto package_name = package_node["name"].as<std::string>();

    Package package{package_name, package_filepath};

    auto resources_node = package_node["resources"];
    deserializeMeshes(&package, resources_node);
    deserializePipelines(&package, resources_node);
    deserializeTextures(&package, resources_node);

    m_assets->insertPackage(std::move(package));
}

void ResourceDeserializer::deserializeMeshes(Package* package, const YAML::Node& package_node)
{
    for (const auto& mesh_node : package_node[GE::toString(Group::MESHES)]) {
        deserializeResource<MeshResource>(package, mesh_node);
    }
}

void ResourceDeserializer::deserializePipelines(Package* package, const YAML::Node& package_node)
{
    for (const auto& material_node : package_node[GE::toString(Group::PIPELINES)]) {
        deserializeResource<PipelineResource>(package, material_node);
    }
}

void ResourceDeserializer::deserializeTextures(Package* package, const YAML::Node& package_node)
{
    for (const auto& texture_node : package_node[GE::toString(Group::TEXTURES)]) {
        deserializeResource<TextureResource>(package, texture_node);
    }
}

template<typename T>
void ResourceDeserializer::deserializeResource(Package* package, const YAML::Node& resource_node)
{
    package->createResource<T>(resource_node.as<typename T::config_t>());
}

} // namespace GE::Assets
