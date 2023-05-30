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

#ifndef FOUNDATION_DMSERVER_ABSTRACT_SCREEN_CONTROLLER_H
#define FOUNDATION_DMSERVER_ABSTRACT_SCREEN_CONTROLLER_H

#include <map>
#include <vector>

#include <event_handler.h>
#include <refbase.h>
#include <surface.h>
#include <transaction/rs_interfaces.h>

#include "abstract_screen.h"
#include "agent_death_recipient.h"
#include "display_manager_agent_controller.h"
#include "dm_common.h"
#include "rsscreen_change_listener.h"
#include "screen.h"
#include "zidl/display_manager_agent_interface.h"

namespace OHOS::Rosen {
class AbstractScreenController : public RefBase {
using OnAbstractScreenConnectCb = std::function<void(sptr<AbstractScreen>)>;
using OnAbstractScreenChangeCb = std::function<void(sptr<AbstractScreen>, DisplayChangeEvent event)>;
public:
    struct AbstractScreenCallback : public RefBase {
        OnAbstractScreenConnectCb onConnect_;
        OnAbstractScreenConnectCb onDisconnect_;
        OnAbstractScreenChangeCb onChange_;
    };

    explicit AbstractScreenController(std::recursive_mutex& mutex);
    ~AbstractScreenController();
    WM_DISALLOW_COPY_AND_MOVE(AbstractScreenController);

    void Init();
    std::vector<ScreenId> GetAllScreenIds() const;
    uint32_t GetRSScreenNum() const;
    sptr<AbstractScreen> GetAbstractScreen(ScreenId dmsScreenId) const;
    std::vector<ScreenId> GetAllValidScreenIds(const std::vector<ScreenId>& screenIds) const;
    sptr<AbstractScreenGroup> GetAbstractScreenGroup(ScreenId dmsScreenId);
    ScreenId GetDefaultAbstractScreenId();
    ScreenId ConvertToRsScreenId(ScreenId dmsScreenId) const;
    ScreenId ConvertToDmsScreenId(ScreenId rsScreenId) const;
    void RegisterAbstractScreenCallback(sptr<AbstractScreenCallback> cb);
    void RegisterRSScreenChangeListener(const sptr<IRSScreenChangeListener>& listener);
    ScreenId CreateVirtualScreen(VirtualScreenOption option, const sptr<IRemoteObject>& displayManagerAgent);
    DMError DestroyVirtualScreen(ScreenId screenId);
    DMError SetVirtualScreenSurface(ScreenId screenId, sptr<Surface> surface);
    void SetBuildInDefaultOrientation(Orientation orientation);
    bool SetOrientation(ScreenId screenId, Orientation orientation, bool isFromWindow);
    bool SetRotation(ScreenId screenId, Rotation rotationAfter, bool isFromWindow);

    bool SetScreenActiveMode(ScreenId screenId, uint32_t modeId);
    const std::shared_ptr<RSDisplayNode>& GetRSDisplayNodeByScreenId(ScreenId dmsScreenId) const;
    void UpdateRSTree(ScreenId dmsScreenId, ScreenId parentScreenId, std::shared_ptr<RSSurfaceNode>& surfaceNode,
        bool isAdd, bool isMultiDisplay);
    bool MakeMirror(ScreenId, std::vector<ScreenId> screens);
    bool MakeExpand(std::vector<ScreenId> screenIds, std::vector<Point> startPoints);
    void RemoveVirtualScreenFromGroup(std::vector<ScreenId> screens);
    bool SetScreenPowerForAll(ScreenPowerState state, PowerStateChangeReason reason) const;
    ScreenPowerState GetScreenPower(ScreenId dmsScreenId) const;
    bool SetVirtualPixelRatio(ScreenId screenId, float virtualPixelRatio);

