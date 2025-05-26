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

#include "resource_serializer.h"
#include "genesis/core/asserts.h"
#include "pipeline_resource.h"
#include "registry.h"
#include "texture_resource.h"
#include "yaml_convert.h"

#include "genesis/core/log.h"

#include <fstream>

namespace GE::Assets {
namespace {

bool writeToFile(const std::string& filepath, const YAML::Node& node)
{
    std::ofstream fout{filepath};
    if (!fout) {
        GE_CORE_ERR("Failed to serialize data to config file '{}'", filepath);
        return false;
    }

    fout << node;
    return true;
}

} // namespace

ResourceSerializer::ResourceSerializer(Registry* registry)
    : m_registry{registry}
{}

bool ResourceSerializer::serialize(const std::string& config_filepath)
{
    auto assets = serializeAssets();

    if (assets.IsNull()) {
        GE_CORE_ERR("Failed to encode an asset registry");
        return false;
    }

    return writeToFile(config_filepath, assets);
}

YAML::Node ResourceSerializer::serializeAssets()
{
    YAML::Node assets_node;

    auto resources_node = assets_node["packages"];

    for (const auto* package : m_registry->allPackages()) {
        auto package_node = serializePackage(*package);

        if (writeToFile(package->filepath(), package_node)) {
            resources_node.push_back(package->filepath());
        }
    }

    return assets_node;
}

YAML::Node ResourceSerializer::serializePackage(const Package& package)
{
    YAML::Node package_node;

    package_node["name"] = package.name();
    auto resources_node = package_node["resources"];

    for (const auto& pipeline : package.getAllOf<PipelineResource>()) {
        resources_node[GE::toString(Group::PIPELINES)].push_back(YAML::Node{*pipeline});
    }

    for (const auto& mesh : package.getAllOf<MeshResource>()) {
        resources_node[GE::toString(Group::MESHES)].push_back(YAML::Node{*mesh});
    }

    for (const auto& texture : package.getAllOf<TextureResource>()) {
        resources_node[GE::toString(Group::TEXTURES)].push_back(YAML::Node{*texture});
    }

    return package_node;
}

} // namespace GE::Assets
