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
#include <gtest/gtest.h>
#include "ability_context_impl.h"
#include "window.h"
#include "window_manager.h"
#include "window_option.h"
#include "window_scene.h"
#include "wm_common.h"

using namespace testing;
using namespace testing::ext;

namespace OHOS {
namespace Rosen {
class WindowDialogWindowTest : public testing::Test {
public:
    static void SetUpTestCase();
    static void TearDownTestCase();
    virtual void SetUp() override;
    virtual void TearDown() override;

    static inline std::shared_ptr<AbilityRuntime::AbilityContext> abilityContext_ = nullptr;
};

void WindowDialogWindowTest::SetUpTestCase()
{
}

void WindowDialogWindowTest::TearDownTestCase()
{
}

void WindowDialogWindowTest::SetUp()
{
}

void WindowDialogWindowTest::TearDown()
{
}

static sptr<WindowScene> CreateWindowScene()
{
    sptr<IWindowLifeCycle> listener = nullptr;
    WindowDialogWindowTest::abilityContext_ = std::make_shared<AbilityRuntime::AbilityContextImpl>();

    sptr<WindowScene> scene = new WindowScene();
    scene->Init(0, WindowDialogWindowTest::abilityContext_, listener);
    return scene;
}

static sptr<Window> CreateDialogWindow(sptr<WindowScene> scene, WindowType type, Rect rect, std::string name = "")
{
    sptr<WindowOption> option = new WindowOption();
    option->SetWindowType(type);
    option->SetWindowMode(WindowMode::WINDOW_MODE_FLOATING);
    option->SetWindowRect(rect);

    static int cnt = 0;
    std::string winName = (name == "") ? "DialogWindowTest" + std::to_string(cnt++) : name;

    return Window::Create(winName, option, scene->GetMainWindow()->GetContext());
}

/**
 * @tc.name: dialogWindow01
 * @tc.desc: FullScreen Main Window + Floating dialogWindow
 * @tc.type: FUNC
 * @tc.require: issueI5GKWZ
 */
HWTEST_F(WindowDialogWindowTest, DialogWindow01, Function | MediumTest | Level2)
{
    sptr<WindowScene> scene = CreateWindowScene();

    struct Rect rect = {0, 0, 100, 200};
    sptr<Window> dialogWindow = CreateDialogWindow(scene, WindowType::WINDOW_TYPE_DIALOG, rect);
    ASSERT_NE(nullptr, dialogWindow);

    ASSERT_EQ(WMError::WM_OK, scene->GoForeground());
    ASSERT_EQ(WMError::WM_OK, dialogWindow->Show());

    ASSERT_EQ(WMError::WM_OK, dialogWindow->Hide());
    ASSERT_EQ(WMError::WM_OK, scene->GoBackground());

    dialogWindow->Destroy();
    scene->GoDestroy();
}

/**
 * @tc.name: dialogWindow02
 * @tc.desc: FullScreen Main Window + 2 dialogWindow
 * @tc.type: FUNC
 * @tc.require: issueI5GKWZ
 */
HWTEST_F(WindowDialogWindowTest, DialogWindow02, Function | MediumTest | Level2)
{
    sptr<WindowScene> scene = CreateWindowScene();

    struct Rect rect = {0, 0, 100, 200};
    sptr<Window> dialogWindow0 = CreateDialogWindow(scene, WindowType::WINDOW_TYPE_DIALOG, rect);
    ASSERT_NE(nullptr, dialogWindow0);

    sptr<Window> dialogWindow1 = CreateDialogWindow(scene, WindowType::WINDOW_TYPE_DIALOG, rect);
    ASSERT_EQ(nullptr, dialogWindow1);

    ASSERT_EQ(WMError::WM_OK, scene->GoForeground());
    ASSERT_EQ(WMError::WM_OK, dialogWindow0->Show());

    ASSERT_EQ(WMError::WM_OK, dialogWindow0->Hide());
    ASSERT_EQ(WMError::WM_OK, scene->GoBackground());

    dialogWindow0->Destroy();
    scene->GoDestroy();
}

/**
 * @tc.name: dialogWindow03
 * @tc.desc: FullScreen Main Window + 2 dialogWindow with the same name
 * @tc.type: FUNC
 * @tc.require: issueI5GKWZ
 */
HWTEST_F(WindowDialogWindowTest, DialogWindow03, Function | MediumTest | Level2)
{
    sptr<WindowScene> scene = CreateWindowScene();

    struct Rect rect = {0, 0, 100, 200};
    sptr<Window> dialogWindow0 = CreateDialogWindow(scene, WindowType::WINDOW_TYPE_DIALOG, rect, "dialog0");
    sptr<Window> dialogWindow1 = CreateDialogWindow(scene, WindowType::WINDOW_TYPE_DIALOG, rect, "dialog0");
    ASSERT_NE(nullptr, dialogWindow0);
    ASSERT_EQ(nullptr, dialogWindow1);

    ASSERT_EQ(WMError::WM_OK, scene->GoForeground());
    ASSERT_EQ(WMError::WM_OK, dialogWindow0->Show());

    ASSERT_EQ(WMError::WM_OK, dialogWindow0->Hide());
    ASSERT_EQ(WMError::WM_OK, scene->GoBackground());

    dialogWindow0->Destroy();
    scene->GoDestroy();
}

/**
 * @tc.name: dialogWindow04
 * @tc.desc: FullScreen Main Window + 2 dialogWindow with the same name but one create after another destroyed
 * @tc.type: FUNC
 * @tc.require: issueI5GKWZ
 */
HWTEST_F(WindowDialogWindowTest, DialogWindow04, Function | MediumTest | Level2)
{
    sptr<WindowScene> scene = CreateWindowScene();
    ASSERT_EQ(WMError::WM_OK, scene->GoForeground());

    struct Rect rect = {0, 0, 100, 200};
    sptr<Window> dialogWindow0 = CreateDialogWindow(scene, WindowType::WINDOW_TYPE_DIALOG, rect, "dialog0");
    ASSERT_NE(nullptr, dialogWindow0);
    ASSERT_EQ(WMError::WM_OK, dialogWindow0->Show());
    ASSERT_EQ(WMError::WM_OK, dialogWindow0->Destroy());


    sptr<Window> dialogWindow1 = CreateDialogWindow(scene, WindowType::WINDOW_TYPE_DIALOG, rect, "dialog0");
    ASSERT_NE(nullptr, dialogWindow1);
    ASSERT_EQ(WMError::WM_OK, dialogWindow1->Show());
    ASSERT_EQ(WMError::WM_OK, dialogWindow1->Destroy());

    ASSERT_EQ(WMError::WM_OK, scene->GoBackground());
    scene->GoDestroy();
}

/**
 * @tc.name: dialogWindow05
 * @tc.desc: FullScreen Main Window + dialogWindow & MainWindow First GoBackground
 * @tc.type: FUNC
 * @tc.require: issueI5GKWZ
 */
HWTEST_F(WindowDialogWindowTest, DialogWindow05, Function | MediumTest | Level2)
{
    sptr<WindowScene> scene = CreateWindowScene();

    struct Rect rect = {0, 0, 100, 200};
    sptr<Window> dialogWindow = CreateDialogWindow(scene, WindowType::WINDOW_TYPE_DIALOG, rect);
    ASSERT_NE(nullptr, dialogWindow);

    ASSERT_EQ(WMError::WM_OK, scene->GoForeground());
    ASSERT_EQ(WMError::WM_OK, dialogWindow->Show());

    ASSERT_EQ(WMError::WM_OK, scene->GoBackground());
    ASSERT_EQ(WMError::WM_OK, dialogWindow->Hide());

    dialogWindow->Destroy();
    scene->GoDestroy();
}

/**
 * @tc.name: dialogWindow06
 * @tc.desc: FullScreen Main Window + dialogWindow & first destroy dialogWindow, then destroy MainWindow
 * @tc.type: FUNC
 * @tc.require: issueI5GKWZ
 */
HWTEST_F(WindowDialogWindowTest, DialogWindow06, Function | MediumTest | Level2)
{
    sptr<WindowScene> scene = CreateWindowScene();

    struct Rect rect = {0, 0, 100, 200};
    sptr<Window> dialogWindow = CreateDialogWindow(scene, WindowType::WINDOW_TYPE_DIALOG, rect);
    ASSERT_NE(nullptr, dialogWindow);

    ASSERT_EQ(WMError::WM_OK, scene->GoForeground());
    ASSERT_EQ(WMError::WM_OK, dialogWindow->Show());

    ASSERT_EQ(WMError::WM_OK, dialogWindow->Hide());
    ASSERT_EQ(WMError::WM_OK, scene->GoBackground());

    ASSERT_EQ(WMError::WM_OK, dialogWindow->Destroy());

    dialogWindow->Destroy();
    scene->GoDestroy();
}

/**
 * @tc.name: dialogWindow07
 * @tc.desc: FullScreen Main Window + dialogWindow & first destroy MainWindow, then destroy dialogWindow
 * @tc.type: FUNC
 * @tc.require: issueI5GKWZ
 */
HWTEST_F(WindowDialogWindowTest, DialogWindow07, Function | MediumTest | Level2)
{
    sptr<WindowScene> scene = CreateWindowScene();

    struct Rect rect = {0, 0, 100, 200};
    sptr<Window> dialogWindow = CreateDialogWindow(scene, WindowType::WINDOW_TYPE_DIALOG, rect);
    ASSERT_NE(nullptr, dialogWindow);

    ASSERT_EQ(WMError::WM_OK, scene->GoForeground());
    ASSERT_EQ(WMError::WM_OK, dialogWindow->Show());

    sptr<Window> mainWindow = scene->GetMainWindow();
    ASSERT_EQ(WMError::WM_OK, mainWindow->Destroy());
    ASSERT_EQ(WMError::WM_OK, dialogWindow->Destroy());

    scene->GoDestroy();
}
} // namespace Rosen
} // namespace OHOS
