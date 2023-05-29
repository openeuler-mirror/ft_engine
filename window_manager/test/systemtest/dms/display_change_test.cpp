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

#include "display_cutout_controller.h"
#include "display_info.h"
#include "display_manager.h"
#include "screen_manager.h"
#include "screen_manager/rs_screen_mode_info.h"
#include "window_manager_hilog.h"

using namespace testing;
using namespace testing::ext;

namespace OHOS {
namespace Rosen {
namespace {
    constexpr HiviewDFX::HiLogLabel LABEL = {LOG_CORE, HILOG_DOMAIN_DISPLAY, "DisplayChangeTest"};
    constexpr uint32_t MAX_TIME_WAITING_FOR_CALLBACK = 20;
    constexpr uint32_t SLEEP_TIME_IN_US = 10000; // 10ms
    constexpr uint32_t SPLIT_TEST_SLEEP_S = 2;
}

class DisplayChangeEventListener : public DisplayManager::IDisplayListener {
public:
    virtual void OnCreate(DisplayId displayId)
    {
        WLOGI("DisplayChangeEventListener::OnCreate displayId=%{public}" PRIu64"", displayId);
    }

    virtual void OnDestroy(DisplayId displayId)
    {
        WLOGI("DisplayChangeEventListener::OnDestroy displayId=%{public}" PRIu64"", displayId);
    }

    virtual void OnChange(DisplayId displayId)
    {
        WLOGI("DisplayChangeEventListener::OnChange displayId=%{public}" PRIu64"", displayId);
        isCallbackCalled_ = true;
        displayId_ = displayId;
    }
    bool isCallbackCalled_ = false;
    DisplayId displayId_ = DISPLAY_ID_INVALID;
};

class DisplayChangeTest : public testing::Test {
public:
    static void SetUpTestCase();
    static void TearDownTestCase();
    virtual void SetUp() override;
    virtual void TearDown() override;
    void ResetDisplayChangeListener();
    bool CheckDisplayChangeEventCallback(bool valueExpected);
    bool ScreenSizeEqual(const sptr<Screen> screen, const sptr<SupportedScreenModes> curInfo) const;
    bool DisplaySizeEqual(const sptr<Display> display, const sptr<SupportedScreenModes> curInfo) const;
    inline bool CheckModeSizeChange(const sptr<SupportedScreenModes> usedInfo,
        const sptr<SupportedScreenModes> curInfo) const;

