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

#ifndef FOUNDATION_DMSERVER_DISPLAY_MANAGER_INTERFACE_H
#define FOUNDATION_DMSERVER_DISPLAY_MANAGER_INTERFACE_H

#include <iremote_broker.h>
#include <pixel_map.h>
#include <surface.h>

#include "display_cutout_controller.h"
#include "display_info.h"
#include "dm_common.h"
#include "screen.h"
#include "screen_info.h"
#include "screen_group_info.h"
#include "zidl/display_manager_agent_interface.h"

namespace OHOS::Rosen {
class IDisplayManager : public IRemoteBroker {
public:
    DECLARE_INTERFACE_DESCRIPTOR(u"OHOS.IDisplayManager");

    enum class DisplayManagerMessage : uint32_t {
        TRANS_ID_GET_DEFAULT_DISPLAY_INFO = 0,
        TRANS_ID_GET_DISPLAY_BY_ID,
        TRANS_ID_GET_DISPLAY_BY_SCREEN,
        TRANS_ID_GET_DISPLAY_SNAPSHOT,
        TRANS_ID_REGISTER_DISPLAY_MANAGER_AGENT,
        TRANS_ID_UNREGISTER_DISPLAY_MANAGER_AGENT,
        TRANS_ID_WAKE_UP_BEGIN,
        TRANS_ID_WAKE_UP_END,
        TRANS_ID_SUSPEND_BEGIN,
        TRANS_ID_SUSPEND_END,
        TRANS_ID_SET_SCREEN_POWER_FOR_ALL,
        TRANS_ID_GET_SCREEN_POWER,
        TRANS_ID_SET_DISPLAY_STATE,
        TRANS_ID_GET_DISPLAY_STATE,
        TRANS_ID_GET_ALL_DISPLAYIDS,
        TRANS_ID_NOTIFY_DISPLAY_EVENT,
        TRANS_ID_SET_FREEZE_EVENT,
        TRANS_ID_SCREEN_BASE = 1000,
        TRANS_ID_CREATE_VIRTUAL_SCREEN = TRANS_ID_SCREEN_BASE,
        TRANS_ID_DESTROY_VIRTUAL_SCREEN,
        TRANS_ID_SET_VIRTUAL_SCREEN_SURFACE,
        TRANS_ID_GET_SCREEN_INFO_BY_ID,
        TRANS_ID_GET_SCREEN_GROUP_INFO_BY_ID,
        TRANS_ID_SET_SCREEN_ACTIVE_MODE,
        TRANS_ID_GET_ALL_SCREEN_INFOS,
        TRANS_ID_SET_ORIENTATION,
        TRANS_ID_SET_VIRTUAL_PIXEL_RATIO,
        TRANS_ID_SCREENGROUP_BASE = 1100,
        TRANS_ID_SCREEN_MAKE_MIRROR = TRANS_ID_SCREENGROUP_BASE,
        TRANS_ID_SCREEN_MAKE_EXPAND,
        TRANS_ID_REMOVE_VIRTUAL_SCREEN_FROM_SCREEN_GROUP,
        TRANS_ID_SCREEN_GAMUT_BASE = 1200,
        TRANS_ID_SCREEN_GET_SUPPORTED_COLOR_GAMUTS = TRANS_ID_SCREEN_GAMUT_BASE,
        TRANS_ID_SCREEN_GET_COLOR_GAMUT,
        TRANS_ID_SCREEN_SET_COLOR_GAMUT,
        TRANS_ID_SCREEN_GET_GAMUT_MAP,
        TRANS_ID_SCREEN_SET_GAMUT_MAP,
        TRANS_ID_SCREEN_SET_COLOR_TRANSFORM,
        TRANS_ID_IS_SCREEN_ROTATION_LOCKED,
        TRANS_ID_SET_SCREEN_ROTATION_LOCKED,
        TRANS_ID_HAS_PRIVATE_WINDOW,
        TRANS_ID_GET_CUTOUT_INFO,
    };

