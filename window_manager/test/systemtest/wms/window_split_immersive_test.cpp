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
#include "avoid_area_controller.h"
#include "window_manager.h"
#include "window_test_utils.h"
#include "wm_common.h"
using namespace testing;
using namespace testing::ext;

namespace OHOS {
namespace Rosen {
using Utils = WindowTestUtils;

class WindowSplitImmersiveTest : public testing::Test {
public:
    static void SetUpTestCase();
    static void TearDownTestCase();
    virtual void SetUp() override;
    virtual void TearDown() override;
    void IsShowTopBar(const sptr<Window>& top, bool isShow);
    void HideAndUnregister(const sptr<Window>& fullWindow, const sptr<Window>& priWindow, const sptr<Window>& top);

    std::vector<sptr<Window>> activeWindows_;
    Utils::TestWindowInfo fullInfo_;
    Utils::TestWindowInfo splitInfo_;

private:
    static constexpr uint32_t SPLIT_TEST_SLEEP_S = 1; // split test sleep time
};

void WindowSplitImmersiveTest::SetUpTestCase()
{
}

void WindowSplitImmersiveTest::TearDownTestCase()
{
}

void WindowSplitImmersiveTest::SetUp()
{
    fullInfo_ = {
        .name = "fullscreen.1",
        .rect = Utils::customAppRect_,
        .type = WindowType::WINDOW_TYPE_APP_MAIN_WINDOW,
        .mode = WindowMode::WINDOW_MODE_FULLSCREEN,
        .needAvoid = true,
        .parentLimit = false,
        .parentId = INVALID_WINDOW_ID,
    };

    splitInfo_ = {
        .name = "primary.1",
        .rect = Utils::customAppRect_,
        .type = WindowType::WINDOW_TYPE_APP_MAIN_WINDOW,
        .mode = WindowMode::WINDOW_MODE_FULLSCREEN,
        .needAvoid = true,
        .parentLimit = false,
        .parentId = INVALID_WINDOW_ID,
    };

    activeWindows_.clear();
}

void WindowSplitImmersiveTest::TearDown()
{
    while (!activeWindows_.empty()) {
        ASSERT_EQ(WMError::WM_OK, activeWindows_.back()->Destroy());
        activeWindows_.pop_back();
    }
}

namespace {
/**
 * @tc.name: SplitImmersive01
 * @tc.desc: one primary window and one fullscreen window, test enter and out split immersive
 * @tc.type: FUNC
 */
HWTEST_F(WindowSplitImmersiveTest, SplitImmersive01, Function | MediumTest | Level3)
{
    // create fullscreen win and show
    fullInfo_.mode = WindowMode::WINDOW_MODE_SPLIT_SECONDARY;
    const sptr<Window>& fullWindow = Utils::CreateTestWindow(fullInfo_);
    activeWindows_.push_back(fullWindow);
    ASSERT_EQ(WMError::WM_OK, fullWindow->Show());
    sleep(SPLIT_TEST_SLEEP_S);

    // enter split mode
    splitInfo_.mode = WindowMode::WINDOW_MODE_SPLIT_PRIMARY;
    const sptr<Window>& priWindow = Utils::CreateTestWindow(splitInfo_);
    activeWindows_.push_back(priWindow);
    ASSERT_EQ(WMError::WM_OK, priWindow->Show());
    sleep(SPLIT_TEST_SLEEP_S);

    // check is enter split Immersive
    ASSERT_EQ(WindowMode::WINDOW_MODE_SPLIT_PRIMARY, priWindow->GetMode());
    ASSERT_EQ(WindowMode::WINDOW_MODE_SPLIT_SECONDARY, fullWindow->GetMode());
    Rect immersivePriRect = priWindow->GetRect();
    ASSERT_EQ(0, immersivePriRect.posX_);
    ASSERT_EQ(0, immersivePriRect.posY_);
    sleep(SPLIT_TEST_SLEEP_S);

    ASSERT_EQ(WMError::WM_OK, fullWindow->Hide());
    sleep(SPLIT_TEST_SLEEP_S);
    ASSERT_EQ(WMError::WM_OK, priWindow->Hide());
    sleep(SPLIT_TEST_SLEEP_S);
}
}
} // namespace Rosen
} // namespace OHOS
