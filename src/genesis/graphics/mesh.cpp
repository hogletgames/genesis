/*
 * BSD 3-Clause License
 *
 * Copyright (c) 2022, Dmitry Shilnenkov
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

#include "mesh.h"
#include "index_buffer.h"
#include "vertex.h"
#include "vertex_buffer.h"

#include "genesis/core/log.h"

#include <tiny_obj_loader.h>

namespace GE {

Mesh::Mesh() = default;

Mesh::~Mesh() = default;

bool Mesh::fromObj(std::string_view filepath)
{
    tinyobj::ObjReaderConfig config{};
    config.triangulate = true;

    tinyobj::ObjReader reader;

    if (!reader.ParseFromFile(filepath.data(), config)) {
        GE_CORE_ERR("Failed to load '{}', warn: '{}', error: '{}'", filepath, reader.Warning(),
                    reader.Error());
        return false;
    }

    return populateBuffers(reader);
}

void Mesh::setBuffers(Scoped<VertexBuffer> vbo, Scoped<IndexBuffer> ibo)
{
    m_vbo = std::move(vbo);
    m_ibo = std::move(ibo);
}

void Mesh::draw(GPUCommandQueue* queue) const
{
    m_vbo->bind(queue);
    m_ibo->bind(queue);
    m_vbo->draw(queue, m_ibo.get());
}

void Mesh::destroy()
{
    m_ibo.reset();
    m_vbo.reset();
}

bool Mesh::populateBuffers(const tinyobj::ObjReader& reader)
{
    const auto&                            attrib = reader.GetAttrib();
    std::unordered_map<vertex_t, uint32_t> unique_vertices;
    std::vector<vertex_t>                  vertices;
    std::vector<uint32_t>                  indices;

    for (const auto& shape : reader.GetShapes()) {
        for (const auto& index : shape.mesh.indices) {
            vertex_t vertex = {
                .position = {attrib.vertices[(3 * index.vertex_index) + 0],
                             attrib.vertices[(3 * index.vertex_index) + 1],
                             attrib.vertices[(3 * index.vertex_index) + 2]},
                .color = {attrib.colors[(3 * index.vertex_index) + 0],
                          attrib.colors[(3 * index.vertex_index) + 1],
                          attrib.colors[(3 * index.vertex_index) + 2]},
                .tex_coord = {attrib.texcoords[(2 * index.texcoord_index) + 0],
                              attrib.texcoords[(2 * index.texcoord_index) + 1]},
            };

            if (!unique_vertices.contains(vertex)) {
                unique_vertices[vertex] = vertices.size();
                vertices.push_back(vertex);
            }

            indices.push_back(unique_vertices[vertex]);
        }
    }

    if (m_vbo = VertexBuffer::create(vertices.size() * sizeof(vertex_t), vertices.data());
        m_vbo == nullptr) {
        GE_CORE_ERR("Failed to create Vertex Buffer");
        return false;
    }

    if (m_ibo = IndexBuffer::create(indices.data(), indices.size()); m_ibo == nullptr) {
        GE_CORE_ERR("Failed to create Index Buffer");
        m_vbo.reset();
        return false;
    }

    return true;
}

} // namespace GE
