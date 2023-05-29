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
class WindowInputMethodTest : public testing::Test {
public:
    static void SetUpTestCase();
    static void TearDownTestCase();
    virtual void SetUp() override;
    virtual void TearDown() override;
    Utils::TestWindowInfo inputMethodWindowInfo_;
    Utils::TestWindowInfo keyGuardWindowInfo_;
};

void WindowInputMethodTest::SetUpTestCase()
{
    auto display = DisplayManager::GetInstance().GetDisplayById(0);
    ASSERT_TRUE((display != nullptr));
    Rect displayRect = {0, 0, display->GetWidth(), display->GetHeight()};
    Utils::InitByDisplayRect(displayRect);
}

void WindowInputMethodTest::TearDownTestCase()
{
}

void WindowInputMethodTest::SetUp()
{
    inputMethodWindowInfo_ = {
        .name = "",
        .rect = Utils::customAppRect_,
        .type = WindowType::WINDOW_TYPE_INPUT_METHOD_FLOAT,
        .mode = WindowMode::WINDOW_MODE_FLOATING,
        .needAvoid = false,
        .parentLimit = false,
        .parentId = INVALID_WINDOW_ID,
    };
    keyGuardWindowInfo_ = {
        .name = "",
        .rect = Utils::customAppRect_,
        .type = WindowType::WINDOW_TYPE_KEYGUARD,
        .mode = WindowMode::WINDOW_MODE_FULLSCREEN,
        .needAvoid = false,
        .parentLimit = false,
        .parentId = INVALID_WINDOW_ID,
    };
}

void WindowInputMethodTest::TearDown()
{
}

namespace {
/**
 * @tc.name: InputMethodWindow01
 * @tc.desc: One InputMethod Floating Window
 * @tc.type: FUNC
 */
HWTEST_F(WindowInputMethodTest, InputMethodWindow01, Function | MediumTest | Level3)
{
    inputMethodWindowInfo_.name = "input_method.1";
    const sptr<Window>& window = Utils::CreateTestWindow(inputMethodWindowInfo_);
    ASSERT_EQ(WindowType::WINDOW_TYPE_INPUT_METHOD_FLOAT, window->GetType());
    ASSERT_EQ(WMError::WM_OK, window->Show());
    ASSERT_EQ(WMError::WM_OK, window->Hide());
}

/**
 * @tc.name: InputMethodWindow02
 * @tc.desc: One KeyGuard Window
 * @tc.type: FUNC
 */
HWTEST_F(WindowInputMethodTest, InputMethodWindow02, Function | MediumTest | Level3)
{
    keyGuardWindowInfo_.name  = "keyGuard.1";
    const sptr<Window>& window = Utils::CreateTestWindow(keyGuardWindowInfo_);
    ASSERT_EQ(WindowType::WINDOW_TYPE_KEYGUARD, window->GetType());
    ASSERT_EQ(WMError::WM_OK, window->Show());
    ASSERT_EQ(WMError::WM_OK, window->Hide());
}

/**
 * @tc.name: InputMethodWindow03
 * @tc.desc: One InputMethod Floating Window & One KeyGuard Window
 * @tc.type: FUNC
 */
HWTEST_F(WindowInputMethodTest, InputMethodWindow03, Function | MediumTest | Level3)
{
    inputMethodWindowInfo_.name = "input_method.2";
    keyGuardWindowInfo_.name  = "keyGuard.2";
    const sptr<Window>& inputMethodWindow = Utils::CreateTestWindow(inputMethodWindowInfo_);
    const sptr<Window>& keyGuardWindow = Utils::CreateTestWindow(keyGuardWindowInfo_);
    keyGuardWindow->Show();
    inputMethodWindow->Show();
    ASSERT_TRUE(Utils::RectEqualTo(keyGuardWindow, Utils::displayRect_));
    ASSERT_EQ(inputMethodWindow->GetRect().width_,  Utils::customAppRect_.width_);
    ASSERT_EQ(inputMethodWindow->GetRect().height_,  Utils::customAppRect_.height_);
}
} // namespace
} // namespace Rosen
} // namespace OHOS
