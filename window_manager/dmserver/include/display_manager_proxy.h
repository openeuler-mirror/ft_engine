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

#ifndef FOUNDATION_DMSERVER_DISPLAY_MANAGER_PROXY_H
#define FOUNDATION_DMSERVER_DISPLAY_MANAGER_PROXY_H

#include "display_manager_interface.h"

#include "dm_common.h"

#include "screen.h"

#include <iremote_proxy.h>

namespace OHOS::Rosen {
class DisplayManagerProxy : public IRemoteProxy<IDisplayManager> {
public:
    explicit DisplayManagerProxy(const sptr<IRemoteObject> &impl)
        : IRemoteProxy<IDisplayManager>(impl) {};
    ~DisplayManagerProxy() {};

    sptr<DisplayInfo> GetDefaultDisplayInfo() override;
    sptr<DisplayInfo> GetDisplayInfoById(DisplayId displayId) override;
    sptr<DisplayInfo> GetDisplayInfoByScreen(ScreenId screenId) override;
    DMError HasPrivateWindow(DisplayId displayId, bool& hasPrivateWindow) override;

    ScreenId CreateVirtualScreen(VirtualScreenOption option,
        const sptr<IRemoteObject>& displayManagerAgent) override;
    DMError DestroyVirtualScreen(ScreenId screenId) override;
    DMError SetVirtualScreenSurface(ScreenId screenId, sptr<Surface> surface) override;
    bool SetOrientation(ScreenId screenId, Orientation orientation) override;
    std::shared_ptr<Media::PixelMap> GetDisplaySnapshot(DisplayId displayId) override;
    bool IsScreenRotationLocked() override;
    void SetScreenRotationLocked(bool isLocked) override;

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
    std::vector<DisplayId> GetAllDisplayIds() override;
    DisplayState GetDisplayState(DisplayId displayId) override;
    sptr<CutoutInfo> GetCutoutInfo(DisplayId displayId) override;
    void NotifyDisplayEvent(DisplayEvent event) override;
    bool SetFreeze(std::vector<DisplayId> displayIds, bool isFreeze) override;
    ScreenId MakeMirror(ScreenId mainScreenId, std::vector<ScreenId> mirrorScreenId) override;
    sptr<ScreenInfo> GetScreenInfoById(ScreenId screenId) override;
    sptr<ScreenGroupInfo> GetScreenGroupInfoById(ScreenId screenId) override;
    std::vector<sptr<ScreenInfo>> GetAllScreenInfos() override;
    ScreenId MakeExpand(std::vector<ScreenId> screenId, std::vector<Point> startPoint) override;
    void RemoveVirtualScreenFromGroup(std::vector<ScreenId> screens) override;
    bool SetScreenActiveMode(ScreenId screenId, uint32_t modeId) override;
    bool SetVirtualPixelRatio(ScreenId screenId, float virtualPixelRatio) override;

private:
    static inline BrokerDelegator<DisplayManagerProxy> delegator_;
};
} // namespace OHOS::Rosen

#endif // FOUNDATION_DMSERVER_DISPLAY_MANAGER_PROXY_H