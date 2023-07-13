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

/**
 * @tc.name: Move01
 * @tc.desc: Floating Window
 * @tc.type: FUNC
 */
TEST_F(WindowMoveTest, Move01)
{
    sptr<Window> window = CreateWindow(WindowMode::WINDOW_MODE_FLOATING);
    ASSERT_EQ(WMError::WM_OK, window->Show());

    ASSERT_EQ(WMError::WM_OK, window->MoveTo(100, 100));
    ASSERT_EQ(100, window->GetRequestRect().posX_);
    ASSERT_EQ(100, window->GetRequestRect().posY_);

    ASSERT_EQ(WMError::WM_OK, window->Hide());
    sleep(1);

    window->Destroy();
}

/**
 * @tc.name: Move02
 * @tc.desc: Fullscreen Window
 * @tc.type: FUNC
 */
TEST_F(WindowMoveTest, Move02)
{
    sptr<Window> window = CreateWindow(WindowMode::WINDOW_MODE_FULLSCREEN);
    ASSERT_EQ(WMError::WM_OK, window->Show());

    ASSERT_NE(WMError::WM_OK, window->MoveTo(100, 100));

    ASSERT_EQ(WMError::WM_OK, window->Hide());
    sleep(1);

    window->Destroy();
}

/**
 * @tc.name: Move03
 * @tc.desc: Undefined Window
 * @tc.type: FUNC
 */
TEST_F(WindowMoveTest, Move03)
{
    sptr<Window> window = CreateWindow(WindowMode::WINDOW_MODE_UNDEFINED);
    ASSERT_EQ(WMError::WM_OK, window->Show());

    ASSERT_NE(WMError::WM_OK, window->MoveTo(100, 100));

    ASSERT_EQ(WMError::WM_OK, window->Hide());
    sleep(1);

    window->Destroy();
}

/**
 * @tc.name: Move04
 * @tc.desc: Pip Window
 * @tc.type: FUNC
 */
TEST_F(WindowMoveTest, Move04)
{
    sptr<Window> window = CreateWindow(WindowMode::WINDOW_MODE_PIP);
    ASSERT_EQ(WMError::WM_OK, window->Show());

    ASSERT_NE(WMError::WM_OK, window->MoveTo(100, 100));

    ASSERT_EQ(WMError::WM_OK, window->Hide());
    sleep(1);

    window->Destroy();
}

/**
 * @tc.name: Move05
 * @tc.desc: Split Primary Window
 * @tc.type: FUNC
 */
TEST_F(WindowMoveTest, Move05)
{
    sptr<Window> window = CreateWindow(WindowMode::WINDOW_MODE_SPLIT_PRIMARY);
    ASSERT_EQ(WMError::WM_OK, window->Show());

    ASSERT_NE(WMError::WM_OK, window->MoveTo(100, 100));

    ASSERT_EQ(WMError::WM_OK, window->Hide());
    sleep(1);

    window->Destroy();
}

/**
 * @tc.name: Move06
 * @tc.desc: Split Secondary Window
 * @tc.type: FUNC
 */
TEST_F(WindowMoveTest, Move06)
{
    sptr<Window> window = CreateWindow(WindowMode::WINDOW_MODE_SPLIT_SECONDARY);
    ASSERT_EQ(WMError::WM_OK, window->Show());

    ASSERT_NE(WMError::WM_OK, window->MoveTo(100, 100));

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
