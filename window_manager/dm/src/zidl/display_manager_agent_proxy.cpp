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

#include "zidl/display_manager_agent_proxy.h"
#include <ipc_types.h>
#include "marshalling_helper.h"
#include "window_manager_hilog.h"

namespace OHOS {
namespace Rosen {
namespace {
    constexpr HiviewDFX::HiLogLabel LABEL = {LOG_CORE, HILOG_DOMAIN_DISPLAY, "DisplayManagerAgentProxy"};
}

void DisplayManagerAgentProxy::NotifyDisplayPowerEvent(DisplayPowerEvent event, EventStatus status)
{
    MessageParcel data;
    MessageParcel reply;
    MessageOption option(MessageOption::TF_ASYNC);
    if (!data.WriteInterfaceToken(GetDescriptor())) {
        WLOGFE("WriteInterfaceToken failed");
        return;
    }

    if (!data.WriteUint32(static_cast<uint32_t>(event))) {
        WLOGFE("Write event failed");
        return;
    }

    if (!data.WriteUint32(static_cast<uint32_t>(status))) {
        WLOGFE("Write status failed");
        return;
    }

    if (Remote()->SendRequest(TRANS_ID_NOTIFY_DISPLAY_POWER_EVENT, data, reply, option) != ERR_NONE) {
        WLOGFE("SendRequest failed");
    }
}

void DisplayManagerAgentProxy::NotifyDisplayStateChanged(DisplayId id, DisplayState state)
{
    MessageParcel data;
    MessageParcel reply;
    MessageOption option(MessageOption::TF_ASYNC);
    if (!data.WriteInterfaceToken(GetDescriptor())) {
        WLOGFE("WriteInterfaceToken failed");
        return;
    }

    if (!data.WriteUint32(static_cast<uint32_t>(state))) {
        WLOGFE("Write DisplayState failed");
        return;
    }

    if (!data.WriteUint64(static_cast<uint64_t>(id))) {
        WLOGFE("Write displayId failed");
        return;
    }

    if (Remote()->SendRequest(TRANS_ID_NOTIFY_DISPLAY_STATE_CHANGED, data, reply, option) != ERR_NONE) {
        WLOGFE("SendRequest failed");
    }
}

void DisplayManagerAgentProxy::OnScreenConnect(sptr<ScreenInfo> screenInfo)
{
    MessageParcel data;
    MessageParcel reply;
    MessageOption option(MessageOption::TF_ASYNC);
    if (!data.WriteInterfaceToken(GetDescriptor())) {
        WLOGFE("WriteInterfaceToken failed");
        return;
    }

    if (!data.WriteParcelable(screenInfo.GetRefPtr())) {
        WLOGFE("Write ScreenInfo failed");
        return;
    }

    if (Remote()->SendRequest(TRANS_ID_ON_SCREEN_CONNECT, data, reply, option) != ERR_NONE) {
        WLOGFE("SendRequest failed");
    }
}

void DisplayManagerAgentProxy::OnScreenDisconnect(ScreenId screenId)
{
    MessageParcel data;
    MessageParcel reply;
    MessageOption option(MessageOption::TF_ASYNC);
    if (!data.WriteInterfaceToken(GetDescriptor())) {
        WLOGFE("WriteInterfaceToken failed");
        return;
    }

    if (!data.WriteUint64(screenId)) {
        WLOGFE("Write ScreenId failed");
        return;
    }

    if (Remote()->SendRequest(TRANS_ID_ON_SCREEN_DISCONNECT, data, reply, option) != ERR_NONE) {
        WLOGFE("SendRequest failed");
    }
}

void DisplayManagerAgentProxy::OnScreenChange(const sptr<ScreenInfo>& screenInfo, ScreenChangeEvent event)
{
    MessageParcel data;
    MessageParcel reply;
    MessageOption option(MessageOption::TF_ASYNC);
    if (!data.WriteInterfaceToken(GetDescriptor())) {
        WLOGFE("WriteInterfaceToken failed");
        return;
    }

    if (!data.WriteParcelable(screenInfo.GetRefPtr())) {
        WLOGFE("Write screenInfo failed");
        return;
    }

    if (!data.WriteUint32(static_cast<uint32_t>(event))) {
        WLOGFE("Write ScreenChangeEvent failed");
        return;
    }

    if (Remote()->SendRequest(TRANS_ID_ON_SCREEN_CHANGED, data, reply, option) != ERR_NONE) {
        WLOGFE("SendRequest failed");
    }
}

void DisplayManagerAgentProxy::OnScreenGroupChange(const std::string& trigger,
    const std::vector<sptr<ScreenInfo>>& screenInfos, ScreenGroupChangeEvent event)
{
    MessageParcel data;
    MessageParcel reply;
    MessageOption option(MessageOption::TF_ASYNC);
    if (!data.WriteInterfaceToken(GetDescriptor())) {
        WLOGFE("WriteInterfaceToken failed");
        return;
    }

    if (!data.WriteString(trigger)) {
        WLOGFE("Write trigger failed");
        return;
    }

    if (!MarshallingHelper::MarshallingVectorParcelableObj<ScreenInfo>(data, screenInfos)) {
        WLOGFE("Write screenInfos failed");
        return;
    }

    if (!data.WriteUint32(static_cast<uint32_t>(event))) {
        WLOGFE("Write ScreenGroupChangeEvent failed");
        return;
    }

    if (Remote()->SendRequest(TRANS_ID_ON_SCREENGROUP_CHANGED, data, reply, option) != ERR_NONE) {
        WLOGFE("SendRequest failed");
    }
}

void DisplayManagerAgentProxy::OnDisplayCreate(sptr<DisplayInfo> displayInfo)
{
    MessageParcel data;
    MessageParcel reply;
    MessageOption option(MessageOption::TF_ASYNC);
    if (!data.WriteInterfaceToken(GetDescriptor())) {
        WLOGFE("WriteInterfaceToken failed");
        return;
    }

    if (!data.WriteParcelable(displayInfo.GetRefPtr())) {
        WLOGFE("Write DisplayInfo failed");
        return;
    }

    if (Remote()->SendRequest(TRANS_ID_ON_DISPLAY_CONNECT, data, reply, option) != ERR_NONE) {
        WLOGFE("SendRequest failed");
    }
}

void DisplayManagerAgentProxy::OnDisplayDestroy(DisplayId displayId)
{
    MessageParcel data;
    MessageParcel reply;
    MessageOption option(MessageOption::TF_ASYNC);
    if (!data.WriteInterfaceToken(GetDescriptor())) {
        WLOGFE("WriteInterfaceToken failed");
        return;
    }

    if (!data.WriteUint64(displayId)) {
        WLOGFE("Write DisplayId failed");
        return;
    }

    if (Remote()->SendRequest(TRANS_ID_ON_DISPLAY_DISCONNECT, data, reply, option) != ERR_NONE) {
        WLOGFE("SendRequest failed");
    }
}

void DisplayManagerAgentProxy::OnDisplayChange(sptr<DisplayInfo> displayInfo, DisplayChangeEvent event)
{
    MessageParcel data;
    MessageParcel reply;
    MessageOption option(MessageOption::TF_ASYNC);
    if (!data.WriteInterfaceToken(GetDescriptor())) {
        WLOGFE("WriteInterfaceToken failed");
        return;
    }

    if (!data.WriteParcelable(displayInfo.GetRefPtr())) {
        WLOGFE("Write DisplayInfo failed");
        return;
    }

    if (!data.WriteUint32(static_cast<uint32_t>(event))) {
        WLOGFE("Write DisplayChangeEvent failed");
        return;
    }

    if (Remote()->SendRequest(TRANS_ID_ON_DISPLAY_CHANGED, data, reply, option) != ERR_NONE) {
        WLOGFE("SendRequest failed");
    }
}

void DisplayManagerAgentProxy::OnScreenshot(sptr<ScreenshotInfo> snapshotInfo)
{
    MessageParcel data;
    MessageParcel reply;
    MessageOption option(MessageOption::TF_ASYNC);
    if (!data.WriteInterfaceToken(GetDescriptor())) {
        WLOGFE("WriteInterfaceToken failed");
        return;
    }
    if (!data.WriteParcelable(snapshotInfo.GetRefPtr())) {
        WLOGFE("Write ScreenshotInfo failed");
        return;
    }
    if (Remote()->SendRequest(TRANS_ID_ON_SCREEN_SHOT, data, reply, option) != ERR_NONE) {
        WLOGFE("SendRequest failed");
    }
}
} // namespace Rosen
} // namespace OHOS

