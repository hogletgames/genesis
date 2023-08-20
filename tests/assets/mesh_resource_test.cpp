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

#include "genesis/assets/mesh_resource.h"
#include "genesis/graphics/graphics.h"

#include <gtest/gtest.h>

namespace {

class MeshResourceTest: public testing::Test
{
protected:
    static void SetUpTestSuite();
    static void TearDownTestSuite();

    static constexpr auto MESH_PATH{"tests/assets/data/mesh_square.obj"};

    GE::Assets::ResourceID id{"genesis", "test", "texture"};
};

void MeshResourceTest::SetUpTestSuite() {
    GE::Graphics::initialize({});
}

void MeshResourceTest::TearDownTestSuite() {
    GE::Graphics::shutdown();
}

TEST_F(MeshResourceTest, MeshID)
{
    auto mesh = GE::Assets::MeshResource::create(id, MESH_PATH);

    ASSERT_NE(mesh, nullptr);
    EXPECT_EQ(mesh->id(), id);
    EXPECT_NE(mesh->mesh(), nullptr);
}

} // namespace
