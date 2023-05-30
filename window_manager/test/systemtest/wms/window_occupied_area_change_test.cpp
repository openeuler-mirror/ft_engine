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
#include "window_test_utils.h"
using namespace testing;
using namespace testing::ext;

namespace OHOS {
namespace Rosen {
namespace {
    constexpr HiviewDFX::HiLogLabel LABEL = {LOG_CORE, HILOG_DOMAIN_WINDOW, "WindowOccupiedAreaChangeTest"};
}

using Utils = WindowTestUtils;
const int WAIT_ASYNC_US = 100000;  // 100000us

class TestOccupiedAreaChangeListener : public IOccupiedAreaChangeListener {
public:
    OccupiedAreaType type_ = OccupiedAreaType::TYPE_INPUT;
    Rect rect_ = { 0, 0, 0, 0 };
    void OnSizeChange(const sptr<OccupiedAreaChangeInfo>& info) override;
};

class WindowOccupiedAreaChangeTest : public testing::Test {
public:
    static void SetUpTestCase();
    static void TearDownTestCase();
    virtual void SetUp() override;
    virtual void TearDown() override;
    static sptr<TestOccupiedAreaChangeListener> testOccupiedAreaChangeListener_;
    Utils::TestWindowInfo fullScreenAppInfo_;
    Utils::TestWindowInfo imeAppInfo_;
};

sptr<TestOccupiedAreaChangeListener> WindowOccupiedAreaChangeTest::testOccupiedAreaChangeListener_ =
    new TestOccupiedAreaChangeListener();

void TestOccupiedAreaChangeListener::OnSizeChange(const sptr<OccupiedAreaChangeInfo>& info)
{
    WLOGFI("OccupiedAreaChangeInfo: [%{public}u, {%{public}u, %{public}u}]",
        info->type_, info->rect_.width_, info->rect_.height_);
    type_ = info->type_;
    rect_ = info->rect_;
}

void WindowOccupiedAreaChangeTest::SetUpTestCase()
{
    auto display = DisplayManager::GetInstance().GetDisplayById(0);
    ASSERT_TRUE((display != nullptr));
    WLOGFI("GetDefaultDisplay: id %{public}" PRIu64", w %{public}d, h %{public}d, fps %{public}u",
        display->GetId(), display->GetWidth(), display->GetHeight(), display->GetRefreshRate());
    Rect displayRect = {0, 0, display->GetWidth(), display->GetHeight()};
    Utils::InitByDisplayRect(displayRect);
}

void WindowOccupiedAreaChangeTest::TearDownTestCase()
{
}

void WindowOccupiedAreaChangeTest::SetUp()
{
    fullScreenAppInfo_ = {
            .name = "FullWindow",
            .rect = Utils::customAppRect_,
            .type = WindowType::WINDOW_TYPE_APP_MAIN_WINDOW,
            .mode = WindowMode::WINDOW_MODE_FULLSCREEN,
            .needAvoid = false,
            .parentLimit = false,
            .parentId = INVALID_WINDOW_ID,
    };
    imeAppInfo_ = {
            .name = "ImeWindow",
            .rect = Utils::customAppRect_,
            .type = WindowType::WINDOW_TYPE_INPUT_METHOD_FLOAT,
            .mode = WindowMode::WINDOW_MODE_FLOATING,
    };
}

void WindowOccupiedAreaChangeTest::TearDown()
{
}

namespace {
/**
 * @tc.name: KeyboardHeightChangeTest01
 * @tc.desc: test get keyboard height when WINDOW_TYPE_INPUT_METHOD_FLOAT show
 * @tc.type: FUNC
 */
HWTEST_F(WindowOccupiedAreaChangeTest, KeyboardHeightChangeTest01, Function | MediumTest | Level3)
{
    fullScreenAppInfo_.name = "KeyboardHeightChangeTest01";
    const sptr<Window>& window1 = Utils::CreateTestWindow(fullScreenAppInfo_);
    ASSERT_NE(nullptr, window1);
    window1->RegisterOccupiedAreaChangeListener(testOccupiedAreaChangeListener_);

    imeAppInfo_.name = "imeWindow1";
    imeAppInfo_.rect = { 10, 200, 300, 400 };
    const sptr<Window>& window2 = Utils::CreateTestWindow(imeAppInfo_);
    ASSERT_NE(nullptr, window2);

    ASSERT_EQ(WMError::WM_OK, window1->Show());
    ASSERT_EQ(WMError::WM_OK, window2->SetCallingWindow(window1->GetWindowId()));
    ASSERT_EQ(WMError::WM_OK, window2->Show());
    // Await 100ms and get callback result in listener.
    usleep(WAIT_ASYNC_US);

    ASSERT_EQ(testOccupiedAreaChangeListener_->rect_.posX_, window2->GetRect().posX_);
    ASSERT_EQ(testOccupiedAreaChangeListener_->rect_.posY_, window2->GetRect().posY_);
    ASSERT_EQ(testOccupiedAreaChangeListener_->rect_.width_, window2->GetRect().width_);
    ASSERT_EQ(testOccupiedAreaChangeListener_->rect_.height_, window2->GetRect().height_);
}

/**
 * @tc.name: KeyboardHeightChangeTest02
 * @tc.desc: test get keyboard height when WINDOW_TYPE_INPUT_METHOD_FLOAT hide
 * @tc.type: FUNC
 */
HWTEST_F(WindowOccupiedAreaChangeTest, KeyboardHeightChangeTest02, Function | MediumTest | Level3)
{
    fullScreenAppInfo_.name = "KeyboardHeightChangeTest02";
    const sptr<Window>& window1 = Utils::CreateTestWindow(fullScreenAppInfo_);
    ASSERT_NE(nullptr, window1);
    window1->RegisterOccupiedAreaChangeListener(testOccupiedAreaChangeListener_);

    imeAppInfo_.name = "imeWindow2";
    imeAppInfo_.rect = { 10, 200, 300, 400 };
    const sptr<Window>& window2 = Utils::CreateTestWindow(imeAppInfo_);
    ASSERT_NE(nullptr, window2);

    ASSERT_EQ(WMError::WM_OK, window1->Show());
    ASSERT_EQ(WMError::WM_OK, window2->SetCallingWindow(window1->GetWindowId()));
    ASSERT_EQ(WMError::WM_OK, window2->Show());
    ASSERT_EQ(WMError::WM_OK, window2->Hide());
    // Await 100ms and get callback result in listener.
    usleep(WAIT_ASYNC_US);
    ASSERT_EQ(testOccupiedAreaChangeListener_->rect_.posX_, 0);
    ASSERT_EQ(testOccupiedAreaChangeListener_->rect_.posY_, 0);
    ASSERT_EQ(testOccupiedAreaChangeListener_->rect_.width_, 0);
    ASSERT_EQ(testOccupiedAreaChangeListener_->rect_.height_, 0);
}

/**
 * @tc.name: KeyboardHeightChangeTest03
 * @tc.desc: test get keyboard height when WINDOW_TYPE_INPUT_METHOD_FLOAT destroy
 * @tc.type: FUNC
 */
HWTEST_F(WindowOccupiedAreaChangeTest, KeyboardHeightChangeTest03, Function | MediumTest | Level3)
{
    fullScreenAppInfo_.name = "KeyboardHeightChangeTest03";
    const sptr<Window>& window1 = Utils::CreateTestWindow(fullScreenAppInfo_);
    ASSERT_NE(nullptr, window1);
    window1->RegisterOccupiedAreaChangeListener(testOccupiedAreaChangeListener_);

    imeAppInfo_.name = "imeWindow3";
    imeAppInfo_.rect = { 10, 200, 300, 400 };
    const sptr<Window>& window2 = Utils::CreateTestWindow(imeAppInfo_);
    ASSERT_NE(nullptr, window2);

    ASSERT_EQ(WMError::WM_OK, window1->Show());
    ASSERT_EQ(WMError::WM_OK, window2->SetCallingWindow(window1->GetWindowId()));
    ASSERT_EQ(WMError::WM_OK, window2->Show());
    ASSERT_EQ(WMError::WM_OK, window2->Destroy());
    // Await 100ms and get callback result in listener.
    usleep(WAIT_ASYNC_US);
    ASSERT_EQ(testOccupiedAreaChangeListener_->rect_.posX_, 0);
    ASSERT_EQ(testOccupiedAreaChangeListener_->rect_.posY_, 0);
    ASSERT_EQ(testOccupiedAreaChangeListener_->rect_.width_, 0);
    ASSERT_EQ(testOccupiedAreaChangeListener_->rect_.height_, 0);
}
}
} // namespace Rosen
} // namespace OHOS
