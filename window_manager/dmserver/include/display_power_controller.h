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

#ifndef OHOS_ROSEN_DISPLAY_POWER_CONTROLLER_H
#define OHOS_ROSEN_DISPLAY_POWER_CONTROLLER_H

#include <map>
#include <mutex>
#include <refbase.h>
#include "display.h"
#include "display_change_listener.h"
#include "dm_common.h"

namespace OHOS {
namespace Rosen {
class DisplayPowerController : public RefBase {
using DisplayStateChangeListener = std::function<void(DisplayId, sptr<DisplayInfo>,
    const std::map<DisplayId, sptr<DisplayInfo>>&, DisplayStateChangeType)>;
public:
    DisplayPowerController(std::recursive_mutex& mutex, DisplayStateChangeListener listener)
        : mutex_(mutex), displayStateChangeListener_(listener)
    {
    }
    virtual ~DisplayPowerController() = default;

    bool SuspendBegin(PowerStateChangeReason reason);
    bool SetDisplayState(DisplayState state);
    DisplayState GetDisplayState(DisplayId displayId);
    void NotifyDisplayEvent(DisplayEvent event);

private:
    DisplayState displayState_ { DisplayState::UNKNOWN };
    bool isKeyguardDrawn_ { false };
    std::recursive_mutex& mutex_;
    DisplayStateChangeListener displayStateChangeListener_;
};
}
}
#endif // OHOS_ROSEN_DISPLAY_POWER_CONTROLLER_H