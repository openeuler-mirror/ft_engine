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
class WindowMoveTest : public testing::Test {
public:
    virtual void SetUp() override;
    virtual void TearDown() override;
};

void WindowMoveTest::SetUp()
{
}

void WindowMoveTest::TearDown()
{
}

static sptr<Window> CreateMainWindow()
{
    sptr<WindowOption> mainOption(new WindowOption());
    mainOption->SetWindowRect({0, 0, 600, 600});
    mainOption->SetWindowType(WindowType::WINDOW_TYPE_APP_MAIN_WINDOW);
    mainOption->SetWindowMode(WindowMode::WINDOW_MODE_FLOATING);
    return Window::Create("main_window", mainOption);
}

/**
 * @tc.name: Move
 * @tc.desc: Window Move
 * @tc.type: FUNC
 */
TEST_F(WindowMoveTest, Move)
{
    sptr<Window> window = CreateMainWindow();
    ASSERT_EQ(WMError::WM_OK, window->Show());

    ASSERT_EQ(WMError::WM_OK, window->MoveTo(100, 100));
    ASSERT_EQ(100, window->GetRequestRect().posX_);
    ASSERT_EQ(100, window->GetRequestRect().posY_);

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
