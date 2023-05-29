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
#include <algorithm>
#include "window_pair.h"
#include "minimize_app.h"
#include "common_test_utils.h"
#include "mock_IWindow.h"
using namespace testing;
using namespace testing::ext;

namespace OHOS {
namespace Rosen {
class WindowPairTest : public testing::Test {
public:
    static void SetUpTestCase();
    static void TearDownTestCase();
    void SetUp() override;
    void TearDown() override;
};

void WindowPairTest::SetUpTestCase()
{
}

void WindowPairTest::TearDownTestCase()
{
}

void WindowPairTest::SetUp()
{
}

void WindowPairTest::TearDown()
{
}

namespace {
/**
 * @tc.name: NotifyShowRecent
 * @tc.desc: Send split screen event to notify create recent view.
 * @tc.type: FUNC
 */
HWTEST_F(WindowPairTest, NotifyShowRecent01, Function | SmallTest | Level2)
{
    sptr<WindowPair> windowPair = new WindowPair(0);
    windowPair->primary_ = nullptr;
    windowPair->secondary_ = nullptr;
    windowPair->NotifyShowRecent(nullptr);
    ASSERT_EQ(nullptr, windowPair->primary_);

    sptr<WindowProperty> property = new WindowProperty();
    property->SetWindowMode(WindowMode::WINDOW_MODE_SPLIT_PRIMARY);
    sptr<WindowNode> node0 = new WindowNode(property);
    windowPair->primary_ = node0;
    windowPair->NotifyShowRecent(node0);
    
    if (windowPair->secondary_ != nullptr) {
        ASSERT_EQ(WindowType::WINDOW_TYPE_LAUNCHER_RECENT, windowPair->secondary_->GetWindowType());
    }
}

/**
 * @tc.name: NotifyCreateOrDestroyDivider
 * @tc.desc: Send split screen event to notify create or destroy divider window.
 * @tc.type: FUNC
 */
HWTEST_F(WindowPairTest, NotifyCreateOrDestroyDivider01, Function | SmallTest | Level2)
{
    sptr<WindowPair> windowPair = new WindowPair(0);
    windowPair->primary_ = nullptr;
    windowPair->NotifyCreateOrDestroyDivider(nullptr, true);
    ASSERT_EQ(nullptr, windowPair->primary_);

    sptr<WindowProperty> property = new WindowProperty();
    property->SetWindowMode(WindowMode::WINDOW_MODE_SPLIT_PRIMARY);
    sptr<WindowNode> node0 = new WindowNode(property);
    windowPair->primary_ = node0;
    windowPair->NotifyCreateOrDestroyDivider(node0, true);
    ASSERT_EQ(nullptr, windowPair->divider_);
}

/**
 * @tc.name: IsPaired
 * @tc.desc: Get whether the window pair is paired
 * @tc.type: FUNC
 */
HWTEST_F(WindowPairTest, IsPaired01, Function | SmallTest | Level2)
{
    sptr<WindowPair> windowPair = new WindowPair(0);
    windowPair->primary_ = nullptr;
    ASSERT_EQ(false, windowPair->IsPaired());
    windowPair->primary_ = new WindowNode();
    windowPair->secondary_ = nullptr;
    ASSERT_EQ(false, windowPair->IsPaired());
}

/**
 * @tc.name: IsPaired
 * @tc.desc: Get whether the window pair is paired
 * @tc.type: FUNC
 */
HWTEST_F(WindowPairTest, IsPaired02, Function | SmallTest | Level2)
{
    sptr<WindowPair> windowPair = new WindowPair(0);
    sptr<WindowProperty> property1 = new WindowProperty();
    property1->SetWindowMode(WindowMode::WINDOW_MODE_SPLIT_PRIMARY);
    sptr<WindowProperty> property2 = new WindowProperty();
    property2->SetWindowMode(WindowMode::WINDOW_MODE_SPLIT_SECONDARY);

    sptr<WindowNode> node1 = new WindowNode(property1);
    windowPair->primary_ = node1;
    sptr<WindowNode> node2 = new WindowNode(property2);
    windowPair->secondary_ = node2;
    windowPair->divider_ = node1;
    ASSERT_EQ(true, windowPair->IsPaired());
}

/**
 * @tc.name: IsPaired
 * @tc.desc: Get whether the window pair is paired
 * @tc.type: FUNC
 */
HWTEST_F(WindowPairTest, IsPaired03, Function | SmallTest | Level2)
{
    sptr<WindowPair> windowPair = new WindowPair(0);
    sptr<WindowProperty> property1 = new WindowProperty();
    property1->SetWindowMode(WindowMode::WINDOW_MODE_SPLIT_PRIMARY);
    sptr<WindowProperty> property2 = new WindowProperty();
    property2->SetWindowMode(WindowMode::WINDOW_MODE_SPLIT_SECONDARY);

    sptr<WindowNode> node1 = new WindowNode(property1);
    windowPair->primary_ = node1;
    sptr<WindowNode> node2 = new WindowNode(property2);
    windowPair->secondary_ = node2;
    windowPair->divider_ = nullptr;
    ASSERT_EQ(false, windowPair->IsPaired());
}

/**
 * @tc.name: Find
 * @tc.desc: Find window node from window pair
 * @tc.type: FUNC
 */
HWTEST_F(WindowPairTest, Find01, Function | SmallTest | Level2)
{
    sptr<WindowPair> windowPair = new WindowPair(0);
    sptr<WindowNode> node1 = nullptr;
    ASSERT_EQ(nullptr, windowPair->Find(node1));
}

/**
 * @tc.name: Find
 * @tc.desc: Find window node from window pair
 * @tc.type: FUNC
 */
HWTEST_F(WindowPairTest, Find02, Function | SmallTest | Level2)
{
    sptr<WindowPair> windowPair = new WindowPair(0);
    sptr<WindowProperty> property1 = new WindowProperty();
    property1->SetWindowId(1);
    sptr<WindowNode> node1 = new WindowNode(property1);
    windowPair->primary_ = node1;
    ASSERT_EQ(node1, windowPair->Find(node1));
}

/**
 * @tc.name: Find
 * @tc.desc: Find window node from window pair
 * @tc.type: FUNC
 */
HWTEST_F(WindowPairTest, Find03, Function | SmallTest | Level2)
{
    sptr<WindowPair> windowPair = new WindowPair(0);
    sptr<WindowProperty> property1 = new WindowProperty();
    property1->SetWindowId(1);
    sptr<WindowNode> node1 = new WindowNode(property1);
    windowPair->primary_ = nullptr;
    windowPair->secondary_ = node1;
    ASSERT_EQ(node1, windowPair->Find(node1));
}

/**
 * @tc.name: Find
 * @tc.desc: Find window node from window pair
 * @tc.type: FUNC
 */
HWTEST_F(WindowPairTest, Find04, Function | SmallTest | Level2)
{
    sptr<WindowPair> windowPair = new WindowPair(0);
    sptr<WindowProperty> property1 = new WindowProperty();
    property1->SetWindowId(1);
    sptr<WindowNode> node1 = new WindowNode(property1);
    windowPair->primary_ = nullptr;
    windowPair->secondary_ = nullptr;
    windowPair->divider_ = node1;
    ASSERT_EQ(node1, windowPair->Find(node1));
}

/**
 * @tc.name: GetSplitRatio
 * @tc.desc: Get split ratio
 * @tc.type: FUNC
 */
HWTEST_F(WindowPairTest, GetSplitRatio01, Function | SmallTest | Level2)
{
    sptr<WindowPair> windowPair = new WindowPair(0);
    windowPair->ratio_ = 0;
    ASSERT_EQ(0, windowPair->GetSplitRatio());
    windowPair->ratio_ = 5;
    ASSERT_EQ(5, windowPair->GetSplitRatio());
}

/**
 * @tc.name: IsForbidDockSliceMove
 * @tc.desc: Get whether dock slice is forbidden to move
 * @tc.type: FUNC
 */
HWTEST_F(WindowPairTest, IsForbidDockSliceMove01, Function | SmallTest | Level2)
{
    sptr<WindowPair> windowPair = new WindowPair(0);
    windowPair->status_ = WindowPairStatus::STATUS_PAIRED_DONE;
    ASSERT_EQ(true, windowPair->IsForbidDockSliceMove());
    windowPair->status_ = WindowPairStatus::STATUS_EMPTY;
    ASSERT_EQ(false, windowPair->IsForbidDockSliceMove());
}

/**
 * @tc.name: IsForbidDockSliceMove
 * @tc.desc: Get whether dock slice is forbidden to move
 * @tc.type: FUNC
 */
HWTEST_F(WindowPairTest, IsForbidDockSliceMove02, Function | SmallTest | Level2)
{
    sptr<WindowPair> windowPair = new WindowPair(0);
    windowPair->status_ = WindowPairStatus::STATUS_PAIRED_DONE;
    sptr<WindowProperty> property1 = new WindowProperty();
    property1->SetWindowFlags(static_cast<uint32_t>(WindowFlag::WINDOW_FLAG_NEED_AVOID));
    windowPair->primary_ = new WindowNode(property1);
    windowPair->secondary_ = new WindowNode(property1);
    ASSERT_EQ(false, windowPair->IsForbidDockSliceMove());
}

/**
 * @tc.name: IsForbidDockSliceMove
 * @tc.desc: Get whether dock slice is forbidden to move
 * @tc.type: FUNC
 */
HWTEST_F(WindowPairTest, IsForbidDockSliceMove03, Function | SmallTest | Level2)
{
    sptr<WindowPair> windowPair = new WindowPair(0);
    windowPair->status_ = WindowPairStatus::STATUS_PAIRED_DONE;
    sptr<WindowProperty> property1 = new WindowProperty();
    property1->SetWindowFlags(static_cast<uint32_t>(WindowFlag::WINDOW_FLAG_NEED_AVOID));
    windowPair->primary_ = nullptr;
    windowPair->secondary_ = new WindowNode(property1);
    ASSERT_EQ(true, windowPair->IsForbidDockSliceMove());
}

/**
 * @tc.name: IsForbidDockSliceMove
 * @tc.desc: Get whether dock slice is forbidden to move
 * @tc.type: FUNC
 */
HWTEST_F(WindowPairTest, IsForbidDockSliceMove04, Function | SmallTest | Level2)
{
    sptr<WindowPair> windowPair = new WindowPair(0);
    windowPair->status_ = WindowPairStatus::STATUS_PAIRED_DONE;
    sptr<WindowProperty> property1 = new WindowProperty();
    property1->SetWindowFlags(static_cast<uint32_t>(WindowFlag::WINDOW_FLAG_FORBID_SPLIT_MOVE));
    windowPair->primary_ = new WindowNode(property1);
    windowPair->secondary_ = new WindowNode(property1);
    ASSERT_EQ(true, windowPair->IsForbidDockSliceMove());
}

/**
 * @tc.name: IsForbidDockSliceMove
 * @tc.desc: Get whether dock slice is forbidden to move
 * @tc.type: FUNC
 */
HWTEST_F(WindowPairTest, IsForbidDockSliceMove05, Function | SmallTest | Level2)
{
    sptr<WindowPair> windowPair = new WindowPair(0);
    windowPair->status_ = WindowPairStatus::STATUS_PAIRED_DONE;
    sptr<WindowProperty> property1 = new WindowProperty();
    property1->SetWindowFlags(static_cast<uint32_t>(WindowFlag::WINDOW_FLAG_NEED_AVOID));
    windowPair->primary_ = new WindowNode(property1);
    windowPair->secondary_ = nullptr;
    ASSERT_EQ(true, windowPair->IsForbidDockSliceMove());
}

/**
 * @tc.name: IsForbidDockSliceMove
 * @tc.desc: Get whether dock slice is forbidden to move
 * @tc.type: FUNC
 */
HWTEST_F(WindowPairTest, IsForbidDockSliceMove06, Function | SmallTest | Level2)
{
    sptr<WindowPair> windowPair = new WindowPair(0);
    windowPair->status_ = WindowPairStatus::STATUS_PAIRED_DONE;
    sptr<WindowProperty> property1 = new WindowProperty();
    property1->SetWindowFlags(static_cast<uint32_t>(WindowFlag::WINDOW_FLAG_NEED_AVOID));
    sptr<WindowProperty> property2 = new WindowProperty();
    property2->SetWindowFlags(static_cast<uint32_t>(WindowFlag::WINDOW_FLAG_FORBID_SPLIT_MOVE));
    windowPair->primary_ = new WindowNode(property1);
    windowPair->secondary_ = new WindowNode(property2);
    ASSERT_EQ(true, windowPair->IsForbidDockSliceMove());
}

/**
 * @tc.name: IsDockSliceInExitSplitModeArea
 * @tc.desc: whether dock slice in exit split screen mode area
 * @tc.type: FUNC
 */
HWTEST_F(WindowPairTest, IsDockSliceInExitSplitModeArea01, Function | SmallTest | Level2)
{
    sptr<WindowPair> windowPair = new WindowPair(0);
    std::vector<int32_t> points {0, 0};
    windowPair->primary_ = nullptr;
    ASSERT_EQ(false, windowPair->IsDockSliceInExitSplitModeArea(points));
}

/**
 * @tc.name: IsDockSliceInExitSplitModeArea
 * @tc.desc: whether dock slice in exit split screen mode area
 * @tc.type: FUNC
 */
HWTEST_F(WindowPairTest, IsDockSliceInExitSplitModeArea02, Function | SmallTest | Level2)
{
    sptr<WindowPair> windowPair = new WindowPair(0);
    std::vector<int32_t> points {2, 0};
    sptr<WindowProperty> property1 = new WindowProperty();
    property1->SetWindowMode(WindowMode::WINDOW_MODE_SPLIT_PRIMARY);
    sptr<WindowProperty> property2 = new WindowProperty();
    property2->SetWindowMode(WindowMode::WINDOW_MODE_SPLIT_SECONDARY);
    sptr<WindowProperty> property3 = new WindowProperty();

    sptr<WindowNode> node1 = new WindowNode(property1);
    windowPair->primary_ = node1;
    sptr<WindowNode> node2 = new WindowNode(property2);
    windowPair->secondary_ = node2;
    Rect rect1 = {1, 1, 10, 20};
    Rect rect2 = {1, 1, 20, 10};
    property2->SetWindowRect(rect1);
    property3->SetWindowRect(rect2);
    windowPair->divider_ = new WindowNode(property2);

    ASSERT_EQ(true, windowPair->IsPaired());
    ASSERT_EQ(true, windowPair->IsDockSliceInExitSplitModeArea(points));
    windowPair->divider_ = new WindowNode(property3);
    ASSERT_EQ(true, windowPair->IsDockSliceInExitSplitModeArea(points));
}

/**
 * @tc.name: IsDockSliceInExitSplitModeArea
 * @tc.desc: whether dock slice in exit split screen mode area
 * @tc.type: FUNC
 */
HWTEST_F(WindowPairTest, IsDockSliceInExitSplitModeArea03, Function | SmallTest | Level2)
{
    sptr<WindowPair> windowPair = new WindowPair(0);
    std::vector<int32_t> points {0, 50};
    sptr<WindowProperty> property1 = new WindowProperty();
    property1->SetWindowMode(WindowMode::WINDOW_MODE_SPLIT_PRIMARY);
    sptr<WindowProperty> property2 = new WindowProperty();
    property2->SetWindowMode(WindowMode::WINDOW_MODE_SPLIT_SECONDARY);

    sptr<WindowNode> node1 = new WindowNode(property1);
    windowPair->primary_ = node1;
    sptr<WindowNode> node2 = new WindowNode(property2);
    windowPair->secondary_ = node2;
    Rect rect1 = {1, 1, 10, 20};
    property2->SetWindowRect(rect1);
    windowPair->divider_ = new WindowNode(property2);

    ASSERT_EQ(true, windowPair->IsPaired());
    ASSERT_EQ(false, windowPair->IsDockSliceInExitSplitModeArea(points));
}

/**
 * @tc.name: IsSplitRelated
 * @tc.desc: Gets whether the window is related to split window.
 * @tc.type: FUNC
 */
HWTEST_F(WindowPairTest, IsSplitRelated01, Function | SmallTest | Level2)
{
    sptr<WindowPair> windowPair = new WindowPair(0);
    sptr<WindowNode> node = nullptr;
    ASSERT_EQ(false, windowPair->IsSplitRelated(node));
}

/**
 * @tc.name: IsSplitRelated
 * @tc.desc: Gets whether the window is related to split window.
 * @tc.type: FUNC
 */
HWTEST_F(WindowPairTest, IsSplitRelated02, Function | SmallTest | Level2)
{
    sptr<WindowPair> windowPair = new WindowPair(0);
    sptr<WindowProperty> property1 = new WindowProperty();
    property1->SetWindowMode(WindowMode::WINDOW_MODE_SPLIT_PRIMARY);
    sptr<WindowNode> node1 = new WindowNode(property1);
    ASSERT_EQ(true, windowPair->IsSplitRelated(node1));
    property1->SetWindowMode(WindowMode::WINDOW_MODE_FULLSCREEN);
    property1->SetWindowType(WindowType::WINDOW_TYPE_DOCK_SLICE);
    sptr<WindowNode> node2 = new WindowNode(property1);
    ASSERT_EQ(true, windowPair->IsSplitRelated(node2));
    property1->SetWindowMode(WindowMode::WINDOW_MODE_FULLSCREEN);
    property1->SetWindowType(WindowType::APP_WINDOW_BASE);
    sptr<WindowNode> node3 = new WindowNode(property1);
    ASSERT_EQ(false, windowPair->IsSplitRelated(node3));
}

/**
 * @tc.name: GetOrderedPair
 * @tc.desc: Get all window node form pair in Z order.
 * @tc.type: FUNC
 */
HWTEST_F(WindowPairTest, GetOrderedPair01, Function | SmallTest | Level2)
{
    sptr<WindowPair> windowPair = new WindowPair(0);
    sptr<WindowNode> node1 = nullptr;
    std::vector<sptr<WindowNode>> vec;
    vec.clear();
    ASSERT_EQ(vec, windowPair->GetOrderedPair(node1));
}

/**
 * @tc.name: GetOrderedPair
 * @tc.desc: Get all window node form pair in Z order.
 * @tc.type: FUNC
 */
HWTEST_F(WindowPairTest, GetOrderedPair02, Function | SmallTest | Level2)
{
    sptr<WindowPair> windowPair = new WindowPair(0);

    // create window property
    sptr<WindowProperty> property0 = new WindowProperty();
    property0->SetWindowMode(WindowMode::WINDOW_MODE_SPLIT_PRIMARY);
    property0->SetWindowType(WindowType::APP_MAIN_WINDOW_BASE);
    sptr<WindowProperty> property1 = new WindowProperty();
    property1->SetWindowMode(WindowMode::WINDOW_MODE_SPLIT_SECONDARY);
    property1->SetWindowType(WindowType::APP_MAIN_WINDOW_BASE);

    // define primary_, secondary_, divider_
    sptr<WindowNode> node1 = new WindowNode(property1);
    windowPair->primary_ = new WindowNode(property0);
    windowPair->secondary_ = new WindowNode(property1);
    windowPair->divider_ = new WindowNode(property0);
    
    ASSERT_EQ(3, windowPair->GetOrderedPair(node1).size());
    ASSERT_EQ(windowPair->secondary_, windowPair->GetOrderedPair(node1).at(1));
    ASSERT_EQ(windowPair->primary_, windowPair->GetOrderedPair(node1).at(0));
}

/**
 * @tc.name: GetOrderedPair
 * @tc.desc: Get all window node form pair in Z order.
 * @tc.type: FUNC
 */
HWTEST_F(WindowPairTest, GetOrderedPair03, Function | SmallTest | Level2)
{
    sptr<WindowPair> windowPair = new WindowPair(0);

    // create window property
    sptr<WindowProperty> property0 = new WindowProperty();
    property0->SetWindowMode(WindowMode::WINDOW_MODE_SPLIT_PRIMARY);
    property0->SetWindowType(WindowType::APP_MAIN_WINDOW_BASE);
    sptr<WindowProperty> property1 = new WindowProperty();
    property1->SetWindowMode(WindowMode::WINDOW_MODE_SPLIT_SECONDARY);
    property1->SetWindowType(WindowType::APP_MAIN_WINDOW_BASE);

    // define primary_, secondary_, divider_
    sptr<WindowNode> node1 = new WindowNode(property0);
    windowPair->primary_ = new WindowNode(property0);
    windowPair->secondary_ = new WindowNode(property1);
    windowPair->divider_ = new WindowNode(property0);
    
    ASSERT_EQ(3, windowPair->GetOrderedPair(node1).size());
    ASSERT_EQ(windowPair->secondary_, windowPair->GetOrderedPair(node1).at(0));
    ASSERT_EQ(windowPair->primary_, windowPair->GetOrderedPair(node1).at(1));
}

/**
 * @tc.name: GetOrderedPair
 * @tc.desc: Get all window node form pair in Z order.
 * @tc.type: FUNC
 */
HWTEST_F(WindowPairTest, GetOrderedPair04, Function | SmallTest | Level2)
{
    sptr<WindowPair> windowPair = new WindowPair(0);
    // create window property
    sptr<WindowProperty> property0 = new WindowProperty();
    property0->SetWindowMode(WindowMode::WINDOW_MODE_SPLIT_PRIMARY);
    property0->SetWindowType(WindowType::WINDOW_TYPE_DOCK_SLICE);
    sptr<WindowProperty> property1 = new WindowProperty();
    property1->SetWindowMode(WindowMode::WINDOW_MODE_SPLIT_SECONDARY);
    property1->SetWindowType(WindowType::APP_MAIN_WINDOW_BASE);
    // define primary_, secondary_, divider_
    sptr<WindowNode> node1 = new WindowNode(property0);
    windowPair->primary_ = new WindowNode(property0);
    windowPair->secondary_ = nullptr;
    windowPair->divider_ = new WindowNode(property0);
    ASSERT_EQ(1, windowPair->GetOrderedPair(node1).size());
    ASSERT_EQ(windowPair->divider_, windowPair->GetOrderedPair(node1).at(0));
}

/**
 * @tc.name: GetOrderedPair
 * @tc.desc: Get all window node form pair in Z order.
 * @tc.type: FUNC
 */
HWTEST_F(WindowPairTest, GetOrderedPair05, Function | SmallTest | Level2)
{
    sptr<WindowPair> windowPair = new WindowPair(0);
    // create window property
    sptr<WindowProperty> property0 = new WindowProperty();
    property0->SetWindowMode(WindowMode::WINDOW_MODE_SPLIT_PRIMARY);
    property0->SetWindowType(WindowType::WINDOW_TYPE_DOCK_SLICE);
    sptr<WindowProperty> property1 = new WindowProperty();
    property1->SetWindowMode(WindowMode::WINDOW_MODE_SPLIT_SECONDARY);
    property1->SetWindowType(WindowType::APP_MAIN_WINDOW_BASE);
    // define primary_, secondary_, divider_
    sptr<WindowNode> node1 = new WindowNode(property0);
    windowPair->primary_ = new WindowNode(property0);
    windowPair->secondary_ = new WindowNode(property0);
    windowPair->divider_ = new WindowNode(property0);
    ASSERT_EQ(1, windowPair->GetOrderedPair(node1).size());
    ASSERT_EQ(windowPair->divider_, windowPair->GetOrderedPair(node1).at(0));
}

/**
 * @tc.name: GetOrderedPair
 * @tc.desc: Get all window node form pair in Z order.
 * @tc.type: FUNC
 */
HWTEST_F(WindowPairTest, GetOrderedPair06, Function | SmallTest | Level2)
{
    sptr<WindowPair> windowPair = new WindowPair(0);
    // create window property
    sptr<WindowProperty> property0 = new WindowProperty();
    property0->SetWindowMode(WindowMode::WINDOW_MODE_SPLIT_PRIMARY);
    property0->SetWindowType(WindowType::APP_MAIN_WINDOW_BASE);
    sptr<WindowProperty> property1 = new WindowProperty();
    property1->SetWindowMode(WindowMode::WINDOW_MODE_SPLIT_SECONDARY);
    property1->SetWindowType(WindowType::WINDOW_TYPE_DOCK_SLICE);
    // define primary_, secondary_, divider_
    sptr<WindowNode> node1 = new WindowNode(property0);
    windowPair->primary_ = nullptr;
    windowPair->secondary_ = new WindowNode(property1);
    windowPair->divider_ = new WindowNode(property0);
    ASSERT_EQ(1, windowPair->GetOrderedPair(node1).size());
    ASSERT_EQ(windowPair->divider_, windowPair->GetOrderedPair(node1).at(0));
}

/**
 * @tc.name: GetOrderedPair
 * @tc.desc: Get all window node form pair in Z order.
 * @tc.type: FUNC
 */
HWTEST_F(WindowPairTest, GetOrderedPair07, Function | SmallTest | Level2)
{
    sptr<WindowPair> windowPair = new WindowPair(0);
    // create window property
    sptr<WindowProperty> property0 = new WindowProperty();
    property0->SetWindowMode(WindowMode::WINDOW_MODE_SPLIT_PRIMARY);
    property0->SetWindowType(WindowType::APP_MAIN_WINDOW_BASE);
    sptr<WindowProperty> property1 = new WindowProperty();
    property1->SetWindowMode(WindowMode::WINDOW_MODE_SPLIT_SECONDARY);
    property1->SetWindowType(WindowType::WINDOW_TYPE_DOCK_SLICE);
    // define primary_, secondary_, divider_
    sptr<WindowNode> node1 = new WindowNode(property0);
    windowPair->primary_ = new WindowNode(property1);
    windowPair->secondary_ = new WindowNode(property1);
    windowPair->divider_ = new WindowNode(property0);
    ASSERT_EQ(1, windowPair->GetOrderedPair(node1).size());
    ASSERT_EQ(windowPair->divider_, windowPair->GetOrderedPair(node1).at(0));
}

/**
 * @tc.name: GetPairedWindows
 * @tc.desc: Get all window node form pair.
 * @tc.type: FUNC
 */
HWTEST_F(WindowPairTest, GetPairedWindows01, Function | SmallTest | Level2)
{
    sptr<WindowPair> windowPair = new WindowPair(0);
    sptr<WindowProperty> property0 = new WindowProperty();
    property0->SetWindowMode(WindowMode::WINDOW_MODE_SPLIT_PRIMARY);
    sptr<WindowProperty> property1 = new WindowProperty();
    property1->SetWindowMode(WindowMode::WINDOW_MODE_SPLIT_SECONDARY);

    // define primary_, secondary_
    windowPair->primary_ = new WindowNode(property0);
    windowPair->secondary_ = new WindowNode(property1);

    // define status_
    windowPair->status_ = WindowPairStatus::STATUS_EMPTY;

    ASSERT_EQ(0, windowPair->GetPairedWindows().size());
    windowPair->status_ = WindowPairStatus::STATUS_PAIRED_DONE;
    ASSERT_EQ(2, windowPair->GetPairedWindows().size());
    ASSERT_EQ(windowPair->secondary_, windowPair->GetPairedWindows().at(1));
    ASSERT_EQ(windowPair->primary_, windowPair->GetPairedWindows().at(0));
}

/**
 * @tc.name: Clear
 * @tc.desc: Clear window pair.
 * @tc.type: FUNC
 */
HWTEST_F(WindowPairTest, Clear01, Function | SmallTest | Level2)
{
    sptr<WindowPair> windowPair = new WindowPair(0);
    sptr<WindowProperty> property0 = new WindowProperty();
    property0->SetWindowMode(WindowMode::WINDOW_MODE_SPLIT_PRIMARY);

    // define divider
    windowPair->divider_ = new WindowNode(property0);
    windowPair->Clear();
    ASSERT_EQ(nullptr, windowPair->divider_);
    ASSERT_EQ(WindowPairStatus::STATUS_EMPTY, windowPair->status_);
}

/**
 * @tc.name: UpdateWindowPairStatus
 * @tc.desc: Update pair status
 * @tc.type: FUNC
 */
HWTEST_F(WindowPairTest, UpdateWindowPairStatus01, Function | SmallTest | Level2)
{
    sptr<WindowPair> windowPair = new WindowPair(0);
    sptr<WindowProperty> property0 = new WindowProperty();

    // define primary_, secondary_, divider_
    windowPair->primary_ = new WindowNode(property0);
    windowPair->secondary_ = new WindowNode(property0);
    windowPair->divider_ = new WindowNode(property0);

    // define status_
    windowPair->status_ = WindowPairStatus::STATUS_SINGLE_PRIMARY;

    windowPair->UpdateWindowPairStatus();
    ASSERT_EQ(WindowPairStatus::STATUS_PAIRED_DONE, windowPair->status_);
}

/**
 * @tc.name: UpdateWindowPairStatus
 * @tc.desc: Update pair status
 * @tc.type: FUNC
 */
HWTEST_F(WindowPairTest, UpdateWindowPairStatus02, Function | SmallTest | Level2)
{
    sptr<WindowPair> windowPair = new WindowPair(0);
    sptr<WindowProperty> property0 = new WindowProperty();

    // define primary_, secondary_, divider_
    windowPair->primary_ = new WindowNode(property0);
    windowPair->secondary_ = new WindowNode(property0);
    windowPair->divider_ = nullptr;

    // define status_
    windowPair->status_ = WindowPairStatus::STATUS_SINGLE_PRIMARY;

    windowPair->UpdateWindowPairStatus();
    ASSERT_EQ(WindowPairStatus::STATUS_PAIRING, windowPair->status_);
}

/**
 * @tc.name: UpdateWindowPairStatus
 * @tc.desc: Update pair status
 * @tc.type: FUNC
 */
HWTEST_F(WindowPairTest, UpdateWindowPairStatus03, Function | SmallTest | Level2)
{
    sptr<WindowPair> windowPair = new WindowPair(0);
    sptr<WindowProperty> property0 = new WindowProperty();

    // define primary_, secondary_, divider_
    windowPair->primary_ = new WindowNode(property0);
    windowPair->secondary_ = nullptr;
    windowPair->divider_ = new WindowNode(property0);

    // define status_
    windowPair->status_ = WindowPairStatus::STATUS_PAIRED_DONE;

    windowPair->UpdateWindowPairStatus();
    ASSERT_EQ(WindowPairStatus::STATUS_EMPTY, windowPair->status_);
    ASSERT_EQ(nullptr, windowPair->primary_);
    ASSERT_EQ(nullptr, windowPair->secondary_);
    ASSERT_EQ(nullptr, windowPair->divider_);
}

/**
 * @tc.name: UpdateWindowPairStatus
 * @tc.desc: Update pair status
 * @tc.type: FUNC
 */
HWTEST_F(WindowPairTest, UpdateWindowPairStatus04, Function | SmallTest | Level2)
{
    sptr<WindowPair> windowPair = new WindowPair(0);
    sptr<WindowProperty> property0 = new WindowProperty();

    // define primary_, secondary_, divider_
    windowPair->primary_ = nullptr;
    windowPair->secondary_ = new WindowNode(property0);
    windowPair->divider_ = new WindowNode(property0);

    // define status_
    windowPair->status_ = WindowPairStatus::STATUS_EMPTY;

    sptr<WindowNode> node1 = windowPair->secondary_;
    sptr<WindowNode> node2 = windowPair->divider_;
    windowPair->UpdateWindowPairStatus();
    ASSERT_EQ(WindowPairStatus::STATUS_SINGLE_SECONDARY, windowPair->status_);
    ASSERT_EQ(nullptr, windowPair->primary_);
    ASSERT_EQ(node1, windowPair->secondary_);
    ASSERT_EQ(node2, windowPair->divider_);
}

/**
 * @tc.name: UpdateWindowPairStatus
 * @tc.desc: Update pair status
 * @tc.type: FUNC
 */
HWTEST_F(WindowPairTest, UpdateWindowPairStatus05, Function | SmallTest | Level2)
{
    sptr<WindowPair> windowPair = new WindowPair(0);
    sptr<WindowProperty> property0 = new WindowProperty();

    // define primary_, secondary_, divider_
    windowPair->primary_ = nullptr;
    windowPair->secondary_ = nullptr;
    windowPair->divider_ = new WindowNode(property0);

    // define status_
    windowPair->status_ = WindowPairStatus::STATUS_SINGLE_PRIMARY;

    sptr<WindowNode> node1 = windowPair->divider_;
    windowPair->UpdateWindowPairStatus();
    ASSERT_EQ(WindowPairStatus::STATUS_EMPTY, windowPair->status_);
    ASSERT_EQ(nullptr, windowPair->primary_);
    ASSERT_EQ(nullptr, windowPair->secondary_);
    ASSERT_EQ(node1, windowPair->divider_);
}

/**
 * @tc.name: SwitchPosition
 * @tc.desc: Switch the position of two paired window.
 * @tc.type: FUNC
 */
HWTEST_F(WindowPairTest, SwitchPosition01, Function | SmallTest | Level2)
{
    sptr<WindowPair> windowPair = new WindowPair(0);
    sptr<WindowProperty> property0 = new WindowProperty();

    // define primary_, secondary_, divider_
    windowPair->primary_ = nullptr;
    windowPair->secondary_ = nullptr;
    windowPair->divider_ = new WindowNode(property0);

    sptr<WindowNode> node1 = windowPair->divider_;
    ASSERT_EQ(nullptr, windowPair->primary_);
    ASSERT_EQ(nullptr, windowPair->secondary_);
    ASSERT_EQ(node1, windowPair->divider_);
}

/**
 * @tc.name: SwitchPosition
 * @tc.desc: Switch the position of two paired window.
 * @tc.type: FUNC
 */
HWTEST_F(WindowPairTest, SwitchPosition02, Function | SmallTest | Level2)
{
    sptr<WindowPair> windowPair = new WindowPair(0);
    sptr<WindowProperty> property0 = new WindowProperty();
    property0->SetWindowMode(WindowMode::WINDOW_MODE_SPLIT_PRIMARY);
    sptr<WindowProperty> property1 = new WindowProperty();
    property1->SetWindowMode(WindowMode::WINDOW_MODE_SPLIT_PRIMARY);

    // define primary_, secondary_
    windowPair->primary_ = new WindowNode(property0);
    windowPair->secondary_ = new WindowNode(property1);

    windowPair->SwitchPosition();
    ASSERT_EQ(WindowMode::WINDOW_MODE_SPLIT_PRIMARY, windowPair->primary_->GetWindowMode());
    ASSERT_EQ(WindowMode::WINDOW_MODE_SPLIT_SECONDARY, windowPair->secondary_->GetWindowMode());
}

/**
 * @tc.name: SwitchPosition
 * @tc.desc: Switch the position of two paired window.
 * @tc.type: FUNC
 */
HWTEST_F(WindowPairTest, SwitchPosition03, Function | SmallTest | Level2)
{
    sptr<WindowPair> windowPair = new WindowPair(0);
    sptr<WindowProperty> property0 = new WindowProperty();
    property0->SetWindowMode(WindowMode::WINDOW_MODE_SPLIT_SECONDARY);
    sptr<WindowProperty> property1 = new WindowProperty();
    property1->SetWindowMode(WindowMode::WINDOW_MODE_SPLIT_SECONDARY);

    // define primary_, secondary_
    windowPair->primary_ = new WindowNode(property0);
    windowPair->secondary_ = new WindowNode(property1);

    windowPair->SwitchPosition();
    ASSERT_EQ(WindowMode::WINDOW_MODE_SPLIT_PRIMARY, windowPair->primary_->GetWindowMode());
    ASSERT_EQ(WindowMode::WINDOW_MODE_SPLIT_SECONDARY, windowPair->secondary_->GetWindowMode());
}

/**
 * @tc.name: HandlePairedNodesChange
 * @tc.desc: Update paired window node
 * @tc.type: FUNC
 */
HWTEST_F(WindowPairTest, HandlePairedNodesChange01, Function | SmallTest | Level2)
{
    sptr<WindowPair> windowPair = new WindowPair(0);
    sptr<WindowProperty> property0 = new WindowProperty();
    property0->SetWindowMode(WindowMode::WINDOW_MODE_FULLSCREEN);
    sptr<WindowProperty> property1 = new WindowProperty();
    property1->SetWindowMode(WindowMode::WINDOW_MODE_SPLIT_PRIMARY);

    // define primary_, secondary_
    windowPair->primary_ = new WindowNode(property0);
    windowPair->secondary_ = new WindowNode(property0);

    windowPair->HandlePairedNodesChange();
    ASSERT_EQ(nullptr, windowPair->primary_);
    ASSERT_EQ(nullptr, windowPair->secondary_);
    ASSERT_EQ(WindowPairStatus::STATUS_EMPTY, windowPair->status_);
}

/**
 * @tc.name: HandlePairedNodesChange
 * @tc.desc: Update paired window node
 * @tc.type: FUNC
 */
HWTEST_F(WindowPairTest, HandlePairedNodesChange02, Function | SmallTest | Level2)
{
    sptr<WindowPair> windowPair = new WindowPair(0);
    sptr<WindowProperty> property0 = new WindowProperty();
    property0->SetWindowMode(WindowMode::WINDOW_MODE_FULLSCREEN);
    sptr<WindowProperty> property1 = new WindowProperty();
    property1->SetWindowMode(WindowMode::WINDOW_MODE_SPLIT_PRIMARY);

    // define primary_, secondary_
    windowPair->primary_ = new WindowNode(property0);
    windowPair->secondary_ = new WindowNode(property1);

    sptr<WindowNode> tmp_node = windowPair->secondary_;
    windowPair->HandlePairedNodesChange();
    ASSERT_EQ(tmp_node, windowPair->primary_);
    ASSERT_EQ(nullptr, windowPair->secondary_);
    ASSERT_EQ(WindowPairStatus::STATUS_SINGLE_PRIMARY, windowPair->status_);
}

/**
 * @tc.name: HandlePairedNodesChange
 * @tc.desc: Update paired window node
 * @tc.type: FUNC
 */
HWTEST_F(WindowPairTest, HandlePairedNodesChange03, Function | SmallTest | Level2)
{
    sptr<WindowPair> windowPair = new WindowPair(0);
    sptr<WindowProperty> property0 = new WindowProperty();
    property0->SetWindowMode(WindowMode::WINDOW_MODE_FULLSCREEN);

    // define primary_, secondary_
    windowPair->primary_ = nullptr;
    windowPair->secondary_ = new WindowNode(property0);

    windowPair->HandlePairedNodesChange();
    ASSERT_EQ(nullptr, windowPair->primary_);
    ASSERT_EQ(nullptr, windowPair->secondary_);
    ASSERT_EQ(WindowPairStatus::STATUS_EMPTY, windowPair->status_);
}

/**
 * @tc.name: HandlePairedNodesChange
 * @tc.desc: Update paired window node
 * @tc.type: FUNC
 */
HWTEST_F(WindowPairTest, HandlePairedNodesChange04, Function | SmallTest | Level2)
{
    sptr<WindowPair> windowPair = new WindowPair(0);
    sptr<WindowProperty> property0 = new WindowProperty();
    property0->SetWindowMode(WindowMode::WINDOW_MODE_FULLSCREEN);
    sptr<WindowProperty> property1 = new WindowProperty();
    property1->SetWindowMode(WindowMode::WINDOW_MODE_SPLIT_PRIMARY);
    sptr<WindowProperty> property2 = new WindowProperty();
    property2->SetWindowMode(WindowMode::WINDOW_MODE_SPLIT_SECONDARY);

    // define primary_, secondary_
    windowPair->primary_ = new WindowNode(property2);
    windowPair->secondary_ = new WindowNode(property0);

    sptr<WindowNode> tmp_node = windowPair->primary_;
    windowPair->HandlePairedNodesChange();
    ASSERT_EQ(nullptr, windowPair->primary_);
    ASSERT_EQ(tmp_node, windowPair->secondary_);
    ASSERT_EQ(WindowPairStatus::STATUS_SINGLE_SECONDARY, windowPair->status_);
}

/**
 * @tc.name: HandlePairedNodesChange
 * @tc.desc: Update paired window node
 * @tc.type: FUNC
 */
HWTEST_F(WindowPairTest, HandlePairedNodesChange05, Function | SmallTest | Level2)
{
    sptr<WindowPair> windowPair = new WindowPair(0);
    sptr<WindowProperty> property1 = new WindowProperty();
    property1->SetWindowMode(WindowMode::WINDOW_MODE_SPLIT_PRIMARY);
    sptr<WindowProperty> property2 = new WindowProperty();
    property2->SetWindowMode(WindowMode::WINDOW_MODE_SPLIT_PRIMARY);

    // define primary_, secondary_
    windowPair->primary_ = new WindowNode(property1);
    windowPair->secondary_ = new WindowNode(property2);

    windowPair->HandlePairedNodesChange();
    ASSERT_EQ(WindowMode::WINDOW_MODE_SPLIT_PRIMARY, windowPair->primary_->GetWindowMode());
    ASSERT_EQ(WindowMode::WINDOW_MODE_SPLIT_SECONDARY, windowPair->secondary_->GetWindowMode());
}

/**
 * @tc.name: HandleRemoveWindow
 * @tc.desc: Handle removed window
 * @tc.type: FUNC
 */
HWTEST_F(WindowPairTest, HandleRemoveWindow01, Function | SmallTest | Level2)
{
    sptr<WindowPair> windowPair = new WindowPair(0);
    sptr<WindowProperty> property1 = new WindowProperty();
    property1->SetWindowMode(WindowMode::WINDOW_MODE_SPLIT_PRIMARY);
    windowPair->primary_ = new WindowNode(property1);

    sptr<WindowNode> node = nullptr;
    windowPair->HandleRemoveWindow(node);
    ASSERT_EQ(WindowMode::WINDOW_MODE_SPLIT_PRIMARY, windowPair->primary_->GetWindowMode());
}

/**
 * @tc.name: HandleRemoveWindow
 * @tc.desc: Handle removed window
 * @tc.type: FUNC
 */
HWTEST_F(WindowPairTest, HandleRemoveWindow02, Function | SmallTest | Level2)
{
    sptr<WindowPair> windowPair = new WindowPair(0);

    sptr<WindowProperty> property1 = new WindowProperty();
    property1->SetWindowMode(WindowMode::WINDOW_MODE_SPLIT_PRIMARY);

    sptr<WindowNode> node1 = new WindowNode(property1);
    sptr<WindowNode> node2 = new WindowNode(property1);

    // define primary_, secondary_, status_
    windowPair->primary_ = nullptr;
    windowPair->secondary_ = nullptr;
    windowPair->divider_ = nullptr;
    windowPair->status_ = WindowPairStatus::STATUS_PAIRING;

    IWindowMocker* w = new IWindowMocker;
    sptr<IWindow> window(w);
    node1->SetWindowToken(window);
    EXPECT_CALL(*w, UpdateWindowMode(_)).Times(1).WillOnce(Return(WMError::WM_OK));
    windowPair->HandleRemoveWindow(node1);
    ASSERT_EQ(nullptr, windowPair->primary_);
    sptr<IWindow> window1 = nullptr;
    node1->SetWindowToken(window1);
    windowPair->HandleRemoveWindow(node1);
    ASSERT_EQ(nullptr, windowPair->primary_);
}

/**
 * @tc.name: TakePairSnapshot
 * @tc.desc: take pair snapsht
 * @tc.type: FUNC
 */
HWTEST_F(WindowPairTest, TakePairSnapshot01, Function | SmallTest | Level2)
{
    sptr<WindowPair> windowPair = new WindowPair(0);

    sptr<WindowProperty> property1 = new WindowProperty();
    property1->SetWindowMode(WindowMode::WINDOW_MODE_SPLIT_PRIMARY);
    sptr<WindowProperty> property2 = new WindowProperty();
    property1->SetWindowMode(WindowMode::WINDOW_MODE_SPLIT_SECONDARY);

    sptr<WindowNode> node1 = new WindowNode(property1);
    sptr<WindowNode> node2 = new WindowNode(property2);

    // define primary_, secondary_, status_
    windowPair->primary_ = node1;
    windowPair->secondary_ = node2;
    windowPair->status_ = WindowPairStatus::STATUS_PAIRED_DONE;

    ASSERT_EQ(true, windowPair->TakePairSnapshot());
    windowPair->primary_ = nullptr;
    ASSERT_EQ(false, windowPair->TakePairSnapshot());
    windowPair->primary_ = node1;
    windowPair->secondary_ = nullptr;
    ASSERT_EQ(false, windowPair->TakePairSnapshot());
    windowPair->status_ = WindowPairStatus::STATUS_PAIRING;
    ASSERT_EQ(false, windowPair->TakePairSnapshot());
}

/**
 * @tc.name: ClearPairSnapshot
 * @tc.desc: Clear Pair Snapshot
 * @tc.type: FUNC
 */
HWTEST_F(WindowPairTest, ClearPairSnapshot01, Function | SmallTest | Level2)
{
    sptr<WindowPair> windowPair = new WindowPair(0);

    sptr<WindowProperty> property1 = new WindowProperty();
    property1->SetWindowMode(WindowMode::WINDOW_MODE_SPLIT_PRIMARY);
    sptr<WindowProperty> property2 = new WindowProperty();
    property1->SetWindowMode(WindowMode::WINDOW_MODE_SPLIT_SECONDARY);

    sptr<WindowNode> node1 = new WindowNode(property1);
    sptr<WindowNode> node2 = new WindowNode(property2);

    // define primary_, secondary_,
    windowPair->primary_ = node1;
    windowPair->secondary_ = node2;

    windowPair->ClearPairSnapshot();
    ASSERT_EQ(nullptr, windowPair->primary_->snapshot_);
    ASSERT_EQ(nullptr, windowPair->secondary_->snapshot_);

    windowPair->primary_ = nullptr;
    ASSERT_EQ(false, windowPair->TakePairSnapshot());
    windowPair->primary_ = node1;
    windowPair->secondary_ = nullptr;
    ASSERT_EQ(false, windowPair->TakePairSnapshot());
    windowPair->status_ = WindowPairStatus::STATUS_PAIRING;
    ASSERT_EQ(false, windowPair->TakePairSnapshot());
}
/**
 * @tc.name: ExitSplitMode
 * @tc.desc: Exit Split Mode
 * @tc.type: FUNC
 */
HWTEST_F(WindowPairTest, ExitSplitMode01, Function | SmallTest | Level2)
{
    sptr<WindowPair> windowPair = new WindowPair(0);
    sptr<WindowProperty> property1 = new WindowProperty();
    property1->SetWindowMode(WindowMode::WINDOW_MODE_SPLIT_PRIMARY);
    sptr<WindowProperty> property2 = new WindowProperty();
    property2->SetWindowMode(WindowMode::WINDOW_MODE_SPLIT_SECONDARY);

    sptr<WindowNode> node1 = new WindowNode(property1);
    windowPair->primary_ = node1;
    sptr<WindowNode> node2 = new WindowNode(property2);
    windowPair->secondary_ = node2;
    windowPair->divider_ = nullptr;
    windowPair->ExitSplitMode();
    ASSERT_EQ(nullptr, windowPair->divider_);
}
/**
 * @tc.name: ExitSplitMode
 * @tc.desc: Exit Split Mode
 * @tc.type: FUNC
 */
HWTEST_F(WindowPairTest, ExitSplitMode02, Function | SmallTest | Level2)
{
    sptr<WindowPair> windowPair = new WindowPair(0);
    sptr<WindowProperty> property1 = new WindowProperty();
    property1->SetWindowMode(WindowMode::WINDOW_MODE_SPLIT_PRIMARY);
    sptr<WindowProperty> property2 = new WindowProperty();
    property2->SetWindowMode(WindowMode::WINDOW_MODE_SPLIT_SECONDARY);
    sptr<WindowProperty> property3 = new WindowProperty();
    property2->SetWindowMode(WindowMode::WINDOW_MODE_FLOATING);

    sptr<WindowNode> node1 = new WindowNode(property1);
    windowPair->primary_ = node1;
    sptr<WindowNode> node2 = new WindowNode(property2);
    windowPair->secondary_ = node2;
    sptr<WindowNode> node3 = new WindowNode(property3);
    const Rect divider_rect2 = {0, 20, 100, 1};
    const Rect primary_rect1 = {0, 0, 20, 20};
    const Rect secondary_rect1 = {0, 20, 50, 70};
    const Rect secondary_rect2 = {0, 20, 10, 10};
    node3->SetWindowRect(divider_rect2);
    node1->SetWindowRect(primary_rect1);
    node2->SetWindowRect(secondary_rect1);
    windowPair->divider_ = node3;
    windowPair->primary_ = node1;
    windowPair->secondary_ = node2;
    windowPair->ExitSplitMode();
    std::vector<wptr<WindowNode>> vec1 = MinimizeApp::needMinimizeAppNodes_[MinimizeReason::SPLIT_QUIT];
    ASSERT_EQ(0, vec1.size());
    node2->SetWindowRect(secondary_rect2);
    windowPair->secondary_ = node2;
    windowPair->ExitSplitMode();
    std::vector<wptr<WindowNode>> vec2 = MinimizeApp::needMinimizeAppNodes_[MinimizeReason::SPLIT_QUIT];
    ASSERT_EQ(0, vec2.size());
    windowPair->Clear();
}
/**
 * @tc.name: ExitSplitMode
 * @tc.desc: Exit Split Mode
 * @tc.type: FUNC
 */
HWTEST_F(WindowPairTest, ExitSplitMode03, Function | SmallTest | Level2)
{
    sptr<WindowPair> windowPair = new WindowPair(0);
    sptr<WindowProperty> property1 = new WindowProperty();
    property1->SetWindowMode(WindowMode::WINDOW_MODE_SPLIT_PRIMARY);
    sptr<WindowProperty> property2 = new WindowProperty();
    property2->SetWindowMode(WindowMode::WINDOW_MODE_SPLIT_SECONDARY);
    sptr<WindowProperty> property3 = new WindowProperty();
    property2->SetWindowMode(WindowMode::WINDOW_MODE_FLOATING);

    sptr<WindowNode> node1 = new WindowNode(property1);
    windowPair->primary_ = node1;
    sptr<WindowNode> node2 = new WindowNode(property2);
    windowPair->secondary_ = node2;
    sptr<WindowNode> node3 = new WindowNode(property3);
    const Rect divider_rect1 = {20, 0, 1, 100};
    const Rect primary_rect1 = {0, 0, 20, 20};
    const Rect secondary_rect1 = {0, 20, 50, 70};
    const Rect secondary_rect2 = {0, 20, 10, 20};
    node3->SetWindowRect(divider_rect1); // is_vertical false
    node2->SetWindowRect(secondary_rect1);
    node1->SetWindowRect(primary_rect1);
    windowPair->divider_ = node3;
    windowPair->primary_ = node1;
    windowPair->secondary_ = node2;
    windowPair->ExitSplitMode();
    std::vector<wptr<WindowNode>> vec1 = MinimizeApp::needMinimizeAppNodes_[MinimizeReason::SPLIT_QUIT];
    ASSERT_EQ(0, vec1.size());
    node2->SetWindowRect(secondary_rect2);
    windowPair->secondary_ = node2;
    std::vector<wptr<WindowNode>> vec2 = MinimizeApp::needMinimizeAppNodes_[MinimizeReason::SPLIT_QUIT];
    ASSERT_EQ(0, vec2.size());
    windowPair->Clear();
}
}
}
}
