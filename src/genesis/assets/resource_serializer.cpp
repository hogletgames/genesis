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
#include "pipeline_resource.h"
#include "registry.h"
#include "texture_resource.h"
#include "yaml_convert.h"

#include "genesis/core/log.h"

#include <fstream>

namespace GE::Assets {

ResourceSerializer::ResourceSerializer(Registry *registry)
    : m_registry{registry}
{}

void ResourceSerializer::visit(MeshResource *resource)
{
    m_assets["resources"]["meshes"].push_back(*resource);
}

void ResourceSerializer::visit(TextureResource *resource)
{
    m_assets["resources"]["textures"].push_back(*resource);
}

void ResourceSerializer::visit(PipelineResource *resource)
{
    m_assets["resources"]["pipelines"].push_back(*resource);
}

bool ResourceSerializer::serialize(const std::string &config_filepath)
{
    m_registry->visitAll(this);

    if (m_assets.IsNull()) {
        GE_CORE_ERR("Failed to encode an asset registry");
        return false;
    }

    std::ofstream fout{config_filepath};

    if (!fout) {
        GE_CORE_ERR("Failed open an asset config file '{}'", config_filepath);
        return false;
    }

    fout << m_assets;
    return true;
}

} // namespace GE::Assets
