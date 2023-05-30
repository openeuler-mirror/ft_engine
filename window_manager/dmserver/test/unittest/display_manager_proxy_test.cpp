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

#include <iremote_broker.h>
#include <iremote_object.h>
#include "display_manager_proxy.h"
#include "iremote_object_mocker.h"

#include <surface.h>

using namespace testing;
using namespace testing::ext;

namespace OHOS {
namespace Rosen {
using RemoteMocker = MockIRemoteObject;
class DisplayManagerProxyTest : public testing::Test {
public:
    static void SetUpTestCase();
    static void TearDownTestCase();
    void SetUp() override;
    void TearDown() override;
};

void DisplayManagerProxyTest::SetUpTestCase()
{
}

void DisplayManagerProxyTest::TearDownTestCase()
{
}

void DisplayManagerProxyTest::SetUp()
{
}

void DisplayManagerProxyTest::TearDown()
{
}

namespace {
/**
 * @tc.name: GetDefaultDisplayInfo
 * @tc.desc: test DisplayManagerProxy::GetDefaultDisplayInfo
 * @tc.type: FUNC
 */
HWTEST_F(DisplayManagerProxyTest, GetDefaultDisplayInfo01, Function | SmallTest | Level1)
{
    DisplayManagerProxy proxy1(nullptr);
    ASSERT_EQ(nullptr, proxy1.remoteObject_);
    auto displayInfo1 = proxy1.GetDefaultDisplayInfo();
    ASSERT_EQ(nullptr, displayInfo1);

    sptr<RemoteMocker> remoteMocker = new RemoteMocker();
    DisplayManagerProxy proxy2(remoteMocker);
    ASSERT_EQ(static_cast<sptr<IRemoteObject>>(remoteMocker), proxy2.remoteObject_);
    auto displayInfo2 = proxy2.GetDefaultDisplayInfo();
    ASSERT_EQ(nullptr, displayInfo2);

    remoteMocker->sendRequestResult_ = 1;
    auto displayInfo3 = proxy2.GetDefaultDisplayInfo();
    ASSERT_EQ(nullptr, displayInfo3);
}
/**
 * @tc.name: GetDisplayInfoById01
 * @tc.desc: test DisplayManagerProxy::GetDisplayInfoById
 * @tc.type: FUNC
 */
HWTEST_F(DisplayManagerProxyTest, GetDisplayInfoById01, Function | SmallTest | Level1)
{
    DisplayManagerProxy proxy1(nullptr);
    ASSERT_EQ(nullptr, proxy1.remoteObject_);
    auto displayInfo1 = proxy1.GetDisplayInfoById(0);
    ASSERT_EQ(nullptr, displayInfo1);

    sptr<RemoteMocker> remoteMocker = new RemoteMocker();
    DisplayManagerProxy proxy2(remoteMocker);
    ASSERT_EQ(static_cast<sptr<IRemoteObject>>(remoteMocker), proxy2.remoteObject_);

    auto displayInfo2 = proxy2.GetDisplayInfoById(0);
    ASSERT_EQ(nullptr, displayInfo2);

    remoteMocker->sendRequestResult_ = 1;
    auto displayInfo3 = proxy2.GetDisplayInfoById(0);
    ASSERT_EQ(nullptr, displayInfo3);
}
/**
 * @tc.name: GetDisplayInfoByScreen01
 * @tc.desc: test DisplayManagerProxy::GetDisplayInfoByScreen
 * @tc.type: FUNC
 */
HWTEST_F(DisplayManagerProxyTest, GetDisplayInfoByScreen01, Function | SmallTest | Level1)
{
    DisplayManagerProxy proxy1(nullptr);
    ASSERT_EQ(nullptr, proxy1.remoteObject_);
    auto displayInfo1 = proxy1.GetDisplayInfoByScreen(0);
    ASSERT_EQ(nullptr, displayInfo1);

    sptr<RemoteMocker> remoteMocker = new RemoteMocker();
    DisplayManagerProxy proxy2(remoteMocker);
    ASSERT_EQ(static_cast<sptr<IRemoteObject>>(remoteMocker), proxy2.remoteObject_);

    auto displayInfo2 = proxy2.GetDisplayInfoByScreen(0);
    ASSERT_EQ(nullptr, displayInfo2);

    remoteMocker->sendRequestResult_ = 1;
    auto displayInfo3 = proxy2.GetDisplayInfoByScreen(0);
    ASSERT_EQ(nullptr, displayInfo3);
}
/**
 * @tc.name: CreateVirtualScreen01
 * @tc.desc: test DisplayManagerProxy::CreateVirtualScreen
 * @tc.type: FUNC
 */
HWTEST_F(DisplayManagerProxyTest, CreateVirtualScreen01, Function | SmallTest | Level1)
{
    DisplayManagerProxy proxy1(nullptr);
    ASSERT_EQ(nullptr, proxy1.remoteObject_);
    VirtualScreenOption virtualOption1;
    virtualOption1.name_ = "testVirtualOption";
    sptr<IRemoteObject> displayManagerAgent1 = new RemoteMocker();
    auto screenId1 = proxy1.CreateVirtualScreen(virtualOption1, displayManagerAgent1);
    ASSERT_EQ(SCREEN_ID_INVALID, screenId1);

    sptr<RemoteMocker> remoteMocker = new RemoteMocker();
    DisplayManagerProxy proxy2(remoteMocker);
    ASSERT_EQ(static_cast<sptr<IRemoteObject>>(remoteMocker), proxy2.remoteObject_);

    VirtualScreenOption virtualOption2;
    virtualOption2.name_ = "testVirtualOption";
    sptr<IRemoteObject> displayManagerAgent2 = new RemoteMocker();
    auto screenId2 = proxy2.CreateVirtualScreen(virtualOption2, displayManagerAgent2);
    ASSERT_EQ(0, screenId2);

    remoteMocker->sendRequestResult_ = 1;
    auto screenId3 = proxy2.CreateVirtualScreen(virtualOption2, displayManagerAgent2);
    ASSERT_EQ(SCREEN_ID_INVALID, screenId3);
}
/**
 * @tc.name: DestroyVirtualScreen01
 * @tc.desc: test DisplayManagerProxy::DestroyVirtualScreen
 * @tc.type: FUNC
 */
HWTEST_F(DisplayManagerProxyTest, DestroyVirtualScreen01, Function | SmallTest | Level1)
{
    DisplayManagerProxy proxy1(nullptr);
    ASSERT_EQ(nullptr, proxy1.remoteObject_);
    auto result1 = proxy1.DestroyVirtualScreen(0);
    ASSERT_EQ(DMError::DM_ERROR_REMOTE_CREATE_FAILED, result1);

    sptr<RemoteMocker> remoteMocker = new RemoteMocker();
    DisplayManagerProxy proxy2(remoteMocker);
    ASSERT_EQ(static_cast<sptr<IRemoteObject>>(remoteMocker), proxy2.remoteObject_);

    auto result2 = proxy2.DestroyVirtualScreen(0);
    ASSERT_EQ(DMError::DM_OK, result2);

    remoteMocker->sendRequestResult_ = 1;
    auto result3 = proxy2.DestroyVirtualScreen(0);
    ASSERT_EQ(DMError::DM_ERROR_IPC_FAILED, result3);
}
/**
 * @tc.name: SetVirtualScreenSurface01
 * @tc.desc: test DisplayManagerProxy::SetVirtualScreenSurface
 * @tc.type: FUNC
 */
HWTEST_F(DisplayManagerProxyTest, SetVirtualScreenSurface01, Function | SmallTest | Level1)
{
    DisplayManagerProxy proxy1(nullptr);
    ASSERT_EQ(nullptr, proxy1.remoteObject_);
    auto result1 = proxy1.SetVirtualScreenSurface(0, nullptr);
    ASSERT_EQ(DMError::DM_ERROR_REMOTE_CREATE_FAILED, result1);

    sptr<RemoteMocker> remoteMocker = new RemoteMocker();
    DisplayManagerProxy proxy2(remoteMocker);
    ASSERT_EQ(static_cast<sptr<IRemoteObject>>(remoteMocker), proxy2.remoteObject_);

    auto result2 = proxy2.SetVirtualScreenSurface(0, nullptr);
    ASSERT_EQ(DMError::DM_OK, result2);
    sptr<Surface> surface = OHOS::Surface::CreateSurfaceAsConsumer();
    auto result3 = proxy2.SetVirtualScreenSurface(0, surface);
    ASSERT_EQ(DMError::DM_OK, result3);

    remoteMocker->sendRequestResult_ = 1;
    auto result4 = proxy2.SetVirtualScreenSurface(0, surface);
    ASSERT_EQ(DMError::DM_ERROR_IPC_FAILED, result4);
}
/**
 * @tc.name: SetOrientation01
 * @tc.desc: test DisplayManagerProxy::SetOrientation
 * @tc.type: FUNC
 */
HWTEST_F(DisplayManagerProxyTest, SetOrientation01, Function | SmallTest | Level1)
{
    DisplayManagerProxy proxy1(nullptr);
    ASSERT_EQ(nullptr, proxy1.remoteObject_);
    auto result1 = proxy1.SetOrientation(0, Orientation::VERTICAL);
    ASSERT_EQ(false, result1);

    sptr<RemoteMocker> remoteMocker = new RemoteMocker();
    DisplayManagerProxy proxy2(remoteMocker);
    ASSERT_EQ(static_cast<sptr<IRemoteObject>>(remoteMocker), proxy2.remoteObject_);

    auto result2 = proxy2.SetOrientation(0, Orientation::VERTICAL);
    ASSERT_EQ(false, result2);

    remoteMocker->sendRequestResult_ = 1;
    auto result3 = proxy2.SetOrientation(0, Orientation::VERTICAL);
    ASSERT_EQ(false, result3);
}
/**
 * @tc.name: GetDisplaySnapshot01
 * @tc.desc: test DisplayManagerProxy::GetDisplaySnapshot
 * @tc.type: FUNC
 */
HWTEST_F(DisplayManagerProxyTest, GetDisplaySnapshot01, Function | SmallTest | Level1)
{
    DisplayManagerProxy proxy1(nullptr);
    ASSERT_EQ(nullptr, proxy1.remoteObject_);
    auto result1 = proxy1.GetDisplaySnapshot(0);
    ASSERT_EQ(nullptr, result1);

    sptr<RemoteMocker> remoteMocker = new RemoteMocker();
    DisplayManagerProxy proxy2(remoteMocker);
    ASSERT_EQ(static_cast<sptr<IRemoteObject>>(remoteMocker), proxy2.remoteObject_);

    auto result2 = proxy2.GetDisplaySnapshot(0);
    ASSERT_EQ(nullptr, result2);
    remoteMocker->sendRequestResult_ = 1;
    auto result3 = proxy2.GetDisplaySnapshot(0);
    ASSERT_EQ(nullptr, result3);
}
/**
 * @tc.name: GetScreenSupportedColorGamuts01
 * @tc.desc: test DisplayManagerProxy::GetScreenSupportedColorGamuts
 * @tc.type: FUNC
 */
HWTEST_F(DisplayManagerProxyTest, GetScreenSupportedColorGamuts01, Function | SmallTest | Level1)
{
    std::vector<ScreenColorGamut> gamutVector;
    DisplayManagerProxy proxy1(nullptr);
    ASSERT_EQ(nullptr, proxy1.remoteObject_);
    auto result1 = proxy1.GetScreenSupportedColorGamuts(0, gamutVector);
    ASSERT_EQ(DMError::DM_ERROR_NULLPTR, result1);
    gamutVector.clear();

    sptr<RemoteMocker> remoteMocker = new RemoteMocker();
    DisplayManagerProxy proxy2(remoteMocker);
    ASSERT_EQ(static_cast<sptr<IRemoteObject>>(remoteMocker), proxy2.remoteObject_);
    auto result2 = proxy2.GetScreenSupportedColorGamuts(0, gamutVector);
    ASSERT_EQ(DMError::DM_OK, result2);
    remoteMocker->sendRequestResult_ = 1;
    auto result3 = proxy2.GetScreenSupportedColorGamuts(0, gamutVector);
    ASSERT_EQ(DMError::DM_ERROR_IPC_FAILED, result3);
}
/**
 * @tc.name: GetScreenColorGamut01
 * @tc.desc: test DisplayManagerProxy::GetScreenColorGamut
 * @tc.type: FUNC
 */
HWTEST_F(DisplayManagerProxyTest, GetScreenColorGamut01, Function | SmallTest | Level1)
{
    DisplayManagerProxy proxy1(nullptr);
    ASSERT_EQ(nullptr, proxy1.remoteObject_);
    ScreenColorGamut screenColorGamut;
    auto result1 = proxy1.GetScreenColorGamut(0, screenColorGamut);
    ASSERT_EQ(DMError::DM_ERROR_NULLPTR, result1);

    sptr<RemoteMocker> remoteMocker = new RemoteMocker();
    DisplayManagerProxy proxy2(remoteMocker);
    ASSERT_EQ(static_cast<sptr<IRemoteObject>>(remoteMocker), proxy2.remoteObject_);
    screenColorGamut = ScreenColorGamut::COLOR_GAMUT_ADOBE_RGB;
    auto result2 = proxy2.GetScreenColorGamut(0, screenColorGamut);
    ASSERT_EQ(DMError::DM_OK, result2);
    ASSERT_EQ(ScreenColorGamut::COLOR_GAMUT_NATIVE, screenColorGamut);

    screenColorGamut = ScreenColorGamut::COLOR_GAMUT_ADOBE_RGB;
    remoteMocker->sendRequestResult_ = 1;
    auto result3 = proxy2.GetScreenColorGamut(0, screenColorGamut);
    ASSERT_EQ(DMError::DM_ERROR_IPC_FAILED, result3);
    ASSERT_EQ(ScreenColorGamut::COLOR_GAMUT_ADOBE_RGB, screenColorGamut);
}
}
} // namespace Rosen
} // namespace OHOS