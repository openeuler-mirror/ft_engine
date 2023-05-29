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

// gtest
#include <gtest/gtest.h>
#include "window_test_utils.h"
#include "wm_common.h"
using namespace testing;
using namespace testing::ext;

namespace OHOS {
namespace Rosen {
using Utils = WindowTestUtils;
constexpr uint32_t MAX_WAIT_COUNT = 100;
constexpr uint32_t WAIT_DUR = 10 * 1000;

class WindowGamutTest : public testing::Test {
public:
    static void SetUpTestCase();
    static void TearDownTestCase();
    virtual void SetUp() override;
    virtual void TearDown() override;
    Utils::TestWindowInfo fullScreenAppInfo_;
};

void WindowGamutTest::SetUpTestCase()
{
}

void WindowGamutTest::TearDownTestCase()
{
}

void WindowGamutTest::SetUp()
{
    fullScreenAppInfo_ = {
            .name = "FullWindow",
            .rect = Utils::customAppRect_,
            .type = WindowType::WINDOW_TYPE_APP_MAIN_WINDOW,
            .mode = WindowMode::WINDOW_MODE_FULLSCREEN,
            .needAvoid = false,
            .parentLimit = false,
            .parentId = INVALID_WINDOW_ID,
    };
}

void WindowGamutTest::TearDown()
{
}

namespace {
/**
 * @tc.name: IsSupportWideGamut01
 * @tc.desc: IsSupportWideGamut
 * @tc.type: FUNC
 */
HWTEST_F(WindowGamutTest, IsSupportWideGamut01, Function | MediumTest | Level3)
{
    const sptr<Window>& window = Utils::CreateTestWindow(fullScreenAppInfo_);

    ASSERT_EQ(true, window->IsSupportWideGamut());

    window->Destroy();
}

/**
 * @tc.name: GetColorSpace01
 * @tc.desc: Get ColorSpace
 * @tc.type: FUNC
 */
HWTEST_F(WindowGamutTest, GetColorSpace01, Function | MediumTest | Level3)
{
    const sptr<Window>& window = Utils::CreateTestWindow(fullScreenAppInfo_);

    ASSERT_EQ(ColorSpace::COLOR_SPACE_DEFAULT, window->GetColorSpace());

    window->Destroy();
}

/**
 * @tc.name: SetColorSpace01
 * @tc.desc: Set ColorSpace, valid param
 * @tc.type: FUNC
 */
HWTEST_F(WindowGamutTest, SetColorSpace01, Function | MediumTest | Level3)
{
    uint32_t i, j;
    const ColorSpace colorSpacesToTest[] = {
        ColorSpace::COLOR_SPACE_DEFAULT,
        ColorSpace::COLOR_SPACE_WIDE_GAMUT
    };
    ColorSpace colorSpace;
    const sptr<Window>& window = Utils::CreateTestWindow(fullScreenAppInfo_);

    ColorSpace colorSpaceBackup = window->GetColorSpace(); // backup origin

    for (j = 0; j < sizeof(colorSpacesToTest) / sizeof(ColorSpace); j++) {
        window->SetColorSpace(colorSpacesToTest[j]); // async func
        for (i = 0; i < MAX_WAIT_COUNT; i++) { // wait some time for async set ok
            colorSpace = window->GetColorSpace();
            if (colorSpace != colorSpacesToTest[j]) {
                usleep(WAIT_DUR);
            } else {
                break;
            }
        }
        ASSERT_EQ(colorSpacesToTest[j], window->GetColorSpace());
    }

    window->SetColorSpace(colorSpaceBackup); // restore

    window->Destroy();
}

/**
 * @tc.name: SetColorSpace02
 * @tc.desc: Set ColorSpace, invalid param
 * @tc.type: FUNC
 */
HWTEST_F(WindowGamutTest, SetColorSpace02, Function | MediumTest | Level3)
{
    const sptr<Window>& window = Utils::CreateTestWindow(fullScreenAppInfo_);

    ColorSpace colorSpaceBackup = window->GetColorSpace();

    ColorSpace invalidColorSpace =
        static_cast<ColorSpace>(static_cast<uint32_t>(ColorSpace::COLOR_SPACE_WIDE_GAMUT) + 1);
    window->SetColorSpace(invalidColorSpace);  // invalid param

    ASSERT_EQ(colorSpaceBackup, window->GetColorSpace());

    window->Destroy();
}
} // namespace
} // namespace Rosen
} // namespace OHOS
