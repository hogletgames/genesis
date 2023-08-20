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

#include "genesis/core/memory.h"

namespace GE::Assets {
namespace {

std::string toString(const YAML::Node& node)
{
    std::stringstream ss;
    ss << node;
    return ss.str();
}

template<typename T>
Scoped<T> resourceAs(const YAML::Node& node)
{
    try {
        return node.as<Scoped<T>>();
    } catch (const std::exception& e) {
        GE_CORE_ERR("Failed to decode '{}': {}", toString(node), e.what());
        return nullptr;
    }
}

} // namespace

ResourceDeserializer::ResourceDeserializer(Registry* registry)
    : m_registry{registry}
{}

bool ResourceDeserializer::deserialize(const std::string& config_filepath)
{
    auto node = YAML::LoadFile(config_filepath);

    if (node.IsNull()) {
        GE_CORE_ERR("Failed to load an assets config file from the '{}'", config_filepath);
        return false;
    }

    auto resources = node["resources"];
    bool result{true};

    result &= populate<MeshResource>(resources["meshes"]);
    result &= populate<PipelineResource>(resources["pipelines"]);
    result &= populate<TextureResource>(resources["textures"]);

    return result;
}

template<typename T>
bool ResourceDeserializer::populate(const YAML::Node& node)
{
    bool result{true};

    for (const auto& resource_node : node) {
        if (auto resource = resourceAs<T>(resource_node); resource) {
            m_registry->add(std::move(resource));
        } else {
            result = false;
        }
    }

    return result;
}

} // namespace GE::Assets
