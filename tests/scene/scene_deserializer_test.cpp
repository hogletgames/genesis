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

#include "genesis/assets/registry.h"
#include "genesis/filesystem/file.h"
#include "genesis/filesystem/filepath.h"
#include "genesis/filesystem/tmp_dir_guard.h"
#include "genesis/scene/components.h"
#include "genesis/scene/entity_node.h"
#include "genesis/scene/scene.h"
#include "genesis/scene/scene_deserializer.h"

#include <gmock/gmock.h>
#include <gtest/gtest.h>

#include <fstream>
#include <string>

using namespace GE::Scene;
using namespace GE::Tests;
using namespace testing;

namespace {

void writeToFile(std::string_view filename, std::string_view content)
{
    std::ofstream file{filename.data()};
    ASSERT_TRUE(file);
    file << content;
}

class SceneDeserializerTest: public Test
{
protected:
    static void SetUpTestCase()
    {
        GE::Log::initialize({GE::Logger::Level::ERROR, GE::Logger::Level::ERROR});
    }

    std::string tmpSceneFilepath() const { return GE::FS::joinPath(tmpDir.path(), "scene.yaml"); }

    GE::FS::TmpDirGuard  tmpDir;
    Scene                scene;
    GE::Assets::Registry assets;
    SceneDeserializer    deserializer{&scene, &assets};
};

TEST_F(SceneDeserializerTest, MultipleLevelsOfEntitiesGoFirst)
{
    constexpr std::string_view SCENE_FILE = R"(
scene:
  name: "TestScene"
  serialization_version: 1
  entities:
    - components:
        - type: Tag
          tag: "parent 1"
      children:
        - components:
            - type: Tag
              tag: "child 1-1"
          children:
            - components:
                - type: Tag
                  tag: "child 1-2"
    - components:
        - type: Tag
          tag: "parent 2"
)";

    auto scene_filepath = tmpSceneFilepath();
    writeToFile(scene_filepath, SCENE_FILE);

    ASSERT_TRUE(deserializer.deserialize(scene_filepath));

    auto parent1 = EntityNode{scene.headEntity()};
    ASSERT_FALSE(parent1.isNull());
    EXPECT_THAT(parent1.entity().get<TagComponent>(), isTagComponent("parent 1"));
    EXPECT_TRUE(parent1.hasNextNode());
    ASSERT_TRUE(parent1.hasChildNode());

    auto child1 = parent1.childNode();
    ASSERT_FALSE(child1.isNull());
    EXPECT_THAT(child1.entity().get<TagComponent>(), isTagComponent("child 1-1"));
    EXPECT_FALSE(child1.hasNextNode());
    ASSERT_TRUE(child1.hasChildNode());

    auto child2 = child1.childNode();
    ASSERT_FALSE(child2.isNull());
    EXPECT_THAT(child2.entity().get<TagComponent>(), isTagComponent("child 1-2"));
    EXPECT_FALSE(child2.hasNextNode());
    EXPECT_FALSE(child2.hasChildNode());

    auto parent2 = parent1.nextNode();
    ASSERT_FALSE(parent2.isNull());
    EXPECT_THAT(parent2.entity().get<TagComponent>(), isTagComponent("parent 2"));
    EXPECT_FALSE(parent2.hasNextNode());
    EXPECT_FALSE(parent2.hasChildNode());
}

