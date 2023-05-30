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
#include "wm_common.h"
using namespace testing;
using namespace testing::ext;

namespace OHOS {
namespace Rosen {
namespace {
    constexpr uint32_t WAIT_ASYNC_US = 100000;  // 100ms
}
using Utils = WindowTestUtils;
class WindowInputTest : public testing::Test {
public:
    static void SetUpTestCase();
    static void TearDownTestCase();
    virtual void SetUp() override;
    virtual void TearDown() override;
    Utils::TestWindowInfo fullScreenWindow_;
};

void WindowInputTest::SetUpTestCase()
{
    auto display = DisplayManager::GetInstance().GetDisplayById(0);
    ASSERT_TRUE((display != nullptr));
    Rect displayRect = {0, 0, display->GetWidth(), display->GetHeight()};
    Utils::InitByDisplayRect(displayRect);
}

void WindowInputTest::TearDownTestCase()
{
}

void WindowInputTest::SetUp()
{
    fullScreenWindow_ = {
        .name = "FullWindow",
        .rect = Utils::customAppRect_,
        .type = WindowType::WINDOW_TYPE_APP_MAIN_WINDOW,
        .mode = WindowMode::WINDOW_MODE_FULLSCREEN,
        .needAvoid = false,
        .parentLimit = false,
        .parentId = INVALID_WINDOW_ID,
    };
}

void WindowInputTest::TearDown()
{
}

namespace {
/**
 * @tc.name: SetTouchHotAreas01
 * @tc.desc: Normal scenario testing for Window#SetTouchHotAreas
 * @tc.type: FUNC
 */
HWTEST_F(WindowInputTest, SetTouchHotAreas01, Function | MediumTest | Level3)
{
    fullScreenWindow_.name = "window_hot_areas.1";
    const sptr<Window>& window = Utils::CreateTestWindow(fullScreenWindow_);
    ASSERT_EQ(WMError::WM_OK, window->Show());

    std::vector<Rect> requestedTouchHotAreas;
    window->GetRequestedTouchHotAreas(requestedTouchHotAreas);
    ASSERT_TRUE(requestedTouchHotAreas.empty());

    usleep(WAIT_ASYNC_US);
    Rect windowRect = window->GetRect();

    std::vector<Rect> rects;
    uint32_t hotAreasNum = 10;
    uint32_t hotAreaWidth = windowRect.width_ / hotAreasNum;
    uint32_t hotAreaHeight = windowRect.height_ / hotAreasNum;
    for (uint32_t i = 0; i < hotAreasNum; ++i) {
        rects.emplace_back(Rect{ hotAreaWidth * i, hotAreaHeight * i, hotAreaWidth, hotAreaHeight });
    }
    ASSERT_EQ(WMError::WM_OK, window->SetTouchHotAreas(rects));
    window->GetRequestedTouchHotAreas(requestedTouchHotAreas);
    ASSERT_EQ(rects.size(), requestedTouchHotAreas.size());
    for (uint32_t i = 0; i < rects.size(); ++i) {
        ASSERT_TRUE(rects[i] == requestedTouchHotAreas[i]);
    }

    rects.clear();
    rects.emplace_back(Rect{ 0, 0, hotAreaWidth, hotAreaHeight });
    ASSERT_EQ(WMError::WM_OK, window->SetTouchHotAreas(rects));
    window->GetRequestedTouchHotAreas(requestedTouchHotAreas);
    ASSERT_EQ(rects.size(), requestedTouchHotAreas.size());
    for (uint32_t i = 0; i < rects.size(); ++i) {
        ASSERT_TRUE(rects[i] == requestedTouchHotAreas[i]);
    }

    rects.clear();
    ASSERT_EQ(WMError::WM_OK, window->SetTouchHotAreas(rects));
    window->GetRequestedTouchHotAreas(requestedTouchHotAreas);
    ASSERT_TRUE(requestedTouchHotAreas.empty());

    ASSERT_EQ(WMError::WM_OK, window->Destroy());
}

/**
 * @tc.name: SetTouchHotAreas02
 * @tc.desc: Abnormal scenario testing for Window#SetTouchHotAreas
 * @tc.type: FUNC
 */
HWTEST_F(WindowInputTest, SetTouchHotAreas02, Function | MediumTest | Level3)
{
    fullScreenWindow_.name = "window_hot_areas.2";
    const sptr<Window>& window = Utils::CreateTestWindow(fullScreenWindow_);
    ASSERT_EQ(WMError::WM_OK, window->Show());

    usleep(WAIT_ASYNC_US);
    Rect windowRect = window->GetRect();

    std::vector<Rect> rects;
    /* maximum hot areas: 10, test exceeding maximum hot areas */
    uint32_t hotAreasNum = 11;
    uint32_t hotAreaWidth = windowRect.width_ / hotAreasNum;
    uint32_t hotAreaHeight = windowRect.height_ / hotAreasNum;
    for (uint32_t i = 0; i < hotAreasNum; ++i) {
        rects.emplace_back(Rect{ hotAreaWidth * i, hotAreaHeight * i, hotAreaWidth, hotAreaHeight });
    }
    ASSERT_EQ(WMError::WM_ERROR_INVALID_PARAM, window->SetTouchHotAreas(rects));

    rects.clear();
    rects.emplace_back(Rect{ -1, 0, windowRect.width_ / 2, windowRect.height_ / 2 });
    ASSERT_EQ(WMError::WM_ERROR_INVALID_PARAM, window->SetTouchHotAreas(rects));

    rects.clear();
    rects.emplace_back(Rect{ 0, -1, windowRect.width_ / 2, windowRect.height_ / 2 });
    ASSERT_EQ(WMError::WM_ERROR_INVALID_PARAM, window->SetTouchHotAreas(rects));

    rects.clear();
    rects.emplace_back(Rect{ 0, 0, 0, windowRect.height_ / 2 });
    ASSERT_EQ(WMError::WM_ERROR_INVALID_PARAM, window->SetTouchHotAreas(rects));

    rects.clear();
    rects.emplace_back(Rect{ 0, 0, windowRect.width_ / 2, 0 });
    ASSERT_EQ(WMError::WM_ERROR_INVALID_PARAM, window->SetTouchHotAreas(rects));

    rects.clear();
    rects.emplace_back(Rect{ windowRect.width_, 0, windowRect.width_ / 2, windowRect.height_ / 2 });
    ASSERT_EQ(WMError::WM_ERROR_INVALID_PARAM, window->SetTouchHotAreas(rects));

    rects.clear();
    rects.emplace_back(Rect{ 0, windowRect.height_, windowRect.width_ / 2, windowRect.height_ / 2 });
    ASSERT_EQ(WMError::WM_ERROR_INVALID_PARAM, window->SetTouchHotAreas(rects));

    std::vector<Rect> requestedTouchHotAreas;
    window->GetRequestedTouchHotAreas(requestedTouchHotAreas);
    ASSERT_TRUE(requestedTouchHotAreas.empty());

    ASSERT_EQ(WMError::WM_OK, window->Destroy());
}
} // namespace
} // namespace Rosen
} // namespace OHOS