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
#include "window_root.h"
#include "window_manager.h"
#include "display_manager.h"

using namespace testing;
using namespace testing::ext;
namespace OHOS {
namespace Rosen {

class WindowRootTest : public testing::Test {
public:
    static void SetUpTestCase();
    static void TearDownTestCase();
    void SetUp() override;
    void TearDown() override;
    static sptr<WindowRoot> windowRoot_;
};

sptr<WindowRoot> WindowRootTest::windowRoot_ = nullptr;

void WindowRootTest::SetUpTestCase()
{
}

void WindowRootTest::TearDownTestCase()
{
}

void WindowRootTest::SetUp()
{
    windowRoot_ = new WindowRoot(nullptr);
}

void WindowRootTest::TearDown()
{
    windowRoot_ = nullptr;
}

namespace {
/**
 * @tc.name: WindowRootTest01
 * @tc.desc: test WindowRoot GetTotalWindowNum
 * @tc.type: FUNC
 */
HWTEST_F(WindowRootTest, WindowRootTest01, Function | SmallTest | Level2)
{
    uint32_t size = windowRoot_->GetTotalWindowNum();
    ASSERT_EQ(size, 0);
}

/**
 * @tc.name: WindowRootTest02
 * @tc.desc: test WindowRoot GetWindowForDumpAceHelpInfo
 * @tc.type: FUNC
 */
HWTEST_F(WindowRootTest, WindowRootTest02, Function | SmallTest | Level2)
{
    sptr<WindowProperty> property = new WindowProperty();
    property->SetWindowType(WindowType::WINDOW_TYPE_DESKTOP);
    sptr<WindowNode> windowNode = new WindowNode(property);
    windowRoot_->SaveWindow(windowNode);
    sptr<WindowNode> node = windowRoot_->GetWindowForDumpAceHelpInfo();
    ASSERT_NE(node, nullptr);
    windowRoot_->DestroyWindowInner(windowNode);

    property = new WindowProperty();
    property->SetWindowType(WindowType::WINDOW_TYPE_NAVIGATION_BAR);
    windowNode = new WindowNode(property);
    windowRoot_->SaveWindow(windowNode);
    node = windowRoot_->GetWindowForDumpAceHelpInfo();
    ASSERT_NE(node, nullptr);
    windowRoot_->DestroyWindowInner(windowNode);

    property = new WindowProperty();
    property->SetWindowType(WindowType::WINDOW_TYPE_STATUS_BAR);
    windowNode = new WindowNode(property);
    windowRoot_->SaveWindow(windowNode);
    node = windowRoot_->GetWindowForDumpAceHelpInfo();
    ASSERT_NE(node, nullptr);
    windowRoot_->DestroyWindowInner(windowNode);

    property = new WindowProperty();
    property->SetWindowType(WindowType::WINDOW_TYPE_KEYGUARD);
    windowNode = new WindowNode(property);
    windowRoot_->SaveWindow(windowNode);
    node = windowRoot_->GetWindowForDumpAceHelpInfo();
    ASSERT_NE(node, nullptr);
    windowRoot_->DestroyWindowInner(windowNode);
}

/**
 * @tc.name: WindowRootTest03
 * @tc.desc: test WindowRoot CreateWindowNodeContainer
 * @tc.type: FUNC
 */
HWTEST_F(WindowRootTest, WindowRootTest03, Function | SmallTest | Level2)
{
    sptr<DisplayInfo> displayInfo = new DisplayInfo();

    displayInfo->SetWidth(49);
    auto container = windowRoot_->CreateWindowNodeContainer(displayInfo);
    ASSERT_EQ(container, nullptr);

    displayInfo->SetWidth(7681);
    container = windowRoot_->CreateWindowNodeContainer(displayInfo);
    ASSERT_EQ(container, nullptr);

    displayInfo->SetWidth(50);
    displayInfo->SetHeight(49);
    container = windowRoot_->CreateWindowNodeContainer(displayInfo);
    ASSERT_EQ(container, nullptr);

    displayInfo->SetHeight(7681);
    container = windowRoot_->CreateWindowNodeContainer(displayInfo);
    ASSERT_EQ(container, nullptr);
}

/**
 * @tc.name: WindowRootTest04
 * @tc.desc: test WindowRoot GetWindowNodeContainer
 * @tc.type: FUNC
 */
HWTEST_F(WindowRootTest, WindowRootTest04, Function | SmallTest | Level2)
{
    auto display = DisplayManager::GetInstance().GetDefaultDisplay();
    ASSERT_NE(display, nullptr);
    sptr<DisplayInfo> displayInfo = display->GetDisplayInfo();
    ASSERT_NE(displayInfo, nullptr);
    displayInfo->SetDisplayId(0);
    displayInfo->SetScreenGroupId(SCREEN_ID_INVALID);
    auto container = windowRoot_->CreateWindowNodeContainer(displayInfo);
    ASSERT_NE(container, nullptr);

    windowRoot_->GetWindowNodeContainer(DISPLAY_ID_INVALID);
}

/**
 * @tc.name: WindowRootTest05
 * @tc.desc: test WindowRoot GetBackgroundNodesByScreenId
 * @tc.type: FUNC
 */
HWTEST_F(WindowRootTest, WindowRootTest05, Function | SmallTest | Level2)
{
    std::vector<sptr<WindowNode>> windowNodes;

    sptr<WindowProperty> property = new WindowProperty();
    property->SetDisplayId(DISPLAY_ID_INVALID);
    sptr<WindowNode> windowNode1 = new WindowNode(property);
    windowRoot_->SaveWindow(windowNode1);
    property->SetDisplayId(0);
    sptr<WindowNode> windowNode2 = new WindowNode(property);
    windowRoot_->SaveWindow(windowNode2);

    auto screenGroupId = DisplayManagerServiceInner::GetInstance().GetScreenGroupIdByDisplayId(DISPLAY_ID_INVALID);
    windowRoot_->GetBackgroundNodesByScreenId(screenGroupId, windowNodes);

    windowRoot_->DestroyWindowInner(windowNode1);
    windowRoot_->DestroyWindowInner(windowNode2);

    ASSERT_EQ(true, true);
}

/**
 * @tc.name: WindowRootTest06
 * @tc.desc: test WindowRoot AddDeathRecipient
 * @tc.type: FUNC
 */
HWTEST_F(WindowRootTest, WindowRootTest06, Function | SmallTest | Level2)
{
    windowRoot_->AddDeathRecipient(nullptr);

    ASSERT_EQ(true, true);
}

/**
 * @tc.name: WindowRootTest07
 * @tc.desc: test WindowRoot SaveWindow
 * @tc.type: FUNC
 */
HWTEST_F(WindowRootTest, WindowRootTest07, Function | SmallTest | Level2)
{
    windowRoot_->SaveWindow(nullptr);

    ASSERT_EQ(true, true);
}

/**
 * @tc.name: WindowRootTest08
 * @tc.desc: test WindowRoot MinimizeStructuredAppWindowsExceptSelf
 * @tc.type: FUNC
 */
HWTEST_F(WindowRootTest, WindowRootTest08, Function | SmallTest | Level2)
{
    sptr<WindowProperty> property = new WindowProperty();
    property->SetDisplayId(DISPLAY_ID_INVALID);
    sptr<WindowNode> windowNode = new WindowNode(property);

    WMError ret = windowRoot_->MinimizeStructuredAppWindowsExceptSelf(windowNode);

    ASSERT_EQ(ret, WMError::WM_ERROR_NULLPTR);
}

/**
 * @tc.name: WindowRootTest09
 * @tc.desc: test WindowRoot MinimizeTargetWindows
 * @tc.type: FUNC
 */
HWTEST_F(WindowRootTest, WindowRootTest09, Function | SmallTest | Level2)
{
    std::vector<uint32_t> windowIds;

    windowRoot_->MinimizeTargetWindows(windowIds);

    windowIds.push_back(INVALID_WINDOW_ID);
    windowRoot_->MinimizeTargetWindows(windowIds);

    sptr<WindowProperty> property = new WindowProperty();
    property->SetWindowId(1);
    property->SetWindowType(WindowType::WINDOW_TYPE_APP_MAIN_WINDOW);
    auto windowNode = new WindowNode(property);
    windowRoot_->SaveWindow(windowNode);
    property = new WindowProperty();
    property->SetWindowId(2);
    property->SetWindowType(WindowType::WINDOW_TYPE_APP_SUB_WINDOW);
    windowNode = new WindowNode(property);
    windowRoot_->SaveWindow(windowNode);
    windowIds.push_back(1);
    windowIds.push_back(2);
    windowRoot_->MinimizeTargetWindows(windowIds);

    ASSERT_EQ(true, true);
}

/**
 * @tc.name: WindowRootTest10
 * @tc.desc: test WindowRoot GetSplitScreenWindowNodes
 * @tc.type: FUNC
 */
HWTEST_F(WindowRootTest, WindowRootTest10, Function | SmallTest | Level2)
{
    std::vector<sptr<WindowNode>> windowNodes = windowRoot_->GetSplitScreenWindowNodes(DISPLAY_ID_INVALID);
    ASSERT_EQ(windowNodes.empty(), true);
}

/**
 * @tc.name: WindowRootTest11
 * @tc.desc: test WindowRoot IsForbidDockSliceMove
 * @tc.type: FUNC
 */
HWTEST_F(WindowRootTest, WindowRootTest11, Function | SmallTest | Level2)
{
    bool ret = windowRoot_->IsForbidDockSliceMove(DISPLAY_ID_INVALID);
    ASSERT_EQ(ret, true);

    ret = windowRoot_->IsForbidDockSliceMove(0);
    ASSERT_EQ(ret, true);
}

/**
 * @tc.name: WindowRootTest12
 * @tc.desc: test WindowRoot IsDockSliceInExitSplitModeArea
 * @tc.type: FUNC
 */
HWTEST_F(WindowRootTest, WindowRootTest12, Function | SmallTest | Level2)
{
    bool ret = windowRoot_->IsDockSliceInExitSplitModeArea(DISPLAY_ID_INVALID);
    ASSERT_EQ(ret, false);

    ret = windowRoot_->IsDockSliceInExitSplitModeArea(0);
    ASSERT_EQ(ret, false);
}

/**
 * @tc.name: WindowRootTest13
 * @tc.desc: test WindowRoot ExitSplitMode
 * @tc.type: FUNC
 */
HWTEST_F(WindowRootTest, WindowRootTest13, Function | SmallTest | Level2)
{
    windowRoot_->ExitSplitMode(DISPLAY_ID_INVALID);

    windowRoot_->ExitSplitMode(0);
    ASSERT_EQ(true, true);
}

/**
 * @tc.name: WindowRootTest14
 * @tc.desc: test WindowRoot AddSurfaceNodeIdWindowNodePair
 * @tc.type: FUNC
 */
HWTEST_F(WindowRootTest, WindowRootTest14, Function | SmallTest | Level2)
{
    windowRoot_->AddSurfaceNodeIdWindowNodePair(INVALID_WINDOW_ID, nullptr);

    ASSERT_EQ(true, true);
}

/**
 * @tc.name: WindowRootTest15
 * @tc.desc: test WindowRoot GetVisibilityWindowInfo
 * @tc.type: FUNC
 */
HWTEST_F(WindowRootTest, WindowRootTest15, Function | SmallTest | Level2)
{
    std::vector<sptr<WindowVisibilityInfo>> infos = {};

    windowRoot_->GetVisibilityWindowInfo(infos);

    ASSERT_EQ(true, true);
}

/**
 * @tc.name: WindowRootTest16
 * @tc.desc: test WindowRoot GetAvoidAreaByType
 * @tc.type: FUNC
 */
HWTEST_F(WindowRootTest, WindowRootTest16, Function | SmallTest | Level2)
{
    AvoidArea emptyArea;
    AvoidArea area;
    sptr<WindowNode> node = new WindowNode();

    area = windowRoot_->GetAvoidAreaByType(node->GetWindowId(), AvoidAreaType::TYPE_CUTOUT);
    ASSERT_EQ(area, emptyArea);

    windowRoot_->windowNodeMap_.insert(std::make_pair(node->GetWindowId(), node));
    area = windowRoot_->GetAvoidAreaByType(node->GetWindowId(), AvoidAreaType::TYPE_CUTOUT);
    ASSERT_EQ(area, emptyArea);
}

/**
 * @tc.name: WindowRootTest17
 * @tc.desc: test WindowRoot MinimizeAllAppWindows
 * @tc.type: FUNC
 */
HWTEST_F(WindowRootTest, WindowRootTest17, Function | SmallTest | Level2)
{
    windowRoot_->MinimizeAllAppWindows(DISPLAY_ID_INVALID);

    ASSERT_EQ(true, true);
}

/**
 * @tc.name: WindowRootTest18
 * @tc.desc: test WindowRoot DestroyLeakStartingWindow
 * @tc.type: FUNC
 */
HWTEST_F(WindowRootTest, WindowRootTest18, Function | SmallTest | Level2)
{
    windowRoot_->DestroyLeakStartingWindow();

    ASSERT_EQ(true, true);
}

/**
 * @tc.name: WindowRootTest19
 * @tc.desc: test WindowRoot PostProcessAddWindowNode
 * @tc.type: FUNC
 */
HWTEST_F(WindowRootTest, WindowRootTest19, Function | SmallTest | Level2)
{
    WMError ret;
    sptr<WindowNode> node = new WindowNode();
    sptr<WindowNode> parentNode = nullptr;
    sptr<DisplayInfo> displayInfo = new DisplayInfo();
    sptr<WindowNodeContainer> container = new WindowNodeContainer(displayInfo, SCREEN_ID_INVALID);

    ret = windowRoot_->PostProcessAddWindowNode(node, parentNode, container);
    ASSERT_EQ(ret, WMError::WM_DO_NOTHING);

    node->currentVisibility_ = true;
    node->GetWindowProperty()->SetWindowType(WindowType::APP_SUB_WINDOW_BASE);
    ret = windowRoot_->PostProcessAddWindowNode(node, parentNode, container);
    ASSERT_EQ(ret, WMError::WM_ERROR_INVALID_TYPE);
    
    node->currentVisibility_ = true;
    node->property_->SetWindowType(WindowType::WINDOW_TYPE_STATUS_BAR);
    node->GetWindowProperty()->SetFocusable(false);
    sptr<WindowNode> child = new WindowNode();
    child->currentVisibility_ = true;
    node->children_.push_back(child);
    node->children_.push_back(nullptr);
    ret = windowRoot_->PostProcessAddWindowNode(node, parentNode, container);
    ASSERT_EQ(ret, WMError::WM_OK);
}

/**
 * @tc.name: WindowRootTest20
 * @tc.desc: test WindowRoot LayoutWhenAddWindowNode
 * @tc.type: FUNC
 */
HWTEST_F(WindowRootTest, WindowRootTest20, Function | SmallTest | Level2)
{
    sptr<WindowNode> node = nullptr;
    windowRoot_->LayoutWhenAddWindowNode(node, true);

    node = new WindowNode();
    windowRoot_->LayoutWhenAddWindowNode(node, true);

    ASSERT_EQ(true, true);
}

/**
 * @tc.name: WindowRootTest21
 * @tc.desc: test WindowRoot AddWindowNode
 * @tc.type: FUNC
 */
HWTEST_F(WindowRootTest, WindowRootTest21, Function | SmallTest | Level2)
{
    WMError ret;
    sptr<WindowNode> node = nullptr;

    ret = windowRoot_->AddWindowNode(INVALID_WINDOW_ID, node, true);
    ASSERT_EQ(ret, WMError::WM_ERROR_NULLPTR);
}

/**
 * @tc.name: WindowRootTest22
 * @tc.desc: test WindowRoot RemoveWindowNode
 * @tc.type: FUNC
 */
HWTEST_F(WindowRootTest, WindowRootTest22, Function | SmallTest | Level2)
{
    WMError ret;
    sptr<WindowNode> node = new WindowNode();

    windowRoot_->windowNodeMap_.insert(std::make_pair(node->GetWindowId(), node));
    ret = windowRoot_->RemoveWindowNode(node->GetWindowId(), true);
    ASSERT_EQ(ret, WMError::WM_ERROR_NULLPTR);
}

/**
 * @tc.name: WindowRootTest23
 * @tc.desc: test WindowRoot UpdateWindowNode
 * @tc.type: FUNC
 */
HWTEST_F(WindowRootTest, WindowRootTest23, Function | SmallTest | Level2)
{
    WMError ret;

    ret = windowRoot_->UpdateWindowNode(INVALID_WINDOW_ID, WindowUpdateReason::UPDATE_MODE);
    ASSERT_EQ(ret, WMError::WM_ERROR_NULLPTR);

    sptr<WindowNode> node = new WindowNode();
    windowRoot_->windowNodeMap_.insert(std::make_pair(node->GetWindowId(), node));
    ret = windowRoot_->UpdateWindowNode(node->GetWindowId(), WindowUpdateReason::UPDATE_MODE);
    ASSERT_EQ(ret, WMError::WM_ERROR_NULLPTR);
}

/**
 * @tc.name: WindowRootTest24
 * @tc.desc: test WindowRoot UpdateSizeChangeReason
 * @tc.type: FUNC
 */
HWTEST_F(WindowRootTest, WindowRootTest24, Function | SmallTest | Level2)
{
    WMError ret;

    ret = windowRoot_->UpdateSizeChangeReason(INVALID_WINDOW_ID, WindowSizeChangeReason::UNDEFINED);
    ASSERT_EQ(ret, WMError::WM_ERROR_NULLPTR);

    sptr<WindowNode> node = new WindowNode();
    windowRoot_->windowNodeMap_.insert(std::make_pair(node->GetWindowId(), node));
    ret = windowRoot_->UpdateSizeChangeReason(node->GetWindowId(), WindowSizeChangeReason::UNDEFINED);
    ASSERT_EQ(ret, WMError::WM_ERROR_NULLPTR);
}

/**
 * @tc.name: WindowRootTest25
 * @tc.desc: test WindowRoot SetBrightness
 * @tc.type: FUNC
 */
HWTEST_F(WindowRootTest, WindowRootTest25, Function | SmallTest | Level2)
{
    windowRoot_->SetBrightness(INVALID_WINDOW_ID, 0);

    sptr<WindowNode> node = new WindowNode();
    windowRoot_->windowNodeMap_.insert(std::make_pair(node->GetWindowId(), node));
    windowRoot_->SetBrightness(node->GetWindowId(), 0);

    ASSERT_EQ(true, true);
}
}
}
}
