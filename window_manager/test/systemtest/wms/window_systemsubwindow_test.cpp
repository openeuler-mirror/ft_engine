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
class WindowSystemSubWindowTest : public testing::Test {
public:
    static void SetUpTestCase();
    static void TearDownTestCase();
    virtual void SetUp() override;
    virtual void TearDown() override;
};

void WindowSystemSubWindowTest::SetUpTestCase()
{
}

void WindowSystemSubWindowTest::TearDownTestCase()
{
}

void WindowSystemSubWindowTest::SetUp()
{
}

void WindowSystemSubWindowTest::TearDown()
{
}

static sptr<Window> CreateBaseWindow(WindowType type, struct Rect rect, uint32_t flags)
{
    sptr<WindowOption> baseOp = new WindowOption();
    baseOp->SetWindowType(type);
    baseOp->SetWindowMode(WindowMode::WINDOW_MODE_FLOATING);
    baseOp->SetWindowRect(rect);
    baseOp->SetWindowFlags(flags);

    static int baseCount = 0;
    std::string baseWindowName = "BaseWindow" + std::to_string(baseCount++);
    sptr<Window> window = Window::Create(baseWindowName, baseOp, nullptr);
    return window;
}

static sptr<Window> CreateAppSubWindow(sptr<Window> parentWindow, WindowType type, struct Rect rect,
    uint32_t flags, std::string name = "")
{
    sptr<WindowOption> subOp = new WindowOption();
    subOp->SetWindowType(type);
    subOp->SetWindowMode(WindowMode::WINDOW_MODE_FLOATING);
    subOp->SetWindowRect(rect);
    subOp->SetWindowFlags(flags);
    subOp->SetParentId(parentWindow->GetWindowId());

    static int cnt = 0;
    std::string subWinName = (name == "") ? "AppSubWindow" + std::to_string(cnt++) : name;
    sptr<Window> window = Window::Create(subWinName, subOp);
    return window;
}

static sptr<Window> CreateSystemSubWindow(sptr<Window> parentWindow, struct Rect rect,
    uint32_t flags, std::string name = "")
{
    sptr<WindowOption> subOp = new WindowOption();
    subOp->SetWindowType(WindowType::WINDOW_TYPE_SYSTEM_SUB_WINDOW);
    subOp->SetWindowMode(WindowMode::WINDOW_MODE_FLOATING);
    subOp->SetWindowRect(rect);
    subOp->SetWindowFlags(flags);
    subOp->SetParentId(parentWindow->GetWindowId());

    static int cnt = 0;
    std::string subWinName = (name == "") ? "SystemSubWindow" + std::to_string(cnt++) : name;
    sptr<Window> window = Window::Create(subWinName, subOp);
    return window;
}

/**
 * @tc.name: SystemSubWindow01
 * @tc.desc: create sub windows with below system Windows
 * @tc.type: FUNC
 */
HWTEST_F(WindowSystemSubWindowTest, SystemSubWindow01, Function | MediumTest | Level2)
{
    std::vector<WindowType> windowTypes = {
        WindowType::WINDOW_TYPE_WALLPAPER,
        WindowType::WINDOW_TYPE_DESKTOP,
    };
    for (auto itor = windowTypes.begin(); itor != windowTypes.end(); itor++) {
        struct Rect baseRect = {0, 0, 100, 200};
        uint32_t baseFlags = 0;
        sptr<Window> baseWindow = CreateBaseWindow(static_cast<WindowType>(*itor), baseRect, baseFlags);
        ASSERT_NE(nullptr, baseWindow);
        struct Rect rect = {0, 0, 100, 200};
        uint32_t flags = 0;
        sptr<Window> subWindow = CreateSystemSubWindow(baseWindow, rect, flags);
        ASSERT_NE(nullptr, subWindow);

        ASSERT_EQ(WMError::WM_OK, baseWindow->Show());
        ASSERT_EQ(WMError::WM_OK, subWindow->Show());

        ASSERT_EQ(WMError::WM_OK, subWindow->Hide());
        ASSERT_EQ(WMError::WM_OK, baseWindow->Hide());

        ASSERT_EQ(WMError::WM_OK, subWindow->Destroy());
        ASSERT_EQ(WMError::WM_OK, baseWindow->Destroy());
    }
}

/**
 * @tc.name: SystemSubWindow02
 * @tc.desc: create sub windows with above system Windows except WINDOW_TYPE_DIALOG
 * @tc.type: FUNC
 */
