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
#include "minimize_app.h"

using namespace testing;
using namespace testing::ext;

namespace OHOS {
namespace Rosen {
class MinimizeAppTest : public testing::Test {
public:
    static void SetUpTestCase();
    static void TearDownTestCase();
    virtual void SetUp() override;
    virtual void TearDown() override;

    sptr<WindowProperty> CreateWindowProperty(uint32_t windowId);
};

void MinimizeAppTest::SetUpTestCase()
{
}

void MinimizeAppTest::TearDownTestCase()
{
}

void MinimizeAppTest::SetUp()
{
}

void MinimizeAppTest::TearDown()
{
}

sptr<WindowProperty> MinimizeAppTest::CreateWindowProperty(uint32_t windowId)
{
    sptr<WindowProperty> property = new WindowProperty();
    property->SetWindowId(windowId);
    return property;
}
namespace {
/**
 * @tc.name: MinimizeAppTest01
 * @tc.desc: add and excute
 * @tc.type: FUNC
 */
HWTEST_F(MinimizeAppTest, MinimizeAppTest01, Function | SmallTest | Level2)
{
    sptr<WindowNode> node1 = new WindowNode(CreateWindowProperty(1));
    sptr<WindowNode> node2 = new WindowNode(CreateWindowProperty(2));
    sptr<WindowNode> node3 = new WindowNode(CreateWindowProperty(3));
    sptr<WindowNode> node4 = new WindowNode(CreateWindowProperty(4));
    sptr<WindowNode> node5 = new WindowNode(CreateWindowProperty(5));
    sptr<WindowNode> node6 = new WindowNode(CreateWindowProperty(6));
    sptr<WindowNode> node7 = new WindowNode(CreateWindowProperty(7));
    sptr<WindowNode> node8 = new WindowNode(CreateWindowProperty(8));
    sptr<WindowNode> node9 = new WindowNode(CreateWindowProperty(9));

    sptr<WindowNode> node10 = new WindowNode();
    sptr<WindowNode> node11 = nullptr;
    sptr<WindowNode> conflictNode = new WindowNode(CreateWindowProperty(2));

    MinimizeApp::AddNeedMinimizeApp(node1, MinimizeReason::MINIMIZE_BUTTON);
    MinimizeApp::AddNeedMinimizeApp(node2, MinimizeReason::MINIMIZE_ALL);
    MinimizeApp::AddNeedMinimizeApp(node3, MinimizeReason::LAYOUT_TILE);
    MinimizeApp::AddNeedMinimizeApp(node4, MinimizeReason::LAYOUT_CASCADE);
    MinimizeApp::AddNeedMinimizeApp(node5, MinimizeReason::MAX_APP_COUNT);
    MinimizeApp::AddNeedMinimizeApp(node6, MinimizeReason::SPLIT_REPLACE);
    MinimizeApp::AddNeedMinimizeApp(node7, MinimizeReason::SPLIT_QUIT);
    MinimizeApp::AddNeedMinimizeApp(node8, MinimizeReason::GESTURE_ANIMATION);
    MinimizeApp::AddNeedMinimizeApp(node9, MinimizeReason::OTHER_WINDOW);
    MinimizeApp::AddNeedMinimizeApp(conflictNode, MinimizeReason::MINIMIZE_ALL);

    ASSERT_EQ(true, MinimizeApp::IsNodeNeedMinimize(node1));
    ASSERT_EQ(true, MinimizeApp::IsNodeNeedMinimize(node2));
    ASSERT_EQ(true, MinimizeApp::IsNodeNeedMinimize(node3));
    ASSERT_EQ(true, MinimizeApp::IsNodeNeedMinimize(node4));
    ASSERT_EQ(true, MinimizeApp::IsNodeNeedMinimize(node5));
    ASSERT_EQ(true, MinimizeApp::IsNodeNeedMinimize(node6));
    ASSERT_EQ(true, MinimizeApp::IsNodeNeedMinimize(node7));
    ASSERT_EQ(true, MinimizeApp::IsNodeNeedMinimize(node8));
    ASSERT_EQ(true, MinimizeApp::IsNodeNeedMinimize(node9));

    ASSERT_EQ(false, MinimizeApp::IsNodeNeedMinimize(node10));
    ASSERT_EQ(false, MinimizeApp::IsNodeNeedMinimize(node11));
    ASSERT_EQ(false, MinimizeApp::IsNodeNeedMinimize(conflictNode));

    MinimizeApp::ExecuteMinimizeTargetReasons(MinimizeReason::SPLIT_REPLACE);
    MinimizeApp::ExecuteMinimizeAll();
}
/**
 * @tc.name: MinimizeAppTest02
 * @tc.desc: add, find and clear
 * @tc.type: FUNC
 */
HWTEST_F(MinimizeAppTest, MinimizeAppTest02, Function | SmallTest | Level2)
{
    sptr<WindowNode> node1 = new WindowNode();

    MinimizeApp::AddNeedMinimizeApp(node1, MinimizeReason::MINIMIZE_ALL);

    auto getNodes = MinimizeApp::GetNeedMinimizeAppNodesWithReason(MinimizeReason::MINIMIZE_ALL);
    ASSERT_EQ(node1, getNodes[0]);

    MinimizeApp::ClearNodesWithReason(MinimizeReason::MINIMIZE_ALL);
}
/**
 * @tc.name: MinimizeAppTest03
 * @tc.desc: add and find
 * @tc.type: FUNC
 */
HWTEST_F(MinimizeAppTest, MinimizeAppTest03, Function | SmallTest | Level2)
{
    MinimizeApp::SetMinimizedByOtherConfig(false);
    sptr<WindowNode> node1 = new WindowNode();

    MinimizeApp::AddNeedMinimizeApp(node1, MinimizeReason::OTHER_WINDOW);
    ASSERT_EQ(false, MinimizeApp::IsNodeNeedMinimize(node1));
    MinimizeApp::SetMinimizedByOtherConfig(true);

    MinimizeApp::ClearNodesWithReason(MinimizeReason::OTHER_WINDOW);
}
/**
 * @tc.name: MinimizeAppTest04
 * @tc.desc: add and recover
 * @tc.type: FUNC
 */
HWTEST_F(MinimizeAppTest, MinimizeAppTest04, Function | SmallTest | Level2)
{
    sptr<WindowNode> node1 = new WindowNode();
    MinimizeApp::AddNeedMinimizeApp(node1, MinimizeReason::LAYOUT_TILE);

    ASSERT_EQ(node1, MinimizeApp::GetRecoverdNodeFromMinimizeList());
    ASSERT_EQ(nullptr, MinimizeApp::GetRecoverdNodeFromMinimizeList());

    MinimizeApp::ClearNodesWithReason(MinimizeReason::LAYOUT_TILE);
}
/**
 * @tc.name: MinimizeAppTest05
 * @tc.desc: insert nullptr to needMinimizeAppNodes_
 * @tc.type: FUNC
 */
HWTEST_F(MinimizeAppTest, MinimizeAppTest05, Function | SmallTest | Level2)
{
    MinimizeApp::needMinimizeAppNodes_[MinimizeReason::MINIMIZE_ALL].emplace_back(nullptr);
    auto getNodes = MinimizeApp::GetNeedMinimizeAppNodesWithReason(MinimizeReason::MINIMIZE_ALL);
    ASSERT_EQ(nullptr, getNodes[0].promote());

    sptr<WindowNode> node1 = new WindowNode();
    MinimizeApp::AddNeedMinimizeApp(node1, MinimizeReason::MINIMIZE_ALL);
    auto getNodes2 = MinimizeApp::GetNeedMinimizeAppNodesWithReason(MinimizeReason::MINIMIZE_ALL);
    ASSERT_EQ(node1, getNodes2[1]);

    MinimizeApp::ClearNodesWithReason(MinimizeReason::MINIMIZE_ALL);
}
/**
 * @tc.name: IsNodeNeedMinimizeWithReason
 * @tc.desc: check node need minimize or not
 * @tc.type: FUNC
 */
HWTEST_F(MinimizeAppTest, IsNodeNeedMinimizeWithReason01, Function | SmallTest | Level2)
{
    ASSERT_EQ(false, MinimizeApp::IsNodeNeedMinimizeWithReason(nullptr, MinimizeReason::MINIMIZE_ALL));

    sptr<WindowNode> node1 = new WindowNode();
    sptr<WindowNode> node2 = new WindowNode();
    MinimizeApp::AddNeedMinimizeApp(node1, MinimizeReason::MINIMIZE_BUTTON);
    MinimizeApp::AddNeedMinimizeApp(node2, MinimizeReason::MINIMIZE_ALL);

    ASSERT_EQ(false, MinimizeApp::IsNodeNeedMinimizeWithReason(node1, MinimizeReason::GESTURE_ANIMATION));
    ASSERT_EQ(false, MinimizeApp::IsNodeNeedMinimizeWithReason(node1, MinimizeReason::MINIMIZE_ALL));
    ASSERT_EQ(true, MinimizeApp::IsNodeNeedMinimizeWithReason(node1, MinimizeReason::MINIMIZE_BUTTON));

    MinimizeApp::ClearNodesWithReason(MinimizeReason::MINIMIZE_ALL);
    MinimizeApp::ClearNodesWithReason(MinimizeReason::MINIMIZE_BUTTON);
}
/**
 * @tc.name: ClearNodesWithReason
 * @tc.desc: clear node with reason
 * @tc.type: FUNC
 */
HWTEST_F(MinimizeAppTest, ClearNodesWithReason01, Function | SmallTest | Level2)
{
    sptr<WindowNode> node1 = new WindowNode();
    sptr<WindowNode> node2 = new WindowNode();
    MinimizeApp::AddNeedMinimizeApp(node1, MinimizeReason::MINIMIZE_BUTTON);
    MinimizeApp::AddNeedMinimizeApp(node2, MinimizeReason::MINIMIZE_ALL);

    MinimizeApp::ClearNodesWithReason(MinimizeReason::MINIMIZE_ALL);
    ASSERT_EQ(0, MinimizeApp::GetNeedMinimizeAppNodesWithReason(MinimizeReason::MINIMIZE_ALL).size());
    MinimizeApp::ClearNodesWithReason(MinimizeReason::GESTURE_ANIMATION);
    ASSERT_EQ(1, MinimizeApp::GetNeedMinimizeAppNodesWithReason(MinimizeReason::MINIMIZE_BUTTON).size());
}
/**
 * @tc.name: GetRecoverdNodeFromMinimizeList
 * @tc.desc: Get recoverd node from list
 * @tc.type: FUNC
 */
HWTEST_F(MinimizeAppTest, GetRecoverdNodeFromMinimizeList01, Function | SmallTest | Level2)
{
    sptr<WindowNode> node1 = new WindowNode();
    MinimizeApp::AddNeedMinimizeApp(node1, MinimizeReason::MINIMIZE_BUTTON);

    ASSERT_EQ(nullptr, MinimizeApp::GetRecoverdNodeFromMinimizeList());
    MinimizeApp::ClearNodesWithReason(MinimizeReason::MINIMIZE_BUTTON);
}
/**
 * @tc.name: IsNodeNeedMinimize
 * @tc.desc: check node need minize or not
 * @tc.type: FUNC
 */
HWTEST_F(MinimizeAppTest, IsNodeNeedMinimize01, Function | SmallTest | Level2)
{
    ASSERT_EQ(false, MinimizeApp::IsNodeNeedMinimize(nullptr));
}
/**
 * @tc.name: ExecuteMinimizeTargetReasons
 * @tc.desc: Execute Minimize With TargetReason
 * @tc.type: FUNC
 */
HWTEST_F(MinimizeAppTest, ExecuteMinimizeTargetReasons01, Function | SmallTest | Level2)
{
    sptr<WindowNode> node1 = new WindowNode();
    MinimizeApp::AddNeedMinimizeApp(node1, MinimizeReason::MINIMIZE_BUTTON);

    MinimizeApp::ExecuteMinimizeTargetReasons(MinimizeReason::MINIMIZE_BUTTON);
    ASSERT_EQ(0, MinimizeApp::GetNeedMinimizeAppNodesWithReason(MinimizeReason::MINIMIZE_BUTTON).size());

    MinimizeApp::AddNeedMinimizeApp(node1, MinimizeReason::MINIMIZE_BUTTON);
    MinimizeApp::ExecuteMinimizeTargetReasons(MinimizeReason::GESTURE_ANIMATION);
    ASSERT_EQ(1, MinimizeApp::GetNeedMinimizeAppNodesWithReason(MinimizeReason::MINIMIZE_BUTTON).size());
}
}
}
}
