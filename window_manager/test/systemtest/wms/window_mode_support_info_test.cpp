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

// gtest
#include <gtest/gtest.h>
#include "window_manager.h"
#include "window_test_utils.h"
#include "wm_common.h"
using namespace testing;
using namespace testing::ext;

namespace OHOS {
namespace Rosen {
using Utils = WindowTestUtils;
class WindowModeSupportInfoTest : public testing::Test {
public:
    static void SetUpTestCase();
    static void TearDownTestCase();
    virtual void SetUp() override;
    virtual void TearDown() override;
    Utils::TestWindowInfo fullAppInfo_1_;
    Utils::TestWindowInfo fullAppInfo_2_;
private:
    static constexpr uint32_t WAIT_SYANC_US = 100000;
};

void WindowModeSupportInfoTest::SetUpTestCase()
{
    auto display = DisplayManager::GetInstance().GetDisplayById(0);
    ASSERT_TRUE((display != nullptr));
    Rect displayRect = {0, 0, display->GetWidth(), display->GetHeight()};
    Utils::InitByDisplayRect(displayRect);
}

void WindowModeSupportInfoTest::TearDownTestCase()
{
}

void WindowModeSupportInfoTest::SetUp()
{
    fullAppInfo_1_ = {
            .name = "FullWindow",
            .rect = Utils::customAppRect_,
            .type = WindowType::WINDOW_TYPE_APP_MAIN_WINDOW,
            .mode = WindowMode::WINDOW_MODE_FULLSCREEN,
            .needAvoid = false,
            .parentLimit = false,
            .parentId = INVALID_WINDOW_ID,
    };
    fullAppInfo_2_ = {
            .name = "FullWindow2",
            .rect = Utils::customAppRect_,
            .type = WindowType::WINDOW_TYPE_APP_MAIN_WINDOW,
            .mode = WindowMode::WINDOW_MODE_FULLSCREEN,
            .needAvoid = false,
            .parentLimit = false,
            .parentId = INVALID_WINDOW_ID,
    };
}

void WindowModeSupportInfoTest::TearDown()
{
}

namespace {
/**
 * @tc.name: WindowModeSupportInfo01
 * @tc.desc: SetRequestModeSupportInfo | GetRequestModeSupportInfo
 * @tc.type: FUNC
 */
HWTEST_F(WindowModeSupportInfoTest, WindowModeSupportInfo01, Function | MediumTest | Level3)
{
    const sptr<Window>& window = Utils::CreateTestWindow(fullAppInfo_1_);

    window->SetRequestModeSupportInfo(WindowModeSupport::WINDOW_MODE_SUPPORT_FULLSCREEN);
    ASSERT_EQ(WindowModeSupport::WINDOW_MODE_SUPPORT_FULLSCREEN, window->GetRequestModeSupportInfo());
    window->Destroy();
}

/**
 * @tc.name: WindowModeSupportInfo02
 * @tc.desc: modeSupportInfo test for single window, only support fullScreen mode
 * @tc.type: FUNC
 */
HWTEST_F(WindowModeSupportInfoTest, WindowModeSupportInfo02, Function | MediumTest | Level3)
{
    const sptr<Window>& window = Utils::CreateTestWindow(fullAppInfo_1_);

    window->SetRequestModeSupportInfo(WindowModeSupport::WINDOW_MODE_SUPPORT_FULLSCREEN);
    ASSERT_EQ(WMError::WM_OK, window->Show());
    ASSERT_EQ(WindowMode::WINDOW_MODE_FULLSCREEN, window->GetMode());

    window->SetWindowMode(WindowMode::WINDOW_MODE_FLOATING);
    ASSERT_EQ(WindowMode::WINDOW_MODE_FULLSCREEN, window->GetMode());

    window->SetWindowMode(WindowMode::WINDOW_MODE_SPLIT_PRIMARY);
    ASSERT_EQ(WindowMode::WINDOW_MODE_FULLSCREEN, window->GetMode());

    window->SetWindowMode(WindowMode::WINDOW_MODE_SPLIT_SECONDARY);
    ASSERT_EQ(WindowMode::WINDOW_MODE_FULLSCREEN, window->GetMode());

    ASSERT_EQ(WMError::WM_OK, window->Hide());
    window->Destroy();
}

/**
 * @tc.name: WindowModeSupportInfo03
 * @tc.desc: modeSupportInfo test for single window, support both fullScreen and floating mode
 * @tc.type: FUNC
 */
HWTEST_F(WindowModeSupportInfoTest, WindowModeSupportInfo03, Function | MediumTest | Level3)
{
    const sptr<Window>& window = Utils::CreateTestWindow(fullAppInfo_1_);

    window->SetRequestModeSupportInfo(WindowModeSupport::WINDOW_MODE_SUPPORT_FULLSCREEN |
        WindowModeSupport::WINDOW_MODE_SUPPORT_FLOATING);
    ASSERT_EQ(WMError::WM_OK, window->Show());
    ASSERT_EQ(WindowMode::WINDOW_MODE_FULLSCREEN, window->GetMode());

    window->SetWindowMode(WindowMode::WINDOW_MODE_FLOATING);
    ASSERT_EQ(WindowMode::WINDOW_MODE_FLOATING, window->GetMode());

    window->SetWindowMode(WindowMode::WINDOW_MODE_FULLSCREEN);
    ASSERT_EQ(WindowMode::WINDOW_MODE_FULLSCREEN, window->GetMode());

    window->SetWindowMode(WindowMode::WINDOW_MODE_SPLIT_PRIMARY);
    ASSERT_EQ(WindowMode::WINDOW_MODE_FULLSCREEN, window->GetMode());

    window->SetWindowMode(WindowMode::WINDOW_MODE_SPLIT_SECONDARY);
    ASSERT_EQ(WindowMode::WINDOW_MODE_FULLSCREEN, window->GetMode());

    ASSERT_EQ(WMError::WM_OK, window->Hide());
    window->Destroy();
}

/**
 * @tc.name: WindowModeSupportInfo04
 * @tc.desc: modeSupportInfo test for single window, window mode is not supported when show, show failed
 * @tc.type: FUNC
 */
HWTEST_F(WindowModeSupportInfoTest, WindowModeSupportInfo04, Function | MediumTest | Level3)
{
    const sptr<Window>& window = Utils::CreateTestWindow(fullAppInfo_1_);
    window->SetRequestModeSupportInfo(WindowModeSupport::WINDOW_MODE_SUPPORT_FLOATING |
    WindowModeSupport::WINDOW_MODE_SUPPORT_SPLIT_PRIMARY |
    WindowModeSupport::WINDOW_MODE_SUPPORT_SPLIT_SECONDARY);
    ASSERT_NE(WMError::WM_OK, window->Show());
    ASSERT_EQ(WMError::WM_OK, window->Hide());
    window->Destroy();
}

/**
 * @tc.name: WindowModeSupportInfo05
 * @tc.desc: modeSupportInfo test for layout cascade
 * @tc.type: FUNC
 */
HWTEST_F(WindowModeSupportInfoTest, WindowModeSupportInfo05, Function | MediumTest | Level3)
{
    const sptr<Window>& window1 = Utils::CreateTestWindow(fullAppInfo_1_);
    window1->SetRequestModeSupportInfo(WindowModeSupport::WINDOW_MODE_SUPPORT_FULLSCREEN);
    const sptr<Window>& window2 = Utils::CreateTestWindow(fullAppInfo_2_);
    window2->SetRequestModeSupportInfo(WindowModeSupport::WINDOW_MODE_SUPPORT_ALL);
    ASSERT_EQ(WMError::WM_OK, window1->Show());
    ASSERT_EQ(WMError::WM_OK, window2->Show());
    WindowManager::GetInstance().SetWindowLayoutMode(WindowLayoutMode::CASCADE);
    usleep(WAIT_SYANC_US);

    ASSERT_EQ(WindowMode::WINDOW_MODE_FULLSCREEN, window1->GetMode());
    ASSERT_EQ(WindowMode::WINDOW_MODE_FLOATING, window2->GetMode());

    window1->Destroy();
    window2->Destroy();
}

/**
 * @tc.name: WindowModeSupportInfo06
 * @tc.desc: modeSupportInfo test for layout tile
 * @tc.type: FUNC
 */
HWTEST_F(WindowModeSupportInfoTest, WindowModeSupportInfo06, Function | MediumTest | Level3)
{
    const sptr<Window>& window = Utils::CreateTestWindow(fullAppInfo_1_);
    window->SetRequestModeSupportInfo(WindowModeSupport::WINDOW_MODE_SUPPORT_FULLSCREEN);
    ASSERT_EQ(WMError::WM_OK, window->Show());
    WindowManager::GetInstance().SetWindowLayoutMode(WindowLayoutMode::TILE);
    usleep(WAIT_SYANC_US);

    ASSERT_EQ(WindowMode::WINDOW_MODE_FULLSCREEN, window->GetMode());

    window->Destroy();
    WindowManager::GetInstance().SetWindowLayoutMode(WindowLayoutMode::CASCADE);
    usleep(WAIT_SYANC_US);
}
} // namespace
} // namespace Rosen
} // namespace OHOS