/*
 * Copyright (c) 2021 Huawei Device Co., Ltd.
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

#ifndef OHOS_WINDOW_MANAGER_AGENT_PROXY_H
#define OHOS_WINDOW_MANAGER_AGENT_PROXY_H

#include "window_manager_agent_interface.h"
#include "iremote_proxy.h"

namespace OHOS {
namespace Rosen {
class WindowManagerAgentProxy : public IRemoteProxy<IWindowManagerAgent> {
public:
    explicit WindowManagerAgentProxy(const sptr<IRemoteObject>& impl) : IRemoteProxy<IWindowManagerAgent>(impl) {};

    ~WindowManagerAgentProxy() {};

    void UpdateFocusChangeInfo(const sptr<FocusChangeInfo>& focusChangeInfo, bool focused) override;
    void UpdateSystemBarRegionTints(DisplayId displayId, const SystemBarRegionTints& tints) override;
    void NotifyAccessibilityWindowInfo(const std::vector<sptr<AccessibilityWindowInfo>>& infos,
        WindowUpdateType type) override;
    void UpdateWindowVisibilityInfo(const std::vector<sptr<WindowVisibilityInfo>>& visibilityInfos) override;
    void UpdateCameraFloatWindowStatus(uint32_t accessTokenId, bool isShowing) override;

private:
    static inline BrokerDelegator<WindowManagerAgentProxy> delegator_;
};
} // namespace Rosen
} // namespace OHOS
#endif // OHOS_WINDOW_MANAGER_PROXY_H
