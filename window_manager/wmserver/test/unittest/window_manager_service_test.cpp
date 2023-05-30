/*
 * Copyright (c) 2022-2022 Huawei Device Co., Ltd.
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
#include "common_test_utils.h"
#include "iremote_object_mocker.h"
#include "mock_RSIWindowAnimationController.h"
#include "window_manager_service.h"

#include <thread>

#include <ability_manager_client.h>
#include <cinttypes>
#include <chrono>
#include <hisysevent.h>
#include <hitrace_meter.h>
#include <ipc_skeleton.h>
#include <parameters.h>
#include <rs_iwindow_animation_controller.h>
#include <system_ability_definition.h>
#include <sstream>
#include "xcollie/watchdog.h"

#include "color_parser.h"
#include "display_manager_service_inner.h"
#include "dm_common.h"
#include "drag_controller.h"
#include "minimize_app.h"
#include "permission.h"
#include "remote_animation.h"
#include "singleton_container.h"
#include "ui/rs_ui_director.h"
#include "window_helper.h"
#include "window_inner_manager.h"
#include "window_manager_agent_controller.h"
#include "window_manager_hilog.h"
#include "wm_common.h"
#include "wm_math.h"


using namespace testing;
using namespace testing::ext;
namespace OHOS {
namespace Rosen {
class WindowManagerServiceTest : public testing::Test {
public:
    static void SetUpTestCase();
    static void TearDownTestCase();
    void SetUp() override;
    void TearDown() override;

    void SetAceessTokenPermission(const std::string processName);
    sptr<WindowManagerService> wms = new WindowManagerService();
};

void WindowManagerServiceTest::SetUpTestCase()
{
}

void WindowManagerServiceTest::TearDownTestCase()
{
}

void WindowManagerServiceTest::SetUp()
{
    CommonTestUtils::SetAceessTokenPermission("WindowManagerServiceTest");
}

void WindowManagerServiceTest::TearDown()
{
}

namespace {
/**
 * @tc.name: OnAddSystemAbility
 * @tc.desc: OnAddSystemAbility test
 * @tc.type: FUNC
 */
HWTEST_F(WindowManagerServiceTest, OnAddSystemAbility01, Function | SmallTest | Level2)
{
    std::string str = "OnAddSystemAbility";
    wms->OnAddSystemAbility(0, str);
    ASSERT_EQ(nullptr, wms->windowCommonEvent_->subscriber_);
}
/**
 * @tc.name: WindowVisibilityChangeCallback
 * @tc.desc: WindowVisibilityChangeCallback test
 * @tc.type: FUNC
 */
HWTEST_F(WindowManagerServiceTest, WindowVisibilityChangeCallback01, Function | SmallTest | Level2)
{
    std::shared_ptr<RSOcclusionData> occlusionData = nullptr;
    wms->WindowVisibilityChangeCallback(occlusionData);
    ASSERT_EQ(nullptr, occlusionData);
}
/**
 * @tc.name: InitWithAbilityManagerServiceAdded
 * @tc.desc: Init with ability manager service added.
 * @tc.type: FUNC
 */
HWTEST_F(WindowManagerServiceTest, InitWithAbilityManagerServiceAdded01, Function | SmallTest | Level2)
{
    wms->wmsHandler_ = new WindowManagerServiceHandler;
    wms->InitWithAbilityManagerServiceAdded();
    ASSERT_NE(nullptr, wms->wmsHandler_);
}
/**
 * @tc.name: Dump
 * @tc.desc: Dump info
 * @tc.type: FUNC
 */
HWTEST_F(WindowManagerServiceTest, Dump01, Function | SmallTest | Level2)
{
    wms->windowDumper_ = nullptr;
    std::vector<std::u16string> args;
    ASSERT_EQ(static_cast<int>(WMError::WM_ERROR_INVALID_PARAM), wms->Dump(-1, args));
    ASSERT_EQ(static_cast<int>(WMError::WM_OK), wms->Dump(0, args));
}
/**
 * @tc.name: NotifyWindowTransition
 * @tc.desc: NotifyWindowTransition test
 * @tc.type: FUNC
 */
HWTEST_F(WindowManagerServiceTest, NotifyWindowTransition01, Function | SmallTest | Level2)
{
    sptr<WindowTransitionInfo> fromInfo = nullptr;
    sptr<WindowTransitionInfo> toInfo = nullptr;
    ASSERT_EQ(WMError::WM_OK, wms->NotifyWindowTransition(fromInfo, toInfo, false));
    ASSERT_EQ(WMError::WM_ERROR_NO_REMOTE_ANIMATION, wms->NotifyWindowTransition(fromInfo, toInfo, true));
}
/**
 * @tc.name: StartingWindow
 * @tc.desc: StartingWindow test
 * @tc.type: FUNC
 */
HWTEST_F(WindowManagerServiceTest, StartingWindow01, Function | SmallTest | Level2)
{
    wms->startingOpen_ = false;
    wms->StartingWindow(nullptr, nullptr, false, 0);
    ASSERT_EQ(false, wms->startingOpen_);
    wms->CancelStartingWindow(nullptr);
    wms->startingOpen_ = true;
    wms->StartingWindow(nullptr, nullptr, false, 0);
    ASSERT_EQ(true, wms->startingOpen_);
    wms->CancelStartingWindow(nullptr);
}
/**
 * @tc.name: CreateWindow
 * @tc.desc: CreateWindow test
 * @tc.type: FUNC
 */
