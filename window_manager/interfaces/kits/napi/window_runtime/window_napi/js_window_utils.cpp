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
#include "js_window_utils.h"
#include <iomanip>
#include <regex>
#include <sstream>
#include "accesstoken_kit.h"
#include "bundle_constants.h"
#include "ipc_skeleton.h"
#include "window_manager_hilog.h"

namespace OHOS {
namespace Rosen {
using namespace AbilityRuntime;
namespace {
    constexpr HiviewDFX::HiLogLabel LABEL = {LOG_CORE, HILOG_DOMAIN_WINDOW, "JsWindowUtils"};
}

NativeValue* WindowTypeInit(NativeEngine* engine)
{
    WLOGFD("[NAPI]WindowTypeInit");

    if (engine == nullptr) {
        WLOGFE("[NAPI]Engine is nullptr");
        return nullptr;
    }

    NativeValue *objValue = engine->CreateObject();
    NativeObject *object = ConvertNativeValueTo<NativeObject>(objValue);
    if (object == nullptr) {
        WLOGFE("[NAPI]Failed to get object");
        return nullptr;
    }

    object->SetProperty("TYPE_APP", CreateJsValue(*engine,
        static_cast<int32_t>(ApiWindowType::TYPE_APP)));
    object->SetProperty("TYPE_SYSTEM_ALERT", CreateJsValue(*engine,
        static_cast<int32_t>(ApiWindowType::TYPE_SYSTEM_ALERT)));
    object->SetProperty("TYPE_INPUT_METHOD", CreateJsValue(*engine,
        static_cast<int32_t>(ApiWindowType::TYPE_INPUT_METHOD)));
    object->SetProperty("TYPE_STATUS_BAR", CreateJsValue(*engine,
        static_cast<int32_t>(ApiWindowType::TYPE_STATUS_BAR)));
    object->SetProperty("TYPE_PANEL", CreateJsValue(*engine,
        static_cast<int32_t>(ApiWindowType::TYPE_PANEL)));
    object->SetProperty("TYPE_KEYGUARD", CreateJsValue(*engine,
        static_cast<int32_t>(ApiWindowType::TYPE_KEYGUARD)));
    object->SetProperty("TYPE_VOLUME_OVERLAY", CreateJsValue(*engine,
        static_cast<int32_t>(ApiWindowType::TYPE_VOLUME_OVERLAY)));
    object->SetProperty("TYPE_NAVIGATION_BAR", CreateJsValue(*engine,
        static_cast<int32_t>(ApiWindowType::TYPE_NAVIGATION_BAR)));
    object->SetProperty("TYPE_FLOAT", CreateJsValue(*engine,
        static_cast<int32_t>(ApiWindowType::TYPE_FLOAT)));
    object->SetProperty("TYPE_FLOAT_CAMERA", CreateJsValue(*engine,
        static_cast<int32_t>(ApiWindowType::TYPE_FLOAT_CAMERA)));
    object->SetProperty("TYPE_WALLPAPER", CreateJsValue(*engine,
        static_cast<int32_t>(ApiWindowType::TYPE_WALLPAPER)));
    object->SetProperty("TYPE_DESKTOP", CreateJsValue(*engine,
        static_cast<int32_t>(ApiWindowType::TYPE_DESKTOP)));
    object->SetProperty("TYPE_LAUNCHER_RECENT", CreateJsValue(*engine,
        static_cast<int32_t>(ApiWindowType::TYPE_LAUNCHER_RECENT)));
    object->SetProperty("TYPE_LAUNCHER_DOCK", CreateJsValue(*engine,
        static_cast<int32_t>(ApiWindowType::TYPE_LAUNCHER_DOCK)));
    object->SetProperty("TYPE_VOICE_INTERACTION", CreateJsValue(*engine,
        static_cast<int32_t>(ApiWindowType::TYPE_VOICE_INTERACTION)));
    object->SetProperty("TYPE_DIALOG", CreateJsValue(*engine,
        static_cast<int32_t>(ApiWindowType::TYPE_DIALOG)));
    object->SetProperty("TYPE_POINTER", CreateJsValue(*engine,
        static_cast<int32_t>(ApiWindowType::TYPE_POINTER)));
    object->SetProperty("TYPE_SCREENSHOT", CreateJsValue(*engine,
        static_cast<int32_t>(ApiWindowType::TYPE_SCREENSHOT)));

    return objValue;
}

NativeValue* AvoidAreaTypeInit(NativeEngine* engine)
{
    WLOGFD("[NAPI]AvoidAreaTypeInit");

    if (engine == nullptr) {
        WLOGFE("[NAPI]Engine is nullptr");
        return nullptr;
    }

    NativeValue *objValue = engine->CreateObject();
    NativeObject *object = ConvertNativeValueTo<NativeObject>(objValue);
    if (object == nullptr) {
        WLOGFE("[NAPI]Failed to get object");
        return nullptr;
    }

    object->SetProperty("TYPE_SYSTEM", CreateJsValue(*engine,
        static_cast<int32_t>(AvoidAreaType::TYPE_SYSTEM)));
    object->SetProperty("TYPE_CUTOUT", CreateJsValue(*engine,
        static_cast<int32_t>(AvoidAreaType::TYPE_CUTOUT)));
    object->SetProperty("TYPE_SYSTEM_GESTURE", CreateJsValue(*engine,
        static_cast<int32_t>(AvoidAreaType::TYPE_SYSTEM_GESTURE)));
    object->SetProperty("TYPE_KEYBOARD", CreateJsValue(*engine, static_cast<int32_t>(AvoidAreaType::TYPE_KEYBOARD)));
    return objValue;
}

NativeValue* WindowModeInit(NativeEngine* engine)
{
    WLOGFD("[NAPI]WindowModeInit");

    if (engine == nullptr) {
        WLOGFE("[NAPI]Engine is nullptr");
        return nullptr;
    }

    NativeValue *objValue = engine->CreateObject();
    NativeObject *object = ConvertNativeValueTo<NativeObject>(objValue);
    if (object == nullptr) {
        WLOGFE("[NAPI]Failed to get object");
        return nullptr;
    }

    object->SetProperty("UNDEFINED", CreateJsValue(*engine,
        static_cast<int32_t>(ApiWindowMode::UNDEFINED)));
    object->SetProperty("FULLSCREEN", CreateJsValue(*engine,
        static_cast<int32_t>(ApiWindowMode::FULLSCREEN)));
    object->SetProperty("PRIMARY", CreateJsValue(*engine,
        static_cast<int32_t>(ApiWindowMode::PRIMARY)));
    object->SetProperty("SECONDARY", CreateJsValue(*engine,
        static_cast<int32_t>(ApiWindowMode::SECONDARY)));
    object->SetProperty("FLOATING", CreateJsValue(*engine,
        static_cast<int32_t>(ApiWindowMode::FLOATING)));
    return objValue;
}

NativeValue* ColorSpaceInit(NativeEngine* engine)
{
    WLOGFD("[NAPI]ColorSpaceInit");

    if (engine == nullptr) {
        WLOGFE("[NAPI]Engine is nullptr");
        return nullptr;
    }

    NativeValue *objValue = engine->CreateObject();
    NativeObject *object = ConvertNativeValueTo<NativeObject>(objValue);
    if (object == nullptr) {
        WLOGFE("[NAPI]Failed to get object");
        return nullptr;
    }

    object->SetProperty("DEFAULT", CreateJsValue(*engine,
        static_cast<int32_t>(ColorSpace::COLOR_SPACE_DEFAULT)));
    object->SetProperty("WIDE_GAMUT", CreateJsValue(*engine,
        static_cast<int32_t>(ColorSpace::COLOR_SPACE_WIDE_GAMUT)));
    return objValue;
}

NativeValue* OrientationInit(NativeEngine* engine)
{
    WLOGFD("[NAPI]OrientationInit");

    if (engine == nullptr) {
        WLOGFE("[NAPI]Engine is nullptr");
        return nullptr;
    }

    NativeValue *objValue = engine->CreateObject();
    NativeObject *object = ConvertNativeValueTo<NativeObject>(objValue);
    if (object == nullptr) {
        WLOGFE("[NAPI]Failed to get object");
        return nullptr;
    }

    object->SetProperty("UNSPECIFIED", CreateJsValue(*engine,
        static_cast<int32_t>(Orientation::UNSPECIFIED)));
    object->SetProperty("PORTRAIT", CreateJsValue(*engine,
        static_cast<int32_t>(Orientation::VERTICAL)));
    object->SetProperty("LANDSCAPE", CreateJsValue(*engine,
        static_cast<int32_t>(Orientation::HORIZONTAL)));
    object->SetProperty("PORTRAIT_INVERTED", CreateJsValue(*engine,
        static_cast<int32_t>(Orientation::REVERSE_VERTICAL)));
    object->SetProperty("LANDSCAPE_INVERTED", CreateJsValue(*engine,
        static_cast<int32_t>(Orientation::REVERSE_HORIZONTAL)));
    object->SetProperty("AUTO_ROTATION", CreateJsValue(*engine,
        static_cast<int32_t>(Orientation::SENSOR)));
    object->SetProperty("AUTO_ROTATION_PORTRAIT", CreateJsValue(*engine,
        static_cast<int32_t>(Orientation::SENSOR_VERTICAL)));
    object->SetProperty("AUTO_ROTATION_LANDSCAPE", CreateJsValue(*engine,
        static_cast<int32_t>(Orientation::SENSOR_HORIZONTAL)));
    object->SetProperty("AUTO_ROTATION_RESTRICTED", CreateJsValue(*engine,
        static_cast<int32_t>(Orientation::AUTO_ROTATION_RESTRICTED)));
    object->SetProperty("AUTO_ROTATION_PORTRAIT_RESTRICTED", CreateJsValue(*engine,
        static_cast<int32_t>(Orientation::AUTO_ROTATION_PORTRAIT_RESTRICTED)));
    object->SetProperty("AUTO_ROTATION_LANDSCAPE_RESTRICTED", CreateJsValue(*engine,
        static_cast<int32_t>(Orientation::AUTO_ROTATION_LANDSCAPE_RESTRICTED)));
    object->SetProperty("LOCKED", CreateJsValue(*engine,
        static_cast<int32_t>(Orientation::LOCKED)));
    return objValue;
}

NativeValue* WindowStageEventTypeInit(NativeEngine* engine)
{
    WLOGFD("[NAPI]WindowStageEventTypeInit");

    if (engine == nullptr) {
        WLOGFE("[NAPI]Engine is nullptr");
        return nullptr;
    }

    NativeValue *objValue = engine->CreateObject();
    NativeObject *object = ConvertNativeValueTo<NativeObject>(objValue);
    if (object == nullptr) {
        WLOGFE("[NAPI]Failed to get object");
        return nullptr;
    }

    object->SetProperty("SHOWN", CreateJsValue(*engine,
        static_cast<int32_t>(LifeCycleEventType::FOREGROUND)));
    object->SetProperty("ACTIVE", CreateJsValue(*engine,
        static_cast<int32_t>(LifeCycleEventType::ACTIVE)));
    object->SetProperty("INACTIVE", CreateJsValue(*engine,
        static_cast<int32_t>(LifeCycleEventType::INACTIVE)));
    object->SetProperty("HIDDEN", CreateJsValue(*engine,
        static_cast<int32_t>(LifeCycleEventType::BACKGROUND)));
    return objValue;
}

NativeValue* WindowLayoutModeInit(NativeEngine* engine)
{
    WLOGFD("[NAPI]WindowLayoutModeInit");
    if (engine == nullptr) {
        WLOGFE("[NAPI]Engine is nullptr");
        return nullptr;
    }

    NativeValue *objValue = engine->CreateObject();
    NativeObject *object = ConvertNativeValueTo<NativeObject>(objValue);
    if (object == nullptr) {
        WLOGFE("[NAPI]Failed to get object");
        return nullptr;
    }

    object->SetProperty("WINDOW_LAYOUT_MODE_CASCADE", CreateJsValue(*engine,
        static_cast<int32_t>(WindowLayoutMode::CASCADE)));
    object->SetProperty("WINDOW_LAYOUT_MODE_TILE", CreateJsValue(*engine,
        static_cast<int32_t>(WindowLayoutMode::TILE)));
    return objValue;
}

NativeValue* BlurStyleInit(NativeEngine* engine)
{
    WLOGFI("[NAPI]BlurStyleInit");
    if (engine == nullptr) {
        WLOGFE("[NAPI]Engine is nullptr");
        return nullptr;
    }

    NativeValue *objValue = engine->CreateObject();
    NativeObject *object = ConvertNativeValueTo<NativeObject>(objValue);
    if (object == nullptr) {
        WLOGFE("[NAPI]Failed to get object");
        return nullptr;
    }

    object->SetProperty("OFF", CreateJsValue(*engine,
        static_cast<int32_t>(WindowBlurStyle::WINDOW_BLUR_OFF)));
    object->SetProperty("THIN", CreateJsValue(*engine,
        static_cast<int32_t>(WindowBlurStyle::WINDOW_BLUR_THIN)));
    object->SetProperty("REGULAR", CreateJsValue(*engine,
        static_cast<int32_t>(WindowBlurStyle::WINDOW_BLUR_REGULAR)));
    object->SetProperty("THICK", CreateJsValue(*engine,
        static_cast<int32_t>(WindowBlurStyle::WINDOW_BLUR_THICK)));
    return objValue;
}

NativeValue* WindowErrorInit(NativeEngine* engine)
{
    WLOGFD("[NAPI]WindowErrorInit");
    if (engine == nullptr) {
        WLOGFE("[NAPI]Engine is nullptr");
        return nullptr;
    }

    NativeValue *objValue = engine->CreateObject();
    NativeObject *object = ConvertNativeValueTo<NativeObject>(objValue);
    if (object == nullptr) {
        WLOGFE("[NAPI]Failed to get object");
        return nullptr;
    }

    object->SetProperty("WM_DO_NOTHING", CreateJsValue(*engine,
        static_cast<int32_t>(WMError::WM_DO_NOTHING)));
    object->SetProperty("WM_ERROR_NO_MEM", CreateJsValue(*engine,
        static_cast<int32_t>(WMError::WM_ERROR_NO_MEM)));
    object->SetProperty("WM_ERROR_DESTROYED_OBJECT", CreateJsValue(*engine,
        static_cast<int32_t>(WMError::WM_ERROR_DESTROYED_OBJECT)));
    object->SetProperty("WM_ERROR_INVALID_WINDOW", CreateJsValue(*engine,
        static_cast<int32_t>(WMError::WM_ERROR_INVALID_WINDOW)));
    object->SetProperty("WM_ERROR_INVALID_WINDOW_MODE_OR_SIZE", CreateJsValue(*engine,
        static_cast<int32_t>(WMError::WM_ERROR_INVALID_WINDOW_MODE_OR_SIZE)));
    object->SetProperty("WM_ERROR_INVALID_OPERATION", CreateJsValue(*engine,
        static_cast<int32_t>(WMError::WM_ERROR_INVALID_OPERATION)));
    object->SetProperty("WM_ERROR_INVALID_PERMISSION", CreateJsValue(*engine,
        static_cast<int32_t>(WMError::WM_ERROR_INVALID_PERMISSION)));
    object->SetProperty("WM_ERROR_NO_REMOTE_ANIMATION", CreateJsValue(*engine,
        static_cast<int32_t>(WMError::WM_ERROR_NO_REMOTE_ANIMATION)));
    object->SetProperty("WM_ERROR_DEVICE_NOT_SUPPORT", CreateJsValue(*engine,
        static_cast<int32_t>(WMError::WM_ERROR_DEVICE_NOT_SUPPORT)));
    object->SetProperty("WM_ERROR_NULLPTR", CreateJsValue(*engine,
        static_cast<int32_t>(WMError::WM_ERROR_NULLPTR)));
    object->SetProperty("WM_ERROR_INVALID_TYPE", CreateJsValue(*engine,
        static_cast<int32_t>(WMError::WM_ERROR_INVALID_TYPE)));
    object->SetProperty("WM_ERROR_INVALID_PARAM", CreateJsValue(*engine,
        static_cast<int32_t>(WMError::WM_ERROR_INVALID_PARAM)));
    object->SetProperty("WM_ERROR_SAMGR", CreateJsValue(*engine,
        static_cast<int32_t>(WMError::WM_ERROR_SAMGR)));
    object->SetProperty("WM_ERROR_IPC_FAILED", CreateJsValue(*engine,
        static_cast<int32_t>(WMError::WM_ERROR_IPC_FAILED)));
    object->SetProperty("WM_ERROR_START_ABILITY_FAILED", CreateJsValue(*engine,
        static_cast<int32_t>(WMError::WM_ERROR_START_ABILITY_FAILED)));
    return objValue;
}

NativeValue* WindowErrorCodeInit(NativeEngine* engine)
{
    WLOGFD("[NAPI]WindowErrorCodeInit");
    if (engine == nullptr) {
        WLOGFE("[NAPI]Engine is nullptr");
        return nullptr;
    }

    NativeValue *objValue = engine->CreateObject();
    NativeObject *object = ConvertNativeValueTo<NativeObject>(objValue);
    if (object == nullptr) {
        WLOGFE("[NAPI]Failed to get object");
        return nullptr;
    }
    object->SetProperty("WM_ERROR_NO_PERMISSION", CreateJsValue(*engine,
        static_cast<int32_t>(WmErrorCode::WM_ERROR_NO_PERMISSION)));
    object->SetProperty("WM_ERROR_INVALID_PARAM", CreateJsValue(*engine,
        static_cast<int32_t>(WmErrorCode::WM_ERROR_INVALID_PARAM)));
    object->SetProperty("WM_ERROR_DEVICE_NOT_SUPPORT", CreateJsValue(*engine,
        static_cast<int32_t>(WmErrorCode::WM_ERROR_DEVICE_NOT_SUPPORT)));
    object->SetProperty("WM_ERROR_REPEAT_OPERATION", CreateJsValue(*engine,
        static_cast<int32_t>(WmErrorCode::WM_ERROR_REPEAT_OPERATION)));
    object->SetProperty("WM_ERROR_STATE_ABNORMALLY", CreateJsValue(*engine,
        static_cast<int32_t>(WmErrorCode::WM_ERROR_STATE_ABNORMALLY)));
    object->SetProperty("WM_ERROR_SYSTEM_ABNORMALLY", CreateJsValue(*engine,
        static_cast<int32_t>(WmErrorCode::WM_ERROR_SYSTEM_ABNORMALLY)));
    object->SetProperty("WM_ERROR_INVALID_CALLING", CreateJsValue(*engine,
        static_cast<int32_t>(WmErrorCode::WM_ERROR_INVALID_CALLING)));
    object->SetProperty("WM_ERROR_STAGE_ABNORMALLY", CreateJsValue(*engine,
        static_cast<int32_t>(WmErrorCode::WM_ERROR_STAGE_ABNORMALLY)));
    object->SetProperty("WM_ERROR_CONTEXT_ABNORMALLY", CreateJsValue(*engine,
        static_cast<int32_t>(WmErrorCode::WM_ERROR_CONTEXT_ABNORMALLY)));
    object->SetProperty("WM_ERROR_START_ABILITY_FAILED", CreateJsValue(*engine,
        static_cast<int32_t>(WmErrorCode::WM_ERROR_START_ABILITY_FAILED)));
    return objValue;
}

NativeValue* GetRectAndConvertToJsValue(NativeEngine& engine, const Rect& rect)
{
    NativeValue* objValue = engine.CreateObject();
    NativeObject* object = ConvertNativeValueTo<NativeObject>(objValue);
    if (object == nullptr) {
        WLOGFE("[NAPI]Failed to convert rect to jsObject");
        return nullptr;
    }
    object->SetProperty("left", CreateJsValue(engine, rect.posX_));
    object->SetProperty("top", CreateJsValue(engine, rect.posY_));
    object->SetProperty("width", CreateJsValue(engine, rect.width_));
    object->SetProperty("height", CreateJsValue(engine, rect.height_));
    return objValue;
}

NativeValue* CreateJsWindowPropertiesObject(NativeEngine& engine, sptr<Window>& window)
{
    WLOGFI("[NAPI]CreateJsWindowPropertiesObject");
    NativeValue* objValue = engine.CreateObject();
    NativeObject* object = ConvertNativeValueTo<NativeObject>(objValue);
    if (object == nullptr) {
        WLOGFE("[NAPI]Failed to convert windowProperties to jsObject");
        return nullptr;
    }

    Rect rect = window->GetRect();
    NativeValue* rectObj = GetRectAndConvertToJsValue(engine, rect);
    if (rectObj == nullptr) {
        WLOGFE("[NAPI]GetRect failed!");
    }
    object->SetProperty("windowRect", rectObj);
    WindowType type = window->GetType();
    if (NATIVE_JS_TO_WINDOW_TYPE_MAP.count(type) != 0) {
        object->SetProperty("type", CreateJsValue(engine, NATIVE_JS_TO_WINDOW_TYPE_MAP.at(type)));
    } else {
        object->SetProperty("type", CreateJsValue(engine, type));
    }
    object->SetProperty("isLayoutFullScreen", CreateJsValue(engine, window->IsLayoutFullScreen()));
    object->SetProperty("isFullScreen", CreateJsValue(engine, window->IsFullScreen()));
    object->SetProperty("touchable", CreateJsValue(engine, window->GetTouchable()));
    object->SetProperty("focusable", CreateJsValue(engine, window->GetFocusable()));
    object->SetProperty("name", CreateJsValue(engine, window->GetWindowName()));
    object->SetProperty("isPrivacyMode", CreateJsValue(engine, window->IsPrivacyMode()));
    object->SetProperty("isKeepScreenOn", CreateJsValue(engine, window->IsKeepScreenOn()));
    object->SetProperty("brightness", CreateJsValue(engine, window->GetBrightness()));
    object->SetProperty("isTransparent", CreateJsValue(engine, window->IsTransparent()));
    object->SetProperty("isRoundCorner", CreateJsValue(engine, false)); // empty method
    object->SetProperty("dimBehindValue", CreateJsValue(engine, 0));
    object->SetProperty("id", CreateJsValue(engine, window->GetWindowId()));
    return objValue;
}
static std::string GetHexColor(uint32_t color)
{
    std::stringstream ioss;
    std::string temp;
    ioss << std::setiosflags(std::ios::uppercase) << std::hex << color;
    ioss >> temp;
    int count = RGBA_LENGTH - static_cast<int>(temp.length());
    std::string tmpColor(count, '0');
    tmpColor += temp;
    std::string finalColor("#");
    finalColor += tmpColor;
    return finalColor;
}

static NativeValue* CreateJsSystemBarRegionTintObject(NativeEngine& engine, const SystemBarRegionTint& tint)
{
    NativeValue* objValue = engine.CreateObject();
    NativeObject* object = ConvertNativeValueTo<NativeObject>(objValue);
    if (object == nullptr) {
        WLOGFE("[NAPI]Failed to convert SystemBarProperty to jsObject");
        return nullptr;
    }
    if (NATIVE_JS_TO_WINDOW_TYPE_MAP.count(tint.type_) != 0) {
        object->SetProperty("type", CreateJsValue(engine, NATIVE_JS_TO_WINDOW_TYPE_MAP.at(tint.type_)));
    } else {
        object->SetProperty("type", CreateJsValue(engine, tint.type_));
    }
    object->SetProperty("isEnable", CreateJsValue(engine, tint.prop_.enable_));
    std::string bkgColor = GetHexColor(tint.prop_.backgroundColor_);
    object->SetProperty("backgroundColor", CreateJsValue(engine, bkgColor));
    std::string contentColor = GetHexColor(tint.prop_.contentColor_);
    object->SetProperty("contentColor", CreateJsValue(engine, contentColor));
    Rect rect = tint.region_;
    object->SetProperty("region", GetRectAndConvertToJsValue(engine, rect));

    WLOGFI("[NAPI]Type %{public}u [%{public}u %{public}s %{public}s]",
        tint.type_, tint.prop_.enable_, bkgColor.c_str(), contentColor.c_str());
    WLOGFI("[NAPI]Region [%{public}d %{public}d %{public}u %{public}u]",
        rect.posX_, rect.posY_, rect.width_, rect.height_);
    return objValue;
}

NativeValue* CreateJsSystemBarRegionTintArrayObject(NativeEngine& engine, const SystemBarRegionTints& tints)
{
    if (tints.empty()) {
        WLOGFE("[NAPI]Empty tints");
        return nullptr;
    }
    NativeValue* objValue = engine.CreateArray(tints.size());
    NativeArray* array = ConvertNativeValueTo<NativeArray>(objValue);
    if (array == nullptr) {
        WLOGFE("[NAPI]Failed to convert SystemBarPropertys to jsArrayObject");
        return nullptr;
    }
    uint32_t index = 0;
    for (size_t i = 0; i < tints.size(); i++) {
        array->SetElement(index++, CreateJsSystemBarRegionTintObject(engine, tints[i]));
    }
    return objValue;
}

bool GetSystemBarStatus(std::map<WindowType, SystemBarProperty>& systemBarProperties,
                        NativeEngine& engine, NativeCallbackInfo& info, sptr<Window>& window)
{
    NativeArray* nativeArray = nullptr;
    uint32_t size = 0;
    if (info.argc > 0 && info.argv[0]->TypeOf() != NATIVE_FUNCTION) {
        nativeArray = ConvertNativeValueTo<NativeArray>(info.argv[0]);
        if (nativeArray == nullptr) {
            WLOGFE("[NAPI]Failed to convert parameter to SystemBarArray");
            return false;
        }
        size = nativeArray->GetLength();
    }
    auto statusProperty = window->GetSystemBarPropertyByType(WindowType::WINDOW_TYPE_STATUS_BAR);
    auto navProperty = window->GetSystemBarPropertyByType(WindowType::WINDOW_TYPE_NAVIGATION_BAR);
    statusProperty.enable_ = false;
    navProperty.enable_ = false;
    systemBarProperties[WindowType::WINDOW_TYPE_STATUS_BAR] = statusProperty;
    systemBarProperties[WindowType::WINDOW_TYPE_NAVIGATION_BAR] = navProperty;
    for (uint32_t i = 0; i < size; i++) {
        std::string name;
        if (!ConvertFromJsValue(engine, nativeArray->GetElement(i), name)) {
            WLOGFE("[NAPI]Failed to convert parameter to SystemBarName");
            return false;
        }
        if (name.compare("status") == 0) {
            systemBarProperties[WindowType::WINDOW_TYPE_STATUS_BAR].enable_ = true;
        } else if (name.compare("navigation") == 0) {
            systemBarProperties[WindowType::WINDOW_TYPE_NAVIGATION_BAR].enable_ = true;
        }
    }
    return true;
}

static uint32_t GetColorFromJs(NativeEngine& engine, NativeObject* jsObject,
    const char* name, uint32_t defaultColor)
{
    NativeValue* jsColor = jsObject->GetProperty(name);
    if (jsColor->TypeOf() != NATIVE_UNDEFINED) {
        std::string colorStr;
        if (!ConvertFromJsValue(engine, jsColor, colorStr)) {
            WLOGFE("[NAPI]Failed to convert parameter to color");
            return defaultColor;
        }
        std::regex pattern("^#([A-Fa-f0-9]{6}|[A-Fa-f0-9]{8})$");
        if (!std::regex_match(colorStr, pattern)) {
            WLOGFE("[NAPI]Invalid color input");
            return defaultColor;
        }
        std::string color = colorStr.substr(1);
        if (color.length() == RGB_LENGTH) {
            color = "FF" + color; // ARGB
        }
        std::stringstream ss;
        uint32_t hexColor;
        ss << std::hex << color;
        ss >> hexColor;
        WLOGFI("[NAPI]Origin %{public}s, process %{public}s, final %{public}x",
            colorStr.c_str(), color.c_str(), hexColor);
        return hexColor;
    }
    return defaultColor;
}

bool SetSystemBarPropertiesFromJs(NativeEngine& engine, NativeObject* jsObject,
    std::map<WindowType, SystemBarProperty>& properties, sptr<Window>& window)
{
    auto statusProperty = window->GetSystemBarPropertyByType(WindowType::WINDOW_TYPE_STATUS_BAR);
    auto navProperty = window->GetSystemBarPropertyByType(WindowType::WINDOW_TYPE_NAVIGATION_BAR);
    properties[WindowType::WINDOW_TYPE_STATUS_BAR] = statusProperty;
    properties[WindowType::WINDOW_TYPE_NAVIGATION_BAR] = navProperty;
    properties[WindowType::WINDOW_TYPE_STATUS_BAR].backgroundColor_ = GetColorFromJs(engine,
        jsObject, "statusBarColor", statusProperty.backgroundColor_);
    properties[WindowType::WINDOW_TYPE_NAVIGATION_BAR].backgroundColor_ = GetColorFromJs(engine,
        jsObject, "navigationBarColor", navProperty.backgroundColor_);
    NativeValue* jsStatusContentColor = jsObject->GetProperty("statusBarContentColor");
    NativeValue* jsStatusIcon = jsObject->GetProperty("isStatusBarLightIcon");
    if (jsStatusContentColor->TypeOf() != NATIVE_UNDEFINED) {
        properties[WindowType::WINDOW_TYPE_STATUS_BAR].contentColor_ =  GetColorFromJs(engine,
            jsObject, "statusBarContentColor", statusProperty.contentColor_);
    } else if (jsStatusIcon->TypeOf() != NATIVE_UNDEFINED) {
        bool isStatusBarLightIcon;
        if (!ConvertFromJsValue(engine, jsStatusIcon, isStatusBarLightIcon)) {
            WLOGFE("[NAPI]Failed to convert parameter to isStatusBarLightIcon");
            return false;
        }
        if (isStatusBarLightIcon) {
            properties[WindowType::WINDOW_TYPE_STATUS_BAR].contentColor_ = SYSTEM_COLOR_WHITE;
        } else {
            properties[WindowType::WINDOW_TYPE_STATUS_BAR].contentColor_ = SYSTEM_COLOR_BLACK;
        }
    }
    NativeValue* jsNavigationContentColor = jsObject->GetProperty("navigationBarContentColor");
    NativeValue* jsNavigationIcon = jsObject->GetProperty("isNavigationBarLightIcon");
    if (jsNavigationContentColor->TypeOf() != NATIVE_UNDEFINED) {
        properties[WindowType::WINDOW_TYPE_NAVIGATION_BAR].contentColor_ = GetColorFromJs(engine,
            jsObject, "navigationBarContentColor", navProperty.contentColor_);
    } else if (jsNavigationIcon->TypeOf() != NATIVE_UNDEFINED) {
        bool isNavigationBarLightIcon;
        if (!ConvertFromJsValue(engine, jsNavigationIcon, isNavigationBarLightIcon)) {
            WLOGFE("[NAPI]Failed to convert parameter to isNavigationBarLightIcon");
            return false;
        }
        if (isNavigationBarLightIcon) {
            properties[WindowType::WINDOW_TYPE_NAVIGATION_BAR].contentColor_ = SYSTEM_COLOR_WHITE;
        } else {
            properties[WindowType::WINDOW_TYPE_NAVIGATION_BAR].contentColor_ = SYSTEM_COLOR_BLACK;
        }
    }
    return true;
}

NativeValue* ConvertAvoidAreaToJsValue(NativeEngine& engine, const AvoidArea& avoidArea, AvoidAreaType type)
{
    NativeValue* objValue = engine.CreateObject();
    NativeObject* object = ConvertNativeValueTo<NativeObject>(objValue);
    if (object == nullptr) {
        WLOGFE("[NAPI]Failed to convert avoidArea to jsObject");
        return nullptr;
    }
    object->SetProperty("visible", CreateJsValue(engine, type == AvoidAreaType::TYPE_CUTOUT ? false : true));
    object->SetProperty("leftRect", GetRectAndConvertToJsValue(engine, avoidArea.leftRect_));
    object->SetProperty("topRect", GetRectAndConvertToJsValue(engine, avoidArea.topRect_));
    object->SetProperty("rightRect", GetRectAndConvertToJsValue(engine, avoidArea.rightRect_));
    object->SetProperty("bottomRect", GetRectAndConvertToJsValue(engine, avoidArea.bottomRect_));
    return objValue;
}

bool CheckCallingPermission(std::string permission)
{
    WLOGFI("[NAPI]Permission: %{public}s", permission.c_str());
    if (!permission.empty() &&
        Security::AccessToken::AccessTokenKit::VerifyAccessToken(IPCSkeleton::GetCallingTokenID(), permission)
        != AppExecFwk::Constants::PERMISSION_GRANTED) {
        WLOGFE("[NAPI]Permission %{public}s is not granted", permission.c_str());
        return false;
    }
    return true;
}

bool GetAPI7Ability(NativeEngine& engine, AppExecFwk::Ability* &ability)
{
    napi_value global;
    auto env = reinterpret_cast<napi_env>(&engine);
    if (napi_get_global(env, &global) != napi_ok) {
        WLOGFI("[NAPI]Get global failed");
        return false;
    }
    napi_value jsAbility;
    napi_status status = napi_get_named_property(env, global, "ability", &jsAbility);
    if (status != napi_ok || jsAbility == nullptr) {
        WLOGFI("[NAPI]Get ability property failed");
        return false;
    }

    if (napi_get_value_external(env, jsAbility, reinterpret_cast<void **>(&ability)) != napi_ok) {
        WLOGFI("[NAPI]Get ability external failed");
        return false;
    }
    if (ability == nullptr) {
        return false;
    } else {
        WLOGFI("[NAPI]Get ability");
    }
    return true;
}
} // namespace Rosen
} // namespace OHOS
