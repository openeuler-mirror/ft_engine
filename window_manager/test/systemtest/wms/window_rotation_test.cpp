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
#include "window_test_utils.h"

#include "display_manager.h"
#include "future.h"
#include "screen_manager.h"
#include "window_manager.h"
#include "window_accessibility_controller.h"
#include "window_impl.h"
#include "wm_common.h"

using namespace testing;
using namespace testing::ext;

namespace OHOS {
namespace Rosen {
using Utils = WindowTestUtils;
class DisplayListener : public DisplayManager::IDisplayListener {
public:
    virtual void OnCreate(DisplayId) override;
    virtual void OnDestroy(DisplayId) override;
    virtual void OnChange(DisplayId) override;
    RunnableFuture<DisplayId> changeFuture_;
};

class ScreenListener : public ScreenManager::IScreenListener {
public:
    virtual void OnConnect(ScreenId) override;
    virtual void OnDisconnect(ScreenId) override;
    virtual void OnChange(ScreenId) override;
    RunnableFuture<ScreenId> changeFuture_;
};

class WindowRotationTest : public testing::Test {
public:
    static void SetUpTestCase();
    static void TearDownTestCase();
    virtual void SetUp() override;
    virtual void TearDown() override;
    std::vector<sptr<Window>> activeWindows_;
    Utils::TestWindowInfo fullInfo_;
    sptr<DisplayListener> displayListener_;
    sptr<ScreenListener> screenListener_;
private:
    static constexpr uint32_t SPLIT_TEST_SLEEP_S = 1;
    static constexpr long FUTURE_GET_RESULT_TIMEOUT = 1000;
};

void DisplayListener::OnCreate(DisplayId displayId)
{
}

void DisplayListener::OnDestroy(DisplayId displayId)
{
}

void DisplayListener::OnChange(DisplayId displayId)
{
    changeFuture_.SetValue(displayId);
}

void ScreenListener::OnConnect(ScreenId screenId)
{
}

void ScreenListener::OnDisconnect(ScreenId screenId)
{
}

void ScreenListener::OnChange(ScreenId screenId)
{
    changeFuture_.SetValue(screenId);
}

void WindowRotationTest::SetUpTestCase()
{
}

void WindowRotationTest::TearDownTestCase()
{
}

void WindowRotationTest::SetUp()
{
    fullInfo_ = {
            .name = "",
            .rect = Utils::customAppRect_,
            .type = WindowType::WINDOW_TYPE_APP_MAIN_WINDOW,
            .mode = WindowMode::WINDOW_MODE_FULLSCREEN,
            .needAvoid = true,
            .parentLimit = false,
            .parentId = INVALID_WINDOW_ID,
    };

    activeWindows_.clear();
    displayListener_ = new DisplayListener();
    DisplayManager::GetInstance().RegisterDisplayListener(displayListener_);
    screenListener_ = new ScreenListener();
    ScreenManager::GetInstance().RegisterScreenListener(screenListener_);
}

void WindowRotationTest::TearDown()
{
    while (!activeWindows_.empty()) {
        ASSERT_EQ(WMError::WM_OK, activeWindows_.back()->Destroy());
        activeWindows_.pop_back();
    }
    DisplayManager::GetInstance().UnregisterDisplayListener(displayListener_);
    ScreenManager::GetInstance().UnregisterScreenListener(screenListener_);
}

namespace {
/**
* @tc.name: WindowRotationTest1
* @tc.desc: create window and SetRequestedOrientation.
* @tc.type: FUNC
*/
HWTEST_F(WindowRotationTest, WindowRotationTest1, Function | MediumTest | Level3)
{
    fullInfo_.name  = "fullscreen.1";
    fullInfo_.orientation_ = Orientation::UNSPECIFIED;
    const sptr<Window>& fullWindow = Utils::CreateTestWindow(fullInfo_);
    activeWindows_.push_back(fullWindow);
    ASSERT_EQ(WMError::WM_OK, fullWindow->Show());
    ASSERT_EQ(WindowMode::WINDOW_MODE_FULLSCREEN, fullWindow->GetMode());
    sleep(SPLIT_TEST_SLEEP_S);

    fullWindow->SetRequestedOrientation(Orientation::REVERSE_HORIZONTAL);
    ASSERT_EQ(Orientation::REVERSE_HORIZONTAL, fullWindow->GetRequestedOrientation());
    DisplayId displayId = displayListener_->changeFuture_.GetResult(FUTURE_GET_RESULT_TIMEOUT);
    displayListener_->changeFuture_.Reset(-1);
    ScreenId screenId = screenListener_->changeFuture_.GetResult(FUTURE_GET_RESULT_TIMEOUT);
    screenListener_->changeFuture_.Reset(-1);
    auto screen = ScreenManager::GetInstance().GetScreenById(screenId);
    auto display = DisplayManager::GetInstance().GetDisplayById(displayId);
    ASSERT_EQ(Orientation::REVERSE_HORIZONTAL, screen->GetOrientation());
    ASSERT_EQ(Orientation::REVERSE_HORIZONTAL, display->GetOrientation());
    sleep(SPLIT_TEST_SLEEP_S);

    ASSERT_EQ(WMError::WM_OK, fullWindow->Hide());
    sleep(SPLIT_TEST_SLEEP_S);
}

/**
* @tc.name: WindowRotationTest2
* @tc.desc: create window with orientation property.
* @tc.type: FUNC
*/
HWTEST_F(WindowRotationTest, WindowRotationTest2, Function | MediumTest | Level3)
{
    fullInfo_.name  = "fullscreen.2";
    fullInfo_.orientation_ = Orientation::REVERSE_HORIZONTAL;
    const sptr<Window>& fullWindow = Utils::CreateTestWindow(fullInfo_);
    activeWindows_.push_back(fullWindow);
    ASSERT_EQ(WMError::WM_OK, fullWindow->Show());
    ASSERT_EQ(WindowMode::WINDOW_MODE_FULLSCREEN, fullWindow->GetMode());

    ASSERT_EQ(Orientation::REVERSE_HORIZONTAL, fullWindow->GetRequestedOrientation());
    DisplayId displayId = displayListener_->changeFuture_.GetResult(FUTURE_GET_RESULT_TIMEOUT);
    ScreenId screenId = screenListener_->changeFuture_.GetResult(FUTURE_GET_RESULT_TIMEOUT);
    auto screen = ScreenManager::GetInstance().GetScreenById(screenId);
    auto display = DisplayManager::GetInstance().GetDisplayById(displayId);
    ASSERT_EQ(Orientation::REVERSE_HORIZONTAL, screen->GetOrientation());
    ASSERT_EQ(Orientation::REVERSE_HORIZONTAL, display->GetOrientation());
    sleep(SPLIT_TEST_SLEEP_S);

    ASSERT_EQ(WMError::WM_OK, fullWindow->Hide());
    screen->SetOrientation(Orientation::UNSPECIFIED);
    sleep(SPLIT_TEST_SLEEP_S);
}

/**
* @tc.name: WindowRotationTest3
* @tc.desc: create floating window with orientation property
* @tc.type: FUNC
*/
HWTEST_F(WindowRotationTest, WindowRotationTest3, Function | MediumTest | Level3)
{
    fullInfo_.name  = "fullscreen.4";
    fullInfo_.orientation_ = Orientation::REVERSE_HORIZONTAL;
    fullInfo_.mode = WindowMode::WINDOW_MODE_FLOATING;
    const sptr<Window>& fullWindow = Utils::CreateTestWindow(fullInfo_);
    fullInfo_.mode = WindowMode::WINDOW_MODE_FULLSCREEN;
    activeWindows_.push_back(fullWindow);
    ASSERT_EQ(WMError::WM_OK, fullWindow->Show());
    ASSERT_EQ(WindowMode::WINDOW_MODE_FLOATING, fullWindow->GetMode());
    sleep(SPLIT_TEST_SLEEP_S);

    ASSERT_EQ(Orientation::REVERSE_HORIZONTAL, fullWindow->GetRequestedOrientation());
    auto display = DisplayManager::GetInstance().GetDefaultDisplay();
    auto screen = ScreenManager::GetInstance().GetAllScreens()[0];
    ASSERT_EQ(Orientation::UNSPECIFIED, screen->GetOrientation());
    ASSERT_EQ(Orientation::UNSPECIFIED, display->GetOrientation());
    sleep(SPLIT_TEST_SLEEP_S);

    ASSERT_EQ(WMError::WM_OK, fullWindow->Hide());
    ASSERT_EQ(Orientation::UNSPECIFIED, screen->GetOrientation());
    ASSERT_EQ(Orientation::UNSPECIFIED, display->GetOrientation());
    sleep(SPLIT_TEST_SLEEP_S);
}

/**
* @tc.name: WindowRotationTest4
* @tc.desc: create window with orientation after setting screen default orientation.
* @tc.type: FUNC
*/
HWTEST_F(WindowRotationTest, WindowRotationTest4, Function | MediumTest | Level3)
{
    ScreenId defaultScreenId = DisplayManager::GetInstance().GetDefaultDisplay()->GetScreenId();
    auto defaultScreen = ScreenManager::GetInstance().GetScreenById(defaultScreenId);
    defaultScreen->SetOrientation(Orientation::REVERSE_HORIZONTAL);
    fullInfo_.name  = "fullscreen.5";
    fullInfo_.orientation_ = Orientation::HORIZONTAL;
    const sptr<Window>& fullWindow = Utils::CreateTestWindow(fullInfo_);
    activeWindows_.push_back(fullWindow);
    ASSERT_EQ(WMError::WM_OK, fullWindow->Show());
    ASSERT_EQ(WindowMode::WINDOW_MODE_FULLSCREEN, fullWindow->GetMode());

    ASSERT_EQ(Orientation::HORIZONTAL, fullWindow->GetRequestedOrientation());
    DisplayId displayId = displayListener_->changeFuture_.GetResult(FUTURE_GET_RESULT_TIMEOUT);
    displayListener_->changeFuture_.Reset(-1);
    ScreenId screenId = screenListener_->changeFuture_.GetResult(FUTURE_GET_RESULT_TIMEOUT);
    screenListener_->changeFuture_.Reset(-1);
    auto screen = ScreenManager::GetInstance().GetScreenById(screenId);
    auto display = DisplayManager::GetInstance().GetDisplayById(displayId);
    ASSERT_EQ(Orientation::HORIZONTAL, screen->GetOrientation());
    ASSERT_EQ(Orientation::HORIZONTAL, display->GetOrientation());
    sleep(SPLIT_TEST_SLEEP_S);

    ASSERT_EQ(WMError::WM_OK, fullWindow->Hide());
    defaultScreen->SetOrientation(Orientation::UNSPECIFIED);
    sleep(SPLIT_TEST_SLEEP_S);
}

/**
* @tc.name: WindowRotationTest5
* @tc.desc: create window with orientation after setting screen default orientation, and toggle shown state for all app
*           windows.
* @tc.type: FUNC
*/
HWTEST_F(WindowRotationTest, WindowRotationTest5, Function | MediumTest | Level3)
{
    ScreenId defaultScreenId = DisplayManager::GetInstance().GetDefaultDisplay()->GetScreenId();
    auto defaultScreen = ScreenManager::GetInstance().GetScreenById(defaultScreenId);
    defaultScreen->SetOrientation(Orientation::REVERSE_HORIZONTAL);
    fullInfo_.name  = "fullscreen.5";
    fullInfo_.orientation_ = Orientation::HORIZONTAL;
    const sptr<Window>& fullWindow = Utils::CreateTestWindow(fullInfo_);
    activeWindows_.push_back(fullWindow);
    ASSERT_EQ(WMError::WM_OK, fullWindow->Show());
    ASSERT_EQ(WindowMode::WINDOW_MODE_FULLSCREEN, fullWindow->GetMode());

    ASSERT_EQ(Orientation::HORIZONTAL, fullWindow->GetRequestedOrientation());
    DisplayId displayId = displayListener_->changeFuture_.GetResult(FUTURE_GET_RESULT_TIMEOUT);
    displayListener_->changeFuture_.Reset(-1);
    ScreenId screenId = screenListener_->changeFuture_.GetResult(FUTURE_GET_RESULT_TIMEOUT);
    screenListener_->changeFuture_.Reset(-1);
    auto screen = ScreenManager::GetInstance().GetScreenById(screenId);
    auto display = DisplayManager::GetInstance().GetDisplayById(displayId);
    ASSERT_EQ(Orientation::HORIZONTAL, screen->GetOrientation());
    ASSERT_EQ(Orientation::HORIZONTAL, display->GetOrientation());
    sleep(SPLIT_TEST_SLEEP_S);

    WindowManager::GetInstance().ToggleShownStateForAllAppWindows();
    ASSERT_EQ(WMError::WM_OK, fullWindow->Hide());
    sleep(SPLIT_TEST_SLEEP_S);

    WindowManager::GetInstance().ToggleShownStateForAllAppWindows();
    sleep(SPLIT_TEST_SLEEP_S);

    ASSERT_EQ(WMError::WM_OK, fullWindow->Hide());
    defaultScreen->SetOrientation(Orientation::UNSPECIFIED);
    sleep(SPLIT_TEST_SLEEP_S);
}

/**
* @tc.name: WindowRotationTest6
* @tc.desc: test window rotation when display is zoomed.
* @tc.type: FUNC
* @tc.require: issueI5NGWL
*/
HWTEST_F(WindowRotationTest, WindowRotationTest6, Function | MediumTest | Level3)
{
    WindowAccessibilityController::GetInstance().SetAnchorAndScale(0, 0, 2);
    sleep(SPLIT_TEST_SLEEP_S);

    fullInfo_.name  = "fullscreen.2";
    fullInfo_.orientation_ = Orientation::REVERSE_HORIZONTAL;
    const sptr<Window>& fullWindow = Utils::CreateTestWindow(fullInfo_);
    activeWindows_.push_back(fullWindow);
    ASSERT_EQ(WMError::WM_OK, fullWindow->Show());

    DisplayId displayId = displayListener_->changeFuture_.GetResult(FUTURE_GET_RESULT_TIMEOUT);
    ScreenId screenId = screenListener_->changeFuture_.GetResult(FUTURE_GET_RESULT_TIMEOUT);
    auto screen = ScreenManager::GetInstance().GetScreenById(screenId);
    auto display = DisplayManager::GetInstance().GetDisplayById(displayId);
    ASSERT_EQ(Orientation::REVERSE_HORIZONTAL, screen->GetOrientation());
    ASSERT_EQ(Orientation::REVERSE_HORIZONTAL, display->GetOrientation());
    sleep(SPLIT_TEST_SLEEP_S);

    Window* ptr = fullWindow.GetRefPtr();
    WindowImpl* implPtr = (WindowImpl*)ptr;
    Transform expect;
    ASSERT_NE(expect, implPtr->GetWindowProperty()->GetZoomTransform());

    WindowAccessibilityController::GetInstance().OffWindowZoom();
    ASSERT_EQ(WMError::WM_OK, fullWindow->Hide());
    screen->SetOrientation(Orientation::UNSPECIFIED);
    sleep(SPLIT_TEST_SLEEP_S);
}
}
} // namespace Rosen
} // namespace OHOS
