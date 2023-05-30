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
#include "display_group_controller.h"
#include "display_group_info.h"
#include "display_manager.h"
#include "remote_animation.h"
#include "window_helper.h"
#include "window_layout_policy.h"
#include "window_layout_policy_cascade.h"
#include "window_node_container.h"

using namespace testing;
using namespace testing::ext;

namespace OHOS {
namespace Rosen {

struct WindowInfo {
    Rect winRect_;
    WindowType winType_;
    WindowMode winMode_;
    WindowSizeChangeReason reason_;
    DragType dragType_;
    bool decorEnable_;
};

class WindowLayoutPolicyTest : public testing::Test {
public:
    static void SetUpTestCase();
    static void TearDownTestCase();
    virtual void SetUp() override;
    virtual void TearDown() override;
    sptr<WindowNode> CreateWindowNode(const WindowInfo& windowInfo);
    sptr<DisplayInfo> CreateDisplayInfo(const Rect& displayRect);

    static WindowInfo windowInfo_;
    static sptr<WindowNodeContainer> container_;
    static sptr<DisplayGroupInfo> displayGroupInfo_;
    static sptr<DisplayInfo> defaultDisplayInfo_;
    static sptr<DisplayGroupController> displayGroupController_;
    static sptr<WindowLayoutPolicy> layoutPolicy_;
};

sptr<WindowNodeContainer> WindowLayoutPolicyTest::container_ = nullptr;
sptr<DisplayGroupInfo> WindowLayoutPolicyTest::displayGroupInfo_ = nullptr;
sptr<DisplayGroupController> WindowLayoutPolicyTest::displayGroupController_ = nullptr;
sptr<WindowLayoutPolicy> WindowLayoutPolicyTest::layoutPolicy_ = nullptr;
sptr<DisplayInfo> WindowLayoutPolicyTest::defaultDisplayInfo_ = nullptr;
WindowInfo WindowLayoutPolicyTest::windowInfo_ = {
    .winRect_ = { 0, 0, 0, 0 },
    .winType_ = WindowType::WINDOW_TYPE_APP_MAIN_WINDOW,
    .winMode_ = WindowMode::WINDOW_MODE_FLOATING,
    .reason_ = WindowSizeChangeReason::UNDEFINED,
    .dragType_ = DragType::DRAG_UNDEFINED,
    .decorEnable_ = false,
};

void WindowLayoutPolicyTest::SetUpTestCase()
{
    auto display = DisplayManager::GetInstance().GetDefaultDisplay();
    ASSERT_TRUE((display != nullptr));

    defaultDisplayInfo_ = display->GetDisplayInfo();
    ASSERT_TRUE((defaultDisplayInfo_ != nullptr));

    windowInfo_ = {
        .winRect_ = { 0, 0, 0, 0 },
        .winType_ = WindowType::WINDOW_TYPE_APP_MAIN_WINDOW,
        .winMode_ = WindowMode::WINDOW_MODE_FLOATING,
        .reason_ = WindowSizeChangeReason::UNDEFINED,
        .dragType_ = DragType::DRAG_UNDEFINED,
        .decorEnable_ = false,
    };

    container_ = new WindowNodeContainer(defaultDisplayInfo_, display->GetScreenId());
    displayGroupInfo_ = container_->displayGroupInfo_;
    displayGroupController_ = container_->displayGroupController_;
    layoutPolicy_ = container_->GetLayoutPolicy();
}

void WindowLayoutPolicyTest::TearDownTestCase()
{
    container_ = nullptr;
    displayGroupInfo_ = nullptr;
    defaultDisplayInfo_ = nullptr;
    displayGroupController_ = nullptr;
    layoutPolicy_ = nullptr;
}

void WindowLayoutPolicyTest::SetUp()
{
    displayGroupInfo_->AddDisplayInfo(defaultDisplayInfo_);
}

void WindowLayoutPolicyTest::TearDown()
{
    displayGroupInfo_->displayInfosMap_.clear();
}

sptr<WindowNode> WindowLayoutPolicyTest::CreateWindowNode(const WindowInfo& windowInfo)
{
    sptr<WindowProperty> property = new WindowProperty();
    property->SetWindowType(windowInfo.winType_);
    property->SetWindowMode(windowInfo.winMode_);
    property->SetWindowRect(windowInfo.winRect_);
    property->SetOriginRect(windowInfo.winRect_);
    property->SetDecorEnable(windowInfo.decorEnable_);
    sptr<WindowNode> node = new WindowNode(property, nullptr, nullptr);
    node->SetWindowSizeChangeReason(windowInfo.reason_);
    node->SetDragType(windowInfo.dragType_);
    return node;
}

sptr<DisplayInfo> WindowLayoutPolicyTest::CreateDisplayInfo(const Rect& displayRect)
{
    sptr<DisplayInfo> displayInfo = new DisplayInfo();
    displayInfo->SetOffsetX(displayRect.posX_);
    displayInfo->SetOffsetY(displayRect.posY_);
    displayInfo->SetWidth(displayRect.width_);
    displayInfo->SetHeight(displayRect.height_);
    displayInfo->SetDisplayId((defaultDisplayInfo_->GetDisplayId() + 1));
    return displayInfo;
}
namespace {
/**
 * @tc.name: CalcEntireWindowHotZone
 * @tc.desc: calc entire window hot zone
 * @tc.type: FUNC
 * @tc.require issueI5LYDC
 */
HWTEST_F(WindowLayoutPolicyTest, CalcEntireWindowHotZone, Function | SmallTest | Level2)
{
    TransformHelper::Vector2 hotZoneScale = {1.f, 1.f}; // ratio 1.0
    sptr<WindowProperty> property = new WindowProperty();

    property->SetWindowType(WindowType::WINDOW_TYPE_APP_MAIN_WINDOW);
    property->SetWindowMode(WindowMode::WINDOW_MODE_FLOATING);
    sptr<WindowNode> node0 = new WindowNode(property, nullptr, nullptr);
    Rect winRect = { 50, 100, 400, 500 }; // rect: 50, 100, 400, 500
    auto actRect0 = layoutPolicy_->CalcEntireWindowHotZone(node0, winRect, 10, 2.f, hotZoneScale); // param: 10, 2.0
    Rect expRect0 = { 30, 80, 440, 540 }; // rect: 30, 80, 440, 540
    ASSERT_EQ(expRect0, actRect0);

    property->SetWindowType(WindowType::WINDOW_TYPE_DOCK_SLICE);
    sptr<WindowNode> node1 = new WindowNode(property, nullptr, nullptr);
    auto actRect1 = layoutPolicy_->CalcEntireWindowHotZone(node1, winRect, 10, 2.f, hotZoneScale); // param: 10, 2.0
    Rect expRect1 = { 30, 100, 440, 500 }; // rect: 30, 100, 440, 500
    ASSERT_EQ(expRect1, actRect1);

    property->SetWindowType(WindowType::WINDOW_TYPE_LAUNCHER_RECENT);
    sptr<WindowNode> node2 = new WindowNode(property, nullptr, nullptr);
    auto actRect2 = layoutPolicy_->CalcEntireWindowHotZone(node2, winRect, 10, 2.f, hotZoneScale); // param: 10, 2.0
    Rect expRect2 = displayGroupInfo_->GetDisplayRect(defaultDisplayInfo_->GetDisplayId());;
    ASSERT_EQ(expRect2, actRect2);
}

/**
 * @tc.name: UpdateFloatingWindowSizeForStretchableWindow01
 * @tc.desc: UpdateFloatingWindowSizeForStretchableWindow test for drag width
 * @tc.type: FUNC
 */
HWTEST_F(WindowLayoutPolicyTest, UpdateFloatingWindowSizeForStretchableWindow01, Function | SmallTest | Level2)
{
    windowInfo_.winRect_ = { 50, 50, 100, 150 };  // rect: 50, 50, 100, 150
    windowInfo_.dragType_ = DragType::DRAG_LEFT_OR_RIGHT;
    windowInfo_.reason_ = WindowSizeChangeReason::DRAG;
    sptr<WindowNode> node = CreateWindowNode(windowInfo_);
    Rect newWinRect = { 50, 50, 200, 200 };       // rect: 50, 50, 200, 200
    layoutPolicy_->UpdateFloatingWindowSizeForStretchableWindow(node, { 0, 0, 0, 0 }, newWinRect);
    Rect expRect = { 50, 50, 200, 300 };          // rect: 50, 50, 200, 300
    ASSERT_EQ(expRect, newWinRect);
}

/**
 * @tc.name: UpdateFloatingWindowSizeForStretchableWindow02
 * @tc.desc: UpdateFloatingWindowSizeForStretchableWindow test for drag coner
 * @tc.type: FUNC
 */
HWTEST_F(WindowLayoutPolicyTest, UpdateFloatingWindowSizeForStretchableWindow02, Function | SmallTest | Level2)
{
    windowInfo_.winRect_ = { 50, 50, 100, 150 }; // rect: 50, 50, 100, 150
    windowInfo_.dragType_ = DragType::DRAG_LEFT_TOP_CORNER;
    windowInfo_.reason_ = WindowSizeChangeReason::DRAG;
    sptr<WindowNode> node = CreateWindowNode(windowInfo_);
    Rect newWinRect = { 50, 50, 200, 200 };      // rect: 50, 50, 200, 200
    layoutPolicy_->UpdateFloatingWindowSizeForStretchableWindow(node, { 0, 0, 0, 0 }, newWinRect);
    Rect expRect = { 50, 50, 200, 300 };         // rect: 50, 50, 200, 300
    ASSERT_EQ(expRect, newWinRect);
}

/**
 * @tc.name: UpdateFloatingWindowSizeForStretchableWindow03
 * @tc.desc: UpdateFloatingWindowSizeForStretchableWindow test for drag height
 * @tc.type: FUNC
 */
HWTEST_F(WindowLayoutPolicyTest, UpdateFloatingWindowSizeForStretchableWindow03, Function | SmallTest | Level2)
{
    windowInfo_.winRect_ = { 50, 50, 100, 150 }; // rect: 50, 50, 100, 150
    windowInfo_.dragType_ = DragType::DRAG_BOTTOM_OR_TOP;
    windowInfo_.reason_ = WindowSizeChangeReason::DRAG;
    sptr<WindowNode> node = CreateWindowNode(windowInfo_);
    Rect newWinRect = { 50, 50, 150, 300 };      // rect: 50, 50, 150, 300
    layoutPolicy_->UpdateFloatingWindowSizeForStretchableWindow(node, { 0, 0, 0, 0 }, newWinRect);
    Rect expRect = { 50, 50, 200, 300 };         // rect: 50, 50, 200, 300
    ASSERT_EQ(expRect, newWinRect);
}

/**
 * @tc.name: LimitWindowToBottomRightCorner
 * @tc.desc: test LimitWindowToBottomRightCorner01
 * @tc.type: FUNC
 */
HWTEST_F(WindowLayoutPolicyTest, LimitWindowToBottomRightCorner01, Function | SmallTest | Level2)
{
    auto displayRect = displayGroupInfo_->GetDisplayRect(defaultDisplayInfo_->GetDisplayId());
    ASSERT_FALSE(WindowHelper::IsEmptyRect(displayRect));
    windowInfo_.winRect_ = {
        .posX_ = static_cast<int32_t>(displayRect.posX_ + displayRect.width_ * 0.5), // ratio: 0.5
        .posY_ = displayRect.posY_,
        .width_ = displayRect.width_,
        .height_ = displayRect.height_
    };
    sptr<WindowNode> node = CreateWindowNode(windowInfo_);
    ASSERT_TRUE(node != nullptr);
    layoutPolicy_->LimitWindowToBottomRightCorner(node);

    Rect winRect = {
        .posX_ = displayRect.posX_,
        .posY_ = displayRect.posY_,
        .width_ = static_cast<uint32_t>(displayRect.width_ * 0.5),  // ratio: 0.5
        .height_ = static_cast<uint32_t>(displayRect.height_ * 0.5) // ratio: 0.5
    };
    node ->SetRequestRect(winRect);
    layoutPolicy_->LimitWindowToBottomRightCorner(node);
}

/**
 * @tc.name: LimitWindowToBottomRightCorner
 * @tc.desc: test LimitWindowToBottomRightCorner02
 * @tc.type: FUNC
 */
HWTEST_F(WindowLayoutPolicyTest, LimitWindowToBottomRightCorner02, Function | SmallTest | Level2)
{
    auto displayRect = displayGroupInfo_->GetDisplayRect(defaultDisplayInfo_->GetDisplayId());
    ASSERT_FALSE(WindowHelper::IsEmptyRect(displayRect));
    windowInfo_.winRect_ = {
        .posX_ = static_cast<int32_t>(displayRect.posX_ + displayRect.width_ * 0.5),  // ratio: 0.5
        .posY_ = static_cast<int32_t>(displayRect.posY_ + displayRect.height_ * 0.5), // ratio: 0.5
        .width_ = displayRect.width_,
        .height_ = displayRect.height_
    };
    sptr<WindowNode> node = CreateWindowNode(windowInfo_);
    ASSERT_TRUE(node != nullptr);
    layoutPolicy_->LimitWindowToBottomRightCorner(node);

    Rect winRect = {
        .posX_ = displayRect.posX_,
        .posY_ = static_cast<int32_t>(displayRect.posY_ + displayRect.height_ * 0.5), // ratio: 0.5
        .width_ = displayRect.width_,
        .height_ = displayRect.height_
    };
    node ->SetRequestRect(winRect);
    layoutPolicy_->LimitWindowToBottomRightCorner(node);
    ASSERT_TRUE(node != nullptr);
}

/**
 * @tc.name: LimitWindowToBottomRightCorner
 * @tc.desc: test LimitWindowToBottomRightCorner03, test childNode
 * @tc.type: FUNC
 */
HWTEST_F(WindowLayoutPolicyTest, LimitWindowToBottomRightCorner03, Function | SmallTest | Level2)
{
    auto displayRect = displayGroupInfo_->GetDisplayRect(defaultDisplayInfo_->GetDisplayId());
    ASSERT_FALSE(WindowHelper::IsEmptyRect(displayRect));
    windowInfo_.winRect_ = {
        .posX_ = static_cast<int32_t>(displayRect.posX_ + displayRect.width_ * 0.5),  // ratio: 0.5
        .posY_ = static_cast<int32_t>(displayRect.posY_ + displayRect.height_ * 0.5), // ratio: 0.5
        .width_ = displayRect.width_,
        .height_ = displayRect.height_
    };
    sptr<WindowNode> parentNode = CreateWindowNode(windowInfo_);
    sptr<WindowNode> node = CreateWindowNode(windowInfo_);
    ASSERT_TRUE(parentNode != nullptr);
    ASSERT_TRUE(node != nullptr);
    parentNode->children_.push_back(node);
    layoutPolicy_->LimitWindowToBottomRightCorner(node);
}

/**
 * @tc.name: UpdateDisplayGroupRect
 * @tc.desc: test UpdateDisplayGroupRect
 * @tc.type: FUNC
 */
HWTEST_F(WindowLayoutPolicyTest, UpdateDisplayGroupRect, Function | SmallTest | Level2)
{
    auto displayRect = displayGroupInfo_->GetDisplayRect(defaultDisplayInfo_->GetDisplayId());
    ASSERT_FALSE(WindowHelper::IsEmptyRect(displayRect));

    Rect newDisplayRect = {
        .posX_ = static_cast<int32_t>(displayRect.posX_ + displayRect.width_),
        .posY_ = displayRect.posY_,
        .width_ = 1920, // width:  1920
        .height_ = 1280 // height: 1080
    };
    auto displayInfo = CreateDisplayInfo(newDisplayRect);
    ASSERT_TRUE(displayInfo != nullptr);
    displayGroupInfo_->AddDisplayInfo(displayInfo);
    layoutPolicy_->UpdateDisplayGroupRect();

    displayGroupInfo_->displayInfosMap_.clear();
    layoutPolicy_->UpdateDisplayGroupRect();
}

/**
 * @tc.name: UpdateDisplayGroupLimitRect
 * @tc.desc: test UpdateDisplayGroupLimitRect
 * @tc.type: FUNC
 */
HWTEST_F(WindowLayoutPolicyTest, UpdateDisplayGroupLimitRect, Function | SmallTest | Level2)
{
    auto displayRect = displayGroupInfo_->GetDisplayRect(defaultDisplayInfo_->GetDisplayId());
    ASSERT_FALSE(WindowHelper::IsEmptyRect(displayRect));
    Rect newDisplayRect = {
        .posX_ = static_cast<int32_t>(displayRect.posX_ + displayRect.width_),
        .posY_ = displayRect.posY_,
        .width_ = 1920, // width:  1920
        .height_ = 1280 // height: 1080
    };
    auto displayInfo = CreateDisplayInfo(newDisplayRect);
    ASSERT_TRUE(displayInfo != nullptr);
    displayGroupInfo_->AddDisplayInfo(displayInfo);
    auto allDisplayRect = displayGroupInfo_->GetAllDisplayRects();
    layoutPolicy_->limitRectMap_ = allDisplayRect;
    layoutPolicy_->UpdateDisplayGroupLimitRect();

    layoutPolicy_->limitRectMap_.clear();
    layoutPolicy_->UpdateDisplayGroupLimitRect();
}

/**
 * @tc.name: UpdateRectInDisplayGroup
 * @tc.desc: test UpdateRectInDisplayGroup, test childNode
 * @tc.type: FUNC
 */
HWTEST_F(WindowLayoutPolicyTest, UpdateRectInDisplayGroup, Function | SmallTest | Level2)
{
    auto displayRect = displayGroupInfo_->GetDisplayRect(defaultDisplayInfo_->GetDisplayId());
    ASSERT_FALSE(WindowHelper::IsEmptyRect(displayRect));
    windowInfo_.winRect_ = {
        .posX_ = static_cast<int32_t>(displayRect.posX_ + displayRect.width_ * 0.5),  // ratio: 0.5
        .posY_ = static_cast<int32_t>(displayRect.posY_ + displayRect.height_ * 0.5), // ratio: 0.5
        .width_ = displayRect.width_,
        .height_ = displayRect.height_
    };
    sptr<WindowNode> parentNode = CreateWindowNode(windowInfo_);
    ASSERT_TRUE(parentNode != nullptr);

    Rect newDisplayRect = {
        .posX_ = static_cast<int32_t>(displayRect.posX_ + displayRect.width_),
        .posY_ = displayRect.posY_,
        .width_ = 1920, // width:  1920
        .height_ = 1280 // height: 1080
    };
    layoutPolicy_->UpdateRectInDisplayGroup(parentNode, displayRect, newDisplayRect);

    // create child node
    sptr<WindowNode> node = CreateWindowNode(windowInfo_);
    ASSERT_TRUE(node != nullptr);
    parentNode->children_.push_back(node);
    layoutPolicy_->UpdateRectInDisplayGroup(parentNode, displayRect, newDisplayRect);
}

/**
 * @tc.name: UpdateMultiDisplayFlag
 * @tc.desc: test UpdateMultiDisplayFlag
 * @tc.type: FUNC
 */
HWTEST_F(WindowLayoutPolicyTest, UpdateMultiDisplayFlag, Function | SmallTest | Level2)
{
    layoutPolicy_->UpdateMultiDisplayFlag();

    auto displayRect = displayGroupInfo_->GetDisplayRect(defaultDisplayInfo_->GetDisplayId());
    ASSERT_FALSE(WindowHelper::IsEmptyRect(displayRect));
    Rect newDisplayRect = {
        .posX_ = static_cast<int32_t>(displayRect.posX_ + displayRect.width_),
        .posY_ = displayRect.posY_,
        .width_ = 1920, // width:  1920
        .height_ = 1280 // height: 1080
    };
    auto displayInfo = CreateDisplayInfo(newDisplayRect);
    ASSERT_TRUE(displayInfo != nullptr);
    displayGroupInfo_->AddDisplayInfo(displayInfo);
    layoutPolicy_->UpdateMultiDisplayFlag();
}

/**
 * @tc.name: UpdateRectInDisplayGroupForAllNodes01
 * @tc.desc: test UpdateRectInDisplayGroupForAllNodes01
 * @tc.type: FUNC
 */
HWTEST_F(WindowLayoutPolicyTest, UpdateRectInDisplayGroupForAllNodes01, Function | SmallTest | Level2)
{
    auto displayRect = displayGroupInfo_->GetDisplayRect(defaultDisplayInfo_->GetDisplayId());
    ASSERT_FALSE(WindowHelper::IsEmptyRect(displayRect));
    Rect newDisplayRect = {
        .posX_ = static_cast<int32_t>(displayRect.posX_ + displayRect.width_),
        .posY_ = displayRect.posY_,
        .width_ = 1920, // width:  1920
        .height_ = 1280 // height: 1080
    };

    layoutPolicy_->UpdateRectInDisplayGroupForAllNodes(0, displayRect, newDisplayRect);

    windowInfo_.winRect_ = {
        .posX_ = static_cast<int32_t>(displayRect.posX_ + displayRect.width_ * 0.5),  // ratio: 0.5
        .posY_ = static_cast<int32_t>(displayRect.posY_ + displayRect.height_ * 0.5), // ratio: 0.5
        .width_ = displayRect.width_,
        .height_ = displayRect.height_
    };
    sptr<WindowNode> node = CreateWindowNode(windowInfo_);
    ASSERT_TRUE(node != nullptr);

    // Add node on display window tree
    layoutPolicy_->displayGroupWindowTree_[0][WindowRootNodeType::APP_WINDOW_NODE]->push_back(node);
    layoutPolicy_->UpdateRectInDisplayGroupForAllNodes(0, displayRect, newDisplayRect);

    node->isShowingOnMultiDisplays_ = true;
    layoutPolicy_->UpdateRectInDisplayGroupForAllNodes(0, displayRect, newDisplayRect);
}

/**
 * @tc.name: UpdateRectInDisplayGroupForAllNodes02
 * @tc.desc: test UpdateRectInDisplayGroupForAllNodes
 * @tc.type: FUNC
 */
HWTEST_F(WindowLayoutPolicyTest, UpdateRectInDisplayGroupForAllNodes02, Function | SmallTest | Level2)
{
    auto displayRect = displayGroupInfo_->GetDisplayRect(defaultDisplayInfo_->GetDisplayId());
    ASSERT_FALSE(WindowHelper::IsEmptyRect(displayRect));
    Rect newDisplayRect = {
        .posX_ = static_cast<int32_t>(displayRect.posX_ + displayRect.width_),
        .posY_ = displayRect.posY_,
        .width_ = 1920, // width:  1920
        .height_ = 1280 // height: 1080
    };

    windowInfo_.winRect_ = {
        .posX_ = static_cast<int32_t>(displayRect.posX_ + displayRect.width_ * 0.5),  // ratio: 0.5
        .posY_ = static_cast<int32_t>(displayRect.posY_ + displayRect.height_ * 0.5), // ratio: 0.5
        .width_ = displayRect.width_,
        .height_ = displayRect.height_
    };

    // Add app node on display window tree
    sptr<WindowNode> node = CreateWindowNode(windowInfo_);
    ASSERT_TRUE(node != nullptr);
    node->isShowingOnMultiDisplays_ = true;
    layoutPolicy_->displayGroupWindowTree_[0][WindowRootNodeType::APP_WINDOW_NODE]->push_back(node);
    layoutPolicy_->UpdateRectInDisplayGroupForAllNodes(0, displayRect, newDisplayRect);

    // Add above node on display window tree
    windowInfo_.winType_ = WindowType::WINDOW_TYPE_FLOAT_CAMERA;
    sptr<WindowNode> aboveNode = CreateWindowNode(windowInfo_);
    ASSERT_TRUE(aboveNode != nullptr);
    layoutPolicy_->displayGroupWindowTree_[0][WindowRootNodeType::ABOVE_WINDOW_NODE]->push_back(aboveNode);
    layoutPolicy_->UpdateRectInDisplayGroupForAllNodes(0, displayRect, newDisplayRect);
}

/**
 * @tc.name: UpdateDisplayRectAndDisplayGroupInfo
 * @tc.desc: test UpdateDisplayRectAndDisplayGroupInfo
 * @tc.type: FUNC
 */
HWTEST_F(WindowLayoutPolicyTest, UpdateDisplayRectAndDisplayGroupInfo, Function | SmallTest | Level2)
{
    std::map<DisplayId, Rect> displayRectMap = {};
    layoutPolicy_->UpdateDisplayRectAndDisplayGroupInfo(displayRectMap);
    displayRectMap = displayGroupInfo_->GetAllDisplayRects();
    layoutPolicy_->UpdateDisplayRectAndDisplayGroupInfo(displayRectMap);
}

/**
 * @tc.name: ProcessDisplayCreate
 * @tc.desc: test ProcessDisplayCreate
 * @tc.type: FUNC
 */
HWTEST_F(WindowLayoutPolicyTest, ProcessDisplayCreate, Function | SmallTest | Level2)
{
    std::map<DisplayId, Rect> newDisplayRectMap = {};
    layoutPolicy_->ProcessDisplayCreate(0, newDisplayRectMap);

    layoutPolicy_->ProcessDisplayCreate(1, newDisplayRectMap);

    auto displayRect = displayGroupInfo_->GetDisplayRect(defaultDisplayInfo_->GetDisplayId());
    ASSERT_FALSE(WindowHelper::IsEmptyRect(displayRect));
    newDisplayRectMap.insert(std::make_pair(0, displayRect));
    Rect newDisplayRect = {
        .posX_ = static_cast<int32_t>(displayRect.posX_ + displayRect.width_),
        .posY_ = displayRect.posY_,
        .width_ = 1920, // width:  1920
        .height_ = 1280 // height: 1080
    };
    newDisplayRectMap.insert(std::make_pair(1, newDisplayRect));
    layoutPolicy_->ProcessDisplayCreate(1, newDisplayRectMap);

    auto displayInfo = CreateDisplayInfo(newDisplayRect);
    ASSERT_TRUE(displayInfo != nullptr);
}

/**
 * @tc.name: ProcessDisplayDestroy
 * @tc.desc: test ProcessDisplayDestroy
 * @tc.type: FUNC
 */
HWTEST_F(WindowLayoutPolicyTest, ProcessDisplayDestroy, Function | SmallTest | Level2)
{
    std::map<DisplayId, Rect> newDisplayRectMap = {};
    layoutPolicy_->ProcessDisplayDestroy(0, newDisplayRectMap);

    layoutPolicy_->ProcessDisplayDestroy(1, newDisplayRectMap);

    auto displayRect = displayGroupInfo_->GetDisplayRect(defaultDisplayInfo_->GetDisplayId());
    ASSERT_FALSE(WindowHelper::IsEmptyRect(displayRect));
}

/**
 * @tc.name: ProcessDisplaySizeChangeOrRotation
 * @tc.desc: test ProcessDisplaySizeChangeOrRotation
 * @tc.type: FUNC
 */
HWTEST_F(WindowLayoutPolicyTest, ProcessDisplaySizeChangeOrRotation, Function | SmallTest | Level2)
{
    std::map<DisplayId, Rect> newDisplayRectMap = {};
    layoutPolicy_->ProcessDisplayDestroy(0, newDisplayRectMap);

    layoutPolicy_->ProcessDisplayDestroy(1, newDisplayRectMap);

    auto displayRect = displayGroupInfo_->GetDisplayRect(defaultDisplayInfo_->GetDisplayId());
    ASSERT_FALSE(WindowHelper::IsEmptyRect(displayRect));
}

/**
 * @tc.name: NotifyAnimationSizeChangeIfNeeded
 * @tc.desc: test NotifyAnimationSizeChangeIfNeeded
 * @tc.type: FUNC
 */
HWTEST_F(WindowLayoutPolicyTest, NotifyAnimationSizeChangeIfNeeded, Function | SmallTest | Level2)
{
    layoutPolicy_->NotifyAnimationSizeChangeIfNeeded();
}

/**
 * @tc.name: LayoutWindowNode
 * @tc.desc: test LayoutWindowNode
 * @tc.type: FUNC
 */
HWTEST_F(WindowLayoutPolicyTest, LayoutWindowNode, Function | SmallTest | Level2)
{
    sptr<WindowNode> node = nullptr;
    layoutPolicy_->LayoutWindowNode(node);

    node = CreateWindowNode(windowInfo_);
    ASSERT_TRUE(node != nullptr);
    layoutPolicy_->LayoutWindowNode(node);

    node->parent_ = container_->appWindowNode_;
    layoutPolicy_->LayoutWindowNode(node);

    node->currentVisibility_ = true;
    layoutPolicy_->LayoutWindowNode(node);

    node->GetWindowProperty()->SetWindowType(WindowType::WINDOW_TYPE_STATUS_BAR);
    layoutPolicy_->LayoutWindowNode(node);

    // create child node
    sptr<WindowNode> child = CreateWindowNode(windowInfo_);
    ASSERT_TRUE(child != nullptr);
    node->children_.push_back(child);
    layoutPolicy_->LayoutWindowNode(node);
}

/**
 * @tc.name: UpdateClientRect
 * @tc.desc: test UpdateClientRect
 * @tc.type: FUNC
 */
HWTEST_F(WindowLayoutPolicyTest, UpdateClientRect, Function | SmallTest | Level2)
{
    sptr<WindowNode> node = CreateWindowNode(windowInfo_);
    ASSERT_TRUE(node != nullptr);
    layoutPolicy_->UpdateClientRect(node->GetWindowRect(), node, WindowSizeChangeReason::MOVE);
}

/**
 * @tc.name: UpdateClientRectAndResetReason
 * @tc.desc: test UpdateClientRectAndResetReason
 * @tc.type: FUNC
 */
HWTEST_F(WindowLayoutPolicyTest, UpdateClientRectAndResetReason, Function | SmallTest | Level2)
{
    sptr<WindowNode> node = CreateWindowNode(windowInfo_);
    ASSERT_TRUE(node != nullptr);
    layoutPolicy_->UpdateClientRectAndResetReason(node, node->GetWindowRect());
}

/**
 * @tc.name: RemoveWindowNode
 * @tc.desc: test RemoveWindowNode
 * @tc.type: FUNC
 */
HWTEST_F(WindowLayoutPolicyTest, RemoveWindowNode, Function | SmallTest | Level2)
{
    sptr<WindowNode> node = CreateWindowNode(windowInfo_);
    ASSERT_TRUE(node != nullptr);
    layoutPolicy_->RemoveWindowNode(node);

    windowInfo_.winType_ = WindowType::WINDOW_TYPE_NAVIGATION_BAR;
    sptr<WindowNode> avoidNode = CreateWindowNode(windowInfo_);
    ASSERT_TRUE(avoidNode != nullptr);
    layoutPolicy_->RemoveWindowNode(avoidNode);

    windowInfo_.winType_ = WindowType::WINDOW_TYPE_DOCK_SLICE;
    sptr<WindowNode> dividerNode = CreateWindowNode(windowInfo_);
    ASSERT_TRUE(dividerNode != nullptr);
    layoutPolicy_->RemoveWindowNode(dividerNode);
}

/**
 * @tc.name: UpdateWindowNode
 * @tc.desc: test UpdateWindowNode
 * @tc.type: FUNC
 */
HWTEST_F(WindowLayoutPolicyTest, UpdateWindowNode, Function | SmallTest | Level2)
{
    auto displayRect = displayGroupInfo_->GetDisplayRect(defaultDisplayInfo_->GetDisplayId());
    ASSERT_FALSE(WindowHelper::IsEmptyRect(displayRect));
    windowInfo_.winRect_ = { 0, static_cast<uint32_t>(displayRect.height_ * 0.9), // ratio: 0.9
        displayRect.width_, static_cast<uint32_t>(displayRect.height_ * 0.1) };   // ratio: 0.1
    windowInfo_.winType_ = WindowType::WINDOW_TYPE_NAVIGATION_BAR;
    sptr<WindowNode> avoidNode = CreateWindowNode(windowInfo_);
    ASSERT_TRUE(avoidNode != nullptr);
    layoutPolicy_->UpdateWindowNode(avoidNode);
}

/**
 * @tc.name: ComputeDecoratedRequestRect
 * @tc.desc: test ComputeDecoratedRequestRect
 * @tc.type: FUNC
 */
HWTEST_F(WindowLayoutPolicyTest, ComputeDecoratedRequestRect, Function | SmallTest | Level2)
{
    sptr<WindowNode> node = CreateWindowNode(windowInfo_);
    ASSERT_TRUE(node != nullptr);
    layoutPolicy_->ComputeDecoratedRequestRect(node);

    sptr<WindowNode> node1 = CreateWindowNode(windowInfo_);
    ASSERT_TRUE(node1 != nullptr);
    windowInfo_.reason_ = WindowSizeChangeReason::MOVE;
    layoutPolicy_->ComputeDecoratedRequestRect(node1);

    windowInfo_.decorEnable_ = true;
    sptr<WindowNode> node2 = CreateWindowNode(windowInfo_);
    ASSERT_TRUE(node2 != nullptr);
    layoutPolicy_->ComputeDecoratedRequestRect(node2);
}

/**
 * @tc.name: CalcAndSetNodeHotZone
 * @tc.desc: test CalcAndSetNodeHotZone
 * @tc.type: FUNC
 */
HWTEST_F(WindowLayoutPolicyTest, CalcAndSetNodeHotZone, Function | SmallTest | Level2)
{
    sptr<WindowNode> node = CreateWindowNode(windowInfo_);
    ASSERT_TRUE(node != nullptr);
    layoutPolicy_->CalcAndSetNodeHotZone(node->GetWindowRect(), node);

    layoutPolicy_->UpdateLayoutRect(node);
    layoutPolicy_->CalcAndSetNodeHotZone(node->GetWindowRect(), node);

    layoutPolicy_->CalcAndSetNodeHotZone(node->GetWindowRect(), node);
}

/**
 * @tc.name: FixWindowSizeByRatioIfDragBeyondLimitRegion01
 * @tc.desc: test FixWindowSizeByRatioIfDragBeyondLimitRegion01
 * @tc.type: FUNC
 */
HWTEST_F(WindowLayoutPolicyTest, FixWindowSizeByRatioIfDragBeyondLimitRegion01, Function | SmallTest | Level2)
{
    sptr<WindowNode> node = CreateWindowNode(windowInfo_);
    ASSERT_TRUE(node != nullptr);
    WindowSizeLimits sizeLimits = { 400, 400, 400, 400, 2.0, 2.0 }; // sizeLimits: 400, 400, 400, 400, 2.0, 2.0
    node->SetWindowUpdatedSizeLimits(sizeLimits);
    Rect finalRect;
    layoutPolicy_->FixWindowSizeByRatioIfDragBeyondLimitRegion(node, finalRect);

    sizeLimits.maxWidth_ = 800;  // maxWidth: 800
    node->SetWindowUpdatedSizeLimits(sizeLimits);
    layoutPolicy_->FixWindowSizeByRatioIfDragBeyondLimitRegion(node, finalRect);

    sizeLimits.maxWidth_ = 400;  // maxWidth:  400
    sizeLimits.maxHeight_ = 800; // maxHeight: 800
    node->SetWindowUpdatedSizeLimits(sizeLimits);
    layoutPolicy_->FixWindowSizeByRatioIfDragBeyondLimitRegion(node, finalRect);

    sizeLimits.maxWidth_ = 800;  // maxWidth:  800
    sizeLimits.maxHeight_ = 800; // maxHeight: 800
    node->SetWindowUpdatedSizeLimits(sizeLimits);
    layoutPolicy_->FixWindowSizeByRatioIfDragBeyondLimitRegion(node, finalRect);

    sizeLimits.maxWidth_ = 800;  // maxWidth:  800
    sizeLimits.maxHeight_ = 800; // maxHeight: 800
    node->SetWindowUpdatedSizeLimits(sizeLimits);

    auto newRect = node->GetWindowRect();
    newRect.height_ = 400;       // maxHeight: 400
    node->SetWindowRect(newRect);
    layoutPolicy_->FixWindowSizeByRatioIfDragBeyondLimitRegion(node, finalRect);

    newRect = { 200, 200, 500, 200 }; // rect: 200, 200, 500, 200
    node->SetWindowRect(newRect);
    layoutPolicy_->FixWindowSizeByRatioIfDragBeyondLimitRegion(node, finalRect);

    newRect = { 200, 200, 100, 200 }; // rect: 200, 200, 100, 200
    node->SetWindowRect(newRect);
    layoutPolicy_->FixWindowSizeByRatioIfDragBeyondLimitRegion(node, finalRect);
}

/**
 * @tc.name: FixWindowSizeByRatioIfDragBeyondLimitRegion02
 * @tc.desc: test FixWindowSizeByRatioIfDragBeyondLimitRegion02
 * @tc.type: FUNC
 */
HWTEST_F(WindowLayoutPolicyTest, FixWindowSizeByRatioIfDragBeyondLimitRegion02, Function | SmallTest | Level2)
{
    sptr<WindowNode> node = CreateWindowNode(windowInfo_);
    ASSERT_TRUE(node != nullptr);
    WindowSizeLimits sizeLimits = { 800, 800, 400, 400, 2.0, 1.0 }; // sizeLimits: 800, 800, 400, 400, 2.0, 1.0
    node->SetWindowUpdatedSizeLimits(sizeLimits);

    Rect newRect = { 200, 200, 300, 200 }; // rect: 200, 200, 300, 200
    Rect finalRect;
    node->SetWindowRect(newRect);
    layoutPolicy_->FixWindowSizeByRatioIfDragBeyondLimitRegion(node, finalRect);

    sizeLimits.minRatio_ = 1.0; // ratio: 1.0
    sizeLimits.maxRatio_ = 1.0; // ratio: 1.0
    node->SetWindowUpdatedSizeLimits(sizeLimits);
    layoutPolicy_->FixWindowSizeByRatioIfDragBeyondLimitRegion(node, finalRect);

    sizeLimits.minRatio_ = 2.0; // ratio: 2.0
    sizeLimits.maxRatio_ = 2.0; // ratio: 2.0
    node->SetWindowUpdatedSizeLimits(sizeLimits);
    layoutPolicy_->FixWindowSizeByRatioIfDragBeyondLimitRegion(node, finalRect);

    newRect = { 200, 200, 400, 200 }; // rect: 200, 200, 400, 200
    node->SetWindowRect(newRect);
    layoutPolicy_->FixWindowSizeByRatioIfDragBeyondLimitRegion(node, finalRect);

    auto displayRect = displayGroupInfo_->GetDisplayRect(defaultDisplayInfo_->GetDisplayId());
    ASSERT_FALSE(WindowHelper::IsEmptyRect(displayRect));
    layoutPolicy_->limitRectMap_[0] = displayRect;
}

/**
 * @tc.name: FixWindowSizeByRatioIfDragBeyondLimitRegion03
 * @tc.desc: test FixWindowSizeByRatioIfDragBeyondLimitRegion03
 * @tc.type: FUNC
 */
HWTEST_F(WindowLayoutPolicyTest, FixWindowSizeByRatioIfDragBeyondLimitRegion03, Function | SmallTest | Level2)
{
    sptr<WindowNode> node = CreateWindowNode(windowInfo_);
    ASSERT_TRUE(node != nullptr);
    WindowSizeLimits sizeLimits = { 800, 800, 400, 400, 2.0, 1.0 }; // sizeLimits: 800, 800, 400, 400, 2.0, 1.0

    node->SetWindowUpdatedSizeLimits(sizeLimits);

    Rect newRect = { 200, 200, 300, 200 }; // rect: 200, 200, 300, 200
    node->SetWindowRect(newRect);

    auto displayRect = displayGroupInfo_->GetDisplayRect(defaultDisplayInfo_->GetDisplayId());
    ASSERT_FALSE(WindowHelper::IsEmptyRect(displayRect));
    layoutPolicy_->limitRectMap_[0] = displayRect;

    windowInfo_.winType_ = WindowType::WINDOW_TYPE_LAUNCHER_DOCK;
    sptr<WindowNode> dockNode = CreateWindowNode(windowInfo_);
    layoutPolicy_->displayGroupWindowTree_[0][WindowRootNodeType::ABOVE_WINDOW_NODE]->push_back(dockNode);

    Rect dockWinRect = { 200, 200, 50, 20 }; // rect: 200, 200, 50, 20
    Rect finalRect;
    dockNode->SetWindowRect(dockWinRect);
    layoutPolicy_->FixWindowSizeByRatioIfDragBeyondLimitRegion(node, finalRect);

    dockWinRect = { 200, 200, 50, (displayRect.height_ - static_cast<uint32_t>(200)) }; // param: 200, 50
    dockNode->SetWindowRect(dockWinRect);
    layoutPolicy_->FixWindowSizeByRatioIfDragBeyondLimitRegion(node, finalRect);

    dockWinRect = { 200, 200, 20, 50 }; // param: 200, 200, 20, 50
    dockNode->SetWindowRect(dockWinRect);
    layoutPolicy_->FixWindowSizeByRatioIfDragBeyondLimitRegion(node, finalRect);

    dockWinRect = { 0, 200, 20, 50 };   // param: 0, 200, 20, 50
    dockNode->SetWindowRect(dockWinRect);
    layoutPolicy_->FixWindowSizeByRatioIfDragBeyondLimitRegion(node, finalRect);

    dockWinRect = { 200, 200, (displayRect.width_ - static_cast<uint32_t>(200)), 50 }; // param: 200, 50
    dockNode->SetWindowRect(dockWinRect);
    layoutPolicy_->FixWindowSizeByRatioIfDragBeyondLimitRegion(node, finalRect);

    dockWinRect = { 200, 200, (displayRect.width_ - static_cast<uint32_t>(100)), 50 }; // param: 200, 100, 50
    dockNode->SetWindowRect(dockWinRect);
    layoutPolicy_->FixWindowSizeByRatioIfDragBeyondLimitRegion(node, finalRect);
}

/**
 * @tc.name: FixWindowSizeByRatioIfDragBeyondLimitRegion04
 * @tc.desc: test FixWindowSizeByRatioIfDragBeyondLimitRegion04
 * @tc.type: FUNC
 */
HWTEST_F(WindowLayoutPolicyTest, FixWindowSizeByRatioIfDragBeyondLimitRegion04, Function | SmallTest | Level2)
{
    sptr<WindowNode> node = CreateWindowNode(windowInfo_);
    ASSERT_TRUE(node != nullptr);
    WindowSizeLimits sizeLimits = { 800, 800, 400, 400, 2.0, 1.0 }; // sizeLimits: 800, 800, 400, 400, 2.0, 1.0
    node->SetWindowUpdatedSizeLimits(sizeLimits);

    Rect newRect = { 200, 200, 300, 200 }; // rect: 200, 200, 300, 200
    node->SetWindowRect(newRect);

    auto displayRect = displayGroupInfo_->GetDisplayRect(defaultDisplayInfo_->GetDisplayId());
    ASSERT_FALSE(WindowHelper::IsEmptyRect(displayRect));
    layoutPolicy_->limitRectMap_[0] = displayRect;
    windowInfo_.winType_ = WindowType::WINDOW_TYPE_LAUNCHER_DOCK;
    sptr<WindowNode> dockNode = CreateWindowNode(windowInfo_);
    layoutPolicy_->displayGroupWindowTree_[0][WindowRootNodeType::ABOVE_WINDOW_NODE]->push_back(dockNode);

    Rect dockWinRect = { 200, (displayRect.height_ * 0.9), // param: 200, 0.9,
        50, ((displayRect.height_ * 0.1)) };               // param: 50, 0.1
    dockNode->SetWindowRect(dockWinRect);
    Rect finalRect;
    layoutPolicy_->FixWindowSizeByRatioIfDragBeyondLimitRegion(node, finalRect);

    float virtualPixelRatio = layoutPolicy_->GetVirtualPixelRatio(node->GetDisplayId());
    uint32_t windowTitleBarH = static_cast<uint32_t>(WINDOW_TITLE_BAR_HEIGHT * virtualPixelRatio);
    int32_t limitMaxPosX = displayRect.posX_ + static_cast<int32_t>(displayRect.width_ - windowTitleBarH);

    newRect = { limitMaxPosX, 200, 300, 200 }; // param: 200, 300
    layoutPolicy_->FixWindowSizeByRatioIfDragBeyondLimitRegion(node, finalRect);

    newRect = { limitMaxPosX, 200, 200, 200 }; // param: 200
    layoutPolicy_->FixWindowSizeByRatioIfDragBeyondLimitRegion(node, finalRect);
}

/**
 * @tc.name: FixWindowSizeByRatioIfDragBeyondLimitRegion05
 * @tc.desc: test FixWindowSizeByRatioIfDragBeyondLimitRegion05
 * @tc.type: FUNC
 */
HWTEST_F(WindowLayoutPolicyTest, FixWindowSizeByRatioIfDragBeyondLimitRegion05, Function | SmallTest | Level2)
{
    sptr<WindowNode> node = CreateWindowNode(windowInfo_);
    ASSERT_TRUE(node != nullptr);
    WindowSizeLimits sizeLimits = { 800, 800, 400, 400, 2.0, 1.0 }; // sizeLimits: 800, 800, 400, 400, 2.0, 1.0
    node->SetWindowUpdatedSizeLimits(sizeLimits);

    Rect newRect = { 200, 200, 300, 200 }; // rect: 200, 200, 300, 200
    node->SetWindowRect(newRect);

    auto displayRect = displayGroupInfo_->GetDisplayRect(defaultDisplayInfo_->GetDisplayId());
    ASSERT_FALSE(WindowHelper::IsEmptyRect(displayRect));
    layoutPolicy_->limitRectMap_[0] = displayRect;
    windowInfo_.winType_ = WindowType::WINDOW_TYPE_LAUNCHER_DOCK;
    sptr<WindowNode> dockNode = CreateWindowNode(windowInfo_);
    ASSERT_TRUE(dockNode != nullptr);
    layoutPolicy_->displayGroupWindowTree_[0][WindowRootNodeType::ABOVE_WINDOW_NODE]->push_back(dockNode);

    Rect dockWinRect = {
        0,
        static_cast<uint32_t>(displayRect.height_ * 0.9), // ratio: 0.9
        static_cast<uint32_t>(displayRect.height_ * 0.1), // ratio: 0.1
        static_cast<uint32_t>(displayRect.height_ * 0.1)  // ratio: 0.1
    };
    dockNode->SetWindowRect(dockWinRect);
    Rect finalRect;
    layoutPolicy_->FixWindowSizeByRatioIfDragBeyondLimitRegion(node, finalRect);

    float virtualPixelRatio = layoutPolicy_->GetVirtualPixelRatio(node->GetDisplayId());
    uint32_t windowTitleBarH = static_cast<uint32_t>(WINDOW_TITLE_BAR_HEIGHT * virtualPixelRatio);
    int32_t limitMinPosX = displayRect.posX_ + static_cast<int32_t>(windowTitleBarH);

    newRect = { limitMinPosX, 200, 300, 200 }; // rect: 200, 300, 200
    layoutPolicy_->FixWindowSizeByRatioIfDragBeyondLimitRegion(node, finalRect);

    newRect = { limitMinPosX, 200, 200, 200 }; // rect: 200, 200, 200
    layoutPolicy_->FixWindowSizeByRatioIfDragBeyondLimitRegion(node, finalRect);
}

/**
 * @tc.name: GetSystemSizeLimits
 * @tc.desc: test GetSystemSizeLimits
 * @tc.type: FUNC
 */
HWTEST_F(WindowLayoutPolicyTest, GetSystemSizeLimits, Function | SmallTest | Level2)
{
    auto displayRect = displayGroupInfo_->GetDisplayRect(defaultDisplayInfo_->GetDisplayId());
    ASSERT_FALSE(WindowHelper::IsEmptyRect(displayRect));

    sptr<WindowNode> node1 = CreateWindowNode(windowInfo_);
    ASSERT_TRUE(node1 != nullptr);
    static_cast<void>(layoutPolicy_->GetSystemSizeLimits(node1, displayRect, 1.0)); // ratio: 1.0

    windowInfo_.winType_ = WindowType::WINDOW_TYPE_FLOAT_CAMERA;
    sptr<WindowNode> node2 = CreateWindowNode(windowInfo_);
    ASSERT_TRUE(node2 != nullptr);
    static_cast<void>(layoutPolicy_->GetSystemSizeLimits(node2, displayRect, 1.0)); // ratio: 1.0
}

/**
 * @tc.name: UpdateWindowSizeLimits
 * @tc.desc: test UpdateWindowSizeLimits
 * @tc.type: FUNC
 */
HWTEST_F(WindowLayoutPolicyTest, UpdateWindowSizeLimits, Function | SmallTest | Level2)
{
    auto displayRect = displayGroupInfo_->GetDisplayRect(defaultDisplayInfo_->GetDisplayId());
    ASSERT_FALSE(WindowHelper::IsEmptyRect(displayRect));

    sptr<WindowNode> node1 = CreateWindowNode(windowInfo_);
    ASSERT_TRUE(node1 != nullptr);
    static_cast<void>(layoutPolicy_->GetSystemSizeLimits(node1, displayRect, 1.0)); // ratio: 1.0

    windowInfo_.winType_ = WindowType::WINDOW_TYPE_FLOAT_CAMERA;
    sptr<WindowNode> node2 = CreateWindowNode(windowInfo_);
    ASSERT_TRUE(node2 != nullptr);
    static_cast<void>(layoutPolicy_->GetSystemSizeLimits(node2, displayRect, 1.0)); // ratio: 1.0
}

/**
 * @tc.name: UpdateFloatingWindowSizeForStretchableWindow04
 * @tc.desc: test UpdateFloatingWindowSizeForStretchableWindow04
 * @tc.type: FUNC
 */
HWTEST_F(WindowLayoutPolicyTest, UpdateFloatingWindowSizeForStretchableWindow04, Function | SmallTest | Level2)
{
    auto displayRect = displayGroupInfo_->GetDisplayRect(defaultDisplayInfo_->GetDisplayId());
    ASSERT_FALSE(WindowHelper::IsEmptyRect(displayRect));

    sptr<WindowNode> node = CreateWindowNode(windowInfo_);
    ASSERT_TRUE(node != nullptr);
    Rect winRect = { 0, 0, 0, 0};
    layoutPolicy_->UpdateFloatingWindowSizeForStretchableWindow(node, displayRect, winRect);

    node->SetWindowSizeChangeReason(WindowSizeChangeReason::DRAG);
    layoutPolicy_->UpdateFloatingWindowSizeForStretchableWindow(node, displayRect, winRect);

    winRect = { 0, 0, 40, 0 }; // width: 40
    node->SetOriginRect(winRect);
    layoutPolicy_->UpdateFloatingWindowSizeForStretchableWindow(node, displayRect, winRect);

    winRect = { 0, 0, 0, 40 }; // height: 40
    node->SetOriginRect(winRect);
    layoutPolicy_->UpdateFloatingWindowSizeForStretchableWindow(node, displayRect, winRect);

    winRect = { 0, 0, 40, 40 }; // width/height: 40
    node->SetOriginRect(winRect);
    layoutPolicy_->UpdateFloatingWindowSizeForStretchableWindow(node, displayRect, winRect);

    node->SetDragType(DragType::DRAG_LEFT_OR_RIGHT);
    layoutPolicy_->UpdateFloatingWindowSizeForStretchableWindow(node, displayRect, winRect);

    node->SetDragType(DragType::DRAG_BOTTOM_OR_TOP);
    layoutPolicy_->UpdateFloatingWindowSizeForStretchableWindow(node, displayRect, winRect);

    node->SetDragType(DragType::DRAG_LEFT_TOP_CORNER);
    layoutPolicy_->UpdateFloatingWindowSizeForStretchableWindow(node, displayRect, winRect);

    node->SetDragType(DragType::DRAG_RIGHT_TOP_CORNER);
    layoutPolicy_->UpdateFloatingWindowSizeForStretchableWindow(node, displayRect, winRect);
}

/**
 * @tc.name: UpdateFloatingWindowSizeForStretchableWindow05
 * @tc.desc: test UpdateFloatingWindowSizeForStretchableWindow05
 * @tc.type: FUNC
 */
HWTEST_F(WindowLayoutPolicyTest, UpdateFloatingWindowSizeForStretchableWindow05, Function | SmallTest | Level2)
{
    auto displayRect = displayGroupInfo_->GetDisplayRect(defaultDisplayInfo_->GetDisplayId());
    ASSERT_FALSE(WindowHelper::IsEmptyRect(displayRect));

    sptr<WindowNode> node = CreateWindowNode(windowInfo_);
    ASSERT_TRUE(node != nullptr);
    WindowSizeLimits sizeLimits = { 800, 800, 400, 400, 2.0, 1.0 }; // sizeLimits: 800, 800, 400, 400, 2.0, 1.0
    node->SetWindowUpdatedSizeLimits(sizeLimits);

    Rect winRect = { 0, 0, 400, 400 }; // width/height: 400
    node->SetOriginRect(winRect);
    layoutPolicy_->UpdateFloatingWindowSizeForStretchableWindow(node, displayRect, winRect);

    winRect = { 0, 0, 300, 300 }; // width/height: 300
    node->SetOriginRect(winRect);
    layoutPolicy_->UpdateFloatingWindowSizeForStretchableWindow(node, displayRect, winRect);

    winRect = { 0, 0, 500, 500 }; // width/height: 500
    node->SetOriginRect(winRect);
    layoutPolicy_->UpdateFloatingWindowSizeForStretchableWindow(node, displayRect, winRect);
}

/**
 * @tc.name: UpdateFloatingWindowSizeBySizeLimits
 * @tc.desc: test UpdateFloatingWindowSizeBySizeLimits
 * @tc.type: FUNC
 */
HWTEST_F(WindowLayoutPolicyTest, UpdateFloatingWindowSizeBySizeLimits, Function | SmallTest | Level2)
{
    auto displayRect = displayGroupInfo_->GetDisplayRect(defaultDisplayInfo_->GetDisplayId());
    ASSERT_FALSE(WindowHelper::IsEmptyRect(displayRect));

    sptr<WindowNode> node = CreateWindowNode(windowInfo_);
    ASSERT_TRUE(node != nullptr);
    WindowSizeLimits sizeLimits = { 800, 400, 800, 400, 2.0, 1.0 }; // sizeLimits: 800, 400, 800, 400, 2.0, 1.0
    node->SetWindowUpdatedSizeLimits(sizeLimits);
    Rect winRect = { 0, 0, 400, 400 }; // width/height: 400
    layoutPolicy_->UpdateFloatingWindowSizeBySizeLimits(node, displayRect, winRect);

    sizeLimits = { 800, 800, 400, 400, 2.0, 1.0 }; // sizeLimits: 800, 800, 400, 400, 2.0, 1.0
    node->SetWindowUpdatedSizeLimits(sizeLimits);
    winRect = { 0, 0, 400, 400 }; // width/height: 400
    layoutPolicy_->UpdateFloatingWindowSizeBySizeLimits(node, displayRect, winRect);

    sizeLimits = { 800, 500, 800, 400, 2.0, 1.0 }; // sizeLimits: 800, 500, 800, 400, 2.0, 1.0
    node->SetWindowUpdatedSizeLimits(sizeLimits);
    layoutPolicy_->UpdateFloatingWindowSizeBySizeLimits(node, displayRect, winRect);

    sizeLimits = { 800, 400, 600, 400, 2.0, 1.0 }; // sizeLimits: 800, 400, 600, 400, 2.0, 1.0
    node->SetWindowUpdatedSizeLimits(sizeLimits);
    layoutPolicy_->UpdateFloatingWindowSizeBySizeLimits(node, displayRect, winRect);

    sizeLimits = { 800, 400, 800, 400, 2.0, 1.0 }; // sizeLimits: 800, 400, 800, 400, 2.0, 1.0
    node->SetWindowUpdatedSizeLimits(sizeLimits);
    layoutPolicy_->UpdateFloatingWindowSizeBySizeLimits(node, displayRect, winRect);

    sizeLimits = { 800, 800, 400, 400, 2.0, 1.0 }; // sizeLimits: 800, 800, 400, 400, 2.0, 1.0
    node->SetWindowUpdatedSizeLimits(sizeLimits);
    node->SetDragType(DragType::DRAG_BOTTOM_OR_TOP);
    layoutPolicy_->UpdateFloatingWindowSizeBySizeLimits(node, displayRect, winRect);

    node->SetDragType(DragType::DRAG_LEFT_OR_RIGHT);
    layoutPolicy_->UpdateFloatingWindowSizeBySizeLimits(node, displayRect, winRect);

    node->GetWindowProperty()->SetWindowType(WindowType::WINDOW_TYPE_DRAGGING_EFFECT);
    layoutPolicy_->UpdateFloatingWindowSizeBySizeLimits(node, displayRect, winRect);
}

/**
 * @tc.name: LimitFloatingWindowSize
 * @tc.desc: test LimitFloatingWindowSize
 * @tc.type: FUNC
 */
HWTEST_F(WindowLayoutPolicyTest, LimitFloatingWindowSize, Function | SmallTest | Level2)
{
    auto displayRect = displayGroupInfo_->GetDisplayRect(defaultDisplayInfo_->GetDisplayId());
    ASSERT_FALSE(WindowHelper::IsEmptyRect(displayRect));

    sptr<WindowNode> node = CreateWindowNode(windowInfo_);
    ASSERT_TRUE(node != nullptr);
    Rect winRect = { 0, 0, 400, 400 }; // width/height: 400
    layoutPolicy_->LimitFloatingWindowSize(node, displayRect, winRect);

    node->SetWindowSizeChangeReason(WindowSizeChangeReason::DRAG);
    node->SetWindowRect(winRect);
    layoutPolicy_->LimitFloatingWindowSize(node, displayRect, winRect);

    Rect newRect = { 10, 0, 400, 400 }; // window rect: 10, 0, 400, 400
    layoutPolicy_->LimitFloatingWindowSize(node, displayRect, newRect);

    newRect = { 0, 10, 400, 400 }; // window rect: 0, 10, 400, 400
    layoutPolicy_->LimitFloatingWindowSize(node, displayRect, newRect);

    newRect = { 10, 10, 400, 400 }; // window rect: 10, 10, 400, 400
    layoutPolicy_->LimitFloatingWindowSize(node, displayRect, newRect);

    node->SetWindowMode(WindowMode::WINDOW_MODE_FULLSCREEN);
    layoutPolicy_->LimitFloatingWindowSize(node, displayRect, winRect);

    node->GetWindowProperty()->SetWindowType(WindowType::WINDOW_TYPE_APP_COMPONENT);
    layoutPolicy_->LimitFloatingWindowSize(node, displayRect, winRect);
}

/**
 * @tc.name: LimitMainFloatingWindowPosition
 * @tc.desc: test LimitMainFloatingWindowPosition
 * @tc.type: FUNC
 */
HWTEST_F(WindowLayoutPolicyTest, LimitMainFloatingWindowPosition, Function | SmallTest | Level2)
{
    sptr<WindowNode> node = CreateWindowNode(windowInfo_);
    ASSERT_TRUE(node != nullptr);
    Rect winRect = { 0, 0, 400, 400 }; // width/height: 400
    node->SetWindowRect(winRect);
    node->SetRequestRect(winRect);
    winRect.posX_ = 20; // posX: 20
    layoutPolicy_->LimitMainFloatingWindowPosition(node, winRect);

    node->SetWindowSizeChangeReason(WindowSizeChangeReason::DRAG);
    node->SetWindowRect(winRect);
    layoutPolicy_->LimitMainFloatingWindowPosition(node, winRect);

    node->SetWindowMode(WindowMode::WINDOW_MODE_FULLSCREEN);
    layoutPolicy_->LimitMainFloatingWindowPosition(node, winRect);
}

/**
 * @tc.name: LimitWindowPositionWhenDrag
 * @tc.desc: test LimitWindowPositionWhenDrag
 * @tc.type: FUNC
 */
HWTEST_F(WindowLayoutPolicyTest, LimitWindowPositionWhenDrag, Function | SmallTest | Level2)
{
    auto displayRect = displayGroupInfo_->GetDisplayRect(defaultDisplayInfo_->GetDisplayId());
    ASSERT_FALSE(WindowHelper::IsEmptyRect(displayRect));

    sptr<WindowNode> node = CreateWindowNode(windowInfo_);
    ASSERT_TRUE(node != nullptr);
    Rect winRect = { 0, 0, 400, 400 }; // width/height: 400
    node->SetWindowRect(winRect);

    windowInfo_.winType_ = WindowType::WINDOW_TYPE_LAUNCHER_DOCK;
    sptr<WindowNode> dockNode = CreateWindowNode(windowInfo_);
    layoutPolicy_->displayGroupWindowTree_[0][WindowRootNodeType::ABOVE_WINDOW_NODE]->push_back(dockNode);

    Rect dockWinRect = { 200, 200, 50, 20 };  // 200, 200, 50, 20
    dockNode->SetWindowRect(dockWinRect);
    layoutPolicy_->LimitWindowPositionWhenDrag(node, winRect);

    dockWinRect = { 200, 200, 50, (displayRect.height_ - static_cast<uint32_t>(200)) }; //param: 200 200 20
    dockNode->SetWindowRect(dockWinRect);
    layoutPolicy_->LimitWindowPositionWhenDrag(node, winRect);

    dockWinRect = { 200, 200, 20, 50 };   // 200, 200, 20, 50
    dockNode->SetWindowRect(dockWinRect);
    layoutPolicy_->LimitWindowPositionWhenDrag(node, winRect);

    dockWinRect = { 0, 200, 20, 50 };     // 200, 200, 20, 50
    dockNode->SetWindowRect(dockWinRect);
    layoutPolicy_->LimitWindowPositionWhenDrag(node, winRect);
}

/**
 * @tc.name: LimitWindowPositionWhenDrag01
 * @tc.desc: test LimitWindowPositionWhenDrag01
 * @tc.type: FUNC
 */
HWTEST_F(WindowLayoutPolicyTest, LimitWindowPositionWhenDrag01, Function | SmallTest | Level2)
{
    auto displayRect = displayGroupInfo_->GetDisplayRect(defaultDisplayInfo_->GetDisplayId());
    ASSERT_FALSE(WindowHelper::IsEmptyRect(displayRect));

    sptr<WindowNode> node = CreateWindowNode(windowInfo_);
    ASSERT_TRUE(node != nullptr);
    Rect winRect = { 0, 0, 400, 400 }; // width/height: 400
    node->SetWindowRect(winRect);

    layoutPolicy_->limitRectMap_[node->GetDisplayId()] = displayRect;
    float virtualPixelRatio = layoutPolicy_->GetVirtualPixelRatio(node->GetDisplayId());
    uint32_t windowTitleBarH = static_cast<uint32_t>(WINDOW_TITLE_BAR_HEIGHT * virtualPixelRatio);
    int32_t limitMinPosX = displayRect.posX_ + static_cast<int32_t>(windowTitleBarH);
    int32_t limitMaxPosX = displayRect.posX_ + static_cast<int32_t>(displayRect.width_ - windowTitleBarH);
    int32_t limitMinPosY = displayRect.posY_;
    int32_t limitMaxPosY = displayRect.posY_ + static_cast<int32_t>(displayRect.height_ - windowTitleBarH);

    Rect newRect = winRect;
    newRect.posX_ = limitMinPosX - newRect.width_ - 1;
    layoutPolicy_->LimitWindowPositionWhenDrag(node, newRect);

    newRect.width_ -= 1;
    layoutPolicy_->LimitWindowPositionWhenDrag(node, newRect);

    winRect.posX_ = limitMaxPosX + 1;
    layoutPolicy_->LimitWindowPositionWhenDrag(node, newRect);

    newRect.width_ = winRect.width_;
    layoutPolicy_->LimitWindowPositionWhenDrag(node, newRect);

    newRect.posY_ = limitMaxPosY + 1;
    layoutPolicy_->LimitWindowPositionWhenDrag(node, newRect);

    newRect.height_ += 1;
    layoutPolicy_->LimitWindowPositionWhenDrag(node, newRect);

    newRect.posY_ = limitMinPosY - 1;
    layoutPolicy_->LimitWindowPositionWhenDrag(node, newRect);

    newRect.height_ = winRect.height_;
    layoutPolicy_->LimitWindowPositionWhenDrag(node, newRect);
}

/**
 * @tc.name: LimitWindowPositionWhenInitRectOrMove
 * @tc.desc: test LimitWindowPositionWhenInitRectOrMove
 * @tc.type: FUNC
 */
HWTEST_F(WindowLayoutPolicyTest, LimitWindowPositionWhenInitRectOrMove, Function | SmallTest | Level2)
{
    auto displayRect = displayGroupInfo_->GetDisplayRect(defaultDisplayInfo_->GetDisplayId());
    ASSERT_FALSE(WindowHelper::IsEmptyRect(displayRect));

    sptr<WindowNode> node = CreateWindowNode(windowInfo_);
    ASSERT_TRUE(node != nullptr);
    Rect winRect = { 0, 0, 400, 400 }; // width/height: 400
    node->SetWindowRect(winRect);

    windowInfo_.winType_ = WindowType::WINDOW_TYPE_LAUNCHER_DOCK;
    sptr<WindowNode> dockNode = CreateWindowNode(windowInfo_);
    ASSERT_TRUE(dockNode != nullptr);
    layoutPolicy_->displayGroupWindowTree_[0][WindowRootNodeType::ABOVE_WINDOW_NODE]->push_back(dockNode);

    Rect dockWinRect = { 200, 200, 50, 20 }; // rect : 200, 200, 50, 20
    dockNode->SetWindowRect(dockWinRect);
    layoutPolicy_->LimitWindowPositionWhenInitRectOrMove(node, winRect);

    dockWinRect = { 200, 200, 50, (displayRect.height_ - static_cast<uint32_t>(200)) }; // param: 200, 50
    dockNode->SetWindowRect(dockWinRect);
    layoutPolicy_->LimitWindowPositionWhenInitRectOrMove(node, winRect);

    dockWinRect = { 200, 200, 20, 50 }; // rect : 200, 200, 20, 50
    dockNode->SetWindowRect(dockWinRect);
    layoutPolicy_->LimitWindowPositionWhenInitRectOrMove(node, winRect);

    dockWinRect = { 0, 200, 20, 50 };   // rect : 0, 200, 20, 50
    dockNode->SetWindowRect(dockWinRect);
    layoutPolicy_->LimitWindowPositionWhenInitRectOrMove(node, winRect);

    node->SetWindowMode(WindowMode::WINDOW_MODE_FULLSCREEN);
    layoutPolicy_->LimitWindowPositionWhenInitRectOrMove(node, winRect);
}

/**
 * @tc.name: GetDockWindowShowState
 * @tc.desc: test GetDockWindowShowState
 * @tc.type: FUNC
 */
HWTEST_F(WindowLayoutPolicyTest, GetDockWindowShowState, Function | SmallTest | Level2)
{
    auto displayRect = displayGroupInfo_->GetDisplayRect(defaultDisplayInfo_->GetDisplayId());
    ASSERT_FALSE(WindowHelper::IsEmptyRect(displayRect));

    Rect dockWinRect = { 200, 200, 50, 20 };  // rect : 200, 200, 50, 20
    layoutPolicy_->GetDockWindowShowState(0, dockWinRect);

    windowInfo_.winType_ = WindowType::WINDOW_TYPE_LAUNCHER_DOCK;
    sptr<WindowNode> dockNode = CreateWindowNode(windowInfo_);
    ASSERT_TRUE(dockNode != nullptr);
    layoutPolicy_->displayGroupWindowTree_[0][WindowRootNodeType::ABOVE_WINDOW_NODE]->push_back(dockNode);

    dockNode->SetWindowRect(dockWinRect);
    layoutPolicy_->GetDockWindowShowState(0, dockWinRect);

    dockWinRect = { 200, 200, 50, (displayRect.height_ - static_cast<uint32_t>(200)) };  // param : 200, 50
    dockNode->SetWindowRect(dockWinRect);
    layoutPolicy_->GetDockWindowShowState(0, dockWinRect);

    dockWinRect = { 200, 200, 20, 50 };  // rect : 200, 200, 20, 50
    dockNode->SetWindowRect(dockWinRect);
    layoutPolicy_->GetDockWindowShowState(0, dockWinRect);

    dockWinRect = { 0, 200, 20, 50 };    // rect : 0, 200, 20, 50
    dockNode->SetWindowRect(dockWinRect);
    layoutPolicy_->GetDockWindowShowState(0, dockWinRect);
}

/**
 * @tc.name: GetAvoidPosType
 * @tc.desc: test GetAvoidPosType
 * @tc.type: FUNC
 */
HWTEST_F(WindowLayoutPolicyTest, GetAvoidPosType, Function | SmallTest | Level2)
{
    auto displayRect = displayGroupInfo_->GetDisplayRect(defaultDisplayInfo_->GetDisplayId());
    ASSERT_FALSE(WindowHelper::IsEmptyRect(displayRect));

    Rect winRect = { 200, 200, 50, 20 }; // rect : 200, 200, 50, 20
    layoutPolicy_->GetAvoidPosType(winRect, 0);

    layoutPolicy_->GetAvoidPosType(winRect, 1);

    winRect.width_ = displayRect.width_;
    layoutPolicy_->GetAvoidPosType(winRect, 0);

    winRect.posY_ = displayRect.posY_;
    layoutPolicy_->GetAvoidPosType(winRect, 0);

    winRect.height_ = displayRect.height_;
    layoutPolicy_->GetAvoidPosType(winRect, 0);

    winRect.posY_ = displayRect.posY_;
    layoutPolicy_->GetAvoidPosType(winRect, 0);
}

/**
 * @tc.name: UpdateLimitRect
 * @tc.desc: test UpdateLimitRect
 * @tc.type: FUNC
 */
HWTEST_F(WindowLayoutPolicyTest, UpdateLimitRect, Function | SmallTest | Level2)
{
    auto displayRect = displayGroupInfo_->GetDisplayRect(defaultDisplayInfo_->GetDisplayId());
    ASSERT_FALSE(WindowHelper::IsEmptyRect(displayRect));

    Rect limitRect = displayRect;

    sptr<WindowNode> node = CreateWindowNode(windowInfo_);
    ASSERT_TRUE(node != nullptr);
    Rect avoidRect = { 200, 200, 50, 20 }; // rect : 200, 200, 50, 20
    node->SetWindowRect(avoidRect);
    layoutPolicy_->UpdateLimitRect(node, limitRect);

    node->GetWindowProperty()->SetWindowType(WindowType::WINDOW_TYPE_NAVIGATION_BAR);
    layoutPolicy_->UpdateLimitRect(node, limitRect);

    avoidRect.width_ = displayRect.width_;
    node->SetWindowRect(avoidRect);
    layoutPolicy_->UpdateLimitRect(node, limitRect);

    avoidRect.posY_ = displayRect.posY_;
    node->SetWindowRect(avoidRect);
    layoutPolicy_->UpdateLimitRect(node, limitRect);

    avoidRect.height_ = displayRect.height_;
    node->SetWindowRect(avoidRect);
    layoutPolicy_->UpdateLimitRect(node, limitRect);

    avoidRect.posY_ = displayRect.posY_;
    node->SetWindowRect(avoidRect);
    layoutPolicy_->UpdateLimitRect(node, limitRect);
}

/**
 * @tc.name: UpdateSurfaceBounds
 * @tc.desc: test UpdateSurfaceBounds
 * @tc.type: FUNC
 */
HWTEST_F(WindowLayoutPolicyTest, UpdateSurfaceBounds, Function | SmallTest | Level2)
{
    sptr<WindowNode> node = CreateWindowNode(windowInfo_);
    ASSERT_TRUE(node != nullptr);
    Rect winRect = { 0, 0, 20, 50 }; // rect : 0, 0, 20, 50
    Rect preRect = { 0, 0, 20, 60 }; // rect : 0, 0, 20, 60
    layoutPolicy_->UpdateSurfaceBounds(node, winRect, preRect);

    node->SetWindowSizeChangeReason(WindowSizeChangeReason::MAXIMIZE);
    layoutPolicy_->UpdateSurfaceBounds(node, winRect, preRect);

    node->SetWindowSizeChangeReason(WindowSizeChangeReason::RECOVER);
    layoutPolicy_->UpdateSurfaceBounds(node, winRect, preRect);

    node->SetWindowSizeChangeReason(WindowSizeChangeReason::ROTATION);
    layoutPolicy_->UpdateSurfaceBounds(node, winRect, preRect);

    node->SetWindowSizeChangeReason(WindowSizeChangeReason::UNDEFINED);
    layoutPolicy_->UpdateSurfaceBounds(node, winRect, preRect);
}
}
}
}
