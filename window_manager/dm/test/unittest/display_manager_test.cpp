/*
 * Copyright (c) 2022-2022 Huawei Device Co., Ltd.
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
#include "display_manager.h"
#include "window.h"

#include "mock_display_manager_adapter.h"
#include "singleton_mocker.h"
#include "display_manager.cpp"

using namespace testing;
using namespace testing::ext;

namespace OHOS {
namespace Rosen {
using Mocker = SingletonMocker<DisplayManagerAdapter, MockDisplayManagerAdapter>;
class DmMockScreenshotListener : public DisplayManager::IScreenshotListener {
public:
    void OnScreenshot(const ScreenshotInfo info) override {}
};
class DmMockDisplayListener : public DisplayManager::IDisplayListener {
public:
    void OnCreate(DisplayId) override {}
    void OnDestroy(DisplayId) override {}
    void OnChange(DisplayId) override {}
};
class DisplayManagerTest : public testing::Test {
public:
    static void SetUpTestCase();
    static void TearDownTestCase();
    virtual void SetUp() override;
    virtual void TearDown() override;
};

void DisplayManagerTest::SetUpTestCase()
{
}

void DisplayManagerTest::TearDownTestCase()
{
}

void DisplayManagerTest::SetUp()
{
}

void DisplayManagerTest::TearDown()
{
}

namespace {
/**
 * @tc.name: Freeze01
 * @tc.desc: success
 * @tc.type: FUNC
 */
HWTEST_F(DisplayManagerTest, Freeze01, Function | SmallTest | Level1)
{
    std::vector<DisplayId> displayIds;
    displayIds.push_back(0);
    bool ret = DisplayManager::GetInstance().Freeze(displayIds);
    ASSERT_TRUE(ret);
}

/**
 * @tc.name: Freeze02
 * @tc.desc: test Freeze displayIds exceed the maximum
 * @tc.type: FUNC
 */
HWTEST_F(DisplayManagerTest, Freeze02, Function | SmallTest | Level1)
{
    std::vector<DisplayId> displayIds;
    for (uint32_t i = 0; i < 33; i++) { // MAX_DISPLAY_SIZE + 1
        displayIds.push_back(i);
    }

    bool ret = DisplayManager::GetInstance().Freeze(displayIds);
    ASSERT_FALSE(ret);
}

/**
 * @tc.name: Freeze03
 * @tc.desc: test Freeze displayIds empty
 * @tc.type: FUNC
 */
HWTEST_F(DisplayManagerTest, Freeze03, Function | SmallTest | Level1)
{
    std::vector<DisplayId> displayIds;
    bool ret = DisplayManager::GetInstance().Freeze(displayIds);
    ASSERT_FALSE(ret);
}

/**
 * @tc.name: Unfreeze01
 * @tc.desc: success
 * @tc.type: FUNC
 */
HWTEST_F(DisplayManagerTest, Unfreeze01, Function | SmallTest | Level1)
{
    std::vector<DisplayId> displayIds;
    displayIds.push_back(0);
    bool ret = DisplayManager::GetInstance().Unfreeze(displayIds);
    ASSERT_TRUE(ret);
}

/**
 * @tc.name: Unfreeze02
 * @tc.desc: test Freeze displayIds exceed the maximum
 * @tc.type: FUNC
 */
HWTEST_F(DisplayManagerTest, Unfreeze02, Function | SmallTest | Level1)
{
    std::vector<DisplayId> displayIds;
    for (uint32_t i = 0; i < 33; i++) { // MAX_DISPLAY_SIZE + 1
        displayIds.push_back(i);
    }

    bool ret = DisplayManager::GetInstance().Unfreeze(displayIds);
    ASSERT_FALSE(ret);
}

/**
 * @tc.name: Unfreeze03
 * @tc.desc: test Freeze displayIds empty
 * @tc.type: FUNC
 */
