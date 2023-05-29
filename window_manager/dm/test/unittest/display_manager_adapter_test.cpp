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
#include "display_manager_adapter.h"

using namespace testing;
using namespace testing::ext;

namespace OHOS {
namespace Rosen {
class DisplayManagerAdapterTest : public testing::Test {
public:
    static void SetUpTestCase();
    static void TearDownTestCase();
    void SetUp() override;
    void TearDown() override;
};

void DisplayManagerAdapterTest::SetUpTestCase()
{
}

void DisplayManagerAdapterTest::TearDownTestCase()
{
}

void DisplayManagerAdapterTest::SetUp()
{
}

void DisplayManagerAdapterTest::TearDown()
{
}

namespace {
/**
 * @tc.name: GetDisplayInfo
 * @tc.desc: test nullptr
 * @tc.type: FUNC
 */
HWTEST_F(DisplayManagerAdapterTest, GetDisplayInfo, Function | SmallTest | Level2)
{
    sptr<DisplayInfo> info = SingletonContainer::Get<DisplayManagerAdapter>().GetDisplayInfo(DISPLAY_ID_INVALID);
    ASSERT_EQ(info, nullptr);
}

/**
 * @tc.name: GetCutoutInfo
 * @tc.desc: test nullptr
 * @tc.type: FUNC
 */
HWTEST_F(DisplayManagerAdapterTest, GetCutoutInfo, Function | SmallTest | Level2)
{
    sptr<CutoutInfo> info = SingletonContainer::Get<DisplayManagerAdapter>().GetCutoutInfo(DISPLAY_ID_INVALID);
    ASSERT_EQ(info, nullptr);
}

/**
 * @tc.name: GetScreenSupportedColorGamuts
 * @tc.desc: test success
 * @tc.type: FUNC
 */
HWTEST_F(DisplayManagerAdapterTest, GetScreenSupportedColorGamuts, Function | SmallTest | Level2)
{
    std::vector<ScreenColorGamut> colorGamuts;
    SingletonContainer::Get<ScreenManagerAdapter>().GetScreenSupportedColorGamuts(0, colorGamuts);
}

/**
 * @tc.name: SetScreenColorGamut
 * @tc.desc: test success
 * @tc.type: FUNC
 */
HWTEST_F(DisplayManagerAdapterTest, SetScreenColorGamut, Function | SmallTest | Level2)
{
    std::vector<ScreenColorGamut> colorGamuts;
    DMError err = SingletonContainer::Get<ScreenManagerAdapter>().SetScreenColorGamut(0, -1);
    ASSERT_EQ(err, DMError::DM_ERROR_RENDER_SERVICE_FAILED);
}

/**
 * @tc.name: GetScreenColorGamut
 * @tc.desc: test success
 * @tc.type: FUNC
 */
HWTEST_F(DisplayManagerAdapterTest, GetScreenColorGamut, Function | SmallTest | Level2)
{
    ScreenColorGamut colorGamut;
    DMError err = SingletonContainer::Get<ScreenManagerAdapter>().GetScreenColorGamut(0, colorGamut);
    ASSERT_EQ(err, DMError::DM_ERROR_RENDER_SERVICE_FAILED);
}

/**
 * @tc.name: GetScreenGamutMap
 * @tc.desc: test success
 * @tc.type: FUNC
 */
HWTEST_F(DisplayManagerAdapterTest, GetScreenGamutMap, Function | SmallTest | Level2)
{
    ScreenGamutMap gamutMap;
    DMError err = SingletonContainer::Get<ScreenManagerAdapter>().GetScreenGamutMap(0, gamutMap);
    ASSERT_EQ(err, DMError::DM_ERROR_RENDER_SERVICE_FAILED);
}

/**
 * @tc.name: SetScreenGamutMap
 * @tc.desc: test success
 * @tc.type: FUNC
 */
HWTEST_F(DisplayManagerAdapterTest, SetScreenGamutMap, Function | SmallTest | Level2)
{
    DMError err = SingletonContainer::Get<ScreenManagerAdapter>().SetScreenGamutMap(0, GAMUT_MAP_CONSTANT);
    ASSERT_EQ(err, DMError::DM_ERROR_RENDER_SERVICE_FAILED);
}

/**
 * @tc.name: SetScreenColorTransform
 * @tc.desc: test success
 * @tc.type: FUNC
 */
HWTEST_F(DisplayManagerAdapterTest, SetScreenColorTransform, Function | SmallTest | Level2)
{
    DMError err = SingletonContainer::Get<ScreenManagerAdapter>().SetScreenColorTransform(0);
    ASSERT_EQ(err, DMError::DM_OK);
}

/**
 * @tc.name: SetFreeze
 * @tc.desc: test success
 * @tc.type: FUNC
 */
HWTEST_F(DisplayManagerAdapterTest, SetFreeze, Function | SmallTest | Level2)
{
    std::vector<DisplayId> displayIds;
    bool ret = SingletonContainer::Get<DisplayManagerAdapter>().SetFreeze(displayIds, false);
    ASSERT_TRUE(ret);
}

/**
 * @tc.name: GetScreenGroupInfoById
 * @tc.desc: test nullptr
 * @tc.type: FUNC
 */
HWTEST_F(DisplayManagerAdapterTest, GetScreenGroupInfoById, Function | SmallTest | Level2)
{
    auto info = SingletonContainer::Get<ScreenManagerAdapter>().GetScreenGroupInfoById(SCREEN_ID_INVALID);
    ASSERT_EQ(info, nullptr);
}

/**
 * @tc.name: GetScreenInfo
 * @tc.desc: test nullptr
 * @tc.type: FUNC
 */
HWTEST_F(DisplayManagerAdapterTest, GetScreenInfo, Function | SmallTest | Level2)
{
    sptr<ScreenInfo> info = SingletonContainer::Get<ScreenManagerAdapter>().GetScreenInfo(SCREEN_ID_INVALID);
    ASSERT_EQ(info, nullptr);
}

/**
 * @tc.name: OnRemoteDied
 * @tc.desc: test nullptr
 * @tc.type: FUNC
 */
HWTEST_F(DisplayManagerAdapterTest, OnRemoteDied, Function | SmallTest | Level2)
{
    sptr<IRemoteObject::DeathRecipient> dmsDeath_ = nullptr;
    dmsDeath_ = new(std::nothrow) DMSDeathRecipient(SingletonContainer::Get<ScreenManagerAdapter>());
    dmsDeath_->OnRemoteDied(nullptr);
}

/**
 * @tc.name: OnRemoteDied01
 * @tc.desc: test nullptr
 * @tc.type: FUNC
 */
HWTEST_F(DisplayManagerAdapterTest, OnRemoteDied01, Function | SmallTest | Level2)
{
    sptr<IRemoteObject::DeathRecipient> dmsDeath_ = nullptr;
    dmsDeath_ = new(std::nothrow) DMSDeathRecipient(SingletonContainer::Get<ScreenManagerAdapter>());
    SingletonContainer::Get<ScreenManagerAdapter>().InitDMSProxy();
    sptr<IRemoteObject> obj = SingletonContainer::Get<ScreenManagerAdapter>().displayManagerServiceProxy_->AsObject();
    wptr<IRemoteObject> wptrDeath = obj;
    dmsDeath_->OnRemoteDied(wptrDeath);
}

/**
 * @tc.name: Clear
 * @tc.desc: test success
 * @tc.type: FUNC
 */
HWTEST_F(DisplayManagerAdapterTest, Clear, Function | SmallTest | Level2)
{
    SingletonContainer::Get<ScreenManagerAdapter>().InitDMSProxy();
    SingletonContainer::Get<ScreenManagerAdapter>().Clear();
    ASSERT_FALSE(SingletonContainer::Get<ScreenManagerAdapter>().isProxyValid_);
}

/**
 * @tc.name: Clear01
 * @tc.desc: test success
 * @tc.type: FUNC
 */
HWTEST_F(DisplayManagerAdapterTest, Clear01, Function | SmallTest | Level2)
{
    SingletonContainer::Get<ScreenManagerAdapter>().InitDMSProxy();
    SingletonContainer::Get<ScreenManagerAdapter>().displayManagerServiceProxy_ = nullptr;
    SingletonContainer::Get<ScreenManagerAdapter>().Clear();
    ASSERT_FALSE(SingletonContainer::Get<ScreenManagerAdapter>().isProxyValid_);
}
}
}
}