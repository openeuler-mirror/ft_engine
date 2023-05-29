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

#include <gtest/gtest.h>
#include "window_manager.h"
#include "mock_window_adapter.h"
#include "singleton_mocker.h"

#include "window_manager.cpp"

using namespace testing;
using namespace testing::ext;

namespace OHOS {
namespace Rosen {
using Mocker = SingletonMocker<WindowAdapter, MockWindowAdapter>;
class TestCameraFloatWindowChangedListener : public ICameraFloatWindowChangedListener {
public:
    void OnCameraFloatWindowChange(uint32_t accessTokenId, bool isShowing) override
    {
        WLOGFI("TestCameraFloatWindowChangedListener [%{public}u, %{public}u]", accessTokenId, isShowing);
    };
};

class TestVisibilityChangedListener : public IVisibilityChangedListener {
public:
    void OnWindowVisibilityChanged(const std::vector<sptr<WindowVisibilityInfo>>& windowVisibilityInfo) override
    {
        WLOGFI("TestVisibilityChangedListener");
    };
};

class TestSystemBarChangedListener : public ISystemBarChangedListener {
public:
    void OnSystemBarPropertyChange(DisplayId displayId, const SystemBarRegionTints& tints) override
    {
        WLOGFI("TestSystemBarChangedListener");
    };
};

class TestWindowUpdateListener : public IWindowUpdateListener {
public:
    void OnWindowUpdate(const std::vector<sptr<AccessibilityWindowInfo>>& infos, WindowUpdateType type) override
    {
        WLOGFI("TestWindowUpdateListener");
    };
};

class WindowManagerTest : public testing::Test {
public:
    static void SetUpTestCase();
    static void TearDownTestCase();
    void SetUp() override;
    void TearDown() override;
};

void WindowManagerTest::SetUpTestCase()
{
}

void WindowManagerTest::TearDownTestCase()
{
}

void WindowManagerTest::SetUp()
{
}

void WindowManagerTest::TearDown()
{
}

namespace {
/**
 * @tc.name: Create01
 * @tc.desc: Create window with no WindowName and no abilityToken
 * @tc.type: FUNC
 */
HWTEST_F(WindowManagerTest, GetVisibilityWindowInfo01, Function | SmallTest | Level2)
{
    std::unique_ptr<Mocker> m = std::make_unique<Mocker>();
    std::vector<sptr<WindowVisibilityInfo>> infos;
    infos.clear();
    EXPECT_CALL(m->Mock(), GetVisibilityWindowInfo(_)).Times(1).WillOnce(Return(WMError::WM_OK));
    ASSERT_EQ(WMError::WM_OK, WindowManager::GetInstance().GetVisibilityWindowInfo(infos));
    EXPECT_CALL(m->Mock(), GetVisibilityWindowInfo(_)).Times(1).WillOnce(Return(WMError::WM_ERROR_INVALID_WINDOW));
    ASSERT_EQ(WMError::WM_ERROR_INVALID_WINDOW, WindowManager::GetInstance().GetVisibilityWindowInfo(infos));
}

/**
 * @tc.name: Create01
 * @tc.desc: Create window with no WindowName and no abilityToken
 * @tc.type: FUNC
 */
HWTEST_F(WindowManagerTest, GetAccessibilityWindowInfo01, Function | SmallTest | Level2)
{
    std::unique_ptr<Mocker> m = std::make_unique<Mocker>();
    std::vector<sptr<AccessibilityWindowInfo>> infos;
    infos.clear();
    EXPECT_CALL(m->Mock(), GetAccessibilityWindowInfo(_)).Times(1).WillOnce(Return(WMError::WM_OK));
    ASSERT_EQ(WMError::WM_OK, WindowManager::GetInstance().GetAccessibilityWindowInfo(infos));
    EXPECT_CALL(m->Mock(), GetAccessibilityWindowInfo(_)).Times(1).WillOnce(Return(WMError::WM_ERROR_INVALID_WINDOW));
    ASSERT_EQ(WMError::WM_ERROR_INVALID_WINDOW, WindowManager::GetInstance().GetAccessibilityWindowInfo(infos));
}

/**
 * @tc.name: RegisterCameraFloatWindowChangedListener01
 * @tc.desc: check RegisterCameraFloatWindowChangedListener
 * @tc.type: FUNC
 */
HWTEST_F(WindowManagerTest, RegisterCameraFloatWindowChangedListener01, Function | SmallTest | Level2)
{
    auto& windowManager = WindowManager::GetInstance();
    auto oldWindowManagerAgent = windowManager.pImpl_->cameraFloatWindowChangedListenerAgent_;
    auto oldListeners = windowManager.pImpl_->cameraFloatWindowChangedListeners_;
    windowManager.pImpl_->cameraFloatWindowChangedListenerAgent_ = nullptr;
    windowManager.pImpl_->cameraFloatWindowChangedListeners_.clear();
    ASSERT_EQ(false, windowManager.RegisterCameraFloatWindowChangedListener(nullptr));

    sptr<TestCameraFloatWindowChangedListener> listener = new TestCameraFloatWindowChangedListener();
    std::unique_ptr<Mocker> m = std::make_unique<Mocker>();
    EXPECT_CALL(m->Mock(), RegisterWindowManagerAgent(_, _)).Times(1).WillOnce(Return(false));
    ASSERT_EQ(false, windowManager.RegisterCameraFloatWindowChangedListener(listener));

    EXPECT_CALL(m->Mock(), RegisterWindowManagerAgent(_, _)).Times(1).WillOnce(Return(true));
    ASSERT_EQ(true, windowManager.RegisterCameraFloatWindowChangedListener(listener));
    ASSERT_EQ(1, windowManager.pImpl_->cameraFloatWindowChangedListeners_.size());

    // to check that the same listner can not be registered twice
    ASSERT_EQ(true, windowManager.RegisterCameraFloatWindowChangedListener(listener));
    ASSERT_EQ(1, windowManager.pImpl_->cameraFloatWindowChangedListeners_.size());

    windowManager.pImpl_->cameraFloatWindowChangedListenerAgent_ = oldWindowManagerAgent;
    windowManager.pImpl_->cameraFloatWindowChangedListeners_ = oldListeners;
}
/**
 * @tc.name: UnregisterCameraFloatWindowChangedListener01
 * @tc.desc: check UnregisterCameraFloatWindowChangedListener
 * @tc.type: FUNC
 */
HWTEST_F(WindowManagerTest, UnregisterCameraFloatWindowChangedListener01, Function | SmallTest | Level2)
{
    auto& windowManager = WindowManager::GetInstance();
    auto oldWindowManagerAgent = windowManager.pImpl_->cameraFloatWindowChangedListenerAgent_;
    auto oldListeners = windowManager.pImpl_->cameraFloatWindowChangedListeners_;
    windowManager.pImpl_->cameraFloatWindowChangedListenerAgent_ = new WindowManagerAgent();
    windowManager.pImpl_->cameraFloatWindowChangedListeners_.clear();

    // check nullpter
    ASSERT_EQ(false, windowManager.UnregisterCameraFloatWindowChangedListener(nullptr));

    sptr<TestCameraFloatWindowChangedListener> listener1 = new TestCameraFloatWindowChangedListener();
    sptr<TestCameraFloatWindowChangedListener> listener2 = new TestCameraFloatWindowChangedListener();
    ASSERT_EQ(true, windowManager.UnregisterCameraFloatWindowChangedListener(listener1));

    windowManager.RegisterCameraFloatWindowChangedListener(listener1);
    windowManager.RegisterCameraFloatWindowChangedListener(listener2);
    ASSERT_EQ(2, windowManager.pImpl_->cameraFloatWindowChangedListeners_.size());

    ASSERT_EQ(true, windowManager.UnregisterCameraFloatWindowChangedListener(listener1));

    ASSERT_EQ(false, windowManager.UnregisterCameraFloatWindowChangedListener(listener2));
    ASSERT_EQ(0, windowManager.pImpl_->cameraFloatWindowChangedListeners_.size());
    ASSERT_EQ(nullptr, windowManager.pImpl_->cameraFloatWindowChangedListenerAgent_);

    windowManager.pImpl_->cameraFloatWindowChangedListeners_.emplace_back(listener1);
    ASSERT_EQ(true, windowManager.UnregisterCameraFloatWindowChangedListener(listener1));
    ASSERT_EQ(0, windowManager.pImpl_->cameraFloatWindowChangedListeners_.size());

    windowManager.pImpl_->cameraFloatWindowChangedListenerAgent_ = oldWindowManagerAgent;
    windowManager.pImpl_->cameraFloatWindowChangedListeners_ = oldListeners;
}

/**
 * @tc.name: RegisterVisibilityChangedListener01
 * @tc.desc: check RegisterVisibilityChangedListener
 * @tc.type: FUNC
 */
HWTEST_F(WindowManagerTest, RegisterVisibilityChangedListener01, Function | SmallTest | Level2)
{
    auto& windowManager = WindowManager::GetInstance();
    auto oldWindowManagerAgent = windowManager.pImpl_->windowVisibilityListenerAgent_;
    auto oldListeners = windowManager.pImpl_->windowVisibilityListeners_;
    windowManager.pImpl_->windowVisibilityListenerAgent_ = nullptr;
    windowManager.pImpl_->windowVisibilityListeners_.clear();
    ASSERT_EQ(false, windowManager.RegisterVisibilityChangedListener(nullptr));

    sptr<TestVisibilityChangedListener> listener = new TestVisibilityChangedListener();
    std::unique_ptr<Mocker> m = std::make_unique<Mocker>();
    EXPECT_CALL(m->Mock(), RegisterWindowManagerAgent(_, _)).Times(1).WillOnce(Return(false));
    ASSERT_EQ(false, windowManager.RegisterVisibilityChangedListener(listener));
    ASSERT_EQ(nullptr, windowManager.pImpl_->windowVisibilityListenerAgent_);

    EXPECT_CALL(m->Mock(), RegisterWindowManagerAgent(_, _)).Times(1).WillOnce(Return(true));
    ASSERT_EQ(true, windowManager.RegisterVisibilityChangedListener(listener));
    ASSERT_EQ(1, windowManager.pImpl_->windowVisibilityListeners_.size());

    // to check that the same listner can not be registered twice
    ASSERT_EQ(true, windowManager.RegisterVisibilityChangedListener(listener));
    ASSERT_EQ(1, windowManager.pImpl_->windowVisibilityListeners_.size());

    windowManager.pImpl_->windowVisibilityListenerAgent_ = oldWindowManagerAgent;
    windowManager.pImpl_->windowVisibilityListeners_ = oldListeners;
}
/**
 * @tc.name: UnregisterVisibilityChangedListener01
 * @tc.desc: check UnregisterVisibilityChangedListener
 * @tc.type: FUNC
 */
HWTEST_F(WindowManagerTest, UnregisterVisibilityChangedListener01, Function | SmallTest | Level2)
{
    auto& windowManager = WindowManager::GetInstance();
    auto oldWindowManagerAgent = windowManager.pImpl_->windowVisibilityListenerAgent_;
    auto oldListeners = windowManager.pImpl_->windowVisibilityListeners_;
    windowManager.pImpl_->windowVisibilityListenerAgent_ = new WindowManagerAgent();
    windowManager.pImpl_->windowVisibilityListeners_.clear();

    // check nullpter
    ASSERT_EQ(false, windowManager.UnregisterVisibilityChangedListener(nullptr));

    sptr<TestVisibilityChangedListener> listener1 = new TestVisibilityChangedListener();
    sptr<TestVisibilityChangedListener> listener2 = new TestVisibilityChangedListener();

    windowManager.RegisterVisibilityChangedListener(listener1);
    windowManager.RegisterVisibilityChangedListener(listener2);
    ASSERT_EQ(2, windowManager.pImpl_->windowVisibilityListeners_.size());

    ASSERT_EQ(true, windowManager.UnregisterVisibilityChangedListener(listener1));
    ASSERT_EQ(false, windowManager.UnregisterVisibilityChangedListener(listener2));
    ASSERT_EQ(0, windowManager.pImpl_->windowVisibilityListeners_.size());
    ASSERT_EQ(nullptr, windowManager.pImpl_->windowVisibilityListenerAgent_);

    windowManager.pImpl_->windowVisibilityListeners_.emplace_back(listener1);
    ASSERT_EQ(true, windowManager.UnregisterVisibilityChangedListener(listener1));
    ASSERT_EQ(0, windowManager.pImpl_->windowVisibilityListeners_.size());

    windowManager.pImpl_->windowVisibilityListenerAgent_ = oldWindowManagerAgent;
    windowManager.pImpl_->windowVisibilityListeners_ = oldListeners;
}

/**
 * @tc.name: RegisterWindowUpdateListener01
 * @tc.desc: check RegisterWindowUpdateListener
 * @tc.type: FUNC
 */
HWTEST_F(WindowManagerTest, RegisterWindowUpdateListener01, Function | SmallTest | Level2)
{
    auto& windowManager = WindowManager::GetInstance();
    auto oldWindowManagerAgent = windowManager.pImpl_->windowUpdateListenerAgent_;
    auto oldListeners = windowManager.pImpl_->windowUpdateListeners_;
    windowManager.pImpl_->windowUpdateListenerAgent_ = nullptr;
    windowManager.pImpl_->windowUpdateListeners_.clear();
    ASSERT_EQ(false, windowManager.RegisterWindowUpdateListener(nullptr));

    sptr<TestWindowUpdateListener> listener = new TestWindowUpdateListener();
    std::unique_ptr<Mocker> m = std::make_unique<Mocker>();
    EXPECT_CALL(m->Mock(), RegisterWindowManagerAgent(_, _)).Times(1).WillOnce(Return(false));
    ASSERT_EQ(false, windowManager.RegisterWindowUpdateListener(listener));
    ASSERT_EQ(nullptr, windowManager.pImpl_->windowUpdateListenerAgent_);

    EXPECT_CALL(m->Mock(), RegisterWindowManagerAgent(_, _)).Times(1).WillOnce(Return(true));
    ASSERT_EQ(true, windowManager.RegisterWindowUpdateListener(listener));
    ASSERT_EQ(1, windowManager.pImpl_->windowUpdateListeners_.size());

    // to check that the same listner can not be registered twice
    ASSERT_EQ(true, windowManager.RegisterWindowUpdateListener(listener));
    ASSERT_EQ(1, windowManager.pImpl_->windowUpdateListeners_.size());

    windowManager.pImpl_->windowUpdateListenerAgent_ = oldWindowManagerAgent;
    windowManager.pImpl_->windowUpdateListeners_ = oldListeners;
}
/**
 * @tc.name: UnregisterWindowUpdateListener01
 * @tc.desc: check UnregisterWindowUpdateListener
 * @tc.type: FUNC
 */
HWTEST_F(WindowManagerTest, UnregisterWindowUpdateListener01, Function | SmallTest | Level2)
{
    auto& windowManager = WindowManager::GetInstance();
    auto oldWindowManagerAgent = windowManager.pImpl_->windowUpdateListenerAgent_;
    auto oldListeners = windowManager.pImpl_->windowUpdateListeners_;
    windowManager.pImpl_->windowUpdateListenerAgent_ = new WindowManagerAgent();
    windowManager.pImpl_->windowUpdateListeners_.clear();

    // check nullpter
    ASSERT_EQ(false, windowManager.UnregisterWindowUpdateListener(nullptr));

    sptr<TestWindowUpdateListener> listener1 = new TestWindowUpdateListener();
    sptr<TestWindowUpdateListener> listener2 = new TestWindowUpdateListener();
    ASSERT_EQ(true, windowManager.UnregisterWindowUpdateListener(listener1));

    windowManager.RegisterWindowUpdateListener(listener1);
    windowManager.RegisterWindowUpdateListener(listener2);
    ASSERT_EQ(2, windowManager.pImpl_->windowUpdateListeners_.size());

    ASSERT_EQ(true, windowManager.UnregisterWindowUpdateListener(listener1));

    ASSERT_EQ(false, windowManager.UnregisterWindowUpdateListener(listener2));
    ASSERT_EQ(0, windowManager.pImpl_->windowUpdateListeners_.size());
    ASSERT_EQ(nullptr, windowManager.pImpl_->windowUpdateListenerAgent_);

    windowManager.pImpl_->windowUpdateListeners_.emplace_back(listener1);
    ASSERT_EQ(true, windowManager.UnregisterWindowUpdateListener(listener1));
    ASSERT_EQ(0, windowManager.pImpl_->windowUpdateListeners_.size());

    windowManager.pImpl_->windowUpdateListenerAgent_ = oldWindowManagerAgent;
    windowManager.pImpl_->windowUpdateListeners_ = oldListeners;
}

/**
 * @tc.name: RegisterSystemBarChangedListener01
 * @tc.desc: check RegisterSystemBarChangedListener
 * @tc.type: FUNC
 */
HWTEST_F(WindowManagerTest, RegisterSystemBarChangedListener01, Function | SmallTest | Level2)
{
    auto& windowManager = WindowManager::GetInstance();
    auto oldWindowManagerAgent = windowManager.pImpl_->systemBarChangedListenerAgent_;
    auto oldListeners = windowManager.pImpl_->systemBarChangedListeners_;
    windowManager.pImpl_->systemBarChangedListenerAgent_ = nullptr;
    windowManager.pImpl_->systemBarChangedListeners_.clear();
    ASSERT_EQ(false, windowManager.RegisterSystemBarChangedListener(nullptr));

    sptr<ISystemBarChangedListener> listener = new TestSystemBarChangedListener();
    std::unique_ptr<Mocker> m = std::make_unique<Mocker>();
    EXPECT_CALL(m->Mock(), RegisterWindowManagerAgent(_, _)).Times(1).WillOnce(Return(false));
    ASSERT_EQ(false, windowManager.RegisterSystemBarChangedListener(listener));
    ASSERT_EQ(nullptr, windowManager.pImpl_->systemBarChangedListenerAgent_);

    EXPECT_CALL(m->Mock(), RegisterWindowManagerAgent(_, _)).Times(1).WillOnce(Return(true));
    ASSERT_EQ(true, windowManager.RegisterSystemBarChangedListener(listener));
    ASSERT_EQ(1, windowManager.pImpl_->systemBarChangedListeners_.size());

    // to check that the same listner can not be registered twice
    ASSERT_EQ(true, windowManager.RegisterSystemBarChangedListener(listener));
    ASSERT_EQ(1, windowManager.pImpl_->systemBarChangedListeners_.size());

    windowManager.pImpl_->systemBarChangedListenerAgent_ = oldWindowManagerAgent;
    windowManager.pImpl_->systemBarChangedListeners_ = oldListeners;
}
/**
 * @tc.name: UnregisterSystemBarChangedListener01
 * @tc.desc: check UnregisterSystemBarChangedListener
 * @tc.type: FUNC
 */
HWTEST_F(WindowManagerTest, UnregisterSystemBarChangedListener01, Function | SmallTest | Level2)
{
    auto& windowManager = WindowManager::GetInstance();
    auto oldWindowManagerAgent = windowManager.pImpl_->systemBarChangedListenerAgent_;
    auto oldListeners = windowManager.pImpl_->systemBarChangedListeners_;
    windowManager.pImpl_->systemBarChangedListenerAgent_ = new WindowManagerAgent();
    windowManager.pImpl_->systemBarChangedListeners_.clear();
    // check nullpter
    ASSERT_EQ(false, windowManager.UnregisterSystemBarChangedListener(nullptr));

    sptr<TestSystemBarChangedListener> listener1 = new TestSystemBarChangedListener();
    sptr<TestSystemBarChangedListener> listener2 = new TestSystemBarChangedListener();
    ASSERT_EQ(true, windowManager.UnregisterSystemBarChangedListener(listener1));

    windowManager.RegisterSystemBarChangedListener(listener1);
    windowManager.RegisterSystemBarChangedListener(listener2);
    ASSERT_EQ(2, windowManager.pImpl_->systemBarChangedListeners_.size());


    ASSERT_EQ(true, windowManager.UnregisterSystemBarChangedListener(listener1));
    ASSERT_EQ(false, windowManager.UnregisterSystemBarChangedListener(listener2));
    ASSERT_EQ(0, windowManager.pImpl_->systemBarChangedListeners_.size());
    ASSERT_EQ(nullptr, windowManager.pImpl_->systemBarChangedListenerAgent_);

    windowManager.pImpl_->systemBarChangedListeners_.push_back(listener1);
    ASSERT_EQ(true, windowManager.UnregisterSystemBarChangedListener(listener1));
    ASSERT_EQ(0, windowManager.pImpl_->systemBarChangedListeners_.size());

    windowManager.pImpl_->systemBarChangedListenerAgent_ = oldWindowManagerAgent;
    windowManager.pImpl_->systemBarChangedListeners_ = oldListeners;
}
/**
 * @tc.name: ToggleShownStateForAllAppWindows
 * @tc.desc: ToggleShownStateForAllAppWindows ok
 * @tc.type: FUNC
 */
HWTEST_F(WindowManagerTest, ToggleShownStateForAllAppWindows, Function | SmallTest | Level2)
{
    std::unique_ptr<Mocker> m = std::make_unique<Mocker>();
    EXPECT_CALL(m->Mock(), ToggleShownStateForAllAppWindows()).Times(1).WillOnce(Return(WMError::WM_OK));
    ASSERT_EQ(WMError::WM_OK, WindowManager::GetInstance().ToggleShownStateForAllAppWindows());
}
}
} // namespace Rosen
} // namespace OHOS
