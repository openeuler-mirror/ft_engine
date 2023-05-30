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

#ifndef MASTER_DISPLAY_MANAGER_AGENT_DEFAULT_H
#define MASTER_DISPLAY_MANAGER_AGENT_DEFAULT_H

#include <zidl/display_manager_agent_stub.h>

namespace OHOS {
namespace Rosen {
class DisplayManagerAgentDefault : public DisplayManagerAgentStub {
public:
    DisplayManagerAgentDefault() = default;
    ~DisplayManagerAgentDefault() = default;

    void NotifyDisplayPowerEvent(DisplayPowerEvent event, EventStatus status) override {};
    void NotifyDisplayStateChanged(DisplayId id, DisplayState state) override {};
    void OnScreenConnect(sptr<ScreenInfo>) override {};
    void OnScreenDisconnect(ScreenId) override {};
    void OnScreenChange(const sptr<ScreenInfo>&, ScreenChangeEvent) override {};
    void OnScreenGroupChange(const std::string& trigger,
        const std::vector<sptr<ScreenInfo>>&, ScreenGroupChangeEvent) override {};
    void OnDisplayCreate(sptr<DisplayInfo>) override {};
    void OnDisplayDestroy(DisplayId) override {};
    void OnDisplayChange(const sptr<DisplayInfo>, DisplayChangeEvent) override {};
    void OnScreenshot(sptr<ScreenshotInfo>) override {};
};
}
}
#endif // MASTER_DISPLAY_MANAGER_AGENT_DEFAULT_H
