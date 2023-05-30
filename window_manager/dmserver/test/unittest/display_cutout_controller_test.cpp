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
#include "display_cutout_controller.h"
#include "display_manager_service_inner.h"
#include "display_manager_service.h"
#include "dm_common.h"

using namespace testing;
using namespace testing::ext;

namespace OHOS {
namespace Rosen {
class DisplayCutoutControllerTest : public testing::Test {
public:
    static void SetUpTestCase();
    static void TearDownTestCase();
    void SetUp() override;
    void TearDown() override;
};

void DisplayCutoutControllerTest::SetUpTestCase()
{
}

void DisplayCutoutControllerTest::TearDownTestCase()
{
}

void DisplayCutoutControllerTest::SetUp()
{
}

void DisplayCutoutControllerTest::TearDown()
{
}

namespace {
/**
 * @tc.name: SetCurvedScreenBoundary
 * @tc.desc: SetCurvedScreenBoundary size < 4
 * @tc.type: FUNC
 */
HWTEST_F(DisplayCutoutControllerTest, SetCurvedScreenBoundary, Function | SmallTest | Level1)
{
    sptr<DisplayCutoutController> controller = new DisplayCutoutController();
    std::vector<int> curvedScreenBoundary;
    curvedScreenBoundary.emplace_back(2);
    controller->SetCurvedScreenBoundary(curvedScreenBoundary);
    ASSERT_EQ(controller->curvedScreenBoundary_.size(), 4);
}

/**
 * @tc.name: SetCutoutSvgPath
 * @tc.desc: SetCutoutSvgPath
 * @tc.type: FUNC
 */
HWTEST_F(DisplayCutoutControllerTest, SetCutoutSvgPath, Function | SmallTest | Level1)
{
    sptr<DisplayCutoutController> controller = new DisplayCutoutController();
    DisplayId displayId = 1;
    std::string svgPath = "m10";
    controller->SetCutoutSvgPath(displayId, svgPath);
    controller->SetCutoutSvgPath(displayId, svgPath);
    ASSERT_EQ(controller->svgPaths_.size(), 1);
    ASSERT_EQ(controller->svgPaths_[displayId].size(), 2);
}

/**
 * @tc.name: GetCutoutInfo
 * @tc.desc: GetCutoutInfo
 * @tc.type: FUNC
 */
HWTEST_F(DisplayCutoutControllerTest, GetCutoutInfo, Function | SmallTest | Level1)
{
    sptr<DisplayCutoutController> controller = new DisplayCutoutController();
    DisplayId displayId = DisplayManagerServiceInner::GetInstance().GetDefaultDisplayId();
    std::string svgPath = "m10";
    controller->SetCutoutSvgPath(displayId, svgPath);
    controller->SetIsWaterfallDisplay(true);
    sptr<CutoutInfo> cutoutInfo = controller->GetCutoutInfo(displayId);
    ASSERT_NE(cutoutInfo, nullptr);
}


/**
 * @tc.name: CalcBuiltInDisplayWaterfallRects
 * @tc.desc: CalcBuiltInDisplayWaterfallRects
 * @tc.type: FUNC
 */
HWTEST_F(DisplayCutoutControllerTest, CalcBuiltInDisplayWaterfallRects, Function | SmallTest | Level1)
{
    sptr<DisplayCutoutController> controller = new DisplayCutoutController();
    controller->SetIsWaterfallDisplay(true);
    std::vector<int> curvedScreenBoundary;
    controller->SetCurvedScreenBoundary(curvedScreenBoundary);
    controller->CalcBuiltInDisplayWaterfallRects();
    ASSERT_TRUE(controller->waterfallDisplayAreaRects_.isUninitialized());
    curvedScreenBoundary.emplace_back(1);
    curvedScreenBoundary.emplace_back(2);
    curvedScreenBoundary.emplace_back(3);
    curvedScreenBoundary.emplace_back(4);
    controller->SetCurvedScreenBoundary(curvedScreenBoundary);
    controller->CalcBuiltInDisplayWaterfallRects();
    ASSERT_TRUE(controller->waterfallDisplayAreaRects_.isUninitialized());
}


/**
 * @tc.name: CalcBuiltInDisplayWaterfallRectsByRotation
 * @tc.desc: CalcBuiltInDisplayWaterfallRectsByRotation
 * @tc.type: FUNC
 */
HWTEST_F(DisplayCutoutControllerTest, CalcBuiltInDisplayWaterfallRectsByRotation, Function | SmallTest | Level1)
{
    sptr<DisplayCutoutController> controller = new DisplayCutoutController();
    uint32_t displayHeight = 300;
    uint32_t displayWidth = 300;
    std::vector<int> curvedScreenBoundary;
    curvedScreenBoundary.emplace_back(0);
    curvedScreenBoundary.emplace_back(0);
    curvedScreenBoundary.emplace_back(500);
    curvedScreenBoundary.emplace_back(500);
    controller->SetCurvedScreenBoundary(curvedScreenBoundary);
    controller->CalcBuiltInDisplayWaterfallRectsByRotation(Rotation::ROTATION_270, displayHeight, displayWidth);
    controller->CalcBuiltInDisplayWaterfallRectsByRotation(Rotation::ROTATION_180, displayHeight, displayWidth);
    controller->CalcBuiltInDisplayWaterfallRectsByRotation(Rotation::ROTATION_90, displayHeight, displayWidth);
    controller->CalcBuiltInDisplayWaterfallRectsByRotation(Rotation::ROTATION_0, displayHeight, displayWidth);
    controller->CalcBuiltInDisplayWaterfallRectsByRotation(static_cast<Rotation>(10), displayHeight, displayWidth);
    ASSERT_FALSE(controller->waterfallDisplayAreaRects_.isUninitialized());
}

/**
 * @tc.name: CheckBoundingRectsBoundary
 * @tc.desc: CheckBoundingRectsBoundary
 * @tc.type: FUNC
 */
HWTEST_F(DisplayCutoutControllerTest, CheckBoundingRectsBoundary, Function | SmallTest | Level1)
{
    sptr<DisplayCutoutController> controller = new DisplayCutoutController();
    DisplayId displayId = DisplayManagerServiceInner::GetInstance().GetDefaultDisplayId();
    std::vector<DMRect> boundingRects;
    controller->CheckBoundingRectsBoundary(displayId, boundingRects);
    ASSERT_TRUE(boundingRects.empty());
}

/**
 * @tc.name: CalcCutoutBoundingRect
 * @tc.desc: CalcCutoutBoundingRect success
 * @tc.type: FUNC
 */
HWTEST_F(DisplayCutoutControllerTest, CalcCutoutBoundingRect, Function | SmallTest | Level1)
{
    sptr<DisplayCutoutController> controller = new DisplayCutoutController();
    std::string svgPath = "M 100,100 m -75,0 a 75,75 0 1,0 150,0 a 75,75 0 1,0 -150,0 z";
    DMRect rect = controller->CalcCutoutBoundingRect(svgPath);
    DMRect emptyRect = {0, 0, 0, 0};
    ASSERT_NE(rect, emptyRect);
}

/**
 * @tc.name: TransferBoundingRectsByRotation01
 * @tc.desc: TransferBoundingRectsByRotation empty
 * @tc.type: FUNC
 */
HWTEST_F(DisplayCutoutControllerTest, TransferBoundingRectsByRotation01, Function | SmallTest | Level1)
{
    sptr<DisplayCutoutController> controller = new DisplayCutoutController();
    DisplayId id = 10;
    std::vector<DMRect> boundingRects;
    controller->TransferBoundingRectsByRotation(id, boundingRects);
    ASSERT_TRUE(boundingRects.empty());
}

/**
 * @tc.name: TransferBoundingRectsByRotation02
 * @tc.desc: TransferBoundingRectsByRotation empty
 * @tc.type: FUNC
 */
HWTEST_F(DisplayCutoutControllerTest, TransferBoundingRectsByRotation02, Function | SmallTest | Level1)
{
    sptr<DisplayCutoutController> controller = new DisplayCutoutController();
    DisplayId id = 10;
    std::vector<DMRect> emptyRects;
    controller->boundingRects_[id] = emptyRects;
    std::vector<DMRect> boundingRects;
    controller->TransferBoundingRectsByRotation(id, boundingRects);
    ASSERT_TRUE(boundingRects.empty());
}

/**
 * @tc.name: TransferBoundingRectsByRotation03
 * @tc.desc: TransferBoundingRectsByRotation success
 * @tc.type: FUNC
 */
HWTEST_F(DisplayCutoutControllerTest, TransferBoundingRectsByRotation03, Function | SmallTest | Level1)
{
    DisplayId id = 11;
    ScreenId sid = 12;
    std::string name = "abstract_display_test";
    SupportedScreenModes modesInfo;
    modesInfo.width_ = 2160;
    modesInfo.height_ = 1600;
    modesInfo.refreshRate_ = 60;
    sptr<SupportedScreenModes> info = new SupportedScreenModes(modesInfo);
    sptr<AbstractScreenController> absScreenController;
    sptr<AbstractScreen> absScreen;
    absScreenController = nullptr;
    absScreen = new AbstractScreen(absScreenController, name, sid, 1);
    auto displayController = DisplayManagerService::GetInstance().abstractDisplayController_;

    sptr<AbstractDisplay> absDisplay = new AbstractDisplay(id, name, info, absScreen);
    absDisplay->RequestRotation(Rotation::ROTATION_0);
    displayController->abstractDisplayMap_.insert((std::make_pair(id, absDisplay)));

    sptr<DisplayCutoutController> controller = new DisplayCutoutController();
    std::vector<DMRect> dmRects;
    DMRect rect = {1, 1, 100, 100};
    dmRects.emplace_back(rect);
    controller->boundingRects_[id] = dmRects;
    ASSERT_FALSE(controller->boundingRects_.count(id) == 0);
    ASSERT_FALSE(controller->boundingRects_[id].empty());
    ASSERT_NE(DisplayManagerServiceInner::GetInstance().GetDisplayById(id), nullptr);
    std::vector<DMRect> boundingRects;
    controller->TransferBoundingRectsByRotation(id, boundingRects);

    absScreenController = DisplayManagerService::GetInstance().abstractScreenController_;
    absScreenController->dmsScreenMap_.insert(std::make_pair(sid, absScreen));
    absScreen->modes_.emplace_back(info);
    absDisplay->RequestRotation(Rotation::ROTATION_180);
    controller->TransferBoundingRectsByRotation(id, boundingRects);

    absDisplay->RequestRotation(Rotation::ROTATION_90);
    controller->TransferBoundingRectsByRotation(id, boundingRects);

    absDisplay->RequestRotation(Rotation::ROTATION_270);
    controller->TransferBoundingRectsByRotation(id, boundingRects);
    ASSERT_FALSE(boundingRects.empty());
    displayController->abstractDisplayMap_.clear();
    absScreenController->dmsScreenMap_.clear();
}
}
} // Rosen
} // OHOS