HWTEST_F(WindowSystemSubWindowTest, SystemSubWindow02, Function | MediumTest | Level2)
{
    std::vector<WindowType> windowTypes = {
        WindowType::WINDOW_TYPE_APP_LAUNCHING,
        WindowType::WINDOW_TYPE_DOCK_SLICE,
        WindowType::WINDOW_TYPE_INCOMING_CALL,
        WindowType::WINDOW_TYPE_SEARCHING_BAR,
        WindowType::WINDOW_TYPE_SYSTEM_ALARM_WINDOW,
        WindowType::WINDOW_TYPE_INPUT_METHOD_FLOAT,
        WindowType::WINDOW_TYPE_FLOAT,
        WindowType::WINDOW_TYPE_TOAST,
        WindowType::WINDOW_TYPE_STATUS_BAR,
        WindowType::WINDOW_TYPE_PANEL,
        WindowType::WINDOW_TYPE_KEYGUARD,
        WindowType::WINDOW_TYPE_VOLUME_OVERLAY,
        WindowType::WINDOW_TYPE_NAVIGATION_BAR,
        WindowType::WINDOW_TYPE_DRAGGING_EFFECT,
        WindowType::WINDOW_TYPE_POINTER,
        WindowType::WINDOW_TYPE_LAUNCHER_RECENT,
        WindowType::WINDOW_TYPE_LAUNCHER_DOCK,
        WindowType::WINDOW_TYPE_BOOT_ANIMATION,
        WindowType::WINDOW_TYPE_FREEZE_DISPLAY,
        WindowType::WINDOW_TYPE_VOICE_INTERACTION,
        WindowType::WINDOW_TYPE_FLOAT_CAMERA,
        WindowType::WINDOW_TYPE_PLACEHOLDER,
        WindowType::WINDOW_TYPE_SCREENSHOT,
    };
    for (auto itor = windowTypes.begin(); itor != windowTypes.end(); itor++) {
        struct Rect baseRect = {0, 0, 100, 200};
        uint32_t baseFlags = 0;
        sptr<Window> baseWindow = CreateBaseWindow(static_cast<WindowType>(*itor), baseRect, baseFlags);
        ASSERT_NE(nullptr, baseWindow);

        struct Rect rect = {0, 0, 100, 200};
        uint32_t flags = 0;
        sptr<Window> subWindow = CreateSystemSubWindow(baseWindow, rect, flags);
        ASSERT_NE(nullptr, subWindow);

        ASSERT_EQ(WMError::WM_OK, baseWindow->Show());
        ASSERT_EQ(WMError::WM_OK, subWindow->Show());

        ASSERT_EQ(WMError::WM_OK, subWindow->Hide());
        ASSERT_EQ(WMError::WM_OK, baseWindow->Hide());

        ASSERT_EQ(WMError::WM_OK, subWindow->Destroy());
        ASSERT_EQ(WMError::WM_OK, baseWindow->Destroy());
    }
}

/**
 * @tc.name: SystemSubWindow03
 * @tc.desc: create sub windows with app main Windows
 * @tc.type: FUNC
 */
HWTEST_F(WindowSystemSubWindowTest, SystemSubWindow03, Function | MediumTest | Level2)
{
    std::vector<WindowType> windowTypes = { WindowType::WINDOW_TYPE_APP_MAIN_WINDOW };
    for (auto itor = windowTypes.begin(); itor != windowTypes.end(); itor++) {
        struct Rect baseRect = {0, 0, 100, 200};
        uint32_t baseFlags = 0;
        sptr<Window> baseWindow = CreateBaseWindow(static_cast<WindowType>(*itor), baseRect, baseFlags);
        ASSERT_NE(nullptr, baseWindow);

        struct Rect rect = {0, 0, 100, 200};
        uint32_t flags = 0;
        sptr<Window> subWindow = CreateSystemSubWindow(baseWindow, rect, flags);
        ASSERT_NE(nullptr, subWindow);

        ASSERT_EQ(WMError::WM_OK, baseWindow->Show());
        ASSERT_EQ(WMError::WM_OK, subWindow->Show());

        ASSERT_EQ(WMError::WM_OK, subWindow->Hide());
        ASSERT_EQ(WMError::WM_OK, baseWindow->Hide());

        ASSERT_EQ(WMError::WM_OK, subWindow->Destroy());
        ASSERT_EQ(WMError::WM_OK, baseWindow->Destroy());
    }
}

/**
 * @tc.name: SystemSubWindow04
 * @tc.desc: create sub windows with app sub Windows
 * @tc.type: FUNC
 */
HWTEST_F(WindowSystemSubWindowTest, SystemSubWindow04, Function | MediumTest | Level2)
{
    std::vector<WindowType> windowTypes = {
        WindowType::WINDOW_TYPE_MEDIA,
        WindowType::WINDOW_TYPE_APP_SUB_WINDOW,
        WindowType::WINDOW_TYPE_APP_COMPONENT,
    };
    for (auto itor = windowTypes.begin(); itor != windowTypes.end(); itor++) {
        struct Rect baseRect = {0, 0, 100, 200};
        uint32_t baseFlags = 0;
        sptr<Window> baseWindow = CreateBaseWindow(WindowType::WINDOW_TYPE_APP_MAIN_WINDOW, baseRect, baseFlags);
        ASSERT_NE(nullptr, baseWindow);

        sptr<Window> appSubWindow = CreateAppSubWindow(baseWindow, static_cast<WindowType>(*itor), baseRect, baseFlags);
        ASSERT_NE(nullptr, appSubWindow);

        struct Rect rect = {0, 0, 100, 200};
        uint32_t flags = 0;
        sptr<Window> subWindow = CreateSystemSubWindow(appSubWindow, rect, flags);
        ASSERT_EQ(nullptr, subWindow);
        ASSERT_EQ(WMError::WM_OK, appSubWindow->Destroy());
        ASSERT_EQ(WMError::WM_OK, baseWindow->Destroy());
    }
}

