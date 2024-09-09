/*
 * BSD 3-Clause License
 *
 * Copyright (c) 2024, Dmitry Shilnenkov
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

#include "component_matchers.h"

#include "genesis/filesystem/filepath.h"
#include "genesis/filesystem/tmp_dir_guard.h"
#include "genesis/scene/components/tag_component.h"
#include "genesis/scene/components/yaml_convert.h"
#include "genesis/scene/entity_node.h"
#include "genesis/scene/scene.h"
#include "genesis/scene/scene_serializer.h"

#include <gmock/gmock.h>
#include <gtest/gtest.h>
#include <yaml-cpp/node/parse.h>

using namespace GE::Scene;
using namespace GE::Tests;
using namespace testing;

namespace {

class SceneSerializerTest: public Test
{
protected:
    std::string tmpSceneFilepath() const { return GE::FS::joinPath(tmpDir.path(), "scene.yaml"); }

    GE::FS::TmpDirGuard tmpDir;
    Scene scene;
    SceneSerializer serializer{&scene};
};

TEST_F(SceneSerializerTest, MultipleLevelsOfEntitiesGoFirst)
{
    {
        EntityNode parent_node_1{scene.createEntity("parent 1")};
        parent_node_1.insert(scene.createEntity("parent 2"));

        auto child_node_1 = parent_node_1.appendChild(scene.createEntity("child 1-1"));
        child_node_1.appendChild(scene.createEntity("child 1-2"));
    }

    auto scene_filepath = tmpSceneFilepath();
    ASSERT_TRUE(serializer.serialize(scene_filepath));

    // Parents

    auto scene_yaml = YAML::LoadFile(scene_filepath)["scene"];
    auto parent_yaml_1 = scene_yaml["entities"][0];
    EXPECT_THAT(parent_yaml_1["components"][0].as<TagComponent>(), isTagComponent("parent 1"));

    auto parent_yaml_2 = scene_yaml["entities"][1];
    EXPECT_THAT(parent_yaml_2["components"][0].as<TagComponent>(), isTagComponent("parent 2"));

    // Children of the 1st parent

    auto child_yaml_1 = parent_yaml_1["children"][0];
    EXPECT_THAT(child_yaml_1["components"][0].as<TagComponent>(), isTagComponent("child 1-1"));

    auto child_yaml_2 = child_yaml_1["children"][0];
    EXPECT_THAT(child_yaml_2["components"][0].as<TagComponent>(), isTagComponent("child 1-2"));
}

TEST_F(SceneSerializerTest, MultipleLevelsOfEntitiesInTheMiddle)
{
    {
        EntityNode parent_node_1{scene.createEntity("parent 1")};
        auto parent_node_2 = parent_node_1.insert(scene.createEntity("parent 2"));
        parent_node_2.insert(scene.createEntity("parent 3"));

        auto child_node_1 = parent_node_2.appendChild(scene.createEntity("child 2-1"));
        child_node_1.appendChild(scene.createEntity("child 2-2"));
    }

    auto scene_filepath = tmpSceneFilepath();
    ASSERT_TRUE(serializer.serialize(scene_filepath));

    // Parents

    auto scene_yaml = YAML::LoadFile(scene_filepath)["scene"];
    auto parent_yaml_1 = scene_yaml["entities"][0];
    EXPECT_THAT(parent_yaml_1["components"][0].as<TagComponent>(), isTagComponent("parent 1"));

    auto parent_yaml_2 = scene_yaml["entities"][1];
    EXPECT_THAT(parent_yaml_2["components"][0].as<TagComponent>(), isTagComponent("parent 2"));

    auto parent_yaml_3 = scene_yaml["entities"][2];
    EXPECT_THAT(parent_yaml_3["components"][0].as<TagComponent>(), isTagComponent("parent 3"));

    // Children of the 2nd parent

    auto child_yaml_1 = parent_yaml_2["children"][0];
    EXPECT_THAT(child_yaml_1["components"][0].as<TagComponent>(), isTagComponent("child 2-1"));

    auto child_yaml_2 = child_yaml_1["children"][0];
    EXPECT_THAT(child_yaml_2["components"][0].as<TagComponent>(), isTagComponent("child 2-2"));
}

TEST_F(SceneSerializerTest, MultipleLevelsOfEntitiesGoLast)
{
    {
        EntityNode parent_node_1{scene.createEntity("parent 1")};
        auto parent_node_2 = parent_node_1.insert(scene.createEntity("parent 2"));
        auto parent_node_3 = parent_node_2.insert(scene.createEntity("parent 3"));

        auto child_node_1 = parent_node_3.appendChild(scene.createEntity("child 3-1"));
        child_node_1.appendChild(scene.createEntity("child 3-2"));
    }

    auto scene_filepath = tmpSceneFilepath();
    ASSERT_TRUE(serializer.serialize(scene_filepath));

    // Parents

    auto scene_yaml = YAML::LoadFile(scene_filepath)["scene"];
    auto parent_yaml_1 = scene_yaml["entities"][0];
    EXPECT_THAT(parent_yaml_1["components"][0].as<TagComponent>(), isTagComponent("parent 1"));

    auto parent_yaml_2 = scene_yaml["entities"][1];
    EXPECT_THAT(parent_yaml_2["components"][0].as<TagComponent>(), isTagComponent("parent 2"));

    auto parent_yaml_3 = scene_yaml["entities"][2];
    EXPECT_THAT(parent_yaml_3["components"][0].as<TagComponent>(), isTagComponent("parent 3"));

    // Children of the 3rd parent

    auto child_yaml_1 = parent_yaml_3["children"][0];
    EXPECT_THAT(child_yaml_1["components"][0].as<TagComponent>(), isTagComponent("child 3-1"));

    auto child_yaml_2 = child_yaml_1["children"][0];
    EXPECT_THAT(child_yaml_2["components"][0].as<TagComponent>(), isTagComponent("child 3-2"));
}

} // namespace
