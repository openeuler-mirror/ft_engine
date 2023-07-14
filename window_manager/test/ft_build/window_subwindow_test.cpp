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
class WindowSubWindowTest : public testing::Test {
public:
    virtual void SetUp() override;
    virtual void TearDown() override;
};

void WindowSubWindowTest::SetUp()
{
}

void WindowSubWindowTest::TearDown()
{
}

static sptr<Window> CreateMainWindow()
{
    sptr<WindowOption> mainOption(new WindowOption());
    mainOption->SetWindowType(WindowType::WINDOW_TYPE_APP_MAIN_WINDOW);
    mainOption->SetWindowMode(WindowMode::WINDOW_MODE_FULLSCREEN);
    return Window::Create("main_window", mainOption);
}

static sptr<Window> CreateSubWindow(sptr<Window> mainWindow, WindowType type,
    struct Rect rect, std::string name = "")
{
    sptr<WindowOption> subOp = new WindowOption();
    subOp->SetWindowType(type);
    subOp->SetWindowMode(WindowMode::WINDOW_MODE_FLOATING);
    subOp->SetWindowRect(rect);
    subOp->SetParentId(mainWindow->GetWindowId());

    static int cnt = 0;
    std::string subWinName = (name == "") ? "SubWindow" + std::to_string(cnt++) : name;
    return Window::Create(subWinName, subOp);
}

/**
 * @tc.name: SubWindow01
 * @tc.desc: FullScreen Main Window + Floating SubWindow
 * @tc.type: FUNC
 */
TEST_F(WindowSubWindowTest, SubWindow01)
{
    sptr<Window> window = CreateMainWindow();

    struct Rect rect = {0, 0, 100, 200};
    sptr<Window> subWindow = CreateSubWindow(window, WindowType::WINDOW_TYPE_APP_SUB_WINDOW, rect);
    ASSERT_NE(nullptr, subWindow);

    ASSERT_EQ(WMError::WM_OK, window->Show());
    ASSERT_EQ(WMError::WM_OK, subWindow->Show());

    ASSERT_EQ(WMError::WM_OK, subWindow->Hide());
    ASSERT_EQ(WMError::WM_OK, window->Hide());
    sleep(1);

    subWindow->Destroy();
    window->Destroy();
}

/**
 * @tc.name: SubWindow02
 * @tc.desc: FullScreen Main Window + 2 SubWindows
 * @tc.type: FUNC
 */
TEST_F(WindowSubWindowTest, SubWindow02)
{
    sptr<Window> window = CreateMainWindow();

    struct Rect rect = {0, 0, 100, 200};
    sptr<Window> subWindow0 = CreateSubWindow(window, WindowType::WINDOW_TYPE_APP_SUB_WINDOW, rect);
    ASSERT_NE(nullptr, subWindow0);

    sptr<Window> subWindow1 = CreateSubWindow(window, WindowType::WINDOW_TYPE_APP_SUB_WINDOW, rect);
    ASSERT_NE(nullptr, subWindow1);

    ASSERT_EQ(WMError::WM_OK, window->Show());
    ASSERT_EQ(WMError::WM_OK, subWindow0->Show());
    ASSERT_EQ(WMError::WM_OK, subWindow1->Show());

    ASSERT_EQ(WMError::WM_OK, subWindow0->Hide());
    ASSERT_EQ(WMError::WM_OK, subWindow1->Hide());
    ASSERT_EQ(WMError::WM_OK, window->Hide());
    sleep(1);

    subWindow0->Destroy();
    subWindow1->Destroy();
    window->Destroy();
}

/**
 * @tc.name: SubWindow03
 * @tc.desc: FullScreen Main Window + 5 subWindows
 * @tc.type: FUNC
 */
