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
#ifndef OHOS_WINDOW_INPUT_CHANNEL_H
#define OHOS_WINDOW_INPUT_CHANNEL_H

#include <i_input_event_consumer.h>
#include <key_event.h>
#include "refbase.h"
#include "vsync_station.h"
#include "window.h"

namespace OHOS {
namespace Rosen {
class WindowInputChannel final: public RefBase {
public:
    explicit WindowInputChannel(const sptr<Window>& window);
    virtual ~WindowInputChannel();
    void HandlePointerEvent(std::shared_ptr<MMI::PointerEvent>& pointerEvent);
    void HandleKeyEvent(std::shared_ptr<MMI::KeyEvent>& keyEvent);
    void Destroy();
private:
    bool IsKeyboardEvent(const std::shared_ptr<MMI::KeyEvent>& keyEvent) const;
    std::mutex mtx_;
    sptr<Window> window_;
    bool isAvailable_;
    static const int32_t MAX_INPUT_NUM = 100;
};
}
}


#endif // OHOS_WINDOW_INPUT_CHANNEL_H
