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
#include <parameters.h>

#include "common_test_utils.h"
#include "display_test_utils.h"
#include "future.h"
#include "screen.h"
#include "surface_draw.h"
#include "virtual_screen_group_change_listener_future.h"
#include "window.h"
#include "window_option.h"
#include "window_manager_hilog.h"

using namespace testing;
using namespace testing::ext;

namespace OHOS {
namespace Rosen {
namespace {
    constexpr HiviewDFX::HiLogLabel LABEL = {LOG_CORE, HILOG_DOMAIN_DISPLAY, "ScreenManagerTest"};
    constexpr uint32_t COLOR_RED = 0xffff0000;
}
class ScreenGroupChangeListener;
class ScreenManagerTest : public testing::Test {
public:
    static void SetUpTestCase();
    static void TearDownTestCase();
    virtual void SetUp() override;
    virtual void TearDown() override;
    sptr<Window> CreateWindowByDisplayId(DisplayId displayId);
    bool DrawWindowColor(const sptr<Window>& window, uint32_t color);
    void CheckScreenStateInGroup(bool, sptr<ScreenGroup>, ScreenId, sptr<Screen>, ScreenId);
    void CheckScreenGroupState(ScreenCombination, ScreenGroupChangeEvent event, ScreenId,
        sptr<ScreenGroup>, sptr<ScreenGroupChangeListener>);
    void CheckScreenGroupStateForMirror(ScreenGroupChangeEvent event, std::vector<ScreenId> mirrorIds,
        sptr<VirtualScreenGroupChangeListenerFuture> virtualScreenGroupChangeListener);
    static sptr<Display> defaultDisplay_;
    static DisplayId defaultDisplayId_;
    static ScreenId defaultScreenId_;
    static std::string defaultName_;
    static uint32_t defaultWidth_;
    static uint32_t defaultHeight_;
    static float defaultDensity_;
    static int32_t defaultFlags_;
    static VirtualScreenOption defaultOption_;
    static uint32_t waitCount_;
    const uint32_t sleepUs_ = 10 * 1000;
    const uint32_t maxWaitCount_ = 2000;
    const uint32_t execTimes_ = 10;
    const uint32_t acquireFrames_ = 1;
    static constexpr uint32_t TEST_SLEEP_S = 1; // test sleep time
    static constexpr uint32_t TEST_SLEEP_S_LONG = 10; // test sleep for 10 seconds
    static constexpr long TIME_OUT = 1000;
};

class ScreenChangeListener : public ScreenManager::IScreenListener {
public:
    virtual void OnConnect(ScreenId screenId) override
    {
        WLOGFI("OnConnect, screenId:%{public}" PRIu64"", screenId);
        connectFuture_.SetValue(screenId);
    }
    virtual void OnDisconnect(ScreenId screenId) override
    {
        WLOGFI("OnDisconnect, screenId:%{public}" PRIu64"", screenId);
        disconnectFuture_.SetValue(screenId);
    }
    virtual void OnChange(ScreenId screenId) override
    {
        WLOGFI("OnChange, screenId:%{public}" PRIu64"", screenId);
        changeFuture_.SetValue(screenId);
    }
    RunnableFuture<ScreenId> connectFuture_;
    RunnableFuture<ScreenId> disconnectFuture_;
    RunnableFuture<ScreenId> changeFuture_;
};

class ScreenGroupChangeListener : public ScreenManager::IScreenGroupListener {
public:
    virtual void OnChange(const std::vector<ScreenId>& screenIds, ScreenGroupChangeEvent event) override
    {
        for (auto screenId : screenIds) {
            changeFuture_.SetValue(std::make_pair(screenId, event));
            usleep(10 * 1000); // wait 10000 us
        }
    }
    RunnableFuture<std::pair<ScreenId, ScreenGroupChangeEvent>> changeFuture_;
};

sptr<Display> ScreenManagerTest::defaultDisplay_ = nullptr;
DisplayId ScreenManagerTest::defaultDisplayId_ = DISPLAY_ID_INVALID;
ScreenId ScreenManagerTest::defaultScreenId_ = INVALID_SCREEN_ID;
std::string ScreenManagerTest::defaultName_ = "virtualScreen01";
uint32_t ScreenManagerTest::defaultWidth_ = 480;
uint32_t ScreenManagerTest::defaultHeight_ = 320;
float ScreenManagerTest::defaultDensity_ = 2.0;
int32_t ScreenManagerTest::defaultFlags_ = 0;
VirtualScreenOption ScreenManagerTest::defaultOption_ = {
    defaultName_, defaultWidth_, defaultHeight_, defaultDensity_, nullptr, defaultFlags_
};
uint32_t ScreenManagerTest::waitCount_ = 0;

void ScreenManagerTest::SetUpTestCase()
{
    defaultDisplay_ = DisplayManager::GetInstance().GetDefaultDisplay();
    defaultDisplayId_ = defaultDisplay_->GetId();
    defaultScreenId_ = defaultDisplay_->GetScreenId();
    defaultWidth_ = defaultDisplay_->GetWidth();
    defaultHeight_ = defaultDisplay_->GetHeight();
    defaultOption_.width_ = defaultWidth_;
    defaultOption_.height_ = defaultHeight_;

    CommonTestUtils::InjectTokenInfoByHapName(0, "com.ohos.systemui", 0);
}

void ScreenManagerTest::TearDownTestCase()
{
}

void ScreenManagerTest::SetUp()
{
}

void ScreenManagerTest::TearDown()
{
}


bool ScreenManagerTest::DrawWindowColor(const sptr<Window>& window, uint32_t color)
{
    auto surfaceNode = window->GetSurfaceNode();
    if (surfaceNode == nullptr) {
        WLOGFE("Failed to GetSurfaceNode!");
        return false;
    }
    Rect rect = window->GetRequestRect();
    uint32_t windowWidth = rect.width_;
    uint32_t windowHeight = rect.height_;
    WLOGFI("windowWidth: %{public}u, windowHeight: %{public}u", windowWidth, windowHeight);
    SurfaceDraw::DrawColor(surfaceNode, windowWidth, windowHeight, color);
    return true;
}

sptr<Window> ScreenManagerTest::CreateWindowByDisplayId(DisplayId displayId)
{
    sptr<WindowOption> option = new WindowOption();
    if (option == nullptr) {
        return nullptr;
    }
    Rect displayRect = {0, 0, 640, 480};
    option->SetDisplayId(displayId);
    option->SetWindowRect(displayRect);
    option->SetWindowType(WindowType::WINDOW_TYPE_APP_MAIN_WINDOW);
    option->SetWindowMode(WindowMode::WINDOW_MODE_FLOATING);
    option->SetWindowName("VirtualWindow01");
    sptr<Window> window = Window::Create(option->GetWindowName(), option);
    return window;
}

#define CHECK_TEST_INIT_SCREEN_STATE \
    std::vector<sptr<Screen>> allScreens = ScreenManager::GetInstance().GetAllScreens(); \
    ASSERT_LT(0, allScreens.size()); \
    ScreenId groupId = SCREEN_ID_INVALID; \
    for (auto screen : allScreens) { \
        if (screen->IsGroup()) { \
        groupId = screen->GetId(); \
        ASSERT_EQ(SCREEN_ID_INVALID, screen->GetParentId()); \
        } \
    } \
    ASSERT_NE(SCREEN_ID_INVALID, groupId); \
    auto group = ScreenManager::GetInstance().GetScreenGroup(groupId); \
    ASSERT_NE(nullptr, group); \
    ASSERT_EQ(groupId, group->GetId()); \
    sptr<ScreenChangeListener> screenListener = new ScreenChangeListener(); \
    sptr<ScreenGroupChangeListener> screenGroupChangeListener = new ScreenGroupChangeListener(); \
    sptr<VirtualScreenGroupChangeListenerFuture> virtualScreenGroupChangeListener \
        = new VirtualScreenGroupChangeListenerFuture(); \
    ScreenManager::GetInstance().RegisterScreenListener(screenListener); \
    ScreenManager::GetInstance().RegisterScreenGroupListener(screenGroupChangeListener); \
    ScreenManager::GetInstance().RegisterVirtualScreenGroupListener(virtualScreenGroupChangeListener); \

#define CHECK_SCREEN_STATE_AFTER_CREATE_VIRTUAL_SCREEN \
    auto virtualScreen = ScreenManager::GetInstance().GetScreenById(virtualScreenId); \
    ASSERT_NE(nullptr, virtualScreen); \
    ASSERT_EQ(virtualScreenId, virtualScreen->GetId()); \
    ScreenId screenId = screenListener->connectFuture_.GetResult(TIME_OUT); \
    screenListener->connectFuture_.Reset(SCREEN_ID_INVALID); \
    ASSERT_EQ(virtualScreenId, screenId); \

void ScreenManagerTest::CheckScreenStateInGroup(
    bool isInGroup, sptr<ScreenGroup> group, ScreenId groupId, sptr<Screen> virtualScreen, ScreenId virtualScreenId)
{
    auto childIds = group->GetChildIds();
    ASSERT_LT(0, childIds.size());
    auto iter = std::find(childIds.begin(), childIds.end(), virtualScreenId);
    if (isInGroup) {
        ASSERT_EQ(groupId, virtualScreen->GetParentId());
    } else {
        ASSERT_EQ(SCREEN_ID_INVALID, virtualScreen->GetParentId());
    }
    if (isInGroup) {
        ASSERT_NE(childIds.end(), iter);
    } else {
        ASSERT_EQ(childIds.end(), iter);
    }
}

void ScreenManagerTest::CheckScreenGroupState(ScreenCombination combination, ScreenGroupChangeEvent event,
    ScreenId virtualScreenId, sptr<ScreenGroup> group, sptr<ScreenGroupChangeListener> screenGroupChangeListener)
{
    auto pair = screenGroupChangeListener->changeFuture_.GetResult(TIME_OUT);
    screenGroupChangeListener->changeFuture_.Reset(
        std::make_pair(SCREEN_ID_INVALID, ScreenGroupChangeEvent::REMOVE_FROM_GROUP));
    ASSERT_EQ(virtualScreenId, pair.first);
    ASSERT_EQ(event, pair.second);
    ASSERT_EQ(combination, group->GetCombination());
}

void ScreenManagerTest::CheckScreenGroupStateForMirror(ScreenGroupChangeEvent event, std::vector<ScreenId> mirrorIds,
    sptr<VirtualScreenGroupChangeListenerFuture> virtualScreenGroupChangeListener)
{
    auto info = virtualScreenGroupChangeListener->mirrorChangeFuture_.GetResult(TIME_OUT);
    virtualScreenGroupChangeListener->mirrorChangeFuture_.Reset({ScreenGroupChangeEvent::CHANGE_GROUP, "", {}});
    ASSERT_EQ(info.event, event);
    ASSERT_EQ(info.ids, mirrorIds);
    ASSERT_GE(info.trigger.size(), 0UL);
}

#define CHECK_SCREEN_STATE_AFTER_DESTROY_VIRTUAL_SCREEN \
    { \
        auto screenId = screenListener->disconnectFuture_.GetResult(TIME_OUT); \
        screenListener->disconnectFuture_.Reset(SCREEN_ID_INVALID); \
        ASSERT_EQ(virtualScreenId, screenId); \
    }

namespace {
/**
 * @tc.name: ScreenManager01
 * @tc.desc: Create a virtual screen and destroy it
 * @tc.type: FUNC
 */
HWTEST_F(ScreenManagerTest, ScreenManager01, Function | MediumTest | Level2)
{
    DisplayTestUtils utils;
    ASSERT_TRUE(utils.CreateSurface());
    defaultOption_.surface_ = utils.psurface_;
    defaultOption_.isForShot_ = false;
    ScreenId virtualScreenId = ScreenManager::GetInstance().CreateVirtualScreen(defaultOption_);
    ASSERT_NE(SCREEN_ID_INVALID, virtualScreenId);
    ASSERT_EQ(DMError::DM_OK, ScreenManager::GetInstance().DestroyVirtualScreen(virtualScreenId));
}

/**
 * @tc.name: ScreenManager02
 * @tc.desc: Create a virtual screen as mirror of default screen, and destroy virtual screen
 * @tc.type: FUNC
 */
HWTEST_F(ScreenManagerTest, ScreenManager02, Function | MediumTest | Level2)
{
    DisplayTestUtils utils;
    ASSERT_TRUE(utils.CreateSurface());
    defaultOption_.surface_ = utils.psurface_;
    defaultOption_.isForShot_ = false;
    ScreenId virtualScreenId = ScreenManager::GetInstance().CreateVirtualScreen(defaultOption_);
    std::vector<ScreenId> mirrorIds;
    mirrorIds.push_back(virtualScreenId);
    ScreenManager::GetInstance().MakeMirror(defaultScreenId_, mirrorIds);
    ASSERT_NE(SCREEN_ID_INVALID, virtualScreenId);
    ASSERT_EQ(DMError::DM_OK, ScreenManager::GetInstance().DestroyVirtualScreen(virtualScreenId));
}

/**
 * @tc.name: ScreenManager03
 * @tc.desc: Create a virtual screen and destroy it for 10 times
 * @tc.type: FUNC
 */
HWTEST_F(ScreenManagerTest, ScreenManager03, Function | MediumTest | Level2)
{
    DisplayTestUtils utils;
    defaultOption_.isForShot_ = false;
    for (uint32_t i = 0; i < execTimes_; i++) {
        ASSERT_TRUE(utils.CreateSurface());
        defaultOption_.surface_ = utils.psurface_;
        ScreenId virtualScreenId = ScreenManager::GetInstance().CreateVirtualScreen(defaultOption_);
        ASSERT_NE(SCREEN_ID_INVALID, virtualScreenId);
        ASSERT_EQ(DMError::DM_OK, ScreenManager::GetInstance().DestroyVirtualScreen(virtualScreenId));
        usleep(sleepUs_);
    }
}

/**
 * @tc.name: ScreenManager04
 * @tc.desc: Create a virtual screen as mirror of default screen, and destroy virtual screen for 10 times
 * @tc.type: FUNC
 */
HWTEST_F(ScreenManagerTest, ScreenManager04, Function | MediumTest | Level2)
{
    DisplayTestUtils utils;
    defaultOption_.isForShot_ = false;
    for (uint32_t i = 0; i < execTimes_; i++) {
        ASSERT_TRUE(utils.CreateSurface());
        defaultOption_.surface_ = utils.psurface_;
        ScreenId virtualScreenId = ScreenManager::GetInstance().CreateVirtualScreen(defaultOption_);
        std::vector<ScreenId> mirrorIds;
        mirrorIds.push_back(virtualScreenId);
        ScreenManager::GetInstance().MakeMirror(defaultScreenId_, mirrorIds);
        ASSERT_NE(SCREEN_ID_INVALID, virtualScreenId);
        ASSERT_EQ(DMError::DM_OK, ScreenManager::GetInstance().DestroyVirtualScreen(virtualScreenId));
        usleep(sleepUs_);
    }
}

/**
 * @tc.name: ScreenManager05
 * @tc.desc: Compare the length and width for recording screen
 * @tc.type: FUNC
 */
HWTEST_F(ScreenManagerTest, ScreenManager05, Function | MediumTest | Level2)
{
    DisplayTestUtils utils;
    utils.SetDefaultWH(defaultDisplay_);
    ASSERT_TRUE(utils.CreateSurface());
    defaultOption_.surface_ = utils.psurface_;
    defaultOption_.isForShot_ = true;
    ScreenId virtualScreenId = ScreenManager::GetInstance().CreateVirtualScreen(defaultOption_);

    ASSERT_NE(SCREEN_ID_INVALID, virtualScreenId);
    uint32_t lastCount = -1u;
    std::vector<ScreenId> mirrorIds;
    mirrorIds.push_back(virtualScreenId);
    ScreenManager::GetInstance().MakeMirror(defaultScreenId_, mirrorIds);

    while (utils.successCount_ < acquireFrames_ && waitCount_ <=  maxWaitCount_) {
        if (lastCount != utils.successCount_) {
            lastCount = utils.successCount_;
        }
        ASSERT_EQ(0, utils.failCount_);
        waitCount_++;
        usleep(sleepUs_);
    }
    ASSERT_EQ(DMError::DM_OK, ScreenManager::GetInstance().DestroyVirtualScreen(virtualScreenId));
    ASSERT_GT(utils.successCount_, 0);
    ASSERT_GT(maxWaitCount_, waitCount_);
    waitCount_ = 0;
}

/**
 * @tc.name: ScreenManager06
 * @tc.desc: Compare the length and width for recording screen, set VirtualScreen Surface before make mirror.
 * @tc.type: FUNC
 */
HWTEST_F(ScreenManagerTest, ScreenManager06, Function | MediumTest | Level2)
{
    DisplayTestUtils utils;
    utils.SetDefaultWH(defaultDisplay_);
    defaultOption_.surface_ = nullptr;
    defaultOption_.isForShot_ = true;
    ScreenId virtualScreenId = ScreenManager::GetInstance().CreateVirtualScreen(defaultOption_);
    ASSERT_NE(SCREEN_ID_INVALID, virtualScreenId);

    ASSERT_TRUE(utils.CreateSurface());
    ASSERT_EQ(DMError::DM_OK, ScreenManager::GetInstance().SetVirtualScreenSurface(virtualScreenId, utils.psurface_));

    uint32_t lastCount = -1u;
    std::vector<ScreenId> mirrorIds;
    mirrorIds.push_back(virtualScreenId);
    ScreenManager::GetInstance().MakeMirror(defaultScreenId_, mirrorIds);

    while (utils.successCount_ < acquireFrames_ && waitCount_ <=  maxWaitCount_) {
        if (lastCount != utils.successCount_) {
            lastCount = utils.successCount_;
        }
        ASSERT_EQ(0, utils.failCount_);
        waitCount_++;
        usleep(sleepUs_);
    }
    DMError res = ScreenManager::GetInstance().DestroyVirtualScreen(virtualScreenId);
    ASSERT_EQ(DMError::DM_OK, res);
    ASSERT_GT(utils.successCount_, 0);
    ASSERT_GT(maxWaitCount_, waitCount_);
    waitCount_ = 0;
}

/**
 * @tc.name: ScreenManager07
 * @tc.desc: Get and set screenMode
 * @tc.type: FUNC
 */
HWTEST_F(ScreenManagerTest, ScreenManager07, Function | MediumTest | Level2)
{
    sptr<Screen> screen = ScreenManager::GetInstance().GetScreenById(defaultScreenId_);
    auto modes = screen->GetSupportedModes();
    auto defaultModeId = screen->GetModeId();
    ASSERT_GT(modes.size(), 0);
    for (uint32_t modeIdx = 0; modeIdx < modes.size(); modeIdx++) {
        ASSERT_EQ(true, screen->SetScreenActiveMode(modeIdx));
        sleep(TEST_SLEEP_S);
        ASSERT_EQ(modeIdx, screen->GetModeId());
    }
    ASSERT_EQ(true, screen->SetScreenActiveMode(defaultModeId));
    sleep(TEST_SLEEP_S);
}

/**
 * @tc.name: ScreenManager08
 * @tc.desc: Create a virtual screen as expansion of default screen, and destroy virtual screen
 * @tc.type: FUNC
 */
HWTEST_F(ScreenManagerTest, ScreenManager08, Function | MediumTest | Level2)
{
    DisplayTestUtils utils;
    ASSERT_TRUE(utils.CreateSurface());
    defaultOption_.surface_ = utils.psurface_;
    defaultOption_.isForShot_ = false;
    CHECK_TEST_INIT_SCREEN_STATE
    ScreenId virtualScreenId = ScreenManager::GetInstance().CreateVirtualScreen(defaultOption_);
    CHECK_SCREEN_STATE_AFTER_CREATE_VIRTUAL_SCREEN
    CheckScreenStateInGroup(false, group, groupId, virtualScreen, virtualScreenId);
    std::vector<sptr<Screen>> screens = ScreenManager::GetInstance().GetAllScreens();
    sptr<Screen> defaultScreen = screens.front();
    std::vector<ExpandOption> options = {{defaultScreen->GetId(), 0, 0}, {virtualScreenId, defaultWidth_, 0}};
    ScreenId expansionGroup = ScreenManager::GetInstance().MakeExpand(options);
    ASSERT_NE(SCREEN_ID_INVALID, expansionGroup);
    CheckScreenGroupState(ScreenCombination::SCREEN_EXPAND, ScreenGroupChangeEvent::ADD_TO_GROUP,
        virtualScreenId, group, screenGroupChangeListener);
    CheckScreenStateInGroup(true, group, groupId, virtualScreen, virtualScreenId);
    ASSERT_EQ(DMError::DM_OK, ScreenManager::GetInstance().DestroyVirtualScreen(virtualScreenId));
    CHECK_SCREEN_STATE_AFTER_DESTROY_VIRTUAL_SCREEN
    CheckScreenGroupState(ScreenCombination::SCREEN_EXPAND, ScreenGroupChangeEvent::REMOVE_FROM_GROUP,
        virtualScreenId, group, screenGroupChangeListener);
    CheckScreenStateInGroup(false, group, groupId, virtualScreen, virtualScreenId);
    ScreenManager::GetInstance().UnregisterScreenListener(screenListener);
    ScreenManager::GetInstance().UnregisterScreenGroupListener(screenGroupChangeListener);
    ScreenManager::GetInstance().UnregisterVirtualScreenGroupListener(virtualScreenGroupChangeListener);
}

/**
 * @tc.name: ScreenManager09
 * @tc.desc: Create a virtual screen as expansion of default screen, create windowNode on virtual screen,
 *           and destroy virtual screen
 * @tc.type: FUNC
 */
HWTEST_F(ScreenManagerTest, ScreenManager09, Function | MediumTest | Level2)
{
    system::SetParameter("rosen.uni.partialrender.enabled", "0");
    DisplayTestUtils utils;
    ASSERT_TRUE(utils.CreateSurface());
    defaultOption_.surface_ = utils.psurface_;
    defaultOption_.isForShot_ = false;
    CHECK_TEST_INIT_SCREEN_STATE
    ScreenId virtualScreenId = ScreenManager::GetInstance().CreateVirtualScreen(defaultOption_);
    CHECK_SCREEN_STATE_AFTER_CREATE_VIRTUAL_SCREEN
    CheckScreenStateInGroup(false, group, groupId, virtualScreen, virtualScreenId);
    sleep(TEST_SLEEP_S);
    std::vector<ExpandOption> options = {{defaultScreenId_, 0, 0}, {virtualScreenId, defaultWidth_, 0}};
    ScreenId expansionId = ScreenManager::GetInstance().MakeExpand(options);
    CheckScreenGroupState(ScreenCombination::SCREEN_EXPAND, ScreenGroupChangeEvent::ADD_TO_GROUP,
        virtualScreenId, group, screenGroupChangeListener);
    CheckScreenStateInGroup(true, group, groupId, virtualScreen, virtualScreenId);
    sleep(TEST_SLEEP_S);
    ASSERT_NE(SCREEN_ID_INVALID, expansionId);
    DisplayId virtualDisplayId = DisplayManager::GetInstance().GetDisplayByScreen(virtualScreenId)->GetId();
    ASSERT_NE(DISPLAY_ID_INVALID, virtualDisplayId);
    sptr<Window> window = CreateWindowByDisplayId(virtualDisplayId);
    ASSERT_NE(nullptr, window);
    ASSERT_EQ(true, DrawWindowColor(window, COLOR_RED));
    sleep(TEST_SLEEP_S);
    ASSERT_EQ(DMError::DM_OK, ScreenManager::GetInstance().DestroyVirtualScreen(virtualScreenId));
    CHECK_SCREEN_STATE_AFTER_DESTROY_VIRTUAL_SCREEN
    CheckScreenGroupState(ScreenCombination::SCREEN_EXPAND, ScreenGroupChangeEvent::REMOVE_FROM_GROUP,
        virtualScreenId, group, screenGroupChangeListener);
    CheckScreenStateInGroup(false, group, groupId, virtualScreen, virtualScreenId);
    ScreenManager::GetInstance().UnregisterScreenListener(screenListener);
    ScreenManager::GetInstance().UnregisterScreenGroupListener(screenGroupChangeListener);
    ScreenManager::GetInstance().UnregisterVirtualScreenGroupListener(virtualScreenGroupChangeListener);
    sleep(TEST_SLEEP_S);
    window->Show();
    sleep(TEST_SLEEP_S_LONG);
    window->Destroy();
    sleep(TEST_SLEEP_S); // Wait for the window object to be destructed
    system::SetParameter("rosen.uni.partialrender.enabled", "4");
}

/**
 * @tc.name: ScreenManager10
 * @tc.desc: Create a virtual screen and destroy it for 10 times, it's not for shot.
 * @tc.type: FUNC
 */
HWTEST_F(ScreenManagerTest, ScreenManager10, Function | MediumTest | Level2)
{
    DisplayTestUtils utils;
    ASSERT_TRUE(utils.CreateSurface());
    defaultOption_.surface_ = utils.psurface_;
    defaultOption_.isForShot_ = false;
    for (uint32_t i = 0; i < execTimes_; i++) {
        CHECK_TEST_INIT_SCREEN_STATE
        ScreenId virtualScreenId = ScreenManager::GetInstance().CreateVirtualScreen(defaultOption_);
        CHECK_SCREEN_STATE_AFTER_CREATE_VIRTUAL_SCREEN
        CheckScreenStateInGroup(false, group, groupId, virtualScreen, virtualScreenId);
        ASSERT_EQ(DMError::DM_OK, ScreenManager::GetInstance().DestroyVirtualScreen(virtualScreenId));
        CHECK_SCREEN_STATE_AFTER_DESTROY_VIRTUAL_SCREEN
        CheckScreenStateInGroup(false, group, groupId, virtualScreen, virtualScreenId);
        ScreenManager::GetInstance().UnregisterScreenListener(screenListener);
        usleep(sleepUs_);
    }
}

/**
 * @tc.name: ScreenManager11
 * @tc.desc: Create a virtual screen , mirror and destroy it for 10 times, it's not for shot.
 * @tc.type: FUNC
 * @tc.require: issueI5M2SK
 */
HWTEST_F(ScreenManagerTest, ScreenManager11, Function | MediumTest | Level2)
{
    DisplayTestUtils utils;
    ASSERT_TRUE(utils.CreateSurface());
    defaultOption_.surface_ = utils.psurface_;
    defaultOption_.isForShot_ = false;
    for (uint32_t i = 0; i < 10; i++) {
        usleep(sleepUs_);
        CHECK_TEST_INIT_SCREEN_STATE
        ScreenId virtualScreenId = ScreenManager::GetInstance().CreateVirtualScreen(defaultOption_);
        CHECK_SCREEN_STATE_AFTER_CREATE_VIRTUAL_SCREEN
        CheckScreenStateInGroup(false, group, groupId, virtualScreen, virtualScreenId);

        std::vector<ScreenId> mirrorIds;
        mirrorIds.push_back(virtualScreenId);
        auto mirrorGroup = ScreenManager::GetInstance().MakeMirror(defaultScreenId_, mirrorIds);
        ASSERT_EQ(mirrorGroup, groupId);
        CheckScreenGroupState(ScreenCombination::SCREEN_MIRROR, ScreenGroupChangeEvent::ADD_TO_GROUP,
            virtualScreenId, group, screenGroupChangeListener);
        CheckScreenGroupStateForMirror(ScreenGroupChangeEvent::ADD_TO_GROUP, mirrorIds,
            virtualScreenGroupChangeListener);
        CheckScreenStateInGroup(true, group, groupId, virtualScreen, virtualScreenId);

        auto screen = ScreenManager::GetInstance().GetScreenById(virtualScreenId);
        ASSERT_EQ(virtualScreenId, screen->GetId());
        ASSERT_NE(SCREEN_ID_INVALID, screen->GetParentId());
        ASSERT_EQ(DMError::DM_OK, ScreenManager::GetInstance().DestroyVirtualScreen(virtualScreenId));
        CHECK_SCREEN_STATE_AFTER_DESTROY_VIRTUAL_SCREEN
        CheckScreenGroupState(ScreenCombination::SCREEN_MIRROR, ScreenGroupChangeEvent::REMOVE_FROM_GROUP,
            virtualScreenId, group, screenGroupChangeListener);
        CheckScreenGroupStateForMirror(ScreenGroupChangeEvent::REMOVE_FROM_GROUP, mirrorIds,
            virtualScreenGroupChangeListener);
        CheckScreenStateInGroup(false, group, groupId, virtualScreen, virtualScreenId);
        ScreenManager::GetInstance().UnregisterScreenListener(screenListener);
        ScreenManager::GetInstance().UnregisterScreenGroupListener(screenGroupChangeListener);
        ScreenManager::GetInstance().UnregisterVirtualScreenGroupListener(virtualScreenGroupChangeListener);
    }
    usleep(sleepUs_);
}

/**
 * @tc.name: ScreenManager12
 * @tc.desc: Create a virtual screen as expansion of default screen cancel Make mirror, and destroy virtual screen
 * @tc.type: FUNC
 * @tc.require: issueI5M2SK
 */
HWTEST_F(ScreenManagerTest, ScreenManager12, Function | MediumTest | Level2)
{
    DisplayTestUtils utils;
    ASSERT_TRUE(utils.CreateSurface());
    defaultOption_.surface_ = utils.psurface_;
    defaultOption_.isForShot_ = false;
    CHECK_TEST_INIT_SCREEN_STATE
    ScreenId virtualScreenId = ScreenManager::GetInstance().CreateVirtualScreen(defaultOption_);
    CHECK_SCREEN_STATE_AFTER_CREATE_VIRTUAL_SCREEN
    CheckScreenStateInGroup(false, group, groupId, virtualScreen, virtualScreenId);

    std::vector<ScreenId> mirrorIds;
    mirrorIds.push_back(virtualScreenId);
    auto mirrorGroup = ScreenManager::GetInstance().MakeMirror(defaultScreenId_, mirrorIds);
    ASSERT_EQ(mirrorGroup, groupId);
    CheckScreenGroupState(ScreenCombination::SCREEN_MIRROR, ScreenGroupChangeEvent::ADD_TO_GROUP,
        virtualScreenId, group, screenGroupChangeListener);
    CheckScreenGroupStateForMirror(ScreenGroupChangeEvent::ADD_TO_GROUP, mirrorIds,
        virtualScreenGroupChangeListener);
    CheckScreenStateInGroup(true, group, groupId, virtualScreen, virtualScreenId);

    ScreenManager::GetInstance().RemoveVirtualScreenFromGroup(mirrorIds);
    CheckScreenGroupState(ScreenCombination::SCREEN_MIRROR, ScreenGroupChangeEvent::REMOVE_FROM_GROUP,
        virtualScreenId, group, screenGroupChangeListener);
    CheckScreenGroupStateForMirror(ScreenGroupChangeEvent::REMOVE_FROM_GROUP, mirrorIds,
        virtualScreenGroupChangeListener);
    CheckScreenStateInGroup(false, group, groupId, virtualScreen, virtualScreenId);

    ASSERT_EQ(DMError::DM_OK, ScreenManager::GetInstance().DestroyVirtualScreen(virtualScreenId));
    CHECK_SCREEN_STATE_AFTER_DESTROY_VIRTUAL_SCREEN
    CheckScreenStateInGroup(false, group, groupId, virtualScreen, virtualScreenId);
    ScreenManager::GetInstance().UnregisterScreenListener(screenListener);
    ScreenManager::GetInstance().UnregisterScreenGroupListener(screenGroupChangeListener);
    ScreenManager::GetInstance().UnregisterVirtualScreenGroupListener(virtualScreenGroupChangeListener);
    usleep(sleepUs_);
}

/**
 * @tc.name: ScreenManager13
 * @tc.desc: Create a virtual screen as expansion of default screen cancel MakeExpand, and destroy virtual screen
 * @tc.type: FUNC
 */
HWTEST_F(ScreenManagerTest, ScreenManager13, Function | MediumTest | Level2)
{
    DisplayTestUtils utils;
    ASSERT_TRUE(utils.CreateSurface());
    defaultOption_.surface_ = utils.psurface_;
    defaultOption_.isForShot_ = false;
    CHECK_TEST_INIT_SCREEN_STATE
    ScreenId virtualScreenId = ScreenManager::GetInstance().CreateVirtualScreen(defaultOption_);
    CHECK_SCREEN_STATE_AFTER_CREATE_VIRTUAL_SCREEN
    CheckScreenStateInGroup(false, group, groupId, virtualScreen, virtualScreenId);

    std::vector<sptr<Screen>> screens = ScreenManager::GetInstance().GetAllScreens();
    sptr<Screen> DefaultScreen = screens.front();
    std::vector<ExpandOption> options = {{DefaultScreen->GetId(), 0, 0}, {virtualScreenId, defaultWidth_, 0}};
    ScreenId expansionGroup = ScreenManager::GetInstance().MakeExpand(options);
    ASSERT_NE(SCREEN_ID_INVALID, expansionGroup);
    CheckScreenGroupState(ScreenCombination::SCREEN_EXPAND, ScreenGroupChangeEvent::ADD_TO_GROUP,
        virtualScreenId, group, screenGroupChangeListener);
    CheckScreenStateInGroup(true, group, groupId, virtualScreen, virtualScreenId);

    std::vector<ScreenId> cancelScreens;
    cancelScreens.emplace_back(virtualScreenId);
    ScreenManager::GetInstance().RemoveVirtualScreenFromGroup(cancelScreens);
    CheckScreenGroupState(ScreenCombination::SCREEN_EXPAND, ScreenGroupChangeEvent::REMOVE_FROM_GROUP,
        virtualScreenId, group, screenGroupChangeListener);
    CheckScreenStateInGroup(false, group, groupId, virtualScreen, virtualScreenId);

    ASSERT_EQ(DMError::DM_OK, ScreenManager::GetInstance().DestroyVirtualScreen(virtualScreenId));
    CHECK_SCREEN_STATE_AFTER_DESTROY_VIRTUAL_SCREEN
    CheckScreenStateInGroup(false, group, groupId, virtualScreen, virtualScreenId);
    ScreenManager::GetInstance().UnregisterScreenListener(screenListener);
    ScreenManager::GetInstance().UnregisterScreenGroupListener(screenGroupChangeListener);
    ScreenManager::GetInstance().UnregisterVirtualScreenGroupListener(virtualScreenGroupChangeListener);
    usleep(sleepUs_);
}

/**
 * @tc.name: ScreenManager14
 * @tc.desc: Create a virtual screen, make expand to make mirror, and destroy virtual screen
 * @tc.type: FUNC
 */
HWTEST_F(ScreenManagerTest, ScreenManager14, Function | MediumTest | Level2)
{
    DisplayTestUtils utils;
    ASSERT_TRUE(utils.CreateSurface());
    defaultOption_.surface_ = utils.psurface_;
    defaultOption_.isForShot_ = false;
    CHECK_TEST_INIT_SCREEN_STATE
    ScreenId virtualScreenId = ScreenManager::GetInstance().CreateVirtualScreen(defaultOption_);
    CHECK_SCREEN_STATE_AFTER_CREATE_VIRTUAL_SCREEN
    CheckScreenStateInGroup(false, group, groupId, virtualScreen, virtualScreenId);

    std::vector<sptr<Screen>> screens = ScreenManager::GetInstance().GetAllScreens();
    sptr<Screen> DefaultScreen = screens.front();
    std::vector<ExpandOption> options = {{DefaultScreen->GetId(), 0, 0}, {virtualScreenId, defaultWidth_, 0}};
    ScreenId expansionGroup = ScreenManager::GetInstance().MakeExpand(options);
    ASSERT_EQ(expansionGroup, groupId);
    CheckScreenGroupState(ScreenCombination::SCREEN_EXPAND, ScreenGroupChangeEvent::ADD_TO_GROUP,
        virtualScreenId, group, screenGroupChangeListener);
    CheckScreenStateInGroup(true, group, groupId, virtualScreen, virtualScreenId);

    std::vector<ScreenId> mirrorScreens;
    mirrorScreens.emplace_back(virtualScreenId);
    auto mirrorGroup = ScreenManager::GetInstance().MakeMirror(defaultScreenId_, mirrorScreens);
    ASSERT_EQ(mirrorGroup, groupId);
    CheckScreenGroupState(ScreenCombination::SCREEN_MIRROR, ScreenGroupChangeEvent::CHANGE_GROUP,
        virtualScreenId, group, screenGroupChangeListener);
    CheckScreenStateInGroup(true, group, groupId, virtualScreen, virtualScreenId);

    ASSERT_EQ(DMError::DM_OK, ScreenManager::GetInstance().DestroyVirtualScreen(virtualScreenId));
    CHECK_SCREEN_STATE_AFTER_DESTROY_VIRTUAL_SCREEN
    CheckScreenGroupState(ScreenCombination::SCREEN_MIRROR, ScreenGroupChangeEvent::REMOVE_FROM_GROUP,
        virtualScreenId, group, screenGroupChangeListener);
    CheckScreenStateInGroup(false, group, groupId, virtualScreen, virtualScreenId);

    ScreenManager::GetInstance().UnregisterScreenListener(screenListener);
    ScreenManager::GetInstance().UnregisterScreenGroupListener(screenGroupChangeListener);
    ScreenManager::GetInstance().UnregisterVirtualScreenGroupListener(virtualScreenGroupChangeListener);
    usleep(sleepUs_);
}

/**
 * @tc.name: ScreenManager15
 * @tc.desc: Create a virtual screen, make mirror to make expand, and destroy virtual screen
 * @tc.type: FUNC
 * @tc.require: issueI5M2SK
 */
HWTEST_F(ScreenManagerTest, ScreenManager15, Function | MediumTest | Level2)
{
    DisplayTestUtils utils;
    ASSERT_TRUE(utils.CreateSurface());
    defaultOption_.surface_ = utils.psurface_;
    defaultOption_.isForShot_ = false;

    CHECK_TEST_INIT_SCREEN_STATE
    ScreenId virtualScreenId = ScreenManager::GetInstance().CreateVirtualScreen(defaultOption_);
    CHECK_SCREEN_STATE_AFTER_CREATE_VIRTUAL_SCREEN
    CheckScreenStateInGroup(false, group, groupId, virtualScreen, virtualScreenId);

    std::vector<ScreenId> mirrorScreens;
    mirrorScreens.emplace_back(virtualScreenId);
    auto mirrorGroup = ScreenManager::GetInstance().MakeMirror(defaultScreenId_, mirrorScreens);
    ASSERT_EQ(mirrorGroup, groupId);
    CheckScreenGroupState(ScreenCombination::SCREEN_MIRROR, ScreenGroupChangeEvent::ADD_TO_GROUP,
        virtualScreenId, group, screenGroupChangeListener);
    CheckScreenGroupStateForMirror(ScreenGroupChangeEvent::ADD_TO_GROUP, mirrorScreens,
        virtualScreenGroupChangeListener);
    CheckScreenStateInGroup(true, group, groupId, virtualScreen, virtualScreenId);

    std::vector<sptr<Screen>> screens = ScreenManager::GetInstance().GetAllScreens();
    sptr<Screen> defaultScreen = screens.front();
    std::vector<ExpandOption> options = {{defaultScreen->GetId(), 0, 0}, {virtualScreenId, defaultWidth_, 0}};
    ScreenId expansionGroup = ScreenManager::GetInstance().MakeExpand(options);
    ASSERT_EQ(expansionGroup, groupId);
    CheckScreenGroupState(ScreenCombination::SCREEN_EXPAND, ScreenGroupChangeEvent::CHANGE_GROUP,
        virtualScreenId, group, screenGroupChangeListener);
    CheckScreenStateInGroup(true, group, groupId, virtualScreen, virtualScreenId);

    ASSERT_EQ(DMError::DM_OK, ScreenManager::GetInstance().DestroyVirtualScreen(virtualScreenId));
    CHECK_SCREEN_STATE_AFTER_DESTROY_VIRTUAL_SCREEN
    CheckScreenGroupState(ScreenCombination::SCREEN_EXPAND, ScreenGroupChangeEvent::REMOVE_FROM_GROUP,
        virtualScreenId, group, screenGroupChangeListener);
    CheckScreenStateInGroup(false, group, groupId, virtualScreen, virtualScreenId);

    ScreenManager::GetInstance().UnregisterScreenListener(screenListener);
    ScreenManager::GetInstance().UnregisterScreenGroupListener(screenGroupChangeListener);
    ScreenManager::GetInstance().UnregisterVirtualScreenGroupListener(virtualScreenGroupChangeListener);
    usleep(sleepUs_);
}

/**
 * @tc.name: ScreenManager16
 * @tc.desc: Screen orientation.
 * @tc.type: FUNC
 * @tc.require: issueI5NDLK
 */
HWTEST_F(ScreenManagerTest, ScreenManager16, Function | MediumTest | Level2)
{
    auto screens = ScreenManager::GetInstance().GetAllScreens();
    auto display = DisplayManager::GetInstance().GetDefaultDisplay();
    uint32_t orientation = static_cast<uint32_t>(Orientation::VERTICAL);
    uint32_t end = static_cast<uint32_t>(Orientation::REVERSE_HORIZONTAL);
    sptr<ScreenChangeListener> screenListener = new ScreenChangeListener();
    ScreenManager::GetInstance().RegisterScreenListener(screenListener);
    for (; orientation <= end; ++orientation) {
        usleep(sleepUs_);
        screens[0]->SetOrientation(static_cast<Orientation>(orientation));
        ScreenId screenId = screenListener->changeFuture_.GetResult(TIME_OUT);
        ASSERT_EQ(screenId, screens[0]->GetId());
        usleep(1E6);
        ASSERT_EQ(static_cast<uint32_t>(screens[0]->GetOrientation()), orientation);
        ASSERT_EQ(static_cast<uint32_t>(display->GetOrientation()), orientation);
    }
    screens[0]->SetOrientation(Orientation::UNSPECIFIED);
    ASSERT_EQ(static_cast<uint32_t>(screens[0]->GetOrientation()), static_cast<uint32_t>(Orientation::UNSPECIFIED));
    ASSERT_EQ(static_cast<uint32_t>(display->GetOrientation()), static_cast<uint32_t>(Orientation::UNSPECIFIED));
    ScreenManager::GetInstance().UnregisterScreenListener(screenListener);
    usleep(sleepUs_);
}

/**
 * @tc.name: ScreenManager17
 * @tc.desc: Create VirtualScreen for 10 times but do not destroy it
 * @tc.type: FUNC
 */
HWTEST_F(ScreenManagerTest, ScreenManager17, Function | MediumTest | Level2)
{
    DisplayTestUtils utils;
    defaultOption_.isForShot_ = false;
    for (uint32_t i = 0; i < execTimes_; i++) {
        usleep(sleepUs_);
        ASSERT_TRUE(utils.CreateSurface());
        defaultOption_.surface_ = utils.psurface_;
        ScreenId virtualScreenId = ScreenManager::GetInstance().CreateVirtualScreen(defaultOption_);
        ASSERT_NE(SCREEN_ID_INVALID, virtualScreenId);
    }
    usleep(sleepUs_);
}

/**
 * @tc.name: ScreenManager18
 * @tc.desc: Set screen rotation lock, and check whether screen rotation lock is Locked.
 * @tc.type: FUNC
 */
HWTEST_F(ScreenManagerTest, ScreenManager18, Function | SmallTest | Level1)
{
    bool originalLockStatus = ScreenManager::GetInstance().IsScreenRotationLocked();
    ScreenManager::GetInstance().SetScreenRotationLocked(!originalLockStatus);
    bool modifiedLockedStatus = ScreenManager::GetInstance().IsScreenRotationLocked();
    ScreenManager::GetInstance().SetScreenRotationLocked(originalLockStatus);
    ASSERT_EQ(!originalLockStatus, modifiedLockedStatus);
    usleep(sleepUs_);
}
}
} // namespace Rosen
} // namespace OHOS
