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

#include <vector>
#include <new>

#include "js_runtime_utils.h"
#include "native_engine/native_reference.h"
#include "display_manager.h"
#include "window_manager_hilog.h"
#include "singleton_container.h"
#include "js_display_listener.h"
#include "js_display.h"
#include "js_display_manager.h"

namespace OHOS {
namespace Rosen {
using namespace AbilityRuntime;
constexpr size_t ARGC_ONE = 1;
constexpr size_t ARGC_TWO = 2;
constexpr int32_t INDEX_ONE = 1;
namespace {
    constexpr HiviewDFX::HiLogLabel LABEL = {LOG_CORE, HILOG_DOMAIN_DISPLAY, "JsDisplayManager"};
}

class JsDisplayManager {
public:
explicit JsDisplayManager(NativeEngine* engine) {
}

~JsDisplayManager() = default;

static void Finalizer(NativeEngine* engine, void* data, void* hint)
{
    WLOGFI("JsDisplayManager::Finalizer is called");
    std::unique_ptr<JsDisplayManager>(static_cast<JsDisplayManager*>(data));
}

static NativeValue* GetDefaultDisplay(NativeEngine* engine, NativeCallbackInfo* info)
{
    JsDisplayManager* me = CheckParamsAndGetThis<JsDisplayManager>(engine, info);
    return (me != nullptr) ? me->OnGetDefaultDisplay(*engine, *info) : nullptr;
}

static NativeValue* GetDefaultDisplaySync(NativeEngine* engine, NativeCallbackInfo* info)
{
    JsDisplayManager* me = CheckParamsAndGetThis<JsDisplayManager>(engine, info);
    return (me != nullptr) ? me->OnGetDefaultDisplaySync(*engine, *info) : nullptr;
}

static NativeValue* GetAllDisplay(NativeEngine* engine, NativeCallbackInfo* info)
{
    JsDisplayManager* me = CheckParamsAndGetThis<JsDisplayManager>(engine, info);
    return (me != nullptr) ? me->OnGetAllDisplay(*engine, *info) : nullptr;
}

static NativeValue* GetAllDisplays(NativeEngine* engine, NativeCallbackInfo* info)
{
    JsDisplayManager* me = CheckParamsAndGetThis<JsDisplayManager>(engine, info);
    return (me != nullptr) ? me->OnGetAllDisplays(*engine, *info) : nullptr;
}

static NativeValue* RegisterDisplayManagerCallback(NativeEngine* engine, NativeCallbackInfo* info)
{
    JsDisplayManager* me = CheckParamsAndGetThis<JsDisplayManager>(engine, info);
    return (me != nullptr) ? me->OnRegisterDisplayManagerCallback(*engine, *info) : nullptr;
}

static NativeValue* UnregisterDisplayManagerCallback(NativeEngine* engine, NativeCallbackInfo* info)
{
    JsDisplayManager* me = CheckParamsAndGetThis<JsDisplayManager>(engine, info);
    return (me != nullptr) ? me->OnUnregisterDisplayManagerCallback(*engine, *info) : nullptr;
}

static NativeValue* HasPrivateWindow(NativeEngine* engine, NativeCallbackInfo* info)
{
    JsDisplayManager* me = CheckParamsAndGetThis<JsDisplayManager>(engine, info);
    return (me != nullptr) ? me->OnHasPrivateWindow(*engine, *info) : nullptr;
}

private:
std::map<std::string, std::map<std::unique_ptr<NativeReference>, sptr<JsDisplayListener>>> jsCbMap_;
std::mutex mtx_;

NativeValue* OnGetDefaultDisplay(NativeEngine& engine, NativeCallbackInfo& info)
{
    WLOGFI("JsDisplayManager::OnGetDefaultDisplay is called");
    DMError errCode = DMError::DM_OK;
    if (info.argc != 0 && info.argc != ARGC_ONE) {
        WLOGFE("JsDisplayManager::OnGetDefaultDisplay params not match");
        errCode = DMError::DM_ERROR_INVALID_PARAM;
    }

    AsyncTask::CompleteCallback complete =
        [=](NativeEngine& engine, AsyncTask& task, int32_t status) {
            if (errCode != DMError::DM_OK) {
                task.Reject(engine, CreateJsError(engine,
                    static_cast<int32_t>(errCode), "JsDisplayManager::OnGetDefaultDisplay failed."));
            }
            sptr<Display> display = SingletonContainer::Get<DisplayManager>().GetDefaultDisplay();
            if (display != nullptr) {
                task.Resolve(engine, CreateJsDisplayObject(engine, display));
                WLOGFI("JsDisplayManager::OnGetDefaultDisplay success");
            } else {
                task.Reject(engine, CreateJsError(engine,
                    static_cast<int32_t>(DMError::DM_ERROR_NULLPTR), "JsDisplayManager::OnGetDefaultDisplay failed."));
            }
        };
    NativeValue* lastParam = nullptr;
    if (info.argc == ARGC_ONE && info.argv[0]->TypeOf() == NATIVE_FUNCTION) {
        lastParam = info.argv[0];
    }
    NativeValue* result = nullptr;
    AsyncTask::Schedule("JsDisplayManager::OnGetDefaultDisplay",
        engine, CreateAsyncTaskWithLastParam(engine, lastParam, nullptr, std::move(complete), &result));
    return result;
}

NativeValue* OnGetDefaultDisplaySync(NativeEngine& engine, NativeCallbackInfo& info)
{
    WLOGFI("JsDisplayManager::OnGetDefaultDisplaySync is called");
    sptr<Display> display = SingletonContainer::Get<DisplayManager>().GetDefaultDisplaySync();
    if (display == nullptr) {
        WLOGFE("JsDisplayManager::OnGetDefaultDisplaySync, display is nullptr.");
        engine.Throw(CreateJsError(engine, static_cast<int32_t>(DmErrorCode::DM_ERROR_INVALID_SCREEN)));
        return engine.CreateUndefined();
    }
    return CreateJsDisplayObject(engine, display);
}

NativeValue* OnGetAllDisplay(NativeEngine& engine, NativeCallbackInfo& info)
{
    WLOGFI("JsDisplayManager::OnGetAllDisplay is called");
    DMError errCode = DMError::DM_OK;
    if (info.argc != 0 && info.argc != ARGC_ONE) {
        WLOGFE("JsDisplayManager::OnGetAllDisplay params not match");
        errCode = DMError::DM_ERROR_INVALID_PARAM;
    }

    AsyncTask::CompleteCallback complete =
        [=](NativeEngine& engine, AsyncTask& task, int32_t status) {
            if (errCode != DMError::DM_OK) {
                task.Reject(engine, CreateJsError(engine,
                    static_cast<int32_t>(errCode), "JsDisplayManager::OnGetAllDisplay failed."));
            }
            std::vector<sptr<Display>> displays = SingletonContainer::Get<DisplayManager>().GetAllDisplays();
            if (!displays.empty()) {
                task.Resolve(engine, CreateJsDisplayArrayObject(engine, displays));
                WLOGFI("JsDisplayManager::GetAllDisplays success");
            } else {
                task.Reject(engine, CreateJsError(engine,
                    static_cast<int32_t>(DMError::DM_ERROR_NULLPTR), "JsDisplayManager::OnGetAllDisplay failed."));
            }
        };

    NativeValue* lastParam = nullptr;
    if (info.argc == ARGC_ONE && info.argv[0]->TypeOf() == NATIVE_FUNCTION) {
        lastParam = info.argv[0];
    }
    NativeValue* result = nullptr;
    AsyncTask::Schedule("JsDisplayManager::OnGetAllDisplay",
        engine, CreateAsyncTaskWithLastParam(engine, lastParam, nullptr, std::move(complete), &result));
    return result;
}

NativeValue* OnGetAllDisplays(NativeEngine& engine, NativeCallbackInfo& info)
{
    WLOGFI("JsDisplayManager::OnGetAllDisplays is called");

    AsyncTask::CompleteCallback complete =
        [=](NativeEngine& engine, AsyncTask& task, int32_t status) {
            std::vector<sptr<Display>> displays = SingletonContainer::Get<DisplayManager>().GetAllDisplays();
            if (!displays.empty()) {
                task.Resolve(engine, CreateJsDisplayArrayObject(engine, displays));
                WLOGFI("JsDisplayManager::GetAllDisplays success");
            } else {
                task.Reject(engine, CreateJsError(engine,
                    static_cast<int32_t>(DmErrorCode::DM_ERROR_INVALID_SCREEN),
                    "JsDisplayManager::OnGetAllDisplays failed."));
            }
        };

    NativeValue* lastParam = nullptr;
    if (info.argc >= ARGC_ONE && info.argv[ARGC_ONE - 1] != nullptr &&
        info.argv[ARGC_ONE - 1]->TypeOf() == NATIVE_FUNCTION) {
        lastParam = info.argv[0];
    }
    NativeValue* result = nullptr;
    AsyncTask::Schedule("JsDisplayManager::OnGetAllDisplays",
        engine, CreateAsyncTaskWithLastParam(engine, lastParam, nullptr, std::move(complete), &result));
    return result;
}

void RegisterDisplayListenerWithType(NativeEngine& engine, const std::string& type, NativeValue* value)
{
    if (IfCallbackRegistered(type, value)) {
        WLOGFE("JsDisplayManager::RegisterDisplayListenerWithType callback already registered!");
        return;
    }
    std::unique_ptr<NativeReference> callbackRef;
    callbackRef.reset(engine.CreateReference(value, 1));
    sptr<JsDisplayListener> displayListener = new(std::nothrow) JsDisplayListener(&engine);
    if (displayListener == nullptr) {
        WLOGFE("displayListener is nullptr");
        return;
    }
    if (type == EVENT_ADD || type == EVENT_REMOVE || type == EVENT_CHANGE) {
        SingletonContainer::Get<DisplayManager>().RegisterDisplayListener(displayListener);
        WLOGFI("JsDisplayManager::RegisterDisplayListenerWithType success");
    } else {
        WLOGFE("JsDisplayManager::RegisterDisplayListenerWithType failed method: %{public}s not support!",
               type.c_str());
        return;
    }
    displayListener->AddCallback(type, value);
    jsCbMap_[type][std::move(callbackRef)] = displayListener;
}

bool IfCallbackRegistered(const std::string& type, NativeValue* jsListenerObject)
{
    if (jsCbMap_.empty() || jsCbMap_.find(type) == jsCbMap_.end()) {
        WLOGFI("JsDisplayManager::IfCallbackRegistered methodName %{public}s not registered!", type.c_str());
        return false;
    }

    for (auto& iter : jsCbMap_[type]) {
        if (jsListenerObject->StrictEquals(iter.first->Get())) {
            WLOGFE("JsDisplayManager::IfCallbackRegistered callback already registered!");
            return true;
        }
    }
    return false;
}

void UnregisterAllDisplayListenerWithType(const std::string& type)
{
    if (jsCbMap_.empty() || jsCbMap_.find(type) == jsCbMap_.end()) {
        WLOGFI("JsDisplayManager::UnregisterAllDisplayListenerWithType methodName %{public}s not registered!",
               type.c_str());
        return;
    }
    for (auto it = jsCbMap_[type].begin(); it != jsCbMap_[type].end();) {
        it->second->RemoveAllCallback();
        if (type == EVENT_ADD || type == EVENT_REMOVE || type == EVENT_CHANGE) {
            sptr<DisplayManager::IDisplayListener> thisListener(it->second);
            SingletonContainer::Get<DisplayManager>().UnregisterDisplayListener(thisListener);
            WLOGFI("JsDisplayManager::UnregisterAllDisplayListenerWithType success");
        }
        jsCbMap_[type].erase(it++);
    }
    jsCbMap_.erase(type);
}

void UnRegisterDisplayListenerWithType(const std::string& type, NativeValue* value)
{
    if (jsCbMap_.empty() || jsCbMap_.find(type) == jsCbMap_.end()) {
        WLOGFI("JsDisplayManager::UnRegisterDisplayListenerWithType methodName %{public}s not registered!",
               type.c_str());
        return;
    }
    for (auto it = jsCbMap_[type].begin(); it != jsCbMap_[type].end();) {
        if (value->StrictEquals(it->first->Get())) {
            it->second->RemoveCallback(type, value);
            if (type == EVENT_ADD || type == EVENT_REMOVE || type == EVENT_CHANGE) {
                sptr<DisplayManager::IDisplayListener> thisListener(it->second);
                SingletonContainer::Get<DisplayManager>().UnregisterDisplayListener(thisListener);
                WLOGFI("JsDisplayManager::UnRegisterDisplayListenerWithType success");
            }
            jsCbMap_[type].erase(it++);
            break;
        } else {
            it++;
        }
    }
    if (jsCbMap_[type].empty()) {
        jsCbMap_.erase(type);
    }
}

NativeValue* OnRegisterDisplayManagerCallback(NativeEngine& engine, NativeCallbackInfo& info)
{
    WLOGFI("JsDisplayManager::OnRegisterDisplayManagerCallback is called");
    if (info.argc < ARGC_TWO) {
        WLOGFE("JsDisplayManager Params not match: %{public}zu", info.argc);
        engine.Throw(CreateJsError(engine, static_cast<int32_t>(DmErrorCode::DM_ERROR_INVALID_PARAM)));
        return engine.CreateUndefined();
    }
    std::string cbType;
    if (!ConvertFromJsValue(engine, info.argv[0], cbType)) {
        engine.Throw(CreateJsError(engine, static_cast<int32_t>(DmErrorCode::DM_ERROR_INVALID_PARAM)));
        WLOGFE("Failed to convert parameter to callbackType");
        return engine.CreateUndefined();
    }
    NativeValue* value = info.argv[INDEX_ONE];
    if (value == nullptr) {
        WLOGFI("JsDisplayManager::OnRegisterDisplayManagerCallback info->argv[1] is nullptr");
        engine.Throw(CreateJsError(engine, static_cast<int32_t>(DmErrorCode::DM_ERROR_INVALID_PARAM)));
        return engine.CreateUndefined();
    }
    if (!value->IsCallable()) {
        WLOGFI("JsDisplayManager::OnRegisterDisplayManagerCallback info->argv[1] is not callable");
        engine.Throw(CreateJsError(engine, static_cast<int32_t>(DmErrorCode::DM_ERROR_INVALID_PARAM)));
        return engine.CreateUndefined();
    }
    std::lock_guard<std::mutex> lock(mtx_);
    RegisterDisplayListenerWithType(engine, cbType, value);
    return engine.CreateUndefined();
}

NativeValue* OnUnregisterDisplayManagerCallback(NativeEngine& engine, NativeCallbackInfo& info)
{
    WLOGFI("JsDisplayManager::OnUnregisterDisplayCallback is called");
    if (info.argc < ARGC_ONE) {
        WLOGFE("JsDisplayManager Params not match %{public}zu", info.argc);
        engine.Throw(CreateJsError(engine, static_cast<int32_t>(DmErrorCode::DM_ERROR_INVALID_PARAM)));
        return engine.CreateUndefined();
    }
    std::string cbType;
    if (!ConvertFromJsValue(engine, info.argv[0], cbType)) {
        WLOGFE("Failed to convert parameter to callbackType");
        engine.Throw(CreateJsError(engine, static_cast<int32_t>(DmErrorCode::DM_ERROR_INVALID_PARAM)));
        return engine.CreateUndefined();
    }
    std::lock_guard<std::mutex> lock(mtx_);
    if (info.argc == ARGC_ONE) {
        UnregisterAllDisplayListenerWithType(cbType);
    } else {
        NativeValue* value = info.argv[INDEX_ONE];
        if ((value == nullptr) || (!value->IsCallable())) {
            UnregisterAllDisplayListenerWithType(cbType);
        } else {
            UnRegisterDisplayListenerWithType(cbType, value);
        }
    }
    return engine.CreateUndefined();
}

NativeValue* OnHasPrivateWindow(NativeEngine& engine, NativeCallbackInfo& info)
{
    bool hasPrivateWindow = false;
    if (info.argc < ARGC_ONE) {
        engine.Throw(CreateJsError(engine, static_cast<int32_t>(DmErrorCode::DM_ERROR_INVALID_PARAM)));
        return engine.CreateUndefined();
    }
    int64_t displayId = static_cast<int64_t>(DISPLAY_ID_INVALID);
    if (!ConvertFromJsValue(engine, info.argv[0], displayId)) {
        WLOGFE("[NAPI]Failed to convert parameter to displayId");
        engine.Throw(CreateJsError(engine, static_cast<int32_t>(DmErrorCode::DM_ERROR_INVALID_PARAM)));
        return engine.CreateUndefined();
    }
    if (displayId < 0) {
        engine.Throw(CreateJsError(engine, static_cast<int32_t>(DmErrorCode::DM_ERROR_INVALID_PARAM)));
        return engine.CreateUndefined();
    }
    DmErrorCode errCode = DM_JS_TO_ERROR_CODE_MAP.at(
        SingletonContainer::Get<DisplayManager>().HasPrivateWindow(displayId, hasPrivateWindow));
    WLOGFI("[NAPI]Display id = %{public}" PRIu64", hasPrivateWindow = %{public}u err = %{public}d",
        static_cast<uint64_t>(displayId), hasPrivateWindow, errCode);
    if (errCode != DmErrorCode::DM_OK) {
        engine.Throw(CreateJsError(engine, static_cast<int32_t>(errCode)));
        return engine.CreateUndefined();
    }
    return engine.CreateBoolean(hasPrivateWindow);
}

NativeValue* CreateJsDisplayArrayObject(NativeEngine& engine, std::vector<sptr<Display>>& displays)
{
    WLOGFI("JsDisplayManager::CreateJsDisplayArrayObject is called");
    NativeValue* arrayValue = engine.CreateArray(displays.size());
    NativeArray* array = ConvertNativeValueTo<NativeArray>(arrayValue);
    if (array == nullptr) {
        WLOGFE("Failed to create display array");
        return engine.CreateUndefined();
    }
    int32_t i = 0;
    for (auto& display : displays) {
        if (display == nullptr) {
            continue;
        }
        array->SetElement(i++, CreateJsDisplayObject(engine, display));
    }
    return arrayValue;
}
};

NativeValue* InitDisplayState(NativeEngine* engine)
{
    WLOGFI("JsDisplayManager::InitDisplayState called");

    if (engine == nullptr) {
        WLOGFE("engine is nullptr");
        return nullptr;
    }

    NativeValue *objValue = engine->CreateObject();
    NativeObject *object = ConvertNativeValueTo<NativeObject>(objValue);
    if (object == nullptr) {
        WLOGFE("Failed to get object");
        return nullptr;
    }

    object->SetProperty("STATE_UNKNOWN", CreateJsValue(*engine, static_cast<int32_t>(DisplayStateMode::STATE_UNKNOWN)));
    object->SetProperty("STATE_OFF", CreateJsValue(*engine, static_cast<int32_t>(DisplayStateMode::STATE_OFF)));
    object->SetProperty("STATE_ON", CreateJsValue(*engine, static_cast<int32_t>(DisplayStateMode::STATE_ON)));
    object->SetProperty("STATE_DOZE",
        CreateJsValue(*engine, static_cast<int32_t>(DisplayStateMode::STATE_DOZE)));
    object->SetProperty("STATE_DOZE_SUSPEND",
        CreateJsValue(*engine, static_cast<int32_t>(DisplayStateMode::STATE_DOZE_SUSPEND)));
    object->SetProperty("STATE_VR",
        CreateJsValue(*engine, static_cast<int32_t>(DisplayStateMode::STATE_VR)));
    object->SetProperty("STATE_ON_SUSPEND",
        CreateJsValue(*engine, static_cast<int32_t>(DisplayStateMode::STATE_ON_SUSPEND)));
    return objValue;
}

NativeValue* InitDisplayErrorCode(NativeEngine* engine)
{
    WLOGFI("JsDisplayManager::InitDisplayErrorCode called");

    if (engine == nullptr) {
        WLOGFE("engine is nullptr");
        return nullptr;
    }

    NativeValue *objValue = engine->CreateObject();
    NativeObject *object = ConvertNativeValueTo<NativeObject>(objValue);
    if (object == nullptr) {
        WLOGFE("Failed to get object");
        return nullptr;
    }

    object->SetProperty("DM_ERROR_NO_PERMISSION",
        CreateJsValue(*engine, static_cast<int32_t>(DmErrorCode::DM_ERROR_NO_PERMISSION)));
    object->SetProperty("DM_ERROR_INVALID_PARAM",
        CreateJsValue(*engine, static_cast<int32_t>(DmErrorCode::DM_ERROR_INVALID_PARAM)));
    object->SetProperty("DM_ERROR_DEVICE_NOT_SUPPORT",
        CreateJsValue(*engine, static_cast<int32_t>(DmErrorCode::DM_ERROR_DEVICE_NOT_SUPPORT)));
    object->SetProperty("DM_ERROR_INVALID_SCREEN",
        CreateJsValue(*engine, static_cast<int32_t>(DmErrorCode::DM_ERROR_INVALID_SCREEN)));
    object->SetProperty("DM_ERROR_INVALID_CALLING",
        CreateJsValue(*engine, static_cast<int32_t>(DmErrorCode::DM_ERROR_INVALID_CALLING)));
    object->SetProperty("DM_ERROR_SYSTEM_INNORMAL",
        CreateJsValue(*engine, static_cast<int32_t>(DmErrorCode::DM_ERROR_SYSTEM_INNORMAL)));

    return objValue;
}

NativeValue* InitDisplayError(NativeEngine* engine)
{
    WLOGFI("JsDisplayManager::InitDisplayError called");

    if (engine == nullptr) {
        WLOGFE("engine is nullptr");
        return nullptr;
    }

    NativeValue *objValue = engine->CreateObject();
    NativeObject *object = ConvertNativeValueTo<NativeObject>(objValue);
    if (object == nullptr) {
        WLOGFE("Failed to get object");
        return nullptr;
    }

    object->SetProperty("DM_ERROR_INIT_DMS_PROXY_LOCKED",
        CreateJsValue(*engine, static_cast<int32_t>(DMError::DM_ERROR_INIT_DMS_PROXY_LOCKED)));
    object->SetProperty("DM_ERROR_IPC_FAILED",
        CreateJsValue(*engine, static_cast<int32_t>(DMError::DM_ERROR_IPC_FAILED)));
    object->SetProperty("DM_ERROR_REMOTE_CREATE_FAILED",
        CreateJsValue(*engine, static_cast<int32_t>(DMError::DM_ERROR_REMOTE_CREATE_FAILED)));
    object->SetProperty("DM_ERROR_NULLPTR",
        CreateJsValue(*engine, static_cast<int32_t>(DMError::DM_ERROR_NULLPTR)));
    object->SetProperty("DM_ERROR_INVALID_PARAM",
        CreateJsValue(*engine, static_cast<int32_t>(DMError::DM_ERROR_INVALID_PARAM)));
    object->SetProperty("DM_ERROR_WRITE_INTERFACE_TOKEN_FAILED",
        CreateJsValue(*engine, static_cast<int32_t>(DMError::DM_ERROR_WRITE_INTERFACE_TOKEN_FAILED)));
    object->SetProperty("DM_ERROR_DEATH_RECIPIENT",
        CreateJsValue(*engine, static_cast<int32_t>(DMError::DM_ERROR_DEATH_RECIPIENT)));
    object->SetProperty("DM_ERROR_INVALID_MODE_ID",
        CreateJsValue(*engine, static_cast<int32_t>(DMError::DM_ERROR_INVALID_MODE_ID)));
    object->SetProperty("DM_ERROR_WRITE_DATA_FAILED",
        CreateJsValue(*engine, static_cast<int32_t>(DMError::DM_ERROR_WRITE_DATA_FAILED)));
    object->SetProperty("DM_ERROR_RENDER_SERVICE_FAILED",
        CreateJsValue(*engine, static_cast<int32_t>(DMError::DM_ERROR_RENDER_SERVICE_FAILED)));
    object->SetProperty("DM_ERROR_UNREGISTER_AGENT_FAILED",
        CreateJsValue(*engine, static_cast<int32_t>(DMError::DM_ERROR_UNREGISTER_AGENT_FAILED)));
    object->SetProperty("DM_ERROR_INVALID_CALLING",
        CreateJsValue(*engine, static_cast<int32_t>(DMError::DM_ERROR_INVALID_CALLING)));
    object->SetProperty("DM_ERROR_UNKNOWN",
        CreateJsValue(*engine, static_cast<int32_t>(DMError::DM_ERROR_UNKNOWN)));

    return objValue;
}

NativeValue* JsDisplayManagerInit(NativeEngine* engine, NativeValue* exportObj)
{
    WLOGFI("JsDisplayManagerInit is called");

    if (engine == nullptr || exportObj == nullptr) {
        WLOGFE("JsDisplayManagerInit engine or exportObj is nullptr");
        return nullptr;
    }

    NativeObject* object = ConvertNativeValueTo<NativeObject>(exportObj);
    if (object == nullptr) {
        WLOGFE("JsDisplayManagerInit object is nullptr");
        return nullptr;
    }

    std::unique_ptr<JsDisplayManager> jsDisplayManager = std::make_unique<JsDisplayManager>(engine);
    object->SetNativePointer(jsDisplayManager.release(), JsDisplayManager::Finalizer, nullptr);

    object->SetProperty("DisplayState", InitDisplayState(engine));
    object->SetProperty("DmErrorCode", InitDisplayErrorCode(engine));
    object->SetProperty("DMError", InitDisplayError(engine));

    const char *moduleName = "JsDisplayManager";
    BindNativeFunction(*engine, *object, "getDefaultDisplay", moduleName, JsDisplayManager::GetDefaultDisplay);
    BindNativeFunction(*engine, *object, "getDefaultDisplaySync", moduleName, JsDisplayManager::GetDefaultDisplaySync);
    BindNativeFunction(*engine, *object, "getAllDisplay", moduleName, JsDisplayManager::GetAllDisplay);
    BindNativeFunction(*engine, *object, "getAllDisplays", moduleName, JsDisplayManager::GetAllDisplays);
    BindNativeFunction(*engine, *object, "hasPrivateWindow", moduleName, JsDisplayManager::HasPrivateWindow);
    BindNativeFunction(*engine, *object, "on", moduleName, JsDisplayManager::RegisterDisplayManagerCallback);
    BindNativeFunction(*engine, *object, "off", moduleName, JsDisplayManager::UnregisterDisplayManagerCallback);
    return engine->CreateUndefined();
}
}  // namespace Rosen
}  // namespace OHOS