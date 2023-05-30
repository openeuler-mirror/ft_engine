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

#include "js_screen.h"

#include <cinttypes>
#include "screen.h"
#include "screen_info.h"
#include "window_manager_hilog.h"

namespace OHOS {
namespace Rosen {
using namespace AbilityRuntime;
constexpr size_t ARGC_ONE = 1;
constexpr size_t ARGC_TWO = 2;
namespace {
    constexpr HiviewDFX::HiLogLabel LABEL = {LOG_CORE, HILOG_DOMAIN_DISPLAY, "JsScreen"};
}

static thread_local std::map<ScreenId, std::shared_ptr<NativeReference>> g_JsScreenMap;
std::recursive_mutex g_mutex;

JsScreen::JsScreen(const sptr<Screen>& screen) : screen_(screen)
{
}

JsScreen::~JsScreen()
{
    WLOGFI("JsScreen::~JsScreen is called");
}

void JsScreen::Finalizer(NativeEngine* engine, void* data, void* hint)
{
    WLOGFI("JsScreen::Finalizer is called");
    auto jsScreen = std::unique_ptr<JsScreen>(static_cast<JsScreen*>(data));
    if (jsScreen == nullptr) {
        WLOGFE("jsScreen::Finalizer jsScreen is null");
        return;
    }
    sptr<Screen> screen = jsScreen->screen_;
    if (screen == nullptr) {
        WLOGFE("JsScreen::Finalizer screen is null");
        return;
    }
    ScreenId screenId = screen->GetId();
    WLOGFI("JsScreen::Finalizer screenId : %{public}" PRIu64"", screenId);
    std::lock_guard<std::recursive_mutex> lock(g_mutex);
    if (g_JsScreenMap.find(screenId) != g_JsScreenMap.end()) {
        WLOGFI("JsScreen::Finalizer screen is destroyed: %{public}" PRIu64"", screenId);
        g_JsScreenMap.erase(screenId);
    }
}

NativeValue* JsScreen::SetOrientation(NativeEngine* engine, NativeCallbackInfo* info)
{
    JsScreen* me = CheckParamsAndGetThis<JsScreen>(engine, info);
    return (me != nullptr) ? me->OnSetOrientation(*engine, *info) : nullptr;
}

NativeValue* JsScreen::OnSetOrientation(NativeEngine& engine, NativeCallbackInfo& info)
{
    WLOGFI("OnSetOrientation is called");
    bool paramValidFlag = true;
    Orientation orientation = Orientation::UNSPECIFIED;
    if (info.argc < ARGC_ONE) {
        WLOGFE("OnSetOrientation Params not match, info argc: %{public}zu", info.argc);
        paramValidFlag = false;
    } else {
        if (!ConvertFromJsValue(engine, info.argv[0], orientation)) {
            paramValidFlag = false;
            WLOGFE("Failed to convert parameter to orientation");
        }
    }
    if (!paramValidFlag) {
        WLOGE("OnSetOrientation paramValidFlag error");
        engine.Throw(CreateJsError(engine, static_cast<int32_t>(DmErrorCode::DM_ERROR_INVALID_PARAM)));
        return engine.CreateUndefined();
    }
    if (orientation < Orientation::BEGIN || orientation > Orientation::END) {
        WLOGE("Orientation param error! orientation value must from enum Orientation");
        engine.Throw(CreateJsError(engine, static_cast<int32_t>(DmErrorCode::DM_ERROR_INVALID_PARAM)));
        return engine.CreateUndefined();
    }

    AsyncTask::CompleteCallback complete =
        [=](NativeEngine& engine, AsyncTask& task, int32_t status) {
            bool res = screen_->SetOrientation(orientation);
            if (res) {
                task.Resolve(engine, engine.CreateUndefined());
                WLOGFI("OnSetOrientation success");
            } else {
                task.Reject(engine, CreateJsError(engine, static_cast<int32_t>(DmErrorCode::DM_ERROR_SYSTEM_INNORMAL),
                                                  "JsScreen::OnSetOrientation failed."));
                WLOGFE("OnSetOrientation failed");
            }
        };
    NativeValue* lastParam = nullptr;
    if (info.argc >= ARGC_TWO && info.argv[ARGC_TWO - 1] != nullptr &&
        info.argv[ARGC_TWO - 1]->TypeOf() == NATIVE_FUNCTION) {
        lastParam = info.argv[ARGC_TWO - 1];
    }
    NativeValue* result = nullptr;
    AsyncTask::Schedule("JsScreen::OnSetOrientation",
        engine, CreateAsyncTaskWithLastParam(engine, lastParam, nullptr, std::move(complete), &result));
    return result;
}


NativeValue* JsScreen::SetScreenActiveMode(NativeEngine* engine, NativeCallbackInfo* info)
{
    WLOGFI("SetScreenActiveMode is called");
    JsScreen* me = CheckParamsAndGetThis<JsScreen>(engine, info);
    return (me != nullptr) ? me->OnSetScreenActiveMode(*engine, *info) : nullptr;
}

NativeValue* JsScreen::OnSetScreenActiveMode(NativeEngine& engine, NativeCallbackInfo& info)
{
    WLOGFI("OnSetScreenActiveMode is called");
    bool paramValidFlag = true;
    uint32_t modeId = 0;
    if (info.argc < ARGC_ONE) {
        WLOGFE("OnSetScreenActiveMode Params not match %{public}zu", info.argc);
        paramValidFlag = false;
    } else {
        if (!ConvertFromJsValue(engine, info.argv[0], modeId)) {
            WLOGFE("Failed to convert parameter to modeId");
            paramValidFlag = false;
        }
    }
    if (!paramValidFlag) {
        WLOGFE("OnSetScreenActiveMode paramValidFlag error");
        engine.Throw(CreateJsError(engine, static_cast<int32_t>(DmErrorCode::DM_ERROR_INVALID_PARAM)));
        return engine.CreateUndefined();
    }

    AsyncTask::CompleteCallback complete =
        [=](NativeEngine& engine, AsyncTask& task, int32_t status) {
            bool res = screen_->SetScreenActiveMode(modeId);
            if (res) {
                task.Resolve(engine, engine.CreateUndefined());
                WLOGFI("OnSetScreenActiveMode success");
            } else {
                task.Reject(engine, CreateJsError(engine, static_cast<int32_t>(DmErrorCode::DM_ERROR_SYSTEM_INNORMAL),
                                                "JsScreen::OnSetScreenActiveMode failed."));
                WLOGFE("OnSetScreenActiveMode failed");
            }
        };
    NativeValue* lastParam = nullptr;
    if (info.argc >= ARGC_TWO && info.argv[ARGC_TWO - 1] != nullptr &&
        info.argv[ARGC_TWO - 1]->TypeOf() == NATIVE_FUNCTION) {
        lastParam = info.argv[ARGC_TWO - 1];
    }
    NativeValue* result = nullptr;
    AsyncTask::Schedule("JsScreen::OnSetScreenActiveMode",
        engine, CreateAsyncTaskWithLastParam(engine, lastParam, nullptr, std::move(complete), &result));
    return result;
}

NativeValue* JsScreen::SetDensityDpi(NativeEngine* engine, NativeCallbackInfo* info)
{
    WLOGFI("SetDensityDpi is called");
    JsScreen* me = CheckParamsAndGetThis<JsScreen>(engine, info);
    return (me != nullptr) ? me->OnSetDensityDpi(*engine, *info) : nullptr;
}

NativeValue* JsScreen::OnSetDensityDpi(NativeEngine& engine, NativeCallbackInfo& info)
{
    WLOGFI("OnSetDensityDpi is called");
    bool paramValidFlag = true;
    uint32_t densityDpi = 0;
    if (info.argc < ARGC_ONE) {
        WLOGFE("OnSetDensityDpi Params not match %{public}zu", info.argc);
        paramValidFlag = false;
    } else {
        if (!ConvertFromJsValue(engine, info.argv[0], densityDpi)) {
            WLOGFE("Failed to convert parameter to densityDpi");
            paramValidFlag = false;
        }
    }
    if (!paramValidFlag) {
        WLOGFE("OnSetDensityDpi paramValidFlag error");
        engine.Throw(CreateJsError(engine, static_cast<int32_t>(DmErrorCode::DM_ERROR_INVALID_PARAM)));
        return engine.CreateUndefined();
    }

    AsyncTask::CompleteCallback complete =
        [=](NativeEngine& engine, AsyncTask& task, int32_t status) {
            bool res = screen_->SetDensityDpi(densityDpi);
            if (res) {
                task.Resolve(engine, engine.CreateUndefined());
                WLOGFI("OnSetDensityDpi success");
            } else {
                task.Reject(engine, CreateJsError(engine, static_cast<int32_t>(DmErrorCode::DM_ERROR_SYSTEM_INNORMAL),
                                                "JsScreen::OnSetDensityDpi failed."));
                WLOGFE("OnSetDensityDpi failed");
            }
        };
    NativeValue* lastParam = nullptr;
    if (info.argc >= ARGC_TWO && info.argv[ARGC_TWO - 1] != nullptr &&
        info.argv[ARGC_TWO - 1]->TypeOf() == NATIVE_FUNCTION) {
        lastParam = info.argv[ARGC_TWO - 1];
    }
    NativeValue* result = nullptr;
    AsyncTask::Schedule("JsScreen::OnSetDensityDpi",
        engine, CreateAsyncTaskWithLastParam(engine, lastParam, nullptr, std::move(complete), &result));
    return result;
}

std::shared_ptr<NativeReference> FindJsDisplayObject(ScreenId screenId)
{
    WLOGFI("[NAPI]Try to find screen %{public}" PRIu64" in g_JsScreenMap", screenId);
    std::lock_guard<std::recursive_mutex> lock(g_mutex);
    if (g_JsScreenMap.find(screenId) == g_JsScreenMap.end()) {
        WLOGFI("[NAPI]Can not find screen %{public}" PRIu64" in g_JsScreenMap", screenId);
        return nullptr;
    }
    return g_JsScreenMap[screenId];
}

NativeValue* CreateJsScreenObject(NativeEngine& engine, sptr<Screen>& screen)
{
    WLOGFI("JsScreen::CreateJsScreen is called");
    NativeValue* objValue = nullptr;
    std::shared_ptr<NativeReference> jsScreenObj = FindJsDisplayObject(screen->GetId());
    if (jsScreenObj != nullptr && jsScreenObj->Get() != nullptr) {
        WLOGFI("[NAPI]FindJsScreenObject %{public}" PRIu64"", screen->GetId());
        objValue = jsScreenObj->Get();
    }
    if (objValue == nullptr) {
        objValue = engine.CreateObject();
    }
    NativeObject* object = ConvertNativeValueTo<NativeObject>(objValue);
    if (object == nullptr) {
        WLOGFE("Failed to convert prop to jsObject");
        return engine.CreateUndefined();
    }
    std::unique_ptr<JsScreen> jsScreen = std::make_unique<JsScreen>(screen);
    object->SetNativePointer(jsScreen.release(), JsScreen::Finalizer, nullptr);
    auto info = screen->GetScreenInfo();
    if (info == nullptr) {
        WLOGFE("Failed to GetScreenInfo");
        return engine.CreateUndefined();
    }
    ScreenId screenId = info->GetScreenId();
    object->SetProperty("id",
        CreateJsValue(engine, screenId == SCREEN_ID_INVALID ? -1 : static_cast<int64_t>(screenId)));
    ScreenId parentId = info->GetParentId();
    object->SetProperty("parent",
        CreateJsValue(engine, parentId == SCREEN_ID_INVALID ? -1 : static_cast<int64_t>(parentId)));
    object->SetProperty("orientation", CreateJsValue(engine, info->GetOrientation()));
    object->SetProperty("activeModeIndex", CreateJsValue(engine, info->GetModeId()));
    object->SetProperty("supportedModeInfo", CreateJsScreenModeArrayObject(engine, info->GetModes()));
    object->SetProperty("densityDpi", CreateJsValue(engine,
        static_cast<uint32_t>(info->GetVirtualPixelRatio() * DOT_PER_INCH))); // Dpi = Density(VPR) * 160.
    if (jsScreenObj == nullptr || jsScreenObj->Get() == nullptr) {
        std::shared_ptr<NativeReference> JsScreenRef;
        JsScreenRef.reset(engine.CreateReference(objValue, 1));
        std::lock_guard<std::recursive_mutex> lock(g_mutex);
        g_JsScreenMap[screenId] = JsScreenRef;
        const char *moduleName = "JsScreen";
        BindNativeFunction(engine, *object, "setScreenActiveMode", moduleName, JsScreen::SetScreenActiveMode);
        BindNativeFunction(engine, *object, "setOrientation", moduleName, JsScreen::SetOrientation);
        BindNativeFunction(engine, *object, "setDensityDpi", moduleName, JsScreen::SetDensityDpi);
    }
    return objValue;
}

NativeValue* CreateJsScreenModeArrayObject(NativeEngine& engine, std::vector<sptr<SupportedScreenModes>> screenModes)
{
    NativeValue* arrayValue = engine.CreateArray(screenModes.size());
    NativeArray* array = ConvertNativeValueTo<NativeArray>(arrayValue);
    size_t i = 0;
    for (const auto& mode : screenModes) {
        array->SetElement(i++, CreateJsScreenModeObject(engine, mode));
    }
    return arrayValue;
}

NativeValue* CreateJsScreenModeObject(NativeEngine& engine, const sptr<SupportedScreenModes>& mode)
{
    WLOGFI("JsScreen::CreateJsScreenMode is called");
    NativeValue* objValue = engine.CreateObject();
    NativeObject* optionObject = ConvertNativeValueTo<NativeObject>(objValue);
    if (optionObject == nullptr) {
        WLOGFE("Failed to convert prop to jsObject");
        return engine.CreateUndefined();
    }
    uint32_t width = mode->width_;
    uint32_t height = mode->height_;
    uint32_t refreshRate = mode->refreshRate_;
    optionObject->SetProperty("width", CreateJsValue(engine, width));
    optionObject->SetProperty("height", CreateJsValue(engine, height));
    optionObject->SetProperty("refreshRate", CreateJsValue(engine, refreshRate));
    return objValue;
}
}  // namespace Rosen
}  // namespace OHOS
