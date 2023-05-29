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

#include "windowipc_fuzzer.h"

#include <iremote_broker.h>
#include <iservice_registry.h>
#include <securec.h>
#include <system_ability_definition.h>

#include "zidl/window_manager_proxy.h"
#include "window_manager_hilog.h"

namespace OHOS ::Rosen {
namespace {
    constexpr HiviewDFX::HiLogLabel LABEL = {LOG_CORE, HILOG_DOMAIN_DISPLAY, "DisplayManagerIPC_Fuzzer"};
}
template<class T>
size_t GetObject(T &object, const uint8_t *data, size_t size)
{
    size_t objectSize = sizeof(object);
    if (objectSize > size) {
        return 0;
    }
    return memcpy_s(&object, objectSize, data, objectSize) == EOK ? objectSize : 0;
}

std::pair<sptr<IWindowManager>, sptr<IRemoteObject>> GetProxy()
{
    sptr<ISystemAbilityManager> systemAbilityManager =
        SystemAbilityManagerClient::GetInstance().GetSystemAbilityManager();
    if (!systemAbilityManager) {
        WLOGFE("Failed to get system ability mgr.");
        return { nullptr, nullptr };
    }
    sptr<IRemoteObject> remoteObject = systemAbilityManager->GetSystemAbility(WINDOW_MANAGER_SERVICE_ID);
    if (!remoteObject) {
        WLOGFE("Failed to get display manager service.");
        return { nullptr, nullptr };
    }
    sptr<IWindowManager> windowManagerServiceProxy = iface_cast<IWindowManager>(remoteObject);
    if ((!windowManagerServiceProxy) || (!windowManagerServiceProxy->AsObject())) {
        WLOGFE("Failed to get system display manager services");
        return { nullptr, nullptr };
    }
    return { windowManagerServiceProxy, remoteObject };
}

bool IPCFuzzTest(const uint8_t* data, size_t size)
{
    uint32_t code;
    int flags, waitTime;
    if (data == nullptr || size < sizeof(code) + sizeof(flags) + sizeof(waitTime)) {
        return false;
    }
    auto proxy = GetProxy();
    if (proxy.first == nullptr || proxy.second == nullptr) {
        return false;
    }
    size_t startPos = 0;
    startPos += GetObject<uint32_t>(code, data + startPos, size - startPos);
    startPos += GetObject<int>(flags, data + startPos, size - startPos);
    startPos += GetObject<int>(waitTime, data + startPos, size - startPos);
    MessageParcel sendData;
    MessageParcel reply;
    MessageOption option(flags, waitTime);
    sendData.WriteBuffer(data + startPos, size - startPos);
    proxy.second->SendRequest(code, sendData, reply, option);
    return true;
}

void IPCSpecificInterfaceFuzzTest1(sptr<IRemoteObject> proxy, MessageParcel& sendData, MessageParcel& reply,
    MessageOption& option)
{
    proxy->SendRequest(static_cast<uint32_t>(IWindowManager::WindowManagerMessage::TRANS_ID_CREATE_WINDOW),
        sendData, reply, option);
    proxy->SendRequest(static_cast<uint32_t>(IWindowManager::WindowManagerMessage::TRANS_ID_ADD_WINDOW),
        sendData, reply, option);
    proxy->SendRequest(static_cast<uint32_t>(IWindowManager::WindowManagerMessage::TRANS_ID_REMOVE_WINDOW),
        sendData, reply, option);
    proxy->SendRequest(static_cast<uint32_t>(IWindowManager::WindowManagerMessage::TRANS_ID_DESTROY_WINDOW),
        sendData, reply, option);
    proxy->SendRequest(static_cast<uint32_t>(IWindowManager::WindowManagerMessage::TRANS_ID_REQUEST_FOCUS),
        sendData, reply, option);
    proxy->SendRequest(
        static_cast<uint32_t>(IWindowManager::WindowManagerMessage::TRANS_ID_REGISTER_FOCUS_CHANGED_LISTENER),
        sendData, reply, option);
    proxy->SendRequest(
        static_cast<uint32_t>(IWindowManager::WindowManagerMessage::TRANS_ID_UNREGISTER_FOCUS_CHANGED_LISTENER),
        sendData, reply, option);
    proxy->SendRequest(
        static_cast<uint32_t>(IWindowManager::WindowManagerMessage::TRANS_ID_REGISTER_WINDOW_MANAGER_AGENT),
        sendData, reply, option);
    proxy->SendRequest(
        static_cast<uint32_t>(IWindowManager::WindowManagerMessage::TRANS_ID_UNREGISTER_WINDOW_MANAGER_AGENT),
        sendData, reply, option);
    proxy->SendRequest(static_cast<uint32_t>(IWindowManager::WindowManagerMessage::TRANS_ID_GET_AVOID_AREA),
        sendData, reply, option);
    proxy->SendRequest(static_cast<uint32_t>(IWindowManager::WindowManagerMessage::TRANS_ID_GET_TOP_WINDOW_ID),
        sendData, reply, option);
    proxy->SendRequest(static_cast<uint32_t>(IWindowManager::WindowManagerMessage::TRANS_ID_NOTIFY_READY_MOVE_OR_DRAG),
        sendData, reply, option);
    proxy->SendRequest(static_cast<uint32_t>(IWindowManager::WindowManagerMessage::TRANS_ID_PROCESS_POINT_DOWN),
        sendData, reply, option);
    proxy->SendRequest(static_cast<uint32_t>(IWindowManager::WindowManagerMessage::TRANS_ID_PROCESS_POINT_UP),
        sendData, reply, option);
    proxy->SendRequest(static_cast<uint32_t>(IWindowManager::WindowManagerMessage::TRANS_ID_MINIMIZE_ALL_APP_WINDOWS),
        sendData, reply, option);
}

void IPCSpecificInterfaceFuzzTest2(sptr<IRemoteObject> proxy, MessageParcel& sendData, MessageParcel& reply,
    MessageOption& option)
{
    proxy->SendRequest(
        static_cast<uint32_t>(IWindowManager::WindowManagerMessage::TRANS_ID_TOGGLE_SHOWN_STATE_FOR_ALL_APP_WINDOWS),
        sendData, reply, option);
    proxy->SendRequest(static_cast<uint32_t>(IWindowManager::WindowManagerMessage::TRANS_ID_SET_BACKGROUND_BLUR),
        sendData, reply, option);
    proxy->SendRequest(static_cast<uint32_t>(IWindowManager::WindowManagerMessage::TRANS_ID_SET_ALPHA),
        sendData, reply, option);
    proxy->SendRequest(static_cast<uint32_t>(IWindowManager::WindowManagerMessage::TRANS_ID_UPDATE_LAYOUT_MODE),
        sendData, reply, option);
    proxy->SendRequest(static_cast<uint32_t>(IWindowManager::WindowManagerMessage::TRANS_ID_UPDATE_PROPERTY),
        sendData, reply, option);
    proxy->SendRequest(
        static_cast<uint32_t>(IWindowManager::WindowManagerMessage::TRANS_ID_GET_ACCESSIBILITY_WINDOW_INFO_ID),
        sendData, reply, option);
    proxy->SendRequest(
        static_cast<uint32_t>(IWindowManager::WindowManagerMessage::TRANS_ID_ANIMATION_SET_CONTROLLER),
        sendData, reply, option);
    proxy->SendRequest(static_cast<uint32_t>(IWindowManager::WindowManagerMessage::TRANS_ID_GET_SYSTEM_CONFIG),
        sendData, reply, option);
    proxy->SendRequest(static_cast<uint32_t>(IWindowManager::WindowManagerMessage::TRANS_ID_NOTIFY_WINDOW_TRANSITION),
        sendData, reply, option);
    proxy->SendRequest(
        static_cast<uint32_t>(IWindowManager::WindowManagerMessage::TRANS_ID_GET_FULLSCREEN_AND_SPLIT_HOT_ZONE),
        sendData, reply, option);
}

bool IPCInterfaceFuzzTest(const uint8_t* data, size_t size)
{
    int flags, waitTime;
    if (data == nullptr || size < sizeof(flags) + sizeof(waitTime)) {
        return false;
    }
    auto proxy = GetProxy();
    if (proxy.first == nullptr || proxy.second == nullptr) {
        return false;
    }
    size_t startPos = 0;
    startPos += GetObject<int>(flags, data + startPos, size - startPos);
    startPos += GetObject<int>(waitTime, data + startPos, size - startPos);
    MessageParcel sendData;
    MessageParcel reply;
    MessageOption option(flags, waitTime);
    sendData.WriteInterfaceToken(proxy.first->GetDescriptor());
    sendData.WriteBuffer(data + startPos, size - startPos);
    IPCSpecificInterfaceFuzzTest1(proxy.second, sendData, reply, option);
    IPCSpecificInterfaceFuzzTest2(proxy.second, sendData, reply, option);
    return true;
}
} // namespace.OHOS::Rosen

/* Fuzzer entry point */
extern "C" int LLVMFuzzerTestOneInput(const uint8_t* data, size_t size)
{
    /* Run your code on data */
    OHOS::Rosen::IPCFuzzTest(data, size);
    OHOS::Rosen::IPCInterfaceFuzzTest(data, size);
    return 0;
}

