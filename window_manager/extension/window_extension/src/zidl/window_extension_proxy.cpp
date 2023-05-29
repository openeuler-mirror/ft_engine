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

#include "window_extension_proxy.h"

#include <ipc_types.h>
#include <message_option.h>

#include "window_manager_hilog.h"

namespace OHOS {
namespace Rosen {
namespace {
    constexpr HiviewDFX::HiLogLabel LABEL = {LOG_CORE, HILOG_DOMAIN_WINDOW, "WindowExtensionProxy"};
}

void WindowExtensionProxy::SetBounds(const Rect& rect)
{
    MessageParcel data;
    MessageParcel reply;
    MessageOption option;

    if (!data.WriteInterfaceToken(GetDescriptor())) {
        WLOGFE("write interface token failed");
        return;
    }
    if (!(data.WriteInt32(rect.posX_) && data.WriteInt32(rect.posY_) &&
        data.WriteInt32(rect.width_) && data.WriteInt32(rect.height_))) {
        WLOGFE("write rect failed");
        return;
    }
    if (Remote()->SendRequest(TRANS_ID_SETBOUNDS, data, reply, option) != ERR_NONE) {
        WLOGFE("send request failed");
        return;
    }
}

void WindowExtensionProxy::Hide()
{
    MessageParcel data;
    MessageParcel reply;
    MessageOption option;
    if (!data.WriteInterfaceToken(GetDescriptor())) {
        WLOGFE("write interface token failed");
        return;
    }
    if (Remote()->SendRequest(TRANS_ID_HIDE_WINDOW, data, reply, option) != ERR_NONE) {
        WLOGFE("send request failed");
    }
}

void WindowExtensionProxy::Show()
{
    MessageParcel data;
    MessageParcel reply;
    MessageOption option;
    if (!data.WriteInterfaceToken(GetDescriptor())) {
        WLOGFE("write interface token failed");
        return;
    }
    if (Remote()->SendRequest(TRANS_ID_SHOW_WINDOW, data, reply, option) != ERR_NONE) {
        WLOGFE("send request failed");
    }
}

void WindowExtensionProxy::GetExtensionWindow(sptr<IWindowExtensionClient>& token)
{
    MessageParcel data;
    MessageParcel replay;
    MessageOption option;
    if (!data.WriteInterfaceToken(GetDescriptor())) {
        WLOGFE("write interface token failed");
        return;
    }
    if (!data.WriteRemoteObject(token->AsObject())) {
        WLOGFE("write object failed");
        return;
    }
    if (Remote()->SendRequest(TRANS_ID_CONNECT_TO_EXTENSION, data, replay, option) != ERR_NONE) {
        WLOGFE("send request failed");
    }
}

void WindowExtensionProxy::RequestFocus()
{
    WLOGFI("called.");
}
} // namespace Rosen
} // namespace OHOS