HWTEST_F(WindowManagerServiceTest, CreateWindow01, Function | SmallTest | Level2)
{
    sptr<IWindow> window = nullptr;
    uint32_t id = 2;
    std::shared_ptr<RSSurfaceNode> RS_node = nullptr;
    sptr<WindowProperty> property = new WindowProperty();
    property->SetWindowType(WindowType::WINDOW_TYPE_WALLPAPER);
    ASSERT_EQ(WMError::WM_ERROR_NULLPTR, wms->CreateWindow(window, property, RS_node, id, nullptr));
    wms->DestroyWindow(id, true);
}
/**
 * @tc.name: AddWindow
 * @tc.desc: AddWindow test
 * @tc.type: FUNC
 */
HWTEST_F(WindowManagerServiceTest, AddWindow01, Function | SmallTest | Level2)
{
    sptr<WindowProperty> property = nullptr;
    ASSERT_EQ(WMError::WM_ERROR_NULLPTR, wms->AddWindow(property));
}
/**
 * @tc.name: RegisterWindowManagerAgent
 * @tc.desc: RegisterWindowManagerAgent test
 * @tc.type: FUNC
 */
HWTEST_F(WindowManagerServiceTest, RegisterWindowManagerAgent01, Function | SmallTest | Level2)
{
    sptr<IWindowManagerAgent> windowManagerAgent = nullptr;
    WindowManagerAgentType type = WindowManagerAgentType::WINDOW_MANAGER_AGENT_TYPE_CAMERA_FLOAT;
    ASSERT_EQ(false, wms->RegisterWindowManagerAgent(type, windowManagerAgent));
    ASSERT_EQ(false, wms->UnregisterWindowManagerAgent(type, windowManagerAgent));
}
/**
 * @tc.name: SetWindowAnimationController
 * @tc.desc: SetWindowAnimationController test
 * @tc.type: FUNC
 */
HWTEST_F(WindowManagerServiceTest, SetWindowAnimationController01, Function | SmallTest | Level2)
{
    sptr<RSIWindowAnimationController> controller = nullptr;
    ASSERT_EQ(WMError::WM_ERROR_NULLPTR, wms->SetWindowAnimationController(controller));
    controller = new RSIWindowAnimationControllerMocker;
    ASSERT_EQ(WMError::WM_OK, wms->SetWindowAnimationController(controller));
}
/**
 * @tc.name: OnWindowEvent
 * @tc.desc: OnWindowEvent test
 * @tc.type: FUNC
 */
HWTEST_F(WindowManagerServiceTest, OnWindowEvent01, Function | SmallTest | Level2)
{
    sptr<IRemoteObject> remoteObject = new IRemoteObjectMocker;
    wms->OnWindowEvent(static_cast<Event>(1), remoteObject);
    wms->OnWindowEvent(Event::REMOTE_DIED, remoteObject);
    ASSERT_EQ(INVALID_WINDOW_ID, wms->windowRoot_->GetWindowIdByObject(remoteObject));
}
/**
 * @tc.name: UpdateProperty
 * @tc.desc: UpdateProperty test
 * @tc.type: FUNC
 */
HWTEST_F(WindowManagerServiceTest, UpdateProperty01, Function | SmallTest | Level2)
{
    sptr<WindowProperty> windowProperty = nullptr;
    ASSERT_EQ(WMError::WM_ERROR_NULLPTR, wms->UpdateProperty(windowProperty,
        PropertyChangeAction::ACTION_UPDATE_ANIMATION_FLAG, true));
}
/**
 * @tc.name: GetModeChangeHotZones
 * @tc.desc: GetModeChangeHotZones test
 * @tc.type: FUNC
 */
HWTEST_F(WindowManagerServiceTest, GetModeChangeHotZones01, Function | SmallTest | Level2)
{
    ModeChangeHotZonesConfig config = {false, 0, 0, 0};
    DisplayId displayId = 0;
    ModeChangeHotZones hotZone;
    wms->hotZonesConfig_ = config;
    ASSERT_EQ(WMError::WM_DO_NOTHING, wms->GetModeChangeHotZones(displayId, hotZone));
    config.isModeChangeHotZoneConfigured_ = true;
    wms->hotZonesConfig_ = config;
    ASSERT_EQ(WMError::WM_ERROR_NULLPTR, wms->GetModeChangeHotZones(displayId, hotZone));
}
/**
 * @tc.name: UpdateAvoidAreaListener
 * @tc.desc: UpdateAvoidAreaListener test
 * @tc.type: FUNC
 */
HWTEST_F(WindowManagerServiceTest, UpdateAvoidAreaListener01, Function | SmallTest | Level2)
{
    sptr<WindowProperty> property = new WindowProperty();
    sptr<WindowNode> node = new WindowNode(property);
    wms->windowRoot_->windowNodeMap_.insert(std::make_pair(0, node));
    ASSERT_EQ(WMError::WM_DO_NOTHING, wms->UpdateAvoidAreaListener(0, true));
}
/**
 * @tc.name: BindDialogTarget
 * @tc.desc: BindDialogTarget test
 * @tc.type: FUNC
 */
HWTEST_F(WindowManagerServiceTest, BindDialogTarget01, Function | SmallTest | Level2)
{
    sptr<IRemoteObject> targetToken = new IRemoteObjectMocker();
    uint32_t id = 0;
    ASSERT_EQ(WMError::WM_ERROR_NULLPTR, wms->BindDialogTarget(id, targetToken));
}
}
}
}
