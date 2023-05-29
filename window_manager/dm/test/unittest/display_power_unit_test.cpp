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
#include "display_manager.h"
#include "screen_manager.h"
#include "mock_display_manager_adapter.h"
#include "singleton_mocker.h"

using namespace testing;
using namespace testing::ext;

namespace OHOS {
namespace Rosen {
using Mocker = SingletonMocker<DisplayManagerAdapter, MockDisplayManagerAdapter>;

class DisplayPowerEventListener : public IDisplayPowerEventListener {
public:
    virtual void OnDisplayPowerEvent(DisplayPowerEvent event, EventStatus status) {}
};

class DisplayPowerUnitTest : public testing::Test {
public:
    static void SetUpTestCase();
    static void TearDownTestCase();
    virtual void SetUp() override;
    virtual void TearDown() override;
    static inline sptr<DisplayPowerEventListener> listener_ = new DisplayPowerEventListener();
    static inline DisplayId defaultId_ = 0;
    static inline uint32_t brightnessLevel_ = 80;
    static inline uint32_t invalidBrightnessLevel_ = 1000000000;
    static inline ScreenPowerState initialPowerState_;
    static inline DisplayState initialState_;
};

void DisplayPowerUnitTest::SetUpTestCase()
{
}

void DisplayPowerUnitTest::TearDownTestCase()
{
}

void DisplayPowerUnitTest::SetUp()
{
    initialPowerState_ = ScreenManager::GetInstance().GetScreenPower(defaultId_);
    initialState_ = DisplayManager::GetInstance().GetDisplayState(defaultId_);
}

void DisplayPowerUnitTest::TearDown()
{
    ScreenManager::GetInstance().SetScreenPowerForAll(initialPowerState_, PowerStateChangeReason::POWER_BUTTON);
    DisplayStateCallback callback;
    DisplayManager::GetInstance().SetDisplayState(initialState_, callback);
}

namespace {
/**
 * @tc.name: register_display_power_event_listener_001
 * @tc.desc: call Register/UnregisterDisplayPowerEventListener with a valid listener and check return value
 * @tc.type: FUNC
 */
HWTEST_F(DisplayPowerUnitTest, register_display_power_event_listener_001, Function | SmallTest | Level2)
{
    Mocker m;

    EXPECT_CALL(m.Mock(), RegisterDisplayManagerAgent(_, DisplayManagerAgentType::DISPLAY_POWER_EVENT_LISTENER))
        .Times(1).WillOnce(Return(true));
    bool ret  = DisplayManager::GetInstance().RegisterDisplayPowerEventListener(listener_);
    ASSERT_EQ(true, ret);

    EXPECT_CALL(m.Mock(), UnregisterDisplayManagerAgent(_, DisplayManagerAgentType::DISPLAY_POWER_EVENT_LISTENER))
        .Times(1).WillOnce(Return(true));
    ret  = DisplayManager::GetInstance().UnregisterDisplayPowerEventListener(listener_);
    ASSERT_EQ(true, ret);
}

/**
 * @tc.name: register_display_power_event_listener_002
 * @tc.desc: call Register/UnregisterDisplayPowerEventListener with nullptr and check return value
 * @tc.type: FUNC
 */
HWTEST_F(DisplayPowerUnitTest, register_display_power_event_listener_002, Function | SmallTest | Level2)
{
    Mocker m;
    EXPECT_CALL(m.Mock(), RegisterDisplayManagerAgent(_, DisplayManagerAgentType::DISPLAY_POWER_EVENT_LISTENER))
        .Times(0);
    bool ret  = DisplayManager::GetInstance().RegisterDisplayPowerEventListener(nullptr);
    ASSERT_EQ(false, ret);

    EXPECT_CALL(m.Mock(), UnregisterDisplayManagerAgent(_, DisplayManagerAgentType::DISPLAY_POWER_EVENT_LISTENER))
        .Times(0);
    ret  = DisplayManager::GetInstance().UnregisterDisplayPowerEventListener(nullptr);
    ASSERT_EQ(false, ret);
}

/**
 * @tc.name: register_display_power_event_listener_003
 * @tc.desc: call Register/UnregisterDisplayPowerEventListener with ipc failed and check return value
 * @tc.type: FUNC
 */
HWTEST_F(DisplayPowerUnitTest, register_display_power_event_listener_003, Function | SmallTest | Level2)
{
    Mocker m;
    EXPECT_CALL(m.Mock(), RegisterDisplayManagerAgent(_, DisplayManagerAgentType::DISPLAY_POWER_EVENT_LISTENER))
        .Times(1).WillOnce(Return(false));
    bool ret  = DisplayManager::GetInstance().RegisterDisplayPowerEventListener(listener_);
    ASSERT_EQ(false, ret);

    EXPECT_CALL(m.Mock(), UnregisterDisplayManagerAgent(_, DisplayManagerAgentType::DISPLAY_POWER_EVENT_LISTENER))
        .Times(0);
    ret  = DisplayManager::GetInstance().UnregisterDisplayPowerEventListener(listener_);
    ASSERT_EQ(false, ret);
}

/**
 * @tc.name: unregister_display_power_event_listener_001
 * @tc.desc: call UnregisterDisplayPowerEventListener with a listener never registered and check return value
 * @tc.type: FUNC
 */
HWTEST_F(DisplayPowerUnitTest, unregister_display_power_event_listener_001, Function | SmallTest | Level2)
{
    Mocker m;
    EXPECT_CALL(m.Mock(), UnregisterDisplayManagerAgent(_, DisplayManagerAgentType::DISPLAY_POWER_EVENT_LISTENER))
        .Times(0);
    bool ret  = DisplayManager::GetInstance().UnregisterDisplayPowerEventListener(listener_);
    ASSERT_EQ(false, ret);
}

/**
 * @tc.name: unregister_display_power_event_listener_002
 * @tc.desc: call UnregisterDisplayPowerEventListener with nullptr and check return value
 * @tc.type: FUNC
 */
HWTEST_F(DisplayPowerUnitTest, unregister_display_power_event_listener_002, Function | SmallTest | Level2)
{
    Mocker m;
    EXPECT_CALL(m.Mock(), UnregisterDisplayManagerAgent(_, DisplayManagerAgentType::DISPLAY_POWER_EVENT_LISTENER))
        .Times(0);
    bool ret  = DisplayManager::GetInstance().UnregisterDisplayPowerEventListener(nullptr);
    ASSERT_EQ(false, ret);
}

/**
 * @tc.name: wake_up_begin_001
 * @tc.desc: call WakeUpBegin and check return value
 * @tc.type: FUNC
 */
HWTEST_F(DisplayPowerUnitTest, wake_up_begin_001, Function | SmallTest | Level2)
{
    Mocker m;
    EXPECT_CALL(m.Mock(), WakeUpBegin(PowerStateChangeReason::POWER_BUTTON)).Times(1).WillOnce(Return(true));;
    bool ret  = DisplayManager::GetInstance().WakeUpBegin(PowerStateChangeReason::POWER_BUTTON);
    ASSERT_EQ(true, ret);

    EXPECT_CALL(m.Mock(), WakeUpBegin(PowerStateChangeReason::POWER_BUTTON)).Times(1).WillOnce(Return(false));;
    ret  = DisplayManager::GetInstance().WakeUpBegin(PowerStateChangeReason::POWER_BUTTON);
    ASSERT_EQ(false, ret);
}

/**
 * @tc.name: wake_up_end_001
 * @tc.desc: call WakeUpEnd and check return value
 * @tc.type: FUNC
 */
HWTEST_F(DisplayPowerUnitTest, wake_up_end_001, Function | SmallTest | Level2)
{
    Mocker m;
    EXPECT_CALL(m.Mock(), WakeUpEnd()).Times(1).WillOnce(Return(true));
    bool ret  = DisplayManager::GetInstance().WakeUpEnd();
    ASSERT_EQ(true, ret);

    EXPECT_CALL(m.Mock(), WakeUpEnd()).Times(1).WillOnce(Return(false));
    ret  = DisplayManager::GetInstance().WakeUpEnd();
    ASSERT_EQ(false, ret);
}

/**
 * @tc.name: suspend_begin_001
 * @tc.desc: call SuspendBegin and check return value
 * @tc.type: FUNC
 */
HWTEST_F(DisplayPowerUnitTest, suspend_begin_001, Function | SmallTest | Level2)
{
    Mocker m;
    EXPECT_CALL(m.Mock(), SuspendBegin(PowerStateChangeReason::POWER_BUTTON)).Times(1).WillOnce(Return(true));;
    bool ret  = DisplayManager::GetInstance().SuspendBegin(PowerStateChangeReason::POWER_BUTTON);
    ASSERT_EQ(true, ret);

    EXPECT_CALL(m.Mock(), SuspendBegin(PowerStateChangeReason::POWER_BUTTON)).Times(1).WillOnce(Return(false));;
    ret  = DisplayManager::GetInstance().SuspendBegin(PowerStateChangeReason::POWER_BUTTON);
    ASSERT_EQ(false, ret);
}

/**
* @tc.name: suspend_end_001
* @tc.desc: call SuspendEnd and check return value
* @tc.type: FUNC
*/
HWTEST_F(DisplayPowerUnitTest, suspend_end_001, Function | SmallTest | Level2)
{
    Mocker m;
    EXPECT_CALL(m.Mock(), SuspendEnd()).Times(1).WillOnce(Return(true));
    bool ret  = DisplayManager::GetInstance().SuspendEnd();
    ASSERT_EQ(true, ret);

    EXPECT_CALL(m.Mock(), SuspendEnd()).Times(1).WillOnce(Return(false));
    ret  = DisplayManager::GetInstance().SuspendEnd();
    ASSERT_EQ(false, ret);
}

/**
* @tc.name: set_screen_brightness_001
* @tc.desc: Call SetScreenBrightness with a valid value and check the GetScreenBrightness return value
* @tc.type: FUNC
*/
HWTEST_F(DisplayPowerUnitTest, set_screen_brightness_001, Function | MediumTest | Level2)
{
    bool ret = DisplayManager::GetInstance().SetScreenBrightness(defaultId_, brightnessLevel_);
    ASSERT_EQ(true, ret);
}

/**
* @tc.name: set_screen_power_for_all_001
* @tc.desc: Call SetScreenPowerForAll with valid value and check the GetScreenPower return value
* @tc.type: FUNC
*/
HWTEST_F(DisplayPowerUnitTest, set_screen_power_for_all_001, Function | MediumTest | Level2)
{
    SingletonMocker<ScreenManagerAdapter, MockScreenManagerAdapter> m;
    EXPECT_CALL(m.Mock(), GetScreenPower(_)).Times(1).WillOnce(Return(ScreenPowerState::POWER_OFF));
    EXPECT_CALL(m.Mock(), SetScreenPowerForAll(_, PowerStateChangeReason::POWER_BUTTON))
        .Times(1).WillOnce(Return(true));

    bool ret = ScreenManager::GetInstance().SetScreenPowerForAll(ScreenPowerState::POWER_OFF,
        PowerStateChangeReason::POWER_BUTTON);
    ASSERT_EQ(true, ret);

    ScreenPowerState state = ScreenManager::GetInstance().GetScreenPower(defaultId_);
    ASSERT_EQ(state, ScreenPowerState::POWER_OFF);
}

/**
* @tc.name: set_display_state_001
* @tc.desc: Call SetDisplayState with valid value and check the GetDisplayState return value
* @tc.type: FUNC
*/
HWTEST_F(DisplayPowerUnitTest, set_display_state_001, Function | MediumTest | Level2)
{
    DisplayState stateToSet = (initialState_ == DisplayState::OFF ? DisplayState::ON : DisplayState::OFF);
    Mocker m;
    EXPECT_CALL(m.Mock(), RegisterDisplayManagerAgent(_, DisplayManagerAgentType::DISPLAY_STATE_LISTENER))
        .Times(1).WillOnce(Return(true));
    EXPECT_CALL(m.Mock(), SetDisplayState(stateToSet)).Times(1).WillOnce(Return(true));
    DisplayStateCallback callback = [](DisplayState state) {};
    bool ret = DisplayManager::GetInstance().SetDisplayState(stateToSet, callback);
    ASSERT_EQ(true, ret);

    EXPECT_CALL(m.Mock(), GetDisplayState(defaultId_)).Times(1).WillOnce(Return(stateToSet));
    DisplayState state = DisplayManager::GetInstance().GetDisplayState(defaultId_);
    ASSERT_EQ(state, stateToSet);
}

/**
* @tc.name: set_display_state_002
* @tc.desc: Call SetDisplayState with invalid callback and check the GetDisplayState return value
* @tc.type: FUNC
*/
HWTEST_F(DisplayPowerUnitTest, set_display_state_002, Function | MediumTest | Level2)
{
    Mocker m;
    EXPECT_CALL(m.Mock(), SetDisplayState(_)).Times(0);
    DisplayState stateToSet = (initialState_ == DisplayState::OFF ? DisplayState::ON : DisplayState::OFF);
    bool ret = DisplayManager::GetInstance().SetDisplayState(stateToSet, nullptr);
    ASSERT_EQ(false, ret);
}
}
}
}