/*
 * Copyright (c) 2021-2022 Huawei Device Co., Ltd.
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
#include "display_manager.h"
#include "screen_manager.h"
#include "window.h"
#include "window_manager_hilog.h"

using namespace testing;
using namespace testing::ext;

namespace OHOS {
namespace Rosen {
namespace {
    constexpr HiviewDFX::HiLogLabel LABEL = {LOG_CORE, HILOG_DOMAIN_DISPLAY, "DisplayPowerTest"};
    constexpr uint32_t MAX_TIME_WAITING_FOR_CALLBACK = 40;
    constexpr uint32_t SLEEP_TIME_IN_US = 50000;
}

class DisplayPowerEventListener : public IDisplayPowerEventListener {
public:
    virtual void OnDisplayPowerEvent(DisplayPowerEvent event, EventStatus status)
    {
        isCallbackCalled_ = true;
        event_ = event;
        status_ = status;
    }
    DisplayPowerEvent event_;
    EventStatus status_;
    bool isCallbackCalled_ { false };
};

class DisplayPowerTest : public testing::Test {
public:
    static void SetUpTestCase();
    static void TearDownTestCase();
    virtual void SetUp() override;
    virtual void TearDown() override;

    static void CheckDisplayStateCallback(bool valueExpected);
    static void CheckDisplayPowerEventCallback(bool valueExpected);

    static inline DisplayId defaultId_;
    static inline uint32_t brightnessLevel_ = 80;
    static inline uint32_t invalidBrightnessLevel_ = 1000000000;
    static inline uint32_t times_ = 0;
    static inline bool isDisplayStateCallbackCalled_ = false;
    static sptr<DisplayPowerEventListener> listener_;

    DisplayState state_ { DisplayState::ON };
    DisplayStateCallback callback_ = [this](DisplayState state) {
        isDisplayStateCallbackCalled_ = true;
        state_ = state;
    };
};

sptr<DisplayPowerEventListener> DisplayPowerTest::listener_ = new DisplayPowerEventListener();

void DisplayPowerTest::SetUpTestCase()
{
    CommonTestUtils::SetAceessTokenPermission("SetDisplayState");
    defaultId_ = DisplayManager::GetInstance().GetDefaultDisplayId();
    if (defaultId_ == DISPLAY_ID_INVALID) {
        WLOGFE("GetDefaultDisplayId failed!");
    }
    DisplayManager::GetInstance().RegisterDisplayPowerEventListener(listener_);
}

void DisplayPowerTest::TearDownTestCase()
{
    DisplayManager::GetInstance().UnregisterDisplayPowerEventListener(listener_);
}

void DisplayPowerTest::SetUp()
{
    times_ = 0;

    isDisplayStateCallbackCalled_ = false;
    state_ = DisplayState::UNKNOWN;

    listener_->isCallbackCalled_ = false;
    listener_->event_ = static_cast<DisplayPowerEvent>(-1);
    listener_->status_ = static_cast<EventStatus>(-1);
}

void DisplayPowerTest::TearDown()
{
}

void DisplayPowerTest::CheckDisplayStateCallback(bool valueExpected)
{
    do {
        if (isDisplayStateCallbackCalled_ == valueExpected) {
            return;
        }
        usleep(SLEEP_TIME_IN_US);
        ++times_;
    } while (times_ <= MAX_TIME_WAITING_FOR_CALLBACK);
}

void DisplayPowerTest::CheckDisplayPowerEventCallback(bool valueExpected)
{
    do {
        if (listener_->isCallbackCalled_ == valueExpected) {
            return;
        }
        usleep(SLEEP_TIME_IN_US);
        ++times_;
    } while (times_ <= MAX_TIME_WAITING_FOR_CALLBACK);
}

namespace {
/**
 * @tc.name: register_display_power_event_listener_001
 * @tc.desc: call RegisterDisplayPowerEventListener with a valid listener and check return value
 * @tc.type: FUNC
 */
HWTEST_F(DisplayPowerTest, register_display_power_event_listener_001, Function | SmallTest | Level2)
{
    sptr<IDisplayPowerEventListener> listener = new DisplayPowerEventListener();
    bool ret = DisplayManager::GetInstance().RegisterDisplayPowerEventListener(listener);
    ASSERT_EQ(true, ret);
    DisplayManager::GetInstance().UnregisterDisplayPowerEventListener(listener);
}

/**
 * @tc.name: register_display_power_event_listener_002
 * @tc.desc: call RegisterDisplayPowerEventListener with an invalid listener and check return value
 * @tc.type: FUNC
 */
HWTEST_F(DisplayPowerTest, register_display_power_event_listener_002, Function | SmallTest | Level2)
{
    bool ret = DisplayManager::GetInstance().RegisterDisplayPowerEventListener(nullptr);
    ASSERT_EQ(false, ret);
}

