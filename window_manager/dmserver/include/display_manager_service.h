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

#ifndef FOUNDATION_DMSERVER_DISPLAY_MANAGER_SERVICE_H
#define FOUNDATION_DMSERVER_DISPLAY_MANAGER_SERVICE_H

#include <map>
#include <mutex>

#include <system_ability.h>
#include <surface.h>
#include <ui/rs_display_node.h>

#include "atomic_map.h"
#include "dm_common.h"
#include "display_dumper.h"
#include "screen.h"
#include "abstract_display.h"
#include "abstract_display_controller.h"
#include "abstract_screen_controller.h"
#include "display_change_listener.h"
#include "display_cutout_controller.h"
#include "display_manager_stub.h"
#include "display_power_controller.h"
#include "rsscreen_change_listener.h"
#include "singleton_delegator.h"
#include "window_info_queried_listener.h"

namespace OHOS::Rosen {
class DisplayManagerService : public SystemAbility, public DisplayManagerStub {
DECLARE_SYSTEM_ABILITY(DisplayManagerService);
WM_DECLARE_SINGLE_INSTANCE_BASE(DisplayManagerService);

public:
    int Dump(int fd, const std::vector<std::u16string>& args) override;
    void OnStart() override;
    void OnStop() override;
    ScreenId CreateVirtualScreen(VirtualScreenOption option,
        const sptr<IRemoteObject>& displayManagerAgent) override;
    DMError DestroyVirtualScreen(ScreenId screenId) override;
    DMError SetVirtualScreenSurface(ScreenId screenId, sptr<Surface> surface) override;
    bool IsScreenRotationLocked() override;
    void SetScreenRotationLocked(bool isLocked) override;

    sptr<DisplayInfo> GetDefaultDisplayInfo() override;
    sptr<DisplayInfo> GetDisplayInfoById(DisplayId displayId) override;
    sptr<DisplayInfo> GetDisplayInfoByScreen(ScreenId screenId) override;
    sptr<CutoutInfo> GetCutoutInfo(DisplayId displayId) override;
    bool SetOrientation(ScreenId screenId, Orientation orientation) override;
    bool SetOrientationFromWindow(ScreenId screenId, Orientation orientation);
    bool SetRotationFromWindow(ScreenId screenId, Rotation targetRotation);
    void SetGravitySensorSubscriptionEnabled();
    std::shared_ptr<Media::PixelMap> GetDisplaySnapshot(DisplayId displayId) override;
    uint32_t GetRSScreenNum() const;
    DMError HasPrivateWindow(DisplayId id, bool& hasPrivateWindow) override;
    // colorspace, gamut
    DMError GetScreenSupportedColorGamuts(ScreenId screenId, std::vector<ScreenColorGamut>& colorGamuts) override;
    DMError GetScreenColorGamut(ScreenId screenId, ScreenColorGamut& colorGamut) override;
    DMError SetScreenColorGamut(ScreenId screenId, int32_t colorGamutIdx) override;
    DMError GetScreenGamutMap(ScreenId screenId, ScreenGamutMap& gamutMap) override;
    DMError SetScreenGamutMap(ScreenId screenId, ScreenGamutMap gamutMap) override;
    DMError SetScreenColorTransform(ScreenId screenId) override;

    bool RegisterDisplayManagerAgent(const sptr<IDisplayManagerAgent>& displayManagerAgent,
        DisplayManagerAgentType type) override;
    bool UnregisterDisplayManagerAgent(const sptr<IDisplayManagerAgent>& displayManagerAgent,
        DisplayManagerAgentType type) override;
    bool WakeUpBegin(PowerStateChangeReason reason) override;
    bool WakeUpEnd() override;
    bool SuspendBegin(PowerStateChangeReason reason) override;
    bool SuspendEnd() override;
    bool SetScreenPowerForAll(ScreenPowerState state, PowerStateChangeReason reason) override;
    ScreenPowerState GetScreenPower(ScreenId dmsScreenId) override;
    bool SetDisplayState(DisplayState state) override;
    void UpdateRSTree(DisplayId displayId, DisplayId parentDisplayId, std::shared_ptr<RSSurfaceNode>& surfaceNode,
        bool isAdd, bool isMultiDisplay);

    DisplayState GetDisplayState(DisplayId displayId) override;
    void NotifyDisplayEvent(DisplayEvent event) override;
    bool SetFreeze(std::vector<DisplayId> displayIds, bool isFreeze) override;

    ScreenId MakeMirror(ScreenId mainScreenId, std::vector<ScreenId> mirrorScreenId) override;
    ScreenId MakeExpand(std::vector<ScreenId> screenId, std::vector<Point> startPoint) override;
    void RemoveVirtualScreenFromGroup(std::vector<ScreenId> screens) override;
    sptr<ScreenInfo> GetScreenInfoById(ScreenId screenId) override;
    sptr<ScreenGroupInfo> GetScreenGroupInfoById(ScreenId screenId) override;
    ScreenId GetScreenGroupIdByScreenId(ScreenId screenId);
    std::vector<sptr<ScreenInfo>> GetAllScreenInfos() override;

    std::vector<DisplayId> GetAllDisplayIds() override;
    bool SetScreenActiveMode(ScreenId screenId, uint32_t modeId) override;
    bool SetVirtualPixelRatio(ScreenId screenId, float virtualPixelRatio) override;
    void RegisterDisplayChangeListener(sptr<IDisplayChangeListener> listener);
    void RegisterWindowInfoQueriedListener(const sptr<IWindowInfoQueriedListener>& listener);
    void RegisterRSScreenChangeListener(const sptr<IRSScreenChangeListener>& listener);
private:
    DisplayManagerService();
    ~DisplayManagerService() = default;
    bool Init();
    void NotifyDisplayStateChange(DisplayId defaultDisplayId, sptr<DisplayInfo> displayInfo,
        const std::map<DisplayId, sptr<DisplayInfo>>& displayInfoMap, DisplayStateChangeType type);
    void NotifyScreenshot(DisplayId displayId);
    ScreenId GetScreenIdByDisplayId(DisplayId displayId) const;
    void ConfigureDisplayManagerService();
    void ConfigureWaterfallDisplayCompressionParams();

    std::recursive_mutex mutex_;
    static inline SingletonDelegator<DisplayManagerService> delegator_;
    sptr<AbstractDisplayController> abstractDisplayController_;
    sptr<AbstractScreenController> abstractScreenController_;
    sptr<DisplayPowerController> displayPowerController_;
    sptr<DisplayCutoutController> displayCutoutController_;
    sptr<IDisplayChangeListener> displayChangeListener_;
    sptr<IWindowInfoQueriedListener> windowInfoQueriedListener_;
    sptr<DisplayDumper> displayDumper_;
    AtomicMap<ScreenId, uint32_t> accessTokenIdMaps_;
    bool isAutoRotationOpen_;
};
} // namespace OHOS::Rosen

#endif // FOUNDATION_DMSERVER_DISPLAY_MANAGER_SERVICE_H