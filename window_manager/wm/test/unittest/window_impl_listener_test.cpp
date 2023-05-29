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

#include <gtest/gtest.h>
#include "window_impl.h"

using namespace testing;
using namespace testing::ext;

namespace OHOS {
namespace Rosen {
class WindowImplListenerTest : public testing::Test {
public:
    static void SetUpTestCase();
    static void TearDownTestCase();
    virtual void SetUp() override;
    virtual void TearDown() override;
    sptr<WindowImpl> window_;
};

void WindowImplListenerTest::SetUpTestCase()
{
}

void WindowImplListenerTest::TearDownTestCase()
{
}

void WindowImplListenerTest::SetUp()
{
    sptr<WindowOption> option = new WindowOption();
    option->SetWindowName("WindowTestListener");
    window_ = new WindowImpl(option);
}

void WindowImplListenerTest::TearDown()
{
    window_ = nullptr;
}

namespace {
/**
 * @tc.name: BasicRegisterUnregister
 * @tc.desc: test the basic method of register/unregister
 * @tc.type: FUNC
 */
HWTEST_F(WindowImplListenerTest, BasicRegisterUnregister, Function | SmallTest | Level2)
{
    std::vector<sptr<IWindowLifeCycle>> holder;
    sptr<IWindowLifeCycle> listener = nullptr;
    window_->RegisterListener(holder, listener);
    ASSERT_EQ(holder.size(), 0);

    listener = new IWindowLifeCycle();
    window_->RegisterListener(holder, listener);
    ASSERT_EQ(holder.size(), 1);
    window_->RegisterListener(holder, listener);
    ASSERT_EQ(holder.size(), 1);
    window_->UnregisterListener(holder, listener);
    ASSERT_EQ(holder.size(), 0);
}

/**
 * @tc.name: InterfacesRegisterUnregister
 * @tc.desc: test all interfaces listener
 * @tc.type: FUNC
 */
HWTEST_F(WindowImplListenerTest, InterfacesRegisterUnregister, Function | SmallTest | Level2)
{
    sptr<IWindowLifeCycle> windowLifeCycle = new IWindowLifeCycle();
    window_->RegisterLifeCycleListener(windowLifeCycle);
    ASSERT_EQ(window_->lifecycleListeners_[window_->GetWindowId()].size(), 1);
    window_->UnregisterLifeCycleListener(windowLifeCycle);
    ASSERT_EQ(window_->lifecycleListeners_[window_->GetWindowId()].size(), 0);

    sptr<IWindowChangeListener> windowChangeListener = new IWindowChangeListener();
    window_->RegisterWindowChangeListener(windowChangeListener);
    ASSERT_EQ(window_->windowChangeListeners_[window_->GetWindowId()].size(), 1);
    window_->UnregisterWindowChangeListener(windowChangeListener);
    ASSERT_EQ(window_->windowChangeListeners_[window_->GetWindowId()].size(), 0);

    sptr<IAvoidAreaChangedListener> avoidAreaChangedListener = new IAvoidAreaChangedListener();
    window_->RegisterAvoidAreaChangeListener(avoidAreaChangedListener);
    ASSERT_EQ(window_->avoidAreaChangeListeners_[window_->GetWindowId()].size(), 1);
    window_->UnregisterAvoidAreaChangeListener(avoidAreaChangedListener);
    ASSERT_EQ(window_->avoidAreaChangeListeners_[window_->GetWindowId()].size(), 0);

    sptr<IWindowDragListener> windowDragListener = new IWindowDragListener();
    window_->RegisterDragListener(windowDragListener);
    ASSERT_EQ(window_->windowDragListeners_.size(), 1);
    window_->UnregisterDragListener(windowDragListener);
    ASSERT_EQ(window_->windowDragListeners_.size(), 0);

    sptr<IDisplayMoveListener> displayMoveListener = new IDisplayMoveListener();
    window_->RegisterDisplayMoveListener(displayMoveListener);
    ASSERT_EQ(window_->displayMoveListeners_.size(), 1);
    window_->UnregisterDisplayMoveListener(displayMoveListener);
    ASSERT_EQ(window_->displayMoveListeners_.size(), 0);

    sptr<IOccupiedAreaChangeListener> occupiedAreaChangeListener = new IOccupiedAreaChangeListener();
    window_->RegisterOccupiedAreaChangeListener(occupiedAreaChangeListener);
    ASSERT_EQ(window_->occupiedAreaChangeListeners_[window_->GetWindowId()].size(), 1);
    window_->UnregisterOccupiedAreaChangeListener(occupiedAreaChangeListener);
    ASSERT_EQ(window_->occupiedAreaChangeListeners_[window_->GetWindowId()].size(), 0);

    sptr<ITouchOutsideListener> touchOutsideListener = new ITouchOutsideListener();
    window_->RegisterTouchOutsideListener(touchOutsideListener);
    ASSERT_EQ(window_->touchOutsideListeners_[window_->GetWindowId()].size(), 1);
    window_->UnregisterTouchOutsideListener(touchOutsideListener);
    ASSERT_EQ(window_->touchOutsideListeners_[window_->GetWindowId()].size(), 0);

    sptr<IScreenshotListener> screenshotListener = new IScreenshotListener();
    window_->RegisterScreenshotListener(screenshotListener);
    ASSERT_EQ(window_->screenshotListeners_[window_->GetWindowId()].size(), 1);
    window_->UnregisterScreenshotListener(screenshotListener);
    ASSERT_EQ(window_->screenshotListeners_[window_->GetWindowId()].size(), 0);

    sptr<IDialogTargetTouchListener> dialogTargetTouchListener = new IDialogTargetTouchListener();
    window_->RegisterDialogTargetTouchListener(dialogTargetTouchListener);
    ASSERT_EQ(window_->dialogTargetTouchListeners_[window_->GetWindowId()].size(), 1);
    window_->UnregisterDialogTargetTouchListener(dialogTargetTouchListener);
    ASSERT_EQ(window_->dialogTargetTouchListeners_[window_->GetWindowId()].size(), 0);
}
}
} // namespace Rosen
} // namespace OHOS