/**
 * @tc.name: unregister_display_power_event_listener_001
 * @tc.desc: call UnregisterDisplayPowerEventListener with a valid listener and check return value
 * @tc.type: FUNC
 */
HWTEST_F(DisplayPowerTest, unregister_display_power_event_listener_001, Function | SmallTest | Level2)
{
    sptr<IDisplayPowerEventListener> listener = new DisplayPowerEventListener();
    DisplayManager::GetInstance().RegisterDisplayPowerEventListener(listener);
    bool ret = DisplayManager::GetInstance().UnregisterDisplayPowerEventListener(listener);
    ASSERT_EQ(true, ret);
}

/**
* @tc.name: unregister_display_power_event_listener_002
* @tc.desc: call UnregisterDisplayPowerEventListener with nullptr and check return value
* @tc.type: FUNC
*/
HWTEST_F(DisplayPowerTest, unregister_display_power_event_listener_002, Function | SmallTest | Level2)
{
    bool ret = DisplayManager::GetInstance().UnregisterDisplayPowerEventListener(nullptr);
    ASSERT_EQ(false, ret);
}

/**
* @tc.name: unregister_display_power_event_listener_003
* @tc.desc: call UnregisterDisplayPowerEventListener with an invalid listener and check return value
* @tc.type: FUNC
*/
HWTEST_F(DisplayPowerTest, unregister_display_power_event_listener_003, Function | SmallTest | Level2)
{
    sptr<IDisplayPowerEventListener> listener = new DisplayPowerEventListener();
    bool ret = DisplayManager::GetInstance().UnregisterDisplayPowerEventListener(listener);
    ASSERT_EQ(false, ret);
}

/**
 * @tc.name: set_display_state_001
 * @tc.desc: Call SetDisplayState and check if it the state set is the same as calling GetDisplayState
 * @tc.type: FUNC
 */
HWTEST_F(DisplayPowerTest, set_display_state_001, Function | MediumTest | Level2)
{
    DisplayState initialState = DisplayManager::GetInstance().GetDisplayState(defaultId_);
    DisplayState stateToSet = (initialState == DisplayState::OFF ? DisplayState::ON : DisplayState::OFF);
    bool ret = DisplayManager::GetInstance().SetDisplayState(stateToSet, callback_);
    ASSERT_EQ(true, ret);
    DisplayState stateGet = DisplayManager::GetInstance().GetDisplayState(defaultId_);
    ASSERT_EQ(stateGet, stateToSet);
    CheckDisplayStateCallback(true);
}

/**
 * @tc.name: set_display_state_002
 * @tc.desc: Call SetDisplayState to set a value already set and check the return value
 * @tc.type: FUNC
 */
HWTEST_F(DisplayPowerTest, set_display_state_002, Function | MediumTest | Level2)
{
    DisplayState initialState = DisplayManager::GetInstance().GetDisplayState(defaultId_);
    bool ret = DisplayManager::GetInstance().SetDisplayState(initialState, callback_);
    ASSERT_EQ(false, ret);
    DisplayState stateGet = DisplayManager::GetInstance().GetDisplayState(defaultId_);
    ASSERT_EQ(stateGet, initialState);
    CheckDisplayStateCallback(false);
    ASSERT_EQ(false, isDisplayStateCallbackCalled_);
}

/**
 * @tc.name: set_display_state_003
 * @tc.desc: Call SetDisplayState with an invalid value and check the return value
 * @tc.type: FUNC
 */
HWTEST_F(DisplayPowerTest, set_display_state_003, Function | MediumTest | Level2)
{
    bool ret = DisplayManager::GetInstance().SetDisplayState(DisplayState::UNKNOWN, callback_);
    ASSERT_EQ(false, ret);
    CheckDisplayStateCallback(false);
    ASSERT_EQ(false, isDisplayStateCallbackCalled_);
    CheckDisplayPowerEventCallback(false);
    ASSERT_EQ(false, listener_->isCallbackCalled_);
}

/**
 * @tc.name: set_display_state_callback_001
 * @tc.desc: Call SetDisplayState and check if callback state is correct
 * @tc.type: FUNC
 */
HWTEST_F(DisplayPowerTest, set_display_state_callback_001, Function | MediumTest | Level2)
{
    DisplayState initialState = DisplayManager::GetInstance().GetDisplayState(defaultId_);
    DisplayState stateToSet = (initialState == DisplayState::OFF ? DisplayState::ON : DisplayState::OFF);
    DisplayManager::GetInstance().SetDisplayState(stateToSet, callback_);
    CheckDisplayStateCallback(true);
    ASSERT_EQ(true, isDisplayStateCallbackCalled_);
    ASSERT_EQ(state_, stateToSet);
}

