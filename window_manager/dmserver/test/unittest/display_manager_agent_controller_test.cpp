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

#include "display_manager_agent_controller.h"

using namespace testing;
using namespace testing::ext;

namespace OHOS {
namespace Rosen {
class DisplayManagerAgentControllerTest : public testing::Test {
public:
    static void SetUpTestCase();
    static void TearDownTestCase();
    void SetUp() override;
    void TearDown() override;
};

void DisplayManagerAgentControllerTest::SetUpTestCase()
{
}

void DisplayManagerAgentControllerTest::TearDownTestCase()
{
}

void DisplayManagerAgentControllerTest::SetUp()
{
}

void DisplayManagerAgentControllerTest::TearDown()
{
}

namespace {
/**
 * @tc.name: OnScreenConnect
 * @tc.desc: OnScreenConnect test
 * @tc.type: FUNC
 */
HWTEST_F(DisplayManagerAgentControllerTest, OnScreenConnect, Function | SmallTest | Level3)
{
    sptr<ScreenInfo> screenInfo = nullptr;
    DisplayManagerAgentController::GetInstance().OnScreenConnect(screenInfo);
    ASSERT_EQ(0, DisplayManagerAgentController::GetInstance().
        dmAgentContainer_.GetAgentsByType(DisplayManagerAgentType::SCREEN_EVENT_LISTENER).size());
}
/**
 * @tc.name: OnScreenChange
 * @tc.desc: OnScreenChange test
 * @tc.type: FUNC
 */
HWTEST_F(DisplayManagerAgentControllerTest, OnScreenChange, Function | SmallTest | Level3)
{
    sptr<ScreenInfo> screenInfo = nullptr;
    DisplayManagerAgentController::GetInstance().OnScreenChange(screenInfo, ScreenChangeEvent::UPDATE_ROTATION);
    ASSERT_EQ(0, DisplayManagerAgentController::GetInstance().
        dmAgentContainer_.GetAgentsByType(DisplayManagerAgentType::SCREEN_EVENT_LISTENER).size());
}
/**
 * @tc.name: OnScreenGroupChange
 * @tc.desc: OnScreenChange test
 * @tc.type: FUNC
 */
HWTEST_F(DisplayManagerAgentControllerTest, OnScreenGroupChange, Function | SmallTest | Level3)
{
    sptr<ScreenInfo> screenInfo = nullptr;
    std::string trigger;
    DisplayManagerAgentController::GetInstance().OnScreenGroupChange(trigger,
        screenInfo, ScreenGroupChangeEvent::ADD_TO_GROUP);
    ASSERT_EQ(0, DisplayManagerAgentController::GetInstance().
        dmAgentContainer_.GetAgentsByType(DisplayManagerAgentType::SCREEN_EVENT_LISTENER).size());
    std::vector<sptr<ScreenInfo>> screenInfos;
    screenInfos.push_back(screenInfo);
    DisplayManagerAgentController::GetInstance().OnScreenGroupChange(trigger,
        screenInfos, ScreenGroupChangeEvent::ADD_TO_GROUP);
}
/**
 * @tc.name: OnDisplayCreate
 * @tc.desc: OnDisplayCreate test
 * @tc.type: FUNC
 */
HWTEST_F(DisplayManagerAgentControllerTest, OnDisplayCreate, Function | SmallTest | Level3)
{
    sptr<DisplayInfo> displayInfo;
    DisplayManagerAgentController::GetInstance().OnDisplayCreate(displayInfo);
    ASSERT_EQ(0, DisplayManagerAgentController::GetInstance().
        dmAgentContainer_.GetAgentsByType(DisplayManagerAgentType::SCREEN_EVENT_LISTENER).size());
}
/**
 * @tc.name: OnDisplayDestroy
 * @tc.desc: OnDisplayDestroy test
 * @tc.type: FUNC
 */
HWTEST_F(DisplayManagerAgentControllerTest, OnDisplayDestroy, Function | SmallTest | Level3)
{
    DisplayId displayId = 0;
    DisplayManagerAgentController::GetInstance().OnDisplayDestroy(displayId);
    ASSERT_EQ(0, DisplayManagerAgentController::GetInstance().
        dmAgentContainer_.GetAgentsByType(DisplayManagerAgentType::SCREEN_EVENT_LISTENER).size());
}
/**
 * @tc.name: OnDisplayChange
 * @tc.desc: OnDisplayChange test
 * @tc.type: FUNC
 */
HWTEST_F(DisplayManagerAgentControllerTest, OnDisplayChange, Function | SmallTest | Level3)
{
    sptr<DisplayInfo> displayInfo = nullptr;
    DisplayManagerAgentController::GetInstance().OnDisplayChange(displayInfo, DisplayChangeEvent::UNKNOWN);
    ASSERT_EQ(0, DisplayManagerAgentController::GetInstance().
        dmAgentContainer_.GetAgentsByType(DisplayManagerAgentType::SCREEN_EVENT_LISTENER).size());
    displayInfo = new DisplayInfo();
    DisplayManagerAgentController::GetInstance().OnDisplayChange(displayInfo, DisplayChangeEvent::UNKNOWN);
    ASSERT_EQ(0, DisplayManagerAgentController::GetInstance().
        dmAgentContainer_.GetAgentsByType(DisplayManagerAgentType::SCREEN_EVENT_LISTENER).size());
}
/**
 * @tc.name: OnScreenshot
 * @tc.desc: OnScreenshot test
 * @tc.type: FUNC
 */
HWTEST_F(DisplayManagerAgentControllerTest, OnScreenshot, Function | SmallTest | Level3)
{
    sptr<ScreenshotInfo> info = nullptr;
    DisplayManagerAgentController::GetInstance().OnScreenshot(info);
    ASSERT_EQ(0, DisplayManagerAgentController::GetInstance().
        dmAgentContainer_.GetAgentsByType(DisplayManagerAgentType::SCREEN_EVENT_LISTENER).size());
}
}
} // namespace Rosen
} // namespace OHOS