    static DisplayId defaultDisplayId_;
    static sptr<Screen> defaultScreen_;
    static sptr<DisplayChangeEventListener> listener_;
    static uint32_t originalDisplayDpi;
    static inline uint32_t times_ = 0;
};
DisplayId DisplayChangeTest::defaultDisplayId_ = DISPLAY_ID_INVALID;
sptr<Screen> DisplayChangeTest::defaultScreen_ = nullptr;
sptr<DisplayChangeEventListener> DisplayChangeTest::listener_ = new DisplayChangeEventListener();
uint32_t DisplayChangeTest::originalDisplayDpi = 0;

void DisplayChangeTest::SetUpTestCase()
{
    defaultDisplayId_ = DisplayManager::GetInstance().GetDefaultDisplayId();
    ASSERT_NE(DISPLAY_ID_INVALID, defaultDisplayId_);
    sptr<Display> defaultDisplay = DisplayManager::GetInstance().GetDisplayById(defaultDisplayId_);
    ASSERT_NE(nullptr, defaultDisplay);
    ScreenId screenId = defaultDisplay->GetScreenId();
    ASSERT_NE(INVALID_SCREEN_ID, screenId);
    defaultScreen_ = ScreenManager::GetInstance().GetScreenById(screenId);
    ASSERT_NE(nullptr, defaultScreen_);

    ASSERT_EQ(true, DisplayManager::GetInstance().RegisterDisplayListener(listener_));
}

void DisplayChangeTest::TearDownTestCase()
{
    DisplayManager::GetInstance().UnregisterDisplayListener(listener_);
}

void DisplayChangeTest::SetUp()
{
    times_ = 0;
    ResetDisplayChangeListener();
}

void DisplayChangeTest::TearDown()
{
}

void DisplayChangeTest::ResetDisplayChangeListener()
{
    ASSERT_NE(nullptr, listener_);
    listener_->isCallbackCalled_ = false;
    listener_->displayId_ = DISPLAY_ID_INVALID;
}

bool DisplayChangeTest::CheckDisplayChangeEventCallback(bool valueExpected)
{
    WLOGI("CheckDisplayChangeEventCallback in");
    do {
        if (listener_->isCallbackCalled_ == valueExpected) {
            WLOGI("CheckDisplayChangeEventCallback: get valueExpected %{public}d for display %{public}" PRIu64"",
                static_cast<int>(valueExpected), listener_->displayId_);
            WLOGI("CheckDisplayChangeEventCallback: already wait times %{public}d", times_);
            return true;
        }
        usleep(SLEEP_TIME_IN_US);
        ++times_;
    } while (times_ <= MAX_TIME_WAITING_FOR_CALLBACK);
    WLOGI("CheckDisplayChangeEventCallback: cannot get valueExpected");
    return false;
}

bool DisplayChangeTest::ScreenSizeEqual(const sptr<Screen> screen, const sptr<SupportedScreenModes> curInfo) const
{
    if (screen == nullptr || curInfo == nullptr) {
        WLOGFI("param is nullptr");
        return false;
    }
    uint32_t sWidth = screen->GetWidth();
    uint32_t sHeight = screen->GetHeight();
    WLOGI("ScreenSizeEqual: ScreenSize: %{public}u %{public}u, ActiveModeInfoSize: %{public}u %{public}u",
        sWidth, sHeight, curInfo->width_, curInfo->height_);
    return ((curInfo->width_ == sWidth) && (curInfo->height_ == sHeight));
}

bool DisplayChangeTest::DisplaySizeEqual(const sptr<Display> display, const sptr<SupportedScreenModes> curInfo) const
{
    if (display == nullptr || curInfo == nullptr) {
        WLOGFI("param is nullptr");
        return false;
    }
    uint32_t dWidth = static_cast<uint32_t>(display->GetWidth());
    uint32_t dHeight = static_cast<uint32_t>(display->GetHeight());
    WLOGI("DisplaySizeEqual:: DisplaySize: %{public}u %{public}u, ActiveModeInfoSize: %{public}u %{public}u",
        dWidth, dHeight, curInfo->width_, curInfo->height_);
    return ((curInfo->width_ == dWidth) && (curInfo->height_ == dHeight));
}


inline bool DisplayChangeTest::CheckModeSizeChange(const sptr<SupportedScreenModes> usedInfo,
    const sptr<SupportedScreenModes> curInfo) const
{
    return (usedInfo->width_ != curInfo->width_ || usedInfo->height_ != curInfo->height_);
}

namespace {
/**
 * @tc.name: RegisterDisplayChangeListener01
 * @tc.desc: Register displayChangeListener with valid listener and check return true
 * @tc.type: FUNC
 */
HWTEST_F(DisplayChangeTest, RegisterDisplayChangeListener01, Function | SmallTest | Level2)
{
    sptr<DisplayChangeEventListener> listener = new DisplayChangeEventListener();
    bool ret = DisplayManager::GetInstance().RegisterDisplayListener(listener);
    ASSERT_EQ(true, ret);
}

/**
 * @tc.name: RegisterDisplayChangeListener02
 * @tc.desc: Register displayChangeListener with nullptr and check return false
 * @tc.type: FUNC
 */
HWTEST_F(DisplayChangeTest, RegisterDisplayChangeListener02, Function | SmallTest | Level2)
{
    bool ret = DisplayManager::GetInstance().RegisterDisplayListener(nullptr);
    ASSERT_EQ(false, ret);
}

/**
 * @tc.name: UnregisterDisplayChangeListener01
 * @tc.desc: Unregister displayChangeListener with valid listener and check return true
 * @tc.type: FUNC
 */
HWTEST_F(DisplayChangeTest, UnregisterDisplayChangeListener01, Function | SmallTest | Level2)
{
    sptr<DisplayChangeEventListener> listener = new DisplayChangeEventListener();
    DisplayManager::GetInstance().RegisterDisplayListener(listener);
    bool ret = DisplayManager::GetInstance().UnregisterDisplayListener(listener);
    ASSERT_EQ(true, ret);
}

/**
 * @tc.name: UnregisterDisplayChangeListener02
 * @tc.desc: Register displayChangeListener with nullptr and check return false
 * @tc.type: FUNC
 */
HWTEST_F(DisplayChangeTest, UnregisterDisplayChangeListener02, Function | SmallTest | Level2)
{
    bool ret = DisplayManager::GetInstance().UnregisterDisplayListener(nullptr);
    ASSERT_EQ(false, ret);
}

/**
 * @tc.name: UnregisterDisplayChangeListener03
 * @tc.desc: Register displayChangeListener with invalid listener and check return false
 * @tc.type: FUNC
 */
HWTEST_F(DisplayChangeTest, UnregisterDisplayChangeListener03, Function | SmallTest | Level2)
{
    sptr<DisplayChangeEventListener> listener = new DisplayChangeEventListener();
    bool ret = DisplayManager::GetInstance().UnregisterDisplayListener(listener);
    ASSERT_EQ(false, ret);
}

/**
 * @tc.name: CheckDisplayStateChange01
 * @tc.desc: DisplayState not change if screen sets same mode
 * @tc.type: FUNC
 */
HWTEST_F(DisplayChangeTest, CheckDisplayStateChange01, Function | SmallTest | Level2)
{
    WLOGFI("CheckDisplayStateChange01");
    uint32_t usedModeIdx = defaultScreen_->GetModeId();
    defaultScreen_->SetScreenActiveMode(usedModeIdx);
    WLOGFI("SetScreenActiveMode: %{public}u", usedModeIdx);
    ASSERT_EQ(false, CheckDisplayChangeEventCallback(true));
}

/**
 * @tc.name: CheckDisplayStateChange02
 * @tc.desc: DisplayState changes if screen sets different mode
 * @tc.type: FUNC
 */
HWTEST_F(DisplayChangeTest, CheckDisplayStateChange02, Function | SmallTest | Level2)
{
    WLOGFI("CheckDisplayStateChange02");
    auto modes = defaultScreen_->GetSupportedModes();
    uint32_t usedModeIdx = defaultScreen_->GetModeId();
    WLOGFI("usedModeIdx / SupportMode size: %{public}u %{public}zu", usedModeIdx, modes.size());

    for (uint32_t modeIdx = 0; modeIdx < modes.size(); modeIdx++) {
        if (modeIdx != usedModeIdx && CheckModeSizeChange(modes[usedModeIdx], modes[modeIdx])) {
            defaultScreen_->SetScreenActiveMode(modeIdx);
            WLOGFI("SetScreenActiveMode: %{public}u -> %{public}u", usedModeIdx, modeIdx);
            ASSERT_EQ(true, CheckDisplayChangeEventCallback(true));
            // reset usedMode
            ResetDisplayChangeListener();
            defaultScreen_->SetScreenActiveMode(usedModeIdx);
            CheckDisplayChangeEventCallback(true);
            break;
        }
    }
}

/**
 * @tc.name: CheckDisplaySizeChange01
 * @tc.desc: Check screen size change as screen mode set if screen sets another mode
 * @tc.type: FUNC
 */
HWTEST_F(DisplayChangeTest, CheckDisplaySizeChange01, Function | MediumTest | Level2)
{
    WLOGFI("CheckDisplaySizeChange01");
    auto modes = defaultScreen_->GetSupportedModes();
    uint32_t usedModeIdx = defaultScreen_->GetModeId();
    WLOGFI("usedModeIdx / SupportMode size: %{public}u %{public}zu", usedModeIdx, modes.size());

    for (uint32_t modeIdx = 0; modeIdx < modes.size(); modeIdx++) {
        if (modeIdx != usedModeIdx && CheckModeSizeChange(modes[usedModeIdx], modes[modeIdx])) {
            defaultScreen_->SetScreenActiveMode(modeIdx);
            WLOGFI("SetScreenActiveMode: %{public}u -> %{public}u", usedModeIdx, modeIdx);
            ASSERT_EQ(true, ScreenSizeEqual(defaultScreen_, modes[modeIdx]));
            ASSERT_EQ(true, CheckDisplayChangeEventCallback(true));
            // reset usedMode
            ResetDisplayChangeListener();
            defaultScreen_->SetScreenActiveMode(usedModeIdx);
            CheckDisplayChangeEventCallback(true);
            break;
        }
    }
}

/**
 * @tc.name: CheckDisplaySizeChange02
 * @tc.desc: Check display size change as screen mode set if screen sets another mode
 * @tc.type: FUNC
 */
HWTEST_F(DisplayChangeTest, CheckDisplaySizeChange02, Function | MediumTest | Level2)
{
    WLOGFI("CheckDisplaySizeChange02");
    auto modes = defaultScreen_->GetSupportedModes();
    uint32_t usedModeIdx = defaultScreen_->GetModeId();
    WLOGFI("usedModeIdx / SupportMode size: %{public}u %{public}zu", usedModeIdx, modes.size());

    for (uint32_t modeIdx = 0; modeIdx < modes.size(); modeIdx++) {
        if (modeIdx != usedModeIdx && CheckModeSizeChange(modes[usedModeIdx], modes[modeIdx])) {
            defaultScreen_->SetScreenActiveMode(modeIdx);
            WLOGFI("SetScreenActiveMode: %{public}u -> %{public}u", usedModeIdx, modeIdx);
            ASSERT_EQ(true, ScreenSizeEqual(defaultScreen_, modes[modeIdx]));
            ASSERT_EQ(true, CheckDisplayChangeEventCallback(true));
            sptr<Display> defaultDisplay = DisplayManager::GetInstance().GetDisplayById(defaultDisplayId_);
            ASSERT_NE(nullptr, defaultDisplay);
            ASSERT_EQ(true, DisplaySizeEqual(defaultDisplay, modes[modeIdx]));
            // reset usedMode
            ResetDisplayChangeListener();
            defaultScreen_->SetScreenActiveMode(usedModeIdx);
            CheckDisplayChangeEventCallback(true);
            break;
        }
    }
}

/**
 * @tc.name: CheckScreenDensityChange01
 * @tc.desc: Check screen density change as set another density for screen
 * @tc.type: FUNC
 */
HWTEST_F(DisplayChangeTest, CheckScreenDensityChange01, Function | SmallTest | Level2)
{
    DisplayChangeTest::originalDisplayDpi = static_cast<uint32_t>(DisplayManager::GetInstance().
        GetDisplayById(defaultDisplayId_)->GetVirtualPixelRatio() * BASELINE_DENSITY);
    ASSERT_NE(0, DisplayChangeTest::originalDisplayDpi);
    uint32_t densityDpi = 320;
    ASSERT_EQ(true, defaultScreen_->SetDensityDpi(densityDpi));
    sleep(SPLIT_TEST_SLEEP_S);
}

/**
 * @tc.name: CheckScreenDensityChange02
 * @tc.desc: Check screen density change as set another density for screen
 * @tc.type: FUNC
 */
HWTEST_F(DisplayChangeTest, CheckScreenDensityChange02, Function | SmallTest | Level2)
{
    uint32_t densityDpi = 80;
    ASSERT_EQ(true, defaultScreen_->SetDensityDpi(densityDpi));
    sleep(SPLIT_TEST_SLEEP_S);
}

/**
 * @tc.name: CheckScreenDensityChange03
 * @tc.desc: Check screen density change as set an invalid density for screen
 * @tc.type: FUNC
 */
HWTEST_F(DisplayChangeTest, CheckScreenDensityChange03, Function | SmallTest | Level2)
{
    uint32_t densityDpi = 700;
    ASSERT_EQ(false, defaultScreen_->SetDensityDpi(densityDpi));
}

/**
 * @tc.name: CheckScreenDensityChange04
 * @tc.desc: Check screen density change as set an invalid density for screen
 * @tc.type: FUNC
 */
HWTEST_F(DisplayChangeTest, CheckScreenDensityChange04, Function | SmallTest | Level2)
{
    uint32_t densityDpi = 40;
    ASSERT_EQ(false, defaultScreen_->SetDensityDpi(densityDpi));
}

/**
 * @tc.name: CheckScreenDensityChange05
 * @tc.desc: Restore original display density
 * @tc.type: FUNC
 */
HWTEST_F(DisplayChangeTest, CheckScreenDensityChange05, Function | SmallTest | Level2)
{
    ASSERT_EQ(true, defaultScreen_->SetDensityDpi(DisplayChangeTest::originalDisplayDpi));
    sleep(SPLIT_TEST_SLEEP_S);
}

/**
 * @tc.name: CheckWaterfallCompression01
 * @tc.desc: check function of waterfall display compression.
 * @tc.type: FUNC
 */
HWTEST_F(DisplayChangeTest, CheckWaterfallCompression01, Function | SmallTest | Level2)
{
    bool originWaterfallEnable = DisplayCutoutController::IsWaterfallDisplay();
    DisplayCutoutController::SetIsWaterfallDisplay(true);

    bool originStatus = DisplayCutoutController::IsWaterfallAreaCompressionEnableWhenHorizontal();
    DisplayCutoutController::SetWaterfallAreaCompressionEnableWhenHorzontal(true);

    uint32_t originSize = DisplayCutoutController::GetWaterfallAreaCompressionSizeWhenHorizontal();
    uint32_t testSizeInVp = 24;
    DisplayCutoutController::SetWaterfallAreaCompressionSizeWhenHorizontal(testSizeInVp);
    
    ASSERT_EQ(true, DisplayCutoutController::IsWaterfallAreaCompressionEnableWhenHorizontal());
    ASSERT_EQ(testSizeInVp, DisplayCutoutController::GetWaterfallAreaCompressionSizeWhenHorizontal());

    Orientation originOrientation = defaultScreen_->GetOrientation();
    DisplayCutoutController::SetWaterfallAreaCompressionSizeWhenHorizontal(originSize);
    ASSERT_EQ(originSize, DisplayCutoutController::GetWaterfallAreaCompressionSizeWhenHorizontal());
    DisplayCutoutController::SetWaterfallAreaCompressionEnableWhenHorzontal(originStatus);
    ASSERT_EQ(originStatus, DisplayCutoutController::IsWaterfallAreaCompressionEnableWhenHorizontal());
    DisplayCutoutController::SetIsWaterfallDisplay(originWaterfallEnable);
    ASSERT_EQ(originOrientation, defaultScreen_->GetOrientation());
}
}
} // namespace Rosen
} // namespace OHOS