HWTEST_F(DisplayManagerTest, Unfreeze03, Function | SmallTest | Level1)
{
    std::vector<DisplayId> displayIds;
    bool ret = DisplayManager::GetInstance().Unfreeze(displayIds);
    ASSERT_FALSE(ret);
}

/**
 * @tc.name: RegisterScreenshotListener01
 * @tc.desc: test RegisterScreenshotListener with null listener
 * @tc.type: FUNC
 */
HWTEST_F(DisplayManagerTest, RegisterScreenshotListener01, Function | SmallTest | Level1)
{
    bool ret = DisplayManager::GetInstance().RegisterScreenshotListener(nullptr);
    ASSERT_FALSE(ret);
}

/**
 * @tc.name: RegisterScreenshotListener02
 * @tc.desc: test RegisterScreenshotListener with null listener
 * @tc.type: FUNC
 */
HWTEST_F(DisplayManagerTest, RegisterScreenshotListener02, Function | SmallTest | Level1)
{
    std::unique_ptr<Mocker> m = std::make_unique<Mocker>();
    EXPECT_CALL(m->Mock(), RegisterDisplayManagerAgent(_, _)).Times(1).WillOnce(Return(false));
    sptr<DisplayManager::IScreenshotListener> listener = new DmMockScreenshotListener();
    bool ret = DisplayManager::GetInstance().RegisterScreenshotListener(listener);
    ASSERT_FALSE(ret);
}

/**
 * @tc.name: UnregisterScreenshotListener01
 * @tc.desc: test UnregisterScreenshotListener with null listener
 * @tc.type: FUNC
 */
HWTEST_F(DisplayManagerTest, UnregisterScreenshotListener01, Function | SmallTest | Level1)
{
    bool ret = DisplayManager::GetInstance().UnregisterScreenshotListener(nullptr);
    ASSERT_FALSE(ret);
}

/**
 * @tc.name: OnDisplayCreate01
 * @tc.desc: OnDisplayCreate
 * @tc.type: FUNC
 */
HWTEST_F(DisplayManagerTest, OnDisplayCreate01, Function | SmallTest | Level1)
{
    std::unique_ptr<Mocker> m = std::make_unique<Mocker>();
    EXPECT_CALL(m->Mock(), RegisterDisplayManagerAgent(_, _)).Times(1).WillOnce(Return(true));
    sptr<DisplayManager::IDisplayListener> listener = new DmMockDisplayListener();
    DisplayManager::GetInstance().RegisterDisplayListener(listener);
    auto displayManagerListener = DisplayManager::GetInstance().pImpl_->displayManagerListener_;
    ASSERT_NE(displayManagerListener, nullptr);
    displayManagerListener->OnDisplayCreate(nullptr);
    sptr<DisplayInfo> displayInfo = new DisplayInfo();
    displayInfo->SetDisplayId(DISPLAY_ID_INVALID);
    displayManagerListener->OnDisplayCreate(displayInfo);
    displayInfo->SetDisplayId(0);
    displayManagerListener->OnDisplayCreate(displayInfo);
    ASSERT_NE(displayManagerListener->pImpl_, nullptr);
    displayManagerListener->pImpl_ = nullptr;
    displayManagerListener->OnDisplayCreate(displayInfo);
    DisplayManager::GetInstance().pImpl_->displayManagerListener_ = nullptr;
}

/**
 * @tc.name: OnDisplayDestroy
 * @tc.desc: OnDisplayDestroy
 * @tc.type: FUNC
 */
