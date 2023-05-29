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

#include <cinttypes>
#include <gtest/gtest.h>

#include "common_test_utils.h"
#include "display_test_utils.h"
#include "future.h"
#include "pixel_map.h"
#include "screenshot_listener_future.h"

using namespace testing;
using namespace testing::ext;

namespace OHOS {
namespace Rosen {
namespace {
    constexpr HiviewDFX::HiLogLabel LABEL = {LOG_CORE, HILOG_DOMAIN_DISPLAY, "ScreenshotTest"};
}
using Utils = DisplayTestUtils;
class ScreenshotListener;
class ScreenshotTest : public testing::Test {
public:
    static void SetUpTestCase();
    static void TearDownTestCase();
    virtual void SetUp() override;
    virtual void TearDown() override;
    static DisplayId defaultId_;
    static Media::Rect defaultScreen_;
    static Media::Size defaultImage_;
    DisplayId invalidId_ = DISPLAY_ID_INVALID;
    Media::Rect invalidRect_ = {-1, -1, -1, -1};
    uint32_t defaultRot_ = 0;
};

DisplayId ScreenshotTest::defaultId_ = DISPLAY_ID_INVALID;
Media::Rect ScreenshotTest::defaultScreen_ = {0, 0, 0, 0};
Media::Size ScreenshotTest::defaultImage_ = {0, 0};

void ScreenshotTest::SetUpTestCase()
{
    auto display = DisplayManager::GetInstance().GetDefaultDisplay();
    if (display == nullptr) {
        WLOGFE("GetDefaultDisplay: failed!\n");
        return;
    }
    WLOGFI("GetDefaultDisplay: id %" PRIu64", w %d, h %d, fps %u\n", display->GetId(), display->GetWidth(),
        display->GetHeight(), display->GetRefreshRate());

    defaultId_ = display->GetId();
    defaultScreen_ = {0, 0, display->GetWidth(), display->GetHeight()};
    defaultImage_ = {display->GetWidth(), display->GetHeight()};

    CommonTestUtils::InjectTokenInfoByHapName(0, "com.ohos.systemui", 0);
}

void ScreenshotTest::TearDownTestCase()
{
}

void ScreenshotTest::SetUp()
{
}

void ScreenshotTest::TearDown()
{
}

namespace {
/**
 * @tc.name: ScreenShotValid
 * @tc.desc: Check if screenshot of default display's ID is valid
 * @tc.type: FUNC
 */
HWTEST_F(ScreenshotTest, ScreenShotValid01, Function | MediumTest | Level2)
{
    ASSERT_NE(nullptr, DisplayManager::GetInstance().GetScreenshot(defaultId_));
}

/**
 * @tc.name: ScreenShotValid
 * @tc.desc: Check if screenshot of invalid display's ID is nullptr
 * @tc.type: FUNC
 */
HWTEST_F(ScreenshotTest, ScreenShotValid02, Function | MediumTest | Level2)
{
    ASSERT_EQ(nullptr, DisplayManager::GetInstance().GetScreenshot(invalidId_));
}

/**
 * @tc.name: ScreenShotValid
 * @tc.desc: Check if screenshot of default display's ID match default display's Media::Size
 * @tc.type: FUNC
 */
HWTEST_F(ScreenshotTest, ScreenShotValid03, Function | MediumTest | Level2)
{
    auto& dm = DisplayManager::GetInstance();
    std::shared_ptr<Media::PixelMap> screenshot = dm.GetScreenshot(defaultId_);
    ASSERT_NE(nullptr, screenshot);

    Media::Size screenSize = {screenshot->GetWidth(), screenshot->GetHeight()};
    ASSERT_TRUE(Utils::SizeEqualToDisplay(dm.GetDefaultDisplay(), screenSize));
}

/**
 * @tc.name: ScreenShotValid
 * @tc.desc: Check if screenshot created by default parameters is valid
 * @tc.type: FUNC
 */
HWTEST_F(ScreenshotTest, ScreenShotValid04, Function | MediumTest | Level2)
{
    auto& dm = DisplayManager::GetInstance();
    std::shared_ptr<Media::PixelMap> screenshot = dm.GetScreenshot(defaultId_, defaultScreen_,
                                                                   defaultImage_, defaultRot_);
    ASSERT_NE(nullptr, screenshot);
}

/**
 * @tc.name: ScreenShotValid
 * @tc.desc: Check if screenshot match default imageSize
 * @tc.type: FUNC
 */
HWTEST_F(ScreenshotTest, ScreenShotValid05, Function | MediumTest | Level2)
{
    auto& dm = DisplayManager::GetInstance();
    std::shared_ptr<Media::PixelMap> screenshot = dm.GetScreenshot(defaultId_, defaultScreen_,
                                                                   defaultImage_, defaultRot_);
    ASSERT_NE(nullptr, screenshot);
    Media::Size screenSize = {screenshot->GetWidth(), screenshot->GetHeight()};
    ASSERT_TRUE(Utils::SizeEqual(defaultImage_, screenSize));

    Media::Size halfDefault_ = {defaultImage_.width / 2, defaultImage_.height / 2};
    Media::Rect halfRect = {defaultScreen_.left, defaultScreen_.top, halfDefault_.width, halfDefault_.height};
    screenshot = dm.GetScreenshot(defaultId_, halfRect, defaultImage_, defaultRot_);
    ASSERT_NE(nullptr, screenshot);
    screenSize = {screenshot->GetWidth(), screenshot->GetHeight()};
    ASSERT_TRUE(Utils::SizeEqual(defaultImage_, screenSize));

    Media::Size halfSize = {halfDefault_.width, halfDefault_.height};
    screenshot = dm.GetScreenshot(defaultId_, defaultScreen_, halfSize, defaultRot_);
    ASSERT_NE(nullptr, screenshot);
    screenSize = {screenshot->GetWidth(), screenshot->GetHeight()};
    ASSERT_TRUE(Utils::SizeEqual(halfSize, screenSize));
}

/**
 * @tc.name: ScreenShotValid
 * @tc.desc: Check if screenshot created by default parameters match default display's Media::Size
 * @tc.type: FUNC
 */
HWTEST_F(ScreenshotTest, ScreenShotValid06, Function | MediumTest | Level2)
{
    auto& dm = DisplayManager::GetInstance();
    std::shared_ptr<Media::PixelMap> screenshot = dm.GetScreenshot(defaultId_, defaultScreen_,
                                                                   defaultImage_, defaultRot_);
    ASSERT_NE(nullptr, screenshot);
    Media::Size screenSize = {screenshot->GetWidth(), screenshot->GetHeight()};
    ASSERT_TRUE(Utils::SizeEqualToDisplay(dm.GetDefaultDisplay(), screenSize));
}

/**
 * @tc.name: ScreenShotValid
 * @tc.desc: Check if screenshot created by invalid display ID is nullptr
 * @tc.type: FUNC
 */
HWTEST_F(ScreenshotTest, ScreenShot07, Function | MediumTest | Level2)
{
    auto& dm = DisplayManager::GetInstance();
    std::shared_ptr<Media::PixelMap> screenshot = dm.GetScreenshot(invalidId_, defaultScreen_,
                                                                   defaultImage_, defaultRot_);
    ASSERT_EQ(nullptr, screenshot);
}

/**
 * @tc.name: ScreenShotValid
 * @tc.desc: Check if screenshot created by invalid screenRect is nullptr
 * @tc.type: FUNC
 */
HWTEST_F(ScreenshotTest, ScreenShot08, Function | MediumTest | Level2)
{
    auto& dm = DisplayManager::GetInstance();
    std::shared_ptr<Media::PixelMap> screenshot = dm.GetScreenshot(defaultId_, invalidRect_,
                                                                   defaultImage_, defaultRot_);
    ASSERT_EQ(nullptr, screenshot);
    Media::Rect invalidScreen = {invalidRect_.left, defaultScreen_.top, defaultScreen_.width, defaultScreen_.height};
    screenshot = dm.GetScreenshot(defaultId_, invalidScreen, defaultImage_, defaultRot_);
    ASSERT_EQ(nullptr, screenshot);
    invalidScreen = {defaultScreen_.left, defaultScreen_.top, invalidRect_.width, defaultScreen_.height};
    screenshot = dm.GetScreenshot(defaultId_, invalidScreen, defaultImage_, defaultRot_);
    ASSERT_EQ(nullptr, screenshot);
}

/**
 * @tc.name: ScreenShotValid
 * @tc.desc: Check if screenshot created by invalid imageSize is nullptr
 * @tc.type: FUNC
 */
HWTEST_F(ScreenshotTest, ScreenShot09, Function | MediumTest | Level2)
{
    auto& dm = DisplayManager::GetInstance();
    Media::Size invalidSize = {invalidRect_.width, invalidRect_.height};
    std::shared_ptr<Media::PixelMap> screenshot = dm.GetScreenshot(defaultId_, defaultScreen_,
                                                                   invalidSize, defaultRot_);
    ASSERT_EQ(nullptr, screenshot);
    invalidSize = {invalidRect_.width, defaultScreen_.height};
    screenshot = dm.GetScreenshot(defaultId_, defaultScreen_, invalidSize, defaultRot_);
    ASSERT_EQ(nullptr, screenshot);
    invalidSize = {defaultScreen_.width, invalidRect_.height};
    screenshot = dm.GetScreenshot(defaultId_, defaultScreen_, invalidSize, defaultRot_);
    ASSERT_EQ(nullptr, screenshot);
}

/**
 * @tc.name: ScreenShotListener01
 * @tc.desc: Check if screenshot listener info valid
 * @tc.type: FUNC
 * @tc.require: issueI5G62O
 */
HWTEST_F(ScreenshotTest, ScreenShotListener01, Function | MediumTest | Level2)
{
    sptr<ScreenshotListenerFuture> screenShotListener = new ScreenshotListenerFuture();

    auto& dm = DisplayManager::GetInstance();
    dm.RegisterScreenshotListener(screenShotListener);
    std::shared_ptr<Media::PixelMap> screenshot = dm.GetScreenshot(defaultId_);
    auto info = screenShotListener->future_.GetResult(1000);

    ASSERT_NE(nullptr, screenshot);
    ASSERT_EQ(info.GetDisplayId(), defaultId_);
    ASSERT_GT(info.GetTrigger().size(), 0UL);

    dm.UnregisterScreenshotListener(screenShotListener);
}
} // namespace
} // namespace Rosen
} // namespace OHOS