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

#ifndef OHOS_ROSEN_DISPLAY_MANAGER_AGRNT_PROXY_H
#define OHOS_ROSEN_DISPLAY_MANAGER_AGRNT_PROXY_H

#include <iremote_proxy.h>
#include "display_manager_agent_interface.h"

namespace OHOS {
namespace Rosen {
class DisplayManagerAgentProxy : public IRemoteProxy<IDisplayManagerAgent> {
public:
    explicit DisplayManagerAgentProxy(const sptr<IRemoteObject>& impl) : IRemoteProxy<IDisplayManagerAgent>(impl) {};
    ~DisplayManagerAgentProxy() = default;

    virtual void NotifyDisplayPowerEvent(DisplayPowerEvent event, EventStatus status) override;
    virtual void NotifyDisplayStateChanged(DisplayId id, DisplayState state) override;
    virtual void OnScreenConnect(sptr<ScreenInfo>) override;
    virtual void OnScreenDisconnect(ScreenId) override;
    virtual void OnScreenChange(const sptr<ScreenInfo>&, ScreenChangeEvent) override;
    virtual void OnScreenGroupChange(const std::string& trigger,
        const std::vector<sptr<ScreenInfo>>&, ScreenGroupChangeEvent) override;
    virtual void OnDisplayCreate(sptr<DisplayInfo>) override;
    virtual void OnDisplayDestroy(DisplayId) override;
    virtual void OnDisplayChange(sptr<DisplayInfo>, DisplayChangeEvent) override;
    virtual void OnScreenshot(sptr<ScreenshotInfo>) override;
private:
    static inline BrokerDelegator<DisplayManagerAgentProxy> delegator_;
};
} // namespace Rosen
} // namespace OHOS
#endif // OHOS_ROSEN_DISPLAY_MANAGER_AGRNT_PROXY_H