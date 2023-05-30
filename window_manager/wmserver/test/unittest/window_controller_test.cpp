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
#include "iremote_object_mocker.h"
#include "mock_rs_iwindow_animation_controller.h"
#include "remote_animation.h"
#include "starting_window.h"
#include "window_controller.h"

using namespace testing;
using namespace testing::ext;

namespace OHOS {
namespace Rosen {
class WindowControllerTest : public testing::Test {
public:
    static void SetUpTestCase();
    static void TearDownTestCase();
    void SetUp() override;
    void TearDown() override;

    static sptr<WindowController> windowController_;
    static sptr<WindowRoot> windowRoot_;
    static sptr<InputWindowMonitor> inputWindowMonitor_;
    static sptr<WindowNode> node_;
    static sptr<WindowTransitionInfo> transitionInfo_;
};

sptr<WindowController> WindowControllerTest::windowController_ = nullptr;
sptr<WindowRoot> WindowControllerTest::windowRoot_ = nullptr;
sptr<InputWindowMonitor> WindowControllerTest::inputWindowMonitor_ = nullptr;
sptr<WindowNode> WindowControllerTest::node_ = nullptr;
sptr<WindowTransitionInfo> WindowControllerTest::transitionInfo_ = nullptr;

void RootCallback(Event event, const sptr<IRemoteObject>& remoteObject)
{
    return;
}

void WindowControllerTest::SetUpTestCase()
{
    windowRoot_ = new WindowRoot(RootCallback);
    windowRoot_->displayIdMap_[0].push_back(0);
    inputWindowMonitor_ = new InputWindowMonitor(windowRoot_);
    windowController_ = new WindowController(windowRoot_, inputWindowMonitor_);
    transitionInfo_ = new WindowTransitionInfo();
    transitionInfo_->SetWindowType(WindowType::WINDOW_TYPE_APP_MAIN_WINDOW);
    node_ = StartingWindow::CreateWindowNode(transitionInfo_, 101); // 101 is windowId
    node_->SetWindowMode(WindowMode::WINDOW_MODE_FULLSCREEN);
}

void WindowControllerTest::TearDownTestCase()
{
}

void WindowControllerTest::SetUp()
{
}

void WindowControllerTest::TearDown()
{
}

namespace {
/**
 * @tc.name: StartingWindow
 * @tc.desc: Window controller starting window
 * @tc.type: FUNC
 */
HWTEST_F(WindowControllerTest, StartingWindow, Function | SmallTest | Level3)
{
    windowRoot_->windowNodeMap_.clear();
    windowController_->StartingWindow(nullptr, nullptr, 0, false);
    ASSERT_EQ(0, windowRoot_->windowNodeMap_.size());

    transitionInfo_->SetWindowMode(WindowMode::WINDOW_MODE_UNDEFINED);
    windowController_->StartingWindow(transitionInfo_, nullptr, 0, false);
    ASSERT_EQ(0, windowRoot_->windowNodeMap_.size());

    sptr<IRemoteObject> abilityTokenMocker = new IRemoteObjectMocker();
    transitionInfo_->SetAbilityToken(abilityTokenMocker);
    windowController_->StartingWindow(transitionInfo_, nullptr, 0, false);
    windowController_->StartingWindow(transitionInfo_, nullptr, 0, true);
    ASSERT_EQ(1, windowRoot_->windowNodeMap_.size());

    windowRoot_->windowNodeMap_.clear();
    sptr<IRemoteObject> iRemoteObjectMocker = new IRemoteObjectMocker();
    RemoteAnimation::windowAnimationController_ = iface_cast<RSIWindowAnimationController>(iRemoteObjectMocker);
    windowController_->StartingWindow(transitionInfo_, nullptr, 0, true);
    ASSERT_EQ(1, windowRoot_->windowNodeMap_.size());

    windowRoot_->windowNodeMap_.clear();
    windowRoot_->windowNodeMap_.insert(std::make_pair(node_->GetWindowId(), node_));
    node_->abilityToken_ = abilityTokenMocker;
    node_->stateMachine_.currState_ = WindowNodeState::SHOW_ANIMATION_PLAYING;
    windowController_->StartingWindow(transitionInfo_, nullptr, 0, false);
    ASSERT_EQ(1, windowRoot_->windowNodeMap_.size());

    node_->stateMachine_.currState_ = WindowNodeState::STARTING_CREATED;
    windowController_->StartingWindow(transitionInfo_, nullptr, 0, false);
    transitionInfo_->SetWindowMode(WindowMode::WINDOW_MODE_FULLSCREEN);
    windowController_->StartingWindow(transitionInfo_, nullptr, 0, false);
    transitionInfo_->SetWindowMode(WindowMode::WINDOW_MODE_FLOATING);
    node_->property_->modeSupportInfo_ = WindowModeSupport::WINDOW_MODE_SUPPORT_ALL;
    windowController_->StartingWindow(transitionInfo_, nullptr, 0, false);
    ASSERT_EQ(1, windowRoot_->windowNodeMap_.size());

    // Cancel starting window
    windowController_->CancelStartingWindow(nullptr);
    windowController_->CancelStartingWindow(abilityTokenMocker);

    node_->startingWindowShown_ = true;
    windowController_->CancelStartingWindow(abilityTokenMocker);
    ASSERT_EQ(0, windowRoot_->windowNodeMap_.size());

    windowRoot_->windowNodeMap_.clear();
    RemoteAnimation::windowAnimationController_ = nullptr;
}
/**
 * @tc.name: NotifyWindowTransition
 * @tc.desc: Window controller notify window transtition
 * @tc.type: FUNC
 */
HWTEST_F(WindowControllerTest, NotifyWindowTransition, Function | SmallTest | Level3)
{
    sptr<WindowTransitionInfo> srcInfo = nullptr;
    sptr<WindowTransitionInfo> dstInfo = nullptr;
    ASSERT_EQ(WMError::WM_ERROR_NO_REMOTE_ANIMATION, windowController_->NotifyWindowTransition(srcInfo, dstInfo));

    srcInfo = new WindowTransitionInfo();
    sptr<IRemoteObject> srcAbilityTokenMocker = new IRemoteObjectMocker();
    srcInfo->SetAbilityToken(srcAbilityTokenMocker);
    sptr<WindowNode> srcNode = StartingWindow::CreateWindowNode(srcInfo, 102); // 102 is windowId
    srcNode->property_->modeSupportInfo_ = WindowModeSupport::WINDOW_MODE_SUPPORT_ALL;

    dstInfo = new WindowTransitionInfo();
    sptr<IRemoteObject> dstAbilityTokenMocker = new IRemoteObjectMocker();
    dstInfo->SetAbilityToken(dstAbilityTokenMocker);
    sptr<WindowNode> dstNode = StartingWindow::CreateWindowNode(dstInfo, 103); // 103 is windowId
    dstNode->property_->modeSupportInfo_ = WindowModeSupport::WINDOW_MODE_SUPPORT_ALL;

    windowRoot_->windowNodeMap_.clear();
    windowRoot_->windowNodeMap_.insert(std::make_pair(srcNode->GetWindowId(), srcNode));
    windowRoot_->windowNodeMap_.insert(std::make_pair(dstNode->GetWindowId(), dstNode));

    sptr<DisplayInfo> displayInfo = new DisplayInfo();
    sptr<WindowNodeContainer> container = new WindowNodeContainer(displayInfo, 0);
    windowRoot_->windowNodeContainerMap_.insert(std::make_pair(0, container));

    sptr<MockRSIWindowAnimationController> mock = new MockRSIWindowAnimationController();
    RemoteAnimation::windowAnimationController_ = mock;
    RemoteAnimation::windowRoot_ = windowRoot_;
    RemoteAnimation::animationFirst_ = true;

    srcInfo->SetTransitionReason(TransitionReason::MINIMIZE);
    srcNode->stateMachine_.currState_ = WindowNodeState::HIDDEN;
    ASSERT_EQ(WMError::WM_ERROR_NO_REMOTE_ANIMATION, windowController_->NotifyWindowTransition(srcInfo, dstInfo));

    srcInfo->SetTransitionReason(TransitionReason::MINIMIZE);
    srcNode->stateMachine_.currState_ = WindowNodeState::STARTING_CREATED;
    EXPECT_CALL(*mock, OnMinimizeWindow(_, _)).Times(1);
    ASSERT_EQ(WMError::WM_OK, windowController_->NotifyWindowTransition(srcInfo, dstInfo));

    srcInfo->SetTransitionReason(TransitionReason::CLOSE);
    srcNode->stateMachine_.currState_ = WindowNodeState::STARTING_CREATED;
    EXPECT_CALL(*mock, OnCloseWindow(_, _)).Times(1);
    ASSERT_EQ(WMError::WM_OK, windowController_->NotifyWindowTransition(srcInfo, dstInfo));

    srcInfo->SetTransitionReason(TransitionReason::BACK_TRANSITION);
    srcNode->stateMachine_.currState_ = WindowNodeState::STARTING_CREATED;
    EXPECT_CALL(*mock, OnAppTransition(_, _, _)).Times(1);
    ASSERT_EQ(WMError::WM_OK, windowController_->NotifyWindowTransition(srcInfo, dstInfo));

    srcInfo->SetTransitionReason(TransitionReason::ABILITY_TRANSITION);
    dstNode->stateMachine_.currState_ = WindowNodeState::STARTING_CREATED;
    dstNode->property_->SetWindowType(WindowType::WINDOW_TYPE_APP_MAIN_WINDOW);
    dstNode->SetWindowMode(WindowMode::WINDOW_MODE_FLOATING);
    EXPECT_CALL(*mock, OnStartApp(_, _, _)).Times(1);
    ASSERT_EQ(WMError::WM_OK, windowController_->NotifyWindowTransition(srcInfo, dstInfo));

    dstNode->stateMachine_.currState_ = WindowNodeState::STARTING_CREATED;
    dstNode->SetWindowMode(WindowMode::WINDOW_MODE_FULLSCREEN);
    EXPECT_CALL(*mock, OnStartApp(_, _, _)).Times(1);
    ASSERT_EQ(WMError::WM_OK, windowController_->NotifyWindowTransition(srcInfo, dstInfo));

    windowRoot_->windowNodeContainerMap_.clear();
    RemoteAnimation::windowAnimationController_ = nullptr;
}
/**
 * @tc.name: FocusWindow
 * @tc.desc: Window controller focus window
 * @tc.type: FUNC
 */
HWTEST_F(WindowControllerTest, FocusWindow, Function | SmallTest | Level3)
{
    sptr<IRemoteObject> abilityToken = nullptr;
    windowController_->GetFocusWindowInfo(abilityToken);

    sptr<DisplayInfo> displayInfo = new DisplayInfo();
    sptr<WindowNodeContainer> container = new WindowNodeContainer(displayInfo, 0);
    windowRoot_->windowNodeContainerMap_.insert(std::make_pair(0, container));

    sptr<WindowNode> windowNode;
    ASSERT_EQ(WMError::WM_ERROR_INVALID_WINDOW, windowController_->GetFocusWindowNode(0, windowNode));

    windowRoot_->windowNodeMap_.clear();
    windowRoot_->windowNodeMap_.insert(std::make_pair(node_->GetWindowId(), node_));
    container->focusedWindow_ = node_->GetWindowId();
    node_->currentVisibility_ = false;
    ASSERT_EQ(WMError::WM_ERROR_INVALID_WINDOW, windowController_->GetFocusWindowNode(0, windowNode));

    node_->currentVisibility_ = true;
    ASSERT_EQ(WMError::WM_OK, windowController_->GetFocusWindowNode(0, windowNode));
    windowRoot_->windowNodeContainerMap_.clear();
}
/**
 * @tc.name: CreateWindow
 * @tc.desc: Window controller create window
 * @tc.type: FUNC
 */
HWTEST_F(WindowControllerTest, CreateWindow, Function | SmallTest | Level3)
{
    sptr<IWindow> window;
    sptr<WindowProperty> property = new WindowProperty();
    std::shared_ptr<RSSurfaceNode> surfaceNode;

    uint32_t windowId;
    property->SetParentId(INVALID_WINDOW_ID);
    ASSERT_EQ(WMError::WM_OK, windowController_->CreateWindow(window, property, surfaceNode, windowId, nullptr, 0, 0));

    property->SetParentId(1);
    property->SetWindowType(WindowType::WINDOW_TYPE_APP_MAIN_WINDOW);
    ASSERT_EQ(WMError::WM_ERROR_INVALID_TYPE,
        windowController_->CreateWindow(window, property, surfaceNode, windowId, nullptr, 0, 0));

    property->SetWindowType(WindowType::WINDOW_TYPE_APP_SUB_WINDOW);
    ASSERT_EQ(WMError::WM_OK, windowController_->CreateWindow(window, property, surfaceNode, windowId, nullptr, 0, 0));

    property->SetWindowType(WindowType::WINDOW_TYPE_SYSTEM_SUB_WINDOW);
    ASSERT_EQ(WMError::WM_OK, windowController_->CreateWindow(window, property, surfaceNode, windowId, nullptr, 0, 0));
    windowRoot_->windowNodeMap_.clear();

    sptr<IRemoteObject> abilityTokenMocker = new IRemoteObjectMocker();
    node_->abilityToken_ = abilityTokenMocker;

    property->SetParentId(INVALID_WINDOW_ID);
    property->SetWindowType(WindowType::WINDOW_TYPE_APP_MAIN_WINDOW);
    ASSERT_EQ(WMError::WM_OK,
        windowController_->CreateWindow(window, property, surfaceNode, windowId, abilityTokenMocker, 0, 0));

    property->SetWindowType(WindowType::WINDOW_TYPE_APP_SUB_WINDOW);
    ASSERT_EQ(WMError::WM_OK,
        windowController_->CreateWindow(window, property, surfaceNode, windowId, abilityTokenMocker, 0, 0));

    property->SetWindowType(WindowType::WINDOW_TYPE_APP_MAIN_WINDOW);
    node_->startingWindowShown_ = false;
    ASSERT_EQ(WMError::WM_OK,
        windowController_->CreateWindow(window, property, surfaceNode, windowId, abilityTokenMocker, 0, 0));
    windowRoot_->windowNodeMap_.clear();
}
/**
 * @tc.name: NotifyAfterAddWindow
 * @tc.desc: Window controller notify after add window
 * @tc.type: FUNC
 */
HWTEST_F(WindowControllerTest, NotifyAfterAddWindow, Function | SmallTest | Level3)
{
    ASSERT_NE(nullptr, windowController_);
    sptr<WindowNode> node0 = new WindowNode();
    windowController_->NotifyAfterAddWindow(node0);
    ASSERT_EQ(0, node0->children_.size());

    sptr<WindowNode> node1 = new WindowNode();
    node1->currentVisibility_ = false;
    sptr<WindowNode> node2= new WindowNode();
    node2->currentVisibility_ = true;

    node0->children_.push_back(node1);
    node0->children_.push_back(node2);
    windowController_->NotifyAfterAddWindow(node0);
    ASSERT_EQ(2, node0->children_.size());
    ASSERT_EQ(nullptr, node0->children_[0]->abilityToken_);
}
/**
 * @tc.name: AddWindowNode
 * @tc.desc: Window controller add window node
 * @tc.type: FUNC
 */
HWTEST_F(WindowControllerTest, AddWindowNode, Function | SmallTest | Level3)
{
    sptr<WindowProperty> property = new WindowProperty();
    property->SetWindowId(0);
    ASSERT_EQ(WMError::WM_ERROR_NULLPTR, windowController_->AddWindowNode(property));

    windowRoot_->windowNodeMap_.clear();
    windowRoot_->windowNodeMap_.insert(std::make_pair(node_->GetWindowId(), node_));
    property->SetWindowId(node_->GetWindowId());
    node_->currentVisibility_ = true;
    node_->startingWindowShown_ = false;
    ASSERT_EQ(WMError::WM_ERROR_INVALID_OPERATION, windowController_->AddWindowNode(property));
    windowRoot_->windowNodeMap_.clear();
}
/**
 * @tc.name: InputCallingWindow
 * @tc.desc: Window controller input calling window
 * @tc.type: FUNC
 */
HWTEST_F(WindowControllerTest, InputCallingWindow, Function | SmallTest | Level3)
{
    windowController_->callingWindowId_ = 0;
    windowRoot_->windowNodeMap_.clear();
    sptr<WindowNode> node = new WindowNode();
    node->property_->callingWindow_ = 0;
    node->property_->displayId_ = DISPLAY_ID_INVALID;
    windowController_->ResizeSoftInputCallingWindowIfNeed(node);
    ASSERT_EQ(0, windowController_->callingWindowId_);

    sptr<DisplayInfo> displayInfo = new DisplayInfo();
    sptr<WindowNodeContainer> container = new WindowNodeContainer(displayInfo, 0);
    windowRoot_->windowNodeContainerMap_.insert(std::make_pair(0, container));
    node->property_->displayId_ = 0;
    windowController_->ResizeSoftInputCallingWindowIfNeed(node);
    ASSERT_EQ(0, windowController_->callingWindowId_);

    windowRoot_->windowNodeMap_.insert(std::make_pair(node_->GetWindowId(), node_));
    container->focusedWindow_ = node_->GetWindowId();
    node_->SetWindowMode(WindowMode::WINDOW_MODE_FLOATING);
    node_->currentVisibility_ = false;
    windowController_->ResizeSoftInputCallingWindowIfNeed(node);
    ASSERT_EQ(0, windowController_->callingWindowId_);

    node_->currentVisibility_ = true;
    node_->SetWindowMode(WindowMode::WINDOW_MODE_FULLSCREEN);
    windowController_->ResizeSoftInputCallingWindowIfNeed(node);

    node_->currentVisibility_ = true;
    node_->SetWindowMode(WindowMode::WINDOW_MODE_FLOATING);
    windowController_->ResizeSoftInputCallingWindowIfNeed(node);
    ASSERT_EQ(0, windowController_->callingWindowId_);

    windowController_->callingWindowId_ = node_->GetWindowId();
    windowController_->callingWindowRestoringRect_ = {0, 0, 0, 0};
    windowController_->RestoreCallingWindowSizeIfNeed();
    ASSERT_EQ(0, windowController_->callingWindowId_);

    windowController_->callingWindowRestoringRect_ = {0, 0, 1, 1};
    windowController_->callingWindowId_ = 0;
    windowController_->RestoreCallingWindowSizeIfNeed();

    windowController_->callingWindowId_ = node_->GetWindowId();
    windowController_->RestoreCallingWindowSizeIfNeed();
    ASSERT_EQ(0, windowController_->callingWindowId_);

    windowController_->callingWindowId_ = node_->GetWindowId();
    node_->SetWindowMode(WindowMode::WINDOW_MODE_FULLSCREEN);
    windowController_->RestoreCallingWindowSizeIfNeed();
    ASSERT_EQ(0, windowController_->callingWindowId_);

    windowRoot_->windowNodeMap_.clear();
    windowRoot_->windowNodeContainerMap_.clear();
}
/**
 * @tc.name: SetDefaultDisplayInfo
 * @tc.desc: Window controller set default display info
 * @tc.type: FUNC
 */
HWTEST_F(WindowControllerTest, SetDefaultDisplayInfo, Function | SmallTest | Level3)
{
    const int32_t displayWidth = 100;
    const int32_t displayHeight = 200;
    windowController_->defaultDisplayRect_ = { 0, 0, 0, 0 };

    sptr<DisplayInfo> displayInfo = nullptr;
    windowController_->SetDefaultDisplayInfo(0, displayInfo);
    ASSERT_EQ(0, windowController_->defaultDisplayRect_.width_);
    ASSERT_EQ(0, windowController_->defaultDisplayRect_.height_);

    displayInfo = new DisplayInfo();
    displayInfo->id_ = 1;
    displayInfo->width_ = displayWidth;
    displayInfo->height_ = displayHeight;

    windowController_->SetDefaultDisplayInfo(0, displayInfo);
    ASSERT_EQ(0, windowController_->defaultDisplayRect_.width_);
    ASSERT_EQ(0, windowController_->defaultDisplayRect_.height_);

    displayInfo->id_ = 0;
    windowController_->SetDefaultDisplayInfo(0, displayInfo);
    ASSERT_EQ(displayWidth, windowController_->defaultDisplayRect_.width_);
    ASSERT_EQ(displayHeight, windowController_->defaultDisplayRect_.height_);
}
/**
 * @tc.name: ProcessDisplayCompression
 * @tc.desc: Window controller process display compression
 * @tc.type: FUNC
 */
HWTEST_F(WindowControllerTest, ProcessDisplayCompression, Function | SmallTest | Level3)
{
    ASSERT_NE(nullptr, windowController_);
    DisplayId defaultDisplayId = 0;
    sptr<DisplayInfo> displayInfo = new DisplayInfo();
    displayInfo->id_ = 1;
    windowController_->ProcessDisplayCompression(defaultDisplayId, displayInfo);
    ASSERT_EQ(nullptr, windowController_->maskingSurfaceNode_);

    displayInfo->id_ = defaultDisplayId;
    displayInfo->waterfallDisplayCompressionStatus_ = false;
    windowController_->ProcessDisplayCompression(defaultDisplayId, displayInfo);
    ASSERT_EQ(nullptr, windowController_->maskingSurfaceNode_);

    displayInfo->waterfallDisplayCompressionStatus_ = true;
    windowController_->ProcessDisplayCompression(defaultDisplayId, displayInfo);
    ASSERT_NE(nullptr, windowController_->maskingSurfaceNode_);
}
/**
 * @tc.name: StopBootAnimationIfNeed
 * @tc.desc: Window controller stop boot animation if need
 * @tc.type: FUNC
 */
HWTEST_F(WindowControllerTest, StopBootAnimationIfNeed, Function | SmallTest | Level3)
{
    ASSERT_NE(nullptr, windowController_);

    sptr<WindowNode> node = nullptr;
    windowController_->isBootAnimationStopped_ = true;
    windowController_->StopBootAnimationIfNeed(node);
    ASSERT_EQ(true, windowController_->isBootAnimationStopped_);

    windowController_->isBootAnimationStopped_ = false;
    windowController_->StopBootAnimationIfNeed(node);
    ASSERT_EQ(false, windowController_->isBootAnimationStopped_);

    node = new WindowNode();
    node->SetDisplayId(DISPLAY_ID_INVALID + 1);
    windowController_->StopBootAnimationIfNeed(node);
    ASSERT_EQ(false, windowController_->isBootAnimationStopped_);

    node->SetDisplayId(DISPLAY_ID_INVALID);
    windowController_->StopBootAnimationIfNeed(node);
    ASSERT_EQ(false, windowController_->isBootAnimationStopped_);
}
/**
 * @tc.name: GetEmbedNodeId
 * @tc.desc: Window controller get embed node id
 * @tc.type: FUNC
 */
HWTEST_F(WindowControllerTest, GetEmbedNodeId, Function | SmallTest | Level3)
{
    std::vector<sptr<WindowNode>> windowNodes;
    sptr<WindowNode> node0 = nullptr;
    sptr<WindowNode> node1 = new WindowNode();
    node1->property_->windowId_ = 1;
    sptr<WindowNode> node2 = new WindowNode();
    node2->property_->windowId_ = 2;
    sptr<WindowNode> node3 = new WindowNode();
    node3->property_->windowId_ = 3;

    node1->property_->SetWindowType(WindowType::WINDOW_TYPE_APP_MAIN_WINDOW);
    ASSERT_EQ(0, windowController_->GetEmbedNodeId(windowNodes, node1));

    node1->property_->SetWindowType(WindowType::WINDOW_TYPE_APP_COMPONENT);
    ASSERT_EQ(0, windowController_->GetEmbedNodeId(windowNodes, node1));

    windowNodes.push_back(node0);
    windowNodes.push_back(node2);
    windowNodes.push_back(node1);
    windowNodes.push_back(node2);
    windowNodes.push_back(node3);

    node1->SetWindowRect({50, 50, 50, 50});
    node3->SetWindowRect({0, 0, 200, 200});
    ASSERT_EQ(node3->GetWindowId(), windowController_->GetEmbedNodeId(windowNodes, node1));
}
/**
 * @tc.name: BindDialogTarget
 * @tc.desc: Window controller bind dialog target
 * @tc.type: FUNC
 */
HWTEST_F(WindowControllerTest, BindDialogTarget, Function | SmallTest | Level3)
{
    windowRoot_->windowNodeMap_.clear();

    uint32_t id = 0;
    sptr<IRemoteObject> abilityTokenMocker = new IRemoteObjectMocker();
    ASSERT_EQ(WMError::WM_ERROR_NULLPTR, windowController_->BindDialogTarget(id, abilityTokenMocker));

    windowRoot_->windowNodeMap_.insert(std::make_pair(node_->GetWindowId(), node_));
    id = node_->GetWindowId();
    ASSERT_EQ(WMError::WM_OK, windowController_->BindDialogTarget(id, abilityTokenMocker));
    windowRoot_->windowNodeMap_.clear();
}
}
}
}
