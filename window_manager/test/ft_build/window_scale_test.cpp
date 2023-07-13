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

static sptr<Window> CreateMainWindow()
{
    sptr<WindowOption> mainOption(new WindowOption());
    mainOption->SetWindowRect({0, 0, 100, 100});
    mainOption->SetWindowType(WindowType::WINDOW_TYPE_APP_MAIN_WINDOW);
    mainOption->SetWindowMode(WindowMode::WINDOW_MODE_FLOATING);
    return Window::Create("main_window", mainOption);
}

/**
 * @tc.name: Resize
 * @tc.desc: Window Resize
 * @tc.type: FUNC
 */
TEST_F(WindowScaleTest, Resize)
{
    sptr<Window> window = CreateMainWindow();
    ASSERT_EQ(WMError::WM_OK, window->Show());

    ASSERT_EQ(WMError::WM_OK, window->Resize(300, 300));
    ASSERT_EQ(300, window->GetRequestRect().width_);
    ASSERT_EQ(300, window->GetRequestRect().height_);

    ASSERT_EQ(WMError::WM_OK, window->Resize(30, 30));
    ASSERT_EQ(30, window->GetRequestRect().width_);
    ASSERT_EQ(30, window->GetRequestRect().height_);

    ASSERT_EQ(WMError::WM_OK, window->Hide());
    sleep(1);

    window->Destroy();
}

/**
 * @tc.name: Transform
 * @tc.desc: Window Transform
 * @tc.type: FUNC
 */
TEST_F(WindowScaleTest, Transform)
{
    sptr<Window> window = CreateMainWindow();
    ASSERT_EQ(WMError::WM_OK, window->Show());

    Transform trans;
    trans.scaleX_ = 0.5f;
    trans.scaleY_ = 0.5f;
    window->SetTransform(trans);
    ASSERT_EQ(0.5, window->GetTransform().scaleX_);
    ASSERT_EQ(0.5, window->GetTransform().scaleY_);

    trans.scaleX_ = 2.0f;
    trans.scaleY_ = 2.0f;
    window->SetTransform(trans);
    ASSERT_EQ(2.0, window->GetTransform().scaleX_);
    ASSERT_EQ(2.0, window->GetTransform().scaleY_);

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