    virtual sptr<DisplayInfo> GetDefaultDisplayInfo() = 0;
    virtual sptr<DisplayInfo> GetDisplayInfoById(DisplayId displayId) = 0;
    virtual sptr<DisplayInfo> GetDisplayInfoByScreen(ScreenId screenId) = 0;
    virtual DMError HasPrivateWindow(DisplayId displayId, bool& hasPrivateWindow) = 0;

    virtual ScreenId CreateVirtualScreen(VirtualScreenOption option,
        const sptr<IRemoteObject>& displayManagerAgent) = 0;
    virtual DMError DestroyVirtualScreen(ScreenId screenId) = 0;
    virtual DMError SetVirtualScreenSurface(ScreenId screenId, sptr<Surface> surface) = 0;
    virtual bool SetOrientation(ScreenId screenId, Orientation orientation) = 0;
    virtual std::shared_ptr<Media::PixelMap> GetDisplaySnapshot(DisplayId displayId) = 0;
    virtual void SetScreenRotationLocked(bool isLocked) = 0;
    virtual bool IsScreenRotationLocked() = 0;

    // colorspace, gamut
    virtual DMError GetScreenSupportedColorGamuts(ScreenId screenId, std::vector<ScreenColorGamut>& colorGamuts) = 0;
    virtual DMError GetScreenColorGamut(ScreenId screenId, ScreenColorGamut& colorGamut) = 0;
    virtual DMError SetScreenColorGamut(ScreenId screenId, int32_t colorGamutIdx) = 0;
    virtual DMError GetScreenGamutMap(ScreenId screenId, ScreenGamutMap& gamutMap) = 0;
    virtual DMError SetScreenGamutMap(ScreenId screenId, ScreenGamutMap gamutMap) = 0;
    virtual DMError SetScreenColorTransform(ScreenId screenId) = 0;

    virtual bool RegisterDisplayManagerAgent(const sptr<IDisplayManagerAgent>& displayManagerAgent,
        DisplayManagerAgentType type) = 0;
    virtual bool UnregisterDisplayManagerAgent(const sptr<IDisplayManagerAgent>& displayManagerAgent,
        DisplayManagerAgentType type) = 0;
    virtual bool WakeUpBegin(PowerStateChangeReason reason) = 0;
    virtual bool WakeUpEnd() = 0;
    virtual bool SuspendBegin(PowerStateChangeReason reason) = 0;
    virtual bool SuspendEnd() = 0;
    virtual bool SetScreenPowerForAll(ScreenPowerState state, PowerStateChangeReason reason) = 0;
    virtual ScreenPowerState GetScreenPower(ScreenId dmsScreenId) = 0;
    virtual bool SetDisplayState(DisplayState state) = 0;
    virtual DisplayState GetDisplayState(DisplayId displayId) = 0;
    virtual std::vector<DisplayId> GetAllDisplayIds() = 0;
    virtual sptr<CutoutInfo> GetCutoutInfo(DisplayId displayId) = 0;
    virtual void NotifyDisplayEvent(DisplayEvent event) = 0;
    virtual bool SetFreeze(std::vector<DisplayId> displayIds, bool isFreeze) = 0;
    virtual sptr<ScreenInfo> GetScreenInfoById(ScreenId screenId) = 0;
    virtual sptr<ScreenGroupInfo> GetScreenGroupInfoById(ScreenId screenId) = 0;
    virtual std::vector<sptr<ScreenInfo>> GetAllScreenInfos() = 0;
    virtual ScreenId MakeMirror(ScreenId mainScreenId, std::vector<ScreenId> mirrorScreenId) = 0;
    virtual ScreenId MakeExpand(std::vector<ScreenId> screenId, std::vector<Point> startPoint) = 0;
    virtual void RemoveVirtualScreenFromGroup(std::vector<ScreenId> screens) = 0;
    virtual bool SetScreenActiveMode(ScreenId screenId, uint32_t modeId) = 0;
    virtual bool SetVirtualPixelRatio(ScreenId screenId, float virtualPixelRatio) = 0;
};
} // namespace OHOS::Rosen

#endif // FOUNDATION_DMSERVER_DISPLAY_MANAGER_INTERFACE_H