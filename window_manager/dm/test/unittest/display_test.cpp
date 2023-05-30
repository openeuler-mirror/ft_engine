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
#include "display_manager.h"
#include "mock_display_manager_adapter.h"
#include "singleton_mocker.h"
#include "display_cutout_controller.h"

using namespace testing;
using namespace testing::ext;

namespace OHOS {
namespace Rosen {
using Mocker = SingletonMocker<DisplayManagerAdapter, MockDisplayManagerAdapter>;
class DisplayTest : public testing::Test {
public:
    static void SetUpTestCase();
    static void TearDownTestCase();
    virtual void SetUp() override;
    virtual void TearDown() override;

    static sptr<Display> defaultDisplay_;
    static DisplayId defaultDisplayId_;
};
sptr<Display> DisplayTest::defaultDisplay_ = nullptr;
DisplayId DisplayTest::defaultDisplayId_ = DISPLAY_ID_INVALID;

void DisplayTest::SetUpTestCase()
{
    defaultDisplay_ = DisplayManager::GetInstance().GetDefaultDisplay();
    defaultDisplayId_ = static_cast<DisplayId>(defaultDisplay_->GetId());
}

void DisplayTest::TearDownTestCase()
{
    defaultDisplay_ = nullptr;
}

void DisplayTest::SetUp()
{
}

void DisplayTest::TearDown()
{
}

namespace {
/**
 * @tc.name: GetCutoutInfo01
 * @tc.desc: GetCutoutInfo with valid defaultDisplayId and return success
 * @tc.type: FUNC
 * @tc.require: issueI5K0JP
 */
HWTEST_F(DisplayTest, GetCutoutInfo01, Function | SmallTest | Level1)
{
    auto cutoutInfo = defaultDisplay_->GetCutoutInfo();
    ASSERT_NE(nullptr, cutoutInfo);
}

/**
 * @tc.name: UpdateDisplayInfo01
 * @tc.desc: UpdateDisplayInfo with nullptr
 * @tc.type: FUNC
 * @tc.require: issueI5K0JP
 */
HWTEST_F(DisplayTest, UpdateDisplayInfo01, Function | SmallTest | Level1)
{
    defaultDisplay_->UpdateDisplayInfo(nullptr);
}

/**
 * @tc.name: GetName
 * @tc.desc: UpdateDisplayInfo with nullptr
 * @tc.type: FUNC
 * @tc.require: issueI5K0JP
 */
HWTEST_F(DisplayTest, GetName, Function | SmallTest | Level1)
{
    defaultDisplay_->GetName();
    defaultDisplay_->GetDpi();
}

/**
 * @tc.name: SetWaterfallCompression01
 * @tc.desc: Set waterfall compression related values with valid input.
 * @tc.type: FUNC
 * @tc.require: issueI5P8CI
 */
HWTEST_F(DisplayTest, SetWaterfallCompression01, Function | SmallTest | Level1)
{
    bool isWaterfallDisplayOrigin = DisplayCutoutController::IsWaterfallDisplay();
    DisplayCutoutController::SetIsWaterfallDisplay(true);
    bool isCompressionEnableOrigin =
        DisplayCutoutController::IsWaterfallAreaCompressionEnableWhenHorizontal();
    DisplayCutoutController::SetWaterfallAreaCompressionEnableWhenHorzontal(true);
    uint32_t testSizeOrigin = DisplayCutoutController::GetWaterfallAreaCompressionSizeWhenHorizontal();
    uint32_t testSize = 20;
    DisplayCutoutController::SetWaterfallAreaCompressionSizeWhenHorizontal(testSize);
    ASSERT_EQ(true, DisplayCutoutController::IsWaterfallDisplay());
    ASSERT_EQ(true, DisplayCutoutController::IsWaterfallAreaCompressionEnableWhenHorizontal());
    ASSERT_EQ(testSize, DisplayCutoutController::GetWaterfallAreaCompressionSizeWhenHorizontal());
    DisplayCutoutController::SetWaterfallAreaCompressionSizeWhenHorizontal(testSizeOrigin);
    ASSERT_EQ(testSizeOrigin, DisplayCutoutController::GetWaterfallAreaCompressionSizeWhenHorizontal());
    DisplayCutoutController::SetWaterfallAreaCompressionEnableWhenHorzontal(isCompressionEnableOrigin);
    ASSERT_EQ(isWaterfallDisplayOrigin, DisplayCutoutController::IsWaterfallAreaCompressionEnableWhenHorizontal());
    DisplayCutoutController::SetIsWaterfallDisplay(isWaterfallDisplayOrigin);
    ASSERT_EQ(isWaterfallDisplayOrigin, DisplayCutoutController::IsWaterfallDisplay());
}

/**
 * @tc.name: SetWaterfallCompression02
 * @tc.desc: Set waterfall compression related values with invalid input.
 * @tc.type: FUNC
 * @tc.require: issueI5P8CI
 */
HWTEST_F(DisplayTest, SetWaterfallCompression02, Function | SmallTest | Level1)
{
    bool isWaterfallDisplayOrigin = DisplayCutoutController::IsWaterfallDisplay();
    DisplayCutoutController::SetIsWaterfallDisplay(true);
    bool isCompressionEnableOrigin =
        DisplayCutoutController::IsWaterfallAreaCompressionEnableWhenHorizontal();
    DisplayCutoutController::SetWaterfallAreaCompressionEnableWhenHorzontal(true);
    uint32_t testSizeOrigin = DisplayCutoutController::GetWaterfallAreaCompressionSizeWhenHorizontal();

    DisplayCutoutController::SetIsWaterfallDisplay(false);
    DisplayCutoutController::SetWaterfallAreaCompressionEnableWhenHorzontal(true);
    ASSERT_EQ(false, DisplayCutoutController::IsWaterfallAreaCompressionEnableWhenHorizontal());

    uint32_t testSize = 20;
    DisplayCutoutController::SetIsWaterfallDisplay(true);
    DisplayCutoutController::SetWaterfallAreaCompressionEnableWhenHorzontal(false);
    DisplayCutoutController::SetWaterfallAreaCompressionSizeWhenHorizontal(testSize);
    ASSERT_EQ(0, DisplayCutoutController::GetWaterfallAreaCompressionSizeWhenHorizontal());

    DisplayCutoutController::SetIsWaterfallDisplay(false);
    DisplayCutoutController::SetWaterfallAreaCompressionEnableWhenHorzontal(false);
    DisplayCutoutController::SetWaterfallAreaCompressionSizeWhenHorizontal(testSize);
    ASSERT_EQ(0, DisplayCutoutController::GetWaterfallAreaCompressionSizeWhenHorizontal());

    DisplayCutoutController::SetWaterfallAreaCompressionSizeWhenHorizontal(testSizeOrigin);
    ASSERT_EQ(testSizeOrigin, DisplayCutoutController::GetWaterfallAreaCompressionSizeWhenHorizontal());
    DisplayCutoutController::SetWaterfallAreaCompressionEnableWhenHorzontal(isCompressionEnableOrigin);
    ASSERT_EQ(isWaterfallDisplayOrigin, DisplayCutoutController::IsWaterfallAreaCompressionEnableWhenHorizontal());
    DisplayCutoutController::SetIsWaterfallDisplay(isWaterfallDisplayOrigin);
    ASSERT_EQ(isWaterfallDisplayOrigin, DisplayCutoutController::IsWaterfallDisplay());
}
/**
 * @tc.name: GetName01
 * @tc.desc: GetName function cover
 * @tc.type: FUNC
 */
HWTEST_F(DisplayTest, GetName01, Function | SmallTest | Level1)
{
    auto name = defaultDisplay_->GetName();
    ASSERT_EQ("display_0", name);
}

/**
 * @tc.name: GetDpi01
 * @tc.desc: GetDpi function cover
 * @tc.type: FUNC
 */
HWTEST_F(DisplayTest, GetDpi01, Function | SmallTest | Level1)
{
    auto dpi = defaultDisplay_->GetDpi();

    auto vpr = defaultDisplay_->GetVirtualPixelRatio();
    ASSERT_EQ(vpr * DOT_PER_INCH, dpi);
}
}
} // namespace Rosen
} // namespace OHOS