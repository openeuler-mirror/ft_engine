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
#include "dm_common.h"
#include "screen_manager.h"
using namespace testing;
using namespace testing::ext;

namespace OHOS {
namespace Rosen {
class ScreenGamutTest : public testing::Test {
public:
    static void SetUpTestCase();
    static void TearDownTestCase();
    virtual void SetUp() override;
    virtual void TearDown() override;
    static sptr<Screen> defaultScreen_;
};

sptr<Screen> ScreenGamutTest::defaultScreen_ = nullptr;

void ScreenGamutTest::SetUpTestCase()
{
    auto screens = ScreenManager::GetInstance().GetAllScreens();
    if (screens.size() > 0) {
        defaultScreen_ = screens[0];
    }
}

void ScreenGamutTest::TearDownTestCase()
{
    defaultScreen_ = nullptr;
}

void ScreenGamutTest::SetUp()
{
}

void ScreenGamutTest::TearDown()
{
}

namespace {
/**
 * @tc.name: GetScreenSupportedColorGamuts01
 * @tc.desc: gamut GetScreenSupportedColorGamuts
 * @tc.type: FUNC
 */
HWTEST_F(ScreenGamutTest, GetScreenSupportedColorGamuts01, Function | MediumTest | Level3)
{
    ASSERT_NE(defaultScreen_, nullptr);
    DMError ret;
    std::vector<ScreenColorGamut> colorGamuts;
    ret = defaultScreen_->GetScreenSupportedColorGamuts(colorGamuts);
    ASSERT_EQ(ret, DMError::DM_OK);
    ASSERT_GT(colorGamuts.size(), 0);
}

/**
 * @tc.name: GetScreenColorGamut01
 * @tc.desc: gamut GetScreenColorGamut
 * @tc.type: FUNC
 */
HWTEST_F(ScreenGamutTest, GetScreenColorGamut01, Function | MediumTest | Level3)
{
    ASSERT_NE(defaultScreen_, nullptr);
    DMError ret;
    ScreenColorGamut colorGamut;
    ret = defaultScreen_->GetScreenColorGamut(colorGamut);
    ASSERT_EQ(ret, DMError::DM_OK);
    ASSERT_NE(COLOR_GAMUT_INVALID, colorGamut);
}

/**
 * @tc.name: SetScreenColorGamut01
 * @tc.desc: gamut SetScreenColorGamut, valid index
 * @tc.type: FUNC
 */
HWTEST_F(ScreenGamutTest, SetScreenColorGamut01, Function | MediumTest | Level3)
{
    ASSERT_NE(defaultScreen_, nullptr);
    DMError ret;
    ScreenColorGamut colorGamutBackup;
    int32_t colorGamutBackupIdx = -1;
    ScreenColorGamut colorGamut;
    std::vector<ScreenColorGamut> colorGamuts;

    ret = defaultScreen_->GetScreenColorGamut(colorGamutBackup); // backup origin
    ASSERT_EQ(ret, DMError::DM_OK);

    ret = defaultScreen_->GetScreenSupportedColorGamuts(colorGamuts);
    ASSERT_EQ(ret, DMError::DM_OK);

    for (int32_t i = 0; i < static_cast<int32_t>(colorGamuts.size()); i++) {
        ret = defaultScreen_->SetScreenColorGamut(i);
        ASSERT_EQ(ret, DMError::DM_OK);

        ret = defaultScreen_->GetScreenColorGamut(colorGamut);
        ASSERT_EQ(ret, DMError::DM_OK);

#ifdef SCREEN_GAMUT_SET_GET_OK
        ASSERT_EQ(colorGamut, colorGamuts[i]);
#endif
        if (colorGamutBackup == colorGamuts[i]) {
            colorGamutBackupIdx = i;
        }
    }

    ASSERT_GE(colorGamutBackupIdx, 0);
    ret = defaultScreen_->SetScreenColorGamut(colorGamutBackupIdx); // restore
    ASSERT_EQ(ret, DMError::DM_OK);
}

/**
 * @tc.name: SetScreenColorGamut02
 * @tc.desc: gamut SetScreenColorGamut, invalid index < 0
 * @tc.type: FUNC
 */
HWTEST_F(ScreenGamutTest, SetScreenColorGamut02, Function | MediumTest | Level3)
{
    ASSERT_NE(defaultScreen_, nullptr);
    DMError ret;
    ScreenColorGamut colorGamutBefore;
    ScreenColorGamut colorGamutAfter;

    ret = defaultScreen_->GetScreenColorGamut(colorGamutBefore);
    ASSERT_EQ(ret, DMError::DM_OK);

    constexpr int32_t invalidColorGamutIndex = -1; // index < 0
    ret = defaultScreen_->SetScreenColorGamut(invalidColorGamutIndex);
    ASSERT_NE(ret, DMError::DM_OK);

    ret = defaultScreen_->GetScreenColorGamut(colorGamutAfter);
    ASSERT_EQ(ret, DMError::DM_OK);

    ASSERT_EQ(colorGamutBefore, colorGamutAfter); // don't change colorgamut after invalid set
}

/**
 * @tc.name: SetScreenColorGamut03
 * @tc.desc: gamut SetScreenColorGamut, invalid index >= size
 * @tc.type: FUNC
 */
HWTEST_F(ScreenGamutTest, SetScreenColorGamut03, Function | MediumTest | Level3)
{
    ASSERT_NE(defaultScreen_, nullptr);
    DMError ret;
    ScreenColorGamut colorGamutBefore;
    ScreenColorGamut colorGamutAfter;

    ret = defaultScreen_->GetScreenColorGamut(colorGamutBefore);
    ASSERT_EQ(ret, DMError::DM_OK);

    std::vector<ScreenColorGamut> colorGamuts;
    ret = defaultScreen_->GetScreenSupportedColorGamuts(colorGamuts);
    ASSERT_EQ(ret, DMError::DM_OK);

    const int32_t invalidColorGamutIndex = static_cast<int32_t>(colorGamuts.size()); // index >= size
    ret = defaultScreen_->SetScreenColorGamut(invalidColorGamutIndex);
    ASSERT_NE(ret, DMError::DM_OK);

    ret = defaultScreen_->GetScreenColorGamut(colorGamutAfter);
    ASSERT_EQ(ret, DMError::DM_OK);

    ASSERT_EQ(colorGamutBefore, colorGamutAfter); // don't change colorgamut after invalid set
}

/**
 * @tc.name: GetScreenGamutMap01
 * @tc.desc: gamut GetScreenGamutMap
 * @tc.type: FUNC
 */
HWTEST_F(ScreenGamutTest, GetScreenGamutMap01, Function | MediumTest | Level3)
{
    ASSERT_NE(defaultScreen_, nullptr);
    DMError ret;
    ScreenGamutMap gamutMap;

    ret = defaultScreen_->GetScreenGamutMap(gamutMap);
    ASSERT_EQ(ret, DMError::DM_OK);
}

/**
 * @tc.name: SetScreenGamutMap01
 * @tc.desc: gamut SetScreenGamutMap, valid param
 * @tc.type: FUNC
 */
HWTEST_F(ScreenGamutTest, SetScreenGamutMap01, Function | MediumTest | Level3)
{
    ASSERT_NE(defaultScreen_, nullptr);
    DMError ret;
    const ScreenGamutMap gamutMaps[] = {
        GAMUT_MAP_CONSTANT,
        GAMUT_MAP_EXTENSION,
        GAMUT_MAP_HDR_CONSTANT,
        GAMUT_MAP_HDR_EXTENSION,
    };
    ScreenGamutMap gamutMap;
    ScreenGamutMap gamutMapBackup;

    ret = defaultScreen_->GetScreenGamutMap(gamutMapBackup); // backup origin
    ASSERT_EQ(ret, DMError::DM_OK);

    for (uint32_t i = 0; i < sizeof(gamutMaps) / sizeof(ScreenGamutMap); i++) {
        ret = defaultScreen_->SetScreenGamutMap(gamutMaps[i]);
        ASSERT_EQ(ret, DMError::DM_OK);

        ret = defaultScreen_->GetScreenGamutMap(gamutMap);
        ASSERT_EQ(ret, DMError::DM_OK);
#ifdef SCREEN_GAMUT_SET_GET_OK
        ASSERT_EQ(gamutMaps[i], gamutMap);
#endif
    }

    ret = defaultScreen_->SetScreenGamutMap(gamutMapBackup); // restore
    ASSERT_EQ(ret, DMError::DM_OK);
}

/**
 * @tc.name: SetScreenGamutMap02
 * @tc.desc: gamut SetScreenGamutMap, invalid param
 * @tc.type: FUNC
 */
HWTEST_F(ScreenGamutTest, SetScreenGamutMap02, Function | MediumTest | Level3)
{
    ASSERT_NE(defaultScreen_, nullptr);
    DMError ret;
    ScreenGamutMap gamutMap;
    ScreenGamutMap gamutMapBefore;
    ScreenGamutMap gamutMapAfter;

    ret = defaultScreen_->GetScreenGamutMap(gamutMapBefore);
    ASSERT_EQ(ret, DMError::DM_OK);

    gamutMap = static_cast<ScreenGamutMap>(static_cast<uint32_t>(ScreenGamutMap::GAMUT_MAP_HDR_EXTENSION) + 1);
    ret = defaultScreen_->SetScreenGamutMap(gamutMap);
    ASSERT_NE(ret, DMError::DM_OK);

    ret = defaultScreen_->GetScreenGamutMap(gamutMapAfter);
    ASSERT_EQ(ret, DMError::DM_OK);

    ASSERT_EQ(gamutMapBefore, gamutMapAfter);
}

/**
 * @tc.name: SetScreenColorTransform01
 * @tc.desc: gamut SetScreenColorTransform
 * @tc.type: FUNC
 */
HWTEST_F(ScreenGamutTest, SetScreenColorTransform01, Function | MediumTest | Level3)
{
    ASSERT_NE(defaultScreen_, nullptr);
    DMError ret;

    ret = defaultScreen_->SetScreenColorTransform();
    ASSERT_EQ(ret, DMError::DM_OK);
}
}
} // namespace Rosen
} // namespace OHOS