HWTEST_F(DisplayManagerTest, OnDisplayDestroy, Function | SmallTest | Level1)
{
    std::unique_ptr<Mocker> m = std::make_unique<Mocker>();
    EXPECT_CALL(m->Mock(), RegisterDisplayManagerAgent(_, _)).Times(1).WillOnce(Return(true));
    sptr<DisplayManager::IDisplayListener> listener = new DmMockDisplayListener();
    DisplayManager::GetInstance().RegisterDisplayListener(listener);
    auto displayManagerListener = DisplayManager::GetInstance().pImpl_->displayManagerListener_;
    ASSERT_NE(displayManagerListener, nullptr);
    displayManagerListener->OnDisplayDestroy(DISPLAY_ID_INVALID);
    displayManagerListener->OnDisplayDestroy(0);
    displayManagerListener->pImpl_ = nullptr;
    displayManagerListener->OnDisplayDestroy(1);
    DisplayManager::GetInstance().pImpl_->displayManagerListener_ = nullptr;
}

/**
 * @tc.name: OnDisplayChange
 * @tc.desc: OnDisplayChange
 * @tc.type: FUNC
 */
HWTEST_F(DisplayManagerTest, OnDisplayChange, Function | SmallTest | Level1)
{
    std::unique_ptr<Mocker> m = std::make_unique<Mocker>();
    EXPECT_CALL(m->Mock(), RegisterDisplayManagerAgent(_, _)).Times(1).WillOnce(Return(true));
    sptr<DisplayManager::IDisplayListener> listener = new DmMockDisplayListener();
    DisplayManager::GetInstance().RegisterDisplayListener(listener);
    auto displayManagerListener = DisplayManager::GetInstance().pImpl_->displayManagerListener_;
    ASSERT_NE(displayManagerListener, nullptr);
    DisplayChangeEvent event = DisplayChangeEvent::DISPLAY_SIZE_CHANGED;
    displayManagerListener->OnDisplayChange(nullptr, event);
    sptr<DisplayInfo> displayInfo = new DisplayInfo();
    displayInfo->SetDisplayId(DISPLAY_ID_INVALID);
    displayManagerListener->OnDisplayChange(displayInfo, event);
    displayInfo->SetDisplayId(0);
    displayManagerListener->OnDisplayChange(displayInfo, event);
    ASSERT_NE(displayManagerListener->pImpl_, nullptr);
    displayManagerListener->pImpl_ = nullptr;
    displayManagerListener->OnDisplayChange(displayInfo, event);
    DisplayManager::GetInstance().pImpl_->displayManagerListener_ = nullptr;
}

/**
 * @tc.name: CheckRectValid
 * @tc.desc: CheckRectValid all
 * @tc.type: FUNC
 */
HWTEST_F(DisplayManagerTest, CheckRectValid, Function | SmallTest | Level1)
{
    int32_t oriHeight = 500;
    int32_t oriWidth = 500;
    Media::Rect rect = {.left = 1, .top = 1, .width = 1, .height = 1};
    bool ret = DisplayManager::GetInstance().pImpl_->CheckRectValid(rect, oriHeight, oriWidth);
    ASSERT_TRUE(ret);
    rect.left = -1;
    ret = DisplayManager::GetInstance().pImpl_->CheckRectValid(rect, oriHeight, oriWidth);
    ASSERT_FALSE(ret);
    rect.left = 1;
    rect.top = -1;
    ret = DisplayManager::GetInstance().pImpl_->CheckRectValid(rect, oriHeight, oriWidth);
    ASSERT_FALSE(ret);
    rect.top = 1;
    rect.width = -1;
    ret = DisplayManager::GetInstance().pImpl_->CheckRectValid(rect, oriHeight, oriWidth);
    ASSERT_FALSE(ret);
    rect.width = 1;
    rect.height = -1;
    ret = DisplayManager::GetInstance().pImpl_->CheckRectValid(rect, oriHeight, oriWidth);
    ASSERT_FALSE(ret);
    rect.width = 500;
    rect.height = 1;
    ret = DisplayManager::GetInstance().pImpl_->CheckRectValid(rect, oriHeight, oriWidth);
    ASSERT_FALSE(ret);
    rect.width = 1;
    rect.height = 500;
    ret = DisplayManager::GetInstance().pImpl_->CheckRectValid(rect, oriHeight, oriWidth);
    ASSERT_FALSE(ret);
}