/**
 * @tc.name: set_display_state_callback_002
 * @tc.desc: Call SetDisplayState to set a value already set and check the DisplayStateCallback
 * @tc.type: FUNC
 */
HWTEST_F(DisplayPowerTest, set_display_state_callback_002, Function | MediumTest | Level2)
{
    DisplayState initialState = DisplayManager::GetInstance().GetDisplayState(defaultId_);
    DisplayManager::GetInstance().SetDisplayState(initialState, callback_);
    CheckDisplayStateCallback(false);
    ASSERT_EQ(false, isDisplayStateCallbackCalled_);
}

/**
 * @tc.name: wake_up_begin_callback_001
 * @tc.desc: Call WakeUpBegin and check the OnDisplayPowerEvent callback is called
 * @tc.type: FUNC
 */
HWTEST_F(DisplayPowerTest, wake_up_begin_callback_001, Function | MediumTest | Level2)
{
    bool ret = DisplayManager::GetInstance().WakeUpBegin(PowerStateChangeReason::POWER_BUTTON);
    ASSERT_EQ(true, ret);
    CheckDisplayPowerEventCallback(true);
    ASSERT_EQ(true, listener_->isCallbackCalled_);
    ASSERT_EQ(DisplayPowerEvent::WAKE_UP, listener_->event_);
    ASSERT_EQ(EventStatus::BEGIN, listener_->status_);
}

/**
 * @tc.name: wake_up_end_callback_001
 * @tc.desc: Call WakeUpEnd and check the OnDisplayPowerEvent callback is called
 * @tc.type: FUNC
 */
HWTEST_F(DisplayPowerTest, wake_up_end_callback_001, Function | MediumTest | Level2)
{
    bool ret = DisplayManager::GetInstance().WakeUpEnd();
    ASSERT_EQ(true, ret);
    CheckDisplayPowerEventCallback(true);
    ASSERT_EQ(true, listener_->isCallbackCalled_);
    ASSERT_EQ(DisplayPowerEvent::WAKE_UP, listener_->event_);
    ASSERT_EQ(EventStatus::END, listener_->status_);
}

/**
 * @tc.name: suspend_begin_callback_001
 * @tc.desc: Call SuspendBegin and check the OnDisplayPowerEvent callback is called
 * @tc.type: FUNC
 */
HWTEST_F(DisplayPowerTest, suspend_begin_callback_001, Function | MediumTest | Level2)
{
    bool ret = DisplayManager::GetInstance().SuspendBegin(PowerStateChangeReason::POWER_BUTTON);
    ASSERT_EQ(true, ret);
    CheckDisplayPowerEventCallback(true);
    ASSERT_EQ(true, listener_->isCallbackCalled_);
    ASSERT_EQ(DisplayPowerEvent::SLEEP, listener_->event_);
    ASSERT_EQ(EventStatus::BEGIN, listener_->status_);
}

/**
* @tc.name: suspend_end_callback_001
* @tc.desc: Call SuspendEnd and check the OnDisplayPowerEvent callback is called
* @tc.type: FUNC
*/
HWTEST_F(DisplayPowerTest, suspend_end_callback_001, Function | MediumTest | Level2)
{
    bool ret = DisplayManager::GetInstance().SuspendEnd();
    ASSERT_EQ(true, ret);
    CheckDisplayPowerEventCallback(true);
    ASSERT_EQ(true, listener_->isCallbackCalled_);
    ASSERT_EQ(DisplayPowerEvent::SLEEP, listener_->event_);
    ASSERT_EQ(EventStatus::END, listener_->status_);
}

/**
* @tc.name: set_screen_power_for_all_001
* @tc.desc: Call SetScreenPowerForAll OFF and check the OnDisplayPowerEvent callback is called
* @tc.type: FUNC
*/
HWTEST_F(DisplayPowerTest, set_screen_power_for_all_001, Function | MediumTest | Level2)
{
    bool ret = ScreenManager::GetInstance().SetScreenPowerForAll(ScreenPowerState::POWER_OFF,
        PowerStateChangeReason::POWER_BUTTON);
    ASSERT_EQ(true, ret);
    CheckDisplayPowerEventCallback(true);
    ASSERT_EQ(true, listener_->isCallbackCalled_);
    ASSERT_EQ(DisplayPowerEvent::DISPLAY_OFF, listener_->event_);
    ASSERT_EQ(EventStatus::END, listener_->status_);
}

