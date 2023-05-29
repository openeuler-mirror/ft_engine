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

#include <atomic>
#include <chrono>
#include <condition_variable>
#include <mutex>

#include <transaction/rs_transaction.h>
#include "display_manager.h"
#include "wm_common.h"
#include "window_manager.h"
#include "window_test_utils.h"
#include "surface_draw.h"
#include "window_manager.h"

using namespace testing;
using namespace testing::ext;

namespace OHOS {
namespace Rosen {
namespace {
    constexpr uint32_t COLOR_RED = 0xffff0000;
    constexpr HiviewDFX::HiLogLabel LABEL = {LOG_CORE, HILOG_DOMAIN_WINDOW, "WindowVisibilityInfoTest"};
    constexpr uint8_t ALPHA = 255;
}

using Utils = WindowTestUtils;
constexpr int WAIT_ASYNC_MS_TIME_OUT = 5000; // 5000ms, filling color is slow

class VisibilityChangedListenerImpl : public IVisibilityChangedListener {
public:
    VisibilityChangedListenerImpl(std::mutex& mutex, std::condition_variable& cv) : mutex_(mutex), cv_(cv) {}
    void OnWindowVisibilityChanged(const std::vector<sptr<WindowVisibilityInfo>>& windowVisibilityInfo) override;
    std::vector<sptr<WindowVisibilityInfo>> windowVisibilityInfos_;
    bool isCallbackCalled_ { false };
private:
    std::mutex& mutex_;
    std::condition_variable& cv_;
};

void VisibilityChangedListenerImpl::OnWindowVisibilityChanged(
    const std::vector<sptr<WindowVisibilityInfo>>& windowVisibilityInfo)
{
    std::unique_lock<std::mutex> lock(mutex_);
    WLOGFI("size:%{public}zu", windowVisibilityInfo.size());
    windowVisibilityInfos_ = std::move(windowVisibilityInfo);
    for (auto& info : windowVisibilityInfos_) {
        WLOGFI("windowId:%{public}u, isVisible_:%{public}d, pid:%{public}d, uid:%{public}d, windowType:%{public}u",
            info->windowId_, info->isVisible_, info->pid_, info->uid_, static_cast<uint32_t>(info->windowType_));
    }
    isCallbackCalled_ = true;
    cv_.notify_all();
}

class WindowVisibilityInfoTest : public testing::Test {
public:
    static void SetUpTestCase();

    static void TearDownTestCase();

    virtual void SetUp() override;

    virtual void TearDown() override;

    static inline std::mutex mutex_;
    static inline std::condition_variable cv_;
    static inline sptr<VisibilityChangedListenerImpl> visibilityChangedListener_ =
        new VisibilityChangedListenerImpl(mutex_, cv_);

    static inline void ResetCallbackCalledFLag()
    {
        std::unique_lock<std::mutex> lock(mutex_);
        visibilityChangedListener_->isCallbackCalled_ = false;
        visibilityChangedListener_->windowVisibilityInfos_.clear();
    }

    static void WaitForCallback();
    Utils::TestWindowInfo fullScreenAppInfo_;
    Utils::TestWindowInfo floatAppInfo_;
    Utils::TestWindowInfo subAppInfo_;
    bool FillColor(sptr<Window> window);
};

bool WindowVisibilityInfoTest::FillColor(sptr<Window> window)
{
    if (window == nullptr) {
        return false;
    }
    auto surfaceNode = window->GetSurfaceNode();
    if (surfaceNode == nullptr) {
        return false;
    }
    Rect rect = window->GetRect();
    bool isDrawSuccess = SurfaceDraw::DrawColor(surfaceNode, rect.width_, rect.height_, COLOR_RED);
    surfaceNode->SetAbilityBGAlpha(ALPHA);
    RSTransaction::FlushImplicitTransaction();
    return isDrawSuccess;
}

void WindowVisibilityInfoTest::SetUpTestCase()
{
    auto display = DisplayManager::GetInstance().GetDisplayById(0);
    ASSERT_TRUE((display != nullptr));
    WLOGFI("GetDefaultDisplay: id %{public}" PRIu64", w %{public}d, h %{public}d, fps %{public}u",
        display->GetId(), display->GetWidth(), display->GetHeight(), display->GetRefreshRate());
    Rect displayRect = {0, 0,
                        static_cast<uint32_t>(display->GetWidth()), static_cast<uint32_t>(display->GetHeight())};
    Utils::InitByDisplayRect(displayRect);
    WindowManager::GetInstance().RegisterVisibilityChangedListener(visibilityChangedListener_);
    auto displayId = DisplayManager::GetInstance().GetDefaultDisplayId();
    WindowManager::GetInstance().MinimizeAllAppWindows(displayId);
}

void WindowVisibilityInfoTest::TearDownTestCase()
{
    WindowManager::GetInstance().UnregisterVisibilityChangedListener(visibilityChangedListener_);
}

void WindowVisibilityInfoTest::SetUp()
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
        .showWhenLocked = true,
        .parentId = INVALID_WINDOW_ID,
    };
}

void WindowVisibilityInfoTest::TearDown()
{
}

void WindowVisibilityInfoTest::WaitForCallback()
{
    std::unique_lock<std::mutex> lock(mutex_);
    visibilityChangedListener_->isCallbackCalled_ = false;
    visibilityChangedListener_->windowVisibilityInfos_.clear();
    auto now = std::chrono::system_clock::now();
    if (!cv_.wait_until(lock, now + std::chrono::milliseconds(WAIT_ASYNC_MS_TIME_OUT),
        []() { return visibilityChangedListener_->isCallbackCalled_; })) {
        WLOGFI("wait_until time out");
    }
}

namespace {
/**
* @tc.name: WindowVisibilityInfoTest01
* @tc.desc: window show or hide
* @tc.type: FUNC
* @tc.require: issueI5FSQW
*/
HWTEST_F(WindowVisibilityInfoTest, WindowVisibilityInfoTest01, Function | MediumTest | Level1)
{
    floatAppInfo_.name = "window1";
    floatAppInfo_.rect = {0, 0, 300, 100};
    sptr<Window> window1 = Utils::CreateTestWindow(floatAppInfo_);

    subAppInfo_.name = "subWindow1";
    subAppInfo_.rect = {0, 600, 300, 100};
    subAppInfo_.parentId = window1->GetWindowId();
    sptr<Window> subWindow1 = Utils::CreateTestWindow(subAppInfo_);

    bool isWindowVisible = false;
    bool isSubWindowVisible = false;

    uint32_t visibilityInfoCount = 0;
    ASSERT_EQ(WMError::WM_OK, window1->Show());
    isWindowVisible = FillColor(window1);
    WaitForCallback();
    visibilityInfoCount = isWindowVisible ? 1 : 0;
    ResetCallbackCalledFLag();

    ASSERT_EQ(WMError::WM_OK, subWindow1->Show());
    isSubWindowVisible = FillColor(subWindow1);
    WaitForCallback();
    visibilityInfoCount = isSubWindowVisible ? 1 : 0;
    ResetCallbackCalledFLag();

    ASSERT_EQ(WMError::WM_OK, subWindow1->Hide());
    WaitForCallback();
    visibilityInfoCount = isSubWindowVisible ? 1 : 0;
    ResetCallbackCalledFLag();

    ASSERT_EQ(WMError::WM_OK, window1->Hide());
    WaitForCallback();
    visibilityInfoCount = isWindowVisible ? 1 : 0;
    ResetCallbackCalledFLag();

    window1->Destroy();
    subWindow1->Destroy();
    sleep(2);
}
}
} // namespace Rosen
} // namespace OHOS

