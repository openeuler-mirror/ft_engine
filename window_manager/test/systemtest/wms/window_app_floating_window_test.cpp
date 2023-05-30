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
#include "ability_context_impl.h"
#include "ipc_skeleton.h"
#include "window.h"
#include "window_manager.h"
#include "window_option.h"
#include "window_scene.h"
#include "window_test_utils.h"
#include "wm_common.h"

using namespace testing;
using namespace testing::ext;

namespace OHOS {
namespace Rosen {
namespace {
    constexpr HiviewDFX::HiLogLabel LABEL = {LOG_CORE, HILOG_DOMAIN_WINDOW, "WindowAppFloatingWindowTest"};
}

class TestCameraFloatWindowChangedListener : public ICameraFloatWindowChangedListener {
public:
    uint32_t accessTokenId_ = 0;
    bool isShowing_ = false;
    void OnCameraFloatWindowChange(uint32_t accessTokenId, bool isShowing) override;
};

class WindowAppFloatingWindowTest : public testing::Test {
public:
    static void SetUpTestCase();
    static void TearDownTestCase();
    virtual void SetUp() override;
    virtual void TearDown() override;

    static inline float virtualPixelRatio_ = 1.0;
    static inline Rect displayRect_ {0, 0, 0, 0};
    static inline std::shared_ptr<AbilityRuntime::AbilityContext> abilityContext_ = nullptr;
    static sptr<TestCameraFloatWindowChangedListener> testCameraFloatWindowChangedListener_;
};

sptr<TestCameraFloatWindowChangedListener> WindowAppFloatingWindowTest::testCameraFloatWindowChangedListener_ =
    new TestCameraFloatWindowChangedListener();

void TestCameraFloatWindowChangedListener::OnCameraFloatWindowChange(uint32_t accessTokenId, bool isShowing)
{
    WLOGFI("TestCameraFloatWindowChangedListener [%{public}u, %{public}u]", accessTokenId, isShowing);
    accessTokenId_ = accessTokenId;
    isShowing_ = isShowing;
}

void WindowAppFloatingWindowTest::SetUpTestCase()
{
    auto display = DisplayManager::GetInstance().GetDisplayById(0);
    ASSERT_TRUE((display != nullptr));
    displayRect_.width_ = display->GetWidth();
    displayRect_.height_ = display->GetHeight();
    WindowTestUtils::InitByDisplayRect(displayRect_);
    virtualPixelRatio_ = WindowTestUtils::GetVirtualPixelRatio(0);
}

void WindowAppFloatingWindowTest::TearDownTestCase()
{
}

void WindowAppFloatingWindowTest::SetUp()
{
}

void WindowAppFloatingWindowTest::TearDown()
{
}

static sptr<WindowScene> CreateWindowScene()
{
    sptr<IWindowLifeCycle> listener = nullptr;
    WindowAppFloatingWindowTest::abilityContext_ = std::make_shared<AbilityRuntime::AbilityContextImpl>();

    sptr<WindowScene> scene = new WindowScene();
    scene->Init(0, WindowAppFloatingWindowTest::abilityContext_, listener);
    return scene;
}

static sptr<Window> CreateAppFloatingWindow(WindowType type, Rect rect, std::string name = "")
{
    sptr<WindowOption> option = new WindowOption();
    option->SetWindowType(type);
    option->SetWindowRect(rect);

    static int cnt = 0;
    std::string winName = (name == "") ? "FloatingWindowTest" + std::to_string(cnt++) : name;

    return Window::Create(winName, option, WindowAppFloatingWindowTest::abilityContext_);
}

static inline Rect GetRectWithVpr(int32_t x, int32_t y, uint32_t w, uint32_t h)
{
    auto vpr = WindowAppFloatingWindowTest::virtualPixelRatio_;
    return {x, y, static_cast<uint32_t>(w * vpr), static_cast<uint32_t>(h * vpr)};
}

/**
 * @tc.name: AppFloatingWindow01
 * @tc.desc: AppFloatingWindow life cycle
 * @tc.type: FUNC
 */
HWTEST_F(WindowAppFloatingWindowTest, AppFloatingWindow01, Function | MediumTest | Level2)
{
    sptr<WindowScene> scene = CreateWindowScene();
    ASSERT_NE(nullptr, scene);

    Rect fltWindRect = GetRectWithVpr(0, 0, 400, 600);
    sptr<Window> fltWin = CreateAppFloatingWindow(WindowType::WINDOW_TYPE_FLOAT, fltWindRect);
    ASSERT_NE(nullptr, fltWin);

    ASSERT_EQ(WMError::WM_OK, scene->GoForeground());
    ASSERT_EQ(WMError::WM_OK, fltWin->Show());

    ASSERT_EQ(WMError::WM_OK, fltWin->Hide());
    ASSERT_EQ(WMError::WM_OK, scene->GoBackground());

    ASSERT_EQ(WMError::WM_OK, fltWin->Destroy());
    ASSERT_EQ(WMError::WM_OK, scene->GoDestroy());
}

/**
 * @tc.name: AppFloatingWindow02
 * @tc.desc: AppFloatingWindow life cycle, main window hide first
 * @tc.type: FUNC
 */
HWTEST_F(WindowAppFloatingWindowTest, AppFloatingWindow02, Function | MediumTest | Level3)
{
    sptr<WindowScene> scene = CreateWindowScene();
    ASSERT_NE(nullptr, scene);

    Rect fltWindRect = GetRectWithVpr(0, 0, 400, 600);
    sptr<Window> fltWin = CreateAppFloatingWindow(WindowType::WINDOW_TYPE_FLOAT, fltWindRect);
    ASSERT_NE(nullptr, fltWin);

    ASSERT_EQ(WMError::WM_OK, scene->GoForeground());
    ASSERT_EQ(WMError::WM_OK, fltWin->Show());

    ASSERT_EQ(WMError::WM_OK, scene->GoBackground());
    ASSERT_EQ(false, scene->GetMainWindow()->GetWindowState() == WindowState::STATE_SHOWN);
    ASSERT_EQ(true, fltWin->GetWindowState() == WindowState::STATE_SHOWN);
    ASSERT_EQ(WMError::WM_OK, fltWin->Hide());

    ASSERT_EQ(WMError::WM_OK, fltWin->Destroy());
    ASSERT_EQ(WMError::WM_OK, scene->GoDestroy());
}

/**
 * @tc.name: AppFloatingWindow03
 * @tc.desc: AppFloatingWindow life cycle, app floating window hide first
 * @tc.type: FUNC
 */
HWTEST_F(WindowAppFloatingWindowTest, AppFloatingWindow03, Function | MediumTest | Level3)
{
    sptr<WindowScene> scene = CreateWindowScene();
    ASSERT_NE(nullptr, scene);

    Rect fltWindRect = GetRectWithVpr(0, 0, 400, 600);
    sptr<Window> fltWin = CreateAppFloatingWindow(WindowType::WINDOW_TYPE_FLOAT, fltWindRect);
    ASSERT_NE(nullptr, fltWin);

    ASSERT_EQ(WMError::WM_OK, scene->GoForeground());
    ASSERT_EQ(WMError::WM_OK, fltWin->Show());

    ASSERT_EQ(WMError::WM_OK, fltWin->Hide());
    ASSERT_EQ(false, fltWin->GetWindowState() == WindowState::STATE_SHOWN);
    ASSERT_EQ(true, scene->GetMainWindow()->GetWindowState() == WindowState::STATE_SHOWN);
    ASSERT_EQ(WMError::WM_OK, scene->GoBackground());

    ASSERT_EQ(WMError::WM_OK, fltWin->Destroy());
    ASSERT_EQ(WMError::WM_OK, scene->GoDestroy());
}

/**
 * @tc.name: AppFloatingWindow04
 * @tc.desc: AppFloatingWindow life cycle, main window destroy first
 * @tc.type: FUNC
 */
HWTEST_F(WindowAppFloatingWindowTest, AppFloatingWindow04, Function | MediumTest | Level3)
{
    sptr<WindowScene> scene = CreateWindowScene();
    ASSERT_NE(nullptr, scene);

    Rect fltWindRect = GetRectWithVpr(0, 0, 400, 600);
    sptr<Window> fltWin = CreateAppFloatingWindow(WindowType::WINDOW_TYPE_FLOAT, fltWindRect);
    ASSERT_NE(nullptr, fltWin);

    ASSERT_EQ(WMError::WM_OK, scene->GoForeground());
    ASSERT_EQ(WMError::WM_OK, fltWin->Show());

    ASSERT_EQ(WMError::WM_OK, scene->GoDestroy());
    ASSERT_EQ(nullptr, scene->GetMainWindow());
    ASSERT_EQ(false, fltWin->GetWindowState() == WindowState::STATE_SHOWN);
    ASSERT_EQ(WMError::WM_OK, fltWin->Destroy());
}

/**
 * @tc.name: AppFloatingWindow05
 * @tc.desc: Camera AppFloatingWindow life cycle
 * @tc.type: FUNC
 * @tc.require: issueI5NEHO
 */
HWTEST_F(WindowAppFloatingWindowTest, AppFloatingWindow05, Function | MediumTest | Level2)
{
    sptr<WindowScene> scene = CreateWindowScene();
    ASSERT_NE(nullptr, scene);

    Rect fltWindRect = GetRectWithVpr(0, 0, 400, 600);
    sptr<Window> fltWin = CreateAppFloatingWindow(WindowType::WINDOW_TYPE_FLOAT_CAMERA, fltWindRect);
    ASSERT_NE(nullptr, fltWin);

    ASSERT_EQ(WMError::WM_OK, scene->GoForeground());
    ASSERT_EQ(WMError::WM_OK, fltWin->Show());

    ASSERT_EQ(WMError::WM_OK, fltWin->Hide());
    ASSERT_EQ(WMError::WM_OK, scene->GoBackground());

    ASSERT_EQ(WMError::WM_OK, fltWin->Destroy());
    ASSERT_EQ(WMError::WM_OK, scene->GoDestroy());
}

/**
 * @tc.name: AppFloatingWindow06
 * @tc.desc: Camera AppFloatingWindow life cycle, main window hide first
 * @tc.type: FUNC
 * @tc.require: issueI5NEHO
 */
HWTEST_F(WindowAppFloatingWindowTest, AppFloatingWindow06, Function | MediumTest | Level3)
{
    sptr<WindowScene> scene = CreateWindowScene();
    ASSERT_NE(nullptr, scene);

    Rect fltWindRect = GetRectWithVpr(0, 0, 400, 600);
    sptr<Window> fltWin = CreateAppFloatingWindow(WindowType::WINDOW_TYPE_FLOAT_CAMERA, fltWindRect);
    ASSERT_NE(nullptr, fltWin);

    ASSERT_EQ(WMError::WM_OK, scene->GoForeground());
    ASSERT_EQ(WMError::WM_OK, fltWin->Show());

    ASSERT_EQ(WMError::WM_OK, scene->GoBackground());
    ASSERT_EQ(false, scene->GetMainWindow()->GetWindowState() == WindowState::STATE_SHOWN);
    ASSERT_EQ(true, fltWin->GetWindowState() == WindowState::STATE_SHOWN);
    ASSERT_EQ(WMError::WM_OK, fltWin->Hide());

    ASSERT_EQ(WMError::WM_OK, fltWin->Destroy());
    ASSERT_EQ(WMError::WM_OK, scene->GoDestroy());
}

/**
 * @tc.name: AppFloatingWindow07
 * @tc.desc: Camera AppFloatingWindow life cycle, app floating window hide first
 * @tc.type: FUNC
 * @tc.require: issueI5NEHO
 */
HWTEST_F(WindowAppFloatingWindowTest, AppFloatingWindow07, Function | MediumTest | Level3)
{
    sptr<WindowScene> scene = CreateWindowScene();
    ASSERT_NE(nullptr, scene);

    Rect fltWindRect = GetRectWithVpr(0, 0, 400, 600);
    sptr<Window> fltWin = CreateAppFloatingWindow(WindowType::WINDOW_TYPE_FLOAT_CAMERA, fltWindRect);
    ASSERT_NE(nullptr, fltWin);

    ASSERT_EQ(WMError::WM_OK, scene->GoForeground());
    ASSERT_EQ(WMError::WM_OK, fltWin->Show());

    ASSERT_EQ(WMError::WM_OK, fltWin->Hide());
    ASSERT_EQ(false, fltWin->GetWindowState() == WindowState::STATE_SHOWN);
    ASSERT_EQ(true, scene->GetMainWindow()->GetWindowState() == WindowState::STATE_SHOWN);
    ASSERT_EQ(WMError::WM_OK, scene->GoBackground());

    ASSERT_EQ(WMError::WM_OK, fltWin->Destroy());
    ASSERT_EQ(WMError::WM_OK, scene->GoDestroy());
}

/**
 * @tc.name: AppFloatingWindow08
 * @tc.desc: Camera AppFloatingWindow life cycle, main window destroy first
 * @tc.type: FUNC
 * @tc.require: issueI5NEHO
 */
HWTEST_F(WindowAppFloatingWindowTest, AppFloatingWindow08, Function | MediumTest | Level3)
{
    sptr<WindowScene> scene = CreateWindowScene();
    ASSERT_NE(nullptr, scene);

    Rect fltWindRect = GetRectWithVpr(0, 0, 400, 600);
    sptr<Window> fltWin = CreateAppFloatingWindow(WindowType::WINDOW_TYPE_FLOAT_CAMERA, fltWindRect);
    ASSERT_NE(nullptr, fltWin);

    ASSERT_EQ(WMError::WM_OK, scene->GoForeground());
    ASSERT_EQ(WMError::WM_OK, fltWin->Show());

    ASSERT_EQ(WMError::WM_OK, scene->GoDestroy());
    ASSERT_EQ(nullptr, scene->GetMainWindow());
    ASSERT_EQ(false, fltWin->GetWindowState() == WindowState::STATE_SHOWN);
    ASSERT_EQ(WMError::WM_OK, fltWin->Destroy());
}

/**
 * @tc.name: AppFloatingWindow09
 * @tc.desc: Camera AppFloatingWindow rect check
 * @tc.type: FUNC
 * @tc.require: issueI5NEHO
 */
HWTEST_F(WindowAppFloatingWindowTest, AppFloatingWindow09, Function | MediumTest | Level3)
{
    sptr<WindowScene> scene = CreateWindowScene();
    ASSERT_NE(nullptr, scene);

    Rect fltWindRect = GetRectWithVpr(10, 20, 10, 10);
    sptr<Window> fltWin = CreateAppFloatingWindow(WindowType::WINDOW_TYPE_FLOAT_CAMERA, fltWindRect);
    ASSERT_NE(nullptr, fltWin);

    ASSERT_EQ(WMError::WM_OK, scene->GoForeground());
    ASSERT_EQ(WMError::WM_OK, fltWin->Show());

    Rect exceptRect = {10, 20, 0, 0};
    uint32_t smallWidth = displayRect_.height_ <= displayRect_.width_ ? displayRect_.height_ : displayRect_.width_;
    float hwRatio = static_cast<float>(displayRect_.height_) / static_cast<float>(displayRect_.width_);
    if (smallWidth <= static_cast<uint32_t>(600 * virtualPixelRatio_)) { // sw <= 600dp
        if (displayRect_.width_ <= displayRect_.height_) {
            exceptRect.width_= static_cast<uint32_t>(smallWidth * 0.3);
        } else {
            exceptRect.width_ = static_cast<uint32_t>(smallWidth * 0.5);
        }
    } else {
        if (displayRect_.width_ <= displayRect_.height_) {
            exceptRect.width_ = static_cast<uint32_t>(smallWidth * 0.12);
        } else {
            exceptRect.width_ = static_cast<uint32_t>(smallWidth * 0.3);
        }
    }
    exceptRect.height_ = static_cast<uint32_t>(exceptRect.width_ * hwRatio);
    ASSERT_TRUE(WindowTestUtils::RectEqualTo(fltWin, exceptRect));

    ASSERT_EQ(WMError::WM_OK, fltWin->Destroy());
    ASSERT_EQ(WMError::WM_OK, scene->GoDestroy());
}

/**
 * @tc.name: AppFloatingWindow10
 * @tc.desc: Camera AppFloatingWindow multi create
 * @tc.type: FUNC
 * @tc.require: issueI5NEHO
 */
HWTEST_F(WindowAppFloatingWindowTest, AppFloatingWindow10, Function | MediumTest | Level3)
{
    sptr<WindowScene> scene = CreateWindowScene();
    ASSERT_NE(nullptr, scene);

    Rect fltWindRect = GetRectWithVpr(0, 0, 400, 600);
    sptr<Window> fltWin = CreateAppFloatingWindow(WindowType::WINDOW_TYPE_FLOAT_CAMERA, fltWindRect);
    ASSERT_NE(nullptr, fltWin);

    sptr<Window> fltWin2 = CreateAppFloatingWindow(WindowType::WINDOW_TYPE_FLOAT_CAMERA, fltWindRect);
    ASSERT_EQ(nullptr, fltWin2);

    ASSERT_EQ(WMError::WM_OK, fltWin->Destroy());
    sptr<Window> fltWin3 = CreateAppFloatingWindow(WindowType::WINDOW_TYPE_FLOAT_CAMERA, fltWindRect);
    ASSERT_NE(nullptr, fltWin3);

    ASSERT_EQ(WMError::WM_OK, fltWin3->Destroy());
    ASSERT_EQ(WMError::WM_OK, scene->GoDestroy());
}

/**
 * @tc.name: AppFloatingWindow11
 * @tc.desc: Camera AppFloatingWindow listener
 * @tc.type: FUNC
 * @tc.require: issueI5NEHR
 */
HWTEST_F(WindowAppFloatingWindowTest, AppFloatingWindow11, Function | MediumTest | Level2)
{
    uint32_t tokenId = static_cast<uint32_t>(IPCSkeleton::GetCallingTokenID());
    WindowManager::GetInstance().RegisterCameraFloatWindowChangedListener(testCameraFloatWindowChangedListener_);
    sptr<WindowScene> scene = CreateWindowScene();
    ASSERT_NE(nullptr, scene);

    Rect fltWindRect = GetRectWithVpr(0, 0, 400, 600);
    sptr<Window> fltWin = CreateAppFloatingWindow(WindowType::WINDOW_TYPE_FLOAT_CAMERA, fltWindRect);
    ASSERT_NE(nullptr, fltWin);

    ASSERT_EQ(WMError::WM_OK, scene->GoForeground());
    ASSERT_EQ(WMError::WM_OK, fltWin->Show());

    usleep(500000); // 500000us = 0.5s
    ASSERT_EQ(tokenId, testCameraFloatWindowChangedListener_->accessTokenId_);
    ASSERT_EQ(true, testCameraFloatWindowChangedListener_->isShowing_);

    ASSERT_EQ(WMError::WM_OK, fltWin->Hide());

    usleep(500000); // 500000us = 0.5s
    ASSERT_EQ(tokenId, testCameraFloatWindowChangedListener_->accessTokenId_);
    ASSERT_EQ(false, testCameraFloatWindowChangedListener_->isShowing_);

    ASSERT_EQ(WMError::WM_OK, fltWin->Destroy());
    ASSERT_EQ(WMError::WM_OK, scene->GoDestroy());

    WindowManager::GetInstance().UnregisterCameraFloatWindowChangedListener(testCameraFloatWindowChangedListener_);
}
} // namespace Rosen
} // namespace OHOS