/**
* @tc.name: set_screen_power_for_all_002
* @tc.desc: Call SetScreenPowerForAll ON and check the OnDisplayPowerEvent callback is called
* @tc.type: FUNC
*/
HWTEST_F(DisplayPowerTest, set_screen_power_for_all_002, Function | MediumTest | Level2)
{
    bool ret = ScreenManager::GetInstance().SetScreenPowerForAll(ScreenPowerState::POWER_ON,
        PowerStateChangeReason::POWER_BUTTON);
    ASSERT_EQ(true, ret);
    CheckDisplayPowerEventCallback(true);
    ASSERT_EQ(true, listener_->isCallbackCalled_);
    ASSERT_EQ(DisplayPowerEvent::DISPLAY_ON, listener_->event_);
    ASSERT_EQ(EventStatus::END, listener_->status_);
}

/**
* @tc.name: set_screen_power_for_all_003
* @tc.desc: Call SetScreenPowerForAll with an invalid value and check the return value
* @tc.type: FUNC
*/
HWTEST_F(DisplayPowerTest, set_screen_power_for_all_003, Function | MediumTest | Level2)
{
    bool ret = ScreenManager::GetInstance().SetScreenPowerForAll(ScreenPowerState::INVALID_STATE,
        PowerStateChangeReason::POWER_BUTTON);
    ASSERT_EQ(false, ret);
    CheckDisplayPowerEventCallback(true);
    ASSERT_EQ(false, listener_->isCallbackCalled_);
}

/**
* @tc.name: set_display_state_power_event_callback_001
* @tc.desc: Call SetDisplayState with a valid value and check the OnDisplayPowerEvent callback is called
* @tc.type: FUNC
*/
HWTEST_F(DisplayPowerTest, set_display_state_power_event_callback_001, Function | MediumTest | Level2)
{
    DisplayState initialState = DisplayManager::GetInstance().GetDisplayState(defaultId_);
    DisplayState stateToSet = (initialState == DisplayState::OFF ? DisplayState::ON : DisplayState::OFF);
    bool ret = DisplayManager::GetInstance().SetDisplayState(stateToSet, callback_);
    ASSERT_EQ(true, ret);
    CheckDisplayPowerEventCallback(true);
    ASSERT_EQ(true, listener_->isCallbackCalled_);
    DisplayPowerEvent expectedEvent = (stateToSet == DisplayState::OFF ? DisplayPowerEvent::DISPLAY_OFF :
        DisplayPowerEvent::DISPLAY_ON);
    ASSERT_EQ(expectedEvent, listener_->event_);
    ASSERT_EQ(EventStatus::BEGIN, listener_->status_);
}

/**
* @tc.name: get_display_power_002
* @tc.desc: Call SetScreenPowerForAll ON and check the GetScreenPower return value
* @tc.type: FUNC
*/
HWTEST_F(DisplayPowerTest, get_display_power_002, Function | MediumTest | Level2)
{
    ScreenPowerState stateToSet = ScreenPowerState::POWER_ON;
    bool ret = ScreenManager::GetInstance().SetScreenPowerForAll(stateToSet, PowerStateChangeReason::POWER_BUTTON);
    ASSERT_EQ(true, ret);
    ScreenPowerState stateGet = ScreenManager::GetInstance().GetScreenPower(defaultId_);
    ASSERT_EQ(stateGet, stateToSet);
}

/**
* @tc.name: set_screen_brightness_001
* @tc.desc: Call SetScreenBrightness with a valid value and check the GetScreenBrightness return value
* @tc.type: FUNC
*/
HWTEST_F(DisplayPowerTest, set_screen_brightness_001, Function | MediumTest | Level2)
{
    bool ret = DisplayManager::GetInstance().SetScreenBrightness(defaultId_, brightnessLevel_);
    ASSERT_EQ(true, ret);
    uint32_t level = DisplayManager::GetInstance().GetScreenBrightness(defaultId_);
    ASSERT_EQ(level, brightnessLevel_);
}

/**
* @tc.name: window_life_cycle_001
* @tc.desc: Add a window and then call SuspendEnd and check window state; Notify unlock and check window state
* @tc.type: FUNC
*/
HWTEST_F(DisplayPowerTest, window_life_cycle_001, Function | MediumTest | Level2)
{
    sptr<WindowOption> option = new WindowOption();
    sptr<Window> window = Window::Create("window1", option, nullptr);
    EXPECT_EQ(WMError::WM_OK, window->Show());

    DisplayManager::GetInstance().SuspendBegin(PowerStateChangeReason::POWER_BUTTON);
    usleep(SLEEP_TIME_IN_US);
    ASSERT_EQ(false, window->GetWindowState() == WindowState::STATE_SHOWN);

    DisplayManager::GetInstance().NotifyDisplayEvent(DisplayEvent::UNLOCK);
    usleep(SLEEP_TIME_IN_US);
    ASSERT_EQ(true, window->GetWindowState() == WindowState::STATE_SHOWN);

    window->Destroy();
}
} // namespace
} // namespace Rosen
} // namespace OHOS