TEST_F(SceneDeserializerTest, MultipleLevelsOfEntitiesInTheMiddle)
{
    constexpr std::string_view SCENE_FILE = R"(
scene:
  name: "TestScene"
  serialization_version: 1
  entities:
    - components:
        - type: Tag
          tag: "parent 1"
    - components:
        - type: Tag
          tag: "parent 2"
      children:
        - components:
            - type: Tag
              tag: "child 2-1"
          children:
            - components:
                - type: Tag
                  tag: "child 2-2"
    - components:
        - type: Tag
          tag: "parent 3"
)";

    auto scene_filepath = tmpSceneFilepath();
    writeToFile(scene_filepath, SCENE_FILE);

    ASSERT_TRUE(deserializer.deserialize(scene_filepath));

    auto parent1 = EntityNode{scene.headEntity()};
    ASSERT_FALSE(parent1.isNull());
    EXPECT_THAT(parent1.entity().get<TagComponent>(), isTagComponent("parent 1"));
    EXPECT_TRUE(parent1.hasNextNode());
    EXPECT_FALSE(parent1.hasChildNode());

    auto parent2 = parent1.nextNode();
    ASSERT_FALSE(parent2.isNull());
    EXPECT_THAT(parent2.entity().get<TagComponent>(), isTagComponent("parent 2"));
    EXPECT_TRUE(parent2.hasNextNode());
    ASSERT_TRUE(parent2.hasChildNode());

    auto child1 = parent2.childNode();
    ASSERT_FALSE(child1.isNull());
    EXPECT_THAT(child1.entity().get<TagComponent>(), isTagComponent("child 2-1"));
    EXPECT_FALSE(child1.hasNextNode());
    ASSERT_TRUE(child1.hasChildNode());

    auto child2 = child1.childNode();
    ASSERT_FALSE(child2.isNull());
    EXPECT_THAT(child2.entity().get<TagComponent>(), isTagComponent("child 2-2"));
    EXPECT_FALSE(child2.hasNextNode());
    EXPECT_FALSE(child2.hasChildNode());

    auto parent3 = parent2.nextNode();
    ASSERT_FALSE(parent3.isNull());
    EXPECT_THAT(parent3.entity().get<TagComponent>(), isTagComponent("parent 3"));
    EXPECT_FALSE(parent3.hasNextNode());
    EXPECT_FALSE(parent3.hasChildNode());
}

TEST_F(SceneDeserializerTest, MultipleLevelsOfEntitiesGoLast)
{
    constexpr std::string_view SCENE_FILE = R"(
scene:
  name: "TestScene"
  serialization_version: 1
  entities:
    - components:
        - type: Tag
          tag: "parent 1"
    - components:
        - type: Tag
          tag: "parent 2"
      children:
        - components:
            - type: Tag
              tag: "child 2-1"
          children:
            - components:
                - type: Tag
                  tag: "child 2-2"
)";

    auto scene_filepath = tmpSceneFilepath();
    writeToFile(scene_filepath, SCENE_FILE);

    ASSERT_TRUE(deserializer.deserialize(scene_filepath));

    auto parent1 = EntityNode{scene.headEntity()};
    ASSERT_FALSE(parent1.isNull());
    EXPECT_THAT(parent1.entity().get<TagComponent>(), isTagComponent("parent 1"));
    EXPECT_TRUE(parent1.hasNextNode());
    EXPECT_FALSE(parent1.hasChildNode());

    auto parent2 = parent1.nextNode();
    ASSERT_FALSE(parent2.isNull());
    EXPECT_THAT(parent2.entity().get<TagComponent>(), isTagComponent("parent 2"));
    EXPECT_FALSE(parent2.hasNextNode());
    ASSERT_TRUE(parent2.hasChildNode());

    auto child1 = parent2.childNode();
    ASSERT_FALSE(child1.isNull());
    EXPECT_THAT(child1.entity().get<TagComponent>(), isTagComponent("child 2-1"));
    EXPECT_FALSE(child1.hasNextNode());
    ASSERT_TRUE(child1.hasChildNode());

    auto child2 = child1.childNode();
    ASSERT_FALSE(child2.isNull());
    EXPECT_THAT(child2.entity().get<TagComponent>(), isTagComponent("child 2-2"));
    EXPECT_FALSE(child2.hasNextNode());
    EXPECT_FALSE(child2.hasChildNode());
}

TEST_F(SceneDeserializerTest, EmptyChildren)
{
    constexpr std::string_view SCENE_FILE = R"(
scene:
  name: "TestScene"
  serialization_version: 1
  entities:
    - components:
        - type: Tag
          tag: "parent 1"
      children:
)";

    auto scene_filepath = tmpSceneFilepath();
    writeToFile(scene_filepath, SCENE_FILE);

    ASSERT_FALSE(deserializer.deserialize(scene_filepath));
}

} // namespace
