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
#include "mock_RSIWindowAnimationController.h"
#include "remote_animation.h"
#include "starting_window.h"
#include "window_helper.h"
#include "window_transition_info.h"
using namespace testing;
using namespace testing::ext;

namespace OHOS {
namespace Rosen {
class StartingWindowTest : public testing::Test {
public:
    static void SetUpTestCase();
    static void TearDownTestCase();
    void SetUp() override;
    void TearDown() override;

    sptr<WindowProperty> CreateWindowProperty();
private:
    static sptr<WindowTransitionInfo> transitionInfo_;
    RSSurfaceNode::SharedPtr CreateRSSurfaceNode();
    static sptr<RSIWindowAnimationController> animationController_;
    static sptr<WindowNode> node_;
};

sptr<WindowTransitionInfo> StartingWindowTest::transitionInfo_ = nullptr;
sptr<RSIWindowAnimationController> StartingWindowTest::animationController_ = nullptr;
sptr<WindowNode> StartingWindowTest::node_ = nullptr;

void StartingWindowTest::SetUpTestCase()
{
}

void StartingWindowTest::TearDownTestCase()
{
}

void StartingWindowTest::SetUp()
{
    transitionInfo_ = new WindowTransitionInfo();
    animationController_ = new RSIWindowAnimationControllerMocker();
    ASSERT_EQ(WMError::WM_OK, RemoteAnimation::SetWindowAnimationController(animationController_));
    transitionInfo_->supportWindowModes_ = {
        AppExecFwk::SupportWindowMode::FULLSCREEN,
        AppExecFwk::SupportWindowMode::SPLIT,
        AppExecFwk::SupportWindowMode::FLOATING
    };
    node_ = StartingWindow::CreateWindowNode(transitionInfo_, 101); // 101 is windowId
    node_->SetWindowRect({0, 0, 100, 100});
}

void StartingWindowTest::TearDown()
{
    transitionInfo_ = nullptr;
    node_ = nullptr;
}

RSSurfaceNode::SharedPtr StartingWindowTest::CreateRSSurfaceNode()
{
    struct RSSurfaceNodeConfig rsSurfaceNodeConfig;
    rsSurfaceNodeConfig.SurfaceNodeName = "startingWindowTestSurfaceNode";
    auto surfaceNode = RSSurfaceNode::Create(rsSurfaceNodeConfig);
    return surfaceNode;
}

sptr<WindowProperty> StartingWindowTest::CreateWindowProperty()
{
    sptr<WindowProperty> property = new WindowProperty();
    return property;
}
namespace {
/**
 * @tc.name: NeedToStopStartingWindow01
 * @tc.desc: stop starting window test without main App window
 * @tc.type: FUNC
 */
HWTEST_F(StartingWindowTest, NeedToStopStartingWindow01, Function | SmallTest | Level2)
{
    transitionInfo_->SetWindowType(WindowType::WINDOW_TYPE_APP_SUB_WINDOW);
    sptr<WindowNode> node = new WindowNode(CreateWindowProperty());
    ASSERT_EQ(true, WindowHelper::CheckSupportWindowMode(node->GetWindowMode(),
        node->GetModeSupportInfo(), transitionInfo_));
}

/**
 * @tc.name: NeedToStopStartingWindow02
 * @tc.desc: need to stop starting window test with unsupport mode
 * @tc.type: FUNC
 */
HWTEST_F(StartingWindowTest, NeedToStopStartingWindow02, Function | SmallTest | Level2)
{
    sptr<WindowNode> node = new WindowNode(CreateWindowProperty());
    node->SetWindowMode(WindowMode::WINDOW_MODE_FULLSCREEN);
    node->SetModeSupportInfo(WindowModeSupport::WINDOW_MODE_SUPPORT_FLOATING);
    ASSERT_EQ(false, WindowHelper::CheckSupportWindowMode(node->GetWindowMode(),
        node->GetModeSupportInfo(), transitionInfo_));
}

/**
 * @tc.name: NeedToStopStartingWindow03
 * @tc.desc: need to stop starting window test with support mode
 * @tc.type: FUNC
 */
HWTEST_F(StartingWindowTest, NeedToStopStartingWindow03, Function | SmallTest | Level2)
{
    sptr<WindowNode> node = new WindowNode(CreateWindowProperty());
    ASSERT_EQ(true, WindowHelper::CheckSupportWindowMode(node->GetWindowMode(),
        node->GetModeSupportInfo(), transitionInfo_));
}

/**
 * @tc.name: NeedToStopStartingWindow04
 * @tc.desc: need to stop starting window test with support mode
 * @tc.type: FUNC
 */
HWTEST_F(StartingWindowTest, NeedToStopStartingWindow04, Function | SmallTest | Level2)
{
    sptr<WindowNode> node = new WindowNode(CreateWindowProperty());
    transitionInfo_->SetShowFlagWhenLocked(true);
    node->SetModeSupportInfo(WindowModeSupport::WINDOW_MODE_SUPPORT_SPLIT_PRIMARY |
                             WindowModeSupport::WINDOW_MODE_SUPPORT_SPLIT_SECONDARY);
    ASSERT_EQ(false, WindowHelper::CheckSupportWindowMode(node->GetWindowMode(),
        node->GetModeSupportInfo(), transitionInfo_));
}

/**
 * @tc.name: CreateWindowNode01
 * @tc.desc: create starting window node test
 * @tc.type: FUNC
 */
HWTEST_F(StartingWindowTest, CreateWindowNode01, Function | SmallTest | Level2)
{
    sptr<WindowTransitionInfo> info = nullptr;
    ASSERT_EQ(nullptr, StartingWindow::CreateWindowNode(info, 0));
}

/**
 * @tc.name: CreateWindowNode02
 * @tc.desc: create starting window node test
 * @tc.type: FUNC
 */
HWTEST_F(StartingWindowTest, CreateWindowNode02, Function | SmallTest | Level2)
{
    transitionInfo_->SetWindowMode(WindowMode::WINDOW_MODE_UNDEFINED);
    sptr<WindowNode> node = StartingWindow::CreateWindowNode(transitionInfo_, 0);
    ASSERT_NE(nullptr, node);
    ASSERT_EQ(WindowMode::WINDOW_MODE_FULLSCREEN, node->GetWindowMode());
}

/**
 * @tc.name: CreateWindowNode03
 * @tc.desc: create starting window node test
 * @tc.type: FUNC
 */
HWTEST_F(StartingWindowTest, CreateWindowNode03, Function | SmallTest | Level2)
{
    transitionInfo_->SetWindowMode(WindowMode::WINDOW_MODE_FLOATING);
    sptr<WindowNode> node = StartingWindow::CreateWindowNode(transitionInfo_, 0);
    ASSERT_NE(nullptr, node);
    ASSERT_EQ(WindowMode::WINDOW_MODE_FLOATING, node->GetWindowMode());
}

/**
 * @tc.name: CreateWindowNode04
 * @tc.desc: create starting window node test
 * @tc.type: FUNC
 */
HWTEST_F(StartingWindowTest, CreateWindowNode04, Function | SmallTest | Level2)
{
    transitionInfo_->SetShowFlagWhenLocked(true);
    sptr<WindowNode> node = StartingWindow::CreateWindowNode(transitionInfo_, 0);
    ASSERT_NE(nullptr, node);
}

/**
 * @tc.name: DrawStartingWindow01
 * @tc.desc: draw starting window node
 * @tc.type: FUNC
 */
HWTEST_F(StartingWindowTest, DrawStartingWindow01, Function | SmallTest | Level2)
{
    std::shared_ptr<Media::PixelMap> pixelMap = std::make_shared<Media::PixelMap>();
    sptr<WindowNode> node = nullptr;
    ASSERT_EQ(WMError::WM_ERROR_NULLPTR, StartingWindow::DrawStartingWindow(node, pixelMap, 0x00FFFFFF, true));
}

/**
 * @tc.name: DrawStartingWindow02
 * @tc.desc: draw starting window node
 * @tc.type: FUNC
 */
HWTEST_F(StartingWindowTest, DrawStartingWindow02, Function | SmallTest | Level2)
{
    node_->leashWinSurfaceNode_ = nullptr;
    std::shared_ptr<Media::PixelMap> pixelMap = std::make_shared<Media::PixelMap>();
    ASSERT_EQ(WMError::WM_OK, StartingWindow::DrawStartingWindow(node_, pixelMap, 0x00FFFFFF, true));
}

/**
 * @tc.name: DrawStartingWindow03
 * @tc.desc: draw starting window node
 * @tc.type: FUNC
 */
HWTEST_F(StartingWindowTest, DrawStartingWindow03, Function | SmallTest | Level2)
{
    std::shared_ptr<Media::PixelMap> pixelMap = std::make_shared<Media::PixelMap>();
    ASSERT_EQ(WMError::WM_OK, StartingWindow::DrawStartingWindow(node_, pixelMap, 0x00FFFFFF, false));
}

/**
 * @tc.name: DrawStartingWindow04
 * @tc.desc: draw starting window node
 * @tc.type: FUNC
 */
HWTEST_F(StartingWindowTest, DrawStartingWindow04, Function | SmallTest | Level2)
{
    node_->startingWinSurfaceNode_ = nullptr;
    std::shared_ptr<Media::PixelMap> pixelMap = std::make_shared<Media::PixelMap>();
    ASSERT_EQ(WMError::WM_ERROR_NULLPTR, StartingWindow::DrawStartingWindow(node_, pixelMap, 0x00FFFFFF, true));
}

/**
 * @tc.name: DrawStartingWindow05
 * @tc.desc: draw starting window node
 * @tc.type: FUNC
 */
HWTEST_F(StartingWindowTest, DrawStartingWindow05, Function | SmallTest | Level2)
{
    ASSERT_EQ(WMError::WM_OK, StartingWindow::DrawStartingWindow(node_, nullptr, 0x00FFFFFF, true));
}

/**
 * @tc.name: DrawStartingWindow06
 * @tc.desc: draw starting window node
 * @tc.type: FUNC
 */
HWTEST_F(StartingWindowTest, DrawStartingWindow06, Function | SmallTest | Level2)
{
    std::shared_ptr<Media::PixelMap> pixelMap = std::make_shared<Media::PixelMap>();
    ASSERT_EQ(WMError::WM_OK, StartingWindow::DrawStartingWindow(node_, pixelMap, 0x00FFFFFF, true));
}

/**
 * @tc.name: HandleClientWindowCreateAndRelease01
 * @tc.desc: handle client window create
 * @tc.type: FUNC
 */
HWTEST_F(StartingWindowTest, HandleClientWindowCreateAndRelease01, Function | SmallTest | Level2)
{
    sptr<IRemoteObject> iRemoteObjectMocker = new IRemoteObjectMocker();
    sptr<IWindow> iWindow = iface_cast<IWindow>(iRemoteObjectMocker);
    ASSERT_NE(nullptr, iWindow);
    uint32_t windowId = 0;
    auto surfaceNode = CreateRSSurfaceNode();
    ASSERT_NE(nullptr, surfaceNode);
    sptr<WindowProperty> windowProperty = new WindowProperty();
    StartingWindow::HandleClientWindowCreate(node_, iWindow, windowId, surfaceNode, windowProperty, 0, 0);
    ASSERT_EQ(node_->GetWindowId(), windowId);
    StartingWindow::ReleaseStartWinSurfaceNode(node_);
}

/**
 * @tc.name: HandleClientWindowCreate02
 * @tc.desc: handle client window create
 * @tc.type: FUNC
 */
HWTEST_F(StartingWindowTest, HandleClientWindowCreate02, Function | SmallTest | Level2)
{
    sptr<IRemoteObject> iRemoteObjectMocker = new IRemoteObjectMocker();
    sptr<IWindow> iWindow = iface_cast<IWindow>(iRemoteObjectMocker);
    ASSERT_NE(nullptr, iWindow);
    uint32_t windowId = 0;
    auto surfaceNode = CreateRSSurfaceNode();
    ASSERT_NE(nullptr, surfaceNode);
    sptr<WindowProperty> windowProperty = new WindowProperty();
    sptr<WindowNode> node = nullptr;
    StartingWindow::HandleClientWindowCreate(node, iWindow, windowId, surfaceNode, windowProperty, 0, 0);
}

/**
 * @tc.name: HandleClientWindowCreateAndRelease03
 * @tc.desc: handle client window create and Release with null leashNode
 * @tc.type: FUNC
 */
HWTEST_F(StartingWindowTest, HandleClientWindowCreateAndRelease03, Function | SmallTest | Level2)
{
    node_->leashWinSurfaceNode_ = nullptr;
    uint32_t windowId = 0;
    auto surfaceNode = CreateRSSurfaceNode();
    ASSERT_NE(nullptr, surfaceNode);
    sptr<WindowProperty> windowProperty = new WindowProperty();
    sptr<IWindow> iWindow = nullptr;
    StartingWindow::HandleClientWindowCreate(node_, iWindow, windowId, surfaceNode, windowProperty, 0, 0);
    StartingWindow::ReleaseStartWinSurfaceNode(node_);
}

/**
 * @tc.name: AddNodeOnRSTree01
 * @tc.desc: Add node on rs tree test with surfaceNode nullptr and hot start
 * @tc.type: FUNC
 */
HWTEST_F(StartingWindowTest, AddNodeOnRSTree01, Function | SmallTest | Level2)
{
    sptr<RSIWindowAnimationController> testController = nullptr;
    ASSERT_EQ(WMError::WM_ERROR_NULLPTR, RemoteAnimation::SetWindowAnimationController(testController));
    AnimationConfig config;
    StartingWindow::AddNodeOnRSTree(node_, config, true);
    ASSERT_EQ(WMError::WM_OK, StartingWindow::DrawStartingWindow(node_, nullptr, 0x66FFFFFF, false));
    usleep(200000);
}

/**
 * @tc.name: AddNodeOnRSTree02
 * @tc.desc: Add node on rs tree test with hot start and surfaceNode
 * @tc.type: FUNC
 */
HWTEST_F(StartingWindowTest, AddNodeOnRSTree02, Function | SmallTest | Level2)
{
    auto surfaceNode = CreateRSSurfaceNode();
    ASSERT_NE(nullptr, surfaceNode);
    node_->surfaceNode_ = surfaceNode;
    sptr<RSIWindowAnimationController> testController = nullptr;
    ASSERT_EQ(WMError::WM_ERROR_NULLPTR, RemoteAnimation::SetWindowAnimationController(testController));
    AnimationConfig config;
    StartingWindow::AddNodeOnRSTree(node_, config, true);
    ASSERT_EQ(WMError::WM_OK, StartingWindow::DrawStartingWindow(node_, nullptr, 0x66FFFFFF, false));
    usleep(200000);
}

/**
 * @tc.name: AddNodeOnRSTree03
 * @tc.desc: Add node on rs tree test
 * @tc.type: FUNC
 */
HWTEST_F(StartingWindowTest, AddNodeOnRSTree03, Function | SmallTest | Level2)
{
    auto surfaceNode = CreateRSSurfaceNode();
    ASSERT_NE(nullptr, surfaceNode);
    node_->surfaceNode_ = surfaceNode;
    node_->leashWinSurfaceNode_ = nullptr;
    sptr<RSIWindowAnimationController> testController = nullptr;
    ASSERT_EQ(WMError::WM_ERROR_NULLPTR, RemoteAnimation::SetWindowAnimationController(testController));
    AnimationConfig config;
    StartingWindow::AddNodeOnRSTree(node_, config, true);
    ASSERT_EQ(WMError::WM_OK, StartingWindow::DrawStartingWindow(node_, nullptr, 0x66FFFFFF, false));
    usleep(200000);
}

/**
 * @tc.name: AddNodeOnRSTree04
 * @tc.desc: Add node on rs tree test with surfaceNode nullptr, with animation controller
 * @tc.type: FUNC
 */
HWTEST_F(StartingWindowTest, AddNodeOnRSTree04, Function | SmallTest | Level2)
{
    sptr<RSIWindowAnimationController> testController = new RSIWindowAnimationControllerMocker();
    ASSERT_EQ(WMError::WM_OK, RemoteAnimation::SetWindowAnimationController(testController));
    AnimationConfig config;
    StartingWindow::AddNodeOnRSTree(node_, config, true);
    ASSERT_EQ(WMError::WM_OK, StartingWindow::DrawStartingWindow(node_, nullptr, 0x66FFFFFF, false));
    usleep(200000);
}

/**
 * @tc.name: AddNodeOnRSTree05
 * @tc.desc: Add node on rs tree test
 * @tc.type: FUNC
 */
HWTEST_F(StartingWindowTest, AddNodeOnRSTree05, Function | SmallTest | Level2)
{
    auto surfaceNode = CreateRSSurfaceNode();
    ASSERT_NE(nullptr, surfaceNode);
    node_->surfaceNode_ = surfaceNode;
    sptr<RSIWindowAnimationController> testController = new RSIWindowAnimationControllerMocker();
    ASSERT_EQ(WMError::WM_OK, RemoteAnimation::SetWindowAnimationController(testController));
    AnimationConfig config;
    StartingWindow::AddNodeOnRSTree(node_, config, true);
    ASSERT_EQ(WMError::WM_OK, StartingWindow::DrawStartingWindow(node_, nullptr, 0x66FFFFFF, false));
    usleep(200000);
}

/**
 * @tc.name: AddNodeOnRSTree06
 * @tc.desc: Add node on rs tree test
 * @tc.type: FUNC
 */
HWTEST_F(StartingWindowTest, AddNodeOnRSTree06, Function | SmallTest | Level2)
{
    auto surfaceNode = CreateRSSurfaceNode();
    ASSERT_NE(nullptr, surfaceNode);
    node_->surfaceNode_ = surfaceNode;
    node_->leashWinSurfaceNode_ = nullptr;
    sptr<RSIWindowAnimationController> testController = new RSIWindowAnimationControllerMocker();
    ASSERT_EQ(WMError::WM_OK, RemoteAnimation::SetWindowAnimationController(testController));
    AnimationConfig config;
    StartingWindow::AddNodeOnRSTree(node_, config, true);
    ASSERT_EQ(WMError::WM_OK, StartingWindow::DrawStartingWindow(node_, nullptr, 0x66FFFFFF, false));
    usleep(200000);
}
}
}
}
