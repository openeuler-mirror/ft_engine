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
#include "display_manager.h"
#include "input_window_monitor.h"
#include "iremote_object_mocker.h"
#include "minimize_app.h"
#include "mock_RSIWindowAnimationController.h"
#include "remote_animation.h"
#include "starting_window.h"
#include "window_helper.h"
using namespace testing;
using namespace testing::ext;

namespace OHOS {
namespace Rosen {
namespace {
    constexpr uint32_t SLEEP_TIME_IN_US = 10000;
}

class RemoteAnimationTest : public testing::Test {
public:
    static void SetUpTestCase();
    static void TearDownTestCase();
    void SetUp() override;
    void TearDown() override;
    void InitRemoteAnimation();
    void CreateWindowNodeContainer();
private:
    RSSurfaceNode::SharedPtr CreateRSSurfaceNode(uint32_t windowId);
    sptr<WindowProperty> CreateWindowProperty(uint32_t windowId);
    Rect GetSurfaceBoundsRect(sptr<WindowNode> node);
    sptr<RSIWindowAnimationController> animationController_;
    sptr<WindowRoot> windowRoot_;
    sptr<WindowController> windowController_;
    std::shared_ptr<AppExecFwk::EventHandler> wmsTaskHandler_;
    sptr<WindowTransitionInfo> transitionInfo_ = nullptr;
    sptr<WindowNode> node_ = nullptr;
};

void RemoteAnimationTest::SetUpTestCase()
{
}

void RemoteAnimationTest::TearDownTestCase()
{
}

void RemoteAnimationTest::SetUp()
{
    InitRemoteAnimation();
    CreateWindowNodeContainer();
    transitionInfo_ = new WindowTransitionInfo();
    sptr<IRemoteObject> token = new IRemoteObjectMocker();
    transitionInfo_->SetAbilityToken(token);
    transitionInfo_->supportWindowModes_ = {
        AppExecFwk::SupportWindowMode::FULLSCREEN,
        AppExecFwk::SupportWindowMode::SPLIT,
        AppExecFwk::SupportWindowMode::FLOATING
    };
    windowController_->StartingWindow(transitionInfo_, nullptr, 0xFFFFFFFF, true);
    node_ = windowRoot_->FindWindowNodeWithToken(transitionInfo_->GetAbilityToken());
    EXPECT_NE(nullptr, node_);
}

void RemoteAnimationTest::TearDown()
{
    windowController_->DestroyWindow(node_->GetWindowId(), false);
    node_ = nullptr;
    animationController_ = nullptr;
    wmsTaskHandler_ = nullptr;
    windowRoot_ = nullptr;
    windowController_ = nullptr;
    transitionInfo_ = nullptr;
}

void RemoteAnimationTest::InitRemoteAnimation()
{
    animationController_ = new RSIWindowAnimationControllerMocker();
    EXPECT_EQ(WMError::WM_OK, RemoteAnimation::SetWindowAnimationController(animationController_));
    RemoteAnimation::isRemoteAnimationEnable_ = true;
    RemoteAnimation::animationFirst_ = true;
    windowRoot_ = new WindowRoot([](Event event, const sptr<IRemoteObject>& remoteObject) {});
    sptr<InputWindowMonitor> inputMonitor = new InputWindowMonitor(windowRoot_);
    windowController_ = new WindowController(windowRoot_, inputMonitor);
    RemoteAnimation::SetWindowControllerAndRoot(windowController_, windowRoot_);
    auto runner = AppExecFwk::EventRunner::Create("RemoteAnimationTest");
    wmsTaskHandler_ = std::make_shared<AppExecFwk::EventHandler>(runner);
    RemoteAnimation::SetMainTaskHandler(wmsTaskHandler_);
}

void RemoteAnimationTest::CreateWindowNodeContainer()
{
    auto display = DisplayManager::GetInstance().GetDefaultDisplay();
    ASSERT_TRUE((display != nullptr));
    sptr<DisplayInfo> displayInfo = display->GetDisplayInfo();
    ASSERT_TRUE((displayInfo != nullptr));
    windowRoot_->CreateWindowNodeContainer(displayInfo);
}

sptr<WindowProperty> RemoteAnimationTest::CreateWindowProperty(uint32_t windowId)
{
    sptr<WindowProperty> property = new WindowProperty();
    property->SetWindowId(windowId);
    return property;
}

Rect RemoteAnimationTest::GetSurfaceBoundsRect(sptr<WindowNode> node)
{
    if (!node->leashWinSurfaceNode_) {
        return {0, 0, 0, 0};
    }
    auto& stagingProperties = node->leashWinSurfaceNode_->GetStagingProperties();
    auto bounds = stagingProperties.GetBounds();
    Rect rect = {bounds[0], bounds[1], bounds[2], bounds[3]}; // 1, 2, 3 is index
    return rect;
}

RSSurfaceNode::SharedPtr RemoteAnimationTest::CreateRSSurfaceNode(uint32_t windowId)
{
    struct RSSurfaceNodeConfig rsSurfaceNodeConfig;
    rsSurfaceNodeConfig.SurfaceNodeName = "AppSurfaceNode" + std::to_string(windowId);
    auto surfaceNode = RSSurfaceNode::Create(rsSurfaceNodeConfig);
    return surfaceNode;
}

namespace {
/**
 * @tc.name: IsRemoteAnimationEnabledAndFirst01
 * @tc.desc: IsRemoteAnimationEnabledAndFirst return false since animationFirst false
 * @tc.type: FUNC
 */
HWTEST_F(RemoteAnimationTest, IsRemoteAnimationEnabledAndFirst01, Function | SmallTest | Level2)
{
    RemoteAnimation::SetAnimationFirst(false);
    EXPECT_EQ(false, RemoteAnimation::animationFirst_);
    EXPECT_EQ(false, RemoteAnimation::IsRemoteAnimationEnabledAndFirst(0));
}

/**
 * @tc.name: IsRemoteAnimationEnabledAndFirst02
 * @tc.desc: IsRemoteAnimationEnabledAndFirst return true since animationFirst true
 * @tc.type: FUNC
 */
HWTEST_F(RemoteAnimationTest, IsRemoteAnimationEnabledAndFirst02, Function | SmallTest | Level2)
{
    EXPECT_EQ(true, RemoteAnimation::animationFirst_);
    EXPECT_EQ(true, RemoteAnimation::IsRemoteAnimationEnabledAndFirst(0));
}

/**
 * @tc.name: IsRemoteAnimationEnabledAndFirst03
 * @tc.desc: IsRemoteAnimationEnabledAndFirst return false since CheckRemoteAnimationEnabled false
 * @tc.type: FUNC
 */
HWTEST_F(RemoteAnimationTest, IsRemoteAnimationEnabledAndFirst03, Function | SmallTest | Level2)
{
    RemoteAnimation::windowRoot_ = nullptr;
    EXPECT_EQ(false, RemoteAnimation::CheckRemoteAnimationEnabled(0));
    EXPECT_EQ(false, RemoteAnimation::IsRemoteAnimationEnabledAndFirst(0));
    RemoteAnimation::windowRoot_ = windowRoot_;
    auto container = RemoteAnimation::windowRoot_->GetOrCreateWindowNodeContainer(0);
    EXPECT_NE(nullptr, container);
    container->isScreenLocked_ = true;
    EXPECT_EQ(false, RemoteAnimation::IsRemoteAnimationEnabledAndFirst(0));
}

/**
 * @tc.name: IsRemoteAnimationEnabledAndFirst04
 * @tc.desc: return false since CheckRemoteAnimationEnabled false, set animationController nullptr
 * @tc.type: FUNC
 */
HWTEST_F(RemoteAnimationTest, IsRemoteAnimationEnabledAndFirst04, Function | SmallTest | Level2)
{
    sptr<RSIWindowAnimationController> controller = nullptr;
    EXPECT_EQ(WMError::WM_ERROR_NULLPTR, RemoteAnimation::SetWindowAnimationController(controller));
    RemoteAnimation::windowAnimationController_ = nullptr;
    EXPECT_EQ(false, RemoteAnimation::IsRemoteAnimationEnabledAndFirst(0));
    controller = new RSIWindowAnimationControllerMocker();
    RemoteAnimation::isRemoteAnimationEnable_ = false;
    EXPECT_EQ(WMError::WM_ERROR_NO_REMOTE_ANIMATION, RemoteAnimation::SetWindowAnimationController(controller));
    EXPECT_EQ(false, RemoteAnimation::CheckRemoteAnimationEnabled(0));
    EXPECT_EQ(false, RemoteAnimation::IsRemoteAnimationEnabledAndFirst(0));
    RemoteAnimation::isRemoteAnimationEnable_ = true;
    EXPECT_EQ(WMError::WM_OK, RemoteAnimation::SetWindowAnimationController(controller));
    EXPECT_EQ(true, RemoteAnimation::IsRemoteAnimationEnabledAndFirst(0));
    RemoteAnimation::SetAnimationFirst(false);
    EXPECT_EQ(false, RemoteAnimation::animationFirst_);
    EXPECT_EQ(false, RemoteAnimation::IsRemoteAnimationEnabledAndFirst(0));
}

/**
 * @tc.name: CheckTransition01
 * @tc.desc: CheckTransition return false
 * @tc.type: FUNC
 */
HWTEST_F(RemoteAnimationTest, CheckTransition01, Function | SmallTest | Level2)
{
    const sptr<WindowNode> node = nullptr;
    EXPECT_EQ(false, RemoteAnimation::CheckTransition(transitionInfo_, node, transitionInfo_, node));
    sptr<WindowNode> srcNode = StartingWindow::CreateWindowNode(transitionInfo_, 0);
    ASSERT_NE(nullptr, srcNode);
    srcNode->leashWinSurfaceNode_ = nullptr; // leash and app surface node is nullptr
    EXPECT_EQ(false, RemoteAnimation::CheckTransition(transitionInfo_, srcNode, transitionInfo_, srcNode));
    srcNode->surfaceNode_ = CreateRSSurfaceNode(0); // leash is null, but surfaceNode is not
    EXPECT_EQ(true, RemoteAnimation::CheckTransition(transitionInfo_, srcNode, transitionInfo_, srcNode));
    srcNode = StartingWindow::CreateWindowNode(transitionInfo_, 0);// leash and app surfaceNode both not nullptr
    ASSERT_NE(nullptr, srcNode);
    sptr<WindowNode> dstNode = StartingWindow::CreateWindowNode(transitionInfo_, 1);
    ASSERT_NE(nullptr, dstNode);
    EXPECT_EQ(true, RemoteAnimation::CheckTransition(transitionInfo_, node, transitionInfo_, dstNode));
    EXPECT_EQ(true, RemoteAnimation::CheckTransition(transitionInfo_, srcNode, transitionInfo_, node));
    dstNode->leashWinSurfaceNode_ = nullptr;
    EXPECT_EQ(false, RemoteAnimation::CheckTransition(transitionInfo_, srcNode, transitionInfo_, dstNode));
    dstNode->surfaceNode_ = CreateRSSurfaceNode(1); // leash is null, but surfaceNode is not
    EXPECT_EQ(true, RemoteAnimation::CheckTransition(transitionInfo_, srcNode, transitionInfo_, dstNode));
}

/**
 * @tc.name: CheckTransition02
 * @tc.desc: CheckTransition return false since windowMode not support
 * @tc.type: FUNC
 */
HWTEST_F(RemoteAnimationTest, CheckTransition02, Function | SmallTest | Level2)
{
    sptr<WindowNode> srcNode = StartingWindow::CreateWindowNode(transitionInfo_, 0);
    ASSERT_NE(nullptr, srcNode);
    sptr<WindowNode> dstNode = StartingWindow::CreateWindowNode(transitionInfo_, 1);
    ASSERT_NE(nullptr, dstNode);
    dstNode->SetWindowMode(WindowMode::WINDOW_MODE_FULLSCREEN);
    dstNode->SetModeSupportInfo(WindowModeSupport::WINDOW_MODE_SUPPORT_FLOATING);
    ASSERT_EQ(false, WindowHelper::CheckSupportWindowMode(dstNode->GetWindowMode(),
        dstNode->GetModeSupportInfo(), transitionInfo_));
    EXPECT_EQ(false, RemoteAnimation::CheckTransition(transitionInfo_, srcNode, transitionInfo_, dstNode));
}

/**
 * @tc.name: CheckTransition03
 * @tc.desc: CheckTransition return true
 * @tc.type: FUNC
 */
HWTEST_F(RemoteAnimationTest, CheckTransition03, Function | SmallTest | Level2)
{
    sptr<WindowNode> srcNode = StartingWindow::CreateWindowNode(transitionInfo_, 0);
    ASSERT_NE(nullptr, srcNode);
    sptr<WindowNode> dstNode = StartingWindow::CreateWindowNode(transitionInfo_, 1);
    ASSERT_NE(nullptr, dstNode);
    EXPECT_EQ(true, RemoteAnimation::CheckTransition(transitionInfo_, srcNode, nullptr, dstNode));
    EXPECT_EQ(true, RemoteAnimation::CheckTransition(transitionInfo_, srcNode, transitionInfo_, dstNode));
}

/**
 * @tc.name: OnRemoteDieAndCallbackTimeOutProcess01
 * @tc.desc: OnRemoteDie and setAnimationController nullptr
 * @tc.type: FUNC
 */
HWTEST_F(RemoteAnimationTest, OnRemoteDieAndCallbackTimeOutProcess01, Function | SmallTest | Level2)
{
    auto testController = RemoteAnimation::windowAnimationController_;
    ASSERT_EQ(true, RemoteAnimation::windowAnimationController_->AsObject() == animationController_->AsObject());
    RemoteAnimation::OnRemoteDie(animationController_->AsObject()); // controller is not nullptr
    EXPECT_EQ(false, RemoteAnimation::CheckAnimationController());
    RemoteAnimation::OnRemoteDie(testController->AsObject()); // controller is nullptr
    RemoteAnimation::SetAnimationFirst(false);
    RemoteAnimation::OnRemoteDie(testController->AsObject()); // controller is nullptr
}

/**
 * @tc.name: OnRemoteDieAndCallbackTimeOutProcess02
 * @tc.desc: OnRemoteDie and not set animation controller
 * @tc.type: FUNC
 */
HWTEST_F(RemoteAnimationTest, OnRemoteDieAndCallbackTimeOutProcess02, Function | SmallTest | Level2)
{
    sptr<IRemoteObject> remoteObject = nullptr;
    RemoteAnimation::windowRoot_ = nullptr;
    RemoteAnimation::OnRemoteDie(remoteObject); // controller is not nullptr
    EXPECT_EQ(true, RemoteAnimation::CheckAnimationController());
}

/**
 * @tc.name: OnRemoteDieAndCallbackTimeOutProcess03
 * @tc.desc: OnRemoteDie and timeout process success
 * @tc.type: FUNC
 */
HWTEST_F(RemoteAnimationTest, OnRemoteDieAndCallbackTimeOutProcess03, Function | SmallTest | Level2)
{
    auto root = RemoteAnimation::windowRoot_;
    node_->stateMachine_.TransitionTo(WindowNodeState::SHOW_ANIMATION_PLAYING);
    auto testController = RemoteAnimation::windowAnimationController_;
    RemoteAnimation::OnRemoteDie(testController->AsObject()); // controller is not nullptr
    EXPECT_EQ(false, RemoteAnimation::CheckAnimationController());
    EXPECT_EQ(true, node_->stateMachine_.currState_ == WindowNodeState::SHOW_ANIMATION_DONE);
}

/**
 * @tc.name: CreateWindowAnimationTarget01
 * @tc.desc: CreateWindowAnimationTarget with null node/different windowType
 * @tc.type: FUNC
 */
HWTEST_F(RemoteAnimationTest, CreateWindowAnimationTarget01, Function | SmallTest | Level2)
{
    sptr<WindowNode> node = nullptr;
    EXPECT_EQ(nullptr, RemoteAnimation::CreateWindowAnimationTarget(transitionInfo_, node));
    sptr<WindowNode> srcNode = new WindowNode(CreateWindowProperty(1)); // 1 is windowId
    EXPECT_EQ(nullptr, RemoteAnimation::CreateWindowAnimationTarget(transitionInfo_, srcNode)); // no surfaceNode
    srcNode->GetWindowProperty()->SetWindowType(WindowType::WINDOW_TYPE_DESKTOP);
    EXPECT_EQ(nullptr, RemoteAnimation::CreateWindowAnimationTarget(transitionInfo_, srcNode)); // no surfaceNode
    EXPECT_NE(nullptr, RemoteAnimation::CreateWindowAnimationTarget(transitionInfo_, node_));
}

/**
 * @tc.name: CreateShowAnimationFinishedCallback01
 * @tc.desc: CreateShowAnimationFinishedCallback with animationFirst false
 * @tc.type: FUNC
 */
HWTEST_F(RemoteAnimationTest, CreateShowAnimationFinishedCallback01, Function | SmallTest | Level2)
{
    RemoteAnimation::SetAnimationFirst(false);
    auto finishCallback = RemoteAnimation::CreateShowAnimationFinishedCallback(node_, node_, true);
    EXPECT_NE(nullptr, finishCallback);
    finishCallback->OnAnimationFinished(); // not animation playing
    usleep(SLEEP_TIME_IN_US);

    node_->stateMachine_.TransitionTo(WindowNodeState::SHOW_ANIMATION_PLAYING);
    finishCallback = RemoteAnimation::CreateShowAnimationFinishedCallback(node_, node_, true);
    EXPECT_NE(nullptr, finishCallback);
    finishCallback->OnAnimationFinished(); // leashSurfaceNode is not nullptr
    usleep(SLEEP_TIME_IN_US);

    sptr<WindowNode> dstNode = nullptr;
    finishCallback = RemoteAnimation::CreateShowAnimationFinishedCallback(node_, dstNode, true);
    EXPECT_NE(nullptr, finishCallback);
    finishCallback->OnAnimationFinished(); // dstNode is nullptr
    usleep(SLEEP_TIME_IN_US);

    dstNode = new WindowNode(CreateWindowProperty(1)); // leashSurfaceNode is nullptr
    dstNode->stateMachine_.TransitionTo(WindowNodeState::HIDE_ANIMATION_PLAYING);
    finishCallback = RemoteAnimation::CreateShowAnimationFinishedCallback(node_, dstNode, true);
    EXPECT_NE(nullptr, finishCallback);
    finishCallback->OnAnimationFinished();
    usleep(SLEEP_TIME_IN_US);
}

/**
 * @tc.name: CreateShowAnimationFinishedCallback02
 * @tc.desc: CreateShowAnimationFinishedCallback with animationFirst true
 * @tc.type: FUNC
 */
HWTEST_F(RemoteAnimationTest, CreateShowAnimationFinishedCallback02, Function | SmallTest | Level2)
{
    sptr<WindowNode> dstNode = nullptr;
    auto finishCallback = RemoteAnimation::CreateShowAnimationFinishedCallback(node_, dstNode, true);
    EXPECT_NE(nullptr, finishCallback);
    finishCallback->OnAnimationFinished(); // dstNode is nullptr
    usleep(SLEEP_TIME_IN_US);

    sptr<WindowNode> srcNode = nullptr;
    finishCallback = RemoteAnimation::CreateShowAnimationFinishedCallback(srcNode, node_, true);
    EXPECT_NE(nullptr, finishCallback);
    finishCallback->OnAnimationFinished(); // process srcNode state task with nullptr, node task count < 0
    usleep(SLEEP_TIME_IN_US);

    srcNode = nullptr;
    node_->stateMachine_.ResetAnimationTaskCount(2); // 2 is animationCount
    finishCallback = RemoteAnimation::CreateShowAnimationFinishedCallback(srcNode, node_, false);
    EXPECT_NE(nullptr, finishCallback);
    finishCallback->OnAnimationFinished(); // process srcNode state task with nullptr, node task count > 0
    usleep(SLEEP_TIME_IN_US);
}

/**
 * @tc.name: CreateHideAnimationFinishedCallback01
 * @tc.desc: CreateHideAnimationFinishedCallback with animationFirst false
 * @tc.type: FUNC
 */
HWTEST_F(RemoteAnimationTest, CreateHideAnimationFinishedCallback01, Function | SmallTest | Level2)
{
    RemoteAnimation::SetAnimationFirst(false);
    sptr<WindowNode> srcNode = nullptr;
    auto finishCallback = RemoteAnimation::CreateHideAnimationFinishedCallback(srcNode, TransitionEvent::MINIMIZE);
    EXPECT_NE(nullptr, finishCallback);
    finishCallback->OnAnimationFinished(); // srcNode is nullptr
    usleep(SLEEP_TIME_IN_US);

    finishCallback = RemoteAnimation::CreateHideAnimationFinishedCallback(node_, TransitionEvent::MINIMIZE);
    EXPECT_NE(nullptr, finishCallback);
    finishCallback->OnAnimationFinished(); // not hide animation playing
    usleep(SLEEP_TIME_IN_US);
    EXPECT_NE(WindowNodeState::HIDE_ANIMATION_DONE, node_->stateMachine_.currState_);

    node_->stateMachine_.TransitionTo(WindowNodeState::HIDE_ANIMATION_PLAYING);
    finishCallback = RemoteAnimation::CreateHideAnimationFinishedCallback(node_, TransitionEvent::CLOSE);
    EXPECT_NE(nullptr, finishCallback);
    finishCallback->OnAnimationFinished(); // hide animation playing and with close
    usleep(SLEEP_TIME_IN_US);
    EXPECT_EQ(WindowNodeState::HIDE_ANIMATION_DONE, node_->stateMachine_.currState_);

    node_->stateMachine_.TransitionTo(WindowNodeState::HIDE_ANIMATION_PLAYING);
    finishCallback = RemoteAnimation::CreateHideAnimationFinishedCallback(node_, TransitionEvent::MINIMIZE);
    EXPECT_NE(nullptr, finishCallback);
    finishCallback->OnAnimationFinished(); // hide animation playing and with MINIMIZE
    usleep(SLEEP_TIME_IN_US);
    EXPECT_EQ(WindowNodeState::HIDE_ANIMATION_DONE, node_->stateMachine_.currState_);

    node_->stateMachine_.TransitionTo(WindowNodeState::STARTING_CREATED);
    node_->abilityToken_ = nullptr;
    finishCallback = RemoteAnimation::CreateHideAnimationFinishedCallback(node_, TransitionEvent::MINIMIZE);
    EXPECT_NE(nullptr, finishCallback);
    finishCallback->OnAnimationFinished(); // abilityToken is nullptr
    usleep(SLEEP_TIME_IN_US);
    EXPECT_EQ(WindowNodeState::STARTING_CREATED, node_->stateMachine_.currState_);
}

/**
 * @tc.name: CreateHideAnimationFinishedCallback02
 * @tc.desc: CreateHideAnimationFinishedCallback with animationFirst true
 * @tc.type: FUNC
 */
HWTEST_F(RemoteAnimationTest, CreateHideAnimationFinishedCallback02, Function | SmallTest | Level2)
{
    sptr<WindowNode> srcNode = nullptr;
    auto finishCallback = RemoteAnimation::CreateHideAnimationFinishedCallback(srcNode, TransitionEvent::CLOSE);
    EXPECT_NE(nullptr, finishCallback);
    finishCallback->OnAnimationFinished(); // srcNode is nullptr
    usleep(SLEEP_TIME_IN_US);

    finishCallback = RemoteAnimation::CreateHideAnimationFinishedCallback(node_, TransitionEvent::MINIMIZE);
    EXPECT_NE(nullptr, finishCallback);
    finishCallback->OnAnimationFinished(); // with minimize
    usleep(SLEEP_TIME_IN_US);

    node_->firstFrameAvaliable_ = true;
    finishCallback = RemoteAnimation::CreateHideAnimationFinishedCallback(node_, TransitionEvent::CLOSE);
    EXPECT_NE(nullptr, finishCallback);
    finishCallback->OnAnimationFinished(); // create hide callback with firstFrameAvaliable_ true
    usleep(SLEEP_TIME_IN_US);

    node_->firstFrameAvaliable_ = false;
    finishCallback = RemoteAnimation::CreateHideAnimationFinishedCallback(node_, TransitionEvent::CLOSE);
    EXPECT_NE(nullptr, finishCallback);
    finishCallback->OnAnimationFinished(); // create hide callback with surfaceNode null
    usleep(SLEEP_TIME_IN_US);

    node_->firstFrameAvaliable_ = false;
    node_->surfaceNode_ = CreateRSSurfaceNode(1);
    finishCallback = RemoteAnimation::CreateHideAnimationFinishedCallback(node_, TransitionEvent::CLOSE);
    EXPECT_NE(nullptr, finishCallback);
    finishCallback->OnAnimationFinished(); // create hide callback with surfaceNode not null
    usleep(SLEEP_TIME_IN_US);

    node_->leashWinSurfaceNode_ = CreateRSSurfaceNode(1);
    finishCallback = RemoteAnimation::CreateHideAnimationFinishedCallback(node_, TransitionEvent::CLOSE);
    EXPECT_NE(nullptr, finishCallback);
    finishCallback->OnAnimationFinished(); // create hide callback with leashWinSurfaceNode_ null
    usleep(SLEEP_TIME_IN_US);
}

/**
 * @tc.name: ProcessNodeStateTask01
 * @tc.desc: ProcessNodeStateTask with different node state
 * @tc.type: FUNC
 */
HWTEST_F(RemoteAnimationTest, ProcessNodeStateTask01, Function | SmallTest | Level2)
{
    // ExecuteFinalStateTask with task is nullptr
    node_->stateMachine_.ResetAnimationTaskCount(1);
    RemoteAnimation::ProcessNodeStateTask(node_);
    usleep(SLEEP_TIME_IN_US);
    EXPECT_EQ(WindowNodeState::STARTING_CREATED, node_->stateMachine_.currState_);

    node_->stateMachine_.ResetAnimationTaskCount(1);
    node_->stateMachine_.TransitionTo(WindowNodeState::HIDE_ANIMATION_PLAYING);
    RemoteAnimation::ProcessNodeStateTask(node_); // hide animation playing
    usleep(SLEEP_TIME_IN_US);
    EXPECT_EQ(WindowNodeState::HIDE_ANIMATION_DONE, node_->stateMachine_.currState_);

    RemoteAnimation::windowRoot_ = nullptr;
    node_->stateMachine_.ResetAnimationTaskCount(1);
    RemoteAnimation::ProcessNodeStateTask(node_); // hide animation playing and windowRoot is nullptr
    usleep(SLEEP_TIME_IN_US);
    EXPECT_EQ(WindowNodeState::HIDE_ANIMATION_DONE, node_->stateMachine_.currState_);

    RemoteAnimation::windowRoot_ = windowRoot_;
    node_->stateMachine_.TransitionTo(WindowNodeState::SHOW_ANIMATION_PLAYING);
    node_->stateMachine_.ResetAnimationTaskCount(1);
    RemoteAnimation::ProcessNodeStateTask(node_); // show animation playing
    usleep(SLEEP_TIME_IN_US);
    EXPECT_EQ(WindowNodeState::SHOW_ANIMATION_DONE, node_->stateMachine_.currState_);

    bool taskExecute = false;
    node_->stateMachine_.SetDestroyTask([&taskExecute] {taskExecute = true;});
    node_->stateMachine_.ResetAnimationTaskCount(1);
    RemoteAnimation::ProcessNodeStateTask(node_); // execute destroy task
    usleep(SLEEP_TIME_IN_US);
    EXPECT_EQ(true, taskExecute);

    RemoteAnimation::windowRoot_ = nullptr;
    node_->stateMachine_.ResetAnimationTaskCount(1);
    RemoteAnimation::ProcessNodeStateTask(node_); // show animation playing and windowRoot is nullptr
    usleep(SLEEP_TIME_IN_US);
    EXPECT_EQ(WindowNodeState::SHOW_ANIMATION_DONE, node_->stateMachine_.currState_);

    wmsTaskHandler_ = nullptr;
    RemoteAnimation::wmsTaskHandler_ = wmsTaskHandler_;
    node_->stateMachine_.ResetAnimationTaskCount(1);
    node_->stateMachine_.SetDestroyTask([&taskExecute] {taskExecute = false;});
    RemoteAnimation::ExecuteFinalStateTask(node_); // handler is nullptr
    usleep(SLEEP_TIME_IN_US);
    EXPECT_EQ(true, taskExecute);
}

/**
 * @tc.name: PostProcessShowCallback01
 * @tc.desc: PostProcessShowCallback with different leashWinSurfaceNode
 * @tc.type: FUNC
 */
HWTEST_F(RemoteAnimationTest, PostProcessShowCallback01, Function | SmallTest | Level2)
{
    sptr<WindowNode> dstNode = nullptr;
    RemoteAnimation::PostProcessShowCallback(dstNode);
    usleep(SLEEP_TIME_IN_US);

    Rect expectRect = {0, 0, 100, 100}; // 100 is test data
    node_->SetWindowRect(expectRect);
    RemoteAnimation::PostProcessShowCallback(node_);
    usleep(SLEEP_TIME_IN_US);
    Rect actualRect = GetSurfaceBoundsRect(node_);
    EXPECT_EQ(expectRect, actualRect);
    node_->leashWinSurfaceNode_ = nullptr;
    RemoteAnimation::PostProcessShowCallback(node_);
}

/**
 * @tc.name: GetTransitionEvent01
 * @tc.desc: GetTransitionEvent with reason not ability_transition
 * @tc.type: FUNC
 */
HWTEST_F(RemoteAnimationTest, GetTransitionEvent01, Function | SmallTest | Level2)
{
    transitionInfo_->SetTransitionReason(TransitionReason::MINIMIZE);
    auto event = RemoteAnimation::GetTransitionEvent(transitionInfo_, transitionInfo_, node_, node_);
    EXPECT_EQ(TransitionEvent::MINIMIZE, event);

    transitionInfo_->SetTransitionReason(TransitionReason::BACK_TRANSITION);
    event = RemoteAnimation::GetTransitionEvent(transitionInfo_, transitionInfo_, node_, node_);
    EXPECT_EQ(TransitionEvent::BACK_TRANSITION, event);

    transitionInfo_->SetTransitionReason(TransitionReason::CLOSE);
    event = RemoteAnimation::GetTransitionEvent(transitionInfo_, transitionInfo_, node_, node_);
    EXPECT_EQ(TransitionEvent::CLOSE, event);

    node_->stateMachine_.TransitionTo(WindowNodeState::HIDE_ANIMATION_PLAYING);
    transitionInfo_->SetTransitionReason(TransitionReason::CLOSE);
    event = RemoteAnimation::GetTransitionEvent(transitionInfo_, transitionInfo_, node_, node_);
    EXPECT_EQ(TransitionEvent::UNKNOWN, event);
}

/**
 * @tc.name: GetTransitionEvent02
 * @tc.desc: GetTransitionEvent with reason ability_transition
 * @tc.type: FUNC
 */
HWTEST_F(RemoteAnimationTest, GetTransitionEvent02, Function | SmallTest | Level2)
{
    sptr<WindowNode> srcNode = nullptr;
    transitionInfo_->SetTransitionReason(TransitionReason::ABILITY_TRANSITION);
    auto event = RemoteAnimation::GetTransitionEvent(transitionInfo_, transitionInfo_, srcNode, node_);
    EXPECT_EQ(TransitionEvent::APP_TRANSITION, event);

    sptr<WindowNode> dstNode = nullptr;
    event = RemoteAnimation::GetTransitionEvent(transitionInfo_, transitionInfo_, node_, dstNode);
    EXPECT_EQ(TransitionEvent::UNKNOWN, event);

    node_->stateMachine_.TransitionTo(WindowNodeState::SHOW_ANIMATION_PLAYING);
    event = RemoteAnimation::GetTransitionEvent(transitionInfo_, transitionInfo_, node_, node_);
    EXPECT_EQ(TransitionEvent::UNKNOWN, event);

    transitionInfo_->SetWindowType(WindowType::WINDOW_TYPE_DESKTOP);
    event = RemoteAnimation::GetTransitionEvent(transitionInfo_, transitionInfo_, node_, node_);
    EXPECT_EQ(TransitionEvent::HOME, event);

    transitionInfo_->SetWindowType(WindowType::WINDOW_TYPE_FLOAT);
    event = RemoteAnimation::GetTransitionEvent(transitionInfo_, transitionInfo_, node_, node_);
    EXPECT_EQ(TransitionEvent::UNKNOWN, event);

    transitionInfo_->SetAbilityToken(nullptr);
    event = RemoteAnimation::GetTransitionEvent(transitionInfo_, transitionInfo_, node_, dstNode);
    EXPECT_EQ(TransitionEvent::UNKNOWN, event);
}

/**
 * @tc.name: GetExpectRect01
 * @tc.desc: GetExpectRect
 * @tc.type: FUNC
 */
HWTEST_F(RemoteAnimationTest, GetExpectRect01, Function | SmallTest | Level2)
{
    auto target = RemoteAnimation::CreateWindowAnimationTarget(transitionInfo_, node_);
    RemoteAnimation::GetExpectRect(node_, target);
    Rect actualRect = GetSurfaceBoundsRect(node_);
    EXPECT_EQ(node_->GetWindowRect(), actualRect); // avoidRect is empty thus return

    sptr<WindowNode> statusBar = new WindowNode(CreateWindowProperty(0));
    ASSERT_NE(nullptr, statusBar);
    statusBar->GetWindowProperty()->SetWindowType(WindowType::WINDOW_TYPE_STATUS_BAR);
    statusBar->SetWindowRect({0, 0, 100, 100});
    windowRoot_->windowNodeMap_[0] = statusBar;

    Rect avoidRect = windowRoot_->GetDisplayRectWithoutSystemBarAreas(node_->GetDisplayId());
    EXPECT_FALSE(WindowHelper::IsEmptyRect(avoidRect));

    RemoteAnimation::GetExpectRect(node_, target);
    actualRect = GetSurfaceBoundsRect(node_);
    EXPECT_EQ(avoidRect, actualRect); // get expect rect

    node_->leashWinSurfaceNode_ = nullptr;
    RemoteAnimation::GetExpectRect(node_, target);

    node_->GetWindowProperty()->SetWindowMode(WindowMode::WINDOW_MODE_FLOATING);
    RemoteAnimation::GetExpectRect(node_, target);
    EXPECT_FALSE(WindowHelper::IsMainFullScreenWindow(node_->GetWindowType(), node_->GetWindowMode()));

    RemoteAnimation::windowRoot_ = nullptr;
    RemoteAnimation::GetExpectRect(node_, target);

    node_->GetWindowProperty()->SetWindowFlags(0);
    RemoteAnimation::GetExpectRect(node_, target);
    bool needAvoid = (node_->GetWindowFlags() & static_cast<uint32_t>(WindowFlag::WINDOW_FLAG_NEED_AVOID));
    EXPECT_EQ(false, needAvoid);
}

/**
 * @tc.name: NotifyAnimationTransition01
 * @tc.desc: NotifyAnimationTransition failed
 * @tc.type: FUNC
 */
HWTEST_F(RemoteAnimationTest, NotifyAnimationTransition01, Function | SmallTest | Level2)
{
    sptr<WindowNode> dstNode = nullptr;
    WMError ret = RemoteAnimation::NotifyAnimationTransition(transitionInfo_, transitionInfo_, node_, dstNode);
    EXPECT_EQ(WMError::WM_ERROR_NULLPTR, ret); //
    dstNode = new WindowNode(CreateWindowProperty(1)); // leashSurfaceNode is nullptr
    ret = RemoteAnimation::NotifyAnimationTransition(transitionInfo_, transitionInfo_, node_, dstNode);
    EXPECT_EQ(WMError::WM_ERROR_NO_MEM, ret); // no surfaceNode thus target is nullptr
}

/**
 * @tc.name: NotifyAnimationTransition02
 * @tc.desc: NotifyAnimationTransition with OnAppTransition
 * @tc.type: FUNC
 */
HWTEST_F(RemoteAnimationTest, NotifyAnimationTransition02, Function | SmallTest | Level2)
{
    sptr<WindowNode> srcNode = new WindowNode(CreateWindowProperty(2)); // 2 is windowId
    MinimizeApp::AddNeedMinimizeApp(srcNode, MinimizeReason::OTHER_WINDOW);

    WMError ret = RemoteAnimation::NotifyAnimationTransition(transitionInfo_, transitionInfo_, srcNode, node_);
    EXPECT_EQ(WMError::WM_OK, ret); //
    EXPECT_EQ(0, MinimizeApp::GetNeedMinimizeAppNodesWithReason(MinimizeReason::OTHER_WINDOW).size());

    RemoteAnimation::SetAnimationFirst(false);
    MinimizeApp::AddNeedMinimizeApp(srcNode, MinimizeReason::OTHER_WINDOW);

    ret = RemoteAnimation::NotifyAnimationTransition(transitionInfo_, transitionInfo_, srcNode, node_);
    EXPECT_EQ(WMError::WM_OK, ret); //
    EXPECT_EQ(1, MinimizeApp::GetNeedMinimizeAppNodesWithReason(MinimizeReason::OTHER_WINDOW).size());

    RemoteAnimation::windowController_ = nullptr;
    ret = RemoteAnimation::NotifyAnimationTransition(transitionInfo_, transitionInfo_, srcNode, node_);
    EXPECT_EQ(WMError::WM_OK, ret);
}

/**
 * @tc.name: NotifyAnimationTransition03
 * @tc.desc: NotifyAnimationTransition with NotifyAnimationStartApp
 * @tc.type: FUNC
 */
HWTEST_F(RemoteAnimationTest, NotifyAnimationTransition03, Function | SmallTest | Level2)
{
    sptr<WindowNode> srcNode = new WindowNode(CreateWindowProperty(2)); // 2 is windowId
    srcNode->GetWindowProperty()->SetWindowType(WindowType::WINDOW_TYPE_DESKTOP);
    node_->surfaceNode_ = CreateRSSurfaceNode(2);
    WMError ret = RemoteAnimation::NotifyAnimationTransition(transitionInfo_, transitionInfo_, srcNode, node_);
    EXPECT_EQ(WMError::WM_OK, ret); // srcNode is nullptr, from launcher

    srcNode->GetWindowProperty()->SetWindowType(WindowType::WINDOW_TYPE_APP_MAIN_WINDOW);
    transitionInfo_->SetIsRecent(true);
    ret = RemoteAnimation::NotifyAnimationTransition(transitionInfo_, transitionInfo_, srcNode, node_);
    EXPECT_EQ(WMError::WM_OK, ret); // srcNode is nullptr, from recent

    srcNode = nullptr;
    transitionInfo_->SetIsRecent(false);
    ret = RemoteAnimation::NotifyAnimationTransition(transitionInfo_, transitionInfo_, srcNode, node_);
    EXPECT_EQ(WMError::WM_OK, ret); // srcNode is null && need minimize false, from other

    MinimizeApp::AddNeedMinimizeApp(srcNode, MinimizeReason::OTHER_WINDOW);
    ret = RemoteAnimation::NotifyAnimationTransition(transitionInfo_, transitionInfo_, srcNode, node_);
    EXPECT_EQ(WMError::WM_OK, ret); // srcNode is nullptr, but need minimize true, from other

    RemoteAnimation::SetAnimationFirst(false);
    ret = RemoteAnimation::NotifyAnimationTransition(transitionInfo_, transitionInfo_, srcNode, node_);
    EXPECT_EQ(WMError::WM_OK, ret); // srcNode is nullptr, but need minimize true, from other
}

/**
 * @tc.name: NotifyAnimationMinimize01
 * @tc.desc: NotifyAnimationMinimize
 * @tc.type: FUNC
 */
HWTEST_F(RemoteAnimationTest, NotifyAnimationMinimize01, Function | SmallTest | Level2)
{
    WMError ret = RemoteAnimation::NotifyAnimationMinimize(transitionInfo_, node_);
    EXPECT_EQ(WMError::WM_OK, ret);

    RemoteAnimation::windowController_ = nullptr;
    ret = RemoteAnimation::NotifyAnimationMinimize(transitionInfo_, node_);
    EXPECT_EQ(WMError::WM_OK, ret);

    node_->leashWinSurfaceNode_ = nullptr;
    ret = RemoteAnimation::NotifyAnimationMinimize(transitionInfo_, node_);
    EXPECT_EQ(WMError::WM_ERROR_NO_MEM, ret); // srcTarget is nullptr
}

/**
 * @tc.name: NotifyAnimationClose01
 * @tc.desc: NotifyAnimationClose
 * @tc.type: FUNC
 */
HWTEST_F(RemoteAnimationTest, NotifyAnimationClose01, Function | SmallTest | Level2)
{
    WMError ret = RemoteAnimation::NotifyAnimationClose(transitionInfo_, node_, TransitionEvent::CLOSE);
    EXPECT_EQ(WMError::WM_OK, ret);

    RemoteAnimation::windowController_ = nullptr;
    ret = RemoteAnimation::NotifyAnimationClose(transitionInfo_, node_, TransitionEvent::CLOSE);
    EXPECT_EQ(WMError::WM_OK, ret);

    node_->leashWinSurfaceNode_ = nullptr;
    ret = RemoteAnimation::NotifyAnimationClose(transitionInfo_, node_, TransitionEvent::CLOSE);
    EXPECT_EQ(WMError::WM_ERROR_NO_MEM, ret); // srcTarget is nullptr
}

/**
 * @tc.name: NotifyAnimationBackTransition01
 * @tc.desc: NotifyAnimationBackTransition failed
 * @tc.type: FUNC
 */
HWTEST_F(RemoteAnimationTest, NotifyAnimationBackTransition01, Function | SmallTest | Level2)
{
    sptr<WindowNode> srcNode = new WindowNode(CreateWindowProperty(2)); // 2 is windowId
    srcNode->leashWinSurfaceNode_ = nullptr;
    WMError ret = RemoteAnimation::NotifyAnimationBackTransition(transitionInfo_, transitionInfo_, srcNode, node_);
    EXPECT_EQ(WMError::WM_ERROR_NO_MEM, ret); // only src Target is null

    sptr<WindowNode> dstNode = new WindowNode(CreateWindowProperty(3)); // 3 is windowId
    dstNode->leashWinSurfaceNode_ = nullptr;
    ret = RemoteAnimation::NotifyAnimationBackTransition(transitionInfo_, transitionInfo_, node_, dstNode);
    EXPECT_EQ(WMError::WM_ERROR_NO_MEM, ret); // only dstTarget is null

    ret = RemoteAnimation::NotifyAnimationBackTransition(transitionInfo_, transitionInfo_, srcNode, dstNode);
    EXPECT_EQ(WMError::WM_ERROR_NO_MEM, ret); // both srcTarget and dstTarget art null

    srcNode = nullptr;
    ret = RemoteAnimation::NotifyAnimationBackTransition(transitionInfo_, transitionInfo_, srcNode, node_);
    EXPECT_EQ(WMError::WM_ERROR_NULLPTR, ret); // only srcNode is null

    dstNode = nullptr;
    ret = RemoteAnimation::NotifyAnimationBackTransition(transitionInfo_, transitionInfo_, node_, dstNode);
    EXPECT_EQ(WMError::WM_ERROR_NULLPTR, ret); // only dstNode is null

    ret = RemoteAnimation::NotifyAnimationBackTransition(transitionInfo_, transitionInfo_, srcNode, dstNode);
    EXPECT_EQ(WMError::WM_ERROR_NULLPTR, ret); // both srcNode and dstNode are null

    RemoteAnimation::SetAnimationFirst(false);
    ret = RemoteAnimation::NotifyAnimationBackTransition(transitionInfo_, transitionInfo_, node_, node_);
    EXPECT_EQ(WMError::WM_ERROR_NO_REMOTE_ANIMATION, ret);
}

/**
 * @tc.name: NotifyAnimationBackTransition02
 * @tc.desc: NotifyAnimationBackTransition success
 * @tc.type: FUNC
 */
HWTEST_F(RemoteAnimationTest, NotifyAnimationBackTransition02, Function | SmallTest | Level2)
{
    sptr<WindowNode> srcNode = new WindowNode(CreateWindowProperty(2)); // 2 is windowId
    srcNode->leashWinSurfaceNode_ = CreateRSSurfaceNode(2); // 2 is windowId
    WMError ret = RemoteAnimation::NotifyAnimationBackTransition(transitionInfo_, transitionInfo_, srcNode, node_);
    EXPECT_EQ(WMError::WM_OK, ret);
    RSIWindowAnimationControllerMocker* testController = reinterpret_cast<RSIWindowAnimationControllerMocker*>(
        animationController_.GetRefPtr());
    EXPECT_NE(nullptr, testController->finishedCallback_);
    testController->finishedCallback_->OnAnimationFinished();
    usleep(SLEEP_TIME_IN_US);

    RemoteAnimation::windowController_ = nullptr;
    ret = RemoteAnimation::NotifyAnimationBackTransition(transitionInfo_, transitionInfo_, srcNode, node_);
    EXPECT_EQ(WMError::WM_OK, ret);
    testController = reinterpret_cast<RSIWindowAnimationControllerMocker*>(
        animationController_.GetRefPtr());
    EXPECT_NE(nullptr, testController->finishedCallback_);
    testController->finishedCallback_->OnAnimationFinished();
    usleep(SLEEP_TIME_IN_US);
}

/**
 * @tc.name: NotifyAnimationByHome01
 * @tc.desc: NotifyAnimationByHome with animationFirst false and create animationTarget failed
 * @tc.type: FUNC
 */
HWTEST_F(RemoteAnimationTest, NotifyAnimationByHome01, Function | SmallTest | Level2)
{
    RemoteAnimation::SetAnimationFirst(false);
    WMError ret = RemoteAnimation::NotifyAnimationByHome();
    EXPECT_EQ(WMError::WM_OK, ret); // no node need home

    sptr<WindowNode> srcNode = new WindowNode(CreateWindowProperty(2)); // 2 is windowId
    MinimizeApp::AddNeedMinimizeApp(srcNode, MinimizeReason::MINIMIZE_ALL);
    ret = RemoteAnimation::NotifyAnimationByHome();
    EXPECT_EQ(WMError::WM_OK, ret); // create animationTarget failed with no surface, and no callback

    srcNode->leashWinSurfaceNode_ = CreateRSSurfaceNode(2); // 2 is windowId
    srcNode->GetWindowProperty()->SetWindowType(WindowType::WINDOW_TYPE_DESKTOP);
    ret = RemoteAnimation::NotifyAnimationByHome();
    EXPECT_EQ(WMError::WM_OK, ret); // create animationTarget failed, and no callback
    RSIWindowAnimationControllerMocker* testController = reinterpret_cast<RSIWindowAnimationControllerMocker*>(
        animationController_.GetRefPtr());
    EXPECT_NE(nullptr, testController->finishedCallback_);
    testController->finishedCallback_->OnAnimationFinished();
    usleep(SLEEP_TIME_IN_US);
    EXPECT_NE(WindowNodeState::HIDE_ANIMATION_DONE, srcNode->stateMachine_.currState_);

    srcNode->stateMachine_.TransitionTo(WindowNodeState::HIDE_ANIMATION_PLAYING);
    ret = RemoteAnimation::NotifyAnimationByHome();
    EXPECT_EQ(WMError::WM_OK, ret); // create animationTarget failed, and no callback

    srcNode->GetWindowProperty()->SetWindowType(WindowType::WINDOW_TYPE_APP_MAIN_WINDOW);
    ret = RemoteAnimation::NotifyAnimationByHome();
    EXPECT_EQ(WMError::WM_OK, ret); // create animationTarget failed, and no callback
}

/*
 * @tc.name: NotifyAnimationByHome02
 * @tc.desc: NotifyAnimationByHome with animationFirst false and create animationTarget success
 * @tc.type: FUNC
 */
HWTEST_F(RemoteAnimationTest, NotifyAnimationByHome02, Function | SmallTest | Level2)
{
    RemoteAnimation::SetAnimationFirst(false);
    sptr<WindowNode> srcNode = new WindowNode(CreateWindowProperty(2)); // 2 is windowId
    srcNode->leashWinSurfaceNode_ = CreateRSSurfaceNode(2); // 2 is windowId
    MinimizeApp::AddNeedMinimizeApp(srcNode, MinimizeReason::MINIMIZE_ALL);
    WMError ret = RemoteAnimation::NotifyAnimationByHome();
    EXPECT_EQ(WMError::WM_OK, ret);

    RSIWindowAnimationControllerMocker* testController = reinterpret_cast<RSIWindowAnimationControllerMocker*>(
        animationController_.GetRefPtr());
    EXPECT_NE(nullptr, testController->finishedCallback_);
    testController->finishedCallback_->OnAnimationFinished();
    usleep(SLEEP_TIME_IN_US);
    EXPECT_EQ(WindowNodeState::HIDE_ANIMATION_DONE, srcNode->stateMachine_.currState_);
}

/*
 * @tc.name: NotifyAnimationByHome03
 * @tc.desc: NotifyAnimationByHome with animationFirst true and create animationTarget success
 * @tc.type: FUNC
 */
HWTEST_F(RemoteAnimationTest, NotifyAnimationByHome03, Function | SmallTest | Level2)
{
    sptr<WindowNode> srcNode = new WindowNode(CreateWindowProperty(2)); // 2 is windowId
    srcNode->leashWinSurfaceNode_ = CreateRSSurfaceNode(2); // 2 is windowId
    MinimizeApp::AddNeedMinimizeApp(srcNode, MinimizeReason::MINIMIZE_ALL);
    EXPECT_EQ(1, MinimizeApp::GetNeedMinimizeAppNodesWithReason(MinimizeReason::MINIMIZE_ALL).size());
    WMError ret = RemoteAnimation::NotifyAnimationByHome();
    EXPECT_EQ(WMError::WM_OK, ret);
    EXPECT_EQ(0, MinimizeApp::GetNeedMinimizeAppNodesWithReason(MinimizeReason::MINIMIZE_ALL).size());

    RSIWindowAnimationControllerMocker* testController = reinterpret_cast<RSIWindowAnimationControllerMocker*>(
        animationController_.GetRefPtr());
    EXPECT_NE(nullptr, testController->finishedCallback_);
    testController->finishedCallback_->OnAnimationFinished();
    usleep(SLEEP_TIME_IN_US);
    EXPECT_EQ(WindowNodeState::HIDE_ANIMATION_DONE, srcNode->stateMachine_.currState_);
}

/*
 * @tc.name: NotifyAnimationTargetsUpdate01
 * @tc.desc: NotifyAnimationTargetsUpdate failed
 * @tc.type: FUNC
 */
HWTEST_F(RemoteAnimationTest, NotifyAnimationTargetsUpdate01, Function | SmallTest | Level2)
{
    std::vector<uint32_t> fullScreenWinIds;
    std::vector<uint32_t> floatWinIds;
    RemoteAnimation::NotifyAnimationTargetsUpdate(fullScreenWinIds, floatWinIds); // fullScreenAnimationTarget is empty
    usleep(SLEEP_TIME_IN_US);
    RSIWindowAnimationControllerMocker* testController = reinterpret_cast<RSIWindowAnimationControllerMocker*>(
        animationController_.GetRefPtr());
    EXPECT_EQ(nullptr, testController->animationTarget_);
    EXPECT_EQ(0, testController->floatingWindowTargets_.size());

    RemoteAnimation::windowRoot_ = nullptr;
    RemoteAnimation::NotifyAnimationTargetsUpdate(fullScreenWinIds, floatWinIds);
    usleep(SLEEP_TIME_IN_US);
    testController = reinterpret_cast<RSIWindowAnimationControllerMocker*>(
        animationController_.GetRefPtr());
    EXPECT_EQ(nullptr, testController->animationTarget_);
    EXPECT_EQ(0, testController->floatingWindowTargets_.size());

    RemoteAnimation::windowAnimationController_ = nullptr;
    RemoteAnimation::NotifyAnimationTargetsUpdate(fullScreenWinIds, floatWinIds);
    usleep(SLEEP_TIME_IN_US);
    testController = reinterpret_cast<RSIWindowAnimationControllerMocker*>(
        animationController_.GetRefPtr());
    EXPECT_EQ(nullptr, testController->animationTarget_);
    EXPECT_EQ(0, testController->floatingWindowTargets_.size());

    wmsTaskHandler_ = nullptr;
    RemoteAnimation::wmsTaskHandler_ = wmsTaskHandler_;
    RemoteAnimation::NotifyAnimationTargetsUpdate(fullScreenWinIds, floatWinIds);
    usleep(SLEEP_TIME_IN_US);
    testController = reinterpret_cast<RSIWindowAnimationControllerMocker*>(
        animationController_.GetRefPtr());
    EXPECT_EQ(nullptr, testController->animationTarget_);
    EXPECT_EQ(0, testController->floatingWindowTargets_.size());
}

/*
 * @tc.name: NotifyAnimationTargetsUpdate02
 * @tc.desc: NotifyAnimationTargetsUpdate success
 * @tc.type: FUNC
 */
HWTEST_F(RemoteAnimationTest, NotifyAnimationTargetsUpdate02, Function | SmallTest | Level2)
{
    std::vector<uint32_t> fullScreenWinIds;
    std::vector<uint32_t> floatWinIds;
    fullScreenWinIds.push_back(2); // 2 is windowId
    floatWinIds.push_back(2); // 2 is windowId
    RemoteAnimation::NotifyAnimationTargetsUpdate(fullScreenWinIds, floatWinIds); // CreateWindowAnimationTarget nullptr
    usleep(SLEEP_TIME_IN_US);
    RSIWindowAnimationControllerMocker* testController = reinterpret_cast<RSIWindowAnimationControllerMocker*>(
        animationController_.GetRefPtr());
    EXPECT_EQ(nullptr, testController->animationTarget_);
    EXPECT_EQ(0, testController->floatingWindowTargets_.size());

    fullScreenWinIds.push_back(node_->GetWindowId()); // 1, 2 in vector
    floatWinIds.push_back(node_->GetWindowId()); // CreateWindowAnimationTarget success
    RemoteAnimation::NotifyAnimationTargetsUpdate(fullScreenWinIds, floatWinIds);
    usleep(SLEEP_TIME_IN_US);
    testController = reinterpret_cast<RSIWindowAnimationControllerMocker*>(
        animationController_.GetRefPtr());
    EXPECT_NE(nullptr, testController->animationTarget_);
    EXPECT_EQ(1, testController->floatingWindowTargets_.size());
}

/*
 * @tc.name: NotifyAnimationScreenUnlock01
 * @tc.desc: NotifyAnimationScreenUnlock success
 * @tc.type: FUNC
 */
HWTEST_F(RemoteAnimationTest, NotifyAnimationScreenUnlock01, Function | SmallTest | Level2)
{
    std::function<void(void)> callback = nullptr;
    WMError ret = RemoteAnimation::NotifyAnimationScreenUnlock(callback);
    EXPECT_EQ(WMError::WM_ERROR_NO_MEM, ret);

    callback = []() {};
    ret = RemoteAnimation::NotifyAnimationScreenUnlock(callback);
    EXPECT_EQ(WMError::WM_OK, ret);

    RemoteAnimation::windowAnimationController_ = nullptr;
    ret = RemoteAnimation::NotifyAnimationScreenUnlock(callback);
    EXPECT_EQ(WMError::WM_ERROR_NO_REMOTE_ANIMATION, ret);
}

/*
 * @tc.name: NotifyAnimationUpdateWallpaper01
 * @tc.desc: NotifyAnimationUpdateWallpaper success
 * @tc.type: FUNC
 */
HWTEST_F(RemoteAnimationTest, NotifyAnimationUpdateWallpaper01, Function | SmallTest | Level2)
{
    RemoteAnimation::NotifyAnimationUpdateWallpaper(node_);
    RSIWindowAnimationControllerMocker* testController = reinterpret_cast<RSIWindowAnimationControllerMocker*>(
        animationController_.GetRefPtr());
    EXPECT_NE(nullptr, testController->animationTarget_);

    testController->animationTarget_ = nullptr;
    RemoteAnimation::windowAnimationController_ = nullptr;
    RemoteAnimation::NotifyAnimationUpdateWallpaper(node_);
    EXPECT_EQ(nullptr, testController->animationTarget_);
}

/*
 * @tc.name: CreateAnimationFinishedCallback01
 * @tc.desc: CreateAnimationFinishedCallback
 * @tc.type: FUNC
 */
HWTEST_F(RemoteAnimationTest, CreateAnimationFinishedCallback01, Function | SmallTest | Level2)
{
    std::function<void(void)> callback = nullptr;
    EXPECT_EQ(nullptr, RemoteAnimation::CreateAnimationFinishedCallback(callback));

    bool testFlag = false;
    callback = [&testFlag]() { testFlag = true; };
    auto finishCallback = RemoteAnimation::CreateAnimationFinishedCallback(callback);
    EXPECT_NE(nullptr, finishCallback);
    finishCallback->OnAnimationFinished();
    usleep(SLEEP_TIME_IN_US);
    EXPECT_EQ(true, testFlag);

    wmsTaskHandler_ = nullptr;
    RemoteAnimation::wmsTaskHandler_ = wmsTaskHandler_;
    callback = [&testFlag]() { testFlag = false; };
    finishCallback = RemoteAnimation::CreateAnimationFinishedCallback(callback);
    EXPECT_NE(nullptr, finishCallback);
    finishCallback->OnAnimationFinished();
    usleep(SLEEP_TIME_IN_US);
    EXPECT_EQ(false, testFlag);
}
}
}
}