/**
 * @tc.name: SystemSubWindow05
 * @tc.desc: create sub windows with system sub Windows
 * @tc.type: FUNC
 */
HWTEST_F(WindowSystemSubWindowTest, SystemSubWindow05, Function | MediumTest | Level3)
{
    struct Rect baseRect = {0, 0, 100, 200};
    uint32_t baseFlags = 0;
    sptr<Window> baseWindow = CreateBaseWindow(WindowType::WINDOW_TYPE_DOCK_SLICE, baseRect, baseFlags);
    ASSERT_NE(nullptr, baseWindow);

    sptr<Window> systemSubWindow = CreateSystemSubWindow(baseWindow, baseRect, baseFlags);
    ASSERT_NE(nullptr, systemSubWindow);

    struct Rect rect = {0, 0, 100, 200};
    uint32_t flags = 0;
    sptr<Window> subWindow = CreateSystemSubWindow(systemSubWindow, rect, flags);
    ASSERT_EQ(nullptr, subWindow);

    ASSERT_EQ(WMError::WM_OK, baseWindow->Show());
    ASSERT_EQ(WMError::WM_OK, systemSubWindow->Show());

    ASSERT_EQ(WMError::WM_OK, systemSubWindow->Hide());
    ASSERT_EQ(WMError::WM_OK, baseWindow->Hide());

    ASSERT_EQ(WMError::WM_OK, systemSubWindow->Destroy());
    ASSERT_EQ(WMError::WM_OK, baseWindow->Destroy());
}

/**
 * @tc.name: SystemSubWindow06
 * @tc.desc: FullScreen Main Window + 2 SubWindows
 * @tc.type: FUNC
 */
HWTEST_F(WindowSystemSubWindowTest, SystemSubWindow06, Function | MediumTest | Level3)
{
    struct Rect baseRect = {0, 0, 100, 200};
    uint32_t baseFlags = 0;
    sptr<Window> baseWindow = CreateBaseWindow(WindowType::WINDOW_TYPE_DOCK_SLICE, baseRect, baseFlags);
    ASSERT_NE(nullptr, baseWindow);

    struct Rect rect = {0, 0, 100, 200};
    uint32_t flags = 0;
    sptr<Window> subWindow = CreateSystemSubWindow(baseWindow, rect, flags);
    ASSERT_NE(nullptr, subWindow);

    ASSERT_EQ(WMError::WM_OK, baseWindow->Show());
    ASSERT_EQ(WMError::WM_OK, subWindow->Show());

    bool isFocus = subWindow->GetFocusable();
    ASSERT_EQ(WMError::WM_OK, subWindow->SetFocusable(!isFocus));
    ASSERT_EQ(WMError::WM_OK, subWindow->MoveTo(0, 0));
    ASSERT_EQ(WMError::WM_OK, subWindow->Resize(200, 400));
    ASSERT_EQ(WMError::WM_OK, subWindow->SetTurnScreenOn(true));

    ASSERT_EQ(WMError::WM_ERROR_INVALID_TYPE, subWindow->SetBrightness(0.5f));
    ASSERT_EQ(WMError::WM_OK, subWindow->SetWindowMode(WindowMode::WINDOW_MODE_FULLSCREEN));

    ASSERT_EQ(WMError::WM_OK, subWindow->Hide());
    ASSERT_EQ(WMError::WM_OK, baseWindow->Hide());

    ASSERT_EQ(WMError::WM_OK, subWindow->Destroy());
    ASSERT_EQ(WMError::WM_OK, baseWindow->Destroy());
}
/**
 * @tc.name: SystemSubWindow07
 * @tc.desc: create sub windows with dialog
 * @tc.type: FUNC
 */
HWTEST_F(WindowSystemSubWindowTest, SystemSubWindow07, Function | MediumTest | Level3)
{
    struct Rect baseRect = {0, 0, 100, 200};
    uint32_t baseFlags = 0;
    sptr<Window> baseWindow = CreateBaseWindow(WindowType::WINDOW_TYPE_DIALOG, baseRect, baseFlags);
    ASSERT_NE(nullptr, baseWindow);

    struct Rect rect = {0, 0, 100, 200};
    uint32_t flags = 0;
    sptr<Window> subWindow = CreateSystemSubWindow(baseWindow, rect, flags);
    ASSERT_EQ(nullptr, subWindow);
    ASSERT_EQ(WMError::WM_OK, baseWindow->Destroy());
}
} // namespace Rosen
} // namespace OHOS
