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

#ifndef OHOS_ROSEN_DISPLAY_MANAGER_AGENT_INTERFACE_H
#define OHOS_ROSEN_DISPLAY_MANAGER_AGENT_INTERFACE_H

#include <iremote_broker.h>
#include "display_info.h"
#include "dm_common.h"
#include "screen_info.h"
#include "screenshot_info.h"

namespace OHOS {
namespace Rosen {
enum class DisplayManagerAgentType : uint32_t {
    DISPLAY_POWER_EVENT_LISTENER,
    DISPLAY_STATE_LISTENER,
    SCREEN_EVENT_LISTENER,
    DISPLAY_EVENT_LISTENER,
    SCREENSHOT_EVENT_LISTENER,
};

class IDisplayManagerAgent : public IRemoteBroker {
public:
    DECLARE_INTERFACE_DESCRIPTOR(u"OHOS.IDisplayManagerAgent");

    enum {
        TRANS_ID_NOTIFY_DISPLAY_POWER_EVENT = 1,
        TRANS_ID_NOTIFY_DISPLAY_STATE_CHANGED,
        TRANS_ID_ON_SCREEN_CONNECT,
        TRANS_ID_ON_SCREEN_DISCONNECT,
        TRANS_ID_ON_SCREEN_CHANGED,
        TRANS_ID_ON_SCREENGROUP_CHANGED,
        TRANS_ID_ON_DISPLAY_CONNECT,
        TRANS_ID_ON_DISPLAY_DISCONNECT,
        TRANS_ID_ON_DISPLAY_CHANGED,
        TRANS_ID_ON_SCREEN_SHOT,
    };
    virtual void NotifyDisplayPowerEvent(DisplayPowerEvent event, EventStatus status) = 0;
    virtual void NotifyDisplayStateChanged(DisplayId id, DisplayState state) = 0;
    virtual void OnScreenConnect(sptr<ScreenInfo>) = 0;
    virtual void OnScreenDisconnect(ScreenId) = 0;
    virtual void OnScreenChange(const sptr<ScreenInfo>&, ScreenChangeEvent) = 0;
    virtual void OnScreenGroupChange(const std::string& trigger,
        const std::vector<sptr<ScreenInfo>>&, ScreenGroupChangeEvent) = 0;
    virtual void OnDisplayCreate(sptr<DisplayInfo>) = 0;
    virtual void OnDisplayDestroy(DisplayId) = 0;
    virtual void OnDisplayChange(sptr<DisplayInfo>, DisplayChangeEvent) = 0;
    virtual void OnScreenshot(sptr<ScreenshotInfo>) = 0;
};
} // namespace Rosen
} // namespace OHOS
#endif // OHOS_ROSEN_DISPLAY_MANAGER_AGENT_INTERFACE_H