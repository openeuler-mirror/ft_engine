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

#ifndef OHOS_INPUT_TRANSFER_STATION_H
#define OHOS_INPUT_TRANSFER_STATION_H

#include "input_manager.h"
#include "pointer_event.h"
#include "window.h"
#include "window_input_channel.h"
#include "wm_single_instance.h"
#include "vsync_station.h"

namespace OHOS {
namespace Rosen {
class InputEventListener;

class InputTransferStation {
WM_DECLARE_SINGLE_INSTANCE(InputTransferStation);
friend class InputEventListener;
public:
    void AddInputWindow(const sptr<Window>& window);
    void RemoveInputWindow(uint32_t windowId);

private:
    sptr<WindowInputChannel> GetInputChannel(uint32_t windowId);

    std::mutex mtx_;
    std::unordered_map<uint32_t, sptr<WindowInputChannel>> windowInputChannels_;
    std::shared_ptr<MMI::IInputEventConsumer> inputListener_ = nullptr;
    std::shared_ptr<AppExecFwk::EventHandler> eventHandler_ = nullptr;
    const std::string INPUT_AND_VSYNC_THREAD = "InputAndVsyncThread";
};

class InputEventListener : public MMI::IInputEventConsumer {
public:
    InputEventListener() = default;
    void OnInputEvent(std::shared_ptr<MMI::PointerEvent> pointerEvent) const override;
    void OnInputEvent(std::shared_ptr<MMI::KeyEvent> keyEvent) const override;
    void OnInputEvent(std::shared_ptr<MMI::AxisEvent> axisEvent) const override;
};
} // namespace Rosen
} // namespace OHOS


#endif // OHOS_INPUT_TRANSFER_STATION_H
