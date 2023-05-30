/*
 * Copyright (c) 2022 Huawei Device Co., Ltd.
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

#ifndef WINDOW_EXTENSION_CLIENT_INTERFACE_H
#define WINDOW_EXTENSION_CLIENT_INTERFACE_H

#include <i_input_event_consumer.h>
#include <iremote_broker.h>
#include <key_event.h>
#include <ui/rs_surface_node.h>

namespace OHOS {
namespace Rosen {
class IWindowExtensionClient : public IRemoteBroker {
public:
    DECLARE_INTERFACE_DESCRIPTOR(u"ohos.rosen.window.extension");
    enum {
        TRANS_ID_ON_WINDOW_READY = 1,
        TRANS_ID_ON_BACK_PRESS,
        TRANS_ID_ON_KEY_EVENT,
        TRANS_ID_ON_POINTER_EVENT,
    };

    virtual void OnWindowReady(const std::shared_ptr<RSSurfaceNode>& surfaceNode) = 0;
    virtual void OnBackPress() = 0;
    virtual void OnKeyEvent(const std::shared_ptr<MMI::KeyEvent>& keyEvent) = 0;
    virtual void OnPointerEvent(const std::shared_ptr<MMI::PointerEvent>& pointerEvent) = 0;
};
} // namespace Rosen
} // namespace OHOS
#endif // WINDOW_EXTENSION_CLIENT_INTERFACE_H