/**
 * @tc.name: CheckSizeValid
 * @tc.desc: CheckSizeValid all
 * @tc.type: FUNC
 */
HWTEST_F(DisplayManagerTest, CheckSizeValid, Function | SmallTest | Level1)
{
    int32_t oriHeight = 500;
    int32_t oriWidth = 500;
    Media::Size size = {.width = 1, .height = 1};
    bool ret = DisplayManager::GetInstance().pImpl_->CheckSizeValid(size, oriHeight, oriWidth);
    ASSERT_TRUE(ret);
    size.width = -1;
    ret = DisplayManager::GetInstance().pImpl_->CheckSizeValid(size, oriHeight, oriWidth);
    ASSERT_FALSE(ret);
    size.width = 1;
    size.height = -1;
    ret = DisplayManager::GetInstance().pImpl_->CheckSizeValid(size, oriHeight, oriWidth);
    ASSERT_FALSE(ret);
    size.width = DisplayManager::MAX_RESOLUTION_SIZE_SCREENSHOT + 1;
    size.height = 1;
    ret = DisplayManager::GetInstance().pImpl_->CheckSizeValid(size, oriHeight, oriWidth);
    ASSERT_FALSE(ret);
    size.width = DisplayManager::MAX_RESOLUTION_SIZE_SCREENSHOT;
    size.height = DisplayManager::MAX_RESOLUTION_SIZE_SCREENSHOT + 1;
    ret = DisplayManager::GetInstance().pImpl_->CheckSizeValid(size, oriHeight, oriWidth);
    ASSERT_FALSE(ret);
}

/**
 * @tc.name: ImplGetDefaultDisplay01
 * @tc.desc: Impl GetDefaultDisplay nullptr
 * @tc.type: FUNC
 */
HWTEST_F(DisplayManagerTest, ImplGetDefaultDisplay01, Function | SmallTest | Level1)
{
    std::unique_ptr<Mocker> m = std::make_unique<Mocker>();
    EXPECT_CALL(m->Mock(), GetDefaultDisplayInfo()).Times(1).WillOnce(Return(nullptr));
    sptr<Display> display = DisplayManager::GetInstance().pImpl_->GetDefaultDisplay();
    ASSERT_EQ(display, nullptr);
}

/**
 * @tc.name: GetDisplayByScreen
 * @tc.desc: for interface coverage & check GetDisplayByScreen
 * @tc.type: FUNC
 */
HWTEST_F(DisplayManagerTest, GetDisplayByScreen, Function | SmallTest | Level1)
{
    auto& displayManager = DisplayManager::GetInstance();
    sptr<Display> display = displayManager.GetDisplayByScreen(SCREEN_ID_INVALID);
    ASSERT_EQ(display, nullptr);

    sptr<DisplayInfo> displayInfo = new DisplayInfo();
    displayInfo->SetDisplayId(DISPLAY_ID_INVALID);
    std::unique_ptr<Mocker> m = std::make_unique<Mocker>();
    EXPECT_CALL(m->Mock(), GetDisplayInfoByScreenId(_)).Times(1).WillOnce(Return(displayInfo));
    display = displayManager.GetDisplayByScreen(1);
    ASSERT_EQ(display, nullptr);
}

/**
 * @tc.name: ImplGetDefaultDisplaySync
 * @tc.desc: Impl GetDefaultDisplaySync nullptr
 * @tc.type: FUNC
 */
HWTEST_F(DisplayManagerTest, ImplGetDefaultDisplaySync, Function | SmallTest | Level1)
{
    std::unique_ptr<Mocker> m = std::make_unique<Mocker>();
    EXPECT_CALL(m->Mock(), GetDefaultDisplayInfo()).Times(1).WillOnce(Return(nullptr));
    sptr<Display> display = DisplayManager::GetInstance().GetDefaultDisplaySync();
    ASSERT_EQ(display, nullptr);
}
}
} // namespace Rosen
} // namespace OHOS