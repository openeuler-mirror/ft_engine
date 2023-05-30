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

#include "zidl/window_manager_stub.h"
#include <ipc_skeleton.h>
#include <rs_iwindow_animation_controller.h>

#include "marshalling_helper.h"
#include "memory_guard.h"
#include "window_manager_hilog.h"

namespace OHOS {
namespace Rosen {
namespace {
    constexpr HiviewDFX::HiLogLabel LABEL = {LOG_CORE, HILOG_DOMAIN_WINDOW, "WindowManagerStub"};
}

int32_t WindowManagerStub::OnRemoteRequest(uint32_t code, MessageParcel &data, MessageParcel &reply,
    MessageOption &option)
{
    MemoryGuard cacheGuard;
    if (data.ReadInterfaceToken() != GetDescriptor()) {
        WLOGFE("InterfaceToken check failed");
        return -1;
    }
    auto msgId = static_cast<WindowManagerMessage>(code);
    switch (msgId) {
        case WindowManagerMessage::TRANS_ID_CREATE_WINDOW: {
            sptr<IRemoteObject> windowObject = data.ReadRemoteObject();
            sptr<IWindow> windowProxy = iface_cast<IWindow>(windowObject);
            sptr<WindowProperty> windowProperty = data.ReadStrongParcelable<WindowProperty>();
            std::shared_ptr<RSSurfaceNode> surfaceNode = RSSurfaceNode::Unmarshalling(data);
            uint32_t windowId;
            sptr<IRemoteObject> token = nullptr;
            if (windowProperty && windowProperty->GetTokenState()) {
                token = data.ReadRemoteObject();
            } else {
                WLOGFI("accept token is nullptr");
            }
            WMError errCode = CreateWindow(windowProxy, windowProperty, surfaceNode, windowId, token);
            reply.WriteUint32(windowId);
            reply.WriteInt32(static_cast<int32_t>(errCode));
            break;
        }
        case WindowManagerMessage::TRANS_ID_ADD_WINDOW: {
            sptr<WindowProperty> windowProperty = data.ReadStrongParcelable<WindowProperty>();
            WMError errCode = AddWindow(windowProperty);
            reply.WriteInt32(static_cast<int32_t>(errCode));
            break;
        }
        case WindowManagerMessage::TRANS_ID_REMOVE_WINDOW: {
            uint32_t windowId = data.ReadUint32();
            WMError errCode = RemoveWindow(windowId);
            reply.WriteInt32(static_cast<int32_t>(errCode));
            break;
        }
        case WindowManagerMessage::TRANS_ID_DESTROY_WINDOW: {
            uint32_t windowId = data.ReadUint32();
            WMError errCode = DestroyWindow(windowId);
            reply.WriteInt32(static_cast<int32_t>(errCode));
            break;
        }
        case WindowManagerMessage::TRANS_ID_REQUEST_FOCUS: {
            uint32_t windowId = data.ReadUint32();
            WMError errCode = RequestFocus(windowId);
            reply.WriteInt32(static_cast<int32_t>(errCode));
            break;
        }
        case WindowManagerMessage::TRANS_ID_GET_AVOID_AREA: {
            uint32_t windowId = data.ReadUint32();
            auto avoidAreaType = static_cast<AvoidAreaType>(data.ReadUint32());
            AvoidArea avoidArea = GetAvoidAreaByType(windowId, avoidAreaType);
            reply.WriteParcelable(&avoidArea);

            break;
        }
        case WindowManagerMessage::TRANS_ID_REGISTER_WINDOW_MANAGER_AGENT: {
            auto type = static_cast<WindowManagerAgentType>(data.ReadUint32());
            sptr<IRemoteObject> windowManagerAgentObject = data.ReadRemoteObject();
            sptr<IWindowManagerAgent> windowManagerAgentProxy =
                iface_cast<IWindowManagerAgent>(windowManagerAgentObject);
            bool ret = RegisterWindowManagerAgent(type, windowManagerAgentProxy);
            reply.WriteBool(ret);
            break;
        }
        case WindowManagerMessage::TRANS_ID_UNREGISTER_WINDOW_MANAGER_AGENT: {
            auto type = static_cast<WindowManagerAgentType>(data.ReadUint32());
            sptr<IRemoteObject> windowManagerAgentObject = data.ReadRemoteObject();
            sptr<IWindowManagerAgent> windowManagerAgentProxy =
                iface_cast<IWindowManagerAgent>(windowManagerAgentObject);
            bool ret = UnregisterWindowManagerAgent(type, windowManagerAgentProxy);
            reply.WriteBool(ret);
            break;
        }
        case WindowManagerMessage::TRANS_ID_NOTIFY_READY_MOVE_OR_DRAG: {
            uint32_t windowId = data.ReadUint32();
            sptr<WindowProperty> windowProperty = data.ReadStrongParcelable<WindowProperty>();
            sptr<MoveDragProperty> moveDragProperty = data.ReadStrongParcelable<MoveDragProperty>();
            NotifyServerReadyToMoveOrDrag(windowId, windowProperty, moveDragProperty);
            break;
        }
        case WindowManagerMessage::TRANS_ID_PROCESS_POINT_DOWN: {
            uint32_t windowId = data.ReadUint32();
            bool isPointDown = data.ReadBool();
            ProcessPointDown(windowId, isPointDown);
            break;
        }
        case WindowManagerMessage::TRANS_ID_PROCESS_POINT_UP: {
            uint32_t windowId = data.ReadUint32();
            ProcessPointUp(windowId);
            break;
        }
        case WindowManagerMessage::TRANS_ID_GET_TOP_WINDOW_ID: {
            uint32_t mainWinId = data.ReadUint32();
            uint32_t topWinId;
            WMError errCode = GetTopWindowId(mainWinId, topWinId);
            reply.WriteUint32(topWinId);
            reply.WriteInt32(static_cast<int32_t>(errCode));
            break;
        }
        case WindowManagerMessage::TRANS_ID_MINIMIZE_ALL_APP_WINDOWS: {
            MinimizeAllAppWindows(data.ReadUint64());
            break;
        }
        case WindowManagerMessage::TRANS_ID_TOGGLE_SHOWN_STATE_FOR_ALL_APP_WINDOWS: {
            WMError errCode = ToggleShownStateForAllAppWindows();
            reply.WriteInt32(static_cast<int32_t>(errCode));
            break;
        }
        case WindowManagerMessage::TRANS_ID_UPDATE_LAYOUT_MODE: {
            auto mode = static_cast<WindowLayoutMode>(data.ReadUint32());
            WMError errCode = SetWindowLayoutMode(mode);
            reply.WriteInt32(static_cast<int32_t>(errCode));
            break;
        }
        case WindowManagerMessage::TRANS_ID_UPDATE_PROPERTY: {
            auto action = static_cast<PropertyChangeAction>(data.ReadUint32());
            sptr<WindowProperty> windowProperty = new WindowProperty();
            windowProperty->Read(data, action);
            WMError errCode = UpdateProperty(windowProperty, action);
            reply.WriteInt32(static_cast<int32_t>(errCode));
            break;
        }
        case WindowManagerMessage::TRANS_ID_GET_ACCESSIBILITY_WINDOW_INFO_ID: {
            std::vector<sptr<AccessibilityWindowInfo>> infos;
            WMError errCode = GetAccessibilityWindowInfo(infos);
            if (!MarshallingHelper::MarshallingVectorParcelableObj<AccessibilityWindowInfo>(reply, infos)) {
                WLOGFE("Write accessibility window infos failed");
                return -1;
            }
            reply.WriteInt32(static_cast<int32_t>(errCode));
            break;
        }
        case WindowManagerMessage::TRANS_ID_GET_VISIBILITY_WINDOW_INFO_ID: {
            std::vector<sptr<WindowVisibilityInfo>> infos;
            WMError errCode = GetVisibilityWindowInfo(infos);
            if (!MarshallingHelper::MarshallingVectorParcelableObj<WindowVisibilityInfo>(reply, infos)) {
                WLOGFE("Write visibility window infos failed");
                return -1;
            }
            reply.WriteInt32(static_cast<int32_t>(errCode));
            break;
        }
        case WindowManagerMessage::TRANS_ID_ANIMATION_SET_CONTROLLER: {
            sptr<IRemoteObject> controllerObject = data.ReadRemoteObject();
            sptr<RSIWindowAnimationController> controller = iface_cast<RSIWindowAnimationController>(controllerObject);
            WMError errCode = SetWindowAnimationController(controller);
            reply.WriteInt32(static_cast<int32_t>(errCode));
            break;
        }
        case WindowManagerMessage::TRANS_ID_GET_SYSTEM_CONFIG: {
            SystemConfig config;
            WMError errCode = GetSystemConfig(config);
            reply.WriteParcelable(&config);
            reply.WriteInt32(static_cast<int32_t>(errCode));
            break;
        }
        case WindowManagerMessage::TRANS_ID_NOTIFY_WINDOW_TRANSITION: {
            sptr<WindowTransitionInfo> from = data.ReadParcelable<WindowTransitionInfo>();
            sptr<WindowTransitionInfo> to = data.ReadParcelable<WindowTransitionInfo>();
            bool isFromClient = data.ReadBool();
            WMError errCode = NotifyWindowTransition(from, to, isFromClient);
            reply.WriteInt32(static_cast<int32_t>(errCode));
            break;
        }
        case WindowManagerMessage::TRANS_ID_GET_FULLSCREEN_AND_SPLIT_HOT_ZONE: {
            DisplayId displayId = data.ReadUint64();
            ModeChangeHotZones hotZones = { { 0, 0, 0, 0 }, { 0, 0, 0, 0 }, { 0, 0, 0, 0 } };
            WMError errCode = GetModeChangeHotZones(displayId, hotZones);
            reply.WriteInt32(static_cast<int32_t>(errCode));

            reply.WriteInt32(hotZones.fullscreen_.posX_);
            reply.WriteInt32(hotZones.fullscreen_.posY_);
            reply.WriteUint32(hotZones.fullscreen_.width_);
            reply.WriteUint32(hotZones.fullscreen_.height_);

            reply.WriteInt32(hotZones.primary_.posX_);
            reply.WriteInt32(hotZones.primary_.posY_);
            reply.WriteUint32(hotZones.primary_.width_);
            reply.WriteUint32(hotZones.primary_.height_);

            reply.WriteInt32(hotZones.secondary_.posX_);
            reply.WriteInt32(hotZones.secondary_.posY_);
            reply.WriteUint32(hotZones.secondary_.width_);
            reply.WriteUint32(hotZones.secondary_.height_);
            break;
        }
        case WindowManagerMessage::TRANS_ID_GET_ANIMATION_CALLBACK: {
            std::vector<uint32_t> windowIds;
            data.ReadUInt32Vector(&windowIds);
            bool isAnimated = data.ReadBool();
            sptr<RSIWindowAnimationFinishedCallback> finishedCallback = nullptr;
            MinimizeWindowsByLauncher(windowIds, isAnimated, finishedCallback);
            if (finishedCallback == nullptr) {
                if (!reply.WriteBool(false)) {
                    WLOGFE("finishedCallback is nullptr and failed to write!");
                    return 0;
                }
            } else {
                if (!reply.WriteBool(true) || !reply.WriteRemoteObject(finishedCallback->AsObject())) {
                    WLOGFE("finishedCallback is not nullptr and failed to write!");
                    return 0;
                }
            }
            break;
        }
        case WindowManagerMessage::TRANS_ID_UPDATE_AVOIDAREA_LISTENER: {
            uint32_t windowId = data.ReadUint32();
            bool haveAvoidAreaListener = data.ReadBool();
            WMError errCode = UpdateAvoidAreaListener(windowId, haveAvoidAreaListener);
            reply.WriteInt32(static_cast<int32_t>(errCode));
            break;
        }
        case WindowManagerMessage::TRANS_ID_UPDATE_RS_TREE: {
            uint32_t windowId = data.ReadUint32();
            bool isAdd = data.ReadBool();
            WMError errCode = UpdateRsTree(windowId, isAdd);
            reply.WriteInt32(static_cast<int32_t>(errCode));
            break;
        }
        case WindowManagerMessage::TRANS_ID_BIND_DIALOG_TARGET: {
            uint32_t windowId = data.ReadUint32();
            sptr<IRemoteObject> targetToken = data.ReadRemoteObject();
            WMError errCode = BindDialogTarget(windowId, targetToken);
            reply.WriteInt32(static_cast<int32_t>(errCode));
            break;
        }
        case WindowManagerMessage::TRANS_ID_SET_ANCHOR_AND_SCALE : {
            int32_t x = data.ReadInt32();
            int32_t y = data.ReadInt32();
            float scale = data.ReadFloat();
            SetAnchorAndScale(x, y, scale);
            break;
        }
        case WindowManagerMessage::TRANS_ID_SET_ANCHOR_OFFSET: {
            int32_t deltaX = data.ReadInt32();
            int32_t deltaY = data.ReadInt32();
            SetAnchorOffset(deltaX, deltaY);
            break;
        }
        case WindowManagerMessage::TRANS_ID_OFF_WINDOW_ZOOM: {
            OffWindowZoom();
            break;
        }
        case WindowManagerMessage::TRANS_ID_GET_SNAPSHOT: {
            uint32_t windowId = data.ReadUint32();
            std::shared_ptr<Media::PixelMap> pixelMap = GetSnapshot(windowId);
            reply.WriteParcelable(pixelMap.get());
            break;
        }
        case WindowManagerMessage::TRANS_ID_NOTIFY_DUMP_INFO_RESULT: {
            std::vector<std::string> info;
            data.ReadStringVector(&info);
            NotifyDumpInfoResult(info);
            break;
        }
        default:
            WLOGFW("unknown transaction code %{public}d", code);
            return IPCObjectStub::OnRemoteRequest(code, data, reply, option);
    }
    return 0;
}
} // namespace Rosen
} // namespace OHOS
