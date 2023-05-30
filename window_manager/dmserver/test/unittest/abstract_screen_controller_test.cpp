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

#include "abstract_screen_controller.h"
#include "iremote_object_mocker.h"

using namespace testing;
using namespace testing::ext;

namespace OHOS {
namespace Rosen {
class AbstractScreenControllerTest : public testing::Test {
public:
    static void SetUpTestCase();
    static void TearDownTestCase();
    void SetUp() override;
    void TearDown() override;

    std::recursive_mutex mutex;
    sptr<AbstractScreenController> absController_ = new AbstractScreenController(mutex);
    std::string name = "AbstractScreenController";
    std::vector<sptr<AbstractScreen>> screenVec;
    std::vector<sptr<AbstractScreenGroup>> screenGroupVec;
};

void AbstractScreenControllerTest::SetUpTestCase()
{
}

void AbstractScreenControllerTest::TearDownTestCase()
{
}

void AbstractScreenControllerTest::SetUp()
{
    uint64_t index = 5;
    for (uint64_t i = 0; i < index; ++i) {
        ScreenId dmsId = i;
        ScreenId rsId = i;
        sptr<AbstractScreen> absScreen = new AbstractScreen(absController_, name, dmsId, rsId);
        sptr<AbstractScreenGroup> absScreenGroup = new AbstractScreenGroup(absController_,
            dmsId, rsId, name, ScreenCombination::SCREEN_ALONE);
        screenVec.emplace_back(absScreen);
        screenGroupVec.emplace_back(absScreenGroup);
        // init dmsScreenMap_
        absController_->dmsScreenMap_.insert(std::make_pair(dmsId, absScreen));
        // init screenIdManager_
        absController_->screenIdManager_.rs2DmsScreenIdMap_.insert(std::make_pair(rsId, dmsId));
        absController_->screenIdManager_.dms2RsScreenIdMap_.insert(std::make_pair(dmsId, rsId));
        // init dmsScreenGroupMap_
        absController_->dmsScreenGroupMap_.insert(std::make_pair(rsId, absScreenGroup));
    }
    screenVec[4]->type_ = ScreenType::UNDEFINED;
    screenVec[3]->type_ = ScreenType::VIRTUAL;
    absController_->dmsScreenMap_.insert(std::make_pair(index, nullptr));
    absController_->screenIdManager_.dms2RsScreenIdMap_.insert(std::make_pair(index, SCREEN_ID_INVALID));
    absController_->dmsScreenGroupMap_.insert(std::make_pair(index, nullptr));
}

void AbstractScreenControllerTest::TearDown()
{
    screenVec.clear();
}

namespace {
/**
 * @tc.name: GetAllValidScreenIds
 * @tc.desc: GetAllValidScreenIds test
 * @tc.type: FUNC
 */
HWTEST_F(AbstractScreenControllerTest, GetAllValidScreenIds, Function | SmallTest | Level3)
{
    std::vector<ScreenId> screenIds {0, 1, 1, 2, 2, 3, 4};
    std::vector<ScreenId> valid {0, 1, 2, 3};
    ASSERT_EQ(valid, absController_->GetAllValidScreenIds(screenIds));
}
/**
 * @tc.name: GetRSDisplayNodeByScreenId
 * @tc.desc: GetRSDisplayNodeByScreenId test
 * @tc.type: FUNC
 */
HWTEST_F(AbstractScreenControllerTest, GetRSDisplayNodeByScreenId, Function | SmallTest | Level3)
{
    ScreenId id = 6;
    std::shared_ptr<RSDisplayNode> node = nullptr;
    ASSERT_EQ(node, absController_->GetRSDisplayNodeByScreenId(id));
}
/**
 * @tc.name: UpdateRSTree
 * @tc.desc: UpdateRSTree test
 * @tc.type: FUNC
 */
HWTEST_F(AbstractScreenControllerTest, UpdateRSTree01, Function | SmallTest | Level3)
{
    ScreenId id = 8;
    std::shared_ptr<RSSurfaceNode> node = nullptr;
    absController_->UpdateRSTree(id, id, node, true, true);
    ASSERT_EQ(nullptr, absController_->GetAbstractScreen(id));
}
/**
 * @tc.name: UpdateRSTree
 * @tc.desc: UpdateRSTree test
 * @tc.type: FUNC
 */
HWTEST_F(AbstractScreenControllerTest, UpdateRSTree02, Function | SmallTest | Level3)
{
    ScreenId id = 1;
    ScreenId parentId = 8;
    std::shared_ptr<RSSurfaceNode> node = nullptr;
    absController_->UpdateRSTree(id, parentId, node, true, true);
    ASSERT_NE(nullptr, absController_->GetAbstractScreen(id));
    ASSERT_EQ(nullptr, absController_->GetAbstractScreen(parentId));
}
/**
 * @tc.name: UpdateRSTree
 * @tc.desc: UpdateRSTree test
 * @tc.type: FUNC
 */
HWTEST_F(AbstractScreenControllerTest, UpdateRSTree03, Function | SmallTest | Level3)
{
    ScreenId id = 1;
    ScreenId parentId = 2;
    std::shared_ptr<RSSurfaceNode> node = nullptr;
    ASSERT_NE(nullptr, absController_->GetAbstractScreen(id));
    sptr<AbstractScreen> parentScreen = absController_->GetAbstractScreen(parentId);
    ASSERT_NE(nullptr, absController_->GetAbstractScreen(parentId));
    parentScreen->rsDisplayNode_ = nullptr;
    absController_->UpdateRSTree(id, parentId, node, true, true);
}
/**
 * @tc.name: RegisterAbstractScreenCallback
 * @tc.desc: RegisterAbstractScreenCallback test
 * @tc.type: FUNC
 */
HWTEST_F(AbstractScreenControllerTest, RegisterAbstractScreenCallback, Function | SmallTest | Level3)
{
    sptr<AbstractScreenController::AbstractScreenCallback> cb = nullptr;
    absController_->RegisterAbstractScreenCallback(cb);
    ASSERT_EQ(6, absController_->dmsScreenMap_.size());
}
/**
 * @tc.name: AddToGroupLocked
 * @tc.desc: AddToGroupLocked test
 * @tc.type: FUNC
 */
HWTEST_F(AbstractScreenControllerTest, AddToGroupLocked, Function | SmallTest | Level3)
{
    ASSERT_EQ(false, absController_->dmsScreenGroupMap_.empty());
    ASSERT_EQ(nullptr, absController_->AddToGroupLocked(screenVec[0]));
}
/**
 * @tc.name: RemoveFromGroupLocked
 * @tc.desc: RemoveFromGroupLocked test
 * @tc.type: FUNC
 */
HWTEST_F(AbstractScreenControllerTest, RemoveFromGroupLocked01, Function | SmallTest | Level3)
{
    sptr<AbstractScreen> screen = screenVec[0];
    screen->groupDmsId_ = 0;
    ASSERT_EQ(nullptr, absController_->RemoveFromGroupLocked(screen));
}
/**
 * @tc.name: RemoveChildFromGroup
 * @tc.desc: RemoveChildFromGroup test
 * @tc.type: FUNC
 */
HWTEST_F(AbstractScreenControllerTest, RemoveChildFromGroup01, Function | SmallTest | Level3)
{
    sptr<AbstractScreen> screen = screenVec[0];
    ScreenId dmsId = screen->dmsId_;
    Point point;
    auto p = std::make_pair(screen, point);
    sptr<AbstractScreenGroup> screenGroup = absController_->dmsScreenGroupMap_[0];
    screenGroup->abstractScreenMap_.insert(std::make_pair(dmsId, p));
    ASSERT_EQ(true, absController_->RemoveChildFromGroup(screen, screenGroup));
}
/**
 * @tc.name: AddAsSuccedentScreenLocked
 * @tc.desc: AddAsSuccedentScreenLocked test
 * @tc.type: FUNC
 */
HWTEST_F(AbstractScreenControllerTest, AddAsSuccedentScreenLocked01, Function | SmallTest | Level3)
{
    sptr<AbstractScreen> screen = screenVec[0];
    absController_->dmsScreenMap_.erase(absController_->GetDefaultAbstractScreenId());
    ASSERT_EQ(nullptr, absController_->AddAsSuccedentScreenLocked(screen));
}
/**
 * @tc.name: AddAsSuccedentScreenLocked
 * @tc.desc: AddAsSuccedentScreenLocked test
 * @tc.type: FUNC
 */
HWTEST_F(AbstractScreenControllerTest, AddAsSuccedentScreenLocked02, Function | SmallTest | Level3)
{
    sptr<AbstractScreen> screen = screenVec[0];
    ASSERT_EQ(nullptr, absController_->AddAsSuccedentScreenLocked(screen));
}
/**
 * @tc.name: CreateVirtualScreen
 * @tc.desc: CreateVirtualScreen test
 * @tc.type: FUNC
 */
HWTEST_F(AbstractScreenControllerTest, CreateVirtualScreen01, Function | SmallTest | Level3)
{
    VirtualScreenOption option;
    sptr<IRemoteObject> displayManagerAgent = new IRemoteObjectMocker();
    ASSERT_EQ(0, absController_->CreateVirtualScreen(option, displayManagerAgent));
}
/**
 * @tc.name: InitVirtualScreen
 * @tc.desc: InitVirtualScreen test
 * @tc.type: FUNC
 */
HWTEST_F(AbstractScreenControllerTest, InitVirtualScreen01, Function | SmallTest | Level3)
{
    VirtualScreenOption option;
    absController_->dmsScreenMap_.erase(absController_->GetDefaultAbstractScreenId());
    sptr<AbstractScreen> screen = absController_->InitVirtualScreen(0, 0, option);
    ASSERT_EQ(0, screen->activeIdx_);
}
/**
 * @tc.name: InitVirtualScreen
 * @tc.desc: InitVirtualScreen test
 * @tc.type: FUNC
 */
HWTEST_F(AbstractScreenControllerTest, InitVirtualScreen02, Function | SmallTest | Level3)
{
    VirtualScreenOption option;
    sptr<AbstractScreen> defaultScreen = absController_->dmsScreenMap_[absController_->GetDefaultAbstractScreenId()];
    sptr<SupportedScreenModes> modes;
    defaultScreen->modes_.emplace_back(modes);
    defaultScreen->activeIdx_ = 0;
    ASSERT_EQ(nullptr, defaultScreen->GetActiveScreenMode());
    sptr<AbstractScreen> screen = absController_->InitVirtualScreen(0, 0, option);
    ASSERT_EQ(ScreenType::VIRTUAL, screen->type_);
}
/**
 * @tc.name: DestroyVirtualScreen
 * @tc.desc: DestroyVirtualScreen test
 * @tc.type: FUNC
 */
HWTEST_F(AbstractScreenControllerTest, DestroyVirtualScreen01, Function | SmallTest | Level3)
{
    ScreenId id = 5;
    ASSERT_EQ(DMError::DM_ERROR_INVALID_PARAM, absController_->DestroyVirtualScreen(id));
}
/**
 * @tc.name: DestroyVirtualScreen
 * @tc.desc: DestroyVirtualScreen test
 * @tc.type: FUNC
 */
HWTEST_F(AbstractScreenControllerTest, DestroyVirtualScreen02, Function | SmallTest | Level3)
{
    ScreenId id = 1;
    ASSERT_EQ(DMError::DM_OK, absController_->DestroyVirtualScreen(id));
}
/**
 * @tc.name: SetVirtualScreenSurface
 * @tc.desc: SetVirtualScreenSurface test
 * @tc.type: FUNC
 */
HWTEST_F(AbstractScreenControllerTest, SetVirtualScreenSurface01, Function | SmallTest | Level3)
{
    ScreenId id = 6;
    sptr<Surface> surface = nullptr;
    ASSERT_EQ(DMError::DM_ERROR_RENDER_SERVICE_FAILED, absController_->SetVirtualScreenSurface(id, surface));
}
/**
 * @tc.name: SetBuildInDefaultOrientation
 * @tc.desc: SetBuildInDefaultOrientation test
 * @tc.type: FUNC
 */
HWTEST_F(AbstractScreenControllerTest, SetBuildInDefaultOrientation, Function | SmallTest | Level3)
{
    Orientation orientation = Orientation::BEGIN;
    absController_->SetBuildInDefaultOrientation(orientation);
    ASSERT_EQ(orientation, absController_->buildInDefaultOrientation_);
}
/**
 * @tc.name: SetOrientation
 * @tc.desc: SetOrientation test
 * @tc.type: FUNC
 */
HWTEST_F(AbstractScreenControllerTest, SetOrientation01, Function | SmallTest | Level3)
{
    absController_->dmsScreenMap_[1]->isScreenGroup_ = true;
    Orientation orientation = Orientation::BEGIN;
    ASSERT_EQ(false, absController_->SetOrientation(1, orientation, true));
}
/**
 * @tc.name: SetRotation
 * @tc.desc: SetRotation test
 * @tc.type: FUNC
 */
HWTEST_F(AbstractScreenControllerTest, SetRotation01, Function | SmallTest | Level3)
{
    absController_->dmsScreenMap_[1]->rotation_ = Rotation::ROTATION_180;
    absController_->screenIdManager_.dms2RsScreenIdMap_.erase(1);
    ASSERT_EQ(false, absController_->SetRotation(1, Rotation::ROTATION_0, true));
}
/**
 * @tc.name: SetScreenActiveMode
 * @tc.desc: SetScreenActiveMode test
 * @tc.type: FUNC
 */
HWTEST_F(AbstractScreenControllerTest, SetScreenActiveMode01, Function | SmallTest | Level3)
{
    ASSERT_EQ(false, absController_->SetScreenActiveMode(5, 0));
}
/**
 * @tc.name: SetScreenActiveMode
 * @tc.desc: SetScreenActiveMode test
 * @tc.type: FUNC
 */
HWTEST_F(AbstractScreenControllerTest, SetScreenActiveMode02, Function | SmallTest | Level3)
{
    absController_->screenIdManager_.dms2RsScreenIdMap_.erase(1);
    ASSERT_EQ(false, absController_->SetScreenActiveMode(1, 0));
}
/**
 * @tc.name: ProcessScreenModeChanged
 * @tc.desc: ProcessScreenModeChanged test
 * @tc.type: FUNC
 */
HWTEST_F(AbstractScreenControllerTest, ProcessScreenModeChanged01, Function | SmallTest | Level3)
{
    absController_->ProcessScreenModeChanged(7);
    ASSERT_EQ(6, absController_->dmsScreenMap_.size());
}
/**
 * @tc.name: ProcessScreenModeChanged
 * @tc.desc: ProcessScreenModeChanged test
 * @tc.type: FUNC
 */
HWTEST_F(AbstractScreenControllerTest, ProcessScreenModeChanged02, Function | SmallTest | Level3)
{
    absController_->ProcessScreenModeChanged(5);
    ASSERT_EQ(nullptr, absController_->dmsScreenMap_[5]);
}
/**
 * @tc.name: ProcessScreenModeChanged
 * @tc.desc: ProcessScreenModeChanged test
 * @tc.type: FUNC
 */
HWTEST_F(AbstractScreenControllerTest, ProcessScreenModeChanged03, Function | SmallTest | Level3)
{
    absController_->ProcessScreenModeChanged(2);
    ASSERT_NE(nullptr, absController_->dmsScreenMap_[2]);
}
/**
 * @tc.name: ChangeScreenGroup
 * @tc.desc: ChangeScreenGroup test
 * @tc.type: FUNC
 */
HWTEST_F(AbstractScreenControllerTest, ChangeScreenGroup01, Function | SmallTest | Level3)
{
    sptr<AbstractScreenGroup> group = screenGroupVec[0];
    Point point;
    auto abs2pointPair = std::make_pair(screenVec[0], point);
    group->abstractScreenMap_.insert(std::make_pair(0, abs2pointPair));
    group->abstractScreenMap_.insert(std::make_pair(1, abs2pointPair));
    std::vector<Point> startPoints;
    std::vector<ScreenId> screens;
    for (ScreenId i = 0; i < 7; ++i) {
        screens.emplace_back(i);
        startPoints.emplace_back(point);
        if (i < absController_->dmsScreenMap_.size() && absController_->dmsScreenMap_[i] != nullptr) {
            absController_->dmsScreenMap_[i]->groupDmsId_ = 1;
        }
    }
    absController_->ChangeScreenGroup(group, screens, startPoints, true, ScreenCombination::SCREEN_ALONE);
    ASSERT_EQ(6, absController_->dmsScreenMap_.size());
}
/**
 * @tc.name: ChangeScreenGroup
 * @tc.desc: ChangeScreenGroup test
 * @tc.type: FUNC
 */
HWTEST_F(AbstractScreenControllerTest, ChangeScreenGroup02, Function | SmallTest | Level3)
{
    sptr<AbstractScreenGroup> group = screenGroupVec[0];
    Point point;
    auto abs2pointPair = std::make_pair(screenVec[0], point);
    group->abstractScreenMap_.insert(std::make_pair(0, abs2pointPair));
    group->abstractScreenMap_.insert(std::make_pair(1, abs2pointPair));
    std::vector<Point> startPoints;
    std::vector<ScreenId> screens;
    for(ScreenId i = 0; i < 7; ++i) {
        screens.emplace_back(i);
        startPoints.emplace_back(point);
        if (i < absController_->dmsScreenMap_.size() && absController_->dmsScreenMap_[i] != nullptr) {
            absController_->dmsScreenMap_[i]->groupDmsId_ = 1;
        }
    }
    absController_->abstractScreenCallback_ = nullptr;
    absController_->ChangeScreenGroup(group, screens, startPoints, true, ScreenCombination::SCREEN_ALONE);
    ASSERT_EQ(6, absController_->dmsScreenMap_.size());
}
/**
 * @tc.name: ChangeScreenGroup
 * @tc.desc: ChangeScreenGroup test
 * @tc.type: FUNC
 */
HWTEST_F(AbstractScreenControllerTest, AddScreenToGroup01, Function | SmallTest | Level3)
{
    sptr<AbstractScreenGroup> group = screenGroupVec[0];
    std::vector<ScreenId> addScreens {0, 1, 2, 3, 4, 5};
    Point point;
    std::vector<Point> addChildPos(10, point);
    std::map<ScreenId, bool> removeChildResMap;
    absController_->AddScreenToGroup(group, addScreens, addChildPos, removeChildResMap);
    absController_->abstractScreenCallback_ = nullptr;
    absController_->rSScreenChangeListener_ = nullptr;
    ASSERT_EQ(6, absController_->dmsScreenMap_.size());
}
/**
 * @tc.name: MakeExpand
 * @tc.desc: MakeExpand test
 * @tc.type: FUNC
 */
HWTEST_F(AbstractScreenControllerTest, MakeExpand01, Function | SmallTest | Level3)
{
    std::vector<ScreenId> screenIds;
    std::vector<Point> startPoints;
    ScreenId defaultId = absController_->GetDefaultAbstractScreenId();
    absController_->dmsScreenMap_[defaultId] = nullptr;
    ASSERT_EQ(false, absController_->MakeExpand(screenIds, startPoints));
}
/**
 * @tc.name: MakeExpand
 * @tc.desc: MakeExpand test
 * @tc.type: FUNC
 */
HWTEST_F(AbstractScreenControllerTest, MakeExpand02, Function | SmallTest | Level3)
{
    std::vector<ScreenId> screenIds;
    std::vector<Point> startPoints;
    ScreenId defaultId = absController_->GetDefaultAbstractScreenId();
    auto defaultScreen = absController_->GetAbstractScreen(defaultId);
    ScreenId groupDmsId = defaultScreen->groupDmsId_;
    absController_->dmsScreenGroupMap_[groupDmsId] = nullptr;
    ASSERT_EQ(false, absController_->MakeExpand(screenIds, startPoints));
}
/**
 * @tc.name: RemoveVirtualScreenFromGroup
 * @tc.desc: RemoveVirtualScreenFromGroup test
 * @tc.type: FUNC
 */
HWTEST_F(AbstractScreenControllerTest, RemoveVirtualScreenFromGroup01, Function | SmallTest | Level3)
{
    std::vector<ScreenId> screens {0, 1, 2, 3, 4, 5, 6, 7};
    absController_->abstractScreenCallback_ = nullptr;
    absController_->RemoveVirtualScreenFromGroup(screens);
    ASSERT_EQ(6, absController_->dmsScreenMap_.size());
}
/**
 * @tc.name: OnRemoteDied
 * @tc.desc: OnRemoteDied test
 * @tc.type: FUNC
 */
HWTEST_F(AbstractScreenControllerTest, OnRemoteDied01, Function | SmallTest | Level3)
{
    sptr<IRemoteObject> agent = nullptr;
    ASSERT_EQ(false, absController_->OnRemoteDied(agent));
}
/**
 * @tc.name: OnRemoteDied
 * @tc.desc: OnRemoteDied test
 * @tc.type: FUNC
 */
HWTEST_F(AbstractScreenControllerTest, OnRemoteDied02, Function | SmallTest | Level3)
{
    sptr<IRemoteObject> agent = new IRemoteObjectMocker();
    ASSERT_EQ(true, absController_->OnRemoteDied(agent));
}
/**
 * @tc.name: OnRemoteDied
 * @tc.desc: OnRemoteDied test
 * @tc.type: FUNC
 */
HWTEST_F(AbstractScreenControllerTest, OnRemoteDied03, Function | SmallTest | Level3)
{
    sptr<IRemoteObject> agent = new IRemoteObjectMocker();
    std::vector<ScreenId> screens {5};
    absController_->screenAgentMap_.insert(std::make_pair(agent, screens));
    ASSERT_EQ(true, absController_->OnRemoteDied(agent));
    ASSERT_EQ(0, absController_->screenAgentMap_.size());
}
/**
 * @tc.name: CreateAndGetNewScreenId
 * @tc.desc: CreateAndGetNewScreenId test
 * @tc.type: FUNC
 */
HWTEST_F(AbstractScreenControllerTest, CreateAndGetNewScreenId01, Function | SmallTest | Level3)
{
    ScreenId rsScreenId = 1;
    ScreenId dmsScreenId = absController_->screenIdManager_.dmsScreenCount_;
    ASSERT_EQ(dmsScreenId, absController_->screenIdManager_.CreateAndGetNewScreenId(rsScreenId));
    ASSERT_EQ(++dmsScreenId, absController_->screenIdManager_.dmsScreenCount_);
}
/**
 * @tc.name: ConvertToRsScreenId
 * @tc.desc: ConvertToRsScreenId test
 * @tc.type: FUNC
 */
HWTEST_F(AbstractScreenControllerTest, ConvertToRsScreenId01, Function | SmallTest | Level3)
{
    ScreenId rsScreenId;
    ScreenId dmsScreenId = 8;
    ASSERT_EQ(false, absController_->screenIdManager_.ConvertToRsScreenId(dmsScreenId, rsScreenId));
}
/**
 * @tc.name: NotifyScreenConnected
 * @tc.desc: NotifyScreenConnected test
 * @tc.type: FUNC
 */
HWTEST_F(AbstractScreenControllerTest, NotifyScreenConnected, Function | SmallTest | Level3)
{
    sptr<ScreenInfo> screenInfo = nullptr;
    absController_->NotifyScreenConnected(screenInfo);
    ASSERT_EQ(6, absController_->dmsScreenMap_.size());
}
/**
 * @tc.name: NotifyScreenConnected
 * @tc.desc: NotifyScreenConnected test
 * @tc.type: FUNC
 */
HWTEST_F(AbstractScreenControllerTest, NotifyScreenChanged, Function | SmallTest | Level3)
{
    sptr<ScreenInfo> screenInfo = nullptr;
    absController_->NotifyScreenChanged(screenInfo, ScreenChangeEvent::UPDATE_ORIENTATION);
    ASSERT_EQ(6, absController_->dmsScreenMap_.size());
}
/**
 * @tc.name: NotifyScreenConnected
 * @tc.desc: NotifyScreenConnected test
 * @tc.type: FUNC
 */
HWTEST_F(AbstractScreenControllerTest, NotifyScreenGroupChanged, Function | SmallTest | Level3)
{
    sptr<ScreenInfo> screenInfo = nullptr;
    absController_->NotifyScreenGroupChanged(screenInfo, ScreenGroupChangeEvent::ADD_TO_GROUP);
    ASSERT_EQ(6, absController_->dmsScreenMap_.size());
}
/**
 * @tc.name: NotifyScreenConnected
 * @tc.desc: NotifyScreenConnected test
 * @tc.type: FUNC
 */
HWTEST_F(AbstractScreenControllerTest, SetScreenPowerForAll, Function | SmallTest | Level3)
{
    for (uint32_t i = 0; i < screenVec.size(); ++i) {
        if (screenVec[i] != nullptr && screenVec[i]->type_ == ScreenType::REAL) {
            screenVec[i]->type_ = ScreenType::UNDEFINED;
        }
    }
    ASSERT_EQ(false, absController_->SetScreenPowerForAll(ScreenPowerState::INVALID_STATE,
        PowerStateChangeReason::POWER_BUTTON));
}
/**
 * @tc.name: SetVirtualPixelRatio
 * @tc.desc: SetVirtualPixelRatio test
 * @tc.type: FUNC
 */
HWTEST_F(AbstractScreenControllerTest, SetVirtualPixelRatio01, Function | SmallTest | Level3)
{
    auto screen = screenVec[0];
    ScreenId id = 0;
    float ratio = 1.0;
    screen->isScreenGroup_ = true;
    ASSERT_EQ(false, absController_->SetVirtualPixelRatio(id, ratio));
}
/**
 * @tc.name: SetVirtualPixelRatio
 * @tc.desc: SetVirtualPixelRatio test
 * @tc.type: FUNC
 */
HWTEST_F(AbstractScreenControllerTest, SetVirtualPixelRatio02, Function | SmallTest | Level3)
{
    ScreenId id = 0;
    float ratio = 1.0;
    absController_->abstractScreenCallback_ = nullptr;
    ASSERT_EQ(true, absController_->SetVirtualPixelRatio(id, ratio));
}
}
} // namespace Rosen
} // namespace OHOS
