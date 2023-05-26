/*
 * Copyright (c) 2022 Huawei Device Co., Ltd.
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

#include <gtest/gtest.h>
#include <hilog/log.h>
#include <memory>
#include <unistd.h>

#include "pipeline/rs_base_render_node.h"
#include "pipeline/rs_context.h"

using namespace testing::ext;

namespace OHOS {
namespace Rosen {
static const int nodeId1 = 1;
static const int nodeId2 = 2;

class RSDropframeTest : public testing::Test {
public:
    static constexpr HiviewDFX::HiLogLabel LOG_LABEL = { LOG_CORE, 0, "RSDropframeTest" };

    static inline std::shared_ptr<RSContext> context = nullptr;
    static inline std::shared_ptr<RSBaseRenderNode> node1 = nullptr;
    static inline std::shared_ptr<RSBaseRenderNode> node2 = nullptr;

    static void SetUpTestCase();
    static void TearDownTestCase();
    void SetUp() override {};
    void TearDown() override;
};

void RSDropframeTest::SetUpTestCase()
{
    context = std::make_shared<RSContext>();
    node1 = std::make_shared<RSBaseRenderNode>(nodeId1, context->weak_from_this());
    node2 = std::make_shared<RSBaseRenderNode>(nodeId2, context->weak_from_this());
    context->GetMutableNodeMap().RegisterRenderNode(node1);
    context->GetMutableNodeMap().RegisterRenderNode(node2);
}

void RSDropframeTest::TearDownTestCase()
{
    context->GetMutableNodeMap().UnregisterRenderNode(node1->GetId());
    context->GetMutableNodeMap().UnregisterRenderNode(node2->GetId());

    node1 = nullptr;
    node2 = nullptr;
    context = nullptr;
}

void RSDropframeTest::TearDown()
{
    auto rootNode = context->GetGlobalRootRenderNode();
    rootNode->ClearChildren();
    node1->ClearChildren();
    node2->ClearChildren();
}

/*
* Function: GetGlobalRootRenderNode
* Type: Function
* Rank: Important(nodeId2)
* EnvConditions: N/A
* CaseDescription: 1. call GetGlobalRootRenderNode
*                  2. check IsOnTheTree
*/
HWTEST_F(RSDropframeTest, GetGlobalRootRenderNode, Function | SmallTest | Level2)
{
    // call GetGlobalRootRenderNode
    auto rootNode = context->GetGlobalRootRenderNode();

    // check IsOnTheTree
    bool rootNodeIsOnTheTree = rootNode->IsOnTheTree();
    EXPECT_EQ(rootNodeIsOnTheTree, true);
}

/*
* Function: AddChild001
* Type: Function
* Rank: Important(nodeId2)
* EnvConditions: N/A
* CaseDescription: 1. call AddChild
*                  2. check IsOnTheTree
*/
HWTEST_F(RSDropframeTest, AddChild001, Function | SmallTest | Level2)
{
    // call AddChild
    auto rootNode = context->GetGlobalRootRenderNode();
    auto child = context->GetMutableNodeMap().GetRenderNode<RSBaseRenderNode>(nodeId1);
    rootNode->AddChild(child);

    // check IsOnTheTree
    bool isOnTheTree = child->IsOnTheTree();
    EXPECT_EQ(isOnTheTree, true);
}

/*
* Function: AddChild002
* Type: Function
* Rank: Important(nodeId2)
* EnvConditions: N/A
* CaseDescription: 1. call AddChild
*                  2. check IsOnTheTree
*/
HWTEST_F(RSDropframeTest, AddChild002, Function | SmallTest | Level2)
{
    // call AddChild
    auto parent = context->GetMutableNodeMap().GetRenderNode<RSBaseRenderNode>(nodeId1);
    auto child = context->GetMutableNodeMap().GetRenderNode<RSBaseRenderNode>(nodeId2);
    parent->AddChild(child, -1);
    
    // check IsOnTheTree
    bool isOnTheTree = child->IsOnTheTree();
    EXPECT_EQ(isOnTheTree, false);
}

/*
* Function: RemoveChild
* Type: Function
* Rank: Important(nodeId2)
* EnvConditions: N/A
* CaseDescription: 1. call RemoveChild
*                  2. check IsOnTheTree
*/
HWTEST_F(RSDropframeTest, RemoveChild, Function | SmallTest | Level2)
{
    // build render tree
    auto rootNode = context->GetGlobalRootRenderNode();
    auto child1 = context->GetMutableNodeMap().GetRenderNode<RSBaseRenderNode>(nodeId1);
    auto child2 = context->GetMutableNodeMap().GetRenderNode<RSBaseRenderNode>(nodeId2);
    child1->AddChild(child2, -1);
    rootNode->AddChild(child1, -1);

    // call RemoveChild
    rootNode->RemoveChild(child1);

    // check IsOnTheTree
    bool child1IsOnTheTree = child1->IsOnTheTree();
    bool child2IsOnTheTree = child2->IsOnTheTree();
    EXPECT_EQ(child1IsOnTheTree, false);
    EXPECT_EQ(child2IsOnTheTree, false);
}

/*
* Function: RemoveFromTree
* Type: Function
* Rank: Important(nodeId2)
* EnvConditions: N/A
* CaseDescription: 1. call RemoveFromTree
*                  2. check IsOnTheTree
*/
HWTEST_F(RSDropframeTest, RemoveFromTree, Function | SmallTest | Level2)
{
    // build render tree
    auto rootNode = context->GetGlobalRootRenderNode();
    auto child1 = context->GetMutableNodeMap().GetRenderNode<RSBaseRenderNode>(nodeId1);
    auto child2 = context->GetMutableNodeMap().GetRenderNode<RSBaseRenderNode>(nodeId2);
    child1->AddChild(child2, -1);
    rootNode->AddChild(child1, -1);
    
    // call RemoveFromTree
    child1->RemoveFromTree();

    // check IsOnTheTree
    bool child1IsOnTheTree = child1->IsOnTheTree();
    bool child2IsOnTheTree = child2->IsOnTheTree();
    EXPECT_EQ(child1IsOnTheTree, false);
    EXPECT_EQ(child2IsOnTheTree, false);
}

/*
* Function: ClearChildren
* Type: Function
* Rank: Important(nodeId2)
* EnvConditions: N/A
* CaseDescription: 1. call ClearChildren
*                  2. check IsOnTheTree
*/
HWTEST_F(RSDropframeTest, ClearChildren, Function | SmallTest | Level2)
{
    // build render tree
    auto rootNode = context->GetGlobalRootRenderNode();
    auto child1 = context->GetMutableNodeMap().GetRenderNode<RSBaseRenderNode>(nodeId1);
    auto child2 = context->GetMutableNodeMap().GetRenderNode<RSBaseRenderNode>(nodeId2);
    rootNode->AddChild(child1, -1);
    rootNode->AddChild(child2, -1);
    
    // call ClearChildren
    rootNode->ClearChildren();

    // check IsOnTheTree
    bool child1IsOnTheTree = child1->IsOnTheTree();
    bool child2IsOnTheTree = child2->IsOnTheTree();
    EXPECT_EQ(child1IsOnTheTree, false);
    EXPECT_EQ(child2IsOnTheTree, false);
}
} // namespace Rosen
} // namespace OHOS