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

#include "zidl/window_manager_agent_proxy.h"
#include <ipc_types.h>
#include "marshalling_helper.h"
#include "window_manager_hilog.h"
#include "wm_common.h"

namespace OHOS {
namespace Rosen {
namespace {
    constexpr HiviewDFX::HiLogLabel LABEL = {LOG_CORE, HILOG_DOMAIN_WINDOW, "WindowManagerAgentProxy"};
}

void WindowManagerAgentProxy::UpdateFocusChangeInfo(const sptr<FocusChangeInfo>& focusChangeInfo, bool focused)
{
    MessageParcel data;
    MessageParcel reply;
    MessageOption option(MessageOption::TF_ASYNC);
    if (focusChangeInfo == nullptr) {
        WLOGFE("Invalid focus change info");
        return;
    }

    if (!data.WriteInterfaceToken(GetDescriptor())) {
        WLOGFE("WriteInterfaceToken failed");
        return;
    }

    if (!data.WriteParcelable(focusChangeInfo)) {
        WLOGFE("Write displayId failed");
        return;
    }

    if (!data.WriteRemoteObject(focusChangeInfo->abilityToken_)) {
        WLOGFI("Write abilityToken failed");
    }

    if (!data.WriteBool(focused)) {
        WLOGFE("Write Focus failed");
        return;
    }

    if (Remote()->SendRequest(static_cast<uint32_t>(WindowManagerAgentMsg::TRANS_ID_UPDATE_FOCUS),
        data, reply, option) != ERR_NONE) {
        WLOGFE("SendRequest failed");
    }
}

void WindowManagerAgentProxy::UpdateSystemBarRegionTints(DisplayId displayId, const SystemBarRegionTints& tints)
{
    MessageParcel data;
    MessageParcel reply;
    MessageOption option(MessageOption::TF_ASYNC);
    if (!data.WriteInterfaceToken(GetDescriptor())) {
        WLOGFE("WriteInterfaceToken failed");
        return;
    }

    if (!data.WriteUint64(displayId)) {
        WLOGFE("Write displayId failed");
        return;
    }
    bool res = MarshallingHelper::MarshallingVectorObj<SystemBarRegionTint>(data, tints,
        [](Parcel& parcel, const SystemBarRegionTint& tint) {
            return parcel.WriteUint32(static_cast<uint32_t>(tint.type_)) && parcel.WriteBool(tint.prop_.enable_) &&
                parcel.WriteUint32(tint.prop_.backgroundColor_) && parcel.WriteUint32(tint.prop_.contentColor_) &&
                parcel.WriteInt32(tint.region_.posX_) && parcel.WriteInt32(tint.region_.posY_) &&
                parcel.WriteInt32(tint.region_.width_) && parcel.WriteInt32(tint.region_.height_);
        }
    );
    if (!res) {
        WLOGFE("Write SystemBarRegionTint failed");
        return;
    }
    if (Remote()->SendRequest(static_cast<uint32_t>(WindowManagerAgentMsg::TRANS_ID_UPDATE_SYSTEM_BAR_PROPS),
        data, reply, option) != ERR_NONE) {
        WLOGFE("SendRequest failed");
    }
}

void WindowManagerAgentProxy::NotifyAccessibilityWindowInfo(const std::vector<sptr<AccessibilityWindowInfo>>& infos,
    WindowUpdateType type)
{
    MessageParcel data;
    MessageParcel reply;
    MessageOption option(MessageOption::TF_ASYNC);
    if (!data.WriteInterfaceToken(GetDescriptor())) {
        WLOGFE("WriteInterfaceToken failed");
        return;
    }

    if (!MarshallingHelper::MarshallingVectorParcelableObj<AccessibilityWindowInfo>(data, infos)) {
        WLOGFE("Write accessibility window infos failed");
        return;
    }

    if (!data.WriteUint32(static_cast<uint32_t>(type))) {
        WLOGFE("Write windowUpdateType failed");
        return;
    }
    if (Remote()->SendRequest(static_cast<uint32_t>(WindowManagerAgentMsg::TRANS_ID_UPDATE_WINDOW_STATUS),
        data, reply, option) != ERR_NONE) {
        WLOGFE("SendRequest failed");
    }
}

void WindowManagerAgentProxy::UpdateWindowVisibilityInfo(
    const std::vector<sptr<WindowVisibilityInfo>>& visibilityInfos)
{
    MessageParcel data;
    MessageParcel reply;
    MessageOption option(MessageOption::TF_ASYNC);
    if (!data.WriteInterfaceToken(GetDescriptor())) {
        WLOGFE("WriteInterfaceToken failed");
        return;
    }
    if (!data.WriteUint32(static_cast<uint32_t>(visibilityInfos.size()))) {
        WLOGFE("write windowVisibilityInfos size failed");
        return;
    }
    for (auto& info : visibilityInfos) {
        if (!data.WriteParcelable(info)) {
            WLOGFE("Write windowVisibilityInfo failed");
            return;
        }
    }

    if (Remote()->SendRequest(static_cast<uint32_t>(WindowManagerAgentMsg::TRANS_ID_UPDATE_WINDOW_VISIBILITY),
        data, reply, option) != ERR_NONE) {
        WLOGFE("SendRequest failed");
    }
}

void WindowManagerAgentProxy::UpdateCameraFloatWindowStatus(uint32_t accessTokenId, bool isShowing)
{
    MessageParcel data;
    MessageParcel reply;
    MessageOption option(MessageOption::TF_ASYNC);
    if (!data.WriteInterfaceToken(GetDescriptor())) {
        WLOGFE("WriteInterfaceToken failed");
        return;
    }

    if (!data.WriteUint32(accessTokenId)) {
        WLOGFE("Write accessTokenId failed");
        return;
    }

    if (!data.WriteBool(isShowing)) {
        WLOGFE("Write is showing status failed");
        return;
    }

    if (Remote()->SendRequest(static_cast<uint32_t>(WindowManagerAgentMsg::TRANS_ID_UPDATE_CAMERA_FLOAT),
        data, reply, option) != ERR_NONE) {
        WLOGFE("SendRequest failed");
    }
}
} // namespace Rosen
} // namespace OHOS