    // colorspace, gamut
    DMError GetScreenSupportedColorGamuts(ScreenId screenId, std::vector<ScreenColorGamut>& colorGamuts);
    DMError GetScreenColorGamut(ScreenId screenId, ScreenColorGamut& colorGamut);
    DMError SetScreenColorGamut(ScreenId screenId, int32_t colorGamutIdx);
    DMError GetScreenGamutMap(ScreenId screenId, ScreenGamutMap& gamutMap);
    DMError SetScreenGamutMap(ScreenId screenId, ScreenGamutMap gamutMap);
    DMError SetScreenColorTransform(ScreenId screenId);

private:
    static inline bool IsVertical(Rotation rotation)
    {
        return (rotation == Rotation::ROTATION_0 || rotation == Rotation::ROTATION_180);
    }
    void SetScreenRotateAnimation(sptr<AbstractScreen>& screen, ScreenId screenId, Rotation rotationAfter);
    void RegisterRsScreenConnectionChangeListener();
    void OnRsScreenConnectionChange(ScreenId rsScreenId, ScreenEvent screenEvent);
    bool OnRemoteDied(const sptr<IRemoteObject>& agent);
    void ProcessScreenConnected(ScreenId rsScreenId);
    sptr<AbstractScreen> InitAndGetScreen(ScreenId rsScreenId);
    void ProcessScreenDisconnected(ScreenId rsScreenId);
    bool InitAbstractScreenModesInfo(sptr<AbstractScreen>& absScreen);
    sptr<AbstractScreen> InitVirtualScreen(ScreenId dmsScreenId, ScreenId rsId, VirtualScreenOption option);
    sptr<AbstractScreenGroup> AddToGroupLocked(sptr<AbstractScreen> newScreen);
    sptr<AbstractScreenGroup> RemoveFromGroupLocked(sptr<AbstractScreen> newScreen);
    bool RemoveChildFromGroup(sptr<AbstractScreen>, sptr<AbstractScreenGroup>);
    bool CheckScreenInScreenGroup(sptr<AbstractScreen> newScreen) const;
    sptr<AbstractScreenGroup> AddAsFirstScreenLocked(sptr<AbstractScreen> newScreen);
    sptr<AbstractScreenGroup> AddAsSuccedentScreenLocked(sptr<AbstractScreen> newScreen);
    void ProcessScreenModeChanged(ScreenId dmsScreenId);
    void ChangeScreenGroup(sptr<AbstractScreenGroup> group, const std::vector<ScreenId>& screens,
        const std::vector<Point>& startPoints, bool filterScreen, ScreenCombination combination);
    void AddScreenToGroup(sptr<AbstractScreenGroup>, const std::vector<ScreenId>&,
        const std::vector<Point>&, std::map<ScreenId, bool>&);
    void NotifyScreenConnected(sptr<ScreenInfo>) const;
    void NotifyScreenDisconnected(ScreenId screenId) const;
    void NotifyScreenChanged(sptr<ScreenInfo> screenInfo, ScreenChangeEvent event) const;
    void NotifyScreenGroupChanged(const sptr<ScreenInfo>& screenInfo, ScreenGroupChangeEvent event) const;
    void NotifyScreenGroupChanged(const std::vector<sptr<ScreenInfo>>& screenInfo, ScreenGroupChangeEvent event) const;

    class ScreenIdManager {
    public:
        ScreenIdManager() = default;
        ~ScreenIdManager() = default;
        WM_DISALLOW_COPY_AND_MOVE(ScreenIdManager);
        ScreenId CreateAndGetNewScreenId(ScreenId rsScreenId);
        bool DeleteScreenId(ScreenId dmsScreenId);
        bool HasDmsScreenId(ScreenId dmsScreenId) const;
        bool HasRsScreenId(ScreenId dmsScreenId) const;
        bool ConvertToRsScreenId(ScreenId, ScreenId&) const;
        ScreenId ConvertToRsScreenId(ScreenId) const;
        bool ConvertToDmsScreenId(ScreenId, ScreenId&) const;
        ScreenId ConvertToDmsScreenId(ScreenId) const;
        uint32_t GetRSScreenNum() const;
    private:
        std::atomic<ScreenId> dmsScreenCount_ {0};
        std::map<ScreenId, ScreenId> rs2DmsScreenIdMap_;
        std::map<ScreenId, ScreenId> dms2RsScreenIdMap_;
    };

    std::recursive_mutex& mutex_;
    OHOS::Rosen::RSInterfaces& rsInterface_;
    ScreenIdManager screenIdManager_;
    std::map<ScreenId, sptr<AbstractScreen>> dmsScreenMap_;
    std::map<ScreenId, sptr<AbstractScreenGroup>> dmsScreenGroupMap_;
    std::map<sptr<IRemoteObject>, std::vector<ScreenId>> screenAgentMap_;
    sptr<AgentDeathRecipient> deathRecipient_ { nullptr };
    sptr<AbstractScreenCallback> abstractScreenCallback_;
    sptr<IRSScreenChangeListener> rSScreenChangeListener_;
    std::shared_ptr<AppExecFwk::EventHandler> controllerHandler_;
    std::atomic<ScreenId> defaultRsScreenId_ {SCREEN_ID_INVALID };
    Orientation buildInDefaultOrientation_ { Orientation::UNSPECIFIED };
    bool isExpandCombination_ = false;
};
} // namespace OHOS::Rosen
#endif // FOUNDATION_DMSERVER_ABSTRACT_SCREEN_CONTROLLER_H