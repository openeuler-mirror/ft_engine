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
#include "window_manager_proxy.h"
#include "window_manager_stub_impl.h"
#include "iremote_object_mocker.h"

using namespace testing;
using namespace testing::ext;
namespace OHOS {
namespace Rosen {
class WindowManagerProxyTest : public testing::Test {
public:
    static void SetUpTestCase();
    static void TearDownTestCase();
    void SetUp() override;
    void TearDown() override;
    sptr<WindowManagerStubImpl> mockWindowManagerStub_;
    sptr<WindowManagerProxy> windowManagerProxy_;
};

void WindowManagerProxyTest::SetUpTestCase()
{
}

void WindowManagerProxyTest::TearDownTestCase()
{
}

void WindowManagerProxyTest::SetUp()
{
    mockWindowManagerStub_ = new WindowManagerStubImpl();
    windowManagerProxy_ = new WindowManagerProxy(mockWindowManagerStub_);
}

void WindowManagerProxyTest::TearDown()
{
}

namespace {
/**
 * @tc.name: RequestFocus
 * @tc.desc: test success
 * @tc.type: FUNC
 */
HWTEST_F(WindowManagerProxyTest, RequestFocus, Function | SmallTest | Level2)
{
    uint32_t windowId = 0;
    WMError err = windowManagerProxy_->RequestFocus(windowId);
    ASSERT_EQ(err, WMError::WM_OK);
}

/**
 * @tc.name: SetWindowAnimationController
 * @tc.desc: test failed
 * @tc.type: FUNC
 */
HWTEST_F(WindowManagerProxyTest, SetWindowAnimationController, Function | SmallTest | Level2)
{
    sptr<RSIWindowAnimationController> controller = nullptr;
    WMError err = windowManagerProxy_->SetWindowAnimationController(controller);
    ASSERT_EQ(err, WMError::WM_ERROR_IPC_FAILED);
}

/**
 * @tc.name: SetWindowAnimationController01
 * @tc.desc: test success
 * @tc.type: FUNC
 */
HWTEST_F(WindowManagerProxyTest, SetWindowAnimationController01, Function | SmallTest | Level2)
{
    sptr<IRemoteObject> iRemoteObjectMocker = new IRemoteObjectMocker();
    sptr<RSIWindowAnimationController> controller = iface_cast<RSIWindowAnimationController>(iRemoteObjectMocker);
    WMError err = windowManagerProxy_->SetWindowAnimationController(controller);
    ASSERT_EQ(err, WMError::WM_OK);
}

/**
 * @tc.name: ToggleShownStateForAllAppWindows
 * @tc.desc: test success
 * @tc.type: FUNC
 */
HWTEST_F(WindowManagerProxyTest, ToggleShownStateForAllAppWindows, Function | SmallTest | Level2)
{
    WMError err = windowManagerProxy_->ToggleShownStateForAllAppWindows();
    ASSERT_EQ(err, WMError::WM_OK);
}

/**
 * @tc.name: GetTopWindowId
 * @tc.desc: test success
 * @tc.type: FUNC
 */
HWTEST_F(WindowManagerProxyTest, GetTopWindowId, Function | SmallTest | Level2)
{
    uint32_t mainWinId = 0;
    uint32_t topWinId;
    WMError err = windowManagerProxy_->GetTopWindowId(mainWinId, topWinId);
    ASSERT_EQ(err, WMError::WM_OK);
}

/**
 * @tc.name: NotifyWindowTransition
 * @tc.desc: test success
 * @tc.type: FUNC
 */
HWTEST_F(WindowManagerProxyTest, NotifyWindowTransition, Function | SmallTest | Level2)
{
    sptr<WindowTransitionInfo> from = new WindowTransitionInfo();
    sptr<WindowTransitionInfo> to = new WindowTransitionInfo();
    bool isFromClient = false;
    WMError err = windowManagerProxy_->NotifyWindowTransition(from, to, isFromClient);
    ASSERT_EQ(err, WMError::WM_OK);
}

/**
 * @tc.name: GetModeChangeHotZones
 * @tc.desc: test success
 * @tc.type: FUNC
 */
HWTEST_F(WindowManagerProxyTest, GetModeChangeHotZones, Function | SmallTest | Level2)
{
    DisplayId displayId = 10;
    ModeChangeHotZones hotZones;
    WMError err = windowManagerProxy_->GetModeChangeHotZones(displayId, hotZones);
    ASSERT_EQ(err, WMError::WM_OK);
}

/**
 * @tc.name: MinimizeWindowsByLauncher
 * @tc.desc: test success
 * @tc.type: FUNC
 */
HWTEST_F(WindowManagerProxyTest, MinimizeWindowsByLauncher, Function | SmallTest | Level2)
{
    std::vector<uint32_t> windowIds;
    bool isAnimated = false;
    sptr<RSIWindowAnimationFinishedCallback> finishCallback;
    windowManagerProxy_->MinimizeWindowsByLauncher(windowIds, isAnimated, finishCallback);
    ASSERT_EQ(finishCallback, nullptr);
}

/**
 * @tc.name: MinimizeWindowsByLauncher01
 * @tc.desc: test success
 * @tc.type: FUNC
 */
HWTEST_F(WindowManagerProxyTest, MinimizeWindowsByLauncher01, Function | SmallTest | Level2)
{
    std::vector<uint32_t> windowIds;
    windowIds.push_back(0);
    windowIds.push_back(1);
    bool isAnimated = false;
    sptr<RSIWindowAnimationFinishedCallback> finishCallback;
    windowManagerProxy_->MinimizeWindowsByLauncher(windowIds, isAnimated, finishCallback);
    ASSERT_EQ(finishCallback, nullptr);
}

/**
 * @tc.name: UpdateRsTree
 * @tc.desc: test success
 * @tc.type: FUNC
 */
HWTEST_F(WindowManagerProxyTest, UpdateRsTree, Function | SmallTest | Level2)
{
    uint32_t windowId = 0;
    bool isAdd = false;
    WMError err = windowManagerProxy_->UpdateRsTree(windowId, isAdd);
    ASSERT_EQ(err, WMError::WM_OK);
}

/**
 * @tc.name: BindDialogTarget
 * @tc.desc: test success
 * @tc.type: FUNC
 */
HWTEST_F(WindowManagerProxyTest, BindDialogTarget, Function | SmallTest | Level2)
{
    uint32_t windowId = 0;
    sptr<IRemoteObject> targetToken = nullptr;
    WMError err = windowManagerProxy_->BindDialogTarget(windowId, targetToken);
    ASSERT_EQ(err, WMError::WM_OK);
}

/**
 * @tc.name: BindDialogTarget01
 * @tc.desc: test success
 * @tc.type: FUNC
 */
HWTEST_F(WindowManagerProxyTest, BindDialogTarget01, Function | SmallTest | Level2)
{
    uint32_t windowId = 0;
    sptr<IRemoteObject> targetToken = nullptr;
    WMError err = windowManagerProxy_->BindDialogTarget(windowId, targetToken);
    ASSERT_EQ(err, WMError::WM_OK);
}

/**
 * @tc.name: GetVisibilityWindowInfo01
 * @tc.desc: test success
 * @tc.type: FUNC
 */
HWTEST_F(WindowManagerProxyTest, GetVisibilityWindowInfo01, Function | SmallTest | Level2)
{
    std::vector<sptr<WindowVisibilityInfo>> infos;
    WMError err = windowManagerProxy_->GetVisibilityWindowInfo(infos);
    ASSERT_EQ(err, WMError::WM_OK);
}

}
}
}
