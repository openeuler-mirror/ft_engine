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
#include <ability_context.h>
#include "window.h"
#include "window_option.h"
#include "window_scene.h"
#include "wm_common.h"

using namespace testing;
using namespace testing::ext;

namespace OHOS {
namespace Rosen {
class WindowSubWindowTest : public testing::Test {
public:
    static void SetUpTestCase();
    static void TearDownTestCase();
    virtual void SetUp() override;
    virtual void TearDown() override;
};

void WindowSubWindowTest::SetUpTestCase()
{
}

void WindowSubWindowTest::TearDownTestCase()
{
}

void WindowSubWindowTest::SetUp()
{
}

void WindowSubWindowTest::TearDown()
{
}

static sptr<WindowScene> CreateWindowScene()
{
    sptr<IWindowLifeCycle> listener = nullptr;
    std::shared_ptr<AbilityRuntime::AbilityContext> abilityContext = nullptr;

    sptr<WindowScene> scene = new WindowScene();
    scene->Init(0, abilityContext, listener);
    return scene;
}

static sptr<Window> CreateSubWindow(sptr<WindowScene> scene, WindowType type,
    struct Rect rect, uint32_t flags, std::string name = "")
{
    sptr<WindowOption> subOp = new WindowOption();
    subOp->SetWindowType(type);
    subOp->SetWindowMode(WindowMode::WINDOW_MODE_FLOATING);
    subOp->SetWindowRect(rect);
    subOp->SetWindowFlags(flags);

    static int cnt = 0;
    std::string subWinName = (name == "") ? "SubWindow" + std::to_string(cnt++) : name;

