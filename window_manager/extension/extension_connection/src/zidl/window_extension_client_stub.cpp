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

#include "window_extension_client_stub.h"

#include "window_manager_hilog.h"

namespace OHOS {
namespace Rosen {
namespace {
    constexpr HiviewDFX::HiLogLabel LABEL = {LOG_CORE, HILOG_DOMAIN_WINDOW, "WindowExtensionClientStub"};
}
int WindowExtensionClientStub::OnRemoteRequest(uint32_t code, MessageParcel& data,
    MessageParcel& reply, MessageOption& option)
{
    if (data.ReadInterfaceToken() != GetDescriptor()) {
        WLOGFE("InterfaceToken check failed");
        return -1;
    }
    WLOGFI(" code is %{public}d", code);
    switch (code) {
        case TRANS_ID_ON_WINDOW_READY: {
            std::shared_ptr<RSSurfaceNode> surfaceNode = RSSurfaceNode::Unmarshalling(data);
            OnWindowReady(surfaceNode);
            break;
        }
        case TRANS_ID_ON_BACK_PRESS: {
            OnBackPress();
            break;
        }
        case TRANS_ID_ON_KEY_EVENT: {
            std::shared_ptr<MMI::KeyEvent> keyEvent = MMI::KeyEvent::Create();
            if (keyEvent == nullptr) {
                WLOGFE("create keyevent failed");
                break;
            }
            keyEvent->ReadFromParcel(data);
            OnKeyEvent(keyEvent);
            break;
        }
        case TRANS_ID_ON_POINTER_EVENT: {
            std::shared_ptr<MMI::PointerEvent> pointerEvent = MMI::PointerEvent::Create();
            if (pointerEvent == nullptr) {
                WLOGFE("create pointer event failed");
                break;
            }
            pointerEvent->ReadFromParcel(data);
            OnPointerEvent(pointerEvent);
            break;
        }
        default: {
            break;
        }
    }
    return 0;
}
} // namespace Rosen
} // namespace OHOS