TEST_F(WindowSubWindowTest, SubWindow03)
{
    sptr<Window> window = CreateMainWindow();

    struct Rect rect = {0, 0, 100, 200};
    sptr<Window> subWindow0 = CreateSubWindow(window, WindowType::WINDOW_TYPE_APP_SUB_WINDOW, rect);
    ASSERT_NE(nullptr, subWindow0);

    sptr<Window> subWindow1 = CreateSubWindow(window, WindowType::WINDOW_TYPE_APP_SUB_WINDOW, rect);
    ASSERT_NE(nullptr, subWindow1);

    sptr<Window> subWindow2 = CreateSubWindow(window, WindowType::WINDOW_TYPE_APP_SUB_WINDOW, rect);
    ASSERT_NE(nullptr, subWindow2);

    sptr<Window> subWindow3 = CreateSubWindow(window, WindowType::WINDOW_TYPE_APP_SUB_WINDOW, rect);
    ASSERT_NE(nullptr, subWindow3);

    sptr<Window> subWindow4 = CreateSubWindow(window, WindowType::WINDOW_TYPE_APP_SUB_WINDOW, rect);
    ASSERT_NE(nullptr, subWindow4);

    ASSERT_EQ(WMError::WM_OK, window->Show());
    ASSERT_EQ(WMError::WM_OK, subWindow0->Show());
    ASSERT_EQ(WMError::WM_OK, subWindow1->Show());
    ASSERT_EQ(WMError::WM_OK, subWindow2->Show());
    ASSERT_EQ(WMError::WM_OK, subWindow3->Show());
    ASSERT_EQ(WMError::WM_OK, subWindow4->Show());

    ASSERT_EQ(WMError::WM_OK, subWindow0->Hide());
    ASSERT_EQ(WMError::WM_OK, subWindow1->Hide());
    ASSERT_EQ(WMError::WM_OK, subWindow2->Hide());
    ASSERT_EQ(WMError::WM_OK, subWindow3->Hide());
    ASSERT_EQ(WMError::WM_OK, subWindow4->Hide());
    ASSERT_EQ(WMError::WM_OK, window->Hide());
    sleep(1);

    subWindow0->Destroy();
    subWindow1->Destroy();
    subWindow2->Destroy();
    subWindow3->Destroy();
    subWindow4->Destroy();
    window->Destroy();
}

/**
 * @tc.name: SubWindow04
 * @tc.desc: FullScreen Main Window + 2 SubWindows with the same name
 * @tc.type: FUNC
 */
TEST_F(WindowSubWindowTest, SubWindow04)
{
    sptr<Window> window = CreateMainWindow();

    struct Rect rect = {0, 0, 100, 200};
    sptr<Window> subWindow0 = CreateSubWindow(window, WindowType::WINDOW_TYPE_APP_SUB_WINDOW, rect, "sub0");
    sptr<Window> subWindow1 = CreateSubWindow(window, WindowType::WINDOW_TYPE_APP_SUB_WINDOW, rect, "sub0");
    ASSERT_NE(nullptr, subWindow0);
    ASSERT_EQ(nullptr, subWindow1);

    ASSERT_EQ(WMError::WM_OK, window->Show());
    ASSERT_EQ(WMError::WM_OK, subWindow0->Show());

    ASSERT_EQ(WMError::WM_OK, subWindow0->Hide());
    ASSERT_EQ(WMError::WM_OK, window->Hide());
    sleep(1);

    subWindow0->Destroy();
    window->Destroy();
}

/**
 * @tc.name: SubWindow05
 * @tc.desc: FullScreen Main Window + 2 subwindows with the same name but one create after another destroyed
 * @tc.type: FUNC
 */
TEST_F(WindowSubWindowTest, SubWindow05)
{
    sptr<Window> window = CreateMainWindow();
    ASSERT_EQ(WMError::WM_OK, window->Show());

    struct Rect rect = {0, 0, 100, 200};
    sptr<Window> subWindow0 = CreateSubWindow(window, WindowType::WINDOW_TYPE_APP_SUB_WINDOW, rect, "sub1");
    ASSERT_NE(nullptr, subWindow0);
    ASSERT_EQ(WMError::WM_OK, subWindow0->Show());
    ASSERT_EQ(WMError::WM_OK, subWindow0->Destroy());


    sptr<Window> subWindow1 = CreateSubWindow(window, WindowType::WINDOW_TYPE_APP_SUB_WINDOW, rect, "sub1");
    ASSERT_NE(nullptr, subWindow1);
    ASSERT_EQ(WMError::WM_OK, subWindow1->Show());
    ASSERT_EQ(WMError::WM_OK, subWindow1->Destroy());

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
