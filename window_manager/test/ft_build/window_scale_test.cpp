/*
 * Copyright (c) 2023 Huawei Technologies Co., Ltd.
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
#include "window.h"
#include "window_option.h"
#include "wm_common.h"

namespace OHOS {
namespace Rosen {
class WindowScaleTest : public testing::Test {
public:
    virtual void SetUp() override;
    virtual void TearDown() override;
};

void WindowScaleTest::SetUp()
{
}

void WindowScaleTest::TearDown()
{
}

static sptr<Window> CreateWindow(WindowMode mode)
{
    sptr<WindowOption> mainOption(new WindowOption());
    mainOption->SetWindowType(WindowType::WINDOW_TYPE_APP_MAIN_WINDOW);
    mainOption->SetWindowMode(mode);
    if (mode != WindowMode::WINDOW_MODE_FULLSCREEN) {
        mainOption->SetWindowRect({0, 0, 600, 600});
    }
    return Window::Create("window", mainOption);
}

class WindowChangeListener : public IWindowChangeListener {
public:
    void OnSizeChange(Rect rect, WindowSizeChangeReason reason) override {
        // real Rect depends on hardware platform, thus can not use ASSERT_EQ
        ASSERT_LE(rect.width_, 400);
        ASSERT_LE(rect.height_, 400);
    }
};

/**
 * @tc.name: Resize01
 * @tc.desc: Floating Window
 * @tc.type: FUNC
 */
TEST_F(WindowScaleTest, Resize01)
{
    sptr<Window> window = CreateWindow(WindowMode::WINDOW_MODE_FLOATING);
    ASSERT_EQ(WMError::WM_OK, window->Show());

    sptr<IWindowChangeListener> listener = new WindowChangeListener();
    ASSERT_EQ(true, window->RegisterWindowChangeListener(listener));

    ASSERT_EQ(WMError::WM_OK, window->Resize(300, 300));
    sleep(1);

    ASSERT_EQ(WMError::WM_OK, window->Hide());
    sleep(1);

    window->Destroy();
}

/**
 * @tc.name: Resize02
 * @tc.desc: Fullscreen Window
 * @tc.type: FUNC
 */
TEST_F(WindowScaleTest, Resize02)
{
    sptr<Window> window = CreateWindow(WindowMode::WINDOW_MODE_FULLSCREEN);
    ASSERT_EQ(WMError::WM_OK, window->Show());

    ASSERT_NE(WMError::WM_OK, window->Resize(300, 300));

    ASSERT_EQ(WMError::WM_OK, window->Hide());
    sleep(1);

    window->Destroy();
}
} // namespace Rosen
} // namespace OHOS

int main(int argc, char *argv[]) {
    testing::InitGoogleTest(&argc, argv);
    return RUN_ALL_TESTS();
}
