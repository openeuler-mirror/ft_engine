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

#include "js_screen_manager.h"

#include <cinttypes>
#include <vector>
#include <new>
#include "js_runtime_utils.h"
#include "js_screen.h"
#include "js_screen_listener.h"
#include "native_engine/native_reference.h"
#include "screen_manager.h"
#include "singleton_container.h"
#include "surface_utils.h"
#include "window_manager_hilog.h"

namespace OHOS {
namespace Rosen {
using namespace AbilityRuntime;
constexpr size_t ARGC_ONE = 1;
constexpr size_t ARGC_TWO = 2;
constexpr size_t ARGC_THREE = 3;
constexpr int32_t INDEX_ONE = 1;
namespace {
    constexpr HiviewDFX::HiLogLabel LABEL = {LOG_CORE, 0, "JsScreenManager"};
}

class JsScreenManager {
public:
explicit JsScreenManager(NativeEngine* engine) {
}

~JsScreenManager() = default;

static void Finalizer(NativeEngine* engine, void* data, void* hint)
{
    WLOGFI("JsScreenManager::Finalizer is called");
    std::unique_ptr<JsScreenManager>(static_cast<JsScreenManager*>(data));
}

static NativeValue* GetAllScreens(NativeEngine* engine, NativeCallbackInfo* info)
{
    JsScreenManager* me = CheckParamsAndGetThis<JsScreenManager>(engine, info);
    return (me != nullptr) ? me->OnGetAllScreens(*engine, *info) : nullptr;
}

static NativeValue* RegisterScreenManagerCallback(NativeEngine* engine, NativeCallbackInfo* info)
{
    JsScreenManager* me = CheckParamsAndGetThis<JsScreenManager>(engine, info);
    return (me != nullptr) ? me->OnRegisterScreenMangerCallback(*engine, *info) : nullptr;
}

static NativeValue* UnregisterScreenMangerCallback(NativeEngine* engine, NativeCallbackInfo* info)
{
    JsScreenManager* me = CheckParamsAndGetThis<JsScreenManager>(engine, info);
    return (me != nullptr) ? me->OnUnregisterScreenManagerCallback(*engine, *info) : nullptr;
}

static NativeValue* MakeMirror(NativeEngine* engine, NativeCallbackInfo* info)
{
    JsScreenManager* me = CheckParamsAndGetThis<JsScreenManager>(engine, info);
    return (me != nullptr) ? me->OnMakeMirror(*engine, *info) : nullptr;
}

static NativeValue* MakeExpand(NativeEngine* engine, NativeCallbackInfo* info)
{
    JsScreenManager* me = CheckParamsAndGetThis<JsScreenManager>(engine, info);
    return (me != nullptr) ? me->OnMakeExpand(*engine, *info) : nullptr;
}

static NativeValue* CreateVirtualScreen(NativeEngine* engine, NativeCallbackInfo* info)
{
    JsScreenManager* me = CheckParamsAndGetThis<JsScreenManager>(engine, info);
    return (me != nullptr) ? me->OnCreateVirtualScreen(*engine, *info) : nullptr;
}

static NativeValue* DestroyVirtualScreen(NativeEngine* engine, NativeCallbackInfo* info)
{
    JsScreenManager* me = CheckParamsAndGetThis<JsScreenManager>(engine, info);
    return (me != nullptr) ? me->OnDestroyVirtualScreen(*engine, *info) : nullptr;
}

static NativeValue* SetVirtualScreenSurface(NativeEngine* engine, NativeCallbackInfo* info)
{
    JsScreenManager* me = CheckParamsAndGetThis<JsScreenManager>(engine, info);
    return (me != nullptr) ? me->OnSetVirtualScreenSurface(*engine, *info) : nullptr;
}

static NativeValue* IsScreenRotationLocked(NativeEngine* engine, NativeCallbackInfo* info)
{
    JsScreenManager* me = CheckParamsAndGetThis<JsScreenManager>(engine, info);
    return (me != nullptr) ? me->OnIsScreenRotationLocked(*engine, *info) : nullptr;
}

static NativeValue* SetScreenRotationLocked(NativeEngine* engine, NativeCallbackInfo* info)
{
    JsScreenManager* me = CheckParamsAndGetThis<JsScreenManager>(engine, info);
    return (me != nullptr) ? me->OnSetScreenRotationLocked(*engine, *info) : nullptr;
}
private:
std::map<std::string, std::map<std::unique_ptr<NativeReference>, sptr<JsScreenListener>>> jsCbMap_;
std::mutex mtx_;

NativeValue* OnGetAllScreens(NativeEngine& engine, NativeCallbackInfo& info)
{
    WLOGFI("OnGetAllScreens is called");
    AsyncTask::CompleteCallback complete =
        [this](NativeEngine& engine, AsyncTask& task, int32_t status) {
            std::vector<sptr<Screen>> screens = SingletonContainer::Get<ScreenManager>().GetAllScreens();
            if (!screens.empty()) {
                task.Resolve(engine, CreateJsScreenVectorObject(engine, screens));
                WLOGFI("JsScreenManager::OnGetAllScreens success");
            } else {
                task.Reject(engine, CreateJsError(engine,
                    static_cast<int32_t>(DmErrorCode::DM_ERROR_INVALID_SCREEN),
                    "JsScreenManager::OnGetAllScreens failed."));
            }
        };
    NativeValue* lastParam = nullptr;
    if (info.argc >= ARGC_ONE && info.argv[ARGC_ONE - 1] != nullptr &&
        info.argv[ARGC_ONE - 1]->TypeOf() == NATIVE_FUNCTION) {
        lastParam = info.argv[ARGC_ONE - 1];
    }
    NativeValue* result = nullptr;
    AsyncTask::Schedule("JsScreenManager::OnGetAllScreens",
        engine, CreateAsyncTaskWithLastParam(engine, lastParam, nullptr, std::move(complete), &result));
    return result;
}

NativeValue* CreateJsScreenVectorObject(NativeEngine& engine, std::vector<sptr<Screen>>& screens)
{
    NativeValue* arrayValue = engine.CreateArray(screens.size());
    NativeArray* array = ConvertNativeValueTo<NativeArray>(arrayValue);
    if (array == nullptr) {
        WLOGFE("Failed to get screens");
        return engine.CreateUndefined();
    }
    size_t i = 0;
    for (auto& screen : screens) {
        if (screen == nullptr) {
            continue;
        }
        array->SetElement(i++, CreateJsScreenObject(engine, screen));
    }
    return arrayValue;
}

bool IfCallbackRegistered(const std::string& type, NativeValue* jsListenerObject)
{
    if (jsCbMap_.empty() || jsCbMap_.find(type) == jsCbMap_.end()) {
        WLOGFI("JsScreenManager::IfCallbackRegistered methodName %{public}s not registered!", type.c_str());
        return false;
    }

    for (auto& iter : jsCbMap_[type]) {
        if (jsListenerObject->StrictEquals(iter.first->Get())) {
            WLOGFE("JsScreenManager::IfCallbackRegistered callback already registered!");
            return true;
        }
    }
    return false;
}

void RegisterScreenListenerWithType(NativeEngine& engine, const std::string& type, NativeValue* value)
{
    if (IfCallbackRegistered(type, value)) {
        WLOGFE("JsScreenManager::RegisterScreenListenerWithType callback already registered!");
        return;
    }
    std::unique_ptr<NativeReference> callbackRef;
    callbackRef.reset(engine.CreateReference(value, 1));
    sptr<JsScreenListener> screenListener = new(std::nothrow) JsScreenListener(&engine);
    if (screenListener == nullptr) {
        WLOGFE("screenListener is nullptr");
        return;
    }
    if (type == EVENT_CONNECT || type == EVENT_DISCONNECT || type == EVENT_CHANGE) {
        SingletonContainer::Get<ScreenManager>().RegisterScreenListener(screenListener);
        WLOGFI("JsScreenManager::RegisterScreenListenerWithType success");
    } else {
        WLOGFE("JsScreenManager::RegisterScreenListenerWithType failed method: %{public}s not support!",
            type.c_str());
        return;
    }
    screenListener->AddCallback(type, value);
    jsCbMap_[type][std::move(callbackRef)] = screenListener;
}

void UnregisterAllScreenListenerWithType(const std::string& type)
{
    if (jsCbMap_.empty() || jsCbMap_.find(type) == jsCbMap_.end()) {
        WLOGFI("JsScreenManager::UnregisterAllScreenListenerWithType methodName %{public}s not registered!",
            type.c_str());
        return;
    }
    for (auto it = jsCbMap_[type].begin(); it != jsCbMap_[type].end();) {
        it->second->RemoveAllCallback();
        if (type == EVENT_CONNECT || type == EVENT_DISCONNECT || type == EVENT_CHANGE) {
            sptr<ScreenManager::IScreenListener> thisListener(it->second);
            SingletonContainer::Get<ScreenManager>().UnregisterScreenListener(thisListener);
            WLOGFI("JsScreenManager::UnregisterAllScreenListenerWithType success");
        }
        jsCbMap_[type].erase(it++);
    }
    jsCbMap_.erase(type);
}

void UnRegisterScreenListenerWithType(const std::string& type, NativeValue* value)
{
    if (jsCbMap_.empty() || jsCbMap_.find(type) == jsCbMap_.end()) {
        WLOGFI("JsScreenManager::UnRegisterScreenListenerWithType methodName %{public}s not registered!",
            type.c_str());
        return;
    }
    if (value == nullptr) {
        WLOGFE("JsScreenManager::UnRegisterScreenListenerWithType value is nullptr");
        return;
    }
    for (auto it = jsCbMap_[type].begin(); it != jsCbMap_[type].end();) {
        if (value->StrictEquals(it->first->Get())) {
            it->second->RemoveCallback(type, value);
            if (type == EVENT_CONNECT || type == EVENT_DISCONNECT || type == EVENT_CHANGE) {
                sptr<ScreenManager::IScreenListener> thisListener(it->second);
                SingletonContainer::Get<ScreenManager>().UnregisterScreenListener(thisListener);
                WLOGFI("JsScreenManager::UnRegisterScreenListenerWithType success");
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

NativeValue* OnRegisterScreenMangerCallback(NativeEngine& engine, NativeCallbackInfo& info)
{
    WLOGFI("JsScreenManager::OnRegisterScreenMangerCallback is called");
    if (info.argc < ARGC_TWO) {
        WLOGFE("Params not match");
        engine.Throw(CreateJsError(engine, static_cast<int32_t>(DmErrorCode::DM_ERROR_INVALID_PARAM)));
        return engine.CreateUndefined();
    }
    std::string cbType;
    if (!ConvertFromJsValue(engine, info.argv[0], cbType)) {
        WLOGFE("Failed to convert parameter to callbackType");
        engine.Throw(CreateJsError(engine, static_cast<int32_t>(DmErrorCode::DM_ERROR_INVALID_PARAM)));
        return engine.CreateUndefined();
    }
    NativeValue* value = info.argv[INDEX_ONE];
    if (value == nullptr) {
        WLOGFI("JsScreenManager::OnRegisterScreenMangerCallback info->argv[1] is nullptr");
        engine.Throw(CreateJsError(engine, static_cast<int32_t>(DmErrorCode::DM_ERROR_INVALID_PARAM)));
        return engine.CreateUndefined();
    }
    if (!value->IsCallable()) {
        WLOGFI("JsScreenManager::OnRegisterScreenMangerCallback info->argv[1] is not callable");
        engine.Throw(CreateJsError(engine, static_cast<int32_t>(DmErrorCode::DM_ERROR_INVALID_PARAM)));
        return engine.CreateUndefined();
    }
    std::lock_guard<std::mutex> lock(mtx_);
    RegisterScreenListenerWithType(engine, cbType, value);
    return engine.CreateUndefined();
}

NativeValue* OnUnregisterScreenManagerCallback(NativeEngine& engine, NativeCallbackInfo& info)
{
    WLOGFI("JsScreenManager::OnUnregisterScreenCallback is called");
    if (info.argc < ARGC_ONE) {
        WLOGFE("Params not match");
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
        UnregisterAllScreenListenerWithType(cbType);
    } else {
        NativeValue* value = info.argv[INDEX_ONE];
        if ((value == nullptr) || (!value->IsCallable())) {
            UnregisterAllScreenListenerWithType(cbType);
        } else {
            UnRegisterScreenListenerWithType(cbType, value);
        }
    }
    return engine.CreateUndefined();
}

NativeValue* OnMakeMirror(NativeEngine& engine, NativeCallbackInfo& info)
{
    WLOGFI("OnMakeMirror is called");
    if (info.argc < ARGC_TWO) {
        engine.Throw(CreateJsError(engine, static_cast<int32_t>(DmErrorCode::DM_ERROR_INVALID_PARAM)));
        return engine.CreateUndefined();
    }

    int64_t mainScreenId;
    if (!ConvertFromJsValue(engine, info.argv[0], mainScreenId)) {
        engine.Throw(CreateJsError(engine, static_cast<int32_t>(DmErrorCode::DM_ERROR_INVALID_PARAM)));
        return engine.CreateUndefined();
    }
    NativeArray* array = ConvertNativeValueTo<NativeArray>(info.argv[INDEX_ONE]);
    if (array == nullptr) {
        engine.Throw(CreateJsError(engine, static_cast<int32_t>(DmErrorCode::DM_ERROR_INVALID_PARAM)));
        return engine.CreateUndefined();
    }
    uint32_t size = array->GetLength();
    std::vector<ScreenId> screenIds;
    for (uint32_t i = 0; i < size; i++) {
        uint32_t screenId;
        NativeValue* value = array->GetElement(i);
        if (!ConvertFromJsValue(engine, value, screenId)) {
            engine.Throw(CreateJsError(engine, static_cast<int32_t>(DmErrorCode::DM_ERROR_INVALID_PARAM)));
            return engine.CreateUndefined();
        }
        screenIds.emplace_back(static_cast<ScreenId>(screenId));
    }

    AsyncTask::CompleteCallback complete =
        [mainScreenId, screenIds](NativeEngine& engine, AsyncTask& task, int32_t status) {
            ScreenId id = SingletonContainer::Get<ScreenManager>().MakeMirror(mainScreenId, screenIds);
            if (id != SCREEN_ID_INVALID) {
                task.Resolve(engine, CreateJsValue(engine, static_cast<uint32_t>(id)));
            } else {
                task.Reject(engine, CreateJsError(engine,
                    static_cast<int32_t>(DmErrorCode::DM_ERROR_INVALID_SCREEN),
                    "JsScreenManager::OnMakeMirror failed."));
            }
        };
    NativeValue* lastParam = nullptr;
    if (info.argc >= ARGC_THREE && info.argv[ARGC_THREE - 1] != nullptr &&
        info.argv[ARGC_THREE - 1]->TypeOf() == NATIVE_FUNCTION) {
        lastParam = info.argv[ARGC_THREE - 1];
    }
    NativeValue* result = nullptr;
    AsyncTask::Schedule("JsScreenManager::OnMakeMirror",
        engine, CreateAsyncTaskWithLastParam(engine, lastParam, nullptr, std::move(complete), &result));
    return result;
}

NativeValue* OnMakeExpand(NativeEngine& engine, NativeCallbackInfo& info)
{
    WLOGFI("OnMakeExpand is called");
    if (info.argc < ARGC_ONE) {
        WLOGFE("Params not match");
        engine.Throw(CreateJsError(engine, static_cast<int32_t>(DmErrorCode::DM_ERROR_INVALID_PARAM)));
        return engine.CreateUndefined();
    }

    NativeArray* array = ConvertNativeValueTo<NativeArray>(info.argv[0]);
    if (array == nullptr) {
        WLOGFE("Failed to get options");
        engine.Throw(CreateJsError(engine, static_cast<int32_t>(DmErrorCode::DM_ERROR_INVALID_PARAM)));
        return engine.CreateUndefined();
    }
    uint32_t size = array->GetLength();
    std::vector<ExpandOption> options;
    for (uint32_t i = 0; i < size; ++i) {
        NativeObject* object = ConvertNativeValueTo<NativeObject>(array->GetElement(i));
        ExpandOption expandOption;
        int32_t res = GetExpandOptionFromJs(engine, object, expandOption);
        if (res == -1) {
            WLOGE("expandoption param %{public}d error!", i);
            engine.Throw(CreateJsError(engine, static_cast<int32_t>(DmErrorCode::DM_ERROR_INVALID_PARAM)));
            return engine.CreateUndefined();
        }
        options.emplace_back(expandOption);
    }

    AsyncTask::CompleteCallback complete =
        [options](NativeEngine& engine, AsyncTask& task, int32_t status) {
            ScreenId id = SingletonContainer::Get<ScreenManager>().MakeExpand(options);
            if (id != SCREEN_ID_INVALID) {
                task.Resolve(engine, CreateJsValue(engine, static_cast<uint32_t>(id)));
                WLOGFI("MakeExpand success");
            } else {
                task.Reject(engine, CreateJsError(engine,
                    static_cast<int32_t>(DmErrorCode::DM_ERROR_INVALID_SCREEN),
                    "JsScreenManager::OnMakeExpand failed."));
                WLOGFE("MakeExpand failed");
            }
        };
    NativeValue* lastParam = nullptr;
    if (info.argc >= ARGC_TWO && info.argv[ARGC_TWO - 1] != nullptr &&
        info.argv[ARGC_TWO - 1]->TypeOf() == NATIVE_FUNCTION) {
        lastParam = info.argv[ARGC_TWO - 1];
    }
    NativeValue* result = nullptr;
    AsyncTask::Schedule("JsScreenManager::OnMakeExpand",
        engine, CreateAsyncTaskWithLastParam(engine, lastParam, nullptr, std::move(complete), &result));
    return result;
}

static int32_t GetExpandOptionFromJs(NativeEngine& engine, NativeObject* optionObject, ExpandOption& option)
{
    NativeValue* screedIdValue = optionObject->GetProperty("screenId");
    NativeValue* startXValue = optionObject->GetProperty("startX");
    NativeValue* startYValue = optionObject->GetProperty("startY");
    uint32_t screenId;
    uint32_t startX;
    uint32_t startY;
    if (!ConvertFromJsValue(engine, screedIdValue, screenId)) {
        WLOGFE("Failed to convert screedIdValue to callbackType");
        return -1;
    }
    if (!ConvertFromJsValue(engine, startXValue, startX)) {
        WLOGFE("Failed to convert startXValue to callbackType");
        return -1;
    }
    if (!ConvertFromJsValue(engine, startYValue, startY)) {
        WLOGFE("Failed to convert startYValue to callbackType");
        return -1;
    }
    option = {screenId, startX, startY};
    return 0;
}

NativeValue* OnCreateVirtualScreen(NativeEngine& engine, NativeCallbackInfo& info)
{
    WLOGFI("JsScreenManager::OnCreateVirtualScreen is called");
    DmErrorCode errCode = DmErrorCode::DM_OK;
    VirtualScreenOption option;
    if (info.argc < ARGC_ONE) {
        WLOGFE("[NAPI]Argc is invalid: %{public}zu", info.argc);
        errCode = DmErrorCode::DM_ERROR_INVALID_PARAM;
    } else {
        NativeObject* object = ConvertNativeValueTo<NativeObject>(info.argv[0]);
        if (object == nullptr) {
            WLOGFE("Failed to convert parameter to VirtualScreenOption.");
            errCode = DmErrorCode::DM_ERROR_INVALID_PARAM;
        } else {
            errCode = GetVirtualScreenOptionFromJs(engine, object, option);
        }
    }
    if (errCode == DmErrorCode::DM_ERROR_INVALID_PARAM) {
        engine.Throw(CreateJsError(engine, static_cast<int32_t>(DmErrorCode::DM_ERROR_INVALID_PARAM)));
        return engine.CreateUndefined();
    }
    AsyncTask::CompleteCallback complete =
        [option](NativeEngine& engine, AsyncTask& task, int32_t status) {
            auto screenId = SingletonContainer::Get<ScreenManager>().CreateVirtualScreen(option);
            auto screen = SingletonContainer::Get<ScreenManager>().GetScreenById(screenId);
            if (screen == nullptr) {
                task.Reject(engine, CreateJsError(engine,
                    static_cast<int32_t>(DmErrorCode::DM_ERROR_INVALID_SCREEN),
                    "ScreenManager::CreateVirtualScreen failed."));
                WLOGFE("ScreenManager::CreateVirtualScreen failed.");
                return;
            }
            task.Resolve(engine, CreateJsScreenObject(engine, screen));
            WLOGFI("JsScreenManager::OnCreateVirtualScreen success");
        };
    NativeValue* lastParam = nullptr;
    if (info.argc >= ARGC_TWO && info.argv[ARGC_TWO - 1] != nullptr &&
        info.argv[ARGC_TWO - 1]->TypeOf() == NATIVE_FUNCTION) {
        lastParam = info.argv[ARGC_TWO - 1];
    }
    NativeValue* result = nullptr;
    AsyncTask::Schedule("JsScreenManager::OnCreateVirtualScreen",
        engine, CreateAsyncTaskWithLastParam(engine, lastParam, nullptr, std::move(complete), &result));
    return result;
}

DmErrorCode GetVirtualScreenOptionFromJs(NativeEngine& engine, NativeObject* optionObject, VirtualScreenOption& option)
{
    NativeValue* name = optionObject->GetProperty("name");
    if (!ConvertFromJsValue(engine, name, option.name_)) {
        WLOGFE("Failed to convert parameter to name.");
        return DmErrorCode::DM_ERROR_INVALID_PARAM;
    }
    NativeValue* width = optionObject->GetProperty("width");
    if (!ConvertFromJsValue(engine, width, option.width_)) {
        WLOGFE("Failed to convert parameter to width.");
        return DmErrorCode::DM_ERROR_INVALID_PARAM;
    }
    NativeValue* height = optionObject->GetProperty("height");
    if (!ConvertFromJsValue(engine, height, option.height_)) {
        WLOGFE("Failed to convert parameter to height.");
        return DmErrorCode::DM_ERROR_INVALID_PARAM;
    }
    NativeValue* density = optionObject->GetProperty("density");
    double densityValue;
    if (!ConvertFromJsValue(engine, density, densityValue)) {
        WLOGFE("Failed to convert parameter to density.");
        return DmErrorCode::DM_ERROR_INVALID_PARAM;
    }
    option.density_ = static_cast<float>(densityValue);

    NativeValue* surfaceIdNativeValue = optionObject->GetProperty("surfaceId");
    if (!GetSurfaceFromJs(engine, surfaceIdNativeValue, option.surface_)) {
        return DmErrorCode::DM_ERROR_INVALID_PARAM;
    }
    return DmErrorCode::DM_OK;
}

bool GetSurfaceFromJs(NativeEngine& engine, NativeValue* surfaceIdNativeValue, sptr<Surface>& surface)
{
    if (surfaceIdNativeValue == nullptr || surfaceIdNativeValue->TypeOf() != NATIVE_STRING) {
        WLOGFE("Failed to convert parameter to surface. Invalidate params.");
        return false;
    }
    char buffer[PATH_MAX];
    size_t length = 0;
    uint64_t surfaceId = 0;
    napi_env env = reinterpret_cast<napi_env>(&engine);
    napi_value surfaceIdValue = reinterpret_cast<napi_value>(surfaceIdNativeValue);
    if (napi_get_value_string_utf8(env, surfaceIdValue, buffer, PATH_MAX, &length) != napi_ok) {
        WLOGFE("Failed to convert parameter to surface.");
        return false;
    }
    std::istringstream inputStream(buffer);
    inputStream >> surfaceId;
    surface = SurfaceUtils::GetInstance()->GetSurface(surfaceId);
    if (surface == nullptr) {
        WLOGFI("GetSurfaceFromJs failed, surfaceId:%{public}" PRIu64"", surfaceId);
    }
    return true;
}

NativeValue* OnDestroyVirtualScreen(NativeEngine& engine, NativeCallbackInfo& info)
{
    WLOGFI("JsScreenManager::OnDestroyVirtualScreen is called");
    DmErrorCode errCode = DmErrorCode::DM_OK;
    int64_t screenId = -1LL;
    if (info.argc < ARGC_ONE) {
        WLOGFE("[NAPI]Argc is invalid: %{public}zu", info.argc);
        errCode = DmErrorCode::DM_ERROR_INVALID_PARAM;
    } else {
        if (!ConvertFromJsValue(engine, info.argv[0], screenId)) {
            WLOGFE("Failed to convert parameter to screen id.");
            errCode = DmErrorCode::DM_ERROR_INVALID_PARAM;
        }
    }
    if (errCode == DmErrorCode::DM_ERROR_INVALID_PARAM || screenId == -1LL) {
        WLOGFE("JsScreenManager::OnDestroyVirtualScreen failed, Invalidate params.");
        engine.Throw(CreateJsError(engine, static_cast<int32_t>(DmErrorCode::DM_ERROR_INVALID_PARAM)));
        return engine.CreateUndefined();
    }
    AsyncTask::CompleteCallback complete =
        [screenId](NativeEngine& engine, AsyncTask& task, int32_t status) {
            auto res = DM_JS_TO_ERROR_CODE_MAP.at(
                SingletonContainer::Get<ScreenManager>().DestroyVirtualScreen(screenId));
            if (res != DmErrorCode::DM_OK) {
                task.Reject(engine, CreateJsError(engine, static_cast<int32_t>(res),
                    "ScreenManager::DestroyVirtualScreen failed."));
                WLOGFE("ScreenManager::DestroyVirtualScreen failed.");
                return;
            }
            task.Resolve(engine, engine.CreateUndefined());
            WLOGFI("JsScreenManager::OnDestroyVirtualScreen success");
        };
    NativeValue* lastParam = nullptr;
    if (info.argc >= ARGC_TWO && info.argv[ARGC_TWO - 1] != nullptr &&
        info.argv[ARGC_TWO - 1]->TypeOf() == NATIVE_FUNCTION) {
        lastParam = info.argv[ARGC_TWO - 1];
    }
    NativeValue* result = nullptr;
    AsyncTask::Schedule("JsScreenManager::OnDestroyVirtualScreen",
        engine, CreateAsyncTaskWithLastParam(engine, lastParam, nullptr, std::move(complete), &result));
    return result;
}

NativeValue* OnSetVirtualScreenSurface(NativeEngine& engine, NativeCallbackInfo& info)
{
    WLOGFI("JsScreenManager::OnSetVirtualScreenSurface is called");
    DmErrorCode errCode = DmErrorCode::DM_OK;
    int64_t screenId = -1LL;
    sptr<Surface> surface;
    if (info.argc < ARGC_TWO) {
        WLOGFE("[NAPI]Argc is invalid: %{public}zu", info.argc);
        errCode = DmErrorCode::DM_ERROR_INVALID_PARAM;
    } else {
        if (!ConvertFromJsValue(engine, info.argv[0], screenId)) {
            WLOGFE("Failed to convert parameter to screen id.");
            errCode = DmErrorCode::DM_ERROR_INVALID_PARAM;
        }
        if (!GetSurfaceFromJs(engine, info.argv[1], surface)) {
            WLOGFE("Failed to convert parameter to surface");
            errCode = DmErrorCode::DM_ERROR_INVALID_PARAM;
        }
    }
    if (errCode == DmErrorCode::DM_ERROR_INVALID_PARAM || surface == nullptr) {
        WLOGFE("JsScreenManager::OnSetVirtualScreenSurface failed, Invalidate params.");
        engine.Throw(CreateJsError(engine, static_cast<int32_t>(DmErrorCode::DM_ERROR_INVALID_PARAM)));
        return engine.CreateUndefined();
    }
    AsyncTask::CompleteCallback complete =
        [screenId, surface](NativeEngine& engine, AsyncTask& task, int32_t status) {
            auto res = DM_JS_TO_ERROR_CODE_MAP.at(
                SingletonContainer::Get<ScreenManager>().SetVirtualScreenSurface(screenId, surface));
            if (res != DmErrorCode::DM_OK) {
                task.Reject(engine, CreateJsError(engine, static_cast<int32_t>(res),
                    "ScreenManager::SetVirtualScreenSurface failed."));
                WLOGFE("ScreenManager::SetVirtualScreenSurface failed.");
                return;
            }
            task.Resolve(engine, engine.CreateUndefined());
            WLOGFI("JsScreenManager::OnSetVirtualScreenSurface success");
        };
    NativeValue* lastParam = nullptr;
    if (info.argc >= ARGC_THREE && info.argv[ARGC_THREE - 1] != nullptr &&
        info.argv[ARGC_THREE - 1]->TypeOf() == NATIVE_FUNCTION) {
        lastParam = info.argv[ARGC_THREE - 1];
    }
    NativeValue* result = nullptr;
    AsyncTask::Schedule("JsScreenManager::OnSetVirtualScreenSurface",
        engine, CreateAsyncTaskWithLastParam(engine, lastParam, nullptr, std::move(complete), &result));
    return result;
}

NativeValue* OnIsScreenRotationLocked(NativeEngine& engine, NativeCallbackInfo& info)
{
    WLOGFI("OnIsScreenRotationLocked is called");
    AsyncTask::CompleteCallback complete =
        [](NativeEngine& engine, AsyncTask& task, int32_t status) {
            bool isLocked = SingletonContainer::Get<ScreenManager>().IsScreenRotationLocked();
            task.Resolve(engine, CreateJsValue(engine, isLocked));
        };
    NativeValue* lastParam = nullptr;
    if (info.argc >= ARGC_ONE && info.argv[ARGC_ONE - 1] != nullptr &&
        info.argv[ARGC_ONE - 1]->TypeOf() == NATIVE_FUNCTION) {
        lastParam = info.argv[ARGC_ONE - 1];
    }
    NativeValue* result = nullptr;
    AsyncTask::Schedule("JsScreenManager::OnIsScreenRotationLocked",
        engine, CreateAsyncTaskWithLastParam(engine, lastParam, nullptr, std::move(complete), &result));
    return result;
}

NativeValue* OnSetScreenRotationLocked(NativeEngine& engine, NativeCallbackInfo& info)
{
    WLOGFI("JsScreenManager::OnSetScreenRotationLocked is called");
    DmErrorCode errCode = DmErrorCode::DM_OK;
    if (info.argc < ARGC_ONE) {
        WLOGFE("[NAPI]Argc is invalid: %{public}zu", info.argc);
        errCode = DmErrorCode::DM_ERROR_INVALID_PARAM;
    }
    bool isLocked = false;
    if (errCode == DmErrorCode::DM_OK) {
        NativeBoolean* nativeVal = ConvertNativeValueTo<NativeBoolean>(info.argv[0]);
        if (nativeVal == nullptr) {
            WLOGFE("[NAPI]Failed to convert parameter to isLocked");
            errCode = DmErrorCode::DM_ERROR_INVALID_PARAM;
        } else {
            isLocked = static_cast<bool>(*nativeVal);
        }
    }
    if (errCode == DmErrorCode::DM_ERROR_INVALID_PARAM) {
        WLOGFE("JsScreenManager::OnSetScreenRotationLocked failed, Invalidate params.");
        engine.Throw(CreateJsError(engine, static_cast<int32_t>(DmErrorCode::DM_ERROR_INVALID_PARAM)));
        return engine.CreateUndefined();
    }

    AsyncTask::CompleteCallback complete =
        [isLocked](NativeEngine& engine, AsyncTask& task, int32_t status) {
            SingletonContainer::Get<ScreenManager>().SetScreenRotationLocked(isLocked);
            task.Resolve(engine, engine.CreateUndefined());
        };
    NativeValue* lastParam = (info.argc <= ARGC_ONE) ? nullptr :
        ((info.argv[ARGC_TWO - 1] != nullptr && info.argv[ARGC_TWO - 1]->TypeOf() == NATIVE_FUNCTION) ?
        info.argv[1] : nullptr);
    NativeValue* result = nullptr;
    AsyncTask::Schedule("JsScreenManager::OnSetScreenRotationLocked",
        engine, CreateAsyncTaskWithLastParam(engine, lastParam, nullptr, std::move(complete), &result));
    return result;
}
};

NativeValue* InitScreenOrientation(NativeEngine* engine)
{
    WLOGFI("JsScreenManager::InitScreenOrientation called");

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

    object->SetProperty("UNSPECIFIED", CreateJsValue(*engine, static_cast<int32_t>(Orientation::UNSPECIFIED)));
    object->SetProperty("VERTICAL", CreateJsValue(*engine, static_cast<int32_t>(Orientation::VERTICAL)));
    object->SetProperty("HORIZONTAL", CreateJsValue(*engine, static_cast<int32_t>(Orientation::HORIZONTAL)));
    object->SetProperty("REVERSE_VERTICAL",
        CreateJsValue(*engine, static_cast<int32_t>(Orientation::REVERSE_VERTICAL)));
    object->SetProperty("REVERSE_HORIZONTAL",
        CreateJsValue(*engine, static_cast<int32_t>(Orientation::REVERSE_HORIZONTAL)));
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

NativeValue* JsScreenManagerInit(NativeEngine* engine, NativeValue* exportObj)
{
    WLOGFI("JsScreenManagerInit is called");

    if (engine == nullptr || exportObj == nullptr) {
        WLOGFE("JsScreenManagerInit engine or exportObj is nullptr");
        return nullptr;
    }

    NativeObject* object = ConvertNativeValueTo<NativeObject>(exportObj);
    if (object == nullptr) {
        WLOGFE("JsScreenManagerInit object is nullptr");
        return nullptr;
    }

    std::unique_ptr<JsScreenManager> jsScreenManager = std::make_unique<JsScreenManager>(engine);
    object->SetNativePointer(jsScreenManager.release(), JsScreenManager::Finalizer, nullptr);

    object->SetProperty("Orientation", InitScreenOrientation(engine));
    object->SetProperty("DmErrorCode", InitDisplayErrorCode(engine));
    object->SetProperty("DMError", InitDisplayError(engine));

    const char *moduleName = "JsScreenManager";
    BindNativeFunction(*engine, *object, "getAllScreens", moduleName, JsScreenManager::GetAllScreens);
    BindNativeFunction(*engine, *object, "on", moduleName, JsScreenManager::RegisterScreenManagerCallback);
    BindNativeFunction(*engine, *object, "off", moduleName, JsScreenManager::UnregisterScreenMangerCallback);
    BindNativeFunction(*engine, *object, "makeMirror", moduleName, JsScreenManager::MakeMirror);
    BindNativeFunction(*engine, *object, "makeExpand", moduleName, JsScreenManager::MakeExpand);
    BindNativeFunction(*engine, *object, "createVirtualScreen", moduleName, JsScreenManager::CreateVirtualScreen);
    BindNativeFunction(*engine, *object, "destroyVirtualScreen", moduleName, JsScreenManager::DestroyVirtualScreen);
    BindNativeFunction(*engine, *object, "setVirtualScreenSurface", moduleName,
        JsScreenManager::SetVirtualScreenSurface);
    BindNativeFunction(*engine, *object, "setScreenRotationLocked", moduleName,
        JsScreenManager::SetScreenRotationLocked);
    BindNativeFunction(*engine, *object, "isScreenRotationLocked", moduleName,
        JsScreenManager::IsScreenRotationLocked);
    return engine->CreateUndefined();
}
}  // namespace Rosen
}  // namespace OHOS