    return scene->CreateWindow(subWinName, subOp);
}

/**
 * @tc.name: SubWindow01
 * @tc.desc: FullScreen Main Window + Floating SubWindow
 * @tc.type: FUNC
 */
HWTEST_F(WindowSubWindowTest, SubWindow01, Function | MediumTest | Level2)
{
    sptr<WindowScene> scene = CreateWindowScene();

    struct Rect rect = {0, 0, 100, 200};
    uint32_t flags = 0;
    sptr<Window> subWindow = CreateSubWindow(scene, WindowType::WINDOW_TYPE_APP_SUB_WINDOW, rect, flags);
    ASSERT_NE(nullptr, subWindow);

    ASSERT_EQ(WMError::WM_OK, scene->GoForeground());
    ASSERT_EQ(WMError::WM_OK, subWindow->Show());

    ASSERT_EQ(WMError::WM_OK, subWindow->Hide());
    ASSERT_EQ(WMError::WM_OK, scene->GoBackground());
    sleep(1);

    subWindow->Destroy();
    scene->GoDestroy();
}

/**
 * @tc.name: SubWindow02
 * @tc.desc: FullScreen Main Window + Floating SubWindow & Parent Limit work
 * @tc.type: FUNC
 */
HWTEST_F(WindowSubWindowTest, SubWindow02, Function | MediumTest | Level2)
{
    sptr<WindowScene> scene = CreateWindowScene();

    struct Rect rect = {0, 0, 100, 200};
    uint32_t flags = static_cast<uint32_t>(WindowFlag::WINDOW_FLAG_PARENT_LIMIT);
    sptr<Window> subWindow = CreateSubWindow(scene, WindowType::WINDOW_TYPE_APP_SUB_WINDOW, rect, flags);
    ASSERT_NE(nullptr, subWindow);

    ASSERT_EQ(WMError::WM_OK, scene->GoForeground());
    ASSERT_EQ(WMError::WM_OK, subWindow->Show());

    ASSERT_EQ(WMError::WM_OK, subWindow->Hide());
    ASSERT_EQ(WMError::WM_OK, scene->GoBackground());
    sleep(1);

    subWindow->Destroy();
    scene->GoDestroy();
}

/**
 * @tc.name: SubWindow03
 * @tc.desc: FullScreen Main Window + Floating MediaWindow
 * @tc.type: FUNC
 */
HWTEST_F(WindowSubWindowTest, SubWindow03, Function | MediumTest | Level2)
{
    sptr<WindowScene> scene = CreateWindowScene();

    struct Rect rect = {0, 2000, 100, 200};
    uint32_t flags = static_cast<uint32_t>(WindowFlag::WINDOW_FLAG_PARENT_LIMIT);
    sptr<Window> subWindow = CreateSubWindow(scene, WindowType::WINDOW_TYPE_MEDIA, rect, flags);
    ASSERT_NE(nullptr, subWindow);

    ASSERT_EQ(WMError::WM_OK, scene->GoForeground());
    ASSERT_EQ(WMError::WM_OK, subWindow->Show());

    ASSERT_EQ(WMError::WM_OK, subWindow->Hide());
    ASSERT_EQ(WMError::WM_OK, scene->GoBackground());
    sleep(1);

    subWindow->Destroy();
    scene->GoDestroy();
}

/**
 * @tc.name: SubWindow04
 * @tc.desc: FullScreen Main Window + Floating MediaWindow
 * @tc.type: FUNC
 */
HWTEST_F(WindowSubWindowTest, SubWindow04, Function | MediumTest | Level2)
{
    sptr<WindowScene> scene = CreateWindowScene();

    struct Rect rect = {0, 2000, 3000, 2000};
    uint32_t flags = static_cast<uint32_t>(WindowFlag::WINDOW_FLAG_PARENT_LIMIT);
    sptr<Window> subWindow = CreateSubWindow(scene, WindowType::WINDOW_TYPE_MEDIA, rect, flags);
    ASSERT_NE(nullptr, subWindow);

    ASSERT_EQ(WMError::WM_OK, scene->GoForeground());
    ASSERT_EQ(WMError::WM_OK, subWindow->Show());

    ASSERT_EQ(WMError::WM_OK, subWindow->Hide());
    ASSERT_EQ(WMError::WM_OK, scene->GoBackground());
    sleep(1);

    subWindow->Destroy();
    scene->GoDestroy();
}

/**
 * @tc.name: SubWindow05
 * @tc.desc: FullScreen Main Window + Floating MediaWindow + Floating SubWindow
 * @tc.type: FUNC
 */
HWTEST_F(WindowSubWindowTest, SubWindow05, Function | MediumTest | Level3)
{
    sptr<WindowScene> scene = CreateWindowScene();

    struct Rect rect = {0, 0, 100, 200};
    uint32_t flags = static_cast<uint32_t>(WindowFlag::WINDOW_FLAG_PARENT_LIMIT);
    sptr<Window> subWindow = CreateSubWindow(scene, WindowType::WINDOW_TYPE_MEDIA, rect, flags);
    ASSERT_NE(nullptr, subWindow);

    sptr<Window> subWindow2 = CreateSubWindow(scene, WindowType::WINDOW_TYPE_APP_SUB_WINDOW, rect, flags);
    ASSERT_NE(nullptr, subWindow2);

    ASSERT_EQ(WMError::WM_OK, scene->GoForeground());
    ASSERT_EQ(WMError::WM_OK, subWindow->Show());
    ASSERT_EQ(WMError::WM_OK, subWindow2->Show());

    ASSERT_EQ(WMError::WM_OK, subWindow->Hide());
    ASSERT_EQ(WMError::WM_OK, subWindow2->Hide());
    ASSERT_EQ(WMError::WM_OK, scene->GoBackground());
    sleep(1);

    subWindow->Destroy();
    subWindow2->Destroy();
    scene->GoDestroy();
}

/**
 * @tc.name: SubWindow06
 * @tc.desc: FullScreen Main Window + 2 SubWindows
 * @tc.type: FUNC
 */
HWTEST_F(WindowSubWindowTest, SubWindow06, Function | MediumTest | Level3)
{
    sptr<WindowScene> scene = CreateWindowScene();

    struct Rect rect = {0, 0, 100, 200};
    sptr<Window> subWindow0 = CreateSubWindow(scene, WindowType::WINDOW_TYPE_APP_SUB_WINDOW, rect, 0);
    ASSERT_NE(nullptr, subWindow0);

    sptr<Window> subWindow1 = CreateSubWindow(scene, WindowType::WINDOW_TYPE_APP_SUB_WINDOW, rect, 0);
    ASSERT_NE(nullptr, subWindow1);

    ASSERT_EQ(WMError::WM_OK, scene->GoForeground());
    ASSERT_EQ(WMError::WM_OK, subWindow0->Show());
    ASSERT_EQ(WMError::WM_OK, subWindow1->Show());

    ASSERT_EQ(WMError::WM_OK, subWindow0->Hide());
    ASSERT_EQ(WMError::WM_OK, subWindow1->Hide());
    ASSERT_EQ(WMError::WM_OK, scene->GoBackground());
    sleep(1);

    subWindow0->Destroy();
    subWindow1->Destroy();
    scene->GoDestroy();
}

/**
 * @tc.name: SubWindow07
 * @tc.desc: FullScreen Main Window + Floating SubWindow & MainWindow First GoBackground
 * @tc.type: FUNC
 */
HWTEST_F(WindowSubWindowTest, SubWindow07, Function | MediumTest | Level4)
{
    sptr<WindowScene> scene = CreateWindowScene();

    struct Rect rect = {0, 0, 100, 200};
    uint32_t flags = static_cast<uint32_t>(WindowFlag::WINDOW_FLAG_PARENT_LIMIT);
    sptr<Window> subWindow = CreateSubWindow(scene, WindowType::WINDOW_TYPE_APP_SUB_WINDOW, rect, flags);
    ASSERT_NE(nullptr, subWindow);

    ASSERT_EQ(WMError::WM_OK, scene->GoForeground());
    ASSERT_EQ(WMError::WM_OK, subWindow->Show());

    ASSERT_EQ(WMError::WM_OK, scene->GoBackground());
    ASSERT_EQ(WMError::WM_OK, subWindow->Hide());
    sleep(1);

    subWindow->Destroy();
    scene->GoDestroy();
}

/**
 * @tc.name: SubWindow08
 * @tc.desc: FullScreen Main Window + Floating SubWindow & only show SubWindow
 * @tc.type: FUNC
 */
HWTEST_F(WindowSubWindowTest, SubWindow08, Function | MediumTest | Level4)
{
    sptr<WindowScene> scene = CreateWindowScene();

    struct Rect rect = {0, 0, 100, 200};
    uint32_t flags = static_cast<uint32_t>(WindowFlag::WINDOW_FLAG_PARENT_LIMIT);
    sptr<Window> subWindow = CreateSubWindow(scene, WindowType::WINDOW_TYPE_APP_SUB_WINDOW, rect, flags);
    ASSERT_NE(nullptr, subWindow);

    ASSERT_EQ(WMError::WM_ERROR_INVALID_PARAM, subWindow->Show());
}

/**
 * @tc.name: SubWindow09
 * @tc.desc: FullScreen Main Window + Floating SubWindow & first destroy SubWindow, then destroy MainWindow
 * @tc.type: FUNC
 */
HWTEST_F(WindowSubWindowTest, SubWindow09, Function | MediumTest | Level2)
{
    sptr<WindowScene> scene = CreateWindowScene();

    struct Rect rect = {0, 0, 100, 200};
    uint32_t flags = static_cast<uint32_t>(WindowFlag::WINDOW_FLAG_PARENT_LIMIT);
    sptr<Window> subWindow = CreateSubWindow(scene, WindowType::WINDOW_TYPE_APP_SUB_WINDOW, rect, flags);
    ASSERT_NE(nullptr, subWindow);

    ASSERT_EQ(WMError::WM_OK, scene->GoForeground());
    ASSERT_EQ(WMError::WM_OK, subWindow->Show());

    ASSERT_EQ(WMError::WM_OK, subWindow->Hide());
    ASSERT_EQ(WMError::WM_OK, scene->GoBackground());
    sleep(1);

    ASSERT_EQ(WMError::WM_OK, subWindow->Destroy());

    subWindow->Destroy();
    scene->GoDestroy();
}

/**
 * @tc.name: SubWindow10
 * @tc.desc: FullScreen Main Window + Floating SubWindow & first destroy MainWindow, then destroy SubWindow
 * @tc.type: FUNC
 */
HWTEST_F(WindowSubWindowTest, SubWindow10, Function | MediumTest | Level2)
{
    sptr<WindowScene> scene = CreateWindowScene();

    struct Rect rect = {0, 0, 100, 200};
    uint32_t flags = static_cast<uint32_t>(WindowFlag::WINDOW_FLAG_PARENT_LIMIT);
    sptr<Window> subWindow = CreateSubWindow(scene, WindowType::WINDOW_TYPE_APP_SUB_WINDOW, rect, flags);
    ASSERT_NE(nullptr, subWindow);

    ASSERT_EQ(WMError::WM_OK, scene->GoForeground());
    ASSERT_EQ(WMError::WM_OK, subWindow->Show());
    sleep(1);

    sptr<Window> mainWindow = scene->GetMainWindow();
    ASSERT_EQ(WMError::WM_OK, mainWindow->Destroy());
    ASSERT_EQ(WMError::WM_OK, subWindow->Destroy());

    scene->GoDestroy();
}

/**
 * @tc.name: SubWindow11
 * @tc.desc: FullScreen Main Window + 5 subWindows
 * @tc.type: FUNC
 */
HWTEST_F(WindowSubWindowTest, SubWindow11, Function | MediumTest | Level3)
{
    sptr<WindowScene> scene = CreateWindowScene();

    struct Rect rect = {0, 0, 100, 200};
    sptr<Window> subWindow0 = CreateSubWindow(scene, WindowType::WINDOW_TYPE_APP_SUB_WINDOW, rect, 0);
    ASSERT_NE(nullptr, subWindow0);

    sptr<Window> subWindow1 = CreateSubWindow(scene, WindowType::WINDOW_TYPE_APP_SUB_WINDOW, rect, 0);
    ASSERT_NE(nullptr, subWindow1);

    sptr<Window> subWindow2 = CreateSubWindow(scene, WindowType::WINDOW_TYPE_APP_SUB_WINDOW, rect, 0);
    ASSERT_NE(nullptr, subWindow2);

    sptr<Window> subWindow3 = CreateSubWindow(scene, WindowType::WINDOW_TYPE_APP_SUB_WINDOW, rect, 0);
    ASSERT_NE(nullptr, subWindow3);

    sptr<Window> subWindow4 = CreateSubWindow(scene, WindowType::WINDOW_TYPE_APP_SUB_WINDOW, rect, 0);
    ASSERT_NE(nullptr, subWindow4);

    ASSERT_EQ(WMError::WM_OK, scene->GoForeground());
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
    ASSERT_EQ(WMError::WM_OK, scene->GoBackground());
    sleep(1);

    subWindow0->Destroy();
    subWindow1->Destroy();
    subWindow2->Destroy();
    subWindow3->Destroy();
    subWindow4->Destroy();
    scene->GoDestroy();
}

/**
 * @tc.name: SubWindow12
 * @tc.desc: FullScreen Main Window + 2 SubWindows with the same name
 * @tc.type: FUNC
 */
HWTEST_F(WindowSubWindowTest, SubWindow12, Function | MediumTest | Level3)
{
    sptr<WindowScene> scene = CreateWindowScene();

    struct Rect rect = {0, 0, 100, 200};
    sptr<Window> subWindow0 = CreateSubWindow(scene, WindowType::WINDOW_TYPE_APP_SUB_WINDOW, rect, 0, "sub0");
    sptr<Window> subWindow1 = CreateSubWindow(scene, WindowType::WINDOW_TYPE_APP_SUB_WINDOW, rect, 0, "sub0");
    ASSERT_NE(nullptr, subWindow0);
    ASSERT_EQ(nullptr, subWindow1);

    ASSERT_EQ(WMError::WM_OK, scene->GoForeground());
    ASSERT_EQ(WMError::WM_OK, subWindow0->Show());

    ASSERT_EQ(WMError::WM_OK, subWindow0->Hide());
    ASSERT_EQ(WMError::WM_OK, scene->GoBackground());
    sleep(1);

    subWindow0->Destroy();
    scene->GoDestroy();
}

/**
 * @tc.name: SubWindow13
 * @tc.desc: FullScreen Main Window + 2 subwindows with the same name but one create after another destroyed
 * @tc.type: FUNC
 */
HWTEST_F(WindowSubWindowTest, SubWindow13, Function | MediumTest | Level3)
{
    sptr<WindowScene> scene = CreateWindowScene();
    ASSERT_EQ(WMError::WM_OK, scene->GoForeground());

    struct Rect rect = {0, 0, 100, 200};
    sptr<Window> subWindow0 = CreateSubWindow(scene, WindowType::WINDOW_TYPE_APP_SUB_WINDOW, rect, 0, "sub1");
    ASSERT_NE(nullptr, subWindow0);
    ASSERT_EQ(WMError::WM_OK, subWindow0->Show());
    ASSERT_EQ(WMError::WM_OK, subWindow0->Destroy());


    sptr<Window> subWindow1 = CreateSubWindow(scene, WindowType::WINDOW_TYPE_APP_SUB_WINDOW, rect, 0, "sub1");
    ASSERT_NE(nullptr, subWindow1);
    ASSERT_EQ(WMError::WM_OK, subWindow1->Show());
    ASSERT_EQ(WMError::WM_OK, subWindow1->Destroy());

    ASSERT_EQ(WMError::WM_OK, scene->GoBackground());
    sleep(1);
    scene->GoDestroy();
}
} // namespace Rosen
} // namespace OHOS
