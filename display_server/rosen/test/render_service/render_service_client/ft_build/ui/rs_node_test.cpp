/*
 * Copyright (c) 2021-2023 Huawei Device Co., Ltd.
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 *     http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 */

#include <memory>
#include "gtest/gtest.h"
#include "animation/rs_animation.h"
#include "animation/rs_transition.h"
#include "modifier/rs_property_modifier.h"
#include "ui/rs_canvas_node.h"
#include "ui/rs_surface_node.h"

namespace OHOS::Rosen {
class RSNodeTest : public testing::Test {
public:
    constexpr static float floatData[] = {
        0.0f, 485.44f, -34.4f,
        std::numeric_limits<float>::max(), std::numeric_limits<float>::min(),
        };
    static void SetUpTestCase();
    static void TearDownTestCase();
    void SetUp() override;
    void TearDown() override;
};

void RSNodeTest::SetUpTestCase() {}
void RSNodeTest::TearDownTestCase() {}
void RSNodeTest::SetUp() {}
void RSNodeTest::TearDown() {}

/**
 * @tc.name: LifeCycle001
 * @tc.desc:
 * @tc.type:FUNC
 */
TEST_F(RSNodeTest, LifeCycle001)
{
    /**
     * @tc.steps: step1. create RSNode and add child
     */
    auto rootNode = RSCanvasNode::Create();
    ASSERT_TRUE(rootNode != nullptr);

    auto child1 = RSCanvasNode::Create();
    auto child2 = RSCanvasNode::Create();
    auto child3 = RSCanvasNode::Create();
    rootNode->AddChild(child1, -1);
    rootNode->AddChild(child2, 0);
    child1->AddChild(child3, 1);

    EXPECT_EQ(rootNode->GetId() + 1, child1->GetId());

    /**
     * @tc.steps: step2. remove child
     */
    rootNode->RemoveChild(child2);
}

/**
 * @tc.name: LifeCycle002
 * @tc.desc:
 * @tc.type:FUNC
 */
TEST_F(RSNodeTest, LifeCycle002)
{
    /**
     * @tc.steps: step1. create RSNode and add child
     */
    auto rootNode = RSCanvasNode::Create();
    ASSERT_TRUE(rootNode != nullptr);

    auto child1 = RSCanvasNode::Create();
    auto child2 = RSCanvasNode::Create();
    auto child3 = RSCanvasNode::Create();
    rootNode->AddChild(child1, -1);
    rootNode->AddChild(child2, 0);
    child1->AddChild(child3, 1);
    /**
     * @tc.steps: step2. remove child
     */
    rootNode->RemoveChild(child2);
}

/**
 * @tc.name: LifeCycle003
 * @tc.desc:
 * @tc.type:FUNC
 */
TEST_F(RSNodeTest, LifeCycle003)
{
    /**
     * @tc.steps: step1. create RSNode and add child
     */
    auto rootNode = RSCanvasNode::Create();
    ASSERT_TRUE(rootNode != nullptr);

    auto child1 = RSCanvasNode::Create();
    auto child2 = RSCanvasNode::Create();
    auto child3 = RSCanvasNode::Create();
    rootNode->AddChild(child1, -1);
    rootNode->AddChild(child2, 0);
    child1->AddChild(child3, 1);
    /**
     * @tc.steps: step2. remove child
     */
    rootNode->RemoveFromTree();
}

/**
 * @tc.name: LifeCycle004
 * @tc.desc:
 * @tc.type:FUNC
 */
TEST_F(RSNodeTest, LifeCycle004)
{
    /**
     * @tc.steps: step1. create RSNode and add child
     */
    auto rootNode = RSCanvasNode::Create();
    ASSERT_TRUE(rootNode != nullptr);

    auto child1 = RSCanvasNode::Create();
    auto child2 = RSCanvasNode::Create();
    auto child3 = RSCanvasNode::Create();
    rootNode->AddChild(child1, -1);
    rootNode->AddChild(child2, 0);
    child1->AddChild(child3, 1);
    /**
     * @tc.steps: step2. remove child
     */
    rootNode->ClearChildren();
}

/**
 * @tc.name: LifeCycle005
 * @tc.desc:
 * @tc.type:FUNC
 */
TEST_F(RSNodeTest, LifeCycle005)
{
    /**
     * @tc.steps: step1. create RSNode and add child
     */
    auto rootNode = RSCanvasNode::Create();
    ASSERT_TRUE(rootNode != nullptr);

    auto child1 = RSCanvasNode::Create();
    auto child2 = RSCanvasNode::Create();
    auto child3 = RSCanvasNode::Create();
    rootNode->AddChild(child1, -1);
    rootNode->AddChild(child2, 0);
    child1->AddChild(child3, 1);
    /**
     * @tc.steps: step2. remove child
     */
    rootNode->RemoveChild(child3);
}

/**
 * @tc.name: Recording001
 * @tc.desc:
 * @tc.type:FUNC
 */
TEST_F(RSNodeTest, Recording001)
{
    /**
     * @tc.steps: step1. create RSNode and RSUIDirector
     */
    auto rsNode = RSCanvasNode::Create();

    /**
     * @tc.steps: step2. begin recording
     */
    EXPECT_FALSE(rsNode->IsRecording());
    rsNode->BeginRecording(500, 400);
    EXPECT_TRUE(rsNode->IsRecording());
}

/**
 * @tc.name: SetPaintOrder001
 * @tc.desc:
 * @tc.type:FUNC
 */
TEST_F(RSNodeTest, SetPaintOrder001)
{
    /**
     * @tc.steps: step1. create RSNode and RSUIDirector
     */
    auto rsNode = RSCanvasNode::Create();
    rsNode->SetPaintOrder(true);
}

/**
 * @tc.name: SetandGetBounds001
 * @tc.desc:
 * @tc.type:FUNC
 */
TEST_F(RSNodeTest, SetandGetBounds001)
{
    auto rsNode = RSCanvasNode::Create();
    rsNode->SetBounds(floatData[0], floatData[1], floatData[2], floatData[3]);
    EXPECT_TRUE(ROSEN_EQ(rsNode->GetStagingProperties().GetBounds().x_, floatData[0]));
    EXPECT_TRUE(ROSEN_EQ(rsNode->GetStagingProperties().GetBounds().y_, floatData[1]));
    EXPECT_TRUE(ROSEN_EQ(rsNode->GetStagingProperties().GetBounds().z_, floatData[2]));
    EXPECT_TRUE(ROSEN_EQ(rsNode->GetStagingProperties().GetBounds().w_, floatData[3]));
}

/**
 * @tc.name: SetandGetBounds002
 * @tc.desc:
 * @tc.type:FUNC
 */
TEST_F(RSNodeTest, SetandGetBounds002)
{
    auto rsNode = RSCanvasNode::Create();
    rsNode->SetBounds(floatData[3], floatData[1], floatData[2], floatData[0]);
    EXPECT_TRUE(ROSEN_EQ(rsNode->GetStagingProperties().GetBounds().x_, floatData[3]));
    EXPECT_TRUE(ROSEN_EQ(rsNode->GetStagingProperties().GetBounds().y_, floatData[1]));
    EXPECT_TRUE(ROSEN_EQ(rsNode->GetStagingProperties().GetBounds().z_, floatData[2]));
    EXPECT_TRUE(ROSEN_EQ(rsNode->GetStagingProperties().GetBounds().w_, floatData[0]));
}

/**
 * @tc.name: SetandGetBounds003
 * @tc.desc:
 * @tc.type:FUNC
 */
TEST_F(RSNodeTest, SetandGetBounds003)
{
    auto rsNode = RSCanvasNode::Create();
    rsNode->SetBounds(floatData[3], floatData[2], floatData[1], floatData[0]);
    EXPECT_TRUE(ROSEN_EQ(rsNode->GetStagingProperties().GetBounds().x_, floatData[3]));
    EXPECT_TRUE(ROSEN_EQ(rsNode->GetStagingProperties().GetBounds().y_, floatData[2]));
    EXPECT_TRUE(ROSEN_EQ(rsNode->GetStagingProperties().GetBounds().z_, floatData[1]));
    EXPECT_TRUE(ROSEN_EQ(rsNode->GetStagingProperties().GetBounds().w_, floatData[0]));
}

/**
 * @tc.name: SetandGetBoundsWidth001
 * @tc.desc:
 * @tc.type:FUNC
 */
TEST_F(RSNodeTest, SetandGetBoundsWidth001)
{
    auto rsNode = RSCanvasNode::Create();
    rsNode->SetBoundsWidth(floatData[1]);
    rsNode->SetBoundsWidth(floatData[1]);
    EXPECT_TRUE(ROSEN_EQ(rsNode->GetStagingProperties().GetBounds().z_, floatData[1]));
}

/**
 * @tc.name: SetandGetBoundsWidth002
 * @tc.desc:
 * @tc.type:FUNC
 */
TEST_F(RSNodeTest, SetandGetBoundsWidth002)
{
    auto rsNode = RSCanvasNode::Create();
    rsNode->SetBoundsWidth(floatData[2]);
    rsNode->SetBoundsWidth(floatData[2]);
    EXPECT_TRUE(ROSEN_EQ(rsNode->GetStagingProperties().GetBounds().z_, floatData[2]));
}

/**
 * @tc.name: SetandGetBoundsWidth003
 * @tc.desc:
 * @tc.type:FUNC
 */
TEST_F(RSNodeTest, SetandGetBoundsWidth003)
{
    auto rsNode = RSCanvasNode::Create();
    rsNode->SetBoundsWidth(floatData[3]);
    EXPECT_TRUE(ROSEN_EQ(rsNode->GetStagingProperties().GetBounds().z_, floatData[3]));
}

/**
 * @tc.name: SetandGetBoundsWidth004
 * @tc.desc:
 * @tc.type:FUNC
 */
TEST_F(RSNodeTest, SetandGetBoundsWidth004)
{
    auto rsNode = RSCanvasNode::Create();
    rsNode->SetBoundsWidth(floatData[4]);
    EXPECT_TRUE(ROSEN_EQ(rsNode->GetStagingProperties().GetBounds().z_, floatData[4]));
}

/**
 * @tc.name: SetandGetBoundsWidth005
 * @tc.desc:
 * @tc.type:FUNC
 */
TEST_F(RSNodeTest, SetandGetBoundsWidth005)
{
    auto rsNode = RSCanvasNode::Create();
    rsNode->SetBoundsWidth(floatData[0]);
    EXPECT_TRUE(ROSEN_EQ(rsNode->GetStagingProperties().GetBounds().z_, floatData[0]));
}

/**
 * @tc.name: SetandGetBoundsHeight001
 * @tc.desc:
 * @tc.type:FUNC
 */
TEST_F(RSNodeTest, SetandGetBoundsHeight001)
{
    auto rsNode = RSCanvasNode::Create();
    rsNode->SetBoundsHeight(floatData[1]);
    rsNode->SetBoundsHeight(floatData[1]);
    EXPECT_TRUE(ROSEN_EQ(rsNode->GetStagingProperties().GetBounds().w_, floatData[1]));
}

/**
 * @tc.name: SetandGetBoundsHeight002
 * @tc.desc:
 * @tc.type:FUNC
 */
TEST_F(RSNodeTest, SetandGetBoundsHeight002)
{
    auto rsNode = RSCanvasNode::Create();
    rsNode->SetBoundsHeight(floatData[2]);
    rsNode->SetBoundsHeight(floatData[2]);
    EXPECT_TRUE(ROSEN_EQ(rsNode->GetStagingProperties().GetBounds().w_, floatData[2]));
}

/**
 * @tc.name: SetandGetBoundsHeight003
 * @tc.desc:
 * @tc.type:FUNC
 */
TEST_F(RSNodeTest, SetandGetBoundsHeight003)
{
    auto rsNode = RSCanvasNode::Create();
    rsNode->SetBoundsHeight(floatData[3]);
    EXPECT_TRUE(ROSEN_EQ(rsNode->GetStagingProperties().GetBounds().w_, floatData[3]));
}

/**
 * @tc.name: SetandGetBoundsHeight004
 * @tc.desc:
 * @tc.type:FUNC
 */
TEST_F(RSNodeTest, SetandGetBoundsHeight004)
{
    auto rsNode = RSCanvasNode::Create();
    rsNode->SetBoundsHeight(floatData[4]);
    EXPECT_TRUE(ROSEN_EQ(rsNode->GetStagingProperties().GetBounds().w_, floatData[4]));
}

/**
 * @tc.name: SetandGetBoundsHeight005
 * @tc.desc:
 * @tc.type:FUNC
 */
TEST_F(RSNodeTest, SetandGetBoundsHeight005)
{
    auto rsNode = RSCanvasNode::Create();
    rsNode->SetBoundsHeight(floatData[0]);
    EXPECT_TRUE(ROSEN_EQ(rsNode->GetStagingProperties().GetBounds().w_, floatData[0]));
}

/**
 * @tc.name: SetandGetFrame001
 * @tc.desc:
 * @tc.type:FUNC
 */
TEST_F(RSNodeTest, SetandGetFrame001)
{
    auto rsNode = RSCanvasNode::Create();
    rsNode->SetFrame(floatData[0], floatData[1], floatData[2], floatData[3]);
    EXPECT_TRUE(ROSEN_EQ(rsNode->GetStagingProperties().GetFrame().x_, floatData[0]));
    EXPECT_TRUE(ROSEN_EQ(rsNode->GetStagingProperties().GetFrame().y_, floatData[1]));
    EXPECT_TRUE(ROSEN_EQ(rsNode->GetStagingProperties().GetFrame().z_, floatData[2]));
    EXPECT_TRUE(ROSEN_EQ(rsNode->GetStagingProperties().GetFrame().w_, floatData[3]));
}

/**
 * @tc.name: SetandGetFrame002
 * @tc.desc:
 * @tc.type:FUNC
 */
TEST_F(RSNodeTest, SetandGetFrame002)
{
    auto rsNode = RSCanvasNode::Create();
    rsNode->SetFrame(floatData[3], floatData[1], floatData[2], floatData[0]);
    EXPECT_TRUE(ROSEN_EQ(rsNode->GetStagingProperties().GetFrame().x_, floatData[3]));
    EXPECT_TRUE(ROSEN_EQ(rsNode->GetStagingProperties().GetFrame().y_, floatData[1]));
    EXPECT_TRUE(ROSEN_EQ(rsNode->GetStagingProperties().GetFrame().z_, floatData[2]));
    EXPECT_TRUE(ROSEN_EQ(rsNode->GetStagingProperties().GetFrame().w_, floatData[0]));
}

/**
 * @tc.name: SetandGetFrame003
 * @tc.desc:
 * @tc.type:FUNC
 */
TEST_F(RSNodeTest, SetandGetFrame003)
{
    auto rsNode = RSCanvasNode::Create();
    rsNode->SetFrame(floatData[3], floatData[2], floatData[1], floatData[0]);
    EXPECT_TRUE(ROSEN_EQ(rsNode->GetStagingProperties().GetFrame().x_, floatData[3]));
    EXPECT_TRUE(ROSEN_EQ(rsNode->GetStagingProperties().GetFrame().y_, floatData[2]));
    EXPECT_TRUE(ROSEN_EQ(rsNode->GetStagingProperties().GetFrame().z_, floatData[1]));
    EXPECT_TRUE(ROSEN_EQ(rsNode->GetStagingProperties().GetFrame().w_, floatData[0]));
}

/**
 * @tc.name: SetandGetFramePositionX001
 * @tc.desc:
 * @tc.type:FUNC
 */
TEST_F(RSNodeTest, SetandGetFramePositionX001)
{
    auto rsNode = RSCanvasNode::Create();
    rsNode->SetFramePositionX(floatData[1]);
    rsNode->SetFramePositionX(floatData[1]);
    EXPECT_TRUE(ROSEN_EQ(rsNode->GetStagingProperties().GetFrame().x_, floatData[1]));
}

/**
 * @tc.name: SetandGetFramePositionX002
 * @tc.desc:
 * @tc.type:FUNC
 */
TEST_F(RSNodeTest, SetandGetFramePositionX002)
{
    auto rsNode = RSCanvasNode::Create();
    rsNode->SetFramePositionX(floatData[2]);
    rsNode->SetFramePositionX(floatData[2]);
    EXPECT_TRUE(ROSEN_EQ(rsNode->GetStagingProperties().GetFrame().x_, floatData[2]));
}

/**
 * @tc.name: SetandGetFramePositionX003
 * @tc.desc:
 * @tc.type:FUNC
 */
TEST_F(RSNodeTest, SetandGetFramePositionX003)
{
    auto rsNode = RSCanvasNode::Create();
    rsNode->SetFramePositionX(floatData[3]);
    EXPECT_TRUE(ROSEN_EQ(rsNode->GetStagingProperties().GetFrame().x_, floatData[3]));
}

/**
 * @tc.name: SetandGetFramePositionX004
 * @tc.desc:
 * @tc.type:FUNC
 */
TEST_F(RSNodeTest, SetandGetFramePositionX004)
{
    auto rsNode = RSCanvasNode::Create();
    rsNode->SetFramePositionX(floatData[4]);
    EXPECT_TRUE(ROSEN_EQ(rsNode->GetStagingProperties().GetFrame().x_, floatData[4]));
}

/**
 * @tc.name: SetandGetFramePositionX005
 * @tc.desc:
 * @tc.type:FUNC
 */
TEST_F(RSNodeTest, SetandGetFramePositionX005)
{
    auto rsNode = RSCanvasNode::Create();
    rsNode->SetFramePositionX(floatData[0]);
    EXPECT_TRUE(ROSEN_EQ(rsNode->GetStagingProperties().GetFrame().x_, floatData[0]));
}

/**
 * @tc.name: SetandGetFramePositionY001
 * @tc.desc:
 * @tc.type:FUNC
 */
TEST_F(RSNodeTest, SetandGetFramePositionY001)
{
    auto rsNode = RSCanvasNode::Create();
    rsNode->SetFramePositionY(floatData[1]);
    rsNode->SetFramePositionY(floatData[1]);
    EXPECT_TRUE(ROSEN_EQ(rsNode->GetStagingProperties().GetFrame().y_, floatData[1]));
}

/**
 * @tc.name: SetandGetFramePositionY002
 * @tc.desc:
 * @tc.type:FUNC
 */
TEST_F(RSNodeTest, SetandGetFramePositionY002)
{
    auto rsNode = RSCanvasNode::Create();
    rsNode->SetFramePositionY(floatData[2]);
    rsNode->SetFramePositionY(floatData[2]);
    EXPECT_TRUE(ROSEN_EQ(rsNode->GetStagingProperties().GetFrame().y_, floatData[2]));
}

/**
 * @tc.name: SetandGetFramePositionY003
 * @tc.desc:
 * @tc.type:FUNC
 */
TEST_F(RSNodeTest, SetandGetFramePositionY003)
{
    auto rsNode = RSCanvasNode::Create();
    rsNode->SetFramePositionY(floatData[3]);
    EXPECT_TRUE(ROSEN_EQ(rsNode->GetStagingProperties().GetFrame().y_, floatData[3]));
}

/**
 * @tc.name: SetandGetFramePositionY004
 * @tc.desc:
 * @tc.type:FUNC
 */
TEST_F(RSNodeTest, SetandGetFramePositionY004)
{
    auto rsNode = RSCanvasNode::Create();
    rsNode->SetFramePositionY(floatData[4]);
    EXPECT_TRUE(ROSEN_EQ(rsNode->GetStagingProperties().GetFrame().y_, floatData[4]));
}

/**
 * @tc.name: SetandGetFramePositionY005
 * @tc.desc:
 * @tc.type:FUNC
 */
TEST_F(RSNodeTest, SetandGetFramePositionY005)
{
    auto rsNode = RSCanvasNode::Create();
    rsNode->SetFramePositionY(floatData[0]);
    EXPECT_TRUE(ROSEN_EQ(rsNode->GetStagingProperties().GetFrame().y_, floatData[0]));
}

/**
 * @tc.name: SetandGetPositionZ001
 * @tc.desc:
 * @tc.type:FUNC
 */
TEST_F(RSNodeTest, SetandGetPositionZ001)
{
    auto rsNode = RSCanvasNode::Create();
    rsNode->SetPositionZ(floatData[1]);
    EXPECT_TRUE(ROSEN_EQ(rsNode->GetStagingProperties().GetPositionZ(), floatData[1]));
}

/**
 * @tc.name: SetandGetPositionZ002
 * @tc.desc:
 * @tc.type:FUNC
 */
TEST_F(RSNodeTest, SetandGetPositionZ002)
{
    auto rsNode = RSCanvasNode::Create();
    rsNode->SetPositionZ(floatData[2]);
    EXPECT_TRUE(ROSEN_EQ(rsNode->GetStagingProperties().GetPositionZ(), floatData[2]));
}

/**
 * @tc.name: SetandGetPositionZ003
 * @tc.desc:
 * @tc.type:FUNC
 */
TEST_F(RSNodeTest, SetandGetPositionZ003)
{
    auto rsNode = RSCanvasNode::Create();
    rsNode->SetPositionZ(floatData[3]);
    EXPECT_TRUE(ROSEN_EQ(rsNode->GetStagingProperties().GetPositionZ(), floatData[3]));
}

/**
 * @tc.name: SetandGetPositionZ004
 * @tc.desc:
 * @tc.type:FUNC
 */
TEST_F(RSNodeTest, SetandGetPositionZ004)
{
    auto rsNode = RSCanvasNode::Create();
    rsNode->SetPositionZ(floatData[4]);
    EXPECT_TRUE(ROSEN_EQ(rsNode->GetStagingProperties().GetPositionZ(), floatData[4]));
}

/**
 * @tc.name: SetandGetPositionZ005
 * @tc.desc:
 * @tc.type:FUNC
 */
TEST_F(RSNodeTest, SetandGetPositionZ005)
{
    auto rsNode = RSCanvasNode::Create();
    rsNode->SetPositionZ(floatData[0]);
    EXPECT_TRUE(ROSEN_EQ(rsNode->GetStagingProperties().GetPositionZ(), floatData[0]));
}

/**
 * @tc.name: SetandGetCornerRadius001
 * @tc.desc:
 * @tc.type:FUNC
 */
TEST_F(RSNodeTest, SetandGetCornerRadius001)
{
    auto rsNode = RSCanvasNode::Create();
    rsNode->SetCornerRadius(floatData[1]);
    EXPECT_TRUE(ROSEN_EQ(rsNode->GetStagingProperties().GetCornerRadius().x_, floatData[1]));
}

/**
 * @tc.name: SetandGetCornerRadius002
 * @tc.desc:
 * @tc.type:FUNC
 */
TEST_F(RSNodeTest, SetandGetCornerRadius002)
{
    auto rsNode = RSCanvasNode::Create();
    rsNode->SetCornerRadius(floatData[2]);
    EXPECT_TRUE(ROSEN_EQ(rsNode->GetStagingProperties().GetCornerRadius().x_, floatData[2]));
}

/**
 * @tc.name: SetandGetCornerRadius003
 * @tc.desc:
 * @tc.type:FUNC
 */
TEST_F(RSNodeTest, SetandGetCornerRadius003)
{
    auto rsNode = RSCanvasNode::Create();
    rsNode->SetCornerRadius(floatData[3]);
    EXPECT_TRUE(ROSEN_EQ(rsNode->GetStagingProperties().GetCornerRadius().x_, floatData[3]));
}

/**
 * @tc.name: SetandGetCornerRadius004
 * @tc.desc:
 * @tc.type:FUNC
 */
TEST_F(RSNodeTest, SetandGetCornerRadius004)
{
    auto rsNode = RSCanvasNode::Create();
    rsNode->SetCornerRadius(floatData[4]);
    EXPECT_TRUE(ROSEN_EQ(rsNode->GetStagingProperties().GetCornerRadius().x_, floatData[4]));
}

/**
 * @tc.name: SetandGetCornerRadius005
 * @tc.desc:
 * @tc.type:FUNC
 */
TEST_F(RSNodeTest, SetandGetCornerRadius005)
{
    auto rsNode = RSCanvasNode::Create();
    rsNode->SetCornerRadius(floatData[0]);
    EXPECT_TRUE(ROSEN_EQ(rsNode->GetStagingProperties().GetCornerRadius().x_, floatData[0]));
}

/**
 * @tc.name: SetandGetRotationThree001
 * @tc.desc:
 * @tc.type:FUNC
 */
TEST_F(RSNodeTest, SetandGetRotationThree001)
{
    auto rsNode = RSCanvasNode::Create();
    rsNode->SetRotation(floatData[1], floatData[2], floatData[3]);
}

/**
 * @tc.name: SetandGetRotation001
 * @tc.desc:
 * @tc.type:FUNC
 */
TEST_F(RSNodeTest, SetandGetRotation001)
{
    auto rsNode = RSCanvasNode::Create();
    rsNode->SetRotation(floatData[1]);
    EXPECT_TRUE(ROSEN_EQ(rsNode->GetStagingProperties().GetRotation(), floatData[1]));
}

/**
 * @tc.name: SetandGetRotation002
 * @tc.desc:
 * @tc.type:FUNC
 */
TEST_F(RSNodeTest, SetandGetRotation002)
{
    auto rsNode = RSCanvasNode::Create();
    rsNode->SetRotation(floatData[2]);
    EXPECT_TRUE(ROSEN_EQ(rsNode->GetStagingProperties().GetRotation(), floatData[2]));
}

/**
 * @tc.name: SetandGetRotation003
 * @tc.desc:
 * @tc.type:FUNC
 */
TEST_F(RSNodeTest, SetandGetRotation003)
{
    auto rsNode = RSCanvasNode::Create();
    rsNode->SetRotation(floatData[3]);
    EXPECT_TRUE(ROSEN_EQ(rsNode->GetStagingProperties().GetRotation(), floatData[3]));
}

/**
 * @tc.name: SetandGetRotation004
 * @tc.desc:
 * @tc.type:FUNC
 */
TEST_F(RSNodeTest, SetandGetRotation004)
{
    auto rsNode = RSCanvasNode::Create();
    rsNode->SetRotation(floatData[4]);
    EXPECT_TRUE(ROSEN_EQ(rsNode->GetStagingProperties().GetRotation(), floatData[4]));
}

/**
 * @tc.name: SetandGetRotation005
 * @tc.desc:
 * @tc.type:FUNC
 */
TEST_F(RSNodeTest, SetandGetRotation005)
{
    auto rsNode = RSCanvasNode::Create();
    rsNode->SetRotation(floatData[0]);
    EXPECT_TRUE(ROSEN_EQ(rsNode->GetStagingProperties().GetRotation(), floatData[0]));
}

/**
 * @tc.name: SetandGetRotationX001
 * @tc.desc:
 * @tc.type:FUNC
 */
TEST_F(RSNodeTest, SetandGetRotationX001)
{
    auto rsNode = RSCanvasNode::Create();
    rsNode->SetRotationX(floatData[1]);
    EXPECT_TRUE(ROSEN_EQ(rsNode->GetStagingProperties().GetRotationX(), floatData[1]));
}

/**
 * @tc.name: SetandGetRotationX002
 * @tc.desc:
 * @tc.type:FUNC
 */
TEST_F(RSNodeTest, SetandGetRotationX002)
{
    auto rsNode = RSCanvasNode::Create();
    rsNode->SetRotationX(floatData[2]);
    EXPECT_TRUE(ROSEN_EQ(rsNode->GetStagingProperties().GetRotationX(), floatData[2]));
}

/**
 * @tc.name: SetandGetRotationX003
 * @tc.desc:
 * @tc.type:FUNC
 */
TEST_F(RSNodeTest, SetandGetRotationX003)
{
    auto rsNode = RSCanvasNode::Create();
    rsNode->SetRotationX(floatData[3]);
    EXPECT_TRUE(ROSEN_EQ(rsNode->GetStagingProperties().GetRotationX(), floatData[3]));
}

/**
 * @tc.name: SetandGetRotationX004
 * @tc.desc:
 * @tc.type:FUNC
 */
TEST_F(RSNodeTest, SetandGetRotationX004)
{
    auto rsNode = RSCanvasNode::Create();
    rsNode->SetRotationX(floatData[4]);
    EXPECT_TRUE(ROSEN_EQ(rsNode->GetStagingProperties().GetRotationX(), floatData[4]));
}

/**
 * @tc.name: SetandGetRotationX005
 * @tc.desc:
 * @tc.type:FUNC
 */
TEST_F(RSNodeTest, SetandGetRotationX005)
{
    auto rsNode = RSCanvasNode::Create();
    rsNode->SetRotationX(floatData[0]);
    EXPECT_TRUE(ROSEN_EQ(rsNode->GetStagingProperties().GetRotationX(), floatData[0]));
}

/**
 * @tc.name: SetandGetRotationY001
 * @tc.desc:
 * @tc.type:FUNC
 */
TEST_F(RSNodeTest, SetandGetRotationY001)
{
    auto rsNode = RSCanvasNode::Create();
    rsNode->SetRotationY(floatData[1]);
    EXPECT_TRUE(ROSEN_EQ(rsNode->GetStagingProperties().GetRotationY(), floatData[1]));
}

/**
 * @tc.name: SetandGetRotationY002
 * @tc.desc:
 * @tc.type:FUNC
 */
TEST_F(RSNodeTest, SetandGetRotationY002)
{
    auto rsNode = RSCanvasNode::Create();
    rsNode->SetRotationY(floatData[2]);
    EXPECT_TRUE(ROSEN_EQ(rsNode->GetStagingProperties().GetRotationY(), floatData[2]));
}

/**
 * @tc.name: SetandGetRotationY003
 * @tc.desc:
 * @tc.type:FUNC
 */
TEST_F(RSNodeTest, SetandGetRotationY003)
{
    auto rsNode = RSCanvasNode::Create();
    rsNode->SetRotationY(floatData[3]);
    EXPECT_TRUE(ROSEN_EQ(rsNode->GetStagingProperties().GetRotationY(), floatData[3]));
}

/**
 * @tc.name: SetandGetRotationY004
 * @tc.desc:
 * @tc.type:FUNC
 */
TEST_F(RSNodeTest, SetandGetRotationY004)
{
    auto rsNode = RSCanvasNode::Create();
    rsNode->SetRotationY(floatData[4]);
    EXPECT_TRUE(ROSEN_EQ(rsNode->GetStagingProperties().GetRotationY(), floatData[4]));
}

/**
 * @tc.name: SetandGetRotationY005
 * @tc.desc:
 * @tc.type:FUNC
 */
TEST_F(RSNodeTest, SetandGetRotationY005)
{
    auto rsNode = RSCanvasNode::Create();
    rsNode->SetRotationY(floatData[0]);
    EXPECT_TRUE(ROSEN_EQ(rsNode->GetStagingProperties().GetRotationY(), floatData[0]));
}


/**
 * @tc.name: SetandGetScaleX001
 * @tc.desc:
 * @tc.type:FUNC
 */
TEST_F(RSNodeTest, SetandGetScaleX001)
{
    auto rsNode = RSCanvasNode::Create();
    rsNode->SetScaleX(floatData[1]);
    EXPECT_TRUE(ROSEN_EQ(rsNode->GetStagingProperties().GetScale().x_, floatData[1]));
}

/**
 * @tc.name: SetandGetScaleX002
 * @tc.desc:
 * @tc.type:FUNC
 */
TEST_F(RSNodeTest, SetandGetScaleX002)
{
    auto rsNode = RSCanvasNode::Create();
    rsNode->SetScaleX(floatData[2]);
    EXPECT_TRUE(ROSEN_EQ(rsNode->GetStagingProperties().GetScale().x_, floatData[2]));
}

/**
 * @tc.name: SetandGetScaleX003
 * @tc.desc:
 * @tc.type:FUNC
 */
TEST_F(RSNodeTest, SetandGetScaleX003)
{
    auto rsNode = RSCanvasNode::Create();
    rsNode->SetScaleX(floatData[3]);
    EXPECT_TRUE(ROSEN_EQ(rsNode->GetStagingProperties().GetScale().x_, floatData[3]));
}

/**
 * @tc.name: SetandGetScaleX004
 * @tc.desc:
 * @tc.type:FUNC
 */
TEST_F(RSNodeTest, SetandGetScaleX004)
{
    auto rsNode = RSCanvasNode::Create();
    rsNode->SetScaleX(floatData[4]);
    EXPECT_TRUE(ROSEN_EQ(rsNode->GetStagingProperties().GetScale().x_, floatData[4]));
}

/**
 * @tc.name: SetandGetScaleX005
 * @tc.desc:
 * @tc.type:FUNC
 */
TEST_F(RSNodeTest, SetandGetScaleX005)
{
    auto rsNode = RSCanvasNode::Create();
    rsNode->SetScaleX(floatData[0]);
    EXPECT_TRUE(ROSEN_EQ(rsNode->GetStagingProperties().GetScale().x_, floatData[0]));
}

/**
 * @tc.name: SetandGetScale001
 * @tc.desc:
 * @tc.type:FUNC
 */
TEST_F(RSNodeTest, SetandGetScale001)
{
    auto rsNode = RSCanvasNode::Create();
    rsNode->SetScale(floatData[1]);
    EXPECT_TRUE(ROSEN_EQ(rsNode->GetStagingProperties().GetScale().x_, floatData[1]));
    EXPECT_TRUE(ROSEN_EQ(rsNode->GetStagingProperties().GetScale().y_, floatData[1]));
}

/**
 * @tc.name: SetandGetScale002
 * @tc.desc:
 * @tc.type:FUNC
 */
TEST_F(RSNodeTest, SetandGetScale002)
{
    auto rsNode = RSCanvasNode::Create();
    rsNode->SetScale(floatData[2]);
    EXPECT_TRUE(ROSEN_EQ(rsNode->GetStagingProperties().GetScale().x_, floatData[2]));
    EXPECT_TRUE(ROSEN_EQ(rsNode->GetStagingProperties().GetScale().y_, floatData[2]));
}

/**
 * @tc.name: SetandGetScale003
 * @tc.desc:
 * @tc.type:FUNC
 */
TEST_F(RSNodeTest, SetandGetScale003)
{
    auto rsNode = RSCanvasNode::Create();
    rsNode->SetScale(floatData[3]);
    EXPECT_TRUE(ROSEN_EQ(rsNode->GetStagingProperties().GetScale().x_, floatData[3]));
    EXPECT_TRUE(ROSEN_EQ(rsNode->GetStagingProperties().GetScale().y_, floatData[3]));
}

/**
 * @tc.name: SetandGetScale004
 * @tc.desc:
 * @tc.type:FUNC
 */
TEST_F(RSNodeTest, SetandGetScale004)
{
    auto rsNode = RSCanvasNode::Create();
    rsNode->SetScale(floatData[3], floatData[0]);
    EXPECT_TRUE(ROSEN_EQ(rsNode->GetStagingProperties().GetScale().x_, floatData[3]));
    EXPECT_TRUE(ROSEN_EQ(rsNode->GetStagingProperties().GetScale().y_, floatData[0]));
}

/**
 * @tc.name: SetandGetScale0005
 * @tc.desc:
 * @tc.type:FUNC
 */
TEST_F(RSNodeTest, SetandGetScale0005)
{
    auto rsNode = RSCanvasNode::Create();
    rsNode->SetScale(floatData[2], floatData[1]);
    EXPECT_TRUE(ROSEN_EQ(rsNode->GetStagingProperties().GetScale().x_, floatData[2]));
    EXPECT_TRUE(ROSEN_EQ(rsNode->GetStagingProperties().GetScale().y_, floatData[1]));
}

/**
 * @tc.name: SetandGetScaleY001
 * @tc.desc:
 * @tc.type:FUNC
 */
TEST_F(RSNodeTest, SetandGetScaleY001)
{
    auto rsNode = RSCanvasNode::Create();
    rsNode->SetScaleY(floatData[1]);
    EXPECT_TRUE(ROSEN_EQ(rsNode->GetStagingProperties().GetScale().y_, floatData[1]));
}

/**
 * @tc.name: SetandGetScaleY002
 * @tc.desc:
 * @tc.type:FUNC
 */
TEST_F(RSNodeTest, SetandGetScaleY002)
{
    auto rsNode = RSCanvasNode::Create();
    rsNode->SetScaleY(floatData[2]);
    EXPECT_TRUE(ROSEN_EQ(rsNode->GetStagingProperties().GetScale().y_, floatData[2]));
}

/**
 * @tc.name: SetandGetScaleY003
 * @tc.desc:
 * @tc.type:FUNC
 */
TEST_F(RSNodeTest, SetandGetScaleY003)
{
    auto rsNode = RSCanvasNode::Create();
    rsNode->SetScaleY(floatData[3]);
    EXPECT_TRUE(ROSEN_EQ(rsNode->GetStagingProperties().GetScale().y_, floatData[3]));
}

/**
 * @tc.name: SetandGetScaleY004
 * @tc.desc:
 * @tc.type:FUNC
 */
TEST_F(RSNodeTest, SetandGetScaleY004)
{
    auto rsNode = RSCanvasNode::Create();
    rsNode->SetScaleY(floatData[4]);
    EXPECT_TRUE(ROSEN_EQ(rsNode->GetStagingProperties().GetScale().y_, floatData[4]));
}

/**
 * @tc.name: SetandGetScaleY005
 * @tc.desc:
 * @tc.type:FUNC
 */
TEST_F(RSNodeTest, SetandGetScaleY005)
{
    auto rsNode = RSCanvasNode::Create();
    rsNode->SetScaleY(floatData[0]);
    EXPECT_TRUE(ROSEN_EQ(rsNode->GetStagingProperties().GetScale().y_, floatData[0]));
}

/**
 * @tc.name: SetandGetAlphaOffscreen001
 * @tc.desc:
 * @tc.type:FUNC
 */
TEST_F(RSNodeTest, SetandGetAlphaOffscreen001)
{
    auto rsNode = RSCanvasNode::Create();
    rsNode->SetAlphaOffscreen(true);
    EXPECT_TRUE(ROSEN_EQ(rsNode->GetStagingProperties().GetAlphaOffscreen(), true));
}

/**
 * @tc.name: SetandGetAlphaOffscreen002
 * @tc.desc:
 * @tc.type:FUNC
 */
TEST_F(RSNodeTest, SetandGetAlphaOffscreen002)
{
    auto rsNode = RSCanvasNode::Create();
    rsNode->SetAlphaOffscreen(false);
    EXPECT_TRUE(ROSEN_EQ(rsNode->GetStagingProperties().GetAlphaOffscreen(), false));
}

/**
 * @tc.name: SetandGetAlpha001
 * @tc.desc:
 * @tc.type:FUNC
 */
TEST_F(RSNodeTest, SetandGetAlpha001)
{
    auto rsNode = RSCanvasNode::Create();
    rsNode->SetAlpha(floatData[1]);
    EXPECT_TRUE(ROSEN_EQ(rsNode->GetStagingProperties().GetAlpha(), floatData[1]));
}

/**
 * @tc.name: SetandGetAlpha002
 * @tc.desc:
 * @tc.type:FUNC
 */
TEST_F(RSNodeTest, SetandGetAlpha002)
{
    auto rsNode = RSCanvasNode::Create();
    rsNode->SetAlpha(floatData[2]);
    EXPECT_TRUE(ROSEN_EQ(rsNode->GetStagingProperties().GetAlpha(), floatData[2]));
}

/**
 * @tc.name: SetandGetAlpha003
 * @tc.desc:
 * @tc.type:FUNC
 */
TEST_F(RSNodeTest, SetandGetAlpha003)
{
    auto rsNode = RSCanvasNode::Create();
    rsNode->SetAlpha(floatData[3]);
    EXPECT_TRUE(ROSEN_EQ(rsNode->GetStagingProperties().GetAlpha(), floatData[3]));
}

/**
 * @tc.name: SetandGetAlpha004
 * @tc.desc:
 * @tc.type:FUNC
 */
TEST_F(RSNodeTest, SetandGetAlpha004)
{
    auto rsNode = RSCanvasNode::Create();
    rsNode->SetAlpha(floatData[4]);
    EXPECT_TRUE(ROSEN_EQ(rsNode->GetStagingProperties().GetAlpha(), floatData[4]));
}

/**
 * @tc.name: SetandGetAlpha005
 * @tc.desc:
 * @tc.type:FUNC
 */
TEST_F(RSNodeTest, SetandGetAlpha005)
{
    auto rsNode = RSCanvasNode::Create();
    rsNode->SetAlpha(floatData[0]);
    EXPECT_TRUE(ROSEN_EQ(rsNode->GetStagingProperties().GetAlpha(), floatData[0]));
}

/**
 * @tc.name: SetandGetBgImageSize001
 * @tc.desc:
 * @tc.type:FUNC
 */
TEST_F(RSNodeTest, SetandGetBgImageSize001)
{
    auto rsNode = RSCanvasNode::Create();
    rsNode->SetBgImageSize(floatData[0], floatData[1]);
    EXPECT_TRUE(ROSEN_EQ(rsNode->GetStagingProperties().GetBgImageWidth(), floatData[0]));
    EXPECT_TRUE(ROSEN_EQ(rsNode->GetStagingProperties().GetBgImageHeight(), floatData[1]));
}

/**
 * @tc.name: SetandGetBgImageSize002
 * @tc.desc:
 * @tc.type:FUNC
 */
TEST_F(RSNodeTest, SetandGetBgImageSize002)
{
    auto rsNode = RSCanvasNode::Create();
    rsNode->SetBgImageSize(floatData[3], floatData[1]);
    EXPECT_TRUE(ROSEN_EQ(rsNode->GetStagingProperties().GetBgImageWidth(), floatData[3]));
    EXPECT_TRUE(ROSEN_EQ(rsNode->GetStagingProperties().GetBgImageHeight(), floatData[1]));
}

/**
 * @tc.name: SetandGetBgImageSize003
 * @tc.desc:
 * @tc.type:FUNC
 */
TEST_F(RSNodeTest, SetandGetBgImageSize003)
{
    auto rsNode = RSCanvasNode::Create();
    rsNode->SetBgImageSize(floatData[3], floatData[2]);
    EXPECT_TRUE(ROSEN_EQ(rsNode->GetStagingProperties().GetBgImageWidth(), floatData[3]));
    EXPECT_TRUE(ROSEN_EQ(rsNode->GetStagingProperties().GetBgImageHeight(), floatData[2]));
}

/**
 * @tc.name: SetandGetBgImageWidth001
 * @tc.desc:
 * @tc.type:FUNC
 */
TEST_F(RSNodeTest, SetandGetBgImageWidth001)
{
    auto rsNode = RSCanvasNode::Create();
    rsNode->SetBgImageWidth(floatData[1]);
    EXPECT_TRUE(ROSEN_EQ(rsNode->GetStagingProperties().GetBgImageWidth(), floatData[1]));
}

/**
 * @tc.name: SetandGetBgImageWidth002
 * @tc.desc:
 * @tc.type:FUNC
 */
TEST_F(RSNodeTest, SetandGetBgImageWidth002)
{
    auto rsNode = RSCanvasNode::Create();
    rsNode->SetBgImageWidth(floatData[2]);
    EXPECT_TRUE(ROSEN_EQ(rsNode->GetStagingProperties().GetBgImageWidth(), floatData[2]));
}

/**
 * @tc.name: SetandGetBgImageWidth003
 * @tc.desc:
 * @tc.type:FUNC
 */
TEST_F(RSNodeTest, SetandGetBgImageWidth003)
{
    auto rsNode = RSCanvasNode::Create();
    rsNode->SetBgImageWidth(floatData[3]);
    EXPECT_TRUE(ROSEN_EQ(rsNode->GetStagingProperties().GetBgImageWidth(), floatData[3]));
}

/**
 * @tc.name: SetandGetBgImageWidth004
 * @tc.desc:
 * @tc.type:FUNC
 */
TEST_F(RSNodeTest, SetandGetBgImageWidth004)
{
    auto rsNode = RSCanvasNode::Create();
    rsNode->SetBgImageWidth(floatData[4]);
    EXPECT_TRUE(ROSEN_EQ(rsNode->GetStagingProperties().GetBgImageWidth(), floatData[4]));
}

/**
 * @tc.name: SetandGetBgImageWidth005
 * @tc.desc:
 * @tc.type:FUNC
 */
TEST_F(RSNodeTest, SetandGetBgImageWidth005)
{
    auto rsNode = RSCanvasNode::Create();
    rsNode->SetBgImageWidth(floatData[0]);
    EXPECT_TRUE(ROSEN_EQ(rsNode->GetStagingProperties().GetBgImageWidth(), floatData[0]));
}

/**
 * @tc.name: SetandGetBgImageHeight001
 * @tc.desc:
 * @tc.type:FUNC
 */
TEST_F(RSNodeTest, SetandGetBgImageHeight001)
{
    auto rsNode = RSCanvasNode::Create();
    rsNode->SetBgImageHeight(floatData[1]);
    EXPECT_TRUE(ROSEN_EQ(rsNode->GetStagingProperties().GetBgImageHeight(), floatData[1]));
}

/**
 * @tc.name: SetandGetBgImageHeight002
 * @tc.desc:
 * @tc.type:FUNC
 */
TEST_F(RSNodeTest, SetandGetBgImageHeight002)
{
    auto rsNode = RSCanvasNode::Create();
    rsNode->SetBgImageHeight(floatData[2]);
    EXPECT_TRUE(ROSEN_EQ(rsNode->GetStagingProperties().GetBgImageHeight(), floatData[2]));
}

/**
 * @tc.name: SetandGetBgImageHeight003
 * @tc.desc:
 * @tc.type:FUNC
 */
TEST_F(RSNodeTest, SetandGetBgImageHeight003)
{
    auto rsNode = RSCanvasNode::Create();
    rsNode->SetBgImageHeight(floatData[3]);
    EXPECT_TRUE(ROSEN_EQ(rsNode->GetStagingProperties().GetBgImageHeight(), floatData[3]));
}

/**
 * @tc.name: SetandGetBgImageHeight004
 * @tc.desc:
 * @tc.type:FUNC
 */
TEST_F(RSNodeTest, SetandGetBgImageHeight004)
{
    auto rsNode = RSCanvasNode::Create();
    rsNode->SetBgImageHeight(floatData[4]);
    EXPECT_TRUE(ROSEN_EQ(rsNode->GetStagingProperties().GetBgImageHeight(), floatData[4]));
}

/**
 * @tc.name: SetandGetBgImageHeight005
 * @tc.desc:
 * @tc.type:FUNC
 */
TEST_F(RSNodeTest, SetandGetBgImageHeight005)
{
    auto rsNode = RSCanvasNode::Create();
    rsNode->SetBgImageHeight(floatData[0]);
    EXPECT_TRUE(ROSEN_EQ(rsNode->GetStagingProperties().GetBgImageHeight(), floatData[0]));
}

/**
 * @tc.name: SetandSetBgImagePosition001
 * @tc.desc:
 * @tc.type:FUNC
 */
TEST_F(RSNodeTest, SetandSetBgImagePosition001)
{
    auto rsNode = RSCanvasNode::Create();
    rsNode->SetBgImagePosition(floatData[2], floatData[3]);
    EXPECT_TRUE(ROSEN_EQ(rsNode->GetStagingProperties().GetBgImagePositionX(), floatData[2]));
    EXPECT_TRUE(ROSEN_EQ(rsNode->GetStagingProperties().GetBgImagePositionY(), floatData[3]));
}

/**
 * @tc.name: SetandSetBgImagePosition002
 * @tc.desc:
 * @tc.type:FUNC
 */
TEST_F(RSNodeTest, SetandSetBgImagePosition002)
{
    auto rsNode = RSCanvasNode::Create();
    rsNode->SetBgImagePosition(floatData[2], floatData[0]);
    EXPECT_TRUE(ROSEN_EQ(rsNode->GetStagingProperties().GetBgImagePositionX(), floatData[2]));
    EXPECT_TRUE(ROSEN_EQ(rsNode->GetStagingProperties().GetBgImagePositionY(), floatData[0]));
}

/**
 * @tc.name: SetandSetBgImagePosition003
 * @tc.desc:
 * @tc.type:FUNC
 */
TEST_F(RSNodeTest, SetandSetBgImagePosition003)
{
    auto rsNode = RSCanvasNode::Create();
    rsNode->SetBgImagePosition(floatData[1], floatData[3]);
    EXPECT_TRUE(ROSEN_EQ(rsNode->GetStagingProperties().GetBgImagePositionX(), floatData[1]));
    EXPECT_TRUE(ROSEN_EQ(rsNode->GetStagingProperties().GetBgImagePositionY(), floatData[3]));
}

/**
 * @tc.name: SetandGetBgImagePositionX001
 * @tc.desc:
 * @tc.type:FUNC
 */
TEST_F(RSNodeTest, SetandGetBgImagePositionX001)
{
    auto rsNode = RSCanvasNode::Create();
    rsNode->SetBgImagePositionX(floatData[1]);
    EXPECT_TRUE(ROSEN_EQ(rsNode->GetStagingProperties().GetBgImagePositionX(), floatData[1]));
}

/**
 * @tc.name: SetandGetBgImagePositionX002
 * @tc.desc:
 * @tc.type:FUNC
 */
TEST_F(RSNodeTest, SetandGetBgImagePositionX002)
{
    auto rsNode = RSCanvasNode::Create();
    rsNode->SetBgImagePositionX(floatData[2]);
    EXPECT_TRUE(ROSEN_EQ(rsNode->GetStagingProperties().GetBgImagePositionX(), floatData[2]));
}

/**
 * @tc.name: SetandGetBgImagePositionX003
 * @tc.desc:
 * @tc.type:FUNC
 */
TEST_F(RSNodeTest, SetandGetBgImagePositionX003)
{
    auto rsNode = RSCanvasNode::Create();
    rsNode->SetBgImagePositionX(floatData[3]);
    EXPECT_TRUE(ROSEN_EQ(rsNode->GetStagingProperties().GetBgImagePositionX(), floatData[3]));
}

/**
 * @tc.name: SetandGetBgImagePositionX004
 * @tc.desc:
 * @tc.type:FUNC
 */
TEST_F(RSNodeTest, SetandGetBgImagePositionX004)
{
    auto rsNode = RSCanvasNode::Create();
    rsNode->SetBgImagePositionX(floatData[4]);
    EXPECT_TRUE(ROSEN_EQ(rsNode->GetStagingProperties().GetBgImagePositionX(), floatData[4]));
}

/**
 * @tc.name: SetandGetBgImagePositionX005
 * @tc.desc:
 * @tc.type:FUNC
 */
TEST_F(RSNodeTest, SetandGetBgImagePositionX005)
{
    auto rsNode = RSCanvasNode::Create();
    rsNode->SetBgImagePositionX(floatData[0]);
    EXPECT_TRUE(ROSEN_EQ(rsNode->GetStagingProperties().GetBgImagePositionX(), floatData[0]));
}

/**
 * @tc.name: SetandGetBgImagePositionY001
 * @tc.desc:
 * @tc.type:FUNC
 */
TEST_F(RSNodeTest, SetandGetBgImagePositionY001)
{
    auto rsNode = RSCanvasNode::Create();
    rsNode->SetBgImagePositionY(floatData[1]);
    EXPECT_TRUE(ROSEN_EQ(rsNode->GetStagingProperties().GetBgImagePositionY(), floatData[1]));
}

/**
 * @tc.name: SetandGetBgImagePositionY002
 * @tc.desc:
 * @tc.type:FUNC
 */
TEST_F(RSNodeTest, SetandGetBgImagePositionY002)
{
    auto rsNode = RSCanvasNode::Create();
    rsNode->SetBgImagePositionY(floatData[2]);
    EXPECT_TRUE(ROSEN_EQ(rsNode->GetStagingProperties().GetBgImagePositionY(), floatData[2]));
}

/**
 * @tc.name: SetandGetBgImagePositionY003
 * @tc.desc:
 * @tc.type:FUNC
 */
TEST_F(RSNodeTest, SetandGetBgImagePositionY003)
{
    auto rsNode = RSCanvasNode::Create();
    rsNode->SetBgImagePositionY(floatData[3]);
    EXPECT_TRUE(ROSEN_EQ(rsNode->GetStagingProperties().GetBgImagePositionY(), floatData[3]));
}

/**
 * @tc.name: SetandGetBgImagePositionY004
 * @tc.desc:
 * @tc.type:FUNC
 */
TEST_F(RSNodeTest, SetandGetBgImagePositionY004)
{
    auto rsNode = RSCanvasNode::Create();
    rsNode->SetBgImagePositionY(floatData[4]);
    EXPECT_TRUE(ROSEN_EQ(rsNode->GetStagingProperties().GetBgImagePositionY(), floatData[4]));
}

/**
 * @tc.name: SetandGetBgImagePositionY005
 * @tc.desc:
 * @tc.type:FUNC
 */
TEST_F(RSNodeTest, SetandGetBgImagePositionY005)
{
    auto rsNode = RSCanvasNode::Create();
    rsNode->SetBgImagePositionY(floatData[0]);
    EXPECT_TRUE(ROSEN_EQ(rsNode->GetStagingProperties().GetBgImagePositionY(), floatData[0]));
}

/**
 * @tc.name: SetandGetBorderWidth001
 * @tc.desc:
 * @tc.type:FUNC
 */
TEST_F(RSNodeTest, SetandGetBorderWidth001)
{
    auto rsNode = RSCanvasNode::Create();
    rsNode->SetBorderWidth(floatData[1]);
    EXPECT_TRUE(ROSEN_EQ(rsNode->GetStagingProperties().GetBorderWidth().x_, floatData[1]));
}

/**
 * @tc.name: SetandGetBorderWidth002
 * @tc.desc:
 * @tc.type:FUNC
 */
TEST_F(RSNodeTest, SetandGetBorderWidth002)
{
    auto rsNode = RSCanvasNode::Create();
    rsNode->SetBorderWidth(floatData[2]);
    EXPECT_TRUE(ROSEN_EQ(rsNode->GetStagingProperties().GetBorderWidth().x_, floatData[2]));
}

/**
 * @tc.name: SetandGetBorderWidth003
 * @tc.desc:
 * @tc.type:FUNC
 */
TEST_F(RSNodeTest, SetandGetBorderWidth003)
{
    auto rsNode = RSCanvasNode::Create();
    rsNode->SetBorderWidth(floatData[3]);
    EXPECT_TRUE(ROSEN_EQ(rsNode->GetStagingProperties().GetBorderWidth().x_, floatData[3]));
}

/**
 * @tc.name: SetandGetBorderWidth004
 * @tc.desc:
 * @tc.type:FUNC
 */
TEST_F(RSNodeTest, SetandGetBorderWidth004)
{
    auto rsNode = RSCanvasNode::Create();
    rsNode->SetBorderWidth(floatData[4]);
    EXPECT_TRUE(ROSEN_EQ(rsNode->GetStagingProperties().GetBorderWidth().x_, floatData[4]));
}

/**
 * @tc.name: SetandGetBorderWidth005
 * @tc.desc:
 * @tc.type:FUNC
 */
TEST_F(RSNodeTest, SetandGetBorderWidth005)
{
    auto rsNode = RSCanvasNode::Create();
    rsNode->SetBorderWidth(floatData[0]);
    EXPECT_TRUE(ROSEN_EQ(rsNode->GetStagingProperties().GetBorderWidth().x_, floatData[0]));
}

/**
 * @tc.name: SetandGetPivot001
 * @tc.desc:
 * @tc.type:FUNC
 */
TEST_F(RSNodeTest, SetandGetPivot001)
{
    auto rsNode = RSCanvasNode::Create();
    rsNode->SetPivot(floatData[0], floatData[1]);
    EXPECT_TRUE(ROSEN_EQ(rsNode->GetStagingProperties().GetPivot().x_, floatData[0]));
    EXPECT_TRUE(ROSEN_EQ(rsNode->GetStagingProperties().GetPivot().y_, floatData[1]));
}

/**
 * @tc.name: SetandGetPivot002
 * @tc.desc:
 * @tc.type:FUNC
 */
TEST_F(RSNodeTest, SetandGetPivot002)
{
    auto rsNode = RSCanvasNode::Create();
    rsNode->SetPivot(floatData[3], floatData[2]);
    EXPECT_TRUE(ROSEN_EQ(rsNode->GetStagingProperties().GetPivot().x_, floatData[3]));
    EXPECT_TRUE(ROSEN_EQ(rsNode->GetStagingProperties().GetPivot().y_, floatData[2]));
}

/**
 * @tc.name: SetandGetPivot003
 * @tc.desc:
 * @tc.type:FUNC
 */
TEST_F(RSNodeTest, SetandGetPivot003)
{
    auto rsNode = RSCanvasNode::Create();
    rsNode->SetPivot(floatData[1], floatData[3]);
    EXPECT_TRUE(ROSEN_EQ(rsNode->GetStagingProperties().GetPivot().x_, floatData[1]));
    EXPECT_TRUE(ROSEN_EQ(rsNode->GetStagingProperties().GetPivot().y_, floatData[3]));
}

/**
 * @tc.name: SetandGetPivotX001
 * @tc.desc:
 * @tc.type:FUNC
 */
TEST_F(RSNodeTest, SetandGetPivotX001)
{
    auto rsNode = RSCanvasNode::Create();
    rsNode->SetPivotX(floatData[1]);
    EXPECT_TRUE(ROSEN_EQ(rsNode->GetStagingProperties().GetPivot().x_, floatData[1]));
}

/**
 * @tc.name: SetandGetPivotX002
 * @tc.desc:
 * @tc.type:FUNC
 */
TEST_F(RSNodeTest, SetandGetPivotX002)
{
    auto rsNode = RSCanvasNode::Create();
    rsNode->SetPivotX(floatData[2]);
    EXPECT_TRUE(ROSEN_EQ(rsNode->GetStagingProperties().GetPivot().x_, floatData[2]));
}

/**
 * @tc.name: SetandGetPivotX003
 * @tc.desc:
 * @tc.type:FUNC
 */
TEST_F(RSNodeTest, SetandGetPivotX003)
{
    auto rsNode = RSCanvasNode::Create();
    rsNode->SetPivotX(floatData[3]);
    EXPECT_TRUE(ROSEN_EQ(rsNode->GetStagingProperties().GetPivot().x_, floatData[3]));
}

/**
 * @tc.name: SetandGetPivotX004
 * @tc.desc:
 * @tc.type:FUNC
 */
TEST_F(RSNodeTest, SetandGetPivotX004)
{
    auto rsNode = RSCanvasNode::Create();
    rsNode->SetPivotX(floatData[4]);
    EXPECT_TRUE(ROSEN_EQ(rsNode->GetStagingProperties().GetPivot().x_, floatData[4]));
}

/**
 * @tc.name: SetandGetPivotX005
 * @tc.desc:
 * @tc.type:FUNC
 */
TEST_F(RSNodeTest, SetandGetPivotX005)
{
    auto rsNode = RSCanvasNode::Create();
    rsNode->SetPivotX(floatData[0]);
    EXPECT_TRUE(ROSEN_EQ(rsNode->GetStagingProperties().GetPivot().x_, floatData[0]));
}

/**
 * @tc.name: SetandGetPivotY001
 * @tc.desc:
 * @tc.type:FUNC
 */
TEST_F(RSNodeTest, SetandGetPivotY001)
{
    auto rsNode = RSCanvasNode::Create();
    rsNode->SetPivotY(floatData[1]);
    EXPECT_TRUE(ROSEN_EQ(rsNode->GetStagingProperties().GetPivot().y_, floatData[1]));
}

/**
 * @tc.name: SetandGetPivotY002
 * @tc.desc:
 * @tc.type:FUNC
 */
TEST_F(RSNodeTest, SetandGetPivotY002)
{
    auto rsNode = RSCanvasNode::Create();
    rsNode->SetPivotY(floatData[2]);
    EXPECT_TRUE(ROSEN_EQ(rsNode->GetStagingProperties().GetPivot().y_, floatData[2]));
}

/**
 * @tc.name: SetandGetPivotY003
 * @tc.desc:
 * @tc.type:FUNC
 */
TEST_F(RSNodeTest, SetandGetPivotY003)
{
    auto rsNode = RSCanvasNode::Create();
    rsNode->SetPivotY(floatData[3]);
    EXPECT_TRUE(ROSEN_EQ(rsNode->GetStagingProperties().GetPivot().y_, floatData[3]));
}

/**
 * @tc.name: SetandGetPivotY004
 * @tc.desc:
 * @tc.type:FUNC
 */
TEST_F(RSNodeTest, SetandGetPivotY004)
{
    auto rsNode = RSCanvasNode::Create();
    rsNode->SetPivotY(floatData[4]);
    EXPECT_TRUE(ROSEN_EQ(rsNode->GetStagingProperties().GetPivot().y_, floatData[4]));
}

/**
 * @tc.name: SetandGetPivotY005
 * @tc.desc:
 * @tc.type:FUNC
 */
TEST_F(RSNodeTest, SetandGetPivotY005)
{
    auto rsNode = RSCanvasNode::Create();
    rsNode->SetPivotY(floatData[0]);
    EXPECT_TRUE(ROSEN_EQ(rsNode->GetStagingProperties().GetPivot().y_, floatData[0]));
}

/**
 * @tc.name: SetandGetShadowOffset001
 * @tc.desc:
 * @tc.type:FUNC
 */
TEST_F(RSNodeTest, SetandGetShadowOffset001)
{
    auto rsNode = RSCanvasNode::Create();
    rsNode->SetShadowOffset(floatData[2], floatData[3]);
    EXPECT_TRUE(ROSEN_EQ(rsNode->GetStagingProperties().GetShadowOffsetX(), floatData[2]));
    EXPECT_TRUE(ROSEN_EQ(rsNode->GetStagingProperties().GetShadowOffsetY(), floatData[3]));
}

/**
 * @tc.name: SetandGetShadowOffset002
 * @tc.desc:
 * @tc.type:FUNC
 */
TEST_F(RSNodeTest, SetandGetShadowOffset002)
{
    auto rsNode = RSCanvasNode::Create();
    rsNode->SetShadowOffset(floatData[2], floatData[0]);
    EXPECT_TRUE(ROSEN_EQ(rsNode->GetStagingProperties().GetShadowOffsetX(), floatData[2]));
    EXPECT_TRUE(ROSEN_EQ(rsNode->GetStagingProperties().GetShadowOffsetY(), floatData[0]));
}

/**
 * @tc.name: SetandGetShadowOffset003
 * @tc.desc:
 * @tc.type:FUNC
 */
TEST_F(RSNodeTest, SetandGetShadowOffset003)
{
    auto rsNode = RSCanvasNode::Create();
    rsNode->SetShadowOffset(floatData[1], floatData[3]);
    EXPECT_TRUE(ROSEN_EQ(rsNode->GetStagingProperties().GetShadowOffsetX(), floatData[1]));
    EXPECT_TRUE(ROSEN_EQ(rsNode->GetStagingProperties().GetShadowOffsetY(), floatData[3]));
}
/**
 * @tc.name: SetandGetShadowOffsetX001
 * @tc.desc:
 * @tc.type:FUNC
 */
TEST_F(RSNodeTest, SetandGetShadowOffsetX001)
{
    auto rsNode = RSCanvasNode::Create();
    rsNode->SetShadowOffsetX(floatData[1]);
    EXPECT_TRUE(ROSEN_EQ(rsNode->GetStagingProperties().GetShadowOffsetX(), floatData[1]));
}

/**
 * @tc.name: SetandGetShadowOffsetX002
 * @tc.desc:
 * @tc.type:FUNC
 */
TEST_F(RSNodeTest, SetandGetShadowOffsetX002)
{
    auto rsNode = RSCanvasNode::Create();
    rsNode->SetShadowOffsetX(floatData[2]);
    EXPECT_TRUE(ROSEN_EQ(rsNode->GetStagingProperties().GetShadowOffsetX(), floatData[2]));
}

/**
 * @tc.name: SetandGetShadowOffsetX003
 * @tc.desc:
 * @tc.type:FUNC
 */
TEST_F(RSNodeTest, SetandGetShadowOffsetX003)
{
    auto rsNode = RSCanvasNode::Create();
    rsNode->SetShadowOffsetX(floatData[3]);
    EXPECT_TRUE(ROSEN_EQ(rsNode->GetStagingProperties().GetShadowOffsetX(), floatData[3]));
}

/**
 * @tc.name: SetandGetShadowOffsetX004
 * @tc.desc:
 * @tc.type:FUNC
 */
TEST_F(RSNodeTest, SetandGetShadowOffsetX004)
{
    auto rsNode = RSCanvasNode::Create();
    rsNode->SetShadowOffsetX(floatData[4]);
    EXPECT_TRUE(ROSEN_EQ(rsNode->GetStagingProperties().GetShadowOffsetX(), floatData[4]));
}

/**
 * @tc.name: SetandGetShadowOffsetX005
 * @tc.desc:
 * @tc.type:FUNC
 */
TEST_F(RSNodeTest, SetandGetShadowOffsetX005)
{
    auto rsNode = RSCanvasNode::Create();
    rsNode->SetShadowOffsetX(floatData[0]);
    EXPECT_TRUE(ROSEN_EQ(rsNode->GetStagingProperties().GetShadowOffsetX(), floatData[0]));
}

/**
 * @tc.name: SetandGetShadowOffsetY001
 * @tc.desc:
 * @tc.type:FUNC
 */
TEST_F(RSNodeTest, SetandGetShadowOffsetY001)
{
    auto rsNode = RSCanvasNode::Create();
    rsNode->SetShadowOffsetY(floatData[1]);
    EXPECT_TRUE(ROSEN_EQ(rsNode->GetStagingProperties().GetShadowOffsetY(), floatData[1]));
}

/**
 * @tc.name: SetandGetShadowOffsetY002
 * @tc.desc:
 * @tc.type:FUNC
 */
TEST_F(RSNodeTest, SetandGetShadowOffsetY002)
{
    auto rsNode = RSCanvasNode::Create();
    rsNode->SetShadowOffsetY(floatData[2]);
    EXPECT_TRUE(ROSEN_EQ(rsNode->GetStagingProperties().GetShadowOffsetY(), floatData[2]));
}

/**
 * @tc.name: SetandGetShadowOffsetY003
 * @tc.desc:
 * @tc.type:FUNC
 */
TEST_F(RSNodeTest, SetandGetShadowOffsetY003)
{
    auto rsNode = RSCanvasNode::Create();
    rsNode->SetShadowOffsetY(floatData[3]);
    EXPECT_TRUE(ROSEN_EQ(rsNode->GetStagingProperties().GetShadowOffsetY(), floatData[3]));
}

/**
 * @tc.name: SetandGetShadowOffsetY004
 * @tc.desc:
 * @tc.type:FUNC
 */
TEST_F(RSNodeTest, SetandGetShadowOffsetY004)
{
    auto rsNode = RSCanvasNode::Create();
    rsNode->SetShadowOffsetY(floatData[4]);
    EXPECT_TRUE(ROSEN_EQ(rsNode->GetStagingProperties().GetShadowOffsetY(), floatData[4]));
}

/**
 * @tc.name: SetandGetShadowOffsetY005
 * @tc.desc:
 * @tc.type:FUNC
 */
TEST_F(RSNodeTest, SetandGetShadowOffsetY005)
{
    auto rsNode = RSCanvasNode::Create();
    rsNode->SetShadowOffsetY(floatData[0]);
    EXPECT_TRUE(ROSEN_EQ(rsNode->GetStagingProperties().GetShadowOffsetY(), floatData[0]));
}

/**
 * @tc.name: SetandGetShadowAlpha001
 * @tc.desc:
 * @tc.type:FUNC
 */
TEST_F(RSNodeTest, SetandGetShadowAlpha001)
{
    auto rsNode = RSCanvasNode::Create();
    rsNode->SetShadowAlpha(floatData[1]);
    EXPECT_TRUE(ROSEN_EQ(rsNode->GetStagingProperties().GetShadowAlpha(), floatData[1], 0.02f));
}

/**
 * @tc.name: SetandGetShadowAlpha002
 * @tc.desc:
 * @tc.type:FUNC
 */
TEST_F(RSNodeTest, SetandGetShadowAlpha002)
{
    auto rsNode = RSCanvasNode::Create();
    rsNode->SetShadowAlpha(floatData[2]);
    EXPECT_TRUE(ROSEN_EQ(rsNode->GetStagingProperties().GetShadowAlpha(), floatData[2], 0.02f));
}

/**
 * @tc.name: SetandGetShadowAlpha003
 * @tc.desc:
 * @tc.type:FUNC
 */
TEST_F(RSNodeTest, SetandGetShadowAlpha003)
{
    auto rsNode = RSCanvasNode::Create();
    rsNode->SetShadowAlpha(floatData[3]);
    EXPECT_TRUE(ROSEN_EQ(rsNode->GetStagingProperties().GetShadowAlpha(), floatData[3], 0.02f));
}

/**
 * @tc.name: SetandGetShadowAlpha004
 * @tc.desc:
 * @tc.type:FUNC
 */
TEST_F(RSNodeTest, SetandGetShadowAlpha004)
{
    auto rsNode = RSCanvasNode::Create();
    rsNode->SetShadowAlpha(floatData[4]);
    EXPECT_TRUE(ROSEN_EQ(rsNode->GetStagingProperties().GetShadowAlpha(), floatData[4], 0.02f));
}

/**
 * @tc.name: SetandGetShadowAlpha005
 * @tc.desc:
 * @tc.type:FUNC
 */
TEST_F(RSNodeTest, SetandGetShadowAlpha005)
{
    auto rsNode = RSCanvasNode::Create();
    rsNode->SetShadowAlpha(floatData[0]);
    EXPECT_TRUE(ROSEN_EQ(rsNode->GetStagingProperties().GetShadowAlpha(),
        std::clamp(floatData[0], 0.0f, 1.0f), 0.02f));
}

/**
 * @tc.name: SetandGetShadowElevation001
 * @tc.desc:
 * @tc.type:FUNC
 */
TEST_F(RSNodeTest, SetandGetShadowElevation001)
{
    auto rsNode = RSCanvasNode::Create();
    rsNode->SetShadowElevation(floatData[1]);
    EXPECT_TRUE(ROSEN_EQ(rsNode->GetStagingProperties().GetShadowElevation(), floatData[1]));
}

/**
 * @tc.name: SetandGetShadowElevation002
 * @tc.desc:
 * @tc.type:FUNC
 */
TEST_F(RSNodeTest, SetandGetShadowElevation002)
{
    auto rsNode = RSCanvasNode::Create();
    rsNode->SetShadowElevation(floatData[2]);
    EXPECT_TRUE(ROSEN_EQ(rsNode->GetStagingProperties().GetShadowElevation(), floatData[2]));
}

/**
 * @tc.name: SetandGetShadowElevation003
 * @tc.desc:
 * @tc.type:FUNC
 */
TEST_F(RSNodeTest, SetandGetShadowElevation003)
{
    auto rsNode = RSCanvasNode::Create();
    rsNode->SetShadowElevation(floatData[3]);
    EXPECT_TRUE(ROSEN_EQ(rsNode->GetStagingProperties().GetShadowElevation(), floatData[3]));
}

/**
 * @tc.name: SetandGetShadowElevation004
 * @tc.desc:
 * @tc.type:FUNC
 */
TEST_F(RSNodeTest, SetandGetShadowElevation004)
{
    auto rsNode = RSCanvasNode::Create();
    rsNode->SetShadowElevation(floatData[4]);
    EXPECT_TRUE(ROSEN_EQ(rsNode->GetStagingProperties().GetShadowElevation(), floatData[4]));
}

/**
 * @tc.name: SetandGetShadowElevation005
 * @tc.desc:
 * @tc.type:FUNC
 */
TEST_F(RSNodeTest, SetandGetShadowElevation005)
{
    auto rsNode = RSCanvasNode::Create();
    rsNode->SetShadowElevation(floatData[0]);
    EXPECT_TRUE(ROSEN_EQ(rsNode->GetStagingProperties().GetShadowElevation(), floatData[0]));
}

/**
 * @tc.name: SetandGetShadowRadius001
 * @tc.desc:
 * @tc.type:FUNC
 */
TEST_F(RSNodeTest, SetandGetShadowRadius001)
{
    auto rsNode = RSCanvasNode::Create();
    rsNode->SetShadowRadius(floatData[1]);
    EXPECT_TRUE(ROSEN_EQ(rsNode->GetStagingProperties().GetShadowRadius(), floatData[1]));
}

/**
 * @tc.name: SetandGetShadowRadius002
 * @tc.desc:
 * @tc.type:FUNC
 */
TEST_F(RSNodeTest, SetandGetShadowRadius002)
{
    auto rsNode = RSCanvasNode::Create();
    rsNode->SetShadowRadius(floatData[2]);
    EXPECT_TRUE(ROSEN_EQ(rsNode->GetStagingProperties().GetShadowRadius(), floatData[2]));
}

/**
 * @tc.name: SetandGetShadowRadius003
 * @tc.desc:
 * @tc.type:FUNC
 */
TEST_F(RSNodeTest, SetandGetShadowRadius003)
{
    auto rsNode = RSCanvasNode::Create();
    rsNode->SetShadowRadius(floatData[3]);
    EXPECT_TRUE(ROSEN_EQ(rsNode->GetStagingProperties().GetShadowRadius(), floatData[3]));
}

/**
 * @tc.name: SetandGetShadowRadius004
 * @tc.desc:
 * @tc.type:FUNC
 */
TEST_F(RSNodeTest, SetandGetShadowRadius004)
{
    auto rsNode = RSCanvasNode::Create();
    rsNode->SetShadowRadius(floatData[4]);
    EXPECT_TRUE(ROSEN_EQ(rsNode->GetStagingProperties().GetShadowRadius(), floatData[4]));
}

/**
 * @tc.name: SetandGetShadowRadius005
 * @tc.desc:
 * @tc.type:FUNC
 */
TEST_F(RSNodeTest, SetandGetShadowRadius005)
{
    auto rsNode = RSCanvasNode::Create();
    rsNode->SetShadowRadius(floatData[0]);
    EXPECT_TRUE(ROSEN_EQ(rsNode->GetStagingProperties().GetShadowRadius(), floatData[0]));
}

/**
 * @tc.name: SetandGetShadowColor001
 * @tc.desc:
 * @tc.type:FUNC
 */
TEST_F(RSNodeTest, SetandGetShadowColor001)
{
    auto rsNode = RSCanvasNode::Create();
    constexpr uint32_t colorValue = 0x034123;
    rsNode->SetShadowColor(colorValue);
    EXPECT_TRUE(rsNode->GetStagingProperties().GetShadowColor() == Color::FromArgbInt(colorValue));
}

/**
 * @tc.name: SetandGetShadowColor002
 * @tc.desc:
 * @tc.type:FUNC
 */
TEST_F(RSNodeTest, SetandGetShadowColor002)
{
    auto rsNode = RSCanvasNode::Create();
    constexpr uint32_t colorValue = 0x45ba87;
    rsNode->SetShadowColor(colorValue);
    EXPECT_TRUE(rsNode->GetStagingProperties().GetShadowColor() == Color::FromArgbInt(colorValue));
}

/**
 * @tc.name: SetandGetShadowColor003
 * @tc.desc:
 * @tc.type:FUNC
 */
TEST_F(RSNodeTest, SetandGetShadowColor003)
{
    auto rsNode = RSCanvasNode::Create();
    constexpr uint32_t colorValue = 0x32aadd;
    rsNode->SetShadowColor(colorValue);
    EXPECT_TRUE(rsNode->GetStagingProperties().GetShadowColor() == Color::FromArgbInt(colorValue));
}

/**
 * @tc.name: SetandGetShadowColor004
 * @tc.desc:
 * @tc.type:FUNC
 */
TEST_F(RSNodeTest, SetandGetShadowColor004)
{
    auto rsNode = RSCanvasNode::Create();
    constexpr uint32_t colorValue = std::numeric_limits<uint32_t>::max();
    rsNode->SetShadowColor(colorValue);
    EXPECT_TRUE(rsNode->GetStagingProperties().GetShadowColor() == Color::FromArgbInt(colorValue));
}

/**
 * @tc.name: SetandGetShadowColor005
 * @tc.desc:
 * @tc.type:FUNC
 */
TEST_F(RSNodeTest, SetandGetShadowColor005)
{
    auto rsNode = RSCanvasNode::Create();
    constexpr uint32_t colorValue = std::numeric_limits<uint32_t>::min();
    rsNode->SetShadowColor(colorValue);
    EXPECT_TRUE(rsNode->GetStagingProperties().GetShadowColor() == Color::FromArgbInt(colorValue));
}

/**
 * @tc.name: SetandGetTranslateThree001
 * @tc.desc:
 * @tc.type:FUNC
 */
TEST_F(RSNodeTest, SetandGetTranslateThree001)
{
    auto rsNode = RSCanvasNode::Create();
    rsNode->SetTranslate(floatData[1], floatData[2], floatData[3]);
    EXPECT_TRUE(ROSEN_EQ(rsNode->GetStagingProperties().GetTranslate().x_, floatData[1]));
    EXPECT_TRUE(ROSEN_EQ(rsNode->GetStagingProperties().GetTranslate().y_, floatData[2]));
    EXPECT_TRUE(ROSEN_EQ(rsNode->GetStagingProperties().GetTranslateZ(), floatData[3]));
}

/**
 * @tc.name: SetandGetTranslateThree002
 * @tc.desc:
 * @tc.type:FUNC
 */
TEST_F(RSNodeTest, SetandGetTranslateThree002)
{
    auto rsNode = RSCanvasNode::Create();
    rsNode->SetTranslate(floatData[0], floatData[0], floatData[2]);
    EXPECT_TRUE(ROSEN_EQ(rsNode->GetStagingProperties().GetTranslate().x_, floatData[0]));
    EXPECT_TRUE(ROSEN_EQ(rsNode->GetStagingProperties().GetTranslate().y_, floatData[0]));
    EXPECT_TRUE(ROSEN_EQ(rsNode->GetStagingProperties().GetTranslateZ(), floatData[2]));
}

/**
 * @tc.name: SetandGetTranslateThree003
 * @tc.desc:
 * @tc.type:FUNC
 */
TEST_F(RSNodeTest, SetandGetTranslateThree003)
{
    auto rsNode = RSCanvasNode::Create();
    rsNode->SetTranslate(floatData[1], floatData[3], floatData[1]);
    EXPECT_TRUE(ROSEN_EQ(rsNode->GetStagingProperties().GetTranslate().x_, floatData[1]));
    EXPECT_TRUE(ROSEN_EQ(rsNode->GetStagingProperties().GetTranslate().y_, floatData[3]));
    EXPECT_TRUE(ROSEN_EQ(rsNode->GetStagingProperties().GetTranslateZ(), floatData[1]));
}

/**
 * @tc.name: SetandGetTranslateX001
 * @tc.desc:
 * @tc.type:FUNC
 */
TEST_F(RSNodeTest, SetandGetTranslateX001)
{
    auto rsNode = RSCanvasNode::Create();
    rsNode->SetTranslateX(floatData[1]);
    EXPECT_TRUE(ROSEN_EQ(rsNode->GetStagingProperties().GetTranslate().x_, floatData[1]));
}

/**
 * @tc.name: SetandGetTranslateX002
 * @tc.desc:
 * @tc.type:FUNC
 */
TEST_F(RSNodeTest, SetandGetTranslateX002)
{
    auto rsNode = RSCanvasNode::Create();
    rsNode->SetTranslateX(floatData[2]);
    EXPECT_TRUE(ROSEN_EQ(rsNode->GetStagingProperties().GetTranslate().x_, floatData[2]));
}

/**
 * @tc.name: SetandGetTranslateX003
 * @tc.desc:
 * @tc.type:FUNC
 */
TEST_F(RSNodeTest, SetandGetTranslateX003)
{
    auto rsNode = RSCanvasNode::Create();
    rsNode->SetTranslateX(floatData[3]);
    EXPECT_TRUE(ROSEN_EQ(rsNode->GetStagingProperties().GetTranslate().x_, floatData[3]));
}

/**
 * @tc.name: SetandGetTranslateX004
 * @tc.desc:
 * @tc.type:FUNC
 */
TEST_F(RSNodeTest, SetandGetTranslateX004)
{
    auto rsNode = RSCanvasNode::Create();
    rsNode->SetTranslateX(floatData[4]);
    EXPECT_TRUE(ROSEN_EQ(rsNode->GetStagingProperties().GetTranslate().x_, floatData[4]));
}

/**
 * @tc.name: SetandGetTranslateX005
 * @tc.desc:
 * @tc.type:FUNC
 */
TEST_F(RSNodeTest, SetandGetTranslateX005)
{
    auto rsNode = RSCanvasNode::Create();
    rsNode->SetTranslateX(floatData[0]);
    EXPECT_TRUE(ROSEN_EQ(rsNode->GetStagingProperties().GetTranslate().x_, floatData[0]));
}

/**
 * @tc.name: SetandGetTranslateY001
 * @tc.desc:
 * @tc.type:FUNC
 */
TEST_F(RSNodeTest, SetandGetTranslateY001)
{
    auto rsNode = RSCanvasNode::Create();
    rsNode->SetTranslateY(floatData[1]);
    EXPECT_TRUE(ROSEN_EQ(rsNode->GetStagingProperties().GetTranslate().y_, floatData[1]));
}

/**
 * @tc.name: SetandGetTranslateY002
 * @tc.desc:
 * @tc.type:FUNC
 */
TEST_F(RSNodeTest, SetandGetTranslateY002)
{
    auto rsNode = RSCanvasNode::Create();
    rsNode->SetTranslateY(floatData[2]);
    EXPECT_TRUE(ROSEN_EQ(rsNode->GetStagingProperties().GetTranslate().y_, floatData[2]));
}

/**
 * @tc.name: SetandGetTranslateY003
 * @tc.desc:
 * @tc.type:FUNC
 */
TEST_F(RSNodeTest, SetandGetTranslateY003)
{
    auto rsNode = RSCanvasNode::Create();
    rsNode->SetTranslateY(floatData[3]);
    EXPECT_TRUE(ROSEN_EQ(rsNode->GetStagingProperties().GetTranslate().y_, floatData[3]));
}

/**
 * @tc.name: SetandGetTranslateY004
 * @tc.desc:
 * @tc.type:FUNC
 */
TEST_F(RSNodeTest, SetandGetTranslateY004)
{
    auto rsNode = RSCanvasNode::Create();
    rsNode->SetTranslateY(floatData[4]);
    EXPECT_TRUE(ROSEN_EQ(rsNode->GetStagingProperties().GetTranslate().y_, floatData[4]));
}

/**
 * @tc.name: SetandGetTranslateY005
 * @tc.desc:
 * @tc.type:FUNC
 */
TEST_F(RSNodeTest, SetandGetTranslateY005)
{
    auto rsNode = RSCanvasNode::Create();
    rsNode->SetTranslateY(floatData[0]);
    EXPECT_TRUE(ROSEN_EQ(rsNode->GetStagingProperties().GetTranslate().y_, floatData[0]));
}

/**
 * @tc.name: SetandGetTranslateZ001
 * @tc.desc:
 * @tc.type:FUNC
 */
TEST_F(RSNodeTest, SetandGetTranslateZ001)
{
    auto rsNode = RSCanvasNode::Create();
    rsNode->SetTranslateZ(floatData[1]);
    EXPECT_TRUE(ROSEN_EQ(rsNode->GetStagingProperties().GetTranslateZ(), floatData[1]));
}

/**
 * @tc.name: SetandGetTranslateZ002
 * @tc.desc:
 * @tc.type:FUNC
 */
TEST_F(RSNodeTest, SetandGetTranslateZ002)
{
    auto rsNode = RSCanvasNode::Create();
    rsNode->SetTranslateZ(floatData[2]);
    EXPECT_TRUE(ROSEN_EQ(rsNode->GetStagingProperties().GetTranslateZ(), floatData[2]));
}

/**
 * @tc.name: SetandGetTranslateZ003
 * @tc.desc:
 * @tc.type:FUNC
 */
TEST_F(RSNodeTest, SetandGetTranslateZ003)
{
    auto rsNode = RSCanvasNode::Create();
    rsNode->SetTranslateZ(floatData[3]);
    EXPECT_TRUE(ROSEN_EQ(rsNode->GetStagingProperties().GetTranslateZ(), floatData[3]));
}

/**
 * @tc.name: SetandGetTranslateZ004
 * @tc.desc:
 * @tc.type:FUNC
 */
TEST_F(RSNodeTest, SetandGetTranslateZ004)
{
    auto rsNode = RSCanvasNode::Create();
    rsNode->SetTranslateZ(floatData[4]);
    EXPECT_TRUE(ROSEN_EQ(rsNode->GetStagingProperties().GetTranslateZ(), floatData[4]));
}

/**
 * @tc.name: SetandGetTranslateZ005
 * @tc.desc:
 * @tc.type:FUNC
 */
TEST_F(RSNodeTest, SetandGetTranslateZ005)
{
    auto rsNode = RSCanvasNode::Create();
    rsNode->SetTranslateZ(floatData[0]);
    EXPECT_TRUE(ROSEN_EQ(rsNode->GetStagingProperties().GetTranslateZ(), floatData[0]));
}

/**
 * @tc.name: SetandGetClipToBounds001
 * @tc.desc:
 * @tc.type:FUNC
 */
TEST_F(RSNodeTest, SetandGetClipToBounds001)
{
    auto rsNode = RSCanvasNode::Create();
    rsNode->SetClipToBounds(true);
    EXPECT_EQ(rsNode->GetStagingProperties().GetClipToBounds(), true);
}

/**
 * @tc.name: SetandGetClipToBounds002
 * @tc.desc:
 * @tc.type:FUNC
 */
TEST_F(RSNodeTest, SetandGetClipToBounds002)
{
    auto rsNode = RSCanvasNode::Create();
    rsNode->SetClipToBounds(false);
    EXPECT_EQ(rsNode->GetStagingProperties().GetClipToBounds(), false);
}

/**
 * @tc.name: SetandGetClipToFrame001
 * @tc.desc:
 * @tc.type:FUNC
 */
TEST_F(RSNodeTest, SetandGetClipToFrame001)
{
    auto rsNode = RSCanvasNode::Create();
    rsNode->SetClipToFrame(true);
    EXPECT_EQ(rsNode->GetStagingProperties().GetClipToFrame(), true);
}

/**
 * @tc.name: SetandGetClipToFrame002
 * @tc.desc:
 * @tc.type:FUNC
 */
TEST_F(RSNodeTest, SetandGetClipToFrame002)
{
    auto rsNode = RSCanvasNode::Create();
    rsNode->SetClipToFrame(false);
    EXPECT_EQ(rsNode->GetStagingProperties().GetClipToFrame(), false);
}

/**
 * @tc.name: SetandGetVisible001
 * @tc.desc:
 * @tc.type:FUNC
 */
TEST_F(RSNodeTest, SetandGetVisible001)
{
    auto rsNode = RSCanvasNode::Create();
    rsNode->SetVisible(true);
    EXPECT_EQ(rsNode->GetStagingProperties().GetVisible(), true);
}

/**
 * @tc.name: SetandGetVisible002
 * @tc.desc:
 * @tc.type:FUNC
 */
TEST_F(RSNodeTest, SetandGetVisible002)
{
    auto rsNode = RSCanvasNode::Create();
    rsNode->SetVisible(false);
    EXPECT_EQ(rsNode->GetStagingProperties().GetVisible(), false);
}

/**
 * @tc.name: SetandGetBorderStyle001
 * @tc.desc:
 * @tc.type:FUNC
 */
TEST_F(RSNodeTest, SetandGetBorderStyle001)
{
    auto rsNode = RSCanvasNode::Create();
    uint32_t borderStyle = static_cast<uint32_t>(BorderStyle::SOLID);
    rsNode->SetBorderStyle(borderStyle);
    EXPECT_TRUE(rsNode->GetStagingProperties().GetBorderStyle().x_ == borderStyle);
}

/**
 * @tc.name: SetandGetBorderStyle002
 * @tc.desc:
 * @tc.type:FUNC
 */
TEST_F(RSNodeTest, SetandGetBorderStyle002)
{
    auto rsNode = RSCanvasNode::Create();
    uint32_t borderStyle = static_cast<uint32_t>(BorderStyle::DASHED);
    rsNode->SetBorderStyle(borderStyle);
    EXPECT_TRUE(rsNode->GetStagingProperties().GetBorderStyle().x_ == borderStyle);
}

/**
 * @tc.name: SetandGetBorderStyle003
 * @tc.desc:
 * @tc.type:FUNC
 */
TEST_F(RSNodeTest, SetandGetBorderStyle003)
{
    auto rsNode = RSCanvasNode::Create();
    uint32_t borderStyle = static_cast<uint32_t>(BorderStyle::DOTTED);
    rsNode->SetBorderStyle(borderStyle);
    EXPECT_TRUE(rsNode->GetStagingProperties().GetBorderStyle().x_ == borderStyle);
}

/**
 * @tc.name: SetandGetBorderStyle004
 * @tc.desc:
 * @tc.type:FUNC
 */
TEST_F(RSNodeTest, SetandGetBorderStyle004)
{
    auto rsNode = RSCanvasNode::Create();
    uint32_t borderStyle = static_cast<uint32_t>(BorderStyle::NONE);
    rsNode->SetBorderStyle(borderStyle);
    EXPECT_TRUE(rsNode->GetStagingProperties().GetBorderStyle().x_ == borderStyle);
}

/**
 * @tc.name: SetandGetFrameGravity001
 * @tc.desc:
 * @tc.type:FUNC
 */
TEST_F(RSNodeTest, SetandGetFrameGravity001)
{
    auto rsNode = RSCanvasNode::Create();
    Gravity gravity = Gravity::CENTER;
    rsNode->SetFrameGravity(gravity);
    EXPECT_TRUE(rsNode->GetStagingProperties().GetFrameGravity() == gravity);
}

/**
 * @tc.name: SetandGetFrameGravity002
 * @tc.desc:
 * @tc.type:FUNC
 */
TEST_F(RSNodeTest, SetandGetFrameGravity002)
{
    auto rsNode = RSCanvasNode::Create();
    Gravity gravity = Gravity::TOP;
    rsNode->SetFrameGravity(gravity);
    EXPECT_TRUE(rsNode->GetStagingProperties().GetFrameGravity() == gravity);
}

/**
 * @tc.name: SetandGetFrameGravity003
 * @tc.desc:
 * @tc.type:FUNC
 */
TEST_F(RSNodeTest, SetandGetFrameGravity003)
{
    auto rsNode = RSCanvasNode::Create();
    Gravity gravity = Gravity::BOTTOM;
    rsNode->SetFrameGravity(gravity);
    EXPECT_TRUE(rsNode->GetStagingProperties().GetFrameGravity() == gravity);
}

/**
 * @tc.name: SetandGetFrameGravity004
 * @tc.desc:
 * @tc.type:FUNC
 */
TEST_F(RSNodeTest, SetandGetFrameGravity004)
{
    auto rsNode = RSCanvasNode::Create();
    Gravity gravity = Gravity::LEFT;
    rsNode->SetFrameGravity(gravity);
    EXPECT_TRUE(rsNode->GetStagingProperties().GetFrameGravity() == gravity);
}

/**
 * @tc.name: SetandGetFrameGravity005
 * @tc.desc:
 * @tc.type:FUNC
 */
TEST_F(RSNodeTest, SetandGetFrameGravity005)
{
    auto rsNode = RSCanvasNode::Create();
    Gravity gravity = Gravity::RIGHT;
    rsNode->SetFrameGravity(gravity);
    EXPECT_TRUE(rsNode->GetStagingProperties().GetFrameGravity() == gravity);
}

/**
 * @tc.name: SetandGetFrameGravity006
 * @tc.desc:
 * @tc.type:FUNC
 */
TEST_F(RSNodeTest, SetandGetFrameGravity006)
{
    auto rsNode = RSCanvasNode::Create();
    Gravity gravity = Gravity::TOP_LEFT;
    rsNode->SetFrameGravity(gravity);
    EXPECT_TRUE(rsNode->GetStagingProperties().GetFrameGravity() == gravity);
}

/**
 * @tc.name: SetandGetFrameGravity007
 * @tc.desc:
 * @tc.type:FUNC
 */
TEST_F(RSNodeTest, SetandGetFrameGravity007)
{
    auto rsNode = RSCanvasNode::Create();
    Gravity gravity = Gravity::TOP_RIGHT;
    rsNode->SetFrameGravity(gravity);
    EXPECT_TRUE(rsNode->GetStagingProperties().GetFrameGravity() == gravity);
}

/**
 * @tc.name: SetandGetFrameGravity008
 * @tc.desc:
 * @tc.type:FUNC
 */
TEST_F(RSNodeTest, SetandGetFrameGravity008)
{
    auto rsNode = RSCanvasNode::Create();
    Gravity gravity = Gravity::BOTTOM_LEFT;
    rsNode->SetFrameGravity(gravity);
    EXPECT_TRUE(rsNode->GetStagingProperties().GetFrameGravity() == gravity);
}

/**
 * @tc.name: SetandGetFrameGravity009
 * @tc.desc:
 * @tc.type:FUNC
 */
TEST_F(RSNodeTest, SetandGetFrameGravity009)
{
    auto rsNode = RSCanvasNode::Create();
    Gravity gravity = Gravity::BOTTOM_RIGHT;
    rsNode->SetFrameGravity(gravity);
    EXPECT_TRUE(rsNode->GetStagingProperties().GetFrameGravity() == gravity);
}

/**
 * @tc.name: SetandGetFrameGravity010
 * @tc.desc:
 * @tc.type:FUNC
 */
TEST_F(RSNodeTest, SetandGetFrameGravity010)
{
    auto rsNode = RSCanvasNode::Create();
    Gravity gravity = Gravity::RESIZE;
    rsNode->SetFrameGravity(gravity);
    EXPECT_TRUE(rsNode->GetStagingProperties().GetFrameGravity() == gravity);
}

/**
 * @tc.name: SetandGetFrameGravity011
 * @tc.desc:
 * @tc.type:FUNC
 */
TEST_F(RSNodeTest, SetandGetFrameGravity011)
{
    auto rsNode = RSCanvasNode::Create();
    Gravity gravity = Gravity::RESIZE_ASPECT;
    rsNode->SetFrameGravity(gravity);
    EXPECT_TRUE(rsNode->GetStagingProperties().GetFrameGravity() == gravity);
}

/**
 * @tc.name: SetandGetFrameGravity012
 * @tc.desc:
 * @tc.type:FUNC
 */
TEST_F(RSNodeTest, SetandGetFrameGravity012)
{
    auto rsNode = RSCanvasNode::Create();
    Gravity gravity = Gravity::RESIZE_ASPECT_FILL;
    rsNode->SetFrameGravity(gravity);
    EXPECT_TRUE(rsNode->GetStagingProperties().GetFrameGravity() == gravity);
}

/**
 * @tc.name: SetandGetForegroundColor001
 * @tc.desc:
 * @tc.type:FUNC
 */
TEST_F(RSNodeTest, SetandGetForegroundColor001)
{
    auto rsNode = RSCanvasNode::Create();
    constexpr uint32_t colorValue = 0x034123;
    rsNode->SetForegroundColor(colorValue);
    EXPECT_TRUE(rsNode->GetStagingProperties().GetForegroundColor() == Color::FromArgbInt(colorValue));
}

/**
 * @tc.name: SetandGetForegroundColor002
 * @tc.desc:
 * @tc.type:FUNC
 */
TEST_F(RSNodeTest, SetandGetForegroundColor002)
{
    auto rsNode = RSCanvasNode::Create();
    constexpr uint32_t colorValue = std::numeric_limits<uint32_t>::max();
    rsNode->SetForegroundColor(colorValue);
    EXPECT_TRUE(rsNode->GetStagingProperties().GetForegroundColor() == Color::FromArgbInt(colorValue));
}

/**
 * @tc.name: SetandGetForegroundColor003
 * @tc.desc:
 * @tc.type:FUNC
 */
TEST_F(RSNodeTest, SetandGetForegroundColor003)
{
    auto rsNode = RSCanvasNode::Create();
    constexpr uint32_t colorValue = std::numeric_limits<uint32_t>::min();
    rsNode->SetForegroundColor(colorValue);
    EXPECT_TRUE(rsNode->GetStagingProperties().GetForegroundColor() == Color::FromArgbInt(colorValue));
}

/**
 * @tc.name: SetandGetBackgroundColor001
 * @tc.desc:
 * @tc.type:FUNC
 */
TEST_F(RSNodeTest, SetandGetBackgroundColor001)
{
    auto rsNode = RSCanvasNode::Create();
    constexpr uint32_t colorValue = 0x034123;
    rsNode->SetBackgroundColor(colorValue);
    EXPECT_TRUE(rsNode->GetStagingProperties().GetBackgroundColor() == Color::FromArgbInt(colorValue));
}

/**
 * @tc.name: SetandGetBackgroundColor002
 * @tc.desc:
 * @tc.type:FUNC
 */
TEST_F(RSNodeTest, SetandGetBackgroundColor002)
{
    auto rsNode = RSCanvasNode::Create();
    constexpr uint32_t colorValue = std::numeric_limits<uint32_t>::max();
    rsNode->SetBackgroundColor(colorValue);
    EXPECT_TRUE(rsNode->GetStagingProperties().GetBackgroundColor() == Color::FromArgbInt(colorValue));
}

/**
 * @tc.name: SetandGetBackgroundColor003
 * @tc.desc:
 * @tc.type:FUNC
 */
TEST_F(RSNodeTest, SetandGetBackgroundColor003)
{
    auto rsNode = RSCanvasNode::Create();
    constexpr uint32_t colorValue = std::numeric_limits<uint32_t>::min();
    rsNode->SetBackgroundColor(colorValue);
    EXPECT_TRUE(rsNode->GetStagingProperties().GetBackgroundColor() == Color::FromArgbInt(colorValue));
}

/**
 * @tc.name: SetandGetBorderColor001
 * @tc.desc:
 * @tc.type:FUNC
 */
TEST_F(RSNodeTest, SetandGetBorderColor001)
{
    auto rsNode = RSCanvasNode::Create();
    constexpr uint32_t colorValue = 0x034123;
    rsNode->SetBorderColor(colorValue);
    EXPECT_TRUE(rsNode->GetStagingProperties().GetBorderColor() == Color::FromArgbInt(colorValue));
}

/**
 * @tc.name: SetandGetBorderColor002
 * @tc.desc:
 * @tc.type:FUNC
 */
TEST_F(RSNodeTest, SetandGetBorderColor002)
{
    auto rsNode = RSCanvasNode::Create();
    constexpr uint32_t colorValue = std::numeric_limits<uint32_t>::max();
    rsNode->SetBorderColor(colorValue);
    EXPECT_TRUE(rsNode->GetStagingProperties().GetBorderColor() == Color::FromArgbInt(colorValue));
}

/**
 * @tc.name: SetandGetBorderColor003
 * @tc.desc:
 * @tc.type:FUNC
 */
TEST_F(RSNodeTest, SetandGetBorderColor003)
{
    auto rsNode = RSCanvasNode::Create();
    constexpr uint32_t colorValue = std::numeric_limits<uint32_t>::min();
    rsNode->SetBorderColor(colorValue);
    EXPECT_TRUE(rsNode->GetStagingProperties().GetBorderColor() == Color::FromArgbInt(colorValue));
}

/**
 * @tc.name: SetandGetRotationVector001
 * @tc.desc:
 * @tc.type:FUNC
 */
TEST_F(RSNodeTest, SetandGetRotationVector001)
{
    auto rsNode = RSCanvasNode::Create();
    Vector4f quaternion(std::numeric_limits<float>::min(), 2.f, 3.f, 4.f);
    rsNode->SetRotation(quaternion);
}

/**
 * @tc.name: SetandGetTranslateVector001
 * @tc.desc:
 * @tc.type:FUNC
 */
TEST_F(RSNodeTest, SetandGetTranslateVector001)
{
    auto rsNode = RSCanvasNode::Create();
    Vector2f quaternion(std::numeric_limits<float>::max(), 2.f);
    rsNode->SetTranslate(quaternion);
}

/**
 * @tc.name: CreateBlurFilter001
 * @tc.desc:
 * @tc.type:FUNC
 */
TEST_F(RSNodeTest, CreateBlurFilter001)
{
    auto rsNode = RSCanvasNode::Create();
    std::shared_ptr<RSFilter> backgroundFilter = RSFilter::CreateBlurFilter(floatData[0], floatData[1]);
    rsNode->SetBackgroundFilter(backgroundFilter);
    EXPECT_TRUE(rsNode->GetStagingProperties().GetBackgroundFilter() == backgroundFilter);
}

/**
 * @tc.name: CreateBlurFilter002
 * @tc.desc:
 * @tc.type:FUNC
 */
TEST_F(RSNodeTest, CreateBlurFilter002)
{
    auto rsNode = RSCanvasNode::Create();
    std::shared_ptr<RSFilter> backgroundFilter = RSFilter::CreateBlurFilter(floatData[1], floatData[2]);
    rsNode->SetBackgroundFilter(backgroundFilter);
    EXPECT_TRUE(rsNode->GetStagingProperties().GetBackgroundFilter() == backgroundFilter);
}

/**
 * @tc.name: CreateBlurFilter003
 * @tc.desc:
 * @tc.type:FUNC
 */
TEST_F(RSNodeTest, CreateBlurFilter003)
{
    auto rsNode = RSCanvasNode::Create();
    std::shared_ptr<RSFilter> backgroundFilter = RSFilter::CreateBlurFilter(floatData[2], floatData[3]);
    rsNode->SetBackgroundFilter(backgroundFilter);
    EXPECT_TRUE(rsNode->GetStagingProperties().GetBackgroundFilter() == backgroundFilter);
}

/**
 * @tc.name: CreateBlurFilter004
 * @tc.desc:
 * @tc.type:FUNC
 */
TEST_F(RSNodeTest, CreateBlurFilter004)
{
    auto rsNode = RSCanvasNode::Create();
    std::shared_ptr<RSFilter> backgroundFilter = RSFilter::CreateBlurFilter(floatData[3], floatData[4]);
    rsNode->SetBackgroundFilter(backgroundFilter);
    EXPECT_TRUE(rsNode->GetStagingProperties().GetBackgroundFilter() == backgroundFilter);
}

/**
 * @tc.name: CreateBlurFilter005
 * @tc.desc:
 * @tc.type:FUNC
 */
TEST_F(RSNodeTest, CreateBlurFilter005)
{
    auto rsNode = RSCanvasNode::Create();
    std::shared_ptr<RSFilter> backgroundFilter = RSFilter::CreateBlurFilter(floatData[4], floatData[0]);
    rsNode->SetBackgroundFilter(backgroundFilter);
    EXPECT_TRUE(rsNode->GetStagingProperties().GetBackgroundFilter() == backgroundFilter);
}

/**
 * @tc.name: CreateNormalFilter001
 * @tc.desc:
 * @tc.type:FUNC
 */
TEST_F(RSNodeTest, CreateNormalFilter001)
{
    auto rsNode = RSCanvasNode::Create();
    std::shared_ptr<RSFilter> filter = RSFilter::CreateBlurFilter(floatData[0], floatData[1]);
    rsNode->SetFilter(filter);
    EXPECT_TRUE(rsNode->GetStagingProperties().GetFilter() == filter);
}

/**
 * @tc.name: CreateNormalFilter002
 * @tc.desc:
 * @tc.type:FUNC
 */
TEST_F(RSNodeTest, CreateNormalFilter002)
{
    auto rsNode = RSCanvasNode::Create();
    std::shared_ptr<RSFilter> filter = RSFilter::CreateBlurFilter(floatData[1], floatData[2]);
    rsNode->SetFilter(filter);
    EXPECT_TRUE(rsNode->GetStagingProperties().GetFilter() == filter);
}

/**
 * @tc.name: CreateNormalFilter003
 * @tc.desc:
 * @tc.type:FUNC
 */
TEST_F(RSNodeTest, CreateNormalFilter003)
{
    auto rsNode = RSCanvasNode::Create();
    std::shared_ptr<RSFilter> filter = RSFilter::CreateBlurFilter(floatData[2], floatData[3]);
    rsNode->SetFilter(filter);
    EXPECT_TRUE(rsNode->GetStagingProperties().GetFilter() == filter);
}

/**
 * @tc.name: CreateNormalFilter004
 * @tc.desc:
 * @tc.type:FUNC
 */
TEST_F(RSNodeTest, CreateNormalFilter004)
{
    auto rsNode = RSCanvasNode::Create();
    std::shared_ptr<RSFilter> filter = RSFilter::CreateBlurFilter(floatData[3], floatData[4]);
    rsNode->SetFilter(filter);
    EXPECT_TRUE(rsNode->GetStagingProperties().GetFilter() == filter);
}

/**
 * @tc.name: CreateNormalFilter005
 * @tc.desc:
 * @tc.type:FUNC
 */
TEST_F(RSNodeTest, CreateNormalFilter005)
{
    auto rsNode = RSCanvasNode::Create();
    std::shared_ptr<RSFilter> filter = RSFilter::CreateBlurFilter(floatData[4], floatData[0]);
    rsNode->SetFilter(filter);
    EXPECT_TRUE(rsNode->GetStagingProperties().GetFilter() == filter);
}

/**
 * @tc.name: SetandGetClipBounds001
 * @tc.desc:
 * @tc.type:FUNC
 */
TEST_F(RSNodeTest, SetandGetClipBounds001)
{
    auto rsNode = RSCanvasNode::Create();
    std::shared_ptr<RSPath> clipPath = RSPath::CreateRSPath();
    rsNode->SetClipBounds(clipPath);
    EXPECT_TRUE(rsNode->GetStagingProperties().GetClipBounds() == clipPath);
}

/**
 * @tc.name: GetId001
 * @tc.desc:
 * @tc.type:FUNC
 */
TEST_F(RSNodeTest, GetId001)
{
    auto rsNode = RSCanvasNode::Create();
    rsNode->GetId();
}

/**
 * @tc.name: GetChildren001
 * @tc.desc:
 * @tc.type:FUNC
 */
TEST_F(RSNodeTest, GetChildren001)
{
    auto rsNode = RSCanvasNode::Create();
    auto c = rsNode->GetChildren();
}

/**
 * @tc.name: GetChildren002
 * @tc.desc:
 * @tc.type:FUNC
 */
TEST_F(RSNodeTest, GetChildren002)
{
    auto rsNode = RSCanvasNode::Create();
    const auto c = rsNode->GetChildren();
}

/**
 * @tc.name: GetStagingProperties001
 * @tc.desc:
 * @tc.type:FUNC
 */
TEST_F(RSNodeTest, GetStagingProperties001)
{
    auto rsNode = RSCanvasNode::Create();
    rsNode->GetStagingProperties();
}

/**
 * @tc.name: GetMotionPathOption001
 * @tc.desc:
 * @tc.type:FUNC
 */
TEST_F(RSNodeTest, GetMotionPathOption002)
{
    auto rsNode = RSCanvasNode::Create();
    rsNode->GetMotionPathOption();
}

/**
 * @tc.name: SetBgImage001
 * @tc.desc:
 * @tc.type:FUNC
 */
TEST_F(RSNodeTest, SetBgImage001)
{
    auto rsNode = RSCanvasNode::Create();
    auto image = std::make_shared<RSImage>();
    rsNode->SetBgImage(image);
}

/**
 * @tc.name: SetBackgroundShader001
 * @tc.desc:
 * @tc.type:FUNC
 */
TEST_F(RSNodeTest, SetBackgroundShader001)
{
    auto rsNode = RSCanvasNode::Create();
    auto shader = RSShader::CreateRSShader();
    rsNode->SetBackgroundShader(shader);
}

/**
 * @tc.name: SetCompositingFilter001
 * @tc.desc:
 * @tc.type:FUNC
 */
TEST_F(RSNodeTest, SetCompositingFilter001)
{
    auto rsNode = RSCanvasNode::Create();
    auto compositingFilter = RSFilter::CreateBlurFilter(0.0f, 0.0f);
    rsNode->SetCompositingFilter(compositingFilter);
}

/**
 * @tc.name: SetShadowPath001
 * @tc.desc:
 * @tc.type:FUNC
 */
TEST_F(RSNodeTest, SetShadowPath001)
{
    auto rsNode = RSCanvasNode::Create();
    auto shadowpath = RSPath::CreateRSPath();
    rsNode->SetShadowPath(shadowpath);
}

template<typename ModifierName, typename PropertyName, typename T>
void SetPropertyTest(RSModifierType modifierType, T value1, T value2)
{
    auto node = RSCanvasNode::Create();
    node->SetProperty<ModifierName, PropertyName, T>(modifierType, value1);
    node->SetProperty<ModifierName, PropertyName, T>(modifierType, value1);
    node->SetProperty<ModifierName, PropertyName, T>(modifierType, value2);
};

/**
 * @tc.name: SetProperty001
 * @tc.desc:
 * @tc.type:FUNC
 */
TEST_F(RSNodeTest, SetProperty001)
{
    SetPropertyTest<RSAlphaModifier, RSAnimatableProperty<float>, float>(
        RSModifierType::ALPHA, 0.0f, 0.5f);
    SetPropertyTest<RSAlphaOffscreenModifier, RSProperty<bool>, bool>(
        RSModifierType::ALPHA_OFFSCREEN, false, true);
    SetPropertyTest<RSBoundsModifier, RSAnimatableProperty<Vector4f>, Vector4f>(
        RSModifierType::BOUNDS, Vector4f(), Vector4f(25.f));
    SetPropertyTest<RSFrameModifier, RSAnimatableProperty<Vector4f>, Vector4f>(
        RSModifierType::FRAME, Vector4f(), Vector4f(25.f));
    SetPropertyTest<RSPositionZModifier, RSAnimatableProperty<float>, float>(
        RSModifierType::POSITION_Z, 1.f, 2.f);
    SetPropertyTest<RSPivotModifier, RSAnimatableProperty<Vector2f>, Vector2f>(
        RSModifierType::PIVOT, Vector2f(0.f, 0.f), Vector2f(1.f, 1.f));
    SetPropertyTest<RSCornerRadiusModifier, RSAnimatableProperty<Vector4f>, Vector4f>(
        RSModifierType::CORNER_RADIUS, Vector4f(), Vector4f(1.f));
    SetPropertyTest<RSQuaternionModifier, RSAnimatableProperty<Quaternion>, Quaternion>(
        RSModifierType::QUATERNION, Quaternion(), Quaternion(0.f, 1.f, 0.f, 0.f));
    SetPropertyTest<RSRotationModifier, RSAnimatableProperty<float>, float>(
        RSModifierType::ROTATION, 45.f, 90.f);
    SetPropertyTest<RSRotationXModifier, RSAnimatableProperty<float>, float>(
        RSModifierType::ROTATION_X, 45.f, 90.f);
    SetPropertyTest<RSRotationYModifier, RSAnimatableProperty<float>, float>(
        RSModifierType::ROTATION_Y, 45.f, 90.f);
    SetPropertyTest<RSTranslateModifier, RSAnimatableProperty<Vector2f>, Vector2f>(
        RSModifierType::TRANSLATE, Vector2f(10.f, 10.f), Vector2f(1.f, 1.f));
    SetPropertyTest<RSTranslateZModifier, RSAnimatableProperty<float>, float>(
        RSModifierType::TRANSLATE_Z, 45.f, 90.f);
    SetPropertyTest<RSScaleModifier, RSAnimatableProperty<Vector2f>, Vector2f>(
        RSModifierType::SCALE, Vector2f(0.f, 0.f), Vector2f(1.f, 1.f));
    SetPropertyTest<RSForegroundColorModifier, RSAnimatableProperty<Color>, Color>(
        RSModifierType::FOREGROUND_COLOR, Color(), Color(0xFF00FF00));
    SetPropertyTest<RSBackgroundColorModifier, RSAnimatableProperty<Color>, Color>(
        RSModifierType::BACKGROUND_COLOR, Color(), Color(0xFF00FF00));
    SetPropertyTest<RSBgImageModifier, RSProperty<std::shared_ptr<RSImage>>, std::shared_ptr<RSImage>>(
        RSModifierType::BG_IMAGE, std::make_shared<RSImage>(), nullptr);
    SetPropertyTest<RSBgImageWidthModifier, RSAnimatableProperty<float>, float>(
        RSModifierType::BG_IMAGE_WIDTH, 45.f, 90.f);
    SetPropertyTest<RSBgImageHeightModifier, RSAnimatableProperty<float>, float>(
        RSModifierType::BG_IMAGE_HEIGHT, 45.f, 90.f);
    SetPropertyTest<RSBorderColorModifier, RSAnimatableProperty<Vector4<Color>>, Vector4<Color>>(
        RSModifierType::BORDER_COLOR, Vector4<Color>(), Vector4<Color>(Color(0xFF00FF00)));
    SetPropertyTest<RSBorderWidthModifier, RSAnimatableProperty<Vector4f>, Vector4f>(
        RSModifierType::BORDER_WIDTH, Vector4f(), Vector4f(1.f));
    SetPropertyTest<RSBorderStyleModifier, RSProperty<Vector4<uint32_t>>, Vector4<uint32_t>>(
        RSModifierType::BORDER_STYLE, Vector4<uint32_t>(), Vector4<uint32_t>(1));
    SetPropertyTest<RSFilterModifier, RSAnimatableProperty<std::shared_ptr<RSFilter>>, std::shared_ptr<RSFilter>>(
        RSModifierType::FILTER, RSFilter::CreateBlurFilter(1.f, 1.f), nullptr);
    SetPropertyTest<RSShadowColorModifier, RSAnimatableProperty<Color>, Color>(
        RSModifierType::SHADOW_COLOR, Color(), Color(0xFF00FF00));
    SetPropertyTest<RSShadowOffsetXModifier, RSAnimatableProperty<float>, float>(
        RSModifierType::SHADOW_OFFSET_X, 1.f, 2.f);
    SetPropertyTest<RSShadowOffsetYModifier, RSAnimatableProperty<float>, float>(
        RSModifierType::SHADOW_OFFSET_Y, 1.f, 2.f);
    SetPropertyTest<RSShadowAlphaModifier, RSAnimatableProperty<float>, float>(
        RSModifierType::SHADOW_ALPHA, 0.2f, 0.5f);
    SetPropertyTest<RSShadowElevationModifier, RSAnimatableProperty<float>, float>(
        RSModifierType::SHADOW_ELEVATION, 1.f, 2.f);
    SetPropertyTest<RSShadowRadiusModifier, RSAnimatableProperty<float>, float>(
        RSModifierType::SHADOW_RADIUS, 1.f, 2.f);
    SetPropertyTest<RSShadowPathModifier, RSProperty<std::shared_ptr<RSPath>>, std::shared_ptr<RSPath>>(
        RSModifierType::SHADOW_PATH, RSPath::CreateRSPath(), nullptr);
    SetPropertyTest<RSFrameGravityModifier, RSProperty<Gravity>, Gravity>(
        RSModifierType::FRAME_GRAVITY, Gravity::TOP_RIGHT, Gravity::RESIZE);
    SetPropertyTest<RSClipBoundsModifier, RSProperty<std::shared_ptr<RSPath>>, std::shared_ptr<RSPath>>(
        RSModifierType::CLIP_BOUNDS, RSPath::CreateRSPath(), nullptr);
    SetPropertyTest<RSClipToBoundsModifier, RSProperty<bool>, bool>(
        RSModifierType::CLIP_TO_BOUNDS, false, true);
    SetPropertyTest<RSClipToFrameModifier, RSProperty<bool>, bool>(
        RSModifierType::CLIP_TO_FRAME, false, true);
    SetPropertyTest<RSVisibleModifier, RSProperty<bool>, bool>(
        RSModifierType::VISIBLE, false, true);
    SetPropertyTest<RSMaskModifier, RSProperty<std::shared_ptr<RSMask>>, std::shared_ptr<RSMask>>(
        RSModifierType::MASK, std::make_shared<RSMask>(), nullptr);
}

/**
 * @tc.name: SetModifier001
 * @tc.desc:
 * @tc.type:FUNC
 */
TEST_F(RSNodeTest, SetModifier001)
{
    RSSurfaceNodeConfig surfaceNodeConfig;
    auto node = RSSurfaceNode::Create(surfaceNodeConfig, false);
    auto value = Vector4f(100.f);
    auto prop = std::make_shared<RSAnimatableProperty<Vector4f>>(value);
    auto modifier = std::make_shared<RSBoundsModifier>(prop);

    node->AddModifier(nullptr);
    node->RemoveModifier(modifier);
    node->AddModifier(modifier);
    ASSERT_TRUE(node != nullptr);
    ASSERT_EQ(node->GetStagingProperties().GetBounds(), value);
    node->AddModifier(modifier);
    node->RemoveModifier(modifier);
}


class RSC_EXPORT MockRSNode : public RSNode {
public:
    MockRSNode() : RSNode(false) {}
    virtual ~MockRSNode() = default;
    std::vector<PropertyId> GetModifierIds() const {
        return RSNode::GetModifierIds();
    }
};

/**
 * @tc.name: SetModifier002
 * @tc.desc:
 * @tc.type:FUNC
 */
TEST_F(RSNodeTest, SetModifier002)
{
    auto node = RSCanvasNode::Create();
    node->SetMotionPathOption(std::make_shared<RSMotionPathOption>(""));
    {
        auto value = Vector4f(100.f);
        auto prop = std::make_shared<RSAnimatableProperty<Vector4f>>(value);
        auto modifier = std::make_shared<RSBoundsModifier>(prop);
        node->AddModifier(modifier);
        ASSERT_TRUE(node != nullptr);
        ASSERT_EQ(node->GetStagingProperties().GetBounds(), value);
        node->RemoveModifier(modifier);
    }
    {
        auto value = Vector4f(100.f);
        auto prop = std::make_shared<RSAnimatableProperty<Vector4f>>(value);
        auto modifier = std::make_shared<RSFrameModifier>(prop);
        node->AddModifier(modifier);
        ASSERT_TRUE(node != nullptr);
        ASSERT_EQ(node->GetStagingProperties().GetFrame(), value);
        node->RemoveModifier(modifier);
    }
    {
        auto value = Vector2f(200.f, 300.f);
        auto prop = std::make_shared<RSAnimatableProperty<Vector2f>>(value);
        auto modifier = std::make_shared<RSTranslateModifier>(prop);
        node->AddModifier(modifier);
        ASSERT_TRUE(node != nullptr);
        ASSERT_EQ(node->GetStagingProperties().GetTranslate(), value);
        node->RemoveModifier(modifier);
    }
    {
        auto value = Vector2f(2.f, 2.f);
        auto prop = std::make_shared<RSAnimatableProperty<Vector2f>>(value);
        auto modifier = std::make_shared<RSScaleModifier>(prop);
        node->AddModifier(modifier);
        ASSERT_TRUE(node != nullptr);
        ASSERT_EQ(node->GetStagingProperties().GetScale(), value);
        node->RemoveModifier(modifier);
    }

    auto node1 = std::make_shared<MockRSNode>();
    node1->DumpNode(0);
    auto animation = std::make_shared<RSTransition>(RSTransitionEffect::OPACITY, true);
    animation->SetDuration(100);
    animation->SetTimingCurve(RSAnimationTimingCurve::EASE_IN_OUT);
    animation->SetFinishCallback([]() {});
    node1->AddAnimation(animation);
    node1->DumpNode(0);
    ASSERT_TRUE(node1->GetModifierIds().size() == 0);
    auto value = Vector2f(2.f, 2.f);
    auto prop = std::make_shared<RSAnimatableProperty<Vector2f>>(value);
    auto modifier = std::make_shared<RSScaleModifier>(prop);
    node1->AddModifier(modifier);
    node1->DumpNode(0);
    ASSERT_TRUE(node1->GetModifierIds().size() == 1);
}
} // namespace OHOS::Rosen
