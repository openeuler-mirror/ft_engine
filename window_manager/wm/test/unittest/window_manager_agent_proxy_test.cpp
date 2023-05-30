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
#include "window_manager_agent_proxy.h"
#include "window_manager_agent.h"

using namespace testing;
using namespace testing::ext;

namespace OHOS {
namespace Rosen {
class WindowManagerAgentProxyTest : public testing::Test {
public:
    static void SetUpTestCase();
    static void TearDownTestCase();
    void SetUp() override;
    void TearDown() override;
    sptr<WindowManagerAgent> mockWindowManagerAgent_;
    sptr<WindowManagerAgentProxy> windowManagerAgentProxy_;
};

void WindowManagerAgentProxyTest::SetUpTestCase()
{
}

void WindowManagerAgentProxyTest::TearDownTestCase()
{
}

void WindowManagerAgentProxyTest::SetUp()
{
    mockWindowManagerAgent_ = new WindowManagerAgent();
    windowManagerAgentProxy_ = new WindowManagerAgentProxy(mockWindowManagerAgent_);
}

void WindowManagerAgentProxyTest::TearDown()
{
}

namespace {
/**
 * @tc.name: UpdateFocusChangeInfo01
 * @tc.desc: test success
 * @tc.type: FUNC
 */
HWTEST_F(WindowManagerAgentProxyTest, UpdateFocusChangeInfo01, Function | SmallTest | Level2)
{
    sptr<FocusChangeInfo> focusChangeInfo;
    windowManagerAgentProxy_->UpdateFocusChangeInfo(focusChangeInfo, false);
}

/**
 * @tc.name: UpdateFocusChangeInfo02
 * @tc.desc: test failed
 * @tc.type: FUNC
 */
HWTEST_F(WindowManagerAgentProxyTest, UpdateFocusChangeInfo02, Function | SmallTest | Level2)
{
    sptr<FocusChangeInfo> focusChangeInfo = nullptr;
    windowManagerAgentProxy_->UpdateFocusChangeInfo(focusChangeInfo, false);
}

/**
 * @tc.name: UpdateSystemBarRegionTints01
 * @tc.desc: test success
 * @tc.type: FUNC
 */
HWTEST_F(WindowManagerAgentProxyTest, UpdateSystemBarRegionTints01, Function | SmallTest | Level2)
{
    SystemBarRegionTints tints;
    windowManagerAgentProxy_->UpdateSystemBarRegionTints(0, tints);
}

/**
 * @tc.name: NotifyAccessibilityWindowInfo01
 * @tc.desc: test success
 * @tc.type: FUNC
 */
HWTEST_F(WindowManagerAgentProxyTest, NotifyAccessibilityWindowInfo01, Function | SmallTest | Level2)
{
    std::vector<sptr<AccessibilityWindowInfo>> info;
    WindowUpdateType type = WindowUpdateType::WINDOW_UPDATE_ADDED;
    windowManagerAgentProxy_->NotifyAccessibilityWindowInfo(info, type);
}

/**
 * @tc.name: UpdateWindowVisibilityInfo01
 * @tc.desc: test success
 * @tc.type: FUNC
 */
HWTEST_F(WindowManagerAgentProxyTest, UpdateWindowVisibilityInfo01, Function | SmallTest | Level2)
{
    std::vector<sptr<WindowVisibilityInfo>> visibilityInfos;
    windowManagerAgentProxy_->UpdateWindowVisibilityInfo(visibilityInfos);
}

/**
 * @tc.name: UpdateCameraFloatWindowStatus01
 * @tc.desc: test success
 * @tc.type: FUNC
 */
HWTEST_F(WindowManagerAgentProxyTest, UpdateCameraFloatWindowStatus01, Function | SmallTest | Level2)
{
    windowManagerAgentProxy_->UpdateCameraFloatWindowStatus(100, false);
}

}
}
}
