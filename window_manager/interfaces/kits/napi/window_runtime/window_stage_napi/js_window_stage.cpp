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

#include "js_window_stage.h"
#include <string>
#include "js_runtime_utils.h"
#include "js_window.h"
#include "js_window_register_manager.h"
#include "js_window_utils.h"
#include "window_manager_hilog.h"
#include "permission.h"

namespace OHOS {
namespace Rosen {
using namespace AbilityRuntime;
namespace {
const int CONTENT_STORAGE_ARG = 2;
constexpr HiviewDFX::HiLogLabel LABEL = {LOG_CORE, HILOG_DOMAIN_WINDOW, "JsWindowStage"};
} // namespace

std::unique_ptr<JsWindowRegisterManager> g_listenerManager = std::make_unique<JsWindowRegisterManager>();
JsWindowStage::JsWindowStage(const std::shared_ptr<Rosen::WindowScene>& windowScene)
    : windowScene_(windowScene)
{
}

JsWindowStage::~JsWindowStage()
{
}

void JsWindowStage::Finalizer(NativeEngine* engine, void* data, void* hint)
{
    WLOGFI("[NAPI]Finalizer");
    std::unique_ptr<JsWindowStage>(static_cast<JsWindowStage*>(data));
}

NativeValue* JsWindowStage::SetUIContent(NativeEngine* engine, NativeCallbackInfo* info)
{
    WLOGFD("[NAPI]SetUIContent");
    JsWindowStage* me = CheckParamsAndGetThis<JsWindowStage>(engine, info);
    return (me != nullptr) ? me->OnSetUIContent(*engine, *info) : nullptr;
}

NativeValue* JsWindowStage::GetMainWindow(NativeEngine* engine, NativeCallbackInfo* info)
{
    WLOGFD("[NAPI]GetMainWindow");
    JsWindowStage* me = CheckParamsAndGetThis<JsWindowStage>(engine, info);
    return (me != nullptr) ? me->OnGetMainWindow(*engine, *info) : nullptr;
}

NativeValue* JsWindowStage::GetMainWindowSync(NativeEngine* engine, NativeCallbackInfo* info)
{
    WLOGFD("[NAPI]GetMainWindowSync");
    JsWindowStage* me = CheckParamsAndGetThis<JsWindowStage>(engine, info);
    return (me != nullptr) ? me->OnGetMainWindowSync(*engine, *info) : nullptr;
}

NativeValue* JsWindowStage::On(NativeEngine* engine, NativeCallbackInfo* info)
{
    WLOGFD("[NAPI]On");
    JsWindowStage* me = CheckParamsAndGetThis<JsWindowStage>(engine, info);
    return (me != nullptr) ? me->OnEvent(*engine, *info) : nullptr;
}

NativeValue* JsWindowStage::Off(NativeEngine* engine, NativeCallbackInfo* info)
{
    WLOGFD("[NAPI]Off");
    JsWindowStage* me = CheckParamsAndGetThis<JsWindowStage>(engine, info);
    return (me != nullptr) ? me->OffEvent(*engine, *info) : nullptr;
}

NativeValue* JsWindowStage::LoadContent(NativeEngine* engine, NativeCallbackInfo* info)
{
    WLOGFD("[NAPI]LoadContent");
    JsWindowStage* me = CheckParamsAndGetThis<JsWindowStage>(engine, info);
    return (me != nullptr) ? me->OnLoadContent(*engine, *info) : nullptr;
}

NativeValue* JsWindowStage::GetWindowMode(NativeEngine* engine, NativeCallbackInfo* info)
{
    WLOGFD("[NAPI]GetWindowMode");
    JsWindowStage* me = CheckParamsAndGetThis<JsWindowStage>(engine, info);
    return (me != nullptr) ? me->OnGetWindowMode(*engine, *info) : nullptr;
}

NativeValue* JsWindowStage::CreateSubWindow(NativeEngine* engine, NativeCallbackInfo* info)
{
    WLOGFD("[NAPI]CreateSubWindow");
    JsWindowStage* me = CheckParamsAndGetThis<JsWindowStage>(engine, info);
    return (me != nullptr) ? me->OnCreateSubWindow(*engine, *info) : nullptr;
}

NativeValue* JsWindowStage::GetSubWindow(NativeEngine* engine, NativeCallbackInfo* info)
{
    WLOGFD("[NAPI]GetSubWindow");
    JsWindowStage* me = CheckParamsAndGetThis<JsWindowStage>(engine, info);
    return (me != nullptr) ? me->OnGetSubWindow(*engine, *info) : nullptr;
}

NativeValue* JsWindowStage::SetShowOnLockScreen(NativeEngine* engine, NativeCallbackInfo* info)
{
    WLOGFD("[NAPI]SetShowOnLockScreen");
    JsWindowStage* me = CheckParamsAndGetThis<JsWindowStage>(engine, info);
    return (me != nullptr) ? me->OnSetShowOnLockScreen(*engine, *info) : nullptr;
}

NativeValue* JsWindowStage::DisableWindowDecor(NativeEngine* engine, NativeCallbackInfo* info)
{
    WLOGFD("[NAPI]DisableWindowDecor");
    JsWindowStage* me = CheckParamsAndGetThis<JsWindowStage>(engine, info);
    return (me != nullptr) ? me->OnDisableWindowDecor(*engine, *info) : nullptr;
}

NativeValue* JsWindowStage::OnSetUIContent(NativeEngine& engine, NativeCallbackInfo& info)
{
    if (info.argc < 2) { // 2: minimum param num
        WLOGFE("[NAPI]Argc is invalid: %{public}zu", info.argc);
        return engine.CreateUndefined();
    }
    auto weakScene = windowScene_.lock();
    if (weakScene == nullptr || weakScene->GetMainWindow() == nullptr) {
        WLOGFE("[NAPI]WindowScene is null or window is null");
        return engine.CreateUndefined();
    }

    // Parse info->argv[0] as abilitycontext
    auto objContext = ConvertNativeValueTo<NativeObject>(info.argv[0]);
    if (objContext == nullptr) {
        WLOGFE("[NAPI]Context is nullptr");
        return engine.CreateUndefined();
    }

    // Parse info->argv[1] as url
    std::string contextUrl;
    if (!ConvertFromJsValue(engine, info.argv[1], contextUrl)) {
        WLOGFE("[NAPI]Failed to convert parameter to url");
        return engine.CreateUndefined();
    }
    weakScene->GetMainWindow()->SetUIContent(contextUrl, &engine, info.argv[CONTENT_STORAGE_ARG]);
    return engine.CreateUndefined();
}

NativeValue* JsWindowStage::OnGetMainWindow(NativeEngine& engine, NativeCallbackInfo& info)
{
    AsyncTask::CompleteCallback complete =
        [weak = windowScene_](NativeEngine& engine, AsyncTask& task, int32_t status) {
            auto weakScene = weak.lock();
            if (weakScene == nullptr) {
                task.Reject(engine, CreateJsError(engine,
                    static_cast<int32_t>(WmErrorCode::WM_ERROR_STAGE_ABNORMALLY)));
                WLOGFE("[NAPI]WindowScene_ is nullptr!");
                return;
            }
            auto window = weakScene->GetMainWindow();
            if (window != nullptr) {
                task.Resolve(engine, OHOS::Rosen::CreateJsWindowObject(engine, window));
                WLOGFI("[NAPI]Get main window [%{public}u, %{public}s]",
                    window->GetWindowId(), window->GetWindowName().c_str());
            } else {
                task.Reject(engine, CreateJsError(engine,
                    static_cast<int32_t>(WmErrorCode::WM_ERROR_STATE_ABNORMALLY),
                    "Get main window failed."));
            }
        };
    NativeValue* callback = (info.argv[0] != nullptr && info.argv[0]->TypeOf() == NATIVE_FUNCTION) ?
        info.argv[0] : nullptr;
    NativeValue* result = nullptr;
    AsyncTask::Schedule("JsWindowStage::OnGetMainWindow",
        engine, CreateAsyncTaskWithLastParam(engine, callback, nullptr, std::move(complete), &result));
    return result;
}

NativeValue* JsWindowStage::OnGetMainWindowSync(NativeEngine& engine, NativeCallbackInfo& info)
{
    auto weakScene = windowScene_.lock();
    if (weakScene == nullptr) {
        WLOGFE("[NAPI]WindowScene is null");
        engine.Throw(CreateJsError(engine, static_cast<int32_t>(WmErrorCode::WM_ERROR_STAGE_ABNORMALLY)));
        return engine.CreateUndefined();
    }
    auto window = weakScene->GetMainWindow();
    if (window == nullptr) {
        WLOGFE("[NAPI]window is null");
        engine.Throw(CreateJsError(engine, static_cast<int32_t>(WmErrorCode::WM_ERROR_STATE_ABNORMALLY)));
        return engine.CreateUndefined();
    }

    return OHOS::Rosen::CreateJsWindowObject(engine, window);
}


NativeValue* JsWindowStage::OnEvent(NativeEngine& engine, NativeCallbackInfo& info)
{
    auto weakScene = windowScene_.lock();
    if (weakScene == nullptr) {
        WLOGFE("[NAPI]Window scene is null");
        engine.Throw(CreateJsError(engine, static_cast<int32_t>(WmErrorCode::WM_ERROR_STATE_ABNORMALLY)));
        return engine.CreateUndefined();
    }
    if (info.argc < 2) { // 2: minimum param nums
        WLOGFE("[NAPI]argc is invalid: %{public}zu", info.argc);
        engine.Throw(CreateJsError(engine, static_cast<int32_t>(WmErrorCode::WM_ERROR_INVALID_PARAM)));
        return engine.CreateUndefined();
    }

    // Parse info->argv[0] as string
    std::string eventString;
    if (!ConvertFromJsValue(engine, info.argv[0], eventString)) {
        WLOGFE("[NAPI]Failed to convert parameter to string");
        engine.Throw(CreateJsError(engine, static_cast<int32_t>(WmErrorCode::WM_ERROR_INVALID_PARAM)));
        return engine.CreateUndefined();
    }
    NativeValue* value = info.argv[1];
    if (!value->IsCallable()) {
        WLOGFE("[NAPI]Callback(info->argv[1]) is not callable");
        engine.Throw(CreateJsError(engine, static_cast<int32_t>(WmErrorCode::WM_ERROR_INVALID_PARAM)));
        return engine.CreateUndefined();
    }

    auto window = weakScene->GetMainWindow();
    if (window == nullptr) {
        WLOGFE("[NAPI]Get window failed");
        engine.Throw(CreateJsError(engine, static_cast<int32_t>(WmErrorCode::WM_ERROR_STATE_ABNORMALLY)));
        return engine.CreateUndefined();
    }
    g_listenerManager->RegisterListener(window, eventString, CaseType::CASE_STAGE, engine, value);
    WLOGFI("[NAPI]Window [%{public}u, %{public}s] register event %{public}s, callback %{public}p",
        window->GetWindowId(), window->GetWindowName().c_str(), eventString.c_str(), value);

    return engine.CreateUndefined();
}

NativeValue* JsWindowStage::OffEvent(NativeEngine& engine, NativeCallbackInfo& info)
{
    auto weakScene = windowScene_.lock();
    if (weakScene == nullptr) {
        WLOGFE("[NAPI]Window scene is null");
        engine.Throw(CreateJsError(engine, static_cast<int32_t>(WmErrorCode::WM_ERROR_STATE_ABNORMALLY)));
        return engine.CreateUndefined();
    }

    // Parse info->argv[0] as string
    std::string eventString;
    if (!ConvertFromJsValue(engine, info.argv[0], eventString)) {
        WLOGFE("[NAPI]Failed to convert parameter to string");
        engine.Throw(CreateJsError(engine, static_cast<int32_t>(WmErrorCode::WM_ERROR_INVALID_PARAM)));
        return engine.CreateUndefined();
    }
    if (eventString.compare("windowStageEvent") != 0) {
        WLOGFE("[NAPI]Envent %{public}s is invalid", eventString.c_str());
        engine.Throw(CreateJsError(engine, static_cast<int32_t>(WmErrorCode::WM_ERROR_INVALID_PARAM)));
        return engine.CreateUndefined();
    }

    auto window = weakScene->GetMainWindow();
    if (window == nullptr) {
        WLOGFE("[NAPI]Get window failed");
        engine.Throw(CreateJsError(engine, static_cast<int32_t>(WmErrorCode::WM_ERROR_STATE_ABNORMALLY)));
        return engine.CreateUndefined();
    }
    NativeValue* value = nullptr;
    if (info.argv[1] == nullptr || info.argv[1]->TypeOf() != NATIVE_FUNCTION) {
        g_listenerManager->UnregisterListener(window, eventString, CaseType::CASE_STAGE, nullptr);
    } else {
        g_listenerManager->UnregisterListener(window, eventString, CaseType::CASE_STAGE, info.argv[1]);
    }

    if (info.argc == 1) {
        g_listenerManager->UnregisterListener(window, eventString, CaseType::CASE_STAGE, nullptr);
    } else {
        value = info.argv[1];
        if (value != nullptr && value->TypeOf() == NATIVE_FUNCTION) {
            g_listenerManager->UnregisterListener(window, eventString, CaseType::CASE_STAGE, value);
        } else {
            g_listenerManager->UnregisterListener(window, eventString, CaseType::CASE_STAGE, nullptr);
        }
    }
    WLOGFI("[NAPI]Window [%{public}u, %{public}s] unregister event %{public}s, callback %{public}p",
        window->GetWindowId(), window->GetWindowName().c_str(), eventString.c_str(), value);

    return engine.CreateUndefined();
}

static void LoadContentTask(std::shared_ptr<NativeReference> contentStorage, std::string contextUrl,
    sptr<Window> weakWindow, NativeEngine& engine, AsyncTask& task)
{
    NativeValue* nativeStorage =  (contentStorage == nullptr) ? nullptr : contentStorage->Get();
    WMError ret = weakWindow->SetUIContent(contextUrl, &engine, nativeStorage, false);
    if (ret == WMError::WM_OK) {
        task.Resolve(engine, engine.CreateUndefined());
    } else {
        task.Reject(engine, CreateJsError(engine, static_cast<int32_t>(ret), "Window load content failed"));
    }
    WLOGFI("[NAPI]Window [%{public}u, %{public}s] load content end, ret = %{public}d",
        weakWindow->GetWindowId(), weakWindow->GetWindowName().c_str(), ret);
    return;
}

NativeValue* JsWindowStage::OnLoadContent(NativeEngine& engine, NativeCallbackInfo& info)
{
    WmErrorCode errCode = WmErrorCode::WM_OK;
    std::string contextUrl;
    if (!ConvertFromJsValue(engine, info.argv[0], contextUrl)) {
        WLOGFE("[NAPI]Failed to convert parameter to context url");
        errCode = WmErrorCode::WM_ERROR_INVALID_PARAM;
    }
    NativeValue* storage = nullptr;
    NativeValue* callBack = nullptr;
    NativeValue* value1 = info.argv[1];
    NativeValue* value2 = info.argv[2]; // 2: param index
    if (value1->TypeOf() == NATIVE_FUNCTION) {
        callBack = value1;
    } else if (value1->TypeOf() == NATIVE_OBJECT) {
        storage = value1;
    }
    if (value2->TypeOf() == NATIVE_FUNCTION) {
        callBack = value2;
    }
    if (errCode == WmErrorCode::WM_ERROR_INVALID_PARAM) {
        WLOGFE("[NAPI]Window scene is null or get invalid param");
        engine.Throw(CreateJsError(engine, static_cast<int32_t>(WmErrorCode::WM_ERROR_INVALID_PARAM)));
        return engine.CreateUndefined();
    }
    std::shared_ptr<NativeReference> contentStorage = (storage == nullptr) ? nullptr :
        std::shared_ptr<NativeReference>(engine.CreateReference(storage, 1));
    AsyncTask::CompleteCallback complete =
        [weak = windowScene_, contentStorage, contextUrl, errCode](
            NativeEngine& engine, AsyncTask& task, int32_t status) {
            auto weakScene = weak.lock();
            if (weakScene == nullptr) {
                WLOGFE("[NAPI]Window scene is null");
                task.Reject(engine, CreateJsError(engine,
                    static_cast<int32_t>(WmErrorCode::WM_ERROR_STAGE_ABNORMALLY)));
                return;
            }
            auto win = weakScene->GetMainWindow();
            if (win == nullptr) {
                task.Reject(engine, CreateJsError(engine,
                    static_cast<int32_t>(WmErrorCode::WM_ERROR_STATE_ABNORMALLY)));
                WLOGFE("[NAPI]Get window failed");
                return;
            }
            LoadContentTask(contentStorage, contextUrl, win, engine, task);
        };
    NativeValue* result = nullptr;
    AsyncTask::Schedule("JsWindowStage::OnLoadContent",
        engine, CreateAsyncTaskWithLastParam(engine, callBack, nullptr, std::move(complete), &result));
    return result;
}

NativeValue* JsWindowStage::OnGetWindowMode(NativeEngine& engine, NativeCallbackInfo& info)
{
    AsyncTask::CompleteCallback complete =
        [weak = windowScene_](NativeEngine& engine, AsyncTask& task, int32_t status) {
            auto weakScene = weak.lock();
            if (weakScene == nullptr) {
                task.Reject(engine, CreateJsError(engine, static_cast<int32_t>(WMError::WM_ERROR_NULLPTR)));
                WLOGFE("[NAPI]windowScene_ is nullptr");
                return;
            }
            auto window = weakScene->GetMainWindow();
            if (window == nullptr) {
                task.Reject(engine, CreateJsError(engine, static_cast<int32_t>(Rosen::WMError::WM_ERROR_NULLPTR),
                    "Get window failed"));
                WLOGFE("[NAPI]Get window failed");
                return;
            }
            Rosen::WindowMode mode = window->GetMode();
            if (NATIVE_TO_JS_WINDOW_MODE_MAP.count(mode) != 0) {
                task.Resolve(engine, CreateJsValue(engine, NATIVE_TO_JS_WINDOW_MODE_MAP.at(mode)));
                WLOGFI("[NAPI]Window [%{public}u, %{public}s] get mode %{public}u, api mode %{public}u",
                    window->GetWindowId(), window->GetWindowName().c_str(),
                    mode, NATIVE_TO_JS_WINDOW_MODE_MAP.at(mode));
            } else {
                task.Resolve(engine, CreateJsValue(engine, mode));
                WLOGFE("[NAPI]Get mode %{public}u, but not in apimode", mode);
            }
        };
    NativeValue* callback = (info.argv[0]->TypeOf() == NATIVE_FUNCTION) ? info.argv[0] : nullptr;
    NativeValue* result = nullptr;
    AsyncTask::Schedule("JsWindowStage::OnGetWindowMode",
        engine, CreateAsyncTaskWithLastParam(engine, callback, nullptr, std::move(complete), &result));
    return result;
}

NativeValue* JsWindowStage::OnCreateSubWindow(NativeEngine& engine, NativeCallbackInfo& info)
{
    WmErrorCode errCode = WmErrorCode::WM_OK;
    std::string windowName;
    if (!ConvertFromJsValue(engine, info.argv[0], windowName)) {
        WLOGFE("[NAPI]Failed to convert parameter to windowName");
        errCode = WmErrorCode::WM_ERROR_INVALID_PARAM;
    }
    if (errCode == WmErrorCode::WM_ERROR_INVALID_PARAM) {
        WLOGFE("[NAPI]get invalid param");
        engine.Throw(CreateJsError(engine, static_cast<int32_t>(WmErrorCode::WM_ERROR_INVALID_PARAM)));
        return engine.CreateUndefined();
    }
    AsyncTask::CompleteCallback complete =
        [weak = windowScene_, windowName](NativeEngine& engine, AsyncTask& task, int32_t status) {
            auto weakScene = weak.lock();
            if (weakScene == nullptr) {
                WLOGFE("[NAPI]Window scene is null");
                task.Reject(engine, CreateJsError(engine,
                    static_cast<int32_t>(WmErrorCode::WM_ERROR_STATE_ABNORMALLY)));
                return;
            }
            sptr<Rosen::WindowOption> windowOption = new Rosen::WindowOption();
            windowOption->SetWindowType(Rosen::WindowType::WINDOW_TYPE_APP_SUB_WINDOW);
            windowOption->SetWindowMode(Rosen::WindowMode::WINDOW_MODE_FLOATING);
            auto window = weakScene->CreateWindow(windowName, windowOption);
            if (window == nullptr) {
                WLOGFE("[NAPI]Get window failed");
                task.Reject(engine, CreateJsError(engine,
                    static_cast<int32_t>(WmErrorCode::WM_ERROR_STATE_ABNORMALLY), "Get window failed"));
                return;
            }
            task.Resolve(engine, CreateJsWindowObject(engine, window));
            WLOGFI("[NAPI]Create sub widdow %{public}s end", windowName.c_str());
        };
    NativeValue* callback = (info.argv[1] != nullptr && info.argv[1]->TypeOf() == NATIVE_FUNCTION) ?
        info.argv[1] : nullptr;
    NativeValue* result = nullptr;
    AsyncTask::Schedule("JsWindowStage::OnCreateSubWindow",
        engine, CreateAsyncTaskWithLastParam(engine, callback, nullptr, std::move(complete), &result));
    return result;
}

static NativeValue* CreateJsSubWindowArrayObject(NativeEngine& engine,
    std::vector<sptr<Window>> subWinVec)
{
    NativeValue* objValue = engine.CreateArray(subWinVec.size());
    NativeArray* array = ConvertNativeValueTo<NativeArray>(objValue);
    if (array == nullptr) {
        WLOGFE("[NAPI]Failed to convert subWinVec to jsArrayObject");
        return nullptr;
    }
    uint32_t index = 0;
    for (size_t i = 0; i < subWinVec.size(); i++) {
        array->SetElement(index++, CreateJsWindowObject(engine, subWinVec[i]));
    }
    return objValue;
}

NativeValue* JsWindowStage::OnGetSubWindow(NativeEngine& engine, NativeCallbackInfo& info)
{
    AsyncTask::CompleteCallback complete =
        [weak = windowScene_](NativeEngine& engine, AsyncTask& task, int32_t status) {
            auto weakScene = weak.lock();
            if (weakScene == nullptr) {
                WLOGFE("[NAPI]Window scene is nullptr");
                task.Reject(engine, CreateJsError(engine,
                    static_cast<int32_t>(WmErrorCode::WM_ERROR_STATE_ABNORMALLY)));
                return;
            }
            std::vector<sptr<Window>> subWindowVec = weakScene->GetSubWindow();
            task.Resolve(engine, CreateJsSubWindowArrayObject(engine, subWindowVec));
            WLOGFI("[NAPI]Get sub windows, size = %{public}zu", subWindowVec.size());
        };
    NativeValue* callback = (info.argv[0] != nullptr && info.argv[0]->TypeOf() == NATIVE_FUNCTION) ?
        info.argv[0] : nullptr;
    NativeValue* result = nullptr;
    AsyncTask::Schedule("JsWindowStage::OnGetSubWindow",
        engine, CreateAsyncTaskWithLastParam(engine, callback, nullptr, std::move(complete), &result));
    return result;
}

NativeValue* JsWindowStage::OnSetShowOnLockScreen(NativeEngine& engine, NativeCallbackInfo& info)
{
    if (!Permission::IsSystemCalling()) {
        WLOGFE("set show on lock screen permission denied!");
        engine.Throw(CreateJsError(engine, static_cast<int32_t>(WmErrorCode::WM_ERROR_INVALID_CALLING)));
        return CreateJsValue(engine, static_cast<int32_t>(WmErrorCode::WM_ERROR_INVALID_CALLING));
    }
    if (info.argc < 1) {
        WLOGFE("[NAPI]Argc is invalid: %{public}zu", info.argc);
        engine.Throw(CreateJsError(engine, static_cast<int32_t>(WmErrorCode::WM_ERROR_INVALID_PARAM)));
        return CreateJsValue(engine, static_cast<int32_t>(WmErrorCode::WM_ERROR_INVALID_PARAM));
    }
    auto weakScene = windowScene_.lock();
    if (weakScene == nullptr || weakScene->GetMainWindow() == nullptr) {
        WLOGFE("[NAPI]WindowScene is null or window is null");
        engine.Throw(CreateJsError(engine, static_cast<int32_t>(WmErrorCode::WM_ERROR_STATE_ABNORMALLY)));
        return CreateJsValue(engine, static_cast<int32_t>(WmErrorCode::WM_ERROR_STATE_ABNORMALLY));
    }

    bool showOnLockScreen = false;
    NativeBoolean* nativeVal = ConvertNativeValueTo<NativeBoolean>(info.argv[0]);
    if (nativeVal == nullptr) {
        WLOGFE("[NAPI]Failed to convert parameter to boolean");
        engine.Throw(CreateJsError(engine, static_cast<int32_t>(WmErrorCode::WM_ERROR_INVALID_PARAM)));
        return CreateJsValue(engine, static_cast<int32_t>(WmErrorCode::WM_ERROR_INVALID_PARAM));
    } else {
        showOnLockScreen = static_cast<bool>(*nativeVal);
    }

    auto window = weakScene->GetMainWindow();
    WmErrorCode ret;
    if (showOnLockScreen) {
        ret = WM_JS_TO_ERROR_CODE_MAP.at(
            window->AddWindowFlag(WindowFlag::WINDOW_FLAG_SHOW_WHEN_LOCKED));
    } else {
        ret = WM_JS_TO_ERROR_CODE_MAP.at(
            window->RemoveWindowFlag(WindowFlag::WINDOW_FLAG_SHOW_WHEN_LOCKED));
    }
    WLOGFI("[NAPI]Window [%{public}u, %{public}s] SetShowOnLockScreen %{public}u, ret = %{public}u",
        window->GetWindowId(), window->GetWindowName().c_str(), showOnLockScreen, ret);

    return CreateJsValue(engine, static_cast<int32_t>(ret));
}

NativeValue* JsWindowStage::OnDisableWindowDecor(NativeEngine& engine, NativeCallbackInfo& info)
{
    auto weakScene = windowScene_.lock();
    if (weakScene == nullptr) {
        WLOGFE("[NAPI]WindowScene is null");
        return CreateJsValue(engine, static_cast<int32_t>(WmErrorCode::WM_ERROR_STAGE_ABNORMALLY));
    }

    auto window = weakScene->GetMainWindow();
    if (window == nullptr) {
        WLOGFE("[NAPI]Window is null");
        return CreateJsValue(engine, static_cast<int32_t>(WmErrorCode::WM_ERROR_STATE_ABNORMALLY));
    }
    window->DisableAppWindowDecor();
    WLOGFI("[NAPI]Window [%{public}u, %{public}s] disable app window decor end",
        window->GetWindowId(), window->GetWindowName().c_str());
    return engine.CreateUndefined();
}

NativeValue* CreateJsWindowStage(NativeEngine& engine,
    std::shared_ptr<Rosen::WindowScene> windowScene)
{
    WLOGFD("[NAPI]CreateJsWindowStage");
    NativeValue* objValue = engine.CreateObject();
    NativeObject* object = ConvertNativeValueTo<NativeObject>(objValue);

    std::unique_ptr<JsWindowStage> jsWindowStage = std::make_unique<JsWindowStage>(windowScene);
    object->SetNativePointer(jsWindowStage.release(), JsWindowStage::Finalizer, nullptr);

    const char *moduleName = "JsWindowStage";
    BindNativeFunction(engine,
        *object, "setUIContent", moduleName, JsWindowStage::SetUIContent);
    BindNativeFunction(engine,
        *object, "loadContent", moduleName, JsWindowStage::LoadContent);
    BindNativeFunction(engine,
        *object, "getMainWindow", moduleName, JsWindowStage::GetMainWindow);
    BindNativeFunction(engine,
        *object, "getMainWindowSync", moduleName, JsWindowStage::GetMainWindowSync);
    BindNativeFunction(engine,
        *object, "getWindowMode", moduleName, JsWindowStage::GetWindowMode);
    BindNativeFunction(engine,
        *object, "createSubWindow", moduleName, JsWindowStage::CreateSubWindow);
    BindNativeFunction(engine,
        *object, "getSubWindow", moduleName, JsWindowStage::GetSubWindow);
    BindNativeFunction(engine, *object, "on", moduleName, JsWindowStage::On);
    BindNativeFunction(engine, *object, "off", moduleName, JsWindowStage::Off);
    BindNativeFunction(engine,
        *object, "setShowOnLockScreen", moduleName, JsWindowStage::SetShowOnLockScreen);
    BindNativeFunction(engine,
        *object, "disableWindowDecor", moduleName, JsWindowStage::DisableWindowDecor);

    return objValue;
}
}  // namespace Rosen
}  // namespace OHOS
