/*
 * Copyright (c) 2023 Huawei Technologies Co., Ltd.
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

#ifndef OHOS_ABILITY_RUNTIME_WINDOW_MANAGER_SERVICE_HANDLER_STUB_H
#define OHOS_ABILITY_RUNTIME_WINDOW_MANAGER_SERVICE_HANDLER_STUB_H

#include "iremote_stub.h"
#include "iremote_broker.h"
#include "window_info.h"
#include "pixel_map.h"

namespace OHOS {
namespace AAFwk {
class IWindowManagerServiceHandler : public OHOS::IRemoteBroker {
public:
    DECLARE_INTERFACE_DESCRIPTOR(u"ohos.aafwk.WindowManagerServiceHandler");

    virtual void NotifyWindowTransition(sptr<AbilityTransitionInfo> fromInfo, sptr<AbilityTransitionInfo> toInfo) = 0;
    virtual int32_t GetFocusWindow(sptr<IRemoteObject>& abilityToken) = 0;
    virtual void StartingWindow(sptr<AbilityTransitionInfo> info,
        std::shared_ptr<Media::PixelMap> pixelMap, uint32_t bgColor) = 0;
    virtual void StartingWindow(sptr<AbilityTransitionInfo> info, std::shared_ptr<Media::PixelMap> pixelMap) = 0;
    virtual void CancelStartingWindow(sptr<IRemoteObject> abilityToken) = 0;
};

class WindowManagerServiceHandlerStub : public IRemoteStub<IWindowManagerServiceHandler> {
public:
    WindowManagerServiceHandlerStub() = default;
    virtual ~WindowManagerServiceHandlerStub() = default;
};
}  // namespace AAFwk
}  // namespace OHOS
#endif  // OHOS_ABILITY_RUNTIME_WINDOW_MANAGER_SERVICE_HANDLER_STUB_H
