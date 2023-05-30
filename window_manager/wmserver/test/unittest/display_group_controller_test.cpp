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
#include <transaction/rs_transaction.h>
#include "iremote_object_mocker.h"
#include "display_group_controller.h"
#include "display_manager.h"
#include "window_helper.h"
#include "window_node_container.h"

using namespace testing;
using namespace testing::ext;

namespace OHOS {
namespace Rosen {
namespace {
    const Rect DEFAULT_RECT = {0, 0, 200, 200};
}
class DisplayGroupControllerTest : public testing::Test {
public:
    static void SetUpTestCase();
    static void TearDownTestCase();
    void SetUp() override;
    void TearDown() override;
    sptr<WindowProperty> CreateWindowProperty(uint32_t windowId,
        WindowType type = WindowType::WINDOW_TYPE_APP_MAIN_WINDOW);
    static void SetDisplayGroupInfo(DisplayId displayId, Rect displayRect);
private:
    static sptr<WindowNodeContainer> container_;
    static sptr<DisplayGroupInfo> displayGroupInfo_;
    static sptr<DisplayGroupController> displayGroupController_;
};

sptr<WindowNodeContainer> DisplayGroupControllerTest::container_ = nullptr;
sptr<DisplayGroupInfo> DisplayGroupControllerTest::displayGroupInfo_ = nullptr;
sptr<DisplayGroupController> DisplayGroupControllerTest::displayGroupController_ = nullptr;

void DisplayGroupControllerTest::SetUpTestCase()
{
    auto display = DisplayManager::GetInstance().GetDefaultDisplay();
    ASSERT_TRUE((display != nullptr));
    container_ = new WindowNodeContainer(display->GetDisplayInfo(), display->GetScreenId());
    displayGroupController_ = container_->displayGroupController_;
    displayGroupInfo_ = displayGroupController_->displayGroupInfo_;
}

void DisplayGroupControllerTest::TearDownTestCase()
{
    container_ = nullptr;
    displayGroupInfo_ = nullptr;
    displayGroupController_ = nullptr;
}

void DisplayGroupControllerTest::SetUp()
{
    DisplayId defaultId = 0;
    displayGroupController_->displayGroupWindowTree_.clear();
    displayGroupController_->InitNewDisplay(defaultId);
    SetDisplayGroupInfo(0, DEFAULT_RECT);
}

void DisplayGroupControllerTest::TearDown()
{
    displayGroupController_->defaultDisplayId_ = 0;
    container_->GetLayoutPolicy()->isMultiDisplay_ = false;
    displayGroupController_->ClearMapOfDestroyedDisplay(0);
    displayGroupController_->ClearMapOfDestroyedDisplay(1);
}

sptr<WindowProperty> DisplayGroupControllerTest::CreateWindowProperty(uint32_t windowId, WindowType type)
{
    sptr<WindowProperty> property = new WindowProperty();
    property->SetWindowId(windowId);
    property->SetWindowType(type);
    return property;
}

void DisplayGroupControllerTest::SetDisplayGroupInfo(DisplayId displayId, Rect displayRect)
{
    sptr<DisplayInfo> displayInfo = new DisplayInfo();
    displayInfo->SetDisplayId(displayId);
    displayInfo->SetOffsetX(displayRect.posX_);
    displayInfo->SetOffsetY(displayRect.posY_);
    displayInfo->SetWidth(displayRect.width_);
    displayInfo->SetHeight(displayRect.height_);
    displayGroupInfo_->displayInfosMap_[displayId] = displayInfo;
    displayGroupInfo_->leftDisplayId_ = 0;
    displayGroupInfo_->rightDisplayId_ = 1;
}

namespace {
/**
 * @tc.name: GetWindowNodesByDisplayIdAndRootType01
 * @tc.desc: Use displayId which not exists
 * @tc.type: FUNC
 */
HWTEST_F(DisplayGroupControllerTest, GetWindowNodesByDisplayIdAndRootType01, Function | SmallTest | Level2)
{
    DisplayId testId = 100; // 100 test display id
    std::vector<sptr<WindowNode>>* rootNodeVectorPtr =
        displayGroupController_->GetWindowNodesByDisplayIdAndRootType(testId, WindowRootNodeType::APP_WINDOW_NODE);
    ASSERT_EQ(nullptr, rootNodeVectorPtr);
}

/**
 * @tc.name: GetWindowNodesByDisplayIdAndRootType02
 * @tc.desc: Use WindowRootNodeType which not exists
 * @tc.type: FUNC
 */
HWTEST_F(DisplayGroupControllerTest, GetWindowNodesByDisplayIdAndRootType02, Function | SmallTest | Level2)
{
    WindowRootNodeType rootType = static_cast<WindowRootNodeType>(100);
    std::vector<sptr<WindowNode>>* rootNodeVectorPtr =
        displayGroupController_->GetWindowNodesByDisplayIdAndRootType(0, rootType);
    ASSERT_EQ(nullptr, rootNodeVectorPtr);
}

/**
 * @tc.name: AddWindowNodeOnWindowTree01
 * @tc.desc: Use WindowRootNodeType which not exists
 * @tc.type: FUNC
 */
HWTEST_F(DisplayGroupControllerTest, AddWindowNodeOnWindowTree01, Function | SmallTest | Level2)
{
    sptr<WindowNode> node1 = new WindowNode(CreateWindowProperty(100));
    ASSERT_NE(nullptr, node1);
    WindowRootNodeType rootType = static_cast<WindowRootNodeType>(100);
    displayGroupController_->AddWindowNodeOnWindowTree(node1, rootType);
}

/**
 * @tc.name: UpdateDisplayGroupWindowTree01
 * @tc.desc: Use appWindowNode with nullptr
 * @tc.type: FUNC
 */
HWTEST_F(DisplayGroupControllerTest, UpdateDisplayGroupWindowTree01, Function | SmallTest | Level2)
{
    auto originRootNode = container_->GetRootNode(WindowRootNodeType::APP_WINDOW_NODE);
    ASSERT_NE(nullptr, originRootNode);
    container_->appWindowNode_ = nullptr;
    ASSERT_EQ(nullptr, container_->GetRootNode(WindowRootNodeType::APP_WINDOW_NODE));
    displayGroupController_->UpdateDisplayGroupWindowTree();
    container_->appWindowNode_ = originRootNode;
    displayGroupController_->UpdateDisplayGroupWindowTree();
}

/**
 * @tc.name: ProcessCrossNodes01
 * @tc.desc: IsShowingOnMultiDisplays_ is true
 * @tc.type: FUNC
 */
HWTEST_F(DisplayGroupControllerTest, ProcessCrossNodes01, Function | SmallTest | Level2)
{
    sptr<WindowNode> node1 = new WindowNode(CreateWindowProperty(100));
    ASSERT_NE(nullptr, node1);
    sptr<WindowNode> child = new WindowNode(CreateWindowProperty(101));
    ASSERT_NE(nullptr, child);
    node1->children_.push_back(child);
    node1->isShowingOnMultiDisplays_ = true;
    std::vector<sptr<WindowNode>>* rootApp = displayGroupController_->GetWindowNodesByDisplayIdAndRootType(
        0, WindowRootNodeType::APP_WINDOW_NODE);
    rootApp->push_back(node1);
    displayGroupController_->ProcessCrossNodes(1, DisplayStateChangeType::CREATE);
}

/**
 * @tc.name: ProcessCrossNodes02
 * @tc.desc: IsShowingOnMultiDisplays_ is true with different DisplayStateChangeType
 * @tc.type: FUNC
 */
HWTEST_F(DisplayGroupControllerTest, ProcessCrossNodes02, Function | SmallTest | Level2)
{
    sptr<WindowNode> node1 = new WindowNode(CreateWindowProperty(100));
    ASSERT_NE(nullptr, node1);
    sptr<WindowNode> child = new WindowNode(CreateWindowProperty(101));
    ASSERT_NE(nullptr, child);
    node1->children_.push_back(child);
    node1->isShowingOnMultiDisplays_ = true;
    std::vector<sptr<WindowNode>>* rootApp = displayGroupController_->GetWindowNodesByDisplayIdAndRootType(
        0, WindowRootNodeType::APP_WINDOW_NODE);
    rootApp->push_back(node1);
    displayGroupController_->ProcessCrossNodes(1, DisplayStateChangeType::SIZE_CHANGE);
}

/**
 * @tc.name: ProcessCrossNodes03
 * @tc.desc: IsShowingOnMultiDisplays_ is true with different DisplayStateChangeType
 * @tc.type: FUNC
 */
HWTEST_F(DisplayGroupControllerTest, ProcessCrossNodes03, Function | SmallTest | Level2)
{
    sptr<WindowNode> node1 = new WindowNode(CreateWindowProperty(100));
    ASSERT_NE(nullptr, node1);
    sptr<WindowNode> child = new WindowNode(CreateWindowProperty(101));
    ASSERT_NE(nullptr, child);
    node1->children_.push_back(child);
    node1->isShowingOnMultiDisplays_ = true;
    std::vector<sptr<WindowNode>>* rootApp = displayGroupController_->GetWindowNodesByDisplayIdAndRootType(
        0, WindowRootNodeType::APP_WINDOW_NODE);
    rootApp->push_back(node1);
    displayGroupController_->ProcessCrossNodes(1, DisplayStateChangeType::UPDATE_ROTATION);
}

/**
 * @tc.name: ProcessCrossNodes04
 * @tc.desc: IsShowingOnMultiDisplays_ is true with multi showing display
 * @tc.type: FUNC
 */
HWTEST_F(DisplayGroupControllerTest, ProcessCrossNodes04, Function | SmallTest | Level2)
{
    sptr<WindowNode> node1 = new WindowNode(CreateWindowProperty(100));
    ASSERT_NE(nullptr, node1);
    sptr<WindowNode> child = new WindowNode(CreateWindowProperty(101));
    ASSERT_NE(nullptr, child);
    node1->children_.push_back(child);
    node1->isShowingOnMultiDisplays_ = true;
    node1->SetShowingDisplays({0, 1});
    std::vector<sptr<WindowNode>>* rootApp = displayGroupController_->GetWindowNodesByDisplayIdAndRootType(
        0, WindowRootNodeType::APP_WINDOW_NODE);
    rootApp->push_back(node1);
    displayGroupController_->ProcessCrossNodes(1, DisplayStateChangeType::DISPLAY_COMPRESS);
}

/**
 * @tc.name: UpdateWindowShowingDisplays01
 * @tc.desc: Show only on left display
 * @tc.type: FUNC
 */
HWTEST_F(DisplayGroupControllerTest, UpdateWindowShowingDisplays01, Function | SmallTest | Level2)
{
    sptr<WindowNode> node1 = new WindowNode(CreateWindowProperty(100));
    ASSERT_NE(nullptr, node1);
    sptr<WindowNode> child = new WindowNode(CreateWindowProperty(101));
    ASSERT_NE(nullptr, child);
    node1->children_.push_back(child);
    node1->SetWindowRect({0, 0, 50, 50});
    displayGroupController_->UpdateWindowShowingDisplays(node1);
    ASSERT_EQ(1, node1->GetShowingDisplays().size());
}

/**
 * @tc.name: UpdateWindowShowingDisplays02
 * @tc.desc: Not show on any display
 * @tc.type: FUNC
 */
HWTEST_F(DisplayGroupControllerTest, UpdateWindowShowingDisplays02, Function | SmallTest | Level2)
{
    sptr<WindowNode> node1 = new WindowNode(CreateWindowProperty(100));
    ASSERT_NE(nullptr, node1);
    sptr<WindowNode> child = new WindowNode(CreateWindowProperty(101));
    ASSERT_NE(nullptr, child);
    node1->children_.push_back(child);
    node1->SetWindowRect({0, 0, 0, 0});
    SetDisplayGroupInfo(0, {0, 0, 0, 0});
    displayGroupController_->InitNewDisplay(1);
    SetDisplayGroupInfo(1, {0, 0, 0, 0});
    displayGroupController_->UpdateWindowShowingDisplays(node1);
}

/**
 * @tc.name: UpdateWindowShowingDisplays03
 * @tc.desc: Show only on right display
 * @tc.type: FUNC
 */
HWTEST_F(DisplayGroupControllerTest, UpdateWindowShowingDisplays03, Function | SmallTest | Level2)
{
    sptr<WindowNode> node1 = new WindowNode(CreateWindowProperty(100));
    ASSERT_NE(nullptr, node1);
    sptr<WindowNode> child = new WindowNode(CreateWindowProperty(101));
    ASSERT_NE(nullptr, child);
    node1->children_.push_back(child);
    node1->SetWindowRect({100, 100, 50, 50});
    SetDisplayGroupInfo(0, {0, 0, 50, 50});
    displayGroupController_->InitNewDisplay(1);
    SetDisplayGroupInfo(1, {50, 50, 100, 100});
    displayGroupController_->UpdateWindowShowingDisplays(node1);
    ASSERT_EQ(1, node1->GetShowingDisplays().size());
}

/**
 * @tc.name: UpdateWindowShowingDisplays04
 * @tc.desc: Show on multi display
 * @tc.type: FUNC
 */
HWTEST_F(DisplayGroupControllerTest, UpdateWindowShowingDisplays04, Function | SmallTest | Level2)
{
    sptr<WindowNode> node1 = new WindowNode(CreateWindowProperty(100));
    ASSERT_NE(nullptr, node1);
    sptr<WindowNode> child = new WindowNode(CreateWindowProperty(101));
    ASSERT_NE(nullptr, child);
    node1->children_.push_back(child);
    node1->SetWindowRect({50, 50, 60, 60}); // 110 > 0 && 50 < 100
    displayGroupController_->InitNewDisplay(0);
    SetDisplayGroupInfo(0, {0, 0, 100, 100});
    displayGroupController_->InitNewDisplay(1);
    SetDisplayGroupInfo(1, {100, 100, 200, 200});
    displayGroupController_->UpdateWindowShowingDisplays(node1);
    ASSERT_EQ(2, node1->GetShowingDisplays().size());
}

/**
 * @tc.name: UpdateWindowDisplayIdIfNeeded01
 * @tc.desc: Not show on any display
 * @tc.type: FUNC
 */
HWTEST_F(DisplayGroupControllerTest, UpdateWindowDisplayIdIfNeeded01, Function | SmallTest | Level2)
{
    sptr<WindowNode> node1 = new WindowNode(CreateWindowProperty(100));
    ASSERT_NE(nullptr, node1);
    node1->SetWindowRect({50, 50, 60, 60});
    displayGroupController_->InitNewDisplay(0);
    SetDisplayGroupInfo(0, {0, 0, 100, 100});
    displayGroupController_->InitNewDisplay(1);
    SetDisplayGroupInfo(1, {100, 100, 200, 200});
    displayGroupController_->UpdateWindowDisplayIdIfNeeded(node1);
    ASSERT_EQ(0, node1->GetShowingDisplays().size());
}

/**
 * @tc.name: UpdateWindowDisplayIdIfNeeded02
 * @tc.desc: Show on left display
 * @tc.type: FUNC
 */
HWTEST_F(DisplayGroupControllerTest, UpdateWindowDisplayIdIfNeeded02, Function | SmallTest | Level2)
{
    sptr<WindowNode> node1 = new WindowNode(CreateWindowProperty(100));
    ASSERT_NE(nullptr, node1);
    node1->SetShowingDisplays({0});
    node1->SetWindowRect({50, 50, 60, 60});
    displayGroupController_->InitNewDisplay(0);
    SetDisplayGroupInfo(0, {0, 0, 100, 100});
    displayGroupController_->InitNewDisplay(1);
    SetDisplayGroupInfo(1, {100, 100, 200, 200});
    displayGroupController_->UpdateWindowDisplayIdIfNeeded(node1);
    ASSERT_EQ(0, node1->GetDisplayId());
}

/**
 * @tc.name: UpdateWindowDisplayIdIfNeeded03
 * @tc.desc: Window covers whole display region
 * @tc.type: FUNC
 */
HWTEST_F(DisplayGroupControllerTest, UpdateWindowDisplayIdIfNeeded03, Function | SmallTest | Level2)
{
    sptr<WindowNode> node1 = new WindowNode(CreateWindowProperty(100));
    ASSERT_NE(nullptr, node1);
    node1->SetShowingDisplays({0, 1});
    node1->SetWindowRect({-50, -50, 200, 200});
    displayGroupController_->InitNewDisplay(0);
    SetDisplayGroupInfo(0, {0, 0, 100, 100});
    displayGroupController_->InitNewDisplay(1);
    SetDisplayGroupInfo(1, {200, 200, 200, 200});
    displayGroupController_->UpdateWindowDisplayIdIfNeeded(node1);
    ASSERT_EQ(0, node1->GetDisplayId());
}

/**
 * @tc.name: UpdateWindowDisplayIdIfNeeded04
 * @tc.desc: Current display is default display
 * @tc.type: FUNC
 */
HWTEST_F(DisplayGroupControllerTest, UpdateWindowDisplayIdIfNeeded04, Function | SmallTest | Level2)
{
    sptr<WindowNode> node1 = new WindowNode(CreateWindowProperty(100));
    ASSERT_NE(nullptr, node1);
    node1->SetShowingDisplays({0, 1});
    node1->SetWindowRect({50, 50, 100, 100});
    displayGroupController_->InitNewDisplay(0);
    SetDisplayGroupInfo(0, {0, 0, 100, 100});
    displayGroupController_->InitNewDisplay(1);
    SetDisplayGroupInfo(1, {200, 200, 200, 200});
    displayGroupController_->UpdateWindowDisplayIdIfNeeded(node1);
    ASSERT_EQ(0, node1->GetDisplayId());
}

/**
 * @tc.name: UpdateWindowDisplayIdIfNeeded05
 * @tc.desc: Current display is expand display
 * @tc.type: FUNC
 */
HWTEST_F(DisplayGroupControllerTest, UpdateWindowDisplayIdIfNeeded05, Function | SmallTest | Level2)
{
    sptr<WindowNode> node1 = new WindowNode(CreateWindowProperty(100));
    ASSERT_NE(nullptr, node1);
    node1->SetShowingDisplays({0, 1});
    node1->SetWindowRect({60, 60, 100, 100});
    SetDisplayGroupInfo(0, {0, 0, 100, 100});
    displayGroupController_->InitNewDisplay(1);
    SetDisplayGroupInfo(1, {200, 200, 200, 200});
    displayGroupController_->UpdateWindowDisplayIdIfNeeded(node1);
    ASSERT_EQ(0, node1->GetDisplayId());
}

/**
 * @tc.name: UpdateWindowDisplayIdIfNeeded06
 * @tc.desc: Current display is expand display
 * @tc.type: FUNC
 */
HWTEST_F(DisplayGroupControllerTest, UpdateWindowDisplayIdIfNeeded06, Function | SmallTest | Level2)
{
    sptr<WindowNode> node1 = new WindowNode(CreateWindowProperty(100));
    ASSERT_NE(nullptr, node1);
    node1->SetShowingDisplays({0, 1});
    node1->SetWindowRect({60, 60, 120, 120});
    SetDisplayGroupInfo(0, {0, 0, 70, 70});
    displayGroupController_->InitNewDisplay(1);
    SetDisplayGroupInfo(1, {70, 70, 200, 200});
    displayGroupController_->UpdateWindowDisplayIdIfNeeded(node1);
    ASSERT_EQ(1, node1->GetDisplayId());
}

/**
 * @tc.name: ChangeToRectInDisplayGroup01
 * @tc.desc: Change to rect in Display Group
 * @tc.type: FUNC
 */
HWTEST_F(DisplayGroupControllerTest, ChangeToRectInDisplayGroup01, Function | SmallTest | Level2)
{
    sptr<WindowNode> node1 = new WindowNode(CreateWindowProperty(100));
    ASSERT_NE(nullptr, node1);
    displayGroupController_->ChangeToRectInDisplayGroup(node1, 0);
    Rect actualRect = node1->GetRequestRect();
    Rect expectRect = {DEFAULT_RECT.posX_, DEFAULT_RECT.posY_, 0, 0};
    ASSERT_EQ(expectRect, actualRect);
}

/**
 * @tc.name: PreProcessWindowNode01
 * @tc.desc: PreProcessWindowNode with child
 * @tc.type: FUNC
 */
HWTEST_F(DisplayGroupControllerTest, PreProcessWindowNode01, Function | SmallTest | Level2)
{
    sptr<WindowNode> node1 = new WindowNode(CreateWindowProperty(100));
    ASSERT_NE(nullptr, node1);
    sptr<WindowNode> child = new WindowNode(CreateWindowProperty(101));
    ASSERT_NE(nullptr, child);
    node1->children_.push_back(child);
    displayGroupController_->PreProcessWindowNode(node1, WindowUpdateType::WINDOW_UPDATE_ADDED);
    Rect actualRect = node1->GetRequestRect();
    Rect expectRect = {DEFAULT_RECT.posX_, DEFAULT_RECT.posY_, 0, 0};
    ASSERT_EQ(expectRect, actualRect);
}

/**
 * @tc.name: PreProcessWindowNode02
 * @tc.desc: PreProcessWindowNode with WINDOW_UPDATE_ACTIVE, and size change reason undefined
 * @tc.type: FUNC
 */
HWTEST_F(DisplayGroupControllerTest, PreProcessWindowNode02, Function | SmallTest | Level2)
{
    sptr<WindowNode> node1 = new WindowNode(CreateWindowProperty(100));
    ASSERT_NE(nullptr, node1);
    container_->GetLayoutPolicy()->isMultiDisplay_ = true;
    displayGroupController_->PreProcessWindowNode(node1, WindowUpdateType::WINDOW_UPDATE_ACTIVE);
    Rect actualRect = node1->GetRequestRect();
    Rect expectRect = {0, 0, 0, 0};
    ASSERT_EQ(expectRect, actualRect);
}

/**
 * @tc.name: PreProcessWindowNode03
 * @tc.desc: PreProcessWindowNode with WINDOW_UPDATE_ADDED, and isShowingOnMultiDisplays_ is true
 * @tc.type: FUNC
 */
HWTEST_F(DisplayGroupControllerTest, PreProcessWindowNode03, Function | SmallTest | Level2)
{
    sptr<WindowNode> node1 = new WindowNode(CreateWindowProperty(100));
    ASSERT_NE(nullptr, node1);
    node1->isShowingOnMultiDisplays_ = true;
    displayGroupController_->PreProcessWindowNode(node1, WindowUpdateType::WINDOW_UPDATE_ADDED);
    Rect actualRect = node1->GetRequestRect();
    Rect expectRect = {0, 0, 0, 0};
    ASSERT_EQ(expectRect, actualRect);
}

/**
 * @tc.name: PreProcessWindowNode04
 * @tc.desc: PreProcessWindowNode with WINDOW_UPDATE_ACTIVE, and size change reason MOVE
 * @tc.type: FUNC
 */
HWTEST_F(DisplayGroupControllerTest, PreProcessWindowNode04, Function | SmallTest | Level2)
{
    sptr<WindowNode> node1 = new WindowNode(CreateWindowProperty(100));
    ASSERT_NE(nullptr, node1);
    sptr<WindowNode> child = new WindowNode(CreateWindowProperty(101));
    ASSERT_NE(nullptr, child);
    node1->children_.push_back(child);
    node1->SetWindowSizeChangeReason(WindowSizeChangeReason::MOVE);
    displayGroupController_->PreProcessWindowNode(node1, WindowUpdateType::WINDOW_UPDATE_ACTIVE);
    Rect actualRect = node1->GetRequestRect();
    Rect expectRect = {DEFAULT_RECT.posX_, DEFAULT_RECT.posY_, 0, 0};
    ASSERT_EQ(expectRect, actualRect);
}

/**
 * @tc.name: PostProcessWindowNode01
 * @tc.desc: PostProcessWindowNode with multi display is true
 * @tc.type: FUNC
 */
HWTEST_F(DisplayGroupControllerTest, PostProcessWindowNode01, Function | SmallTest | Level2)
{
    sptr<WindowNode> node1 = new WindowNode(CreateWindowProperty(100));
    ASSERT_NE(nullptr, node1);
    container_->GetLayoutPolicy()->isMultiDisplay_ = true;
    ASSERT_EQ(true, container_->GetLayoutPolicy()->IsMultiDisplay());
}

/**
 * @tc.name: UpdateWindowDisplayId01
 * @tc.desc: UpdateWindowDisplayId with windowToken
 * @tc.type: FUNC
 */
HWTEST_F(DisplayGroupControllerTest, UpdateWindowDisplayId01, Function | SmallTest | Level2)
{
    sptr<IRemoteObject> iRemoteObjectMocker = new IRemoteObjectMocker();
    sptr<IWindow> iWindow = iface_cast<IWindow>(iRemoteObjectMocker);
    ASSERT_NE(nullptr, iWindow);
    sptr<WindowNode> node1 = new WindowNode(CreateWindowProperty(100));
    ASSERT_NE(nullptr, node1);
    node1->SetWindowToken(iWindow);
    ASSERT_NE(nullptr, node1->GetWindowToken());
    displayGroupController_->UpdateWindowDisplayId(node1, 1);
    ASSERT_EQ(1, node1->GetDisplayId());
}

/**
 * @tc.name: MoveCrossNodeToTargetDisplay01
 * @tc.desc: TargetDisplayId equals to default displayId
 * @tc.type: FUNC
 */
HWTEST_F(DisplayGroupControllerTest, MoveCrossNodeToTargetDisplay01, Function | SmallTest | Level2)
{
    sptr<WindowNode> node1 = new WindowNode(CreateWindowProperty(100));
    ASSERT_NE(nullptr, node1);
    sptr<WindowNode> child = new WindowNode(CreateWindowProperty(101));
    ASSERT_NE(nullptr, child);
    node1->children_.push_back(child);
    displayGroupController_->MoveCrossNodeToTargetDisplay(node1, 0);
    auto showingDisplays = child->GetShowingDisplays();
    ASSERT_NE(0, showingDisplays.size());
    ASSERT_EQ(0, showingDisplays[0]);
}

/**
 * @tc.name: MoveCrossNodeToTargetDisplay02
 * @tc.desc: No child and targetDisplayId not equals to default displayId
 * @tc.type: FUNC
 */
HWTEST_F(DisplayGroupControllerTest, MoveCrossNodeToTargetDisplay02, Function | SmallTest | Level2)
{
    sptr<WindowNode> node1 = new WindowNode(CreateWindowProperty(100));
    ASSERT_NE(nullptr, node1);
    displayGroupController_->MoveCrossNodeToTargetDisplay(node1, 1);
    ASSERT_EQ(1, node1->GetDisplayId());
}

/**
 * @tc.name: MoveCrossNodeToTargetDisplay03
 * @tc.desc: TargetDisplayId not equals to default displayId
 * @tc.type: FUNC
 */
HWTEST_F(DisplayGroupControllerTest, MoveCrossNodeToTargetDisplay03, Function | SmallTest | Level2)
{
    sptr<WindowNode> node1 = new WindowNode(CreateWindowProperty(100));
    ASSERT_NE(nullptr, node1);
    sptr<WindowNode> child = new WindowNode(CreateWindowProperty(101));
    ASSERT_NE(nullptr, child);
    node1->children_.push_back(child);
    displayGroupController_->MoveCrossNodeToTargetDisplay(node1, 1);
    ASSERT_EQ(1, node1->GetDisplayId());
    ASSERT_EQ(1, child->GetDisplayId());
}

/**
 * @tc.name: MoveNotCrossNodeToDefaultDisplay01
 * @tc.desc: MoveNotCrossNodeToDefaultDisplay with window type pointer without child
 * @tc.type: FUNC
 */
HWTEST_F(DisplayGroupControllerTest, MoveNotCrossNodeToDefaultDisplay01, Function | SmallTest | Level2)
{
    sptr<WindowNode> node1 = new WindowNode(CreateWindowProperty(100, WindowType::WINDOW_TYPE_POINTER));
    ASSERT_NE(nullptr, node1);
    displayGroupController_->MoveNotCrossNodeToDefaultDisplay(node1, 1);
    Rect actualRect = node1->GetRequestRect();
    Rect expectRect = {100, 100, 0, 0};
    ASSERT_EQ(expectRect, actualRect);
}

/**
 * @tc.name: MoveNotCrossNodeToDefaultDisplay02
 * @tc.desc: MoveNotCrossNodeToDefaultDisplay with window type app with child
 * @tc.type: FUNC
 */
HWTEST_F(DisplayGroupControllerTest, MoveNotCrossNodeToDefaultDisplay02, Function | SmallTest | Level2)
{
    sptr<WindowNode> node1 = new WindowNode(CreateWindowProperty(100));
    ASSERT_NE(nullptr, node1);
    sptr<WindowNode> child = new WindowNode(CreateWindowProperty(101));
    ASSERT_NE(nullptr, child);
    node1->children_.push_back(child);
    displayGroupController_->InitNewDisplay(1);
    SetDisplayGroupInfo(1, {200, 200, 200, 200});
    displayGroupController_->MoveNotCrossNodeToDefaultDisplay(node1, 1);
    Rect actualRect = node1->GetRequestRect();
    Rect expectRect = {-200, -200, 0, 0};
    ASSERT_EQ(expectRect, actualRect);
    ASSERT_EQ(expectRect, child->GetRequestRect());
}

/**
 * @tc.name: ProcessNotCrossNodesOnDestroyedDisplay01
 * @tc.desc: DisplayId equals to defaultDisplayId
 * @tc.type: FUNC
 */
HWTEST_F(DisplayGroupControllerTest, ProcessNotCrossNodesOnDestroyedDisplay01, Function | SmallTest | Level2)
{
    std::vector<uint32_t> windowIds;
    displayGroupController_->ProcessNotCrossNodesOnDestroyedDisplay(0, windowIds);
    ASSERT_EQ(0, windowIds.size());
}

/**
 * @tc.name: ProcessNotCrossNodesOnDestroyedDisplay02
 * @tc.desc: DisplayId not equals to defaultDisplayId or node displayId
 * @tc.type: FUNC
 */
HWTEST_F(DisplayGroupControllerTest, ProcessNotCrossNodesOnDestroyedDisplay02, Function | SmallTest | Level2)
{
    displayGroupController_->InitNewDisplay(1);
    SetDisplayGroupInfo(1, {200, 200, 200, 200});
    sptr<WindowNode> node1 = new WindowNode(CreateWindowProperty(100));
    ASSERT_NE(nullptr, node1);
    displayGroupController_->InitNewDisplay(1);
    SetDisplayGroupInfo(1, {200, 200, 200, 200});
    std::vector<sptr<WindowNode>>* rootApp = displayGroupController_->GetWindowNodesByDisplayIdAndRootType(
        1, WindowRootNodeType::APP_WINDOW_NODE);
    rootApp->push_back(node1);
    std::vector<uint32_t> windowIds;
    displayGroupController_->ProcessNotCrossNodesOnDestroyedDisplay(1, windowIds);
    ASSERT_EQ(0, node1->GetDisplayId());
}

/**
 * @tc.name: ProcessNotCrossNodesOnDestroyedDisplay03
 * @tc.desc: DisplayId not equals to defaultDisplayId but equals to node displayId, isShowingOnMultiDisplays_ is true
 * @tc.type: FUNC
 */
HWTEST_F(DisplayGroupControllerTest, ProcessNotCrossNodesOnDestroyedDisplay03, Function | SmallTest | Level2)
{
    displayGroupController_->InitNewDisplay(1);
    SetDisplayGroupInfo(1, {200, 200, 200, 200});
    sptr<WindowNode> node1 = new WindowNode(CreateWindowProperty(100));
    ASSERT_NE(nullptr, node1);
    displayGroupController_->InitNewDisplay(1);
    SetDisplayGroupInfo(1, {200, 200, 200, 200});
    std::vector<sptr<WindowNode>>* rootApp = displayGroupController_->GetWindowNodesByDisplayIdAndRootType(
        1, WindowRootNodeType::APP_WINDOW_NODE);
    rootApp->push_back(node1);
    node1->SetDisplayId(1);
    node1->isShowingOnMultiDisplays_ = true;
    std::vector<uint32_t> windowIds;
    displayGroupController_->ProcessNotCrossNodesOnDestroyedDisplay(1, windowIds);
    ASSERT_EQ(1, node1->GetDisplayId());
}

/**
 * @tc.name: ProcessNotCrossNodesOnDestroyedDisplay04
 * @tc.desc: Node with WINDOW_TYPE_STATUS_BAR
 * @tc.type: FUNC
 */
HWTEST_F(DisplayGroupControllerTest, ProcessNotCrossNodesOnDestroyedDisplay04, Function | SmallTest | Level2)
{
    displayGroupController_->InitNewDisplay(1);
    SetDisplayGroupInfo(1, {200, 200, 200, 200});
    sptr<WindowNode> node1 = new WindowNode(CreateWindowProperty(100, WindowType::WINDOW_TYPE_STATUS_BAR));
    ASSERT_NE(nullptr, node1);
    displayGroupController_->InitNewDisplay(1);
    SetDisplayGroupInfo(1, {200, 200, 200, 200});
    std::vector<sptr<WindowNode>>* rootApp = displayGroupController_->GetWindowNodesByDisplayIdAndRootType(
        1, WindowRootNodeType::ABOVE_WINDOW_NODE);
    rootApp->push_back(node1);
    node1->SetDisplayId(1);
    std::vector<uint32_t> windowIds;
    displayGroupController_->ProcessNotCrossNodesOnDestroyedDisplay(1, windowIds);
    ASSERT_EQ(1, windowIds.size());
    ASSERT_EQ(1, node1->GetDisplayId());
}

/**
 * @tc.name: ProcessNotCrossNodesOnDestroyedDisplay05
 * @tc.desc: Node with WINDOW_TYPE_NAVIGATION_BAR
 * @tc.type: FUNC
 */
HWTEST_F(DisplayGroupControllerTest, ProcessNotCrossNodesOnDestroyedDisplay05, Function | SmallTest | Level2)
{
    displayGroupController_->InitNewDisplay(1);
    SetDisplayGroupInfo(1, {200, 200, 200, 200});
    sptr<WindowNode> node1 = new WindowNode(CreateWindowProperty(100, WindowType::WINDOW_TYPE_NAVIGATION_BAR));
    ASSERT_NE(nullptr, node1);
    displayGroupController_->InitNewDisplay(1);
    SetDisplayGroupInfo(1, {200, 200, 200, 200});
    std::vector<sptr<WindowNode>>* rootApp = displayGroupController_->GetWindowNodesByDisplayIdAndRootType(
        1, WindowRootNodeType::ABOVE_WINDOW_NODE);
    rootApp->push_back(node1);
    node1->SetDisplayId(1);
    std::vector<uint32_t> windowIds;
    displayGroupController_->ProcessNotCrossNodesOnDestroyedDisplay(1, windowIds);
    ASSERT_EQ(1, windowIds.size());
    ASSERT_EQ(1, node1->GetDisplayId());
}

/**
 * @tc.name: ProcessNotCrossNodesOnDestroyedDisplay06
 * @tc.desc: Execute to move to default display
 * @tc.type: FUNC
 */
HWTEST_F(DisplayGroupControllerTest, ProcessNotCrossNodesOnDestroyedDisplay06, Function | SmallTest | Level2)
{
    displayGroupController_->InitNewDisplay(1);
    SetDisplayGroupInfo(1, {200, 200, 200, 200});
    sptr<WindowNode> node1 = new WindowNode(CreateWindowProperty(100));
    ASSERT_NE(nullptr, node1);
    displayGroupController_->InitNewDisplay(1);
    SetDisplayGroupInfo(1, {200, 200, 200, 200});
    std::vector<sptr<WindowNode>>* rootApp = displayGroupController_->GetWindowNodesByDisplayIdAndRootType(
        1, WindowRootNodeType::APP_WINDOW_NODE);
    rootApp->push_back(node1);
    node1->SetDisplayId(1);
    std::vector<uint32_t> windowIds;
    windowIds.push_back(node1->GetWindowId());
    displayGroupController_->ProcessNotCrossNodesOnDestroyedDisplay(1, windowIds);
    ASSERT_EQ(0, node1->GetDisplayId());
}

/**
 * @tc.name: ProcessNotCrossNodesOnDestroyedDisplay07
 * @tc.desc: DisplayId not equals to defaultDisplayId, and no node on groupWindowTree
 * @tc.type: FUNC
 */
HWTEST_F(DisplayGroupControllerTest, ProcessNotCrossNodesOnDestroyedDisplay07, Function | SmallTest | Level2)
{
    displayGroupController_->InitNewDisplay(1);
    SetDisplayGroupInfo(1, {200, 200, 200, 200});
    std::vector<uint32_t> windowIds;
    displayGroupController_->ProcessNotCrossNodesOnDestroyedDisplay(1, windowIds);
    ASSERT_EQ(0, windowIds.size());
}

/**
 * @tc.name: UpdateNodeSizeChangeReasonWithRotation01
 * @tc.desc: UpdateNodeSizeChangeReasonWithRotation Success
 * @tc.type: FUNC
 */
HWTEST_F(DisplayGroupControllerTest, UpdateNodeSizeChangeReasonWithRotation01, Function | SmallTest | Level2)
{
    sptr<WindowNode> node1 = new WindowNode(CreateWindowProperty(100));
    ASSERT_NE(nullptr, node1);
    std::vector<sptr<WindowNode>>* rootApp = displayGroupController_->GetWindowNodesByDisplayIdAndRootType(
        0, WindowRootNodeType::APP_WINDOW_NODE);
    rootApp->push_back(node1);
    displayGroupController_->UpdateNodeSizeChangeReasonWithRotation(0);
    ASSERT_EQ(WindowSizeChangeReason::ROTATION, node1->GetWindowSizeChangeReason());
}

/**
 * @tc.name: UpdateNodeSizeChangeReasonWithRotation02
 * @tc.desc: UpdateNodeSizeChangeReasonWithRotation failed
 * @tc.type: FUNC
 */
HWTEST_F(DisplayGroupControllerTest, UpdateNodeSizeChangeReasonWithRotation02, Function | SmallTest | Level2)
{
    sptr<WindowNode> node1 = new WindowNode(CreateWindowProperty(100, WindowType::WINDOW_TYPE_DOCK_SLICE));
    ASSERT_NE(nullptr, node1);
    std::vector<sptr<WindowNode>>* rootApp = displayGroupController_->GetWindowNodesByDisplayIdAndRootType(
        0, WindowRootNodeType::ABOVE_WINDOW_NODE);
    rootApp->push_back(node1);
    displayGroupController_->UpdateNodeSizeChangeReasonWithRotation(0);
    ASSERT_NE(WindowSizeChangeReason::ROTATION, node1->GetWindowSizeChangeReason());
}

/**
 * @tc.name: UpdateNodeSizeChangeReasonWithRotation03
 * @tc.desc: UpdateNodeSizeChangeReasonWithRotation with rootNodeVectorPtr null
 * @tc.type: FUNC
 */
HWTEST_F(DisplayGroupControllerTest, UpdateNodeSizeChangeReasonWithRotation03, Function | SmallTest | Level2)
{
    displayGroupController_->displayGroupWindowTree_[0].erase(WindowRootNodeType::ABOVE_WINDOW_NODE);
    ASSERT_EQ(nullptr, displayGroupController_->GetWindowNodesByDisplayIdAndRootType(
        0, WindowRootNodeType::ABOVE_WINDOW_NODE));
    displayGroupController_->UpdateNodeSizeChangeReasonWithRotation(0);
    ASSERT_EQ(nullptr, displayGroupController_->GetWindowNodesByDisplayIdAndRootType(
        0, WindowRootNodeType::ABOVE_WINDOW_NODE));
}

/**
 * @tc.name: ProcessDisplayChange01
 * @tc.desc: ProcessDisplayChange with different DisplayStateChangeType
 * @tc.type: FUNC
 */
HWTEST_F(DisplayGroupControllerTest, ProcessDisplayChange01, Function | SmallTest | Level2)
{
    sptr<DisplayInfo> displayInfo = new DisplayInfo();
    ASSERT_NE(nullptr, displayInfo);
    displayInfo->SetDisplayId(0);
    displayGroupController_->ProcessDisplayChange(0, displayInfo, displayGroupInfo_->GetAllDisplayRects(),
        DisplayStateChangeType::UPDATE_ROTATION);
    displayGroupController_->ProcessDisplayChange(0, displayInfo, displayGroupInfo_->GetAllDisplayRects(),
        DisplayStateChangeType::DISPLAY_COMPRESS);
    displayGroupController_->ProcessDisplayChange(0, displayInfo, displayGroupInfo_->GetAllDisplayRects(),
        DisplayStateChangeType::SIZE_CHANGE);
    displayGroupController_->ProcessDisplayChange(0, displayInfo, displayGroupInfo_->GetAllDisplayRects(),
        DisplayStateChangeType::VIRTUAL_PIXEL_RATIO_CHANGE);
    displayGroupController_->ProcessDisplayChange(0, displayInfo, displayGroupInfo_->GetAllDisplayRects(),
        DisplayStateChangeType::DESTROY);
}

/**
 * @tc.name: ProcessDisplaySizeChangeOrRotation01
 * @tc.desc: ProcessDisplaySizeChangeOrRotation with layout policy null
 * @tc.type: FUNC
 */
HWTEST_F(DisplayGroupControllerTest, ProcessDisplaySizeChangeOrRotation01, Function | SmallTest | Level2)
{
    auto oriLayoutPolicy = container_->GetLayoutPolicy();
    container_->layoutPolicy_ = nullptr;
    displayGroupController_->ProcessDisplaySizeChangeOrRotation(0, 0, displayGroupInfo_->GetAllDisplayRects(),
        DisplayStateChangeType::UPDATE_ROTATION);
    ASSERT_EQ(nullptr, container_->GetLayoutPolicy());
    container_->layoutPolicy_ = oriLayoutPolicy;
}

/**
 * @tc.name: GetWindowPairByDisplayId
 * @tc.desc: GetWindowPairByDisplayId with displayId 1, which not exists
 * @tc.type: FUNC
 */
HWTEST_F(DisplayGroupControllerTest, GetWindowPairByDisplayId01, Function | SmallTest | Level2)
{
    ASSERT_EQ(nullptr, displayGroupController_->GetWindowPairByDisplayId(1));
}
}
}
}
