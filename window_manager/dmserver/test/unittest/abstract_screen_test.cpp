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

#include "abstract_screen.h"
#include "abstract_screen_controller.h"

using namespace testing;
using namespace testing::ext;

namespace OHOS {
namespace Rosen {
class AbstractScreenTest : public testing::Test {
public:
    static void SetUpTestCase();
    static void TearDownTestCase();
    void SetUp() override;
    void TearDown() override;

    static sptr<AbstractScreen> absScreen_;
    static std::recursive_mutex mutex_;
    static std::string name_;
};

sptr<AbstractScreen> AbstractScreenTest::absScreen_ = nullptr;
std::recursive_mutex AbstractScreenTest::mutex_;
std::string AbstractScreenTest::name_ = "AbstractScreenTest";

void AbstractScreenTest::SetUpTestCase()
{
    sptr<AbstractScreenController> absScreenController = new AbstractScreenController(mutex_);
    absScreen_ = new AbstractScreen(absScreenController, name_, 0, 0);
    absScreen_->modes_.clear();
    absScreen_->activeIdx_ = 0;
}

void AbstractScreenTest::TearDownTestCase()
{
}

void AbstractScreenTest::SetUp()
{
}

void AbstractScreenTest::TearDown()
{
}

namespace {
/**
 * @tc.name: GetScreenMode
 * @tc.desc: Get screen mode
 * @tc.type: FUNC
 */
HWTEST_F(AbstractScreenTest, GetScreenMode, Function | SmallTest | Level3)
{
    sptr<SupportedScreenModes> mode0 = new SupportedScreenModes();
    sptr<SupportedScreenModes> mode1 = new SupportedScreenModes();
    absScreen_->modes_ = {mode0, mode1};

    absScreen_->activeIdx_ = -1;
    ASSERT_EQ(nullptr, absScreen_->GetActiveScreenMode());
    absScreen_->activeIdx_ = static_cast<int32_t>(absScreen_->modes_.size());
    ASSERT_EQ(nullptr, absScreen_->GetActiveScreenMode());
    absScreen_->activeIdx_ = 0;
    ASSERT_EQ(mode0, absScreen_->GetActiveScreenMode());
    absScreen_->activeIdx_ = 1;
    ASSERT_EQ(mode1, absScreen_->GetActiveScreenMode());

    ASSERT_EQ(mode0, (absScreen_->GetAbstractScreenModes())[0]);
    ASSERT_EQ(mode1, (absScreen_->GetAbstractScreenModes())[1]);
}
/**
 * @tc.name: ConvertToScreenInfo
 * @tc.desc: Convert to screen info
 * @tc.type: FUNC
 */
HWTEST_F(AbstractScreenTest, ConvertToScreenInfo, Function | SmallTest | Level3)
{
    ASSERT_NE(nullptr, absScreen_->ConvertToScreenInfo());
}
/**
 * @tc.name: RSTree
 * @tc.desc: RS tree
 * @tc.type: FUNC
 */
HWTEST_F(AbstractScreenTest, RSTree, Function | SmallTest | Level3)
{
    std::shared_ptr<RSSurfaceNode> surfaceNode;
    absScreen_->rsDisplayNode_ = nullptr;
    absScreen_->UpdateRSTree(surfaceNode, true);
    absScreen_->UpdateDisplayGroupRSTree(surfaceNode, 0, true);

    struct RSDisplayNodeConfig config;
    absScreen_->rsDisplayNode_ = std::make_shared<RSDisplayNode>(config);
    ASSERT_NE(nullptr, absScreen_->rsDisplayNode_);
    absScreen_->UpdateRSTree(surfaceNode, true);
    absScreen_->UpdateDisplayGroupRSTree(surfaceNode, 0, true);

    struct RSSurfaceNodeConfig rsSurfaceNodeConfig;
    surfaceNode = RSSurfaceNode::Create(rsSurfaceNodeConfig, RSSurfaceNodeType::DEFAULT);
    absScreen_->UpdateDisplayGroupRSTree(surfaceNode, 0, true);
    absScreen_->UpdateDisplayGroupRSTree(surfaceNode, 0, false);
    ASSERT_NE(nullptr, absScreen_->rsDisplayNode_);
    absScreen_->rsDisplayNode_ = nullptr;
}
/**
 * @tc.name: ColorGamut
 * @tc.desc: Screen color gamut
 * @tc.type: FUNC
 */
HWTEST_F(AbstractScreenTest, ColorGamut, Function | SmallTest | Level3)
{
    sptr<AbstractScreenController> absScreenController0 = new AbstractScreenController(mutex_);
    sptr<AbstractScreen> absScreen0 = new AbstractScreen(absScreenController0, name_, 0, -1ULL);
    std::vector<ScreenColorGamut> colorGamuts;
    ASSERT_EQ(DMError::DM_ERROR_RENDER_SERVICE_FAILED, absScreen0->GetScreenSupportedColorGamuts(colorGamuts));
    ScreenColorGamut colorGamut;
    ASSERT_EQ(DMError::DM_ERROR_RENDER_SERVICE_FAILED, absScreen0->GetScreenColorGamut(colorGamut));

    ASSERT_EQ(DMError::DM_ERROR_RENDER_SERVICE_FAILED, absScreen0->SetScreenColorGamut(0));

    ScreenGamutMap gamutMap;
    ASSERT_EQ(DMError::DM_ERROR_RENDER_SERVICE_FAILED, absScreen0->GetScreenGamutMap(gamutMap));

    gamutMap = ScreenGamutMap::GAMUT_MAP_HDR_EXTENSION;
    ASSERT_EQ(DMError::DM_ERROR_RENDER_SERVICE_FAILED, absScreen0->GetScreenGamutMap(gamutMap));
}
/**
 * @tc.name: FillScreenInfo
 * @tc.desc: Fill screen info
 * @tc.type: FUNC
 */
HWTEST_F(AbstractScreenTest, FillScreenInfo, Function | SmallTest | Level3)
{
    absScreen_->FillScreenInfo(nullptr);
    sptr<ScreenInfo> info = new ScreenInfo();
    ASSERT_NE(nullptr, info);

    absScreen_->virtualPixelRatio_ = 0.f;
    absScreen_->FillScreenInfo(info);
    ASSERT_EQ(1.f, info->virtualPixelRatio_);

    absScreen_->virtualPixelRatio_ = 2.f;
    absScreen_->FillScreenInfo(info);
    ASSERT_EQ(2.f, info->virtualPixelRatio_);
}
/**
 * @tc.name: CalcRotation
 * @tc.desc: Calc rotation
 * @tc.type: FUNC
 */
HWTEST_F(AbstractScreenTest, CalcRotation, Function | SmallTest | Level3)
{
    absScreen_->modes_.clear();
    absScreen_->activeIdx_ = 0;

    ASSERT_EQ(Rotation::ROTATION_0, absScreen_->CalcRotation(Orientation::UNSPECIFIED));

    sptr<SupportedScreenModes> mode = new SupportedScreenModes();
    mode->width_ = 1;
    mode->height_ = 1;
    absScreen_->modes_ = {mode};

    ASSERT_EQ(Rotation::ROTATION_0, absScreen_->CalcRotation(Orientation::UNSPECIFIED));
    ASSERT_EQ(Rotation::ROTATION_90, absScreen_->CalcRotation(Orientation::VERTICAL));
    ASSERT_EQ(Rotation::ROTATION_0, absScreen_->CalcRotation(Orientation::HORIZONTAL));
    ASSERT_EQ(Rotation::ROTATION_270, absScreen_->CalcRotation(Orientation::REVERSE_VERTICAL));
    ASSERT_EQ(Rotation::ROTATION_180, absScreen_->CalcRotation(Orientation::REVERSE_HORIZONTAL));
    ASSERT_EQ(Rotation::ROTATION_0, absScreen_->CalcRotation(Orientation::LOCKED));
}
}
} // namespace Rosen
} // namespace OHOS
