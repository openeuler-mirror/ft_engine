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

#include "display_manager_proxy.h"

#include <cinttypes>
#include <ipc_types.h>
#include <parcel.h>

#include "marshalling_helper.h"
#include "window_manager_hilog.h"

namespace OHOS::Rosen {
namespace {
    constexpr HiviewDFX::HiLogLabel LABEL = {LOG_CORE, HILOG_DOMAIN_DISPLAY, "DisplayManagerProxy"};
}

sptr<DisplayInfo> DisplayManagerProxy::GetDefaultDisplayInfo()
{
    sptr<IRemoteObject> remote = Remote();
    if (remote == nullptr) {
        WLOGFW("GetDefaultDisplayInfo: remote is nullptr");
        return nullptr;
    }

    MessageParcel data;
    MessageParcel reply;
    MessageOption option;
    if (!data.WriteInterfaceToken(GetDescriptor())) {
        WLOGFE("GetDefaultDisplayInfo: WriteInterfaceToken failed");
        return nullptr;
    }
    if (remote->SendRequest(static_cast<uint32_t>(DisplayManagerMessage::TRANS_ID_GET_DEFAULT_DISPLAY_INFO),
        data, reply, option) != ERR_NONE) {
        WLOGFW("GetDefaultDisplayInfo: SendRequest failed");
        return nullptr;
    }
    sptr<DisplayInfo> info = reply.ReadParcelable<DisplayInfo>();
    if (info == nullptr) {
        WLOGFW("DisplayManagerProxy::GetDefaultDisplayInfo SendRequest nullptr.");
    }
    return info;
}

sptr<DisplayInfo> DisplayManagerProxy::GetDisplayInfoById(DisplayId displayId)
{
    sptr<IRemoteObject> remote = Remote();
    if (remote == nullptr) {
        WLOGFW("GetDisplayInfoById: remote is nullptr");
        return nullptr;
    }

    MessageParcel data;
    MessageParcel reply;
    MessageOption option;
    if (!data.WriteInterfaceToken(GetDescriptor())) {
        WLOGFE("GetDisplayInfoById: WriteInterfaceToken failed");
        return nullptr;
    }
    if (!data.WriteUint64(displayId)) {
        WLOGFW("GetDisplayInfoById: WriteUint64 displayId failed");
        return nullptr;
    }
    if (remote->SendRequest(static_cast<uint32_t>(DisplayManagerMessage::TRANS_ID_GET_DISPLAY_BY_ID),
        data, reply, option) != ERR_NONE) {
        WLOGFW("GetDisplayInfoById: SendRequest failed");
        return nullptr;
    }

    sptr<DisplayInfo> info = reply.ReadParcelable<DisplayInfo>();
    if (info == nullptr) {
        WLOGFW("DisplayManagerProxy::GetDisplayInfoById SendRequest nullptr.");
        return nullptr;
    }
    return info;
}

sptr<DisplayInfo> DisplayManagerProxy::GetDisplayInfoByScreen(ScreenId screenId)
{
    sptr<IRemoteObject> remote = Remote();
    if (remote == nullptr) {
        WLOGFE("fail to get displayInfo by screenId: remote is null");
        return nullptr;
    }

    MessageParcel data;
    MessageParcel reply;
    MessageOption option;
    if (!data.WriteInterfaceToken(GetDescriptor())) {
        WLOGFE("fail to get displayInfo by screenId: WriteInterfaceToken failed");
        return nullptr;
    }
    if (!data.WriteUint64(screenId)) {
        WLOGFW("fail to get displayInfo by screenId: WriteUint64 displayId failed");
        return nullptr;
    }
    if (remote->SendRequest(static_cast<uint32_t>(DisplayManagerMessage::TRANS_ID_GET_DISPLAY_BY_SCREEN),
        data, reply, option) != ERR_NONE) {
        WLOGFW("fail to get displayInfo by screenId: SendRequest failed");
        return nullptr;
    }

    sptr<DisplayInfo> info = reply.ReadParcelable<DisplayInfo>();
    if (info == nullptr) {
        WLOGFW("fail to get displayInfo by screenId: SendRequest null");
        return nullptr;
    }
    return info;
}

ScreenId DisplayManagerProxy::CreateVirtualScreen(VirtualScreenOption virtualOption,
    const sptr<IRemoteObject>& displayManagerAgent)
{
    sptr<IRemoteObject> remote = Remote();
    if (remote == nullptr) {
        WLOGFW("CreateVirtualScreen: remote is nullptr");
        return SCREEN_ID_INVALID;
    }

    MessageParcel data;
    MessageParcel reply;
    MessageOption option;
    if (!data.WriteInterfaceToken(GetDescriptor())) {
        WLOGFE("CreateVirtualScreen: WriteInterfaceToken failed");
        return SCREEN_ID_INVALID;
    }
    bool res = data.WriteString(virtualOption.name_) && data.WriteUint32(virtualOption.width_) &&
        data.WriteUint32(virtualOption.height_) && data.WriteFloat(virtualOption.density_) &&
        data.WriteInt32(virtualOption.flags_) && data.WriteBool(virtualOption.isForShot_);
    if (virtualOption.surface_ != nullptr && virtualOption.surface_->GetProducer() != nullptr) {
        res = res &&
            data.WriteBool(true) &&
            data.WriteRemoteObject(virtualOption.surface_->GetProducer()->AsObject());
    } else {
        WLOGFW("CreateVirtualScreen: surface is nullptr");
        res = res && data.WriteBool(false);
    }
    if (displayManagerAgent != nullptr) {
        res = res &&
            data.WriteRemoteObject(displayManagerAgent);
    }
    if (!res) {
        WLOGFE("Write data failed");
        return SCREEN_ID_INVALID;
    }
    if (remote->SendRequest(static_cast<uint32_t>(DisplayManagerMessage::TRANS_ID_CREATE_VIRTUAL_SCREEN),
        data, reply, option) != ERR_NONE) {
        WLOGFW("CreateVirtualScreen: SendRequest failed");
        return SCREEN_ID_INVALID;
    }

    ScreenId screenId = static_cast<ScreenId>(reply.ReadUint64());
    WLOGFI("CreateVirtualScreen %" PRIu64"", screenId);
    return screenId;
}

DMError DisplayManagerProxy::DestroyVirtualScreen(ScreenId screenId)
{
    sptr<IRemoteObject> remote = Remote();
    if (remote == nullptr) {
        WLOGFW("DestroyVirtualScreen: remote is nullptr");
        return DMError::DM_ERROR_REMOTE_CREATE_FAILED;
    }

    MessageParcel data;
    MessageParcel reply;
    MessageOption option;
    if (!data.WriteInterfaceToken(GetDescriptor())) {
        WLOGFE("DestroyVirtualScreen: WriteInterfaceToken failed");
        return DMError::DM_ERROR_WRITE_INTERFACE_TOKEN_FAILED;
    }
    if (!data.WriteUint64(static_cast<uint64_t>(screenId))) {
        WLOGFW("DestroyVirtualScreen: WriteUint64 screenId failed");
        return DMError::DM_ERROR_IPC_FAILED;
    }
    if (remote->SendRequest(static_cast<uint32_t>(DisplayManagerMessage::TRANS_ID_DESTROY_VIRTUAL_SCREEN),
        data, reply, option) != ERR_NONE) {
        WLOGFW("DestroyVirtualScreen: SendRequest failed");
        return DMError::DM_ERROR_IPC_FAILED;
    }
    return static_cast<DMError>(reply.ReadInt32());
}

DMError DisplayManagerProxy::SetVirtualScreenSurface(ScreenId screenId, sptr<Surface> surface)
{
    sptr<IRemoteObject> remote = Remote();
    if (remote == nullptr) {
        WLOGFW("SetVirtualScreenSurface: remote is nullptr");
        return DMError::DM_ERROR_REMOTE_CREATE_FAILED;
    }

    MessageParcel data;
    MessageParcel reply;
    MessageOption option;
    if (!data.WriteInterfaceToken(GetDescriptor())) {
        WLOGFE("SetVirtualScreenSurface: WriteInterfaceToken failed");
        return DMError::DM_ERROR_WRITE_INTERFACE_TOKEN_FAILED;
    }
    bool res = data.WriteUint64(static_cast<uint64_t>(screenId));
    if (surface != nullptr && surface->GetProducer() != nullptr) {
        res = res &&
            data.WriteBool(true) &&
            data.WriteRemoteObject(surface->GetProducer()->AsObject());
    } else {
        WLOGFW("SetVirtualScreenSurface: surface is nullptr");
        res = res && data.WriteBool(false);
    }
    if (!res) {
        WLOGFW("SetVirtualScreenSurface: Write screenId/surface failed");
        return DMError::DM_ERROR_IPC_FAILED;
    }
    if (remote->SendRequest(static_cast<uint32_t>(DisplayManagerMessage::TRANS_ID_SET_VIRTUAL_SCREEN_SURFACE),
        data, reply, option) != ERR_NONE) {
        WLOGFW("SetVirtualScreenSurface: SendRequest failed");
        return DMError::DM_ERROR_IPC_FAILED;
    }
    return static_cast<DMError>(reply.ReadInt32());
}

bool DisplayManagerProxy::SetOrientation(ScreenId screenId, Orientation orientation)
{
    sptr<IRemoteObject> remote = Remote();
    if (remote == nullptr) {
        WLOGFW("fail to set orientation: remote is null");
        return false;
    }

    MessageParcel data;
    MessageParcel reply;
    MessageOption option;
    if (!data.WriteInterfaceToken(GetDescriptor())) {
        WLOGFE("fail to set orientation: WriteInterfaceToken failed");
        return false;
    }
    if (!data.WriteUint64(static_cast<uint64_t>(screenId))) {
        WLOGFW("fail to set orientation: Write screenId failed");
        return false;
    }
    if (!data.WriteUint32(static_cast<uint32_t>(orientation))) {
        WLOGFW("fail to set orientation: Write orientation failed");
        return false;
    }
    if (remote->SendRequest(static_cast<uint32_t>(DisplayManagerMessage::TRANS_ID_SET_ORIENTATION),
        data, reply, option) != ERR_NONE) {
        WLOGFW("fail to set orientation: SendRequest failed");
        return false;
    }
    return reply.ReadBool();
}

std::shared_ptr<Media::PixelMap> DisplayManagerProxy::GetDisplaySnapshot(DisplayId displayId)
{
    sptr<IRemoteObject> remote = Remote();
    if (remote == nullptr) {
        WLOGFW("GetDisplaySnapshot: remote is nullptr");
        return nullptr;
    }

    MessageParcel data;
    MessageParcel reply;
    MessageOption option;
    if (!data.WriteInterfaceToken(GetDescriptor())) {
        WLOGFE("GetDisplaySnapshot: WriteInterfaceToken failed");
        return nullptr;
    }

    if (!data.WriteUint64(displayId)) {
        WLOGFE("Write displayId failed");
        return nullptr;
    }

    if (remote->SendRequest(static_cast<uint32_t>(DisplayManagerMessage::TRANS_ID_GET_DISPLAY_SNAPSHOT),
        data, reply, option) != ERR_NONE) {
        WLOGFW("GetDisplaySnapshot: SendRequest failed");
        return nullptr;
    }

    std::shared_ptr<Media::PixelMap> pixelMap(reply.ReadParcelable<Media::PixelMap>());
    if (pixelMap == nullptr) {
        WLOGFW("DisplayManagerProxy::GetDisplaySnapshot SendRequest nullptr.");
        return nullptr;
    }
    return pixelMap;
}

DMError DisplayManagerProxy::GetScreenSupportedColorGamuts(ScreenId screenId,
    std::vector<ScreenColorGamut>& colorGamuts)
{
    sptr<IRemoteObject> remote = Remote();
    if (remote == nullptr) {
        WLOGFW("DisplayManagerProxy::GetScreenSupportedColorGamuts: remote is nullptr");
        return DMError::DM_ERROR_NULLPTR;
    }

    MessageParcel data;
    MessageParcel reply;
    MessageOption option;
    if (!data.WriteInterfaceToken(GetDescriptor())) {
        WLOGFW("DisplayManagerProxy::GetScreenSupportedColorGamuts: WriteInterfaceToken failed");
        return DMError::DM_ERROR_WRITE_INTERFACE_TOKEN_FAILED;
    }
    if (!data.WriteUint64(static_cast<uint64_t>(screenId))) {
        WLOGFW("DisplayManagerProxy::GetScreenSupportedColorGamuts: WriteUint64 screenId failed");
        return DMError::DM_ERROR_IPC_FAILED;
    }
    if (remote->SendRequest(static_cast<uint32_t>(DisplayManagerMessage::TRANS_ID_SCREEN_GET_SUPPORTED_COLOR_GAMUTS),
        data, reply, option) != ERR_NONE) {
        WLOGFW("DisplayManagerProxy::GetScreenSupportedColorGamuts: SendRequest failed");
        return DMError::DM_ERROR_IPC_FAILED;
    }
    DMError ret = static_cast<DMError>(reply.ReadInt32());
    if (ret != DMError::DM_OK) {
        return ret;
    }
    MarshallingHelper::UnmarshallingVectorObj<ScreenColorGamut>(reply, colorGamuts,
        [](Parcel& parcel, ScreenColorGamut& color) {
            uint32_t value;
            bool res = parcel.ReadUint32(value);
            color = static_cast<ScreenColorGamut>(value);
            return res;
        }
    );
    return ret;
}

DMError DisplayManagerProxy::GetScreenColorGamut(ScreenId screenId, ScreenColorGamut& colorGamut)
{
    sptr<IRemoteObject> remote = Remote();
    if (remote == nullptr) {
        WLOGFW("DisplayManagerProxy::GetScreenColorGamut: remote is nullptr");
        return DMError::DM_ERROR_NULLPTR;
    }

    MessageParcel data;
    MessageParcel reply;
    MessageOption option;
    if (!data.WriteInterfaceToken(GetDescriptor())) {
        WLOGFW("DisplayManagerProxy::GetScreenColorGamut: WriteInterfaceToken failed");
        return DMError::DM_ERROR_WRITE_INTERFACE_TOKEN_FAILED;
    }
    if (!data.WriteUint64(static_cast<uint64_t>(screenId))) {
        WLOGFW("DisplayManagerProxy::GetScreenColorGamut: WriteUint64 uint64_t failed");
        return DMError::DM_ERROR_IPC_FAILED;
    }
    if (remote->SendRequest(static_cast<uint32_t>(DisplayManagerMessage::TRANS_ID_SCREEN_GET_COLOR_GAMUT),
        data, reply, option) != ERR_NONE) {
        WLOGFW("DisplayManagerProxy::GetScreenColorGamut: SendRequest failed");
        return DMError::DM_ERROR_IPC_FAILED;
    }
    DMError ret = static_cast<DMError>(reply.ReadInt32());
    if (ret != DMError::DM_OK) {
        return ret;
    }
    colorGamut = static_cast<ScreenColorGamut>(reply.ReadUint32());
    return ret;
}

DMError DisplayManagerProxy::SetScreenColorGamut(ScreenId screenId, int32_t colorGamutIdx)
{
    sptr<IRemoteObject> remote = Remote();
    if (remote == nullptr) {
        WLOGFW("DisplayManagerProxy::SetScreenColorGamut: remote is nullptr");
        return DMError::DM_ERROR_NULLPTR;
    }

    MessageParcel data;
    MessageParcel reply;
    MessageOption option;
    if (!data.WriteInterfaceToken(GetDescriptor())) {
        WLOGFW("DisplayManagerProxy::SetScreenColorGamut: WriteInterfaceToken failed");
        return DMError::DM_ERROR_WRITE_INTERFACE_TOKEN_FAILED;
    }
    if (!data.WriteUint64(static_cast<uint64_t>(screenId)) || !data.WriteInt32(colorGamutIdx)) {
        WLOGFW("DisplayManagerProxy::SetScreenColorGamut: Write failed");
        return DMError::DM_ERROR_IPC_FAILED;
    }
    if (remote->SendRequest(static_cast<uint32_t>(DisplayManagerMessage::TRANS_ID_SCREEN_SET_COLOR_GAMUT),
        data, reply, option) != ERR_NONE) {
        WLOGFW("DisplayManagerProxy::SetScreenColorGamut: SendRequest failed");
        return DMError::DM_ERROR_IPC_FAILED;
    }
    return static_cast<DMError>(reply.ReadInt32());
}

DMError DisplayManagerProxy::GetScreenGamutMap(ScreenId screenId, ScreenGamutMap& gamutMap)
{
    sptr<IRemoteObject> remote = Remote();
    if (remote == nullptr) {
        WLOGFW("DisplayManagerProxy::GetScreenGamutMap: remote is nullptr");
        return DMError::DM_ERROR_NULLPTR;
    }

    MessageParcel data;
    MessageParcel reply;
    MessageOption option;
    if (!data.WriteInterfaceToken(GetDescriptor())) {
        WLOGFW("DisplayManagerProxy::GetScreenGamutMap: WriteInterfaceToken failed");
        return DMError::DM_ERROR_WRITE_INTERFACE_TOKEN_FAILED;
    }
    if (!data.WriteUint64(static_cast<uint64_t>(screenId))) {
        WLOGFW("DisplayManagerProxy::GetScreenGamutMap: WriteUint64 screenId failed");
        return DMError::DM_ERROR_IPC_FAILED;
    }
    if (remote->SendRequest(static_cast<uint32_t>(DisplayManagerMessage::TRANS_ID_SCREEN_GET_GAMUT_MAP),
        data, reply, option) != ERR_NONE) {
        WLOGFW("DisplayManagerProxy::GetScreenGamutMap: SendRequest failed");
        return DMError::DM_ERROR_IPC_FAILED;
    }
    DMError ret = static_cast<DMError>(reply.ReadInt32());
    if (ret != DMError::DM_OK) {
        return ret;
    }
    gamutMap = static_cast<ScreenGamutMap>(reply.ReadUint32());
    return ret;
}

DMError DisplayManagerProxy::SetScreenGamutMap(ScreenId screenId, ScreenGamutMap gamutMap)
{
    sptr<IRemoteObject> remote = Remote();
    if (remote == nullptr) {
        WLOGFW("DisplayManagerProxy::SetScreenGamutMap: remote is nullptr");
        return DMError::DM_ERROR_NULLPTR;
    }

    MessageParcel data;
    MessageParcel reply;
    MessageOption option;
    if (!data.WriteInterfaceToken(GetDescriptor())) {
        WLOGFW("DisplayManagerProxy::SetScreenGamutMap: WriteInterfaceToken failed");
        return DMError::DM_ERROR_WRITE_INTERFACE_TOKEN_FAILED;
    }
    if (!data.WriteUint64(static_cast<uint64_t>(screenId)) || !data.WriteUint32(static_cast<uint32_t>(gamutMap))) {
        WLOGFW("DisplayManagerProxy::SetScreenGamutMap: Writ failed");
        return DMError::DM_ERROR_IPC_FAILED;
    }
    if (remote->SendRequest(static_cast<uint32_t>(DisplayManagerMessage::TRANS_ID_SCREEN_SET_GAMUT_MAP),
        data, reply, option) != ERR_NONE) {
        WLOGFW("DisplayManagerProxy::SetScreenGamutMap: SendRequest failed");
        return DMError::DM_ERROR_IPC_FAILED;
    }
    return static_cast<DMError>(reply.ReadInt32());
}

DMError DisplayManagerProxy::SetScreenColorTransform(ScreenId screenId)
{
    sptr<IRemoteObject> remote = Remote();
    if (remote == nullptr) {
        WLOGFW("DisplayManagerProxy::SetScreenColorTransform: remote is nullptr");
        return DMError::DM_ERROR_NULLPTR;
    }

    MessageParcel data;
    MessageParcel reply;
    MessageOption option;
    if (!data.WriteInterfaceToken(GetDescriptor())) {
        WLOGFW("DisplayManagerProxy::SetScreenColorTransform: WriteInterfaceToken failed");
        return DMError::DM_ERROR_WRITE_INTERFACE_TOKEN_FAILED;
    }
    if (!data.WriteUint64(static_cast<uint64_t>(screenId))) {
        WLOGFW("DisplayManagerProxy::SetScreenColorTransform: WriteUint64 screenId failed");
        return DMError::DM_ERROR_IPC_FAILED;
    }
    if (remote->SendRequest(static_cast<uint32_t>(DisplayManagerMessage::TRANS_ID_SCREEN_SET_COLOR_TRANSFORM),
        data, reply, option) != ERR_NONE) {
        WLOGFW("DisplayManagerProxy::SetScreenColorTransform: SendRequest failed");
        return DMError::DM_ERROR_IPC_FAILED;
    }
    return static_cast<DMError>(reply.ReadInt32());
}

bool DisplayManagerProxy::RegisterDisplayManagerAgent(const sptr<IDisplayManagerAgent>& displayManagerAgent,
    DisplayManagerAgentType type)
{
    MessageParcel data;
    MessageParcel reply;
    MessageOption option;
    if (!data.WriteInterfaceToken(GetDescriptor())) {
        WLOGFE("WriteInterfaceToken failed");
        return false;
    }

    if (!data.WriteRemoteObject(displayManagerAgent->AsObject())) {
        WLOGFE("Write IDisplayManagerAgent failed");
        return false;
    }

    if (!data.WriteUint32(static_cast<uint32_t>(type))) {
        WLOGFE("Write DisplayManagerAgent type failed");
        return false;
    }

    if (Remote()->SendRequest(static_cast<uint32_t>(DisplayManagerMessage::TRANS_ID_REGISTER_DISPLAY_MANAGER_AGENT),
        data, reply, option) != ERR_NONE) {
        WLOGFE("SendRequest failed");
        return false;
    }
    return reply.ReadBool();
}

bool DisplayManagerProxy::UnregisterDisplayManagerAgent(const sptr<IDisplayManagerAgent>& displayManagerAgent,
    DisplayManagerAgentType type)
{
    MessageParcel data;
    MessageParcel reply;
    MessageOption option;
    if (!data.WriteInterfaceToken(GetDescriptor())) {
        WLOGFE("WriteInterfaceToken failed");
        return false;
    }

    if (!data.WriteRemoteObject(displayManagerAgent->AsObject())) {
        WLOGFE("Write IWindowManagerAgent failed");
        return false;
    }

    if (!data.WriteUint32(static_cast<uint32_t>(type))) {
        WLOGFE("Write DisplayManagerAgent type failed");
        return false;
    }

    if (Remote()->SendRequest(static_cast<uint32_t>(DisplayManagerMessage::TRANS_ID_UNREGISTER_DISPLAY_MANAGER_AGENT),
        data, reply, option) != ERR_NONE) {
        WLOGFE("SendRequest failed");
        return false;
    }
    return reply.ReadBool();
}

bool DisplayManagerProxy::WakeUpBegin(PowerStateChangeReason reason)
{
    MessageParcel data;
    MessageParcel reply;
    MessageOption option;
    if (!data.WriteInterfaceToken(GetDescriptor())) {
        WLOGFE("WriteInterfaceToken failed");
        return false;
    }
    if (!data.WriteUint32(static_cast<uint32_t>(reason))) {
        WLOGFE("Write PowerStateChangeReason failed");
        return false;
    }
    if (Remote()->SendRequest(static_cast<uint32_t>(DisplayManagerMessage::TRANS_ID_WAKE_UP_BEGIN),
        data, reply, option) != ERR_NONE) {
        WLOGFW("SendRequest failed");
        return false;
    }
    return reply.ReadBool();
}

bool DisplayManagerProxy::WakeUpEnd()
{
    MessageParcel data;
    MessageParcel reply;
    MessageOption option;
    if (!data.WriteInterfaceToken(GetDescriptor())) {
        WLOGFE("WriteInterfaceToken failed");
        return false;
    }
    if (Remote()->SendRequest(static_cast<uint32_t>(DisplayManagerMessage::TRANS_ID_WAKE_UP_END),
        data, reply, option) != ERR_NONE) {
        WLOGFW("SendRequest failed");
        return false;
    }
    return reply.ReadBool();
}

bool DisplayManagerProxy::SuspendBegin(PowerStateChangeReason reason)
{
    MessageParcel data;
    MessageParcel reply;
    MessageOption option;
    if (!data.WriteInterfaceToken(GetDescriptor())) {
        WLOGFE("WriteInterfaceToken failed");
        return false;
    }
    if (!data.WriteUint32(static_cast<uint32_t>(reason))) {
        WLOGFE("Write PowerStateChangeReason failed");
        return false;
    }
    if (Remote()->SendRequest(static_cast<uint32_t>(DisplayManagerMessage::TRANS_ID_SUSPEND_BEGIN),
        data, reply, option) != ERR_NONE) {
        WLOGFW("SendRequest failed");
        return false;
    }
    return reply.ReadBool();
}

bool DisplayManagerProxy::SuspendEnd()
{
    MessageParcel data;
    MessageParcel reply;
    MessageOption option;
    if (!data.WriteInterfaceToken(GetDescriptor())) {
        WLOGFE("WriteInterfaceToken failed");
        return false;
    }
    if (Remote()->SendRequest(static_cast<uint32_t>(DisplayManagerMessage::TRANS_ID_SUSPEND_END),
        data, reply, option) != ERR_NONE) {
        WLOGFW("SendRequest failed");
        return false;
    }
    return reply.ReadBool();
}

bool DisplayManagerProxy::SetScreenPowerForAll(ScreenPowerState state, PowerStateChangeReason reason)
{
    MessageParcel data;
    MessageParcel reply;
    MessageOption option;
    if (!data.WriteInterfaceToken(GetDescriptor())) {
        WLOGFE("WriteInterfaceToken failed");
        return false;
    }
    if (!data.WriteUint32(static_cast<uint32_t>(state))) {
        WLOGFE("Write ScreenPowerState failed");
        return false;
    }
    if (!data.WriteUint32(static_cast<uint32_t>(reason))) {
        WLOGFE("Write PowerStateChangeReason failed");
        return false;
    }
    if (Remote()->SendRequest(static_cast<uint32_t>(DisplayManagerMessage::TRANS_ID_SET_SCREEN_POWER_FOR_ALL),
        data, reply, option) != ERR_NONE) {
        WLOGFW("SendRequest failed");
        return false;
    }
    return reply.ReadBool();
}

ScreenPowerState DisplayManagerProxy::GetScreenPower(ScreenId dmsScreenId)
{
    MessageParcel data;
    MessageParcel reply;
    MessageOption option;
    if (!data.WriteInterfaceToken(GetDescriptor())) {
        WLOGFE("WriteInterfaceToken failed");
        return ScreenPowerState::INVALID_STATE;
    }
    if (!data.WriteUint64(static_cast<uint64_t>(dmsScreenId))) {
        WLOGFE("Write dmsScreenId failed");
        return ScreenPowerState::INVALID_STATE;
    }
    if (Remote()->SendRequest(static_cast<uint32_t>(DisplayManagerMessage::TRANS_ID_GET_SCREEN_POWER),
        data, reply, option) != ERR_NONE) {
        WLOGFW("SendRequest failed");
        return ScreenPowerState::INVALID_STATE;
    }
    return static_cast<ScreenPowerState>(reply.ReadUint32());
}

bool DisplayManagerProxy::SetDisplayState(DisplayState state)
{
    MessageParcel data;
    MessageParcel reply;
    MessageOption option;
    if (!data.WriteInterfaceToken(GetDescriptor())) {
        WLOGFE("WriteInterfaceToken failed");
        return false;
    }
    if (!data.WriteUint32(static_cast<uint32_t>(state))) {
        WLOGFE("Write DisplayState failed");
        return false;
    }
    if (Remote()->SendRequest(static_cast<uint32_t>(DisplayManagerMessage::TRANS_ID_SET_DISPLAY_STATE),
        data, reply, option) != ERR_NONE) {
        WLOGFW("SendRequest failed");
        return false;
    }
    return reply.ReadBool();
}

DisplayState DisplayManagerProxy::GetDisplayState(DisplayId displayId)
{
    MessageParcel data;
    MessageParcel reply;
    MessageOption option;
    if (!data.WriteInterfaceToken(GetDescriptor())) {
        WLOGFE("WriteInterfaceToken failed");
        return DisplayState::UNKNOWN;
    }
    if (!data.WriteUint64(displayId)) {
        WLOGFE("Write displayId failed");
        return DisplayState::UNKNOWN;
    }
    if (Remote()->SendRequest(static_cast<uint32_t>(DisplayManagerMessage::TRANS_ID_GET_DISPLAY_STATE),
        data, reply, option) != ERR_NONE) {
        WLOGFW("SendRequest failed");
        return DisplayState::UNKNOWN;
    }
    return static_cast<DisplayState>(reply.ReadUint32());
}

std::vector<DisplayId> DisplayManagerProxy::GetAllDisplayIds()
{
    std::vector<DisplayId> allDisplayIds;
    MessageParcel data;
    MessageParcel reply;
    MessageOption option;
    if (!data.WriteInterfaceToken(GetDescriptor())) {
        WLOGFE("WriteInterfaceToken failed");
        return allDisplayIds;
    }
    if (Remote()->SendRequest(static_cast<uint32_t>(DisplayManagerMessage::TRANS_ID_GET_ALL_DISPLAYIDS),
        data, reply, option) != ERR_NONE) {
        WLOGFW("SendRequest failed");
        return allDisplayIds;
    }
    reply.ReadUInt64Vector(&allDisplayIds);
    return allDisplayIds;
}

sptr<CutoutInfo> DisplayManagerProxy::GetCutoutInfo(DisplayId displayId)
{
    sptr<IRemoteObject> remote = Remote();
    if (remote == nullptr) {
        WLOGFW("GetCutoutInfo: remote is null");
        return nullptr;
    }
    MessageParcel data;
    MessageParcel reply;
    MessageOption option;
    if (!data.WriteInterfaceToken(GetDescriptor())) {
        WLOGFE("GetCutoutInfo: GetCutoutInfo failed");
        return nullptr;
    }
    if (!data.WriteUint64(displayId)) {
        WLOGFE("GetCutoutInfo: write displayId failed");
        return nullptr;
    }
    if (remote->SendRequest(static_cast<uint32_t>(DisplayManagerMessage::TRANS_ID_GET_CUTOUT_INFO),
        data, reply, option) != ERR_NONE) {
        WLOGFW("GetCutoutInfo: GetCutoutInfo failed");
        return nullptr;
    }
    sptr<CutoutInfo> info = reply.ReadParcelable<CutoutInfo>();
    return info;
}

DMError DisplayManagerProxy::HasPrivateWindow(DisplayId displayId, bool& hasPrivateWindow)
{
    MessageParcel data;
    MessageParcel reply;
    MessageOption option;
    if (!data.WriteInterfaceToken(GetDescriptor())) {
        return DMError::DM_ERROR_IPC_FAILED;
    }

    if (!data.WriteUint64(displayId)) {
        return DMError::DM_ERROR_IPC_FAILED;
    }

    if (Remote()->SendRequest(static_cast<uint32_t>(DisplayManagerMessage::TRANS_ID_HAS_PRIVATE_WINDOW),
        data, reply, option) != ERR_NONE) {
        return DMError::DM_ERROR_IPC_FAILED;
    }
    DMError ret = static_cast<DMError>(reply.ReadInt32());
    hasPrivateWindow = reply.ReadBool();
    return ret;
}

void DisplayManagerProxy::NotifyDisplayEvent(DisplayEvent event)
{
    MessageParcel data;
    MessageParcel reply;
    MessageOption option;
    if (!data.WriteInterfaceToken(GetDescriptor())) {
        WLOGFE("WriteInterfaceToken failed");
        return;
    }
    if (!data.WriteUint32(static_cast<uint32_t>(event))) {
        WLOGFE("Write DisplayEvent failed");
        return;
    }
    if (Remote()->SendRequest(static_cast<uint32_t>(DisplayManagerMessage::TRANS_ID_NOTIFY_DISPLAY_EVENT),
        data, reply, option) != ERR_NONE) {
        WLOGFW("SendRequest failed");
        return;
    }
}

bool DisplayManagerProxy::SetFreeze(std::vector<DisplayId> displayIds, bool isFreeze)
{
    MessageParcel data;
    MessageParcel reply;
    MessageOption option;
    if (!data.WriteInterfaceToken(GetDescriptor())) {
        WLOGFE("WriteInterfaceToken failed");
        return false;
    }
    if (!data.WriteUInt64Vector(displayIds)) {
        WLOGFE("set freeze fail: write displayId failed.");
        return false;
    }
    if (!data.WriteBool(isFreeze)) {
        WLOGFE("set freeze fail: write freeze flag failed.");
        return false;
    }

    if (Remote()->SendRequest(static_cast<uint32_t>(DisplayManagerMessage::TRANS_ID_SET_FREEZE_EVENT),
        data, reply, option) != ERR_NONE) {
        WLOGFE("SendRequest failed");
        return false;
    }
    return true;
}

ScreenId DisplayManagerProxy::MakeMirror(ScreenId mainScreenId, std::vector<ScreenId> mirrorScreenId)
{
    sptr<IRemoteObject> remote = Remote();
    if (remote == nullptr) {
        WLOGFW("create mirror fail: remote is null");
        return SCREEN_ID_INVALID;
    }

    MessageParcel data;
    MessageParcel reply;
    MessageOption option;
    if (!data.WriteInterfaceToken(GetDescriptor())) {
        WLOGFE("create mirror fail: WriteInterfaceToken failed");
        return SCREEN_ID_INVALID;
    }
    bool res = data.WriteUint64(static_cast<uint64_t>(mainScreenId)) &&
        data.WriteUInt64Vector(mirrorScreenId);
    if (!res) {
        WLOGFE("create mirror fail: data write failed");
        return SCREEN_ID_INVALID;
    }
    if (remote->SendRequest(static_cast<uint32_t>(DisplayManagerMessage::TRANS_ID_SCREEN_MAKE_MIRROR),
        data, reply, option) != ERR_NONE) {
        WLOGFW("create mirror fail: SendRequest failed");
        return SCREEN_ID_INVALID;
    }
    return static_cast<ScreenId>(reply.ReadUint64());
}

sptr<ScreenInfo> DisplayManagerProxy::GetScreenInfoById(ScreenId screenId)
{
    sptr<IRemoteObject> remote = Remote();
    if (remote == nullptr) {
        WLOGFW("GetScreenInfoById: remote is nullptr");
        return nullptr;
    }

    MessageParcel data;
    MessageParcel reply;
    MessageOption option;
    if (!data.WriteInterfaceToken(GetDescriptor())) {
        WLOGFE("GetScreenInfoById: WriteInterfaceToken failed");
        return nullptr;
    }
    if (!data.WriteUint64(screenId)) {
        WLOGFE("GetScreenInfoById: Write screenId failed");
        return nullptr;
    }
    if (remote->SendRequest(static_cast<uint32_t>(DisplayManagerMessage::TRANS_ID_GET_SCREEN_INFO_BY_ID),
        data, reply, option) != ERR_NONE) {
        WLOGFW("GetScreenInfoById: SendRequest failed");
        return nullptr;
    }

    sptr<ScreenInfo> info = reply.ReadStrongParcelable<ScreenInfo>();
    if (info == nullptr) {
        WLOGFW("GetScreenInfoById SendRequest nullptr.");
        return nullptr;
    }
    for (auto& mode : info->GetModes()) {
        WLOGFI("info modes is width: %{public}u, height: %{public}u, refreshRate: %{public}u",
            mode->width_, mode->height_, mode->refreshRate_);
    }
    return info;
}

sptr<ScreenGroupInfo> DisplayManagerProxy::GetScreenGroupInfoById(ScreenId screenId)
{
    sptr<IRemoteObject> remote = Remote();
    if (remote == nullptr) {
        WLOGFW("GetScreenGroupInfoById: remote is nullptr");
        return nullptr;
    }

    MessageParcel data;
    MessageParcel reply;
    MessageOption option;
    if (!data.WriteInterfaceToken(GetDescriptor())) {
        WLOGFE("GetScreenGroupInfoById: WriteInterfaceToken failed");
        return nullptr;
    }
    if (!data.WriteUint64(screenId)) {
        WLOGFE("GetScreenGroupInfoById: Write screenId failed");
        return nullptr;
    }
    if (remote->SendRequest(static_cast<uint32_t>(DisplayManagerMessage::TRANS_ID_GET_SCREEN_GROUP_INFO_BY_ID),
        data, reply, option) != ERR_NONE) {
        WLOGFW("GetScreenGroupInfoById: SendRequest failed");
        return nullptr;
    }

    sptr<ScreenGroupInfo> info = reply.ReadStrongParcelable<ScreenGroupInfo>();
    if (info == nullptr) {
        WLOGFW("GetScreenGroupInfoById SendRequest nullptr.");
        return nullptr;
    }
    return info;
}

std::vector<sptr<ScreenInfo>> DisplayManagerProxy::GetAllScreenInfos()
{
    std::vector<sptr<ScreenInfo>> screenInfos;
    sptr<IRemoteObject> remote = Remote();
    if (remote == nullptr) {
        WLOGFW("GetAllScreenInfos: remote is nullptr");
        return screenInfos;
    }

    MessageParcel data;
    MessageParcel reply;
    MessageOption option;
    if (!data.WriteInterfaceToken(GetDescriptor())) {
        WLOGFE("GetAllScreenInfos: WriteInterfaceToken failed");
        return screenInfos;
    }
    if (remote->SendRequest(static_cast<uint32_t>(DisplayManagerMessage::TRANS_ID_GET_ALL_SCREEN_INFOS),
        data, reply, option) != ERR_NONE) {
        WLOGFW("GetAllScreenInfos: SendRequest failed");
        return screenInfos;
    }

    MarshallingHelper::UnmarshallingVectorParcelableObj<ScreenInfo>(reply, screenInfos);
    return screenInfos;
}

ScreenId DisplayManagerProxy::MakeExpand(std::vector<ScreenId> screenId, std::vector<Point> startPoint)
{
    sptr<IRemoteObject> remote = Remote();
    if (remote == nullptr) {
        WLOGFW("MakeExpand: remote is null");
        return SCREEN_ID_INVALID;
    }

    MessageParcel data;
    MessageParcel reply;
    MessageOption option;
    if (!data.WriteInterfaceToken(GetDescriptor())) {
        WLOGFE("MakeExpand: WriteInterfaceToken failed");
        return SCREEN_ID_INVALID;
    }
    if (!data.WriteUInt64Vector(screenId)) {
        WLOGFE("MakeExpand: write screenId failed");
        return SCREEN_ID_INVALID;
    }
    if (!MarshallingHelper::MarshallingVectorObj<Point>(data, startPoint, [](Parcel& parcel, const Point& point) {
            return parcel.WriteInt32(point.posX_) && parcel.WriteInt32(point.posY_);
        })) {
        WLOGFE("MakeExpand: write startPoint failed");
        return SCREEN_ID_INVALID;
    }
    if (remote->SendRequest(static_cast<uint32_t>(DisplayManagerMessage::TRANS_ID_SCREEN_MAKE_EXPAND),
        data, reply, option) != ERR_NONE) {
        WLOGFE("MakeExpand: SendRequest failed");
        return SCREEN_ID_INVALID;
    }
    return static_cast<ScreenId>(reply.ReadUint64());
}

void DisplayManagerProxy::RemoveVirtualScreenFromGroup(std::vector<ScreenId> screens)
{
    sptr<IRemoteObject> remote = Remote();
    if (remote == nullptr) {
        WLOGFW("cancel make mirror or expand fail: remote is null");
        return;
    }

    MessageParcel data;
    MessageParcel reply;
    MessageOption option(MessageOption::TF_ASYNC);
    if (!data.WriteInterfaceToken(GetDescriptor())) {
        WLOGFE("cancel make mirror or expand fail: WriteInterfaceToken failed");
        return;
    }
    bool res = data.WriteUInt64Vector(screens);
    if (!res) {
        WLOGFE("cancel make mirror or expand fail: write screens failed.");
        return;
    }
    if (remote->SendRequest(static_cast<uint32_t>(
        DisplayManagerMessage::TRANS_ID_REMOVE_VIRTUAL_SCREEN_FROM_SCREEN_GROUP),
        data, reply, option) != ERR_NONE) {
        WLOGFW("cancel make mirror or expand fail: SendRequest failed");
    }
}

bool DisplayManagerProxy::SetScreenActiveMode(ScreenId screenId, uint32_t modeId)
{
    sptr<IRemoteObject> remote = Remote();
    if (remote == nullptr) {
        WLOGFW("SetScreenActiveMode: remote is null");
        return false;
    }

    MessageParcel data;
    MessageParcel reply;
    MessageOption option;
    if (!data.WriteInterfaceToken(GetDescriptor())) {
        WLOGFE("SetScreenActiveMode: WriteInterfaceToken failed");
        return false;
    }
    if (!data.WriteUint64(screenId) || !data.WriteUint32(modeId)) {
        WLOGFE("SetScreenActiveMode: write screenId/modeId failed");
        return false;
    }
    if (remote->SendRequest(static_cast<uint32_t>(DisplayManagerMessage::TRANS_ID_SET_SCREEN_ACTIVE_MODE),
        data, reply, option) != ERR_NONE) {
        WLOGFE("SetScreenActiveMode: SendRequest failed");
        return false;
    }
    return reply.ReadBool();
}

bool DisplayManagerProxy::SetVirtualPixelRatio(ScreenId screenId, float virtualPixelRatio)
{
    sptr<IRemoteObject> remote = Remote();
    if (remote == nullptr) {
        WLOGFW("SetVirtualPixelRatio: remote is null");
        return false;
    }

    MessageParcel data;
    MessageParcel reply;
    MessageOption option;
    if (!data.WriteInterfaceToken(GetDescriptor())) {
        WLOGFE("SetVirtualPixelRatio: WriteInterfaceToken failed");
        return false;
    }
    if (!data.WriteUint64(screenId) || !data.WriteFloat(virtualPixelRatio)) {
        WLOGFE("SetVirtualPixelRatio: write screenId/modeId failed");
        return false;
    }
    if (remote->SendRequest(static_cast<uint32_t>(DisplayManagerMessage::TRANS_ID_SET_VIRTUAL_PIXEL_RATIO),
        data, reply, option) != ERR_NONE) {
        WLOGFE("SetVirtualPixelRatio: SendRequest failed");
        return false;
    }
    return reply.ReadBool();
}

bool DisplayManagerProxy::IsScreenRotationLocked()
{
    sptr<IRemoteObject> remote = Remote();
    if (remote == nullptr) {
        WLOGFW("IsScreenRotationLocked: remote is nullptr");
        return false;
    }
    MessageParcel data;
    MessageParcel reply;
    MessageOption option;
    if (!data.WriteInterfaceToken(GetDescriptor())) {
        WLOGFE("IsScreenRotationLocked: WriteInterfaceToken failed");
        return false;
    }
    if (remote->SendRequest(static_cast<uint32_t>(DisplayManagerMessage::TRANS_ID_IS_SCREEN_ROTATION_LOCKED),
        data, reply, option) != ERR_NONE) {
        WLOGFW("IsScreenRotationLocked: SendRequest failed");
        return false;
    }
    bool isLocked = reply.ReadBool();
    return isLocked;
}

void DisplayManagerProxy::SetScreenRotationLocked(bool isLocked)
{
    sptr<IRemoteObject> remote = Remote();
    if (remote == nullptr) {
        WLOGFW("SetScreenRotationLocked: remote is null");
        return;
    }

    MessageParcel data;
    MessageParcel reply;
    MessageOption option;
    if (!data.WriteInterfaceToken(GetDescriptor())) {
        WLOGFE("SetScreenRotationLocked: WriteInterfaceToken failed");
        return;
    }
    if (!data.WriteBool(isLocked)) {
        WLOGFE("SetScreenRotationLocked: write isLocked failed");
        return;
    }
    if (remote->SendRequest(static_cast<uint32_t>(DisplayManagerMessage::TRANS_ID_SET_SCREEN_ROTATION_LOCKED),
        data, reply, option) != ERR_NONE) {
        WLOGFE("SetScreenRotationLocked: SendRequest failed");
        return;
    }
}
} // namespace OHOS::Rosen