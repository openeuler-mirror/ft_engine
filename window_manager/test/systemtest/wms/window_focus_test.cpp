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
#include "wm_common.h"
#include "window_manager.h"
#include "window_test_utils.h"
using namespace testing;
using namespace testing::ext;

namespace OHOS {
namespace Rosen {
namespace {
    constexpr HiviewDFX::HiLogLabel LABEL = {LOG_CORE, HILOG_DOMAIN_WINDOW, "WindowFocusTest"};
}

using Utils = WindowTestUtils;
const int WAIT_ASYNC_US = 100000;  // 100000us

class TestFocusChangedListener : public IFocusChangedListener {
public:
    uint32_t focusedWindow_ = INVALID_WINDOW_ID;
    uint32_t unfocusedWindow_ = INVALID_WINDOW_ID;
    void OnFocused(const sptr<FocusChangeInfo>& focusChangeInfo) override;
    void OnUnfocused(const sptr<FocusChangeInfo>& focusChangeInfo) override;
};

class WindowFocusTest : public testing::Test {
public:
    static void SetUpTestCase();
    static void TearDownTestCase();
    virtual void SetUp() override;
    virtual void TearDown() override;
    static sptr<TestFocusChangedListener> testFocusChangedListener_;
    Utils::TestWindowInfo fullScreenAppInfo_;
    Utils::TestWindowInfo floatAppInfo_;
    Utils::TestWindowInfo subAppInfo_;
};

sptr<TestFocusChangedListener> WindowFocusTest::testFocusChangedListener_ =
    new TestFocusChangedListener();

void TestFocusChangedListener::OnFocused(const sptr<FocusChangeInfo>& focusChangeInfo)
{
    WLOGFI("TestFocusChangedListener Focused ID: %{public}u", focusChangeInfo->windowId_);
    focusedWindow_ = focusChangeInfo->windowId_;
}

void TestFocusChangedListener::OnUnfocused(const sptr<FocusChangeInfo>& focusChangeInfo)
{
    WLOGFI("TestFocusChangedListener Unfocused ID: %{public}u", focusChangeInfo->windowId_);
    unfocusedWindow_ = focusChangeInfo->windowId_;
}

void WindowFocusTest::SetUpTestCase()
{
    auto display = DisplayManager::GetInstance().GetDisplayById(0);
    ASSERT_TRUE((display != nullptr));
    WLOGFI("GetDefaultDisplay: id %{public}" PRIu64", w %{public}d, h %{public}d, fps %{public}u",
        display->GetId(), display->GetWidth(), display->GetHeight(), display->GetRefreshRate());
    Rect displayRect = {0, 0, display->GetWidth(), display->GetHeight()};
    Utils::InitByDisplayRect(displayRect);
}

void WindowFocusTest::TearDownTestCase()
{
}

void WindowFocusTest::SetUp()
{
    fullScreenAppInfo_ = {
        .name = "FullWindow",
        .rect = Utils::customAppRect_,
        .type = WindowType::WINDOW_TYPE_APP_MAIN_WINDOW,
        .mode = WindowMode::WINDOW_MODE_FULLSCREEN,
        .needAvoid = false,
        .parentLimit = false,
        .showWhenLocked = true,
        .parentId = INVALID_WINDOW_ID,
    };
    floatAppInfo_ = {
        .name = "ParentWindow",
        .rect = Utils::customAppRect_,
        .type = WindowType::WINDOW_TYPE_APP_MAIN_WINDOW,
        .mode = WindowMode::WINDOW_MODE_FLOATING,
        .needAvoid = false,
        .parentLimit = false,
        .showWhenLocked = true,
        .parentId = INVALID_WINDOW_ID,
    };
    subAppInfo_ = {
        .name = "SubWindow",
        .rect = Utils::customAppRect_,
        .type = WindowType::WINDOW_TYPE_APP_SUB_WINDOW,
        .mode = WindowMode::WINDOW_MODE_FLOATING,
        .needAvoid = false,
        .parentLimit = false,
        .showWhenLocked = false,
        .parentId = INVALID_WINDOW_ID,
    };
    WindowManager::GetInstance().RegisterFocusChangedListener(testFocusChangedListener_);
}

void WindowFocusTest::TearDown()
{
    WindowManager::GetInstance().UnregisterFocusChangedListener(testFocusChangedListener_);
}

namespace {
/**
 * @tc.name: FocusChangedTest01
 * @tc.desc: add main window and sub window and show it to test focus
 * @tc.type: FUNC
 */
HWTEST_F(WindowFocusTest, FocusChangedTest01, Function | MediumTest | Level3)
{
    fullScreenAppInfo_.name = "FocusChangedTest01_1";
    fullScreenAppInfo_.focusable_ = false;
    const sptr<Window>& window1 = Utils::CreateTestWindow(fullScreenAppInfo_);
    ASSERT_NE(nullptr, window1);

    floatAppInfo_.name = "FocusChangedTest01_2";
    floatAppInfo_.rect = { 10, 200, 300, 400 };
    const sptr<Window>& window2 = Utils::CreateTestWindow(floatAppInfo_);
    ASSERT_NE(nullptr, window2);

    floatAppInfo_.name = "FocusChangedTest01_3";
    floatAppInfo_.rect = { 250, 150, 300, 500 };
    const sptr<Window>& window3 = Utils::CreateTestWindow(floatAppInfo_);
    ASSERT_NE(nullptr, window3);

    subAppInfo_.name = "FocusChangedTest01_4";
    subAppInfo_.rect = { 400, 200, 100, 100 };
    subAppInfo_.parentId = window3->GetWindowId();
    const sptr<Window>& subWindow = Utils::CreateTestWindow(subAppInfo_);

    ASSERT_EQ(WMError::WM_OK, window3->Show());
    // Await 100ms and get callback result in listener.
    usleep(WAIT_ASYNC_US);
    ASSERT_EQ(window3->GetWindowId(), testFocusChangedListener_->focusedWindow_);

    ASSERT_EQ(WMError::WM_OK, window1->Show());
    // Await 100ms and get callback result in listener.
    usleep(WAIT_ASYNC_US);
    ASSERT_EQ(window3->GetWindowId(), testFocusChangedListener_->focusedWindow_);

    ASSERT_EQ(WMError::WM_OK, window2->Show());
    // Await 100ms and get callback result in listener.
    usleep(WAIT_ASYNC_US);
    ASSERT_EQ(window3->GetWindowId(), testFocusChangedListener_->unfocusedWindow_);
    ASSERT_EQ(window2->GetWindowId(), testFocusChangedListener_->focusedWindow_);

    ASSERT_EQ(WMError::WM_OK, subWindow->Show());
    // Await 100ms and get callback result in listener.
    usleep(WAIT_ASYNC_US);
    ASSERT_EQ(window2->GetWindowId(), testFocusChangedListener_->unfocusedWindow_);
    ASSERT_EQ(subWindow->GetWindowId(), testFocusChangedListener_->focusedWindow_);

    window1->Destroy();
    window2->Destroy();
    window3->Destroy();
    subWindow->Destroy();
}

/**
 * @tc.name: FocusChangedTest02
 * @tc.desc: hide focused window to test focus
 * @tc.type: FUNC
 */
HWTEST_F(WindowFocusTest, FocusChangedTest02, Function | MediumTest | Level3)
{
    floatAppInfo_.name = "FocusChangedTest02_1";
    floatAppInfo_.rect = { 10, 200, 300, 400 };
    const sptr<Window>& mainWindow = Utils::CreateTestWindow(floatAppInfo_);
    ASSERT_NE(nullptr, mainWindow);
    ASSERT_EQ(WMError::WM_OK, mainWindow->Show());

    subAppInfo_.name = "FocusChangedTest02_2";
    subAppInfo_.rect = { 400, 200, 100, 100 };
    subAppInfo_.parentId = mainWindow->GetWindowId();
    const sptr<Window>& subWindow = Utils::CreateTestWindow(subAppInfo_);
    ASSERT_NE(nullptr, subWindow);
    ASSERT_EQ(WMError::WM_OK, subWindow->Show());
    usleep(WAIT_ASYNC_US);
    ASSERT_EQ(subWindow->GetWindowId(), testFocusChangedListener_->focusedWindow_);

    ASSERT_EQ(WMError::WM_OK, subWindow->Hide());
    // Await 100ms and get callback result in listener.
    usleep(WAIT_ASYNC_US);
    ASSERT_EQ(mainWindow->GetWindowId(), testFocusChangedListener_->focusedWindow_);

    mainWindow->Destroy();
    subWindow->Destroy();
}

/**
 * @tc.name: FocusChangedTest03
 * @tc.desc: hide focused window to test focus
 * @tc.type: FUNC
 */
HWTEST_F(WindowFocusTest, FocusChangedTest03, Function | MediumTest | Level3)
{
    floatAppInfo_.name = "FocusChangedTest03_1";
    floatAppInfo_.rect = { 10, 200, 300, 400 };
    const sptr<Window>& mainWindow1 = Utils::CreateTestWindow(floatAppInfo_);
    ASSERT_NE(nullptr, mainWindow1);
    ASSERT_EQ(WMError::WM_OK, mainWindow1->Show());

    subAppInfo_.name = "FocusChangedTest03_2";
    subAppInfo_.rect = { 400, 200, 100, 100 };
    subAppInfo_.parentId = mainWindow1->GetWindowId();
    const sptr<Window>& aboveSubWindow = Utils::CreateTestWindow(subAppInfo_);
    ASSERT_NE(nullptr, aboveSubWindow);
    ASSERT_EQ(WMError::WM_OK, aboveSubWindow->Show());

    floatAppInfo_.name = "FocusChangedTest03_3";
    floatAppInfo_.rect = { 200, 200, 100, 100 };
    const sptr<Window>& mainWindow2 = Utils::CreateTestWindow(floatAppInfo_);
    ASSERT_NE(nullptr, mainWindow2);
    ASSERT_EQ(WMError::WM_OK, mainWindow2->Show());
    usleep(WAIT_ASYNC_US);
    ASSERT_EQ(mainWindow2->GetWindowId(), testFocusChangedListener_->focusedWindow_);

    ASSERT_EQ(WMError::WM_OK, mainWindow2->Hide());
    // Await 100ms and get callback result in listener.
    usleep(WAIT_ASYNC_US);
    ASSERT_EQ(mainWindow2->GetWindowId(), testFocusChangedListener_->unfocusedWindow_);
    ASSERT_EQ(aboveSubWindow->GetWindowId(), testFocusChangedListener_->focusedWindow_);

    mainWindow1->Destroy();
    mainWindow2->Destroy();
    aboveSubWindow->Destroy();
}

/**
 * @tc.name: FocusChangedTest04
 * @tc.desc: hide focused window to test focus
 * @tc.type: FUNC
 */
HWTEST_F(WindowFocusTest, FocusChangedTest04, Function | MediumTest | Level3)
{
    floatAppInfo_.name = "FocusChangedTest04_1";
    floatAppInfo_.rect = { 10, 200, 300, 400 };
    const sptr<Window>& mainWindow1 = Utils::CreateTestWindow(floatAppInfo_);
    ASSERT_NE(nullptr, mainWindow1);
    ASSERT_EQ(WMError::WM_OK, mainWindow1->Show());

    subAppInfo_.name = "FocusChangedTest04_2";
    subAppInfo_.rect = { 400, 200, 100, 100 };
    subAppInfo_.parentId = mainWindow1->GetWindowId();
    subAppInfo_.type = WindowType::WINDOW_TYPE_MEDIA;
    const sptr<Window>& belowSubWindow = Utils::CreateTestWindow(subAppInfo_);
    ASSERT_NE(nullptr, belowSubWindow);
    ASSERT_EQ(WMError::WM_OK, belowSubWindow->Show());

    floatAppInfo_.name = "FocusChangedTest04_3";
    floatAppInfo_.rect = { 200, 200, 100, 100 };
    const sptr<Window>& mainWindow2 = Utils::CreateTestWindow(floatAppInfo_);
    ASSERT_NE(nullptr, mainWindow2);
    ASSERT_EQ(WMError::WM_OK, mainWindow2->Show());
    usleep(WAIT_ASYNC_US);
    ASSERT_EQ(mainWindow2->GetWindowId(), testFocusChangedListener_->focusedWindow_);

    ASSERT_EQ(WMError::WM_OK, mainWindow2->Hide());
    // Await 100ms and get callback result in listener.
    usleep(WAIT_ASYNC_US);
    ASSERT_EQ(mainWindow2->GetWindowId(), testFocusChangedListener_->unfocusedWindow_);
    ASSERT_EQ(mainWindow1->GetWindowId(), testFocusChangedListener_->focusedWindow_);

    mainWindow1->Destroy();
    mainWindow2->Destroy();
    belowSubWindow->Destroy();
}

/**
 * @tc.name: FocusChangedTest05
 * @tc.desc: hide focused window to test focus
 * @tc.type: FUNC
 */
HWTEST_F(WindowFocusTest, FocusChangedTest05, Function | MediumTest | Level3)
{
    floatAppInfo_.name = "FocusChangedTest05_1";
    floatAppInfo_.rect = { 10, 200, 300, 400 };
    const sptr<Window>& mainWindow1 = Utils::CreateTestWindow(floatAppInfo_);
    ASSERT_NE(nullptr, mainWindow1);
    ASSERT_EQ(WMError::WM_OK, mainWindow1->Show());

    floatAppInfo_.name = "FocusChangedTest05_2";
    floatAppInfo_.rect = { 200, 200, 100, 100 };
    const sptr<Window>& mainWindow2 = Utils::CreateTestWindow(floatAppInfo_);
    ASSERT_NE(nullptr, mainWindow2);
    ASSERT_EQ(WMError::WM_OK, mainWindow2->Show());

    subAppInfo_.name = "FocusChangedTest05_3";
    subAppInfo_.rect = { 400, 200, 100, 100 };
    subAppInfo_.parentId = mainWindow2->GetWindowId();
    subAppInfo_.type = WindowType::WINDOW_TYPE_MEDIA;
    const sptr<Window>& belowSubWindow = Utils::CreateTestWindow(subAppInfo_);
    ASSERT_NE(nullptr, belowSubWindow);
    ASSERT_EQ(WMError::WM_OK, belowSubWindow->Show());
    usleep(WAIT_ASYNC_US);
    ASSERT_EQ(belowSubWindow->GetWindowId(), testFocusChangedListener_->focusedWindow_);

    ASSERT_EQ(WMError::WM_OK, belowSubWindow->Hide());
    // Await 100ms and get callback result in listener.
    usleep(WAIT_ASYNC_US);
    ASSERT_EQ(belowSubWindow->GetWindowId(), testFocusChangedListener_->unfocusedWindow_);
    ASSERT_EQ(mainWindow1->GetWindowId(), testFocusChangedListener_->focusedWindow_);

    ASSERT_EQ(WMError::WM_OK, belowSubWindow->Show());
    usleep(WAIT_ASYNC_US);
    ASSERT_EQ(mainWindow1->GetWindowId(), testFocusChangedListener_->unfocusedWindow_);
    ASSERT_EQ(belowSubWindow->GetWindowId(), testFocusChangedListener_->focusedWindow_);
    ASSERT_EQ(WMError::WM_OK, mainWindow2->Hide());
    usleep(WAIT_ASYNC_US);
    ASSERT_EQ(belowSubWindow->GetWindowId(), testFocusChangedListener_->unfocusedWindow_);
    ASSERT_EQ(mainWindow1->GetWindowId(), testFocusChangedListener_->focusedWindow_);

    mainWindow1->Destroy();
    mainWindow2->Destroy();
    belowSubWindow->Destroy();
}

/**
* @tc.name: FocusChangedTest06
* @tc.desc: hide unfocused window to test focus
* @tc.type: FUNC
*/
HWTEST_F(WindowFocusTest, FocusChangedTest06, Function | MediumTest | Level3)
{
    floatAppInfo_.name = "FocusChangedTest06_1";
    floatAppInfo_.rect = { 10, 200, 300, 400 };
    const sptr<Window>& mainWindow = Utils::CreateTestWindow(floatAppInfo_);
    ASSERT_NE(nullptr, mainWindow);
    ASSERT_EQ(WMError::WM_OK, mainWindow->Show());

    subAppInfo_.name = "FocusChangedTest06_2";
    subAppInfo_.rect = { 100, 200, 100, 100 };
    subAppInfo_.parentId = mainWindow->GetWindowId();
    subAppInfo_.type = WindowType::WINDOW_TYPE_MEDIA;
    const sptr<Window>& belowSubWindow = Utils::CreateTestWindow(subAppInfo_);
    ASSERT_NE(nullptr, belowSubWindow);
    ASSERT_EQ(WMError::WM_OK, belowSubWindow->Show());

    subAppInfo_.name = "FocusChangedTest06_3";
    subAppInfo_.rect = { 400, 200, 100, 100 };
    subAppInfo_.parentId = mainWindow->GetWindowId();
    subAppInfo_.type = WindowType::WINDOW_TYPE_APP_SUB_WINDOW;
    const sptr<Window>& aboveSubWindow = Utils::CreateTestWindow(subAppInfo_);
    ASSERT_NE(nullptr, aboveSubWindow);
    ASSERT_EQ(WMError::WM_OK, aboveSubWindow->Show());
    usleep(WAIT_ASYNC_US);
    ASSERT_EQ(aboveSubWindow->GetWindowId(), testFocusChangedListener_->focusedWindow_);

    ASSERT_EQ(WMError::WM_OK, belowSubWindow->Hide());
    // Await 100ms and get callback result in listener.
    usleep(WAIT_ASYNC_US);
    ASSERT_EQ(aboveSubWindow->GetWindowId(), testFocusChangedListener_->focusedWindow_);
}

/**
* @tc.name: FocusChangedTest07
* @tc.desc: destroy focused window to test focus
* @tc.type: FUNC
*/
HWTEST_F(WindowFocusTest, FocusChangedTest07, Function | MediumTest | Level3)
{
    floatAppInfo_.name = "FocusChangedTest07_1";
    floatAppInfo_.rect = { 10, 200, 300, 400 };
    const sptr<Window>& mainWindow1 = Utils::CreateTestWindow(floatAppInfo_);
    ASSERT_NE(nullptr, mainWindow1);
    ASSERT_EQ(WMError::WM_OK, mainWindow1->Show());

    floatAppInfo_.name = "FocusChangedTest07_2";
    floatAppInfo_.rect = { 250, 150, 300, 500 };
    const sptr<Window>& mainWindow2 = Utils::CreateTestWindow(floatAppInfo_);
    ASSERT_NE(nullptr, mainWindow2);
    ASSERT_EQ(WMError::WM_OK, mainWindow2->Show());

    floatAppInfo_.name = "FocusChangedTest07_3";
    floatAppInfo_.rect = { 300, 400, 10, 400 };
    const sptr<Window>& mainWindow3 = Utils::CreateTestWindow(floatAppInfo_);
    ASSERT_NE(nullptr, mainWindow3);
    ASSERT_EQ(WMError::WM_OK, mainWindow3->Show());

    subAppInfo_.name = "FocusChangedTest07_4";
    subAppInfo_.rect = { 20, 100, 100, 100 };
    subAppInfo_.parentId = mainWindow1->GetWindowId();
    subAppInfo_.type = WindowType::WINDOW_TYPE_MEDIA;
    const sptr<Window>& belowSubWindow1 = Utils::CreateTestWindow(subAppInfo_);
    ASSERT_NE(nullptr, belowSubWindow1);

    subAppInfo_.name = "FocusChangedTest07_5";
    subAppInfo_.rect = { 400, 200, 100, 100 };
    subAppInfo_.parentId = mainWindow2->GetWindowId();
    subAppInfo_.type = WindowType::WINDOW_TYPE_APP_SUB_WINDOW;
    const sptr<Window>& aboveSubWindow = Utils::CreateTestWindow(subAppInfo_);
    ASSERT_NE(nullptr, aboveSubWindow);

    subAppInfo_.name = "FocusChangedTest07_6";
    subAppInfo_.rect = { 310, 410, 100, 100 };
    subAppInfo_.parentId = mainWindow3->GetWindowId();
    subAppInfo_.type = WindowType::WINDOW_TYPE_MEDIA;
    const sptr<Window>& belowSubWindow2 = Utils::CreateTestWindow(subAppInfo_);
    ASSERT_NE(nullptr, belowSubWindow2);

    ASSERT_EQ(WMError::WM_OK, aboveSubWindow->Show());
    ASSERT_EQ(WMError::WM_OK, belowSubWindow2->Show());
    ASSERT_EQ(WMError::WM_OK, belowSubWindow1->Show());
    usleep(WAIT_ASYNC_US);
    ASSERT_EQ(belowSubWindow1->GetWindowId(), testFocusChangedListener_->focusedWindow_);

    ASSERT_EQ(WMError::WM_OK, mainWindow1->Destroy());
    // Await 100ms and get callback result in listener.
    usleep(WAIT_ASYNC_US);
    ASSERT_EQ(belowSubWindow1->GetWindowId(), testFocusChangedListener_->unfocusedWindow_);
    ASSERT_EQ(mainWindow3->GetWindowId(), testFocusChangedListener_->focusedWindow_);

    ASSERT_EQ(WMError::WM_OK, mainWindow3->Destroy());
    usleep(WAIT_ASYNC_US);
    ASSERT_EQ(mainWindow3->GetWindowId(), testFocusChangedListener_->unfocusedWindow_);
    ASSERT_EQ(aboveSubWindow->GetWindowId(), testFocusChangedListener_->focusedWindow_);

    ASSERT_EQ(WMError::WM_OK, aboveSubWindow->Destroy());
    usleep(WAIT_ASYNC_US);
    ASSERT_EQ(aboveSubWindow->GetWindowId(), testFocusChangedListener_->unfocusedWindow_);
    ASSERT_EQ(mainWindow2->GetWindowId(), testFocusChangedListener_->focusedWindow_);

    mainWindow2->Destroy();
}

/**
* @tc.name: FocusChangedTest08
* @tc.desc: destroy unfocused window to test focus
* @tc.type: FUNC
*/
HWTEST_F(WindowFocusTest, FocusChangedTest08, Function | MediumTest | Level3)
{
    floatAppInfo_.name = "FocusChangedTest08_1";
    floatAppInfo_.rect = { 10, 200, 300, 400 };
    const sptr<Window>& mainWindow1 = Utils::CreateTestWindow(floatAppInfo_);
    ASSERT_NE(nullptr, mainWindow1);
    ASSERT_EQ(WMError::WM_OK, mainWindow1->Show());

    floatAppInfo_.name = "FocusChangedTest08_2";
    floatAppInfo_.rect = { 250, 150, 300, 500 };
    const sptr<Window>& mainWindow2 = Utils::CreateTestWindow(floatAppInfo_);
    ASSERT_NE(nullptr, mainWindow2);
    ASSERT_EQ(WMError::WM_OK, mainWindow2->Show());

    subAppInfo_.name = "FocusChangedTest08_3";
    subAppInfo_.rect = { 20, 100, 100, 100 };
    subAppInfo_.parentId = mainWindow1->GetWindowId();
    subAppInfo_.type = WindowType::WINDOW_TYPE_MEDIA;
    const sptr<Window>& belowSubWindow = Utils::CreateTestWindow(subAppInfo_);
    ASSERT_NE(nullptr, belowSubWindow);
    ASSERT_EQ(WMError::WM_OK, belowSubWindow->Show());
    usleep(WAIT_ASYNC_US);
    ASSERT_EQ(belowSubWindow->GetWindowId(), testFocusChangedListener_->focusedWindow_);

    ASSERT_EQ(WMError::WM_OK, mainWindow2->Destroy());
    // Await 100ms and get callback result in listener.
    usleep(WAIT_ASYNC_US);
    ASSERT_EQ(belowSubWindow->GetWindowId(), testFocusChangedListener_->focusedWindow_);

    mainWindow1->Destroy();
}
}
} // namespace Rosen
} // namespace OHOS
