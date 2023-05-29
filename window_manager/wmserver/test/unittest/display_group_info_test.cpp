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
#include "display_group_info.h"
#include "wm_common.h"
#include "dm_common.h"
#include "common_test_utils.h"

using namespace testing;
using namespace testing::ext;

namespace OHOS {
namespace Rosen {

class DisplayGroupInfoTest : public testing::Test {
public:
    static void SetUpTestCase();
    static void TearDownTestCase();
    void SetUp() override;
    void TearDown() override;

    void AddTestDisplayInfo(std::map<DisplayId, sptr<DisplayInfo>>& displayInfosMap_);
    sptr<DisplayGroupInfo> displayGroupInfo_;
};

void DisplayGroupInfoTest::SetUpTestCase()
{
}

void DisplayGroupInfoTest::TearDownTestCase()
{
}

void DisplayGroupInfoTest::SetUp()
{
    ScreenId displayGroupId = 0;
    sptr<DisplayInfo> displayInfo = new DisplayInfo();
    DisplayId displayId = 0;
    displayInfo->SetDisplayId(displayId);
    displayInfo->SetOffsetX(1);
    displayInfo->SetOffsetY(1);
    displayInfo->SetWidth(1);
    displayInfo->SetHeight(1);
    displayGroupInfo_ = new DisplayGroupInfo(displayGroupId, displayInfo);
    AddTestDisplayInfo(displayGroupInfo_->displayInfosMap_);
}

void DisplayGroupInfoTest::TearDown()
{
}

void DisplayGroupInfoTest::AddTestDisplayInfo(std::map<DisplayId, sptr<DisplayInfo>>& displayInfosMap_)
{
    sptr<DisplayInfo> displayInfo1 = new DisplayInfo();
    DisplayId displayId1 = 1;
    int32_t offsetX1 = 0;
    int32_t offsetY1 = 0;
    int32_t height1 = 0;
    int32_t width1 = 0;
    displayInfo1->SetDisplayId(displayId1);
    displayInfo1->SetOffsetX(offsetX1);
    displayInfo1->SetOffsetY(offsetY1);
    displayInfo1->SetHeight(height1);
    displayInfo1->SetWidth(width1);
    displayInfosMap_.insert(std::make_pair(displayId1, displayInfo1));
    sptr<DisplayInfo> displayInfo2 = new DisplayInfo();
    DisplayId displayId2 = 2;
    int32_t offsetX2 = 2;
    int32_t offsetY2 = 2;
    int32_t height2 = 2;
    int32_t width2 = 2;
    displayInfo2->SetDisplayId(displayId2);
    displayInfo2->SetOffsetX(offsetX2);
    displayInfo2->SetOffsetY(offsetY2);
    displayInfo2->SetHeight(height2);
    displayInfo2->SetWidth(width2);
    displayInfosMap_.insert(std::make_pair(displayId2, displayInfo2));
}

namespace {
/**
 * @tc.name: AddDisplayInfo0
 * @tc.desc: normal function
 * @tc.type: FUNC
 */
HWTEST_F(DisplayGroupInfoTest, AddDisplayInfo01, Function | SmallTest | Level2)
{
    sptr<DisplayInfo> displayInfo = new DisplayInfo();
    displayInfo->SetDisplayId(0);
    displayGroupInfo_->AddDisplayInfo(displayInfo);
    ASSERT_EQ(3u, displayGroupInfo_->displayInfosMap_.size());
}

/**
 * @tc.name: AddDisplayInfo1
 * @tc.desc: normal function
 * @tc.type: FUNC
 */
HWTEST_F(DisplayGroupInfoTest, AddDisplayInfo02, Function | SmallTest | Level2)
{
    sptr<DisplayInfo> displayInfo = new DisplayInfo();
    displayInfo->SetDisplayId(3);
    displayGroupInfo_->AddDisplayInfo(displayInfo);
    ASSERT_EQ(4u, displayGroupInfo_->displayInfosMap_.size());
}

/**
 * @tc.name: RemoveDisplayInfo01
 * @tc.desc: normal function
 * @tc.type: FUNC
 */
HWTEST_F(DisplayGroupInfoTest, RemoveDisplayInfo01, Function | SmallTest | Level2)
{
    DisplayId displayId = 0;
    displayGroupInfo_->RemoveDisplayInfo(displayId);
    ASSERT_EQ(2u, displayGroupInfo_->displayInfosMap_.size());
}

/**
 * @tc.name: RemoveDisplayInfo02
 * @tc.desc: normal function
 * @tc.type: FUNC
 */
HWTEST_F(DisplayGroupInfoTest, RemoveDisplayInfo02, Function | SmallTest | Level2)
{
    DisplayId displayId = 3;
    displayGroupInfo_->RemoveDisplayInfo(displayId);
    ASSERT_EQ(3u, displayGroupInfo_->displayInfosMap_.size());
}

/**
 * @tc.name: GetAllDisplayRects01
 * @tc.desc: normal function
 * @tc.type: FUNC
 */
HWTEST_F(DisplayGroupInfoTest, GetAllDisplayRects01, Function | SmallTest | Level2)
{
    ASSERT_EQ(3u, displayGroupInfo_->GetAllDisplayRects().size());
}

/**
 * @tc.name: UpdateLeftAndRightDisplayId01
 * @tc.desc: normal function
 * @tc.type: FUNC
 */
HWTEST_F(DisplayGroupInfoTest, UpdateLeftAndRightDisplayId01, Function | SmallTest | Level2)
{
    displayGroupInfo_->UpdateLeftAndRightDisplayId();
    ASSERT_EQ(1u, displayGroupInfo_->GetLeftDisplayId());
    ASSERT_EQ(2u, displayGroupInfo_->GetRightDisplayId());
}

/**
 * @tc.name: SetDisplayRotation01 and GetDisplayRotation01
 * @tc.desc: normal function
 * @tc.type: FUNC
 */
HWTEST_F(DisplayGroupInfoTest, SetDisplayRotation01, Function | SmallTest | Level2)
{
    DisplayId displayId = 0;
    displayGroupInfo_->SetDisplayRotation(displayId, Rotation::ROTATION_90);
    ASSERT_EQ(Rotation::ROTATION_90, displayGroupInfo_->GetDisplayRotation(displayId));
}

/**
 * @tc.name: SetDisplayRotation02 and GetDisplayRotation02
 * @tc.desc: normal function
 * @tc.type: FUNC
 */
HWTEST_F(DisplayGroupInfoTest, SetDisplayRotation02, Function | SmallTest | Level2)
{
    DisplayId displayId = 3;
    displayGroupInfo_->SetDisplayRotation(displayId, Rotation::ROTATION_90);
    ASSERT_EQ(Rotation::ROTATION_0, displayGroupInfo_->GetDisplayRotation(displayId));
}

/**
 * @tc.name: SetDisplayVirtualPixelRatio01 and GetDisplayVirtualPixelRatio01
 * @tc.desc: normal function
 * @tc.type: FUNC
 */
HWTEST_F(DisplayGroupInfoTest, SetDisplayVirtualPixelRatio01, Function | SmallTest | Level2)
{
    DisplayId displayId = 0;
    displayGroupInfo_->SetDisplayVirtualPixelRatio(displayId, 0.1f);
    ASSERT_FLOAT_EQ(0.1f, displayGroupInfo_->GetDisplayVirtualPixelRatio(displayId));
}

/**
 * @tc.name: SetDisplayVirtualPixelRatio02 and GetDisplayVirtualPixelRatio02
 * @tc.desc: normal function
 * @tc.type: FUNC
 */
HWTEST_F(DisplayGroupInfoTest, SetDisplayVirtualPixelRatio02, Function | SmallTest | Level2)
{
    DisplayId displayId = 3;
    displayGroupInfo_->SetDisplayVirtualPixelRatio(displayId, 0.1f);
    ASSERT_EQ(1.0f, displayGroupInfo_->GetDisplayVirtualPixelRatio(displayId));
}

/**
 * @tc.name: SetDisplayRect01 and GetDisplayRect01
 * @tc.desc: normal function
 * @tc.type: FUNC
 */
HWTEST_F(DisplayGroupInfoTest, SetDisplayRect01, Function | SmallTest | Level2)
{
    DisplayId displayId = 0;
    Rect rect = {3, 3, 3, 3};
    displayGroupInfo_->SetDisplayRect(displayId, rect);
    ASSERT_EQ(rect, displayGroupInfo_->GetDisplayRect(displayId));
}

/**
 * @tc.name: SetDisplayRect02 and GetDisplayRect02
 * @tc.desc: normal function
 * @tc.type: FUNC
 */
HWTEST_F(DisplayGroupInfoTest, SetDisplayRect02, Function | SmallTest | Level2)
{
    DisplayId displayId = 3;
    Rect rect1 = {3, 3, 3, 3};
    displayGroupInfo_->SetDisplayRect(displayId, rect1);
    Rect rect2 = {0, 0, 0, 0};
    ASSERT_EQ(rect2, displayGroupInfo_->GetDisplayRect(displayId));
}

/**
 * @tc.name: UpdateDisplayInfo01
 * @tc.desc: normal function
 * @tc.type: FUNC
 */
HWTEST_F(DisplayGroupInfoTest, UpdateDisplayInfo01, Function | SmallTest | Level2)
{
    sptr<DisplayInfo> displayInfo = new DisplayInfo();
    DisplayId displayId = 0;
    displayInfo->SetDisplayId(displayId);
    ScreenId screenId = 1;
    displayInfo->SetScreenId(screenId);
    displayGroupInfo_->UpdateDisplayInfo(displayInfo);
    ASSERT_EQ(1u, displayGroupInfo_->GetDisplayInfo(displayId)->GetScreenId());
}

/**
 * @tc.name: UpdateDisplayInfo02
 * @tc.desc: normal function
 * @tc.type: FUNC
 */
HWTEST_F(DisplayGroupInfoTest, UpdateDisplayInfo02, Function | SmallTest | Level2)
{
    sptr<DisplayInfo> displayInfo = new DisplayInfo();
    DisplayId displayId = 3;
    displayInfo->SetDisplayId(displayId);
    ScreenId screenId = 1;
    displayInfo->SetScreenId(screenId);
    displayGroupInfo_->UpdateDisplayInfo(displayInfo);
    ASSERT_EQ(nullptr, displayGroupInfo_->GetDisplayInfo(displayId));
}
}
}
}