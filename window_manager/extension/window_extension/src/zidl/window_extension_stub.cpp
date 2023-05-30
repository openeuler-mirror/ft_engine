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

#include "window_extension_stub.h"
#include "window_manager_hilog.h"

namespace OHOS {
namespace Rosen {
namespace {
    constexpr HiviewDFX::HiLogLabel LABEL = {LOG_CORE, HILOG_DOMAIN_WINDOW, "WindowExtensionStub"};
}

int WindowExtensionStub::OnRemoteRequest(uint32_t code, MessageParcel& data,
    MessageParcel& reply, MessageOption& option)
{
    if (data.ReadInterfaceToken() != GetDescriptor()) {
        WLOGFE("InterfaceToken check failed");
        return -1;
    }
    WLOGFI("code is %{public}u", code);
    switch (code) {
        case TRANS_ID_SETBOUNDS: {
            Rect rect {data.ReadInt32(), data.ReadInt32(),
                data.ReadInt32(), data.ReadInt32()};
            SetBounds(rect);
            break;
        }
        case TRANS_ID_HIDE_WINDOW: {
            Hide();
            break;
        }
        case TRANS_ID_SHOW_WINDOW: {
            Show();
            break;
        }
        case TRANS_ID_REQUESTFOCUS: {
            RequestFocus();
            break;
        }
        case TRANS_ID_CONNECT_TO_EXTENSION: {
            sptr<IRemoteObject> object = data.ReadRemoteObject();
            sptr<IWindowExtensionClient> token = iface_cast<IWindowExtensionClient>(object);
            GetExtensionWindow(token);
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
