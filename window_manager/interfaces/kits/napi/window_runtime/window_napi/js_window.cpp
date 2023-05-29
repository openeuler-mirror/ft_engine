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

#include "js_window.h"
#include <new>

#ifndef WINDOW_PREVIEW
#include "js_transition_controller.h"
#else
#include "mock/js_transition_controller.h"
#endif

#include "js_window_utils.h"
#include "window.h"
#include "window_helper.h"
#include "window_manager_hilog.h"
#include "window_option.h"
#include "wm_math.h"
#include "pixel_map.h"
#include "pixel_map_napi.h"
#include "napi_remote_object.h"
#include "permission.h"
#include "request_info.h"

namespace OHOS {
namespace Rosen {
using namespace AbilityRuntime;
namespace {
    constexpr HiviewDFX::HiLogLabel LABEL = {LOG_CORE, HILOG_DOMAIN_WINDOW, "JsWindow"};
    constexpr Rect g_emptyRect = {0, 0, 0, 0};
}

static thread_local std::map<std::string, std::shared_ptr<NativeReference>> g_jsWindowMap;
std::recursive_mutex g_mutex;
static int ctorCnt = 0;
static int dtorCnt = 0;
static int finalizerCnt = 0;
JsWindow::JsWindow(const sptr<Window>& window)
    : windowToken_(window), registerManager_(std::make_unique<JsWindowRegisterManager>())
{
    NotifyNativeWinDestroyFunc func = [](std::string windowName) {
        std::lock_guard<std::recursive_mutex> lock(g_mutex);
        if (windowName.empty() || g_jsWindowMap.count(windowName) == 0) {
            WLOGFE("[NAPI]Can not find window %{public}s ", windowName.c_str());
            return;
        }
        g_jsWindowMap.erase(windowName);
        WLOGFD("[NAPI]Destroy window %{public}s in js window", windowName.c_str());
    };
    windowToken_->RegisterWindowDestroyedListener(func);
    WLOGFD("[NAPI] constructorCnt: %{public}d", ++ctorCnt);
}

JsWindow::~JsWindow()
{
    WLOGFD("[NAPI] deConstructorCnt:%{public}d", ++dtorCnt);
    windowToken_ = nullptr;
}

std::string JsWindow::GetWindowName()
{
    if (windowToken_ == nullptr) {
        return "";
    }
    return windowToken_->GetWindowName();
}

void JsWindow::Finalizer(NativeEngine* engine, void* data, void* hint)
{
    WLOGFD("[NAPI]finalizerCnt:%{public}d", ++finalizerCnt);
    auto jsWin = std::unique_ptr<JsWindow>(static_cast<JsWindow*>(data));
    if (jsWin == nullptr) {
        WLOGFE("[NAPI]jsWin is nullptr");
        return;
    }
    std::string windowName = jsWin->GetWindowName();
    WLOGFD("[NAPI]Window %{public}s", windowName.c_str());
    std::lock_guard<std::recursive_mutex> lock(g_mutex);
    g_jsWindowMap.erase(windowName);
    WLOGFD("[NAPI]Remove window %{public}s from g_jsWindowMap", windowName.c_str());
}

NativeValue* JsWindow::Show(NativeEngine* engine, NativeCallbackInfo* info)
{
    WLOGFD("[NAPI]Show");
    JsWindow* me = CheckParamsAndGetThis<JsWindow>(engine, info);
    return (me != nullptr) ? me->OnShow(*engine, *info) : nullptr;
}

NativeValue* JsWindow::ShowWindow(NativeEngine* engine, NativeCallbackInfo* info)
{
    WLOGFD("[NAPI]Show");
    JsWindow* me = CheckParamsAndGetThis<JsWindow>(engine, info);
    return (me != nullptr) ? me->OnShowWindow(*engine, *info) : nullptr;
}

NativeValue* JsWindow::ShowWithAnimation(NativeEngine* engine, NativeCallbackInfo* info)
{
    WLOGFD("[NAPI]ShowWithAnimation");
    JsWindow* me = CheckParamsAndGetThis<JsWindow>(engine, info);
    return (me != nullptr) ? me->OnShowWithAnimation(*engine, *info) : nullptr;
}

NativeValue* JsWindow::Destroy(NativeEngine* engine, NativeCallbackInfo* info)
{
    WLOGFD("[NAPI]Destroy");
    JsWindow* me = CheckParamsAndGetThis<JsWindow>(engine, info);
    return (me != nullptr) ? me->OnDestroy(*engine, *info) : nullptr;
}

NativeValue* JsWindow::DestroyWindow(NativeEngine* engine, NativeCallbackInfo* info)
{
    WLOGFD("[NAPI]Destroy");
    JsWindow* me = CheckParamsAndGetThis<JsWindow>(engine, info);
    return (me != nullptr) ? me->OnDestroyWindow(*engine, *info) : nullptr;
}

NativeValue* JsWindow::Hide(NativeEngine* engine, NativeCallbackInfo* info)
{
    WLOGFD("[NAPI]Hide");
    JsWindow* me = CheckParamsAndGetThis<JsWindow>(engine, info);
    return (me != nullptr) ? me->OnHide(*engine, *info) : nullptr;
}

NativeValue* JsWindow::HideWithAnimation(NativeEngine* engine, NativeCallbackInfo* info)
{
    WLOGFD("[NAPI]HideWithAnimation");
    JsWindow* me = CheckParamsAndGetThis<JsWindow>(engine, info);
    return (me != nullptr) ? me->OnHideWithAnimation(*engine, *info) : nullptr;
}

NativeValue* JsWindow::MoveTo(NativeEngine* engine, NativeCallbackInfo* info)
{
    WLOGFD("[NAPI]MoveTo");
    JsWindow* me = CheckParamsAndGetThis<JsWindow>(engine, info);
    return (me != nullptr) ? me->OnMoveTo(*engine, *info) : nullptr;
}

NativeValue* JsWindow::MoveWindowTo(NativeEngine* engine, NativeCallbackInfo* info)
{
    WLOGFD("[NAPI]MoveTo");
    JsWindow* me = CheckParamsAndGetThis<JsWindow>(engine, info);
    return (me != nullptr) ? me->OnMoveWindowTo(*engine, *info) : nullptr;
}

NativeValue* JsWindow::Resize(NativeEngine* engine, NativeCallbackInfo* info)
{
    WLOGFD("[NAPI]Resize");
    JsWindow* me = CheckParamsAndGetThis<JsWindow>(engine, info);
    return (me != nullptr) ? me->OnResize(*engine, *info) : nullptr;
}

NativeValue* JsWindow::ResizeWindow(NativeEngine* engine, NativeCallbackInfo* info)
{
    WLOGFD("[NAPI]Resize");
    JsWindow* me = CheckParamsAndGetThis<JsWindow>(engine, info);
    return (me != nullptr) ? me->OnResizeWindow(*engine, *info) : nullptr;
}

NativeValue* JsWindow::SetWindowType(NativeEngine* engine, NativeCallbackInfo* info)
{
    WLOGFD("[NAPI]SetWindowType");
    JsWindow* me = CheckParamsAndGetThis<JsWindow>(engine, info);
    return (me != nullptr) ? me->OnSetWindowType(*engine, *info) : nullptr;
}

NativeValue* JsWindow::SetWindowMode(NativeEngine* engine, NativeCallbackInfo* info)
{
    WLOGFD("[NAPI]SetWindowMode");
    JsWindow* me = CheckParamsAndGetThis<JsWindow>(engine, info);
    return (me != nullptr) ? me->OnSetWindowMode(*engine, *info) : nullptr;
}

NativeValue* JsWindow::GetProperties(NativeEngine* engine, NativeCallbackInfo* info)
{
    WLOGFD("[NAPI]GetProperties");
    JsWindow* me = CheckParamsAndGetThis<JsWindow>(engine, info);
    return (me != nullptr) ? me->OnGetProperties(*engine, *info) : nullptr;
}

NativeValue* JsWindow::GetWindowPropertiesSync(NativeEngine* engine, NativeCallbackInfo* info)
{
    WLOGFD("[NAPI]GetProperties");
    JsWindow* me = CheckParamsAndGetThis<JsWindow>(engine, info);
    return (me != nullptr) ? me->OnGetWindowPropertiesSync(*engine, *info) : nullptr;
}

NativeValue* JsWindow::RegisterWindowCallback(NativeEngine* engine, NativeCallbackInfo* info)
{
    WLOGFD("[NAPI]RegisterWindowCallback");
    JsWindow* me = CheckParamsAndGetThis<JsWindow>(engine, info);
    return (me != nullptr) ? me->OnRegisterWindowCallback(*engine, *info) : nullptr;
}

NativeValue* JsWindow::UnregisterWindowCallback(NativeEngine* engine, NativeCallbackInfo* info)
{
    WLOGFD("[NAPI]UnregisterWindowCallback");
    JsWindow* me = CheckParamsAndGetThis<JsWindow>(engine, info);
    return (me != nullptr) ? me->OnUnregisterWindowCallback(*engine, *info) : nullptr;
}

NativeValue* JsWindow::BindDialogTarget(NativeEngine* engine, NativeCallbackInfo* info)
{
    WLOGFD("[NAPI]BindDialogTarget");
    JsWindow* me = CheckParamsAndGetThis<JsWindow>(engine, info);
    return (me != nullptr) ? me->OnBindDialogTarget(*engine, *info) : nullptr;
}

NativeValue* JsWindow::LoadContent(NativeEngine* engine, NativeCallbackInfo* info)
{
    WLOGFD("[NAPI]LoadContent");
    JsWindow* me = CheckParamsAndGetThis<JsWindow>(engine, info);
    return (me != nullptr) ? me->OnLoadContent(*engine, *info) : nullptr;
}

NativeValue* JsWindow::SetUIContent(NativeEngine* engine, NativeCallbackInfo* info)
{
    WLOGFD("[NAPI]LoadContent");
    JsWindow* me = CheckParamsAndGetThis<JsWindow>(engine, info);
    return (me != nullptr) ? me->OnSetUIContent(*engine, *info) : nullptr;
}

NativeValue* JsWindow::SetFullScreen(NativeEngine* engine, NativeCallbackInfo* info)
{
    WLOGFD("[NAPI]SetFullScreen");
    JsWindow* me = CheckParamsAndGetThis<JsWindow>(engine, info);
    return (me != nullptr) ? me->OnSetFullScreen(*engine, *info) : nullptr;
}

NativeValue* JsWindow::SetLayoutFullScreen(NativeEngine* engine, NativeCallbackInfo* info)
{
    WLOGFD("[NAPI]SetLayoutFullScreen");
    JsWindow* me = CheckParamsAndGetThis<JsWindow>(engine, info);
    return (me != nullptr) ? me->OnSetLayoutFullScreen(*engine, *info) : nullptr;
}

NativeValue* JsWindow::SetWindowLayoutFullScreen(NativeEngine* engine, NativeCallbackInfo* info)
{
    WLOGFD("[NAPI]SetLayoutFullScreen");
    JsWindow* me = CheckParamsAndGetThis<JsWindow>(engine, info);
    return (me != nullptr) ? me->OnSetWindowLayoutFullScreen(*engine, *info) : nullptr;
}

NativeValue* JsWindow::SetSystemBarEnable(NativeEngine* engine, NativeCallbackInfo* info)
{
    WLOGFD("[NAPI]SetSystemBarEnable");
    JsWindow* me = CheckParamsAndGetThis<JsWindow>(engine, info);
    return (me != nullptr) ? me->OnSetSystemBarEnable(*engine, *info) : nullptr;
}

NativeValue* JsWindow::SetWindowSystemBarEnable(NativeEngine* engine, NativeCallbackInfo* info)
{
    WLOGFD("[NAPI]SetSystemBarEnable");
    JsWindow* me = CheckParamsAndGetThis<JsWindow>(engine, info);
    return (me != nullptr) ? me->OnSetWindowSystemBarEnable(*engine, *info) : nullptr;
}

NativeValue* JsWindow::SetSystemBarProperties(NativeEngine* engine, NativeCallbackInfo* info)
{
    WLOGFD("[NAPI]SetSystemBarProperties");
    JsWindow* me = CheckParamsAndGetThis<JsWindow>(engine, info);
    return (me != nullptr) ? me->OnSetSystemBarProperties(*engine, *info) : nullptr;
}

NativeValue* JsWindow::SetWindowSystemBarProperties(NativeEngine* engine, NativeCallbackInfo* info)
{
    WLOGFD("[NAPI]SetWindowSystemBarProperties");
    JsWindow* me = CheckParamsAndGetThis<JsWindow>(engine, info);
    return (me != nullptr) ? me->OnSetWindowSystemBarProperties(*engine, *info) : nullptr;
}

NativeValue* JsWindow::GetAvoidArea(NativeEngine* engine, NativeCallbackInfo* info)
{
    WLOGFD("[NAPI]GetAvoidArea");
    JsWindow* me = CheckParamsAndGetThis<JsWindow>(engine, info);
    return (me != nullptr) ? me->OnGetAvoidArea(*engine, *info) : nullptr;
}

NativeValue* JsWindow::GetWindowAvoidAreaSync(NativeEngine* engine, NativeCallbackInfo* info)
{
    WLOGFD("[NAPI]GetAvoidArea");
    JsWindow* me = CheckParamsAndGetThis<JsWindow>(engine, info);
    return (me != nullptr) ? me->OnGetWindowAvoidAreaSync(*engine, *info) : nullptr;
}

NativeValue* JsWindow::IsShowing(NativeEngine* engine, NativeCallbackInfo* info)
{
    WLOGFD("[NAPI]IsShowing");
    JsWindow* me = CheckParamsAndGetThis<JsWindow>(engine, info);
    return (me != nullptr) ? me->OnIsShowing(*engine, *info) : nullptr;
}

NativeValue* JsWindow::IsWindowShowingSync(NativeEngine* engine, NativeCallbackInfo* info)
{
    WLOGFD("[NAPI]IsShowing");
    JsWindow* me = CheckParamsAndGetThis<JsWindow>(engine, info);
    return (me != nullptr) ? me->OnIsWindowShowingSync(*engine, *info) : nullptr;
}

NativeValue* JsWindow::IsSupportWideGamut(NativeEngine* engine, NativeCallbackInfo* info)
{
    WLOGFD("[NAPI]IsSupportWideGamut");
    JsWindow* me = CheckParamsAndGetThis<JsWindow>(engine, info);
    return (me != nullptr) ? me->OnIsSupportWideGamut(*engine, *info) : nullptr;
}

NativeValue* JsWindow::IsWindowSupportWideGamut(NativeEngine* engine, NativeCallbackInfo* info)
{
    WLOGFD("[NAPI]IsSupportWideGamut");
    JsWindow* me = CheckParamsAndGetThis<JsWindow>(engine, info);
    return (me != nullptr) ? me->OnIsWindowSupportWideGamut(*engine, *info) : nullptr;
}

NativeValue* JsWindow::SetBackgroundColor(NativeEngine* engine, NativeCallbackInfo* info)
{
    WLOGFD("[NAPI]SetBackgroundColor");
    JsWindow* me = CheckParamsAndGetThis<JsWindow>(engine, info);
    return (me != nullptr) ? me->OnSetBackgroundColor(*engine, *info) : nullptr;
}

NativeValue* JsWindow::SetWindowBackgroundColorSync(NativeEngine* engine, NativeCallbackInfo* info)
{
    WLOGFD("[NAPI]SetBackgroundColor");
    JsWindow* me = CheckParamsAndGetThis<JsWindow>(engine, info);
    return (me != nullptr) ? me->OnSetWindowBackgroundColorSync(*engine, *info) : nullptr;
}

NativeValue* JsWindow::SetBrightness(NativeEngine* engine, NativeCallbackInfo* info)
{
    WLOGFD("[NAPI]SetBrightness");
    JsWindow* me = CheckParamsAndGetThis<JsWindow>(engine, info);
    return (me != nullptr) ? me->OnSetBrightness(*engine, *info) : nullptr;
}

NativeValue* JsWindow::SetWindowBrightness(NativeEngine* engine, NativeCallbackInfo* info)
{
    WLOGFD("[NAPI]SetBrightness");
    JsWindow* me = CheckParamsAndGetThis<JsWindow>(engine, info);
    return (me != nullptr) ? me->OnSetWindowBrightness(*engine, *info) : nullptr;
}

NativeValue* JsWindow::SetDimBehind(NativeEngine* engine, NativeCallbackInfo* info)
{
    WLOGFD("[NAPI]SetDimBehind");
    JsWindow* me = CheckParamsAndGetThis<JsWindow>(engine, info);
    return (me != nullptr) ? me->OnSetDimBehind(*engine, *info) : nullptr;
}

NativeValue* JsWindow::SetFocusable(NativeEngine* engine, NativeCallbackInfo* info)
{
    WLOGFD("[NAPI]SetFocusable");
    JsWindow* me = CheckParamsAndGetThis<JsWindow>(engine, info);
    return (me != nullptr) ? me->OnSetFocusable(*engine, *info) : nullptr;
}

NativeValue* JsWindow::SetWindowFocusable(NativeEngine* engine, NativeCallbackInfo* info)
{
    WLOGFD("[NAPI]SetFocusable");
    JsWindow* me = CheckParamsAndGetThis<JsWindow>(engine, info);
    return (me != nullptr) ? me->OnSetWindowFocusable(*engine, *info) : nullptr;
}

NativeValue* JsWindow::SetKeepScreenOn(NativeEngine* engine, NativeCallbackInfo* info)
{
    WLOGFD("[NAPI]SetKeepScreenOn");
    JsWindow* me = CheckParamsAndGetThis<JsWindow>(engine, info);
    return (me != nullptr) ? me->OnSetKeepScreenOn(*engine, *info) : nullptr;
}

NativeValue* JsWindow::SetWindowKeepScreenOn(NativeEngine* engine, NativeCallbackInfo* info)
{
    WLOGFD("[NAPI]SetKeepScreenOn");
    JsWindow* me = CheckParamsAndGetThis<JsWindow>(engine, info);
    return (me != nullptr) ? me->OnSetWindowKeepScreenOn(*engine, *info) : nullptr;
}

NativeValue* JsWindow::SetWakeUpScreen(NativeEngine* engine, NativeCallbackInfo* info)
{
    WLOGFD("[NAPI]SetWakeUpScreen");
    JsWindow* me = CheckParamsAndGetThis<JsWindow>(engine, info);
    return (me != nullptr) ? me->OnSetWakeUpScreen(*engine, *info) : nullptr;
}

NativeValue* JsWindow::SetOutsideTouchable(NativeEngine* engine, NativeCallbackInfo* info)
{
    WLOGFD("[NAPI]SetOutsideTouchable");
    JsWindow* me = CheckParamsAndGetThis<JsWindow>(engine, info);
    return (me != nullptr) ? me->OnSetOutsideTouchable(*engine, *info) : nullptr;
}

NativeValue* JsWindow::SetPrivacyMode(NativeEngine* engine, NativeCallbackInfo* info)
{
    WLOGFD("[NAPI]SetPrivacyMode");
    JsWindow* me = CheckParamsAndGetThis<JsWindow>(engine, info);
    return (me != nullptr) ? me->OnSetPrivacyMode(*engine, *info) : nullptr;
}

NativeValue* JsWindow::SetWindowPrivacyMode(NativeEngine* engine, NativeCallbackInfo* info)
{
    WLOGFD("[NAPI]SetPrivacyMode");
    JsWindow* me = CheckParamsAndGetThis<JsWindow>(engine, info);
    return (me != nullptr) ? me->OnSetWindowPrivacyMode(*engine, *info) : nullptr;
}

NativeValue* JsWindow::SetTouchable(NativeEngine* engine, NativeCallbackInfo* info)
{
    WLOGFD("[NAPI]SetTouchable");
    JsWindow* me = CheckParamsAndGetThis<JsWindow>(engine, info);
    return (me != nullptr) ? me->OnSetTouchable(*engine, *info) : nullptr;
}

NativeValue* JsWindow::SetWindowTouchable(NativeEngine* engine, NativeCallbackInfo* info)
{
    WLOGFD("[NAPI]SetTouchable");
    JsWindow* me = CheckParamsAndGetThis<JsWindow>(engine, info);
    return (me != nullptr) ? me->OnSetWindowTouchable(*engine, *info) : nullptr;
}

NativeValue* JsWindow::SetTransparent(NativeEngine* engine, NativeCallbackInfo* info)
{
    WLOGFD("[NAPI]SetTransparent");
    JsWindow* me = CheckParamsAndGetThis<JsWindow>(engine, info);
    return (me != nullptr) ? me->OnSetTransparent(*engine, *info) : nullptr;
}

NativeValue* JsWindow::SetCallingWindow(NativeEngine* engine, NativeCallbackInfo* info)
{
    WLOGFD("[NAPI]SetCallingWindow");
    JsWindow* me = CheckParamsAndGetThis<JsWindow>(engine, info);
    return (me != nullptr) ? me->OnSetCallingWindow(*engine, *info) : nullptr;
}

NativeValue* JsWindow::SetPreferredOrientation(NativeEngine* engine, NativeCallbackInfo* info)
{
    WLOGFD("[NAPI]SetPreferredOrientation");
    JsWindow* me = CheckParamsAndGetThis<JsWindow>(engine, info);
    return (me != nullptr) ? me->OnSetPreferredOrientation(*engine, *info) : nullptr;
}

NativeValue* JsWindow::SetSnapshotSkip(NativeEngine* engine, NativeCallbackInfo* info)
{
    WLOGFD("NAPI");
    JsWindow* me = CheckParamsAndGetThis<JsWindow>(engine, info);
    return (me != nullptr) ? me->OnSetSnapshotSkip(*engine, *info) : nullptr;
}

NativeValue* JsWindow::DisableWindowDecor(NativeEngine* engine, NativeCallbackInfo* info)
{
    WLOGFD("[NAPI]DisableWindowDecor");
    JsWindow* me = CheckParamsAndGetThis<JsWindow>(engine, info);
    return (me != nullptr) ? me->OnDisableWindowDecor(*engine, *info) : nullptr;
}

NativeValue* JsWindow::SetColorSpace(NativeEngine* engine, NativeCallbackInfo* info)
{
    WLOGFD("[NAPI]SetColorSpace");
    JsWindow* me = CheckParamsAndGetThis<JsWindow>(engine, info);
    return (me != nullptr) ? me->OnSetColorSpace(*engine, *info) : nullptr;
}

NativeValue* JsWindow::SetWindowColorSpace(NativeEngine* engine, NativeCallbackInfo* info)
{
    WLOGFD("[NAPI]SetColorSpace");
    JsWindow* me = CheckParamsAndGetThis<JsWindow>(engine, info);
    return (me != nullptr) ? me->OnSetWindowColorSpace(*engine, *info) : nullptr;
}

NativeValue* JsWindow::GetColorSpace(NativeEngine* engine, NativeCallbackInfo* info)
{
    WLOGFD("[NAPI]GetColorSpace");
    JsWindow* me = CheckParamsAndGetThis<JsWindow>(engine, info);
    return (me != nullptr) ? me->OnGetColorSpace(*engine, *info) : nullptr;
}

NativeValue* JsWindow::GetWindowColorSpaceSync(NativeEngine* engine, NativeCallbackInfo* info)
{
    WLOGFD("[NAPI]GetColorSpace");
    JsWindow* me = CheckParamsAndGetThis<JsWindow>(engine, info);
    return (me != nullptr) ? me->OnGetWindowColorSpaceSync(*engine, *info) : nullptr;
}

NativeValue* JsWindow::Dump(NativeEngine* engine, NativeCallbackInfo* info)
{
    WLOGFD("[NAPI]Dump");
    JsWindow* me = CheckParamsAndGetThis<JsWindow>(engine, info);
    return (me != nullptr) ? me->OnDump(*engine, *info) : nullptr;
}

NativeValue* JsWindow::SetForbidSplitMove(NativeEngine* engine, NativeCallbackInfo* info)
{
    WLOGFD("[NAPI]SetForbidSplitMove");
    JsWindow* me = CheckParamsAndGetThis<JsWindow>(engine, info);
    return (me != nullptr) ? me->OnSetForbidSplitMove(*engine, *info) : nullptr;
}

NativeValue* JsWindow::Opacity(NativeEngine* engine, NativeCallbackInfo* info)
{
    WLOGFD("[NAPI]Opacity");
    JsWindow* me = CheckParamsAndGetThis<JsWindow>(engine, info);
    return (me != nullptr) ? me->OnOpacity(*engine, *info) : nullptr;
}

NativeValue* JsWindow::Scale(NativeEngine* engine, NativeCallbackInfo* info)
{
    WLOGFD("[NAPI]Scale");
    JsWindow* me = CheckParamsAndGetThis<JsWindow>(engine, info);
    return (me != nullptr) ? me->OnScale(*engine, *info) : nullptr;
}

NativeValue* JsWindow::Rotate(NativeEngine* engine, NativeCallbackInfo* info)
{
    WLOGFD("[NAPI]Rotate");
    JsWindow* me = CheckParamsAndGetThis<JsWindow>(engine, info);
    return (me != nullptr) ? me->OnRotate(*engine, *info) : nullptr;
}

NativeValue* JsWindow::Translate(NativeEngine* engine, NativeCallbackInfo* info)
{
    WLOGFD("[NAPI]Translate");
    JsWindow* me = CheckParamsAndGetThis<JsWindow>(engine, info);
    return (me != nullptr) ? me->OnTranslate(*engine, *info) : nullptr;
}

NativeValue* JsWindow::GetTransitionController(NativeEngine* engine, NativeCallbackInfo* info)
{
    WLOGFD("[NAPI]GetTransitionController");
    JsWindow* me = CheckParamsAndGetThis<JsWindow>(engine, info);
    return (me != nullptr) ? me->OnGetTransitionController(*engine, *info) : nullptr;
}

NativeValue* JsWindow::SetCornerRadius(NativeEngine* engine, NativeCallbackInfo* info)
{
    WLOGFD("[NAPI]SetCornerRadius");
    JsWindow* me = CheckParamsAndGetThis<JsWindow>(engine, info);
    return (me != nullptr) ? me->OnSetCornerRadius(*engine, *info) : nullptr;
}

NativeValue* JsWindow::SetShadow(NativeEngine* engine, NativeCallbackInfo* info)
{
    WLOGFD("[NAPI]SetShadow");
    JsWindow* me = CheckParamsAndGetThis<JsWindow>(engine, info);
    return (me != nullptr) ? me->OnSetShadow(*engine, *info) : nullptr;
}

NativeValue* JsWindow::SetBlur(NativeEngine* engine, NativeCallbackInfo* info)
{
    WLOGFD("[NAPI]SetBlur");
    JsWindow* me = CheckParamsAndGetThis<JsWindow>(engine, info);
    return (me != nullptr) ? me->OnSetBlur(*engine, *info) : nullptr;
}

NativeValue* JsWindow::SetBackdropBlur(NativeEngine* engine, NativeCallbackInfo* info)
{
    WLOGFD("[NAPI]SetBackdropBlur");
    JsWindow* me = CheckParamsAndGetThis<JsWindow>(engine, info);
    return (me != nullptr) ? me->OnSetBackdropBlur(*engine, *info) : nullptr;
}

NativeValue* JsWindow::SetBackdropBlurStyle(NativeEngine* engine, NativeCallbackInfo* info)
{
    WLOGFD("[NAPI]SetBackdropBlurStyle");
    JsWindow* me = CheckParamsAndGetThis<JsWindow>(engine, info);
    return (me != nullptr) ? me->OnSetBackdropBlurStyle(*engine, *info) : nullptr;
}

NativeValue* JsWindow::OnShow(NativeEngine& engine, NativeCallbackInfo& info)
{
    WMError errCode = WMError::WM_OK;
    if (info.argc > 1) {
        WLOGFE("[NAPI]Argc is invalid: %{public}zu", info.argc);
        errCode = WMError::WM_ERROR_INVALID_PARAM;
    }
    wptr<Window> weakToken(windowToken_);
    AsyncTask::CompleteCallback complete =
        [weakToken, errCode](NativeEngine& engine, AsyncTask& task, int32_t status) {
            auto weakWindow = weakToken.promote();
            if (weakWindow == nullptr || errCode != WMError::WM_OK) {
                task.Reject(engine, CreateJsError(engine, static_cast<int32_t>(errCode)));
                WLOGFE("[NAPI]window is nullptr or get invalid param");
                return;
            }
            WMError ret = weakWindow->Show(0, false);
            if (ret == WMError::WM_OK) {
                task.Resolve(engine, engine.CreateUndefined());
            } else {
                task.Reject(engine, CreateJsError(engine, static_cast<int32_t>(ret), "Window show failed"));
            }
            WLOGFD("[NAPI]Window [%{public}u, %{public}s] show end, ret = %{public}d",
                weakWindow->GetWindowId(), weakWindow->GetWindowName().c_str(), ret);
        };
    NativeValue* result = nullptr;
    NativeValue* lastParam = (info.argc == 0) ? nullptr :
        (info.argv[0]->TypeOf() == NATIVE_FUNCTION ? info.argv[0] : nullptr);
    AsyncTask::Schedule("JsWindow::OnShow",
        engine, CreateAsyncTaskWithLastParam(engine, lastParam, nullptr, std::move(complete), &result));
    return result;
}

NativeValue* JsWindow::OnShowWindow(NativeEngine& engine, NativeCallbackInfo& info)
{
    wptr<Window> weakToken(windowToken_);
    AsyncTask::CompleteCallback complete =
        [weakToken](NativeEngine& engine, AsyncTask& task, int32_t status) {
            auto weakWindow = weakToken.promote();
            if (weakWindow == nullptr) {
                task.Reject(engine, CreateJsError(engine,
                    static_cast<int32_t>(WmErrorCode::WM_ERROR_STATE_ABNORMALLY)));
                WLOGFE("[NAPI]window is nullptr or get invalid param");
                return;
            }
            WMError ret = weakWindow->Show(0, false);
            if (ret == WMError::WM_OK) {
                task.Resolve(engine, engine.CreateUndefined());
            } else {
                task.Reject(engine, CreateJsError(engine,
                    static_cast<int32_t>(WM_JS_TO_ERROR_CODE_MAP.at(ret)), "Window show failed"));
            }
            WLOGFD("[NAPI]Window [%{public}u, %{public}s] show end, ret = %{public}d",
                weakWindow->GetWindowId(), weakWindow->GetWindowName().c_str(), ret);
        };
    NativeValue* result = nullptr;
    NativeValue* lastParam = (info.argc == 0) ? nullptr :
        ((info.argv[0] != nullptr && info.argv[0]->TypeOf() == NATIVE_FUNCTION) ?
        info.argv[0] : nullptr);
    AsyncTask::Schedule("JsWindow::OnShow",
        engine, CreateAsyncTaskWithLastParam(engine, lastParam, nullptr, std::move(complete), &result));
    return result;
}

NativeValue* JsWindow::OnShowWithAnimation(NativeEngine& engine, NativeCallbackInfo& info)
{
    WmErrorCode errCode = WmErrorCode::WM_OK;
    auto winType = windowToken_->GetType();
    if (!WindowHelper::IsSystemWindow(winType)) {
        WLOGFE("[NAPI]window Type %{public}u is not supported", static_cast<uint32_t>(winType));
        errCode = WmErrorCode::WM_ERROR_INVALID_CALLING;
    }
    wptr<Window> weakToken(windowToken_);
    AsyncTask::CompleteCallback complete =
        [weakToken, errCode](NativeEngine& engine, AsyncTask& task, int32_t status) {
            if (errCode != WmErrorCode::WM_OK) {
                task.Reject(engine, CreateJsError(engine, static_cast<int32_t>(errCode)));
                return;
            }
            auto weakWindow = weakToken.promote();
            if (weakWindow == nullptr) {
                WLOGFE("[NAPI]window is nullptr");
                task.Reject(engine, CreateJsError(engine,
                    static_cast<int32_t>(WmErrorCode::WM_ERROR_STATE_ABNORMALLY)));
                return;
            }
            WmErrorCode ret = WM_JS_TO_ERROR_CODE_MAP.at(weakWindow->Show(0, true));
            if (ret == WmErrorCode::WM_OK) {
                task.Resolve(engine, engine.CreateUndefined());
            } else {
                task.Reject(engine, CreateJsError(engine, static_cast<int32_t>(ret), "Window show failed"));
            }
            WLOGFD("[NAPI]Window [%{public}u, %{public}s] ShowWithAnimation end, ret = %{public}d",
                weakWindow->GetWindowId(), weakWindow->GetWindowName().c_str(), ret);
        };
    NativeValue* result = nullptr;
    NativeValue* lastParam = (info.argc == 0) ? nullptr :
        ((info.argv[0] != nullptr && info.argv[0]->TypeOf() == NATIVE_FUNCTION) ?
        info.argv[0] : nullptr);
    AsyncTask::Schedule("JsWindow::OnShowWithAnimation",
        engine, CreateAsyncTaskWithLastParam(engine, lastParam, nullptr, std::move(complete), &result));
    return result;
}

NativeValue* JsWindow::OnDestroy(NativeEngine& engine, NativeCallbackInfo& info)
{
    WMError errCode = WMError::WM_OK;
    if (info.argc > 1) {
        WLOGFE("[NAPI]Argc is invalid: %{public}zu", info.argc);
        errCode = WMError::WM_ERROR_INVALID_PARAM;
    }
    wptr<Window> weakToken(windowToken_);
    AsyncTask::CompleteCallback complete =
        [this, weakToken, errCode](NativeEngine& engine, AsyncTask& task, int32_t status) {
            auto weakWindow = weakToken.promote();
            if (weakWindow == nullptr || errCode != WMError::WM_OK) {
                task.Reject(engine, CreateJsError(engine, static_cast<int32_t>(errCode)));
                WLOGFE("[NAPI]window is nullptr or get invalid param");
                return;
            }
            WMError ret = weakWindow->Destroy();
            WLOGFD("[NAPI]Window [%{public}u, %{public}s] destroy end, ret = %{public}d",
                weakWindow->GetWindowId(), weakWindow->GetWindowName().c_str(), ret);
            if (ret != WMError::WM_OK) {
                task.Reject(engine, CreateJsError(engine, static_cast<int32_t>(ret), "Window destroy failed"));
                return;
            }
            windowToken_ = nullptr; // ensure window dtor when finalizer invalid
            task.Resolve(engine, engine.CreateUndefined());
        };

    NativeValue* lastParam = (info.argc == 0) ? nullptr :
        (info.argv[0]->TypeOf() == NATIVE_FUNCTION ? info.argv[0] : nullptr);
    NativeValue* result = nullptr;
    AsyncTask::Schedule("JsWindow::OnDestroy",
        engine, CreateAsyncTaskWithLastParam(engine, lastParam, nullptr, std::move(complete), &result));
    return result;
}

NativeValue* JsWindow::OnDestroyWindow(NativeEngine& engine, NativeCallbackInfo& info)
{
    wptr<Window> weakToken(windowToken_);
    AsyncTask::CompleteCallback complete =
        [this, weakToken](NativeEngine& engine, AsyncTask& task, int32_t status) {
            auto weakWindow = weakToken.promote();
            if (weakWindow == nullptr) {
                WLOGFE("[NAPI]window is nullptr or get invalid param");
                task.Reject(engine,
                    CreateJsError(engine, static_cast<int32_t>(WmErrorCode::WM_ERROR_STATE_ABNORMALLY)));
                return;
            }
            WmErrorCode ret = WM_JS_TO_ERROR_CODE_MAP.at(weakWindow->Destroy());
            WLOGFD("[NAPI]Window [%{public}u, %{public}s] destroy end, ret = %{public}d",
                weakWindow->GetWindowId(), weakWindow->GetWindowName().c_str(), ret);
            if (ret != WmErrorCode::WM_OK) {
                task.Reject(engine,
                    CreateJsError(engine, static_cast<int32_t>(ret),
                    "Window destroy failed"));
                return;
            }
            windowToken_ = nullptr; // ensure window dtor when finalizer invalid
            task.Resolve(engine, engine.CreateUndefined());
        };

    NativeValue* lastParam = (info.argc == 0) ? nullptr :
        ((info.argv[0] != nullptr && info.argv[0]->TypeOf() == NATIVE_FUNCTION) ?
        info.argv[0] : nullptr);
    NativeValue* result = nullptr;
    AsyncTask::Schedule("JsWindow::OnDestroyWindow",
        engine, CreateAsyncTaskWithLastParam(engine, lastParam, nullptr, std::move(complete), &result));
    return result;
}

NativeValue* JsWindow::OnHide(NativeEngine& engine, NativeCallbackInfo& info)
{
    wptr<Window> weakToken(windowToken_);
    AsyncTask::CompleteCallback complete =
        [weakToken](NativeEngine& engine, AsyncTask& task, int32_t status) {
            auto weakWindow = weakToken.promote();
            if (weakWindow == nullptr) {
                WLOGFE("[NAPI]window is nullptr or get invalid param");
                task.Reject(engine,
                    CreateJsError(engine, static_cast<int32_t>(WmErrorCode::WM_ERROR_STATE_ABNORMALLY)));
                return;
            }
            WmErrorCode ret = WM_JS_TO_ERROR_CODE_MAP.at(weakWindow->Hide(0, false));
            if (ret == WmErrorCode::WM_OK) {
                task.Resolve(engine, engine.CreateUndefined());
            } else {
                task.Reject(engine, CreateJsError(engine, static_cast<int32_t>(ret), "Window hide failed"));
            }
            WLOGFD("[NAPI]Window [%{public}u, %{public}s] hide end, ret = %{public}d",
                weakWindow->GetWindowId(), weakWindow->GetWindowName().c_str(), ret);
        };

    NativeValue* result = nullptr;
    NativeValue* lastParam = (info.argc == 0) ? nullptr :
        (info.argv[0] != nullptr && info.argv[0]->TypeOf() == NATIVE_FUNCTION ? info.argv[0] : nullptr);
    AsyncTask::Schedule("JsWindow::OnHide",
        engine, CreateAsyncTaskWithLastParam(engine, lastParam, nullptr, std::move(complete), &result));
    return result;
}

NativeValue* JsWindow::OnHideWithAnimation(NativeEngine& engine, NativeCallbackInfo& info)
{
    WmErrorCode errCode = WmErrorCode::WM_OK;
    if (windowToken_) {
        auto winType = windowToken_->GetType();
        if (!WindowHelper::IsSystemWindow(winType)) {
            WLOGFE("[NAPI]window Type %{public}u is not supported", static_cast<uint32_t>(winType));
            errCode = WmErrorCode::WM_ERROR_INVALID_CALLING;
        }
    } else {
        errCode = WmErrorCode::WM_ERROR_INVALID_CALLING;
    }
    wptr<Window> weakToken(windowToken_);
    AsyncTask::CompleteCallback complete =
        [weakToken, errCode](NativeEngine& engine, AsyncTask& task, int32_t status) {
            if (errCode != WmErrorCode::WM_OK) {
                task.Reject(engine,
                    CreateJsError(engine, static_cast<int32_t>(errCode)));
                return;
            }
            auto weakWindow = weakToken.promote();
            if (weakWindow == nullptr) {
                WLOGFE("[NAPI]window is nullptr");
                task.Reject(engine,
                    CreateJsError(engine, static_cast<int32_t>(WmErrorCode::WM_ERROR_STATE_ABNORMALLY)));
                return;
            }
            WmErrorCode ret = WM_JS_TO_ERROR_CODE_MAP.at(weakWindow->Hide(0, true));
            if (ret == WmErrorCode::WM_OK) {
                task.Resolve(engine, engine.CreateUndefined());
            } else {
                task.Reject(engine, CreateJsError(engine, static_cast<int32_t>(ret), "Window show failed"));
            }
            WLOGFD("[NAPI]Window [%{public}u, %{public}s] HideWithAnimation end, ret = %{public}d",
                weakWindow->GetWindowId(), weakWindow->GetWindowName().c_str(), ret);
        };
    NativeValue* result = nullptr;
    NativeValue* lastParam = (info.argc == 0) ? nullptr :
        ((info.argv[0] != nullptr && info.argv[0]->TypeOf() == NATIVE_FUNCTION) ?
        info.argv[0] : nullptr);
    AsyncTask::Schedule("JsWindow::OnHideWithAnimation",
        engine, CreateAsyncTaskWithLastParam(engine, lastParam, nullptr, std::move(complete), &result));
    return result;
}

NativeValue* JsWindow::OnMoveTo(NativeEngine& engine, NativeCallbackInfo& info)
{
    WMError errCode = WMError::WM_OK;
    if (info.argc < 2 || info.argc > 3) { // 2:minimum param num, 3: maximum param num
        WLOGFE("[NAPI]Argc is invalid: %{public}zu", info.argc);
        errCode = WMError::WM_ERROR_INVALID_PARAM;
    }
    int32_t x = 0;
    if (errCode == WMError::WM_OK && !ConvertFromJsValue(engine, info.argv[0], x)) {
        WLOGFE("[NAPI]Failed to convert parameter to x");
        errCode = WMError::WM_ERROR_INVALID_PARAM;
    }

    int32_t y = 0;
    if (errCode == WMError::WM_OK && !ConvertFromJsValue(engine, info.argv[1], y)) {
        WLOGFE("[NAPI]Failed to convert parameter to y");
        errCode = WMError::WM_ERROR_INVALID_PARAM;
    }

    wptr<Window> weakToken(windowToken_);
    AsyncTask::CompleteCallback complete =
        [weakToken, errCode, x, y](NativeEngine& engine, AsyncTask& task, int32_t status) {
            auto weakWindow = weakToken.promote();
            if (weakWindow == nullptr || errCode != WMError::WM_OK) {
                task.Reject(engine, CreateJsError(engine, static_cast<int32_t>(errCode)));
                WLOGFE("[NAPI]window is nullptr or get invalid param");
                return;
            }
            WMError ret = weakWindow->MoveTo(x, y);
            if (ret == WMError::WM_OK) {
                task.Resolve(engine, engine.CreateUndefined());
            } else {
                task.Reject(engine, CreateJsError(engine, static_cast<int32_t>(ret), "Window move failed"));
            }
            WLOGFD("[NAPI]Window [%{public}u, %{public}s] move end, ret = %{public}d",
                weakWindow->GetWindowId(), weakWindow->GetWindowName().c_str(), ret);
        };
    // 2: params num; 2: index of callback
    NativeValue* lastParam = (info.argc <= 2) ? nullptr :
        (info.argv[2]->TypeOf() == NATIVE_FUNCTION ? info.argv[2] : nullptr);
    NativeValue* result = nullptr;
    AsyncTask::Schedule("JsWindow::OnMoveTo",
        engine, CreateAsyncTaskWithLastParam(engine, lastParam, nullptr, std::move(complete), &result));
    return result;
}

NativeValue* JsWindow::OnMoveWindowTo(NativeEngine& engine, NativeCallbackInfo& info)
{
    WmErrorCode errCode = WmErrorCode::WM_OK;
    if (info.argc < 2) { // 2:minimum param num
        WLOGFE("[NAPI]Argc is invalid: %{public}zu", info.argc);
        errCode = WmErrorCode::WM_ERROR_INVALID_PARAM;
    }
    int32_t x = 0;
    if (errCode == WmErrorCode::WM_OK && !ConvertFromJsValue(engine, info.argv[0], x)) {
        WLOGFE("[NAPI]Failed to convert parameter to x");
        errCode = WmErrorCode::WM_ERROR_INVALID_PARAM;
    }
    int32_t y = 0;
    if (errCode == WmErrorCode::WM_OK && !ConvertFromJsValue(engine, info.argv[1], y)) {
        WLOGFE("[NAPI]Failed to convert parameter to y");
        errCode = WmErrorCode::WM_ERROR_INVALID_PARAM;
    }
    if (errCode == WmErrorCode::WM_ERROR_INVALID_PARAM) {
        engine.Throw(CreateJsError(engine, static_cast<int32_t>(WmErrorCode::WM_ERROR_INVALID_PARAM)));
        return engine.CreateUndefined();
    }

    wptr<Window> weakToken(windowToken_);
    AsyncTask::CompleteCallback complete =
        [weakToken, x, y](NativeEngine& engine, AsyncTask& task, int32_t status) {
            auto weakWindow = weakToken.promote();
            if (weakWindow == nullptr) {
                WLOGFE("[NAPI]window is nullptr");
                task.Reject(engine, CreateJsError(engine,
                    static_cast<int32_t>(WmErrorCode::WM_ERROR_STATE_ABNORMALLY)));
                return;
            }
            WmErrorCode ret = WM_JS_TO_ERROR_CODE_MAP.at(weakWindow->MoveTo(x, y));
            if (ret == WmErrorCode::WM_OK) {
                task.Resolve(engine, engine.CreateUndefined());
            } else {
                task.Reject(engine,
                    CreateJsError(engine, static_cast<int32_t>(ret),
                    "Window move failed"));
            }
            WLOGFD("[NAPI]Window [%{public}u, %{public}s] move end, ret = %{public}d",
                weakWindow->GetWindowId(), weakWindow->GetWindowName().c_str(), ret);
        };

    // 2: params num; 2: index of callback
    NativeValue* lastParam = (info.argc <= 2) ? nullptr :
        ((info.argv[2] != nullptr && info.argv[2]->TypeOf() == NATIVE_FUNCTION) ?
        info.argv[2] : nullptr);
    NativeValue* result = nullptr;
    AsyncTask::Schedule("JsWindow::OnMoveWindowTo",
        engine, CreateAsyncTaskWithLastParam(engine, lastParam, nullptr, std::move(complete), &result));
    return result;
}

NativeValue* JsWindow::OnResize(NativeEngine& engine, NativeCallbackInfo& info)
{
    WMError errCode = WMError::WM_OK;
    if (info.argc < 2 || info.argc > 3) { // 2: minimum param num, 3: maximum param num
        WLOGFE("[NAPI]Argc is invalid: %{public}zu", info.argc);
        errCode = WMError::WM_ERROR_INVALID_PARAM;
    }
    int32_t width = 0;
    if (errCode == WMError::WM_OK && !ConvertFromJsValue(engine, info.argv[0], width)) {
        WLOGFE("[NAPI]Failed to convert parameter to width");
        errCode = WMError::WM_ERROR_INVALID_PARAM;
    }
    int32_t height = 0;
    if (errCode == WMError::WM_OK && !ConvertFromJsValue(engine, info.argv[1], height)) {
        WLOGFE("[NAPI]Failed to convert parameter to height");
        errCode = WMError::WM_ERROR_INVALID_PARAM;
    }
    if (width <= 0 || height <= 0) {
        WLOGFE("[NAPI]width or height should greater than 0!");
        errCode = WMError::WM_ERROR_INVALID_PARAM;
    }
    wptr<Window> weakToken(windowToken_);
    AsyncTask::CompleteCallback complete =
        [weakToken, errCode, width, height](NativeEngine& engine, AsyncTask& task, int32_t status) {
            auto weakWindow = weakToken.promote();
            if (weakWindow == nullptr || errCode != WMError::WM_OK) {
                task.Reject(engine, CreateJsError(engine, static_cast<int32_t>(errCode)));
                WLOGFE("[NAPI]window is nullptr or get invalid param");
                return;
            }
            WMError ret = weakWindow->Resize(static_cast<uint32_t>(width), static_cast<uint32_t>(height));
            if (ret == WMError::WM_OK) {
                task.Resolve(engine, engine.CreateUndefined());
            } else {
                task.Reject(engine, CreateJsError(engine, static_cast<int32_t>(ret), "Window resize failed"));
            }
            WLOGFD("[NAPI]Window [%{public}u, %{public}s] resize end, ret = %{public}d",
                weakWindow->GetWindowId(), weakWindow->GetWindowName().c_str(), ret);
        };
    // 2: params num; 2: index of callback
    NativeValue* lastParam = (info.argc <= 2) ? nullptr :
        (info.argv[2]->TypeOf() == NATIVE_FUNCTION ? info.argv[2] : nullptr);
    NativeValue* result = nullptr;
    AsyncTask::Schedule("JsWindow::OnResize",
        engine, CreateAsyncTaskWithLastParam(engine, lastParam, nullptr, std::move(complete), &result));
    return result;
}

NativeValue* JsWindow::OnResizeWindow(NativeEngine& engine, NativeCallbackInfo& info)
{
    WmErrorCode errCode = WmErrorCode::WM_OK;
    if (info.argc < 2) { // 2: minimum param num
        WLOGFE("[NAPI]Argc is invalid: %{public}zu", info.argc);
        errCode = WmErrorCode::WM_ERROR_INVALID_PARAM;
    }
    int32_t width = 0;
    if (errCode == WmErrorCode::WM_OK && !ConvertFromJsValue(engine, info.argv[0], width)) {
        WLOGFE("[NAPI]Failed to convert parameter to width");
        errCode = WmErrorCode::WM_ERROR_INVALID_PARAM;
    }
    int32_t height = 0;
    if (errCode == WmErrorCode::WM_OK && !ConvertFromJsValue(engine, info.argv[1], height)) {
        WLOGFE("[NAPI]Failed to convert parameter to height");
        errCode = WmErrorCode::WM_ERROR_INVALID_PARAM;
    }
    if (width <= 0 || height <= 0) {
        WLOGFE("[NAPI]width or height should greater than 0!");
        errCode = WmErrorCode::WM_ERROR_INVALID_PARAM;
    }
    if (errCode == WmErrorCode::WM_ERROR_INVALID_PARAM) {
        engine.Throw(CreateJsError(engine, static_cast<int32_t>(WmErrorCode::WM_ERROR_INVALID_PARAM)));
        return engine.CreateUndefined();
    }

    wptr<Window> weakToken(windowToken_);
    AsyncTask::CompleteCallback complete =
        [weakToken, width, height](NativeEngine& engine, AsyncTask& task, int32_t status) {
            auto weakWindow = weakToken.promote();
            if (weakWindow == nullptr) {
                task.Reject(engine,
                    CreateJsError(engine, static_cast<int32_t>(WmErrorCode::WM_ERROR_STATE_ABNORMALLY)));
                return;
            }
            WmErrorCode ret = WM_JS_TO_ERROR_CODE_MAP.at(
                weakWindow->Resize(static_cast<uint32_t>(width), static_cast<uint32_t>(height)));
            if (ret == WmErrorCode::WM_OK) {
                task.Resolve(engine, engine.CreateUndefined());
            } else {
                task.Reject(engine, CreateJsError(engine, static_cast<int32_t>(ret), "Window resize failed"));
            }
            WLOGFD("[NAPI]Window [%{public}u, %{public}s] resize end, ret = %{public}d",
                weakWindow->GetWindowId(), weakWindow->GetWindowName().c_str(), ret);
        };

    // 2: params num; 2: index of callback
    NativeValue* lastParam = (info.argc <= 2) ? nullptr :
        ((info.argv[2] != nullptr && info.argv[2]->TypeOf() == NATIVE_FUNCTION) ?
        info.argv[2] : nullptr);
    NativeValue* result = nullptr;
    AsyncTask::Schedule("JsWindow::OnResizeWindow",
        engine, CreateAsyncTaskWithLastParam(engine, lastParam, nullptr, std::move(complete), &result));
    return result;
}

NativeValue* JsWindow::OnSetWindowType(NativeEngine& engine, NativeCallbackInfo& info)
{
    WMError errCode = WMError::WM_OK;
    if (info.argc < 1 || info.argc > 2) { // 2 is max num of argc
        WLOGFE("[NAPI]Argc is invalid: %{public}zu", info.argc);
        errCode = WMError::WM_ERROR_INVALID_PARAM;
    }
    WindowType winType = WindowType::SYSTEM_WINDOW_BASE;
    if (errCode == WMError::WM_OK) {
        NativeNumber* nativeType = ConvertNativeValueTo<NativeNumber>(info.argv[0]);
        if (nativeType == nullptr) {
            WLOGFE("[NAPI]Failed to convert parameter to windowType");
            errCode = WMError::WM_ERROR_INVALID_PARAM;
        } else if (static_cast<uint32_t>(*nativeType) >= static_cast<uint32_t>(WindowType::SYSTEM_WINDOW_BASE)) {
            winType = static_cast<WindowType>(static_cast<uint32_t>(*nativeType)); // adapt to the old version
        } else {
            if (JS_TO_NATIVE_WINDOW_TYPE_MAP.count(
                static_cast<ApiWindowType>(static_cast<uint32_t>(*nativeType))) != 0) {
                winType = JS_TO_NATIVE_WINDOW_TYPE_MAP.at(
                    static_cast<ApiWindowType>(static_cast<uint32_t>(*nativeType)));
            } else {
                WLOGFE("[NAPI]Do not surppot this type: %{public}u", static_cast<uint32_t>(*nativeType));
                errCode = WMError::WM_ERROR_INVALID_PARAM;
            }
        }
    }

    wptr<Window> weakToken(windowToken_);
    AsyncTask::CompleteCallback complete =
        [weakToken, winType, errCode](NativeEngine& engine, AsyncTask& task, int32_t status) {
            auto weakWindow = weakToken.promote();
            if (weakWindow == nullptr || errCode != WMError::WM_OK) {
                task.Reject(engine, CreateJsError(engine, static_cast<int32_t>(errCode)));
                WLOGFE("[NAPI]window is nullptr or get invalid param");
                return;
            }
            WMError ret = weakWindow->SetWindowType(winType);
            if (ret == WMError::WM_OK) {
                task.Resolve(engine, engine.CreateUndefined());
            } else {
                task.Reject(engine, CreateJsError(engine, static_cast<int32_t>(ret), "Window set type failed"));
            }
            WLOGFD("[NAPI]Window [%{public}u, %{public}s] set type end, ret = %{public}d",
                weakWindow->GetWindowId(), weakWindow->GetWindowName().c_str(), ret);
        };

    NativeValue* lastParam = (info.argc <= 1) ? nullptr :
        (info.argv[1]->TypeOf() == NATIVE_FUNCTION ? info.argv[1] : nullptr);
    NativeValue* result = nullptr;
    AsyncTask::Schedule("JsWindow::OnSetWindowType",
        engine, CreateAsyncTaskWithLastParam(engine, lastParam, nullptr, std::move(complete), &result));
    return result;
}

NativeValue* JsWindow::OnSetWindowMode(NativeEngine& engine, NativeCallbackInfo& info)
{
    if (!Permission::IsSystemCalling()) {
        WLOGFE("set window mode permission denied!");
        engine.Throw(CreateJsError(engine, static_cast<int32_t>(WmErrorCode::WM_ERROR_INVALID_CALLING)));
        return engine.CreateUndefined();
    }
    WmErrorCode errCode = WmErrorCode::WM_OK;
    if (info.argc < 1) {
        errCode = WmErrorCode::WM_ERROR_INVALID_PARAM;
    }
    WindowMode winMode = WindowMode::WINDOW_MODE_FULLSCREEN;
    if (errCode == WmErrorCode::WM_OK) {
        NativeNumber* nativeMode = ConvertNativeValueTo<NativeNumber>(info.argv[0]);
        if (nativeMode == nullptr) {
            errCode = WmErrorCode::WM_ERROR_INVALID_PARAM;
        } else {
            if (static_cast<uint32_t>(*nativeMode) >= static_cast<uint32_t>(WindowMode::WINDOW_MODE_SPLIT_PRIMARY)) {
                winMode = static_cast<WindowMode>(static_cast<uint32_t>(*nativeMode));
            } else if (static_cast<uint32_t>(*nativeMode) >= static_cast<uint32_t>(ApiWindowMode::UNDEFINED) &&
                static_cast<uint32_t>(*nativeMode) <= static_cast<uint32_t>(ApiWindowMode::MODE_END)) {
                winMode = JS_TO_NATIVE_WINDOW_MODE_MAP.at(
                    static_cast<ApiWindowMode>(static_cast<uint32_t>(*nativeMode)));
            } else {
                errCode = WmErrorCode::WM_ERROR_INVALID_PARAM;
            }
        }
    }
    if (errCode == WmErrorCode::WM_ERROR_INVALID_PARAM) {
        engine.Throw(CreateJsError(engine, static_cast<int32_t>(WmErrorCode::WM_ERROR_INVALID_PARAM)));
        return engine.CreateUndefined();
    }
    wptr<Window> weakToken(windowToken_);
    AsyncTask::CompleteCallback complete =
        [weakToken, winMode](NativeEngine& engine, AsyncTask& task, int32_t status) {
            auto weakWindow = weakToken.promote();
            if (weakWindow == nullptr) {
                task.Reject(engine, CreateJsError(engine,
                    static_cast<int32_t>(WmErrorCode::WM_ERROR_STATE_ABNORMALLY)));
                return;
            }
            WmErrorCode ret = WM_JS_TO_ERROR_CODE_MAP.at(weakWindow->SetWindowMode(winMode));
            if (ret == WmErrorCode::WM_OK) {
                task.Resolve(engine, engine.CreateUndefined());
            } else {
                task.Reject(engine,
                    CreateJsError(engine, static_cast<int32_t>(ret), "Window set mode failed"));
            }
            WLOGFD("[NAPI]Window [%{public}u, %{public}s] set type end, ret = %{public}d",
                weakWindow->GetWindowId(), weakWindow->GetWindowName().c_str(), ret);
        };

    NativeValue* lastParam = (info.argc == 1) ? nullptr :
        ((info.argv[1] != nullptr && info.argv[1]->TypeOf() == NATIVE_FUNCTION) ?
        info.argv[1] : nullptr);
    NativeValue* result = nullptr;
    AsyncTask::Schedule("JsWindow::OnSetWindowMode",
        engine, CreateAsyncTaskWithLastParam(engine, lastParam, nullptr, std::move(complete), &result));
    return result;
}

NativeValue* JsWindow::OnGetProperties(NativeEngine& engine, NativeCallbackInfo& info)
{
    WMError errCode = WMError::WM_OK;
    if (info.argc > 1) {
        WLOGFE("[NAPI]Argc is invalid: %{public}zu", info.argc);
        errCode = WMError::WM_ERROR_INVALID_PARAM;
    }
    wptr<Window> weakToken(windowToken_);
    AsyncTask::CompleteCallback complete =
        [weakToken, errCode](NativeEngine& engine, AsyncTask& task, int32_t status) {
            auto weakWindow = weakToken.promote();
            if (weakWindow == nullptr || errCode != WMError::WM_OK) {
                task.Reject(engine, CreateJsError(engine, static_cast<int32_t>(errCode)));
                WLOGFE("[NAPI]window is nullptr or get invalid param");
                return;
            }
            auto objValue = CreateJsWindowPropertiesObject(engine, weakWindow);
            if (objValue != nullptr) {
                task.Resolve(engine, objValue);
            } else {
                task.Reject(engine, CreateJsError(engine,
                    static_cast<int32_t>(WMError::WM_ERROR_NULLPTR), "Window get properties failed"));
            }
            WLOGFD("[NAPI]Window [%{public}u, %{public}s] get properties end, objValue = %{public}p",
                weakWindow->GetWindowId(), weakWindow->GetWindowName().c_str(), objValue);
        };

    NativeValue* lastParam = (info.argc == 0) ? nullptr :
        (info.argv[0]->TypeOf() == NATIVE_FUNCTION ? info.argv[0] : nullptr);
    NativeValue* result = nullptr;
    AsyncTask::Schedule("JsWindow::OnGetProperties",
        engine, CreateAsyncTaskWithLastParam(engine, lastParam, nullptr, std::move(complete), &result));
    return result;
}

NativeValue* JsWindow::OnGetWindowPropertiesSync(NativeEngine& engine, NativeCallbackInfo& info)
{
    wptr<Window> weakToken(windowToken_);
    auto window = weakToken.promote();
    if (window == nullptr) {
        WLOGFE("[NAPI]window is nullptr or get invalid param");
        engine.Throw(CreateJsError(engine, static_cast<int32_t>(WmErrorCode::WM_ERROR_STATE_ABNORMALLY)));
        return engine.CreateUndefined();
    }
    auto objValue = CreateJsWindowPropertiesObject(engine, window);
    WLOGFD("[NAPI]Window [%{public}u, %{public}s] get properties end, objValue = %{public}p",
        window->GetWindowId(), window->GetWindowName().c_str(), objValue);
    if (objValue != nullptr) {
        return objValue;
    } else {
        engine.Throw(CreateJsError(engine, static_cast<int32_t>(WmErrorCode::WM_ERROR_STATE_ABNORMALLY)));
        return engine.CreateUndefined();
    }
}

NativeValue* JsWindow::OnRegisterWindowCallback(NativeEngine& engine, NativeCallbackInfo& info)
{
    if (windowToken_ == nullptr) {
        WLOGFE("[NAPI]Window is nullptr");
        engine.Throw(CreateJsError(engine, static_cast<int32_t>(WmErrorCode::WM_ERROR_INVALID_PARAM)));
        return engine.CreateUndefined();
    }
    if (info.argc < 2) { // 2: params num
        WLOGFE("[NAPI]Argc is invalid: %{public}zu", info.argc);
        engine.Throw(CreateJsError(engine, static_cast<int32_t>(WmErrorCode::WM_ERROR_INVALID_PARAM)));
        return engine.CreateUndefined();
    }
    std::string cbType;
    if (!ConvertFromJsValue(engine, info.argv[0], cbType)) {
        WLOGFE("[NAPI]Failed to convert parameter to callbackType");
        engine.Throw(CreateJsError(engine, static_cast<int32_t>(WmErrorCode::WM_ERROR_INVALID_PARAM)));
        return engine.CreateUndefined();
    }
    NativeValue* value = info.argv[1];
    if (!value->IsCallable()) {
        WLOGFD("[NAPI]Callback(info->argv[1]) is not callable");
        engine.Throw(CreateJsError(engine, static_cast<int32_t>(WmErrorCode::WM_ERROR_INVALID_PARAM)));
        return engine.CreateUndefined();
    }
    registerManager_->RegisterListener(windowToken_, cbType, CaseType::CASE_WINDOW, engine, value);
    WLOGFD("[NAPI]Register end, window [%{public}u, %{public}s], type = %{public}s, callback = %{public}p",
        windowToken_->GetWindowId(), windowToken_->GetWindowName().c_str(), cbType.c_str(), value);
    return engine.CreateUndefined();
}

NativeValue* JsWindow::OnUnregisterWindowCallback(NativeEngine& engine, NativeCallbackInfo& info)
{
    if (windowToken_ == nullptr || info.argc < 1) { // 2: maximum params nums
        WLOGFE("[NAPI]Window is nullptr or argc is invalid: %{public}zu", info.argc);
        engine.Throw(CreateJsError(engine, static_cast<int32_t>(WmErrorCode::WM_ERROR_INVALID_PARAM)));
        return engine.CreateUndefined();
    }
    std::string cbType;
    if (!ConvertFromJsValue(engine, info.argv[0], cbType)) {
        WLOGFE("[NAPI]Failed to convert parameter to callbackType");
        engine.Throw(CreateJsError(engine, static_cast<int32_t>(WmErrorCode::WM_ERROR_INVALID_PARAM)));
        return engine.CreateUndefined();
    }

    NativeValue* value = nullptr;
    if (info.argc == 1) {
        registerManager_->UnregisterListener(windowToken_, cbType, CaseType::CASE_WINDOW, value);
    } else {
        value = info.argv[1];
        if (value == nullptr || !value->IsCallable()) {
            registerManager_->UnregisterListener(windowToken_, cbType, CaseType::CASE_WINDOW, nullptr);
        } else {
            registerManager_->UnregisterListener(windowToken_, cbType, CaseType::CASE_WINDOW, value);
        }
    }

    WLOGFD("[NAPI]Unregister end, window [%{public}u, %{public}s], type = %{public}s, callback = %{public}p",
        windowToken_->GetWindowId(), windowToken_->GetWindowName().c_str(), cbType.c_str(), value);
    return engine.CreateUndefined();
}

NativeValue* JsWindow::OnBindDialogTarget(NativeEngine& engine, NativeCallbackInfo& info)
{
    WmErrorCode errCode = WmErrorCode::WM_OK;
    if (windowToken_ == nullptr || info.argc <= 1) { // 1: invalid params nums
        errCode = WmErrorCode::WM_ERROR_INVALID_PARAM;
    }

    NativeValue* value = nullptr;
    sptr<IRemoteObject> token = nullptr;
    token = NAPI_ohos_rpc_getNativeRemoteObject(
        reinterpret_cast<napi_env>(&engine), reinterpret_cast<napi_value>(info.argv[0]));
    if (token == nullptr) {
        std::shared_ptr<AbilityRuntime::RequestInfo> requestInfo = AbilityRuntime::RequestInfo::UnwrapRequestInfo(engine, info.argv[0]);
        if (requestInfo != nullptr) {
            token = requestInfo->GetToken();
        }
    }
    if (token == nullptr) {
        errCode = WmErrorCode::WM_ERROR_INVALID_PARAM;
    }

    if (errCode == WmErrorCode::WM_ERROR_INVALID_PARAM) {
        engine.Throw(CreateJsError(engine, static_cast<int32_t>(WmErrorCode::WM_ERROR_INVALID_PARAM)));
        return engine.CreateUndefined();
    }

    value = info.argv[1];
    if (value == nullptr || !value->IsCallable()) {
        registerManager_->RegisterListener(windowToken_,
            "dialogDeathRecipient", CaseType::CASE_WINDOW, engine, nullptr);
    } else {
        registerManager_->RegisterListener(windowToken_,
            "dialogDeathRecipient", CaseType::CASE_WINDOW, engine, value);
    }

    wptr<Window> weakToken(windowToken_);
    AsyncTask::CompleteCallback complete =
        [weakToken, token](NativeEngine& engine, AsyncTask& task, int32_t status) {
            auto weakWindow = weakToken.promote();
            if (weakWindow == nullptr) {
                task.Reject(engine, CreateJsError(engine,
                    static_cast<int32_t>(WmErrorCode::WM_ERROR_STATE_ABNORMALLY)));
                return;
            }

            WmErrorCode ret = WM_JS_TO_ERROR_CODE_MAP.at(weakWindow->BindDialogTarget(token));
            if (ret == WmErrorCode::WM_OK) {
                task.Resolve(engine, engine.CreateUndefined());
            } else {
                task.Reject(engine, CreateJsError(engine, static_cast<int32_t>(ret), "Bind Dialog Target failed"));
            }

            WLOGFD("[NAPI]BindDialogTarget end, window [%{public}u, %{public}s]",
                weakToken->GetWindowId(), weakToken->GetWindowName().c_str());
        };

    NativeValue* result = nullptr;
    NativeValue* lastParam = (info.argc == 2) ? nullptr :
        (info.argv[2]->TypeOf() == NATIVE_FUNCTION ? info.argv[2] : nullptr);
    AsyncTask::Schedule("JsWindow::OnBindDialogTarget",
        engine, CreateAsyncTaskWithLastParam(engine, lastParam, nullptr, std::move(complete), &result));
    return result;
}

static void LoadContentTask(std::shared_ptr<NativeReference> contentStorage, std::string contextUrl,
    sptr<Window> weakWindow, NativeEngine& engine, AsyncTask& task)
{
    NativeValue* nativeStorage =  (contentStorage == nullptr) ? nullptr : contentStorage->Get();
    AppExecFwk::Ability* ability = nullptr;
    GetAPI7Ability(engine, ability);
    WmErrorCode ret = WM_JS_TO_ERROR_CODE_MAP.at(
        weakWindow->SetUIContent(contextUrl, &engine, nativeStorage, false, ability));
    if (ret == WmErrorCode::WM_OK) {
        task.Resolve(engine, engine.CreateUndefined());
    } else {
        task.Reject(engine, CreateJsError(engine, static_cast<int32_t>(ret), "Window load content failed"));
    }
    WLOGFD("[NAPI]Window [%{public}u, %{public}s] load content end, ret = %{public}d",
        weakWindow->GetWindowId(), weakWindow->GetWindowName().c_str(), ret);
    return;
}

NativeValue* JsWindow::LoadContentScheduleOld(NativeEngine& engine, NativeCallbackInfo& info)
{
    WMError errCode = WMError::WM_OK;
    if (info.argc < 1 || info.argc > 2) { // 2 maximum param num
        WLOGFE("[NAPI]Argc is invalid: %{public}zu", info.argc);
        errCode = WMError::WM_ERROR_INVALID_PARAM;
    }
    std::string contextUrl;
    if (errCode == WMError::WM_OK && !ConvertFromJsValue(engine, info.argv[0], contextUrl)) {
        WLOGFE("[NAPI]Failed to convert parameter to context url");
        errCode = WMError::WM_ERROR_INVALID_PARAM;
    }
    NativeValue* storage = nullptr;
    NativeValue* callBack = nullptr;
    if (info.argc == 2) { // 2 param num
        callBack = info.argv[1];
    }
    std::shared_ptr<NativeReference> contentStorage = (storage == nullptr) ? nullptr :
        std::shared_ptr<NativeReference>(engine.CreateReference(storage, 1));
    wptr<Window> weakToken(windowToken_);
    AsyncTask::CompleteCallback complete =
        [weakToken, contentStorage, contextUrl, errCode](NativeEngine& engine, AsyncTask& task, int32_t status) {
            auto weakWindow = weakToken.promote();
            if (weakWindow == nullptr || errCode != WMError::WM_OK) {
                task.Reject(engine, CreateJsError(engine, static_cast<int32_t>(errCode)));
                WLOGFE("[NAPI]Window is nullptr or get invalid param");
                return;
            }
            LoadContentTask(contentStorage, contextUrl, weakWindow, engine, task);
        };
    NativeValue* result = nullptr;
    AsyncTask::Schedule("JsWindow::OnLoadContent",
        engine, CreateAsyncTaskWithLastParam(engine, callBack, nullptr, std::move(complete), &result));
    return result;
}

NativeValue* JsWindow::LoadContentScheduleNew(NativeEngine& engine, NativeCallbackInfo& info)
{
    WmErrorCode errCode = WmErrorCode::WM_OK;
    if (info.argc < 2) { // 2 param num
        WLOGFE("[NAPI]Argc is invalid: %{public}zu", info.argc);
        errCode = WmErrorCode::WM_ERROR_INVALID_PARAM;
    }
    std::string contextUrl;
    if (errCode == WmErrorCode::WM_OK && !ConvertFromJsValue(engine, info.argv[0], contextUrl)) {
        WLOGFE("[NAPI]Failed to convert parameter to context url");
        errCode = WmErrorCode::WM_ERROR_INVALID_PARAM;
    }
    NativeValue* storage = nullptr;
    NativeValue* callBack = nullptr;
    if (info.argc == 2) { // 2: num of params
        storage = info.argv[1];
    } else if (info.argc >= 3) { // 3: num of params
        storage = info.argv[1];
        callBack = ((info.argv[2] != nullptr && info.argv[2]->TypeOf() == NATIVE_FUNCTION) ? // 2 param num
            info.argv[2] : nullptr); // 2 param num
    }
    if (errCode == WmErrorCode::WM_ERROR_INVALID_PARAM) {
        WLOGFE("[NAPI]Window is nullptr or get invalid param");
        engine.Throw(CreateJsError(engine, static_cast<int32_t>(WmErrorCode::WM_ERROR_INVALID_PARAM)));
        return engine.CreateUndefined();
    }
    std::shared_ptr<NativeReference> contentStorage = (storage == nullptr) ? nullptr :
        std::shared_ptr<NativeReference>(engine.CreateReference(storage, 1));
    wptr<Window> weakToken(windowToken_);
    AsyncTask::CompleteCallback complete =
        [weakToken, contentStorage, contextUrl](NativeEngine& engine, AsyncTask& task, int32_t status) {
            auto weakWindow = weakToken.promote();
            if (weakWindow == nullptr) {
                WLOGFE("[NAPI]Window is nullptr or get invalid param");
                task.Reject(engine, CreateJsError(engine,
                    static_cast<int32_t>(WmErrorCode::WM_ERROR_STATE_ABNORMALLY)));
                return;
            }
            LoadContentTask(contentStorage, contextUrl, weakWindow, engine, task);
        };
    NativeValue* result = nullptr;
    AsyncTask::Schedule("JsWindow::OnLoadContent",
        engine, CreateAsyncTaskWithLastParam(engine, callBack, nullptr, std::move(complete), &result));
    return result;
}

NativeValue* JsWindow::OnLoadContent(NativeEngine& engine, NativeCallbackInfo& info)
{
    bool oldApi = false;
    if (info.argc == 1) {
        oldApi = true;
    } else if (info.argc == 2) { // 2 param num
        NativeValue* value = info.argv[1];
        if (value== nullptr || value->TypeOf() != NATIVE_FUNCTION) {
            oldApi = false;
        } else {
            oldApi = true;
        }
    }
    if (oldApi) {
        return LoadContentScheduleOld(engine, info);
    } else {
        return LoadContentScheduleNew(engine, info);
    }
}

NativeValue* JsWindow::OnSetUIContent(NativeEngine& engine, NativeCallbackInfo& info)
{
    WmErrorCode errCode = WmErrorCode::WM_OK;
    if (info.argc < 1) { // 2 maximum param num
        WLOGFE("[NAPI]Argc is invalid: %{public}zu", info.argc);
        errCode = WmErrorCode::WM_ERROR_INVALID_PARAM;
    }
    std::string contextUrl;
    if (errCode == WmErrorCode::WM_OK && !ConvertFromJsValue(engine, info.argv[0], contextUrl)) {
        WLOGFE("[NAPI]Failed to convert parameter to context url");
        errCode = WmErrorCode::WM_ERROR_INVALID_PARAM;
    }
    NativeValue* storage = nullptr;
    NativeValue* callBack = nullptr;
    if (info.argc >= 2) { // 2 param num
        callBack = info.argv[1];
    }
    std::shared_ptr<NativeReference> contentStorage = (storage == nullptr) ? nullptr :
        std::shared_ptr<NativeReference>(engine.CreateReference(storage, 1));
    if (errCode == WmErrorCode::WM_ERROR_INVALID_PARAM) {
        engine.Throw(CreateJsError(engine, static_cast<int32_t>(WmErrorCode::WM_ERROR_INVALID_PARAM)));
        return engine.CreateUndefined();
    }

    wptr<Window> weakToken(windowToken_);
    AsyncTask::CompleteCallback complete =
        [weakToken, contentStorage, contextUrl](NativeEngine& engine, AsyncTask& task, int32_t status) {
            auto weakWindow = weakToken.promote();
            if (weakWindow == nullptr) {
                WLOGFE("[NAPI]Window is nullptr");
                task.Reject(engine,
                    CreateJsError(engine, static_cast<int32_t>(WmErrorCode::WM_ERROR_STATE_ABNORMALLY)));
                return;
            }
            LoadContentTask(contentStorage, contextUrl, weakWindow, engine, task);
        };
    NativeValue* result = nullptr;
    AsyncTask::Schedule("JsWindow::OnSetUIContent",
        engine, CreateAsyncTaskWithLastParam(engine, callBack, nullptr, std::move(complete), &result));
    return result;
}

NativeValue* JsWindow::OnSetFullScreen(NativeEngine& engine, NativeCallbackInfo& info)
{
    WMError errCode = WMError::WM_OK;
    if (info.argc < 1 || info.argc > 2) { // 2: maximum params num
        WLOGFE("[NAPI]Argc is invalid: %{public}zu", info.argc);
        errCode = WMError::WM_ERROR_INVALID_PARAM;
    }
    bool isFullScreen = false;
    if (errCode == WMError::WM_OK) {
        NativeBoolean* nativeVal = ConvertNativeValueTo<NativeBoolean>(info.argv[0]);
        if (nativeVal == nullptr) {
            WLOGFE("[NAPI]Failed to convert parameter to isFullScreen");
            errCode = WMError::WM_ERROR_INVALID_PARAM;
        } else {
            isFullScreen = static_cast<bool>(*nativeVal);
        }
    }

    wptr<Window> weakToken(windowToken_);
    AsyncTask::CompleteCallback complete =
        [weakToken, isFullScreen, errCode](NativeEngine& engine, AsyncTask& task, int32_t status) {
            auto weakWindow = weakToken.promote();
            if (weakWindow == nullptr || errCode != WMError::WM_OK) {
                task.Reject(engine, CreateJsError(engine, static_cast<int32_t>(errCode), "Invalidate params."));
                return;
            }
            WMError ret = weakWindow->SetFullScreen(isFullScreen);
            if (ret == WMError::WM_OK) {
                task.Resolve(engine, engine.CreateUndefined());
            } else {
                task.Reject(engine, CreateJsError(engine, static_cast<int32_t>(ret), "Window SetFullScreen failed."));
            }
            WLOGFD("[NAPI]Window [%{public}u, %{public}s] set full screen end, ret = %{public}d",
                weakWindow->GetWindowId(), weakWindow->GetWindowName().c_str(), ret);
        };

    NativeValue* lastParam = (info.argc <= 1) ? nullptr :
        (info.argv[1]->TypeOf() == NATIVE_FUNCTION ? info.argv[1] : nullptr);
    NativeValue* result = nullptr;
    AsyncTask::Schedule("JsWindow::OnSetFullScreen",
        engine, CreateAsyncTaskWithLastParam(engine, lastParam, nullptr, std::move(complete), &result));
    return result;
}

NativeValue* JsWindow::OnSetLayoutFullScreen(NativeEngine& engine, NativeCallbackInfo& info)
{
    WMError errCode = WMError::WM_OK;
    if (info.argc < 1 || info.argc > 2) { // 2: maximum params num
        WLOGFE("[NAPI]Argc is invalid: %{public}zu", info.argc);
        errCode = WMError::WM_ERROR_INVALID_PARAM;
    }
    bool isLayoutFullScreen = false;
    if (errCode == WMError::WM_OK) {
        NativeBoolean* nativeVal = ConvertNativeValueTo<NativeBoolean>(info.argv[0]);
        if (nativeVal == nullptr) {
            WLOGFE("[NAPI]Failed to convert parameter to isLayoutFullScreen");
            errCode = WMError::WM_ERROR_INVALID_PARAM;
        } else {
            isLayoutFullScreen = static_cast<bool>(*nativeVal);
        }
    }
    wptr<Window> weakToken(windowToken_);
    AsyncTask::CompleteCallback complete =
        [weakToken, isLayoutFullScreen, errCode](NativeEngine& engine, AsyncTask& task, int32_t status) {
            auto weakWindow = weakToken.promote();
            if (weakWindow == nullptr || errCode != WMError::WM_OK) {
                task.Reject(engine, CreateJsError(engine, static_cast<int32_t>(errCode), "Invalidate params."));
                return;
            }
            WMError ret = weakWindow->SetLayoutFullScreen(isLayoutFullScreen);
            if (ret == WMError::WM_OK) {
                task.Resolve(engine, engine.CreateUndefined());
            } else {
                task.Reject(engine, CreateJsError(engine,
                    static_cast<int32_t>(ret), "Window OnSetLayoutFullScreen failed."));
            }
            WLOGFD("[NAPI]Window [%{public}u, %{public}s] set layout full screen end, ret = %{public}d",
                weakWindow->GetWindowId(), weakWindow->GetWindowName().c_str(), ret);
        };
    NativeValue* lastParam = (info.argc <= 1) ? nullptr :
        (info.argv[1]->TypeOf() == NATIVE_FUNCTION ? info.argv[1] : nullptr);
    NativeValue* result = nullptr;
    AsyncTask::Schedule("JsWindow::OnSetLayoutFullScreen",
        engine, CreateAsyncTaskWithLastParam(engine, lastParam, nullptr, std::move(complete), &result));
    return result;
}

NativeValue* JsWindow::OnSetWindowLayoutFullScreen(NativeEngine& engine, NativeCallbackInfo& info)
{
    WmErrorCode errCode = WmErrorCode::WM_OK;
    if (info.argc < 1) { // 1: params num
        WLOGFE("[NAPI]Argc is invalid: %{public}zu", info.argc);
        errCode = WmErrorCode::WM_ERROR_INVALID_PARAM;
    }
    bool isLayoutFullScreen = false;
    if (errCode == WmErrorCode::WM_OK) {
        NativeBoolean* nativeVal = ConvertNativeValueTo<NativeBoolean>(info.argv[0]);
        if (nativeVal == nullptr) {
            WLOGFE("[NAPI]Failed to convert parameter to isLayoutFullScreen");
            errCode = WmErrorCode::WM_ERROR_INVALID_PARAM;
        } else {
            isLayoutFullScreen = static_cast<bool>(*nativeVal);
        }
    }
    if (errCode != WmErrorCode::WM_OK) {
        engine.Throw(CreateJsError(engine, static_cast<int32_t>(WmErrorCode::WM_ERROR_INVALID_PARAM)));
        return engine.CreateUndefined();
    }

    wptr<Window> weakToken(windowToken_);
    AsyncTask::CompleteCallback complete =
        [weakToken, isLayoutFullScreen](NativeEngine& engine, AsyncTask& task, int32_t status) {
            auto weakWindow = weakToken.promote();
            if (weakWindow == nullptr) {
                task.Reject(engine,
                    CreateJsError(engine, static_cast<int32_t>(WmErrorCode::WM_ERROR_STATE_ABNORMALLY),
                    "Invalidate params."));
                return;
            }
            WmErrorCode ret = WM_JS_TO_ERROR_CODE_MAP.at(weakWindow->SetLayoutFullScreen(isLayoutFullScreen));
            if (ret == WmErrorCode::WM_OK) {
                task.Resolve(engine, engine.CreateUndefined());
            } else {
                task.Reject(engine, CreateJsError(engine,
                    static_cast<int32_t>(ret), "Window OnSetLayoutFullScreen failed."));
            }
            WLOGFD("[NAPI]Window [%{public}u, %{public}s] set layout full screen end, ret = %{public}d",
                weakWindow->GetWindowId(), weakWindow->GetWindowName().c_str(), ret);
        };

    NativeValue* lastParam = (info.argc <= 1) ? nullptr :
        ((info.argv[1] != nullptr && info.argv[1]->TypeOf() == NATIVE_FUNCTION) ?
        info.argv[1] : nullptr);
    NativeValue* result = nullptr;
    AsyncTask::Schedule("JsWindow::OnSetWindowLayoutFullScreen",
        engine, CreateAsyncTaskWithLastParam(engine, lastParam, nullptr, std::move(complete), &result));
    return result;
}

NativeValue* JsWindow::OnSetSystemBarEnable(NativeEngine& engine, NativeCallbackInfo& info)
{
    WMError errCode = WMError::WM_OK;
    if (info.argc > 2 || windowToken_ == nullptr) { // 2: maximum params num
        WLOGFE("[NAPI]Window is nullptr or argc is invalid: %{public}zu", info.argc);
        errCode = WMError::WM_ERROR_INVALID_PARAM;
    }
    std::map<WindowType, SystemBarProperty> systemBarProperties;
    if (errCode == WMError::WM_OK && !GetSystemBarStatus(systemBarProperties, engine, info, windowToken_)) {
        errCode = WMError::WM_ERROR_INVALID_PARAM;
    }
    wptr<Window> weakToken(windowToken_);
    AsyncTask::CompleteCallback complete =
        [weakToken, systemBarProperties, errCode](NativeEngine& engine, AsyncTask& task, int32_t status) {
            auto weakWindow = weakToken.promote();
            if (weakWindow == nullptr || errCode != WMError::WM_OK) {
                task.Reject(engine, CreateJsError(engine, static_cast<int32_t>(errCode)));
                WLOGFE("[NAPI]window is nullptr or get invalid param");
                return;
            }
            WMError ret = weakWindow->SetSystemBarProperty(WindowType::WINDOW_TYPE_STATUS_BAR,
                systemBarProperties.at(WindowType::WINDOW_TYPE_STATUS_BAR));
            ret = weakWindow->SetSystemBarProperty(WindowType::WINDOW_TYPE_NAVIGATION_BAR,
                systemBarProperties.at(WindowType::WINDOW_TYPE_NAVIGATION_BAR));
            if (ret == WMError::WM_OK) {
                task.Resolve(engine, engine.CreateUndefined());
            } else {
                task.Reject(engine, CreateJsError(engine,
                    static_cast<int32_t>(ret), "JsWindow::OnSetSystemBarEnable failed"));
            }
            WLOGFD("[NAPI]Window [%{public}u, %{public}s] set system bar enable end, ret = %{public}d",
                weakWindow->GetWindowId(), weakWindow->GetWindowName().c_str(), ret);
        };
    NativeValue* lastParam = nullptr;
    if (info.argc > 0 && info.argv[0]->TypeOf() == NATIVE_FUNCTION) {
        lastParam = info.argv[0];
    } else if (info.argc > 1 && info.argv[1]->TypeOf() == NATIVE_FUNCTION) {
        lastParam = info.argv[1];
    }
    NativeValue* result = nullptr;
    AsyncTask::Schedule("JsWindow::OnSetSystemBarEnable",
        engine, CreateAsyncTaskWithLastParam(engine, lastParam, nullptr, std::move(complete), &result));
    return result;
}

NativeValue* JsWindow::OnSetWindowSystemBarEnable(NativeEngine& engine, NativeCallbackInfo& info)
{
    WmErrorCode errCode = WmErrorCode::WM_OK;
    std::map<WindowType, SystemBarProperty> systemBarProperties;
    if (info.argc < 1 || windowToken_ == nullptr || // 1: params num
        !GetSystemBarStatus(systemBarProperties, engine, info, windowToken_)) {
        errCode = WmErrorCode::WM_ERROR_INVALID_PARAM;
    }
    if (errCode == WmErrorCode::WM_ERROR_INVALID_PARAM) {
        engine.Throw(CreateJsError(engine, static_cast<int32_t>(WmErrorCode::WM_ERROR_INVALID_PARAM)));
        return engine.CreateUndefined();
    }
    wptr<Window> weakToken(windowToken_);
    AsyncTask::CompleteCallback complete =
        [weakToken, systemBarProperties](NativeEngine& engine, AsyncTask& task, int32_t status) {
            auto weakWindow = weakToken.promote();
            if (weakWindow == nullptr) {
                task.Reject(engine,
                    CreateJsError(engine, static_cast<int32_t>(WmErrorCode::WM_ERROR_STATE_ABNORMALLY)));
                return;
            }
            WmErrorCode ret = WM_JS_TO_ERROR_CODE_MAP.at(
                weakWindow->SetSystemBarProperty(WindowType::WINDOW_TYPE_STATUS_BAR,
                systemBarProperties.at(WindowType::WINDOW_TYPE_STATUS_BAR)));
            if (ret != WmErrorCode::WM_OK) {
                task.Reject(engine, CreateJsError(engine,
                    static_cast<int32_t>(ret), "JsWindow::OnSetWindowSystemBarEnable failed"));
            }
            ret = WM_JS_TO_ERROR_CODE_MAP.at(
                weakWindow->SetSystemBarProperty(WindowType::WINDOW_TYPE_NAVIGATION_BAR,
                systemBarProperties.at(WindowType::WINDOW_TYPE_NAVIGATION_BAR)));
            if (ret == WmErrorCode::WM_OK) {
                task.Resolve(engine, engine.CreateUndefined());
            } else {
                task.Reject(engine, CreateJsError(engine,
                    static_cast<int32_t>(ret), "JsWindow::OnSetWindowSystemBarEnable failed"));
            }
        };
    NativeValue* lastParam = nullptr;
    if (info.argc >= 1 && info.argv[0] != nullptr && info.argv[0]->TypeOf() == NATIVE_FUNCTION) {
        lastParam = info.argv[0];
    } else if (info.argc >= 2 && // 2 arg count
        info.argv[1] != nullptr && info.argv[1]->TypeOf() == NATIVE_FUNCTION) {
        lastParam = info.argv[1];
    }
    NativeValue* result = nullptr;
    AsyncTask::Schedule("JsWindow::OnSetWindowSystemBarEnable",
        engine, CreateAsyncTaskWithLastParam(engine, lastParam, nullptr, std::move(complete), &result));
    return result;
}

NativeValue* JsWindow::OnSetSystemBarProperties(NativeEngine& engine, NativeCallbackInfo& info)
{
    WMError errCode = WMError::WM_OK;
    if (info.argc < 1 || info.argc > 2) { // 2: maximum params num
        WLOGFE("[NAPI]Argc is invalid: %{public}zu", info.argc);
        errCode = WMError::WM_ERROR_INVALID_PARAM;
    }
    std::map<WindowType, SystemBarProperty> systemBarProperties;
    if (errCode == WMError::WM_OK) {
        NativeObject* nativeObj = ConvertNativeValueTo<NativeObject>(info.argv[0]);
        if (nativeObj == nullptr) {
            WLOGFE("[NAPI]Failed to convert object to SystemBarProperties");
            errCode = WMError::WM_ERROR_INVALID_PARAM;
        } else {
            if (!SetSystemBarPropertiesFromJs(engine, nativeObj, systemBarProperties, windowToken_)) {
                WLOGFE("[NAPI]Failed to GetSystemBarProperties From Js Object");
                errCode = WMError::WM_ERROR_INVALID_PARAM;
            }
        }
    }
    wptr<Window> weakToken(windowToken_);
    AsyncTask::CompleteCallback complete =
        [weakToken, systemBarProperties, errCode](NativeEngine& engine, AsyncTask& task, int32_t status) {
            auto weakWindow = weakToken.promote();
            if (weakWindow == nullptr || errCode != WMError::WM_OK) {
                task.Reject(engine, CreateJsError(engine, static_cast<int32_t>(errCode)));
                WLOGFE("[NAPI]window is nullptr or get invalid param");
                return;
            }
            WMError ret = weakWindow->SetSystemBarProperty(WindowType::WINDOW_TYPE_STATUS_BAR,
                systemBarProperties.at(WindowType::WINDOW_TYPE_STATUS_BAR));
            ret = weakWindow->SetSystemBarProperty(WindowType::WINDOW_TYPE_NAVIGATION_BAR,
                systemBarProperties.at(WindowType::WINDOW_TYPE_NAVIGATION_BAR));
            if (ret == WMError::WM_OK) {
                task.Resolve(engine, engine.CreateUndefined());
            } else {
                task.Reject(engine, CreateJsError(engine,
                    static_cast<int32_t>(WMError::WM_ERROR_NULLPTR), "JsWindow::OnSetSystemBarProperties failed"));
            }
            WLOGFD("[NAPI]Window [%{public}u, %{public}s] set system bar properties end, ret = %{public}d",
                weakWindow->GetWindowId(), weakWindow->GetWindowName().c_str(), ret);
        };

    NativeValue* lastParam = (info.argc <= 1) ? nullptr :
        (info.argv[1]->TypeOf() == NATIVE_FUNCTION ? info.argv[1] : nullptr);
    NativeValue* result = nullptr;
    AsyncTask::Schedule("JsWindow::OnSetSystemBarProperties",
        engine, CreateAsyncTaskWithLastParam(engine, lastParam, nullptr, std::move(complete), &result));
    return result;
}

NativeValue* JsWindow::OnSetWindowSystemBarProperties(NativeEngine& engine, NativeCallbackInfo& info)
{
    WmErrorCode errCode = WmErrorCode::WM_OK;
    if (info.argc < 1) { // 2: maximum params num
        errCode = WmErrorCode::WM_ERROR_INVALID_PARAM;
    }
    std::map<WindowType, SystemBarProperty> systemBarProperties;
    if (errCode == WmErrorCode::WM_OK) {
        NativeObject* nativeObj = ConvertNativeValueTo<NativeObject>(info.argv[0]);
        if (nativeObj == nullptr) {
            errCode = WmErrorCode::WM_ERROR_INVALID_PARAM;
        } else {
            if (!SetSystemBarPropertiesFromJs(engine, nativeObj, systemBarProperties, windowToken_)) {
                errCode = WmErrorCode::WM_ERROR_INVALID_PARAM;
            }
        }
    }
    if (errCode == WmErrorCode::WM_ERROR_INVALID_PARAM) {
        engine.Throw(CreateJsError(engine, static_cast<int32_t>(WmErrorCode::WM_ERROR_INVALID_PARAM)));
        return engine.CreateUndefined();
    }

    wptr<Window> weakToken(windowToken_);
    AsyncTask::CompleteCallback complete =
        [weakToken, systemBarProperties](NativeEngine& engine, AsyncTask& task, int32_t status) {
            auto weakWindow = weakToken.promote();
            if (weakWindow == nullptr) {
                task.Reject(engine,
                    CreateJsError(engine, static_cast<int32_t>(WmErrorCode::WM_ERROR_STATE_ABNORMALLY)));
                return;
            }
            WmErrorCode ret = WM_JS_TO_ERROR_CODE_MAP.at(
                weakWindow->SetSystemBarProperty(WindowType::WINDOW_TYPE_STATUS_BAR,
                systemBarProperties.at(WindowType::WINDOW_TYPE_STATUS_BAR)));
            if (ret != WmErrorCode::WM_OK) {
                task.Reject(engine, CreateJsError(engine, static_cast<int32_t>(ret)));
            }
            ret = WM_JS_TO_ERROR_CODE_MAP.at(
                weakWindow->SetSystemBarProperty(WindowType::WINDOW_TYPE_NAVIGATION_BAR,
                systemBarProperties.at(WindowType::WINDOW_TYPE_NAVIGATION_BAR)));
            if (ret != WmErrorCode::WM_OK) {
                task.Reject(engine, CreateJsError(engine, static_cast<int32_t>(ret)));
            }
            task.Resolve(engine, engine.CreateUndefined());
        };

    NativeValue* lastParam = (info.argc <= 1) ? nullptr :
        ((info.argv[1] != nullptr && info.argv[1]->TypeOf() == NATIVE_FUNCTION) ?
        info.argv[1] : nullptr);
    NativeValue* result = nullptr;
    AsyncTask::Schedule("JsWindow::OnSetWindowSystemBarProperties",
        engine, CreateAsyncTaskWithLastParam(engine, lastParam, nullptr, std::move(complete), &result));
    return result;
}

NativeValue* JsWindow::OnGetAvoidArea(NativeEngine& engine, NativeCallbackInfo& info)
{
    WMError errCode = WMError::WM_OK;
    if (info.argc < 1 || info.argc > 2) { // 2: maximum params num
        WLOGFE("[NAPI]Argc is invalid: %{public}zu", info.argc);
        errCode = WMError::WM_ERROR_INVALID_PARAM;
    }
    AvoidAreaType avoidAreaType = AvoidAreaType::TYPE_SYSTEM;
    if (errCode == WMError::WM_OK) {
        NativeNumber* nativeMode = ConvertNativeValueTo<NativeNumber>(info.argv[0]);
        if (nativeMode == nullptr) {
            WLOGFE("[NAPI]Failed to convert parameter to AvoidAreaType");
            errCode = WMError::WM_ERROR_INVALID_PARAM;
        } else {
            avoidAreaType = static_cast<AvoidAreaType>(static_cast<uint32_t>(*nativeMode));
            errCode = ((avoidAreaType > AvoidAreaType::TYPE_KEYBOARD) ||
                (avoidAreaType < AvoidAreaType::TYPE_SYSTEM)) ? WMError::WM_ERROR_INVALID_PARAM : WMError::WM_OK;
        }
    }
    wptr<Window> weakToken(windowToken_);
    AsyncTask::CompleteCallback complete =
        [weakToken, errCode, avoidAreaType](NativeEngine& engine, AsyncTask& task, int32_t status) {
            auto weakWindow = weakToken.promote();
            if (weakWindow == nullptr || errCode != WMError::WM_OK) {
                task.Reject(engine, CreateJsError(engine, static_cast<int32_t>(errCode)));
                WLOGFE("[NAPI]window is nullptr or get invalid param");
                return;
            }
            // getAvoidRect by avoidAreaType
            AvoidArea avoidArea;
            WMError ret = weakWindow->GetAvoidAreaByType(avoidAreaType, avoidArea);
            if (ret != WMError::WM_OK) {
                avoidArea.topRect_ = g_emptyRect;
                avoidArea.leftRect_ = g_emptyRect;
                avoidArea.rightRect_ = g_emptyRect;
                avoidArea.bottomRect_ = g_emptyRect;
            }
            NativeValue* avoidAreaObj = ConvertAvoidAreaToJsValue(engine, avoidArea, avoidAreaType);
            if (avoidAreaObj != nullptr) {
                task.Resolve(engine, avoidAreaObj);
            } else {
                task.Reject(engine, CreateJsError(engine,
                    static_cast<int32_t>(WMError::WM_ERROR_NULLPTR), "JsWindow::OnGetAvoidArea failed"));
            }
            WLOGFD("[NAPI]Window [%{public}u, %{public}s] get avoid area end, ret = %{public}d",
                weakWindow->GetWindowId(), weakWindow->GetWindowName().c_str(), ret);
        };
    NativeValue* lastParam = (info.argc <= 1) ? nullptr :
        (info.argv[1]->TypeOf() == NATIVE_FUNCTION ? info.argv[1] : nullptr);
    NativeValue* result = nullptr;
    AsyncTask::Schedule("JsWindow::OnGetAvoidArea",
        engine, CreateAsyncTaskWithLastParam(engine, lastParam, nullptr, std::move(complete), &result));
    return result;
}

NativeValue* JsWindow::OnGetWindowAvoidAreaSync(NativeEngine& engine, NativeCallbackInfo& info)
{
    WmErrorCode errCode = WmErrorCode::WM_OK;
    if (info.argc < 1) { // 1: params num
        engine.Throw(CreateJsError(engine, static_cast<int32_t>(WmErrorCode::WM_ERROR_INVALID_PARAM)));
        return engine.CreateUndefined();
    }
    AvoidAreaType avoidAreaType = AvoidAreaType::TYPE_SYSTEM;
    NativeNumber* nativeMode = ConvertNativeValueTo<NativeNumber>(info.argv[0]);
    if (nativeMode == nullptr) {
        errCode = WmErrorCode::WM_ERROR_INVALID_PARAM;
    } else {
        avoidAreaType = static_cast<AvoidAreaType>(static_cast<uint32_t>(*nativeMode));
        errCode = ((avoidAreaType > AvoidAreaType::TYPE_KEYBOARD) || (avoidAreaType < AvoidAreaType::TYPE_SYSTEM)) ?
            WmErrorCode::WM_ERROR_INVALID_PARAM : WmErrorCode::WM_OK;
    }
    if (errCode == WmErrorCode::WM_ERROR_INVALID_PARAM) {
        engine.Throw(CreateJsError(engine, static_cast<int32_t>(WmErrorCode::WM_ERROR_INVALID_PARAM)));
        return engine.CreateUndefined();
    }

    wptr<Window> weakToken(windowToken_);
    auto window = weakToken.promote();
    if (window == nullptr) {
        engine.Throw(CreateJsError(engine, static_cast<int32_t>(WmErrorCode::WM_ERROR_STATE_ABNORMALLY)));
        return engine.CreateUndefined();
    }
    // getAvoidRect by avoidAreaType
    AvoidArea avoidArea;
    WmErrorCode ret = WM_JS_TO_ERROR_CODE_MAP.at(window->GetAvoidAreaByType(avoidAreaType, avoidArea));
    if (ret != WmErrorCode::WM_OK) {
        avoidArea.topRect_ = g_emptyRect;
        avoidArea.leftRect_ = g_emptyRect;
        avoidArea.rightRect_ = g_emptyRect;
        avoidArea.bottomRect_ = g_emptyRect;
    }
    NativeValue* avoidAreaObj = ConvertAvoidAreaToJsValue(engine, avoidArea, avoidAreaType);
    if (avoidAreaObj != nullptr) {
        return avoidAreaObj;
    } else {
        engine.Throw(CreateJsError(engine, static_cast<int32_t>(WmErrorCode::WM_ERROR_STATE_ABNORMALLY)));
        return engine.CreateUndefined();
    }
}

NativeValue* JsWindow::OnIsShowing(NativeEngine& engine, NativeCallbackInfo& info)
{
    WMError errCode = WMError::WM_OK;
    if (info.argc > 1) {
        WLOGFE("[NAPI]Argc is invalid: %{public}zu", info.argc);
        errCode = WMError::WM_ERROR_INVALID_PARAM;
    }
    wptr<Window> weakToken(windowToken_);
    AsyncTask::CompleteCallback complete =
        [weakToken, errCode](NativeEngine& engine, AsyncTask& task, int32_t status) {
            auto weakWindow = weakToken.promote();
            if (weakWindow == nullptr || errCode != WMError::WM_OK) {
                task.Reject(engine, CreateJsError(engine, static_cast<int32_t>(errCode)));
                WLOGFE("[NAPI]window is nullptr or get invalid param");
                return;
            }
            bool state = weakWindow->GetWindowState() == WindowState::STATE_SHOWN;
            task.Resolve(engine, CreateJsValue(engine, state));
            WLOGFD("[NAPI]Window [%{public}u, %{public}s] get show state end, state = %{public}u",
                weakWindow->GetWindowId(), weakWindow->GetWindowName().c_str(), state);
        };

    NativeValue* lastParam = (info.argc == 0) ? nullptr :
        (info.argv[0]->TypeOf() == NATIVE_FUNCTION ? info.argv[0] : nullptr);
    NativeValue* result = nullptr;
    AsyncTask::Schedule("JsWindow::OnIsShowing",
        engine, CreateAsyncTaskWithLastParam(engine, lastParam, nullptr, std::move(complete), &result));
    return result;
}

NativeValue* JsWindow::OnIsWindowShowingSync(NativeEngine& engine, NativeCallbackInfo& info)
{
    wptr<Window> weakToken(windowToken_);
    auto window = weakToken.promote();
    if (window == nullptr) {
        WLOGFE("[NAPI]window is nullptr");
        engine.Throw(CreateJsError(engine, static_cast<int32_t>(WmErrorCode::WM_ERROR_STATE_ABNORMALLY)));
        return engine.CreateUndefined();
    }
    bool state = (window->GetWindowState() == WindowState::STATE_SHOWN);
    WLOGFD("[NAPI]Window [%{public}u, %{public}s] get show state end, state = %{public}u",
        window->GetWindowId(), window->GetWindowName().c_str(), state);
    return CreateJsValue(engine, state);
}

NativeValue* JsWindow::OnSetPreferredOrientation(NativeEngine& engine, NativeCallbackInfo& info)
{
    WmErrorCode errCode = WmErrorCode::WM_OK;
    Orientation requestedOrientation = Orientation::UNSPECIFIED;
    if (info.argc < 1) { // 1: params num
        WLOGFE("[NAPI]Argc is invalid: %{public}zu", info.argc);
        errCode = WmErrorCode::WM_ERROR_INVALID_PARAM;
    } else {
        NativeNumber* nativeType = ConvertNativeValueTo<NativeNumber>(info.argv[0]);
        if (nativeType == nullptr) {
            errCode = WmErrorCode::WM_ERROR_INVALID_PARAM;
            WLOGFE("[NAPI]Failed to convert parameter to Orientation");
        } else {
            requestedOrientation = JS_TO_NATIVE_ORIENTATION_MAP.at(
                static_cast<ApiOrientation>(static_cast<uint32_t>(*nativeType)));
            if (requestedOrientation < Orientation::BEGIN || requestedOrientation > Orientation::END) {
                WLOGFE("[NAPI]Orientation %{public}u invalid!", static_cast<uint32_t>(requestedOrientation));
                errCode = WmErrorCode::WM_ERROR_INVALID_PARAM;
            }
        }
    }
    if (errCode == WmErrorCode::WM_ERROR_INVALID_PARAM) {
        WLOGFE("[NAPI]get invalid param");
        engine.Throw(CreateJsError(engine, static_cast<int32_t>(WmErrorCode::WM_ERROR_INVALID_PARAM)));
        return engine.CreateUndefined();
    }

    wptr<Window> weakToken(windowToken_);
    AsyncTask::CompleteCallback complete =
        [weakToken, requestedOrientation](NativeEngine& engine, AsyncTask& task, int32_t status) {
            auto weakWindow = weakToken.promote();
            if (weakWindow == nullptr) {
                WLOGFE("[NAPI]Window is nullptr");
                task.Reject(engine, CreateJsError(engine, static_cast<int32_t>(WmErrorCode::WM_ERROR_STATE_ABNORMALLY),
                    "OnSetPreferredOrientation failed"));
                return;
            }
            weakWindow->SetRequestedOrientation(requestedOrientation);
            task.Resolve(engine, engine.CreateUndefined());
            WLOGFD("[NAPI]Window [%{public}u, %{public}s] OnSetPreferredOrientation end, orientation = %{public}u",
                weakWindow->GetWindowId(),
                weakWindow->GetWindowName().c_str(),
                static_cast<uint32_t>(requestedOrientation));
        };

    NativeValue* lastParam = (info.argc <= 1) ? nullptr :
        ((info.argv[1] != nullptr && info.argv[1]->TypeOf() == NATIVE_FUNCTION) ?
        info.argv[1] : nullptr);
    NativeValue* result = nullptr;
    AsyncTask::Schedule("JsWindow::OnSetPreferredOrientation",
        engine, CreateAsyncTaskWithLastParam(engine, lastParam, nullptr, std::move(complete), &result));
    return result;
}

NativeValue* JsWindow::OnIsSupportWideGamut(NativeEngine& engine, NativeCallbackInfo& info)
{
    WMError errCode = WMError::WM_OK;
    if (info.argc > 1) {
        WLOGFE("[NAPI]Argc is invalid: %{public}zu", info.argc);
        errCode = WMError::WM_ERROR_INVALID_PARAM;
    }
    wptr<Window> weakToken(windowToken_);
    AsyncTask::CompleteCallback complete =
        [weakToken, errCode](NativeEngine& engine, AsyncTask& task, int32_t status) {
            auto weakWindow = weakToken.promote();
            if (weakWindow == nullptr || errCode != WMError::WM_OK) {
                task.Reject(engine, CreateJsError(engine, static_cast<int32_t>(errCode)));
                WLOGFE("[NAPI]window is nullptr or get invalid param");
                return;
            }
            bool flag = weakWindow->IsSupportWideGamut();
            task.Resolve(engine, CreateJsValue(engine, flag));
            WLOGFD("[NAPI]Window [%{public}u, %{public}s] OnIsSupportWideGamut end, ret = %{public}u",
                weakWindow->GetWindowId(), weakWindow->GetWindowName().c_str(), flag);
        };

    NativeValue* lastParam = (info.argc == 0) ? nullptr :
        (info.argv[0]->TypeOf() == NATIVE_FUNCTION ? info.argv[0] : nullptr);
    NativeValue* result = nullptr;
    AsyncTask::Schedule("JsWindow::OnIsSupportWideGamut",
        engine, CreateAsyncTaskWithLastParam(engine, lastParam, nullptr, std::move(complete), &result));
    return result;
}

NativeValue* JsWindow::OnIsWindowSupportWideGamut(NativeEngine& engine, NativeCallbackInfo& info)
{
    wptr<Window> weakToken(windowToken_);
    AsyncTask::CompleteCallback complete =
        [weakToken](NativeEngine& engine, AsyncTask& task, int32_t status) {
            auto weakWindow = weakToken.promote();
            if (weakWindow == nullptr) {
                WLOGFE("[NAPI]window is nullptr or get invalid param");
                task.Reject(engine,
                    CreateJsError(engine, static_cast<int32_t>(WmErrorCode::WM_ERROR_STATE_ABNORMALLY)));
                return;
            }
            bool flag = weakWindow->IsSupportWideGamut();
            task.Resolve(engine, CreateJsValue(engine, flag));
            WLOGFD("[NAPI]Window [%{public}u, %{public}s] OnIsWindowSupportWideGamut end, ret = %{public}u",
                weakWindow->GetWindowId(), weakWindow->GetWindowName().c_str(), flag);
        };

    NativeValue* lastParam = (info.argc == 0) ? nullptr :
        ((info.argv[0] != nullptr && info.argv[0]->TypeOf() == NATIVE_FUNCTION) ?
        info.argv[0] : nullptr);
    NativeValue* result = nullptr;
    AsyncTask::Schedule("JsWindow::OnIsWindowSupportWideGamut",
        engine, CreateAsyncTaskWithLastParam(engine, lastParam, nullptr, std::move(complete), &result));
    return result;
}

NativeValue* JsWindow::OnSetBackgroundColor(NativeEngine& engine, NativeCallbackInfo& info)
{
    WMError errCode = WMError::WM_OK;
    if (info.argc < 1 || info.argc > 2) { // 2: maximum params num
        WLOGFE("[NAPI]Argc is invalid: %{public}zu", info.argc);
        errCode = WMError::WM_ERROR_INVALID_PARAM;
    }
    std::string color;
    if (errCode == WMError::WM_OK && !ConvertFromJsValue(engine, info.argv[0], color)) {
        WLOGFE("[NAPI]Failed to convert parameter to background color");
        errCode = WMError::WM_ERROR_INVALID_PARAM;
    }

    wptr<Window> weakToken(windowToken_);
    AsyncTask::CompleteCallback complete =
        [weakToken, color, errCode](NativeEngine& engine, AsyncTask& task, int32_t status) {
            auto weakWindow = weakToken.promote();
            if (weakWindow == nullptr || errCode != WMError::WM_OK) {
                task.Reject(engine, CreateJsError(engine, static_cast<int32_t>(errCode), "Invalidate params."));
                return;
            }
            WMError ret = weakWindow->SetBackgroundColor(color);
            if (ret == WMError::WM_OK) {
                task.Resolve(engine, engine.CreateUndefined());
            } else {
                task.Reject(engine, CreateJsError(engine, static_cast<int32_t>(ret),
                    "Window set background color failed"));
            }
            WLOGFD("[NAPI]Window [%{public}u, %{public}s] set background color end",
                weakWindow->GetWindowId(), weakWindow->GetWindowName().c_str());
        };

    NativeValue* lastParam = (info.argc <= 1) ? nullptr :
        (info.argv[1]->TypeOf() == NATIVE_FUNCTION ? info.argv[1] : nullptr);
    NativeValue* result = nullptr;
    AsyncTask::Schedule("JsWindow::OnSetBackgroundColor",
        engine, CreateAsyncTaskWithLastParam(engine, lastParam, nullptr, std::move(complete), &result));
    return result;
}

NativeValue* JsWindow::OnSetWindowBackgroundColorSync(NativeEngine& engine, NativeCallbackInfo& info)
{
    WmErrorCode errCode = WmErrorCode::WM_OK;
    if (info.argc < 1) { // 1: params num
        WLOGFE("[NAPI]Argc is invalid: %{public}zu", info.argc);
        errCode = WmErrorCode::WM_ERROR_INVALID_PARAM;
    }
    std::string color;
    if (errCode == WmErrorCode::WM_OK && !ConvertFromJsValue(engine, info.argv[0], color)) {
        WLOGFE("[NAPI]Failed to convert parameter to background color");
        errCode = WmErrorCode::WM_ERROR_INVALID_PARAM;
    }
    if (errCode == WmErrorCode::WM_ERROR_INVALID_PARAM) {
        engine.Throw(CreateJsError(engine, static_cast<int32_t>(WmErrorCode::WM_ERROR_INVALID_PARAM)));
        return engine.CreateUndefined();
    }

    wptr<Window> weakToken(windowToken_);
    auto window = weakToken.promote();
    if (window == nullptr) {
        engine.Throw(CreateJsError(engine, static_cast<int32_t>(WmErrorCode::WM_ERROR_STATE_ABNORMALLY)));
        return engine.CreateUndefined();
    }
    WmErrorCode ret = WM_JS_TO_ERROR_CODE_MAP.at(window->SetBackgroundColor(color));
    if (ret == WmErrorCode::WM_OK) {
        WLOGFD("[NAPI]Window [%{public}u, %{public}s] set background color end",
            window->GetWindowId(), window->GetWindowName().c_str());
        return engine.CreateUndefined();
    } else {
        engine.Throw(CreateJsError(engine, static_cast<int32_t>(ret)));
        return engine.CreateUndefined();
    }
}

NativeValue* JsWindow::OnSetBrightness(NativeEngine& engine, NativeCallbackInfo& info)
{
    WMError errCode = WMError::WM_OK;
    if (info.argc < 1 || info.argc > 2) { // 2: maximum params num
        WLOGFE("[NAPI]Argc is invalid: %{public}zu", info.argc);
        errCode = WMError::WM_ERROR_INVALID_PARAM;
    }
    float brightness = UNDEFINED_BRIGHTNESS;
    if (errCode == WMError::WM_OK) {
        NativeNumber* nativeVal = ConvertNativeValueTo<NativeNumber>(info.argv[0]);
        if (nativeVal == nullptr) {
            WLOGFE("[NAPI]Failed to convert parameter to brightness");
            errCode = WMError::WM_ERROR_INVALID_PARAM;
        } else {
            brightness = static_cast<double>(*nativeVal);
        }
    }

    wptr<Window> weakToken(windowToken_);
    AsyncTask::CompleteCallback complete =
        [weakToken, brightness, errCode](NativeEngine& engine, AsyncTask& task, int32_t status) {
            auto weakWindow = weakToken.promote();
            if (weakWindow == nullptr || errCode != WMError::WM_OK) {
                task.Reject(engine, CreateJsError(engine, static_cast<int32_t>(errCode), "Invalidate params."));
                return;
            }
            WMError ret = weakWindow->SetBrightness(brightness);
            if (ret == WMError::WM_OK) {
                task.Resolve(engine, engine.CreateUndefined());
            } else {
                task.Reject(engine, CreateJsError(engine, static_cast<int32_t>(ret), "Window set brightness failed"));
            }
            WLOGFD("[NAPI]Window [%{public}u, %{public}s] set brightness end",
                weakWindow->GetWindowId(), weakWindow->GetWindowName().c_str());
        };

    NativeValue* lastParam = (info.argc <= 1) ? nullptr :
        (info.argv[1]->TypeOf() == NATIVE_FUNCTION ? info.argv[1] : nullptr);
    NativeValue* result = nullptr;
    AsyncTask::Schedule("JsWindow::OnSetBrightness",
        engine, CreateAsyncTaskWithLastParam(engine, lastParam, nullptr, std::move(complete), &result));
    return result;
}

NativeValue* JsWindow::OnSetWindowBrightness(NativeEngine& engine, NativeCallbackInfo& info)
{
    WmErrorCode errCode = WmErrorCode::WM_OK;
    if (info.argc < 1) { // 1: params num
        WLOGFE("[NAPI]Argc is invalid: %{public}zu", info.argc);
        errCode = WmErrorCode::WM_ERROR_INVALID_PARAM;
    }
    float brightness = UNDEFINED_BRIGHTNESS;
    if (errCode == WmErrorCode::WM_OK) {
        NativeNumber* nativeVal = ConvertNativeValueTo<NativeNumber>(info.argv[0]);
        if (nativeVal == nullptr) {
            WLOGFE("[NAPI]Failed to convert parameter to brightness");
            errCode = WmErrorCode::WM_ERROR_INVALID_PARAM;
        } else {
            brightness = static_cast<double>(*nativeVal);
        }
    }
    if (errCode == WmErrorCode::WM_ERROR_INVALID_PARAM) {
        engine.Throw(CreateJsError(engine, static_cast<int32_t>(WmErrorCode::WM_ERROR_INVALID_PARAM)));
        return engine.CreateUndefined();
    }

    wptr<Window> weakToken(windowToken_);
    AsyncTask::CompleteCallback complete =
        [weakToken, brightness](NativeEngine& engine, AsyncTask& task, int32_t status) {
            auto weakWindow = weakToken.promote();
            if (weakWindow == nullptr) {
                task.Reject(engine,
                    CreateJsError(engine, static_cast<int32_t>(WmErrorCode::WM_ERROR_STATE_ABNORMALLY),
                    "Invalidate params."));
                return;
            }
            WmErrorCode ret = WM_JS_TO_ERROR_CODE_MAP.at(weakWindow->SetBrightness(brightness));
            if (ret == WmErrorCode::WM_OK) {
                task.Resolve(engine, engine.CreateUndefined());
            } else {
                task.Reject(engine, CreateJsError(engine, static_cast<int32_t>(ret), "Window set brightness failed"));
            }
            WLOGFD("[NAPI]Window [%{public}u, %{public}s] set brightness end",
                weakWindow->GetWindowId(), weakWindow->GetWindowName().c_str());
        };

    NativeValue* lastParam = (info.argc <= 1) ? nullptr :
        ((info.argv[1] != nullptr && info.argv[1]->TypeOf() == NATIVE_FUNCTION) ?
        info.argv[1] : nullptr);
    NativeValue* result = nullptr;
    AsyncTask::Schedule("JsWindow::OnSetWindowBrightness",
        engine, CreateAsyncTaskWithLastParam(engine, lastParam, nullptr, std::move(complete), &result));
    return result;
}

NativeValue* JsWindow::OnSetDimBehind(NativeEngine& engine, NativeCallbackInfo& info)
{
    AsyncTask::CompleteCallback complete =
        [](NativeEngine& engine, AsyncTask& task, int32_t status) {
            task.Reject(engine, CreateJsError(engine, static_cast<int32_t>(WMError::WM_ERROR_DEVICE_NOT_SUPPORT)));
        };

    NativeValue* lastParam = (info.argc <= 1) ? nullptr :
        (info.argv[1]->TypeOf() == NATIVE_FUNCTION ? info.argv[1] : nullptr);
    NativeValue* result = nullptr;
    AsyncTask::Schedule("JsWindow::OnSetDimBehind",
        engine, CreateAsyncTaskWithLastParam(engine, lastParam, nullptr, std::move(complete), &result));
    return result;
}

NativeValue* JsWindow::OnSetFocusable(NativeEngine& engine, NativeCallbackInfo& info)
{
    WMError errCode = WMError::WM_OK;
    if (info.argc < 1 || info.argc > 2) { // 2: maximum params num
        WLOGFE("[NAPI]Argc is invalid: %{public}zu", info.argc);
        errCode = WMError::WM_ERROR_INVALID_PARAM;
    }
    bool focusable = true;
    if (errCode == WMError::WM_OK) {
        NativeBoolean* nativeVal = ConvertNativeValueTo<NativeBoolean>(info.argv[0]);
        if (nativeVal == nullptr) {
            WLOGFE("[NAPI]Failed to convert parameter to focusable");
            errCode = WMError::WM_ERROR_INVALID_PARAM;
        } else {
            focusable = static_cast<bool>(*nativeVal);
        }
    }

    wptr<Window> weakToken(windowToken_);
    AsyncTask::CompleteCallback complete =
        [weakToken, focusable, errCode](NativeEngine& engine, AsyncTask& task, int32_t status) {
            auto weakWindow = weakToken.promote();
            if (weakWindow == nullptr || errCode != WMError::WM_OK) {
                task.Reject(engine, CreateJsError(engine, static_cast<int32_t>(errCode), "Invalidate params."));
                return;
            }
            WMError ret = weakWindow->SetFocusable(focusable);
            if (ret == WMError::WM_OK) {
                task.Resolve(engine, engine.CreateUndefined());
            } else {
                task.Reject(engine, CreateJsError(engine, static_cast<int32_t>(ret), "Window set focusable failed"));
            }
            WLOGFD("[NAPI]Window [%{public}u, %{public}s] set focusable end",
                weakWindow->GetWindowId(), weakWindow->GetWindowName().c_str());
        };

    NativeValue* lastParam = (info.argc <= 1) ? nullptr :
        (info.argv[1]->TypeOf() == NATIVE_FUNCTION ? info.argv[1] : nullptr);
    NativeValue* result = nullptr;
    AsyncTask::Schedule("JsWindow::OnSetFocusable",
        engine, CreateAsyncTaskWithLastParam(engine, lastParam, nullptr, std::move(complete), &result));
    return result;
}

NativeValue* JsWindow::OnSetWindowFocusable(NativeEngine& engine, NativeCallbackInfo& info)
{
    WmErrorCode errCode = WmErrorCode::WM_OK;
    if (info.argc < 1) { // 1: maximum params num
        WLOGFE("[NAPI]Argc is invalid: %{public}zu", info.argc);
        errCode = WmErrorCode::WM_ERROR_INVALID_PARAM;
    }
    bool focusable = true;
    if (errCode == WmErrorCode::WM_OK) {
        NativeBoolean* nativeVal = ConvertNativeValueTo<NativeBoolean>(info.argv[0]);
        if (nativeVal == nullptr) {
            WLOGFE("[NAPI]Failed to convert parameter to focusable");
            errCode = WmErrorCode::WM_ERROR_INVALID_PARAM;
        } else {
            focusable = static_cast<bool>(*nativeVal);
        }
    }
    if (errCode == WmErrorCode::WM_ERROR_INVALID_PARAM) {
        engine.Throw(CreateJsError(engine, static_cast<int32_t>(WmErrorCode::WM_ERROR_INVALID_PARAM)));
        return engine.CreateUndefined();
    }

    wptr<Window> weakToken(windowToken_);
    AsyncTask::CompleteCallback complete =
        [weakToken, focusable](NativeEngine& engine, AsyncTask& task, int32_t status) {
            auto weakWindow = weakToken.promote();
            if (weakWindow == nullptr) {
                task.Reject(engine,
                    CreateJsError(engine, static_cast<int32_t>(WmErrorCode::WM_ERROR_STATE_ABNORMALLY),
                    "Invalidate params."));
                return;
            }
            WmErrorCode ret = WM_JS_TO_ERROR_CODE_MAP.at(weakWindow->SetFocusable(focusable));
            if (ret == WmErrorCode::WM_OK) {
                task.Resolve(engine, engine.CreateUndefined());
            } else {
                task.Reject(engine, CreateJsError(engine, static_cast<int32_t>(ret), "Window set focusable failed"));
            }
            WLOGFD("[NAPI]Window [%{public}u, %{public}s] set focusable end",
                weakWindow->GetWindowId(), weakWindow->GetWindowName().c_str());
        };

    NativeValue* lastParam = (info.argc <= 1) ? nullptr :
        ((info.argv[1] != nullptr && info.argv[1]->TypeOf() == NATIVE_FUNCTION) ?
        info.argv[1] : nullptr);
    NativeValue* result = nullptr;
    AsyncTask::Schedule("JsWindow::OnSetWindowFocusable",
        engine, CreateAsyncTaskWithLastParam(engine, lastParam, nullptr, std::move(complete), &result));
    return result;
}

NativeValue* JsWindow::OnSetKeepScreenOn(NativeEngine& engine, NativeCallbackInfo& info)
{
    WMError errCode = WMError::WM_OK;
    if (info.argc < 1 || info.argc > 2) { // 2: maximum params num
        WLOGFE("[NAPI]Argc is invalid: %{public}zu", info.argc);
        errCode = WMError::WM_ERROR_INVALID_PARAM;
    }
    bool keepScreenOn = true;
    if (errCode == WMError::WM_OK) {
        NativeBoolean* nativeVal = ConvertNativeValueTo<NativeBoolean>(info.argv[0]);
        if (nativeVal == nullptr) {
            WLOGFE("[NAPI]Failed to convert parameter to keepScreenOn");
            errCode = WMError::WM_ERROR_INVALID_PARAM;
        } else {
            keepScreenOn = static_cast<bool>(*nativeVal);
        }
    }

    wptr<Window> weakToken(windowToken_);
    AsyncTask::CompleteCallback complete =
        [weakToken, keepScreenOn, errCode](NativeEngine& engine, AsyncTask& task, int32_t status) {
            auto weakWindow = weakToken.promote();
            if (weakWindow == nullptr || errCode != WMError::WM_OK) {
                task.Reject(engine, CreateJsError(engine, static_cast<int32_t>(errCode), "Invalidate params."));
                return;
            }
            WMError ret = weakWindow->SetKeepScreenOn(keepScreenOn);
            if (ret == WMError::WM_OK) {
                task.Resolve(engine, engine.CreateUndefined());
            } else {
                task.Reject(engine, CreateJsError(engine, static_cast<int32_t>(ret),
                    "Window set keep screen on failed"));
            }
            WLOGFD("[NAPI]Window [%{public}u, %{public}s] set keep screen on end",
                weakWindow->GetWindowId(), weakWindow->GetWindowName().c_str());
        };

    NativeValue* lastParam = (info.argc <= 1) ? nullptr :
        (info.argv[1]->TypeOf() == NATIVE_FUNCTION ? info.argv[1] : nullptr);
    NativeValue* result = nullptr;
    AsyncTask::Schedule("JsWindow::OnSetKeepScreenOn",
        engine, CreateAsyncTaskWithLastParam(engine, lastParam, nullptr, std::move(complete), &result));
    return result;
}

NativeValue* JsWindow::OnSetWindowKeepScreenOn(NativeEngine& engine, NativeCallbackInfo& info)
{
    WmErrorCode errCode = WmErrorCode::WM_OK;
    if (info.argc < 1) { // 1: params num
        WLOGFE("[NAPI]Argc is invalid: %{public}zu", info.argc);
        errCode = WmErrorCode::WM_ERROR_INVALID_PARAM;
    }
    bool keepScreenOn = true;
    if (errCode == WmErrorCode::WM_OK) {
        NativeBoolean* nativeVal = ConvertNativeValueTo<NativeBoolean>(info.argv[0]);
        if (nativeVal == nullptr) {
            WLOGFE("[NAPI]Failed to convert parameter to keepScreenOn");
            errCode = WmErrorCode::WM_ERROR_INVALID_PARAM;
        } else {
            keepScreenOn = static_cast<bool>(*nativeVal);
        }
    }
    if (errCode == WmErrorCode::WM_ERROR_INVALID_PARAM) {
        engine.Throw(CreateJsError(engine, static_cast<int32_t>(WmErrorCode::WM_ERROR_INVALID_PARAM)));
        return engine.CreateUndefined();
    }

    wptr<Window> weakToken(windowToken_);
    AsyncTask::CompleteCallback complete =
        [weakToken, keepScreenOn](NativeEngine& engine, AsyncTask& task, int32_t status) {
            auto weakWindow = weakToken.promote();
            if (weakWindow == nullptr) {
                task.Reject(engine,
                    CreateJsError(engine, static_cast<int32_t>(WmErrorCode::WM_ERROR_STATE_ABNORMALLY),
                    "Invalidate params."));
                return;
            }
            WmErrorCode ret = WM_JS_TO_ERROR_CODE_MAP.at(weakWindow->SetKeepScreenOn(keepScreenOn));
            if (ret == WmErrorCode::WM_OK) {
                task.Resolve(engine, engine.CreateUndefined());
            } else {
                task.Reject(engine, CreateJsError(engine, static_cast<int32_t>(ret),
                    "Window set keep screen on failed"));
            }
            WLOGFD("[NAPI]Window [%{public}u, %{public}s] set keep screen on end",
                weakWindow->GetWindowId(), weakWindow->GetWindowName().c_str());
        };

    NativeValue* lastParam = (info.argc <= 1) ? nullptr :
        ((info.argv[1] != nullptr && info.argv[1]->TypeOf() == NATIVE_FUNCTION) ?
        info.argv[1] : nullptr);
    NativeValue* result = nullptr;
    AsyncTask::Schedule("JsWindow::OnSetWindowKeepScreenOn",
        engine, CreateAsyncTaskWithLastParam(engine, lastParam, nullptr, std::move(complete), &result));
    return result;
}

NativeValue* JsWindow::OnSetWakeUpScreen(NativeEngine& engine, NativeCallbackInfo& info)
{
    if (!Permission::IsSystemCalling()) {
        WLOGFE("set wake up screen permission denied!");
        engine.Throw(CreateJsError(engine, static_cast<int32_t>(WmErrorCode::WM_ERROR_INVALID_CALLING)));
        return engine.CreateUndefined();
    }
    WmErrorCode errCode = WmErrorCode::WM_OK;
    if (info.argc < 1 || windowToken_ == nullptr) {
        WLOGFE("[NAPI]Argc is invalid: %{public}zu", info.argc);
        engine.Throw(CreateJsError(engine, static_cast<int32_t>(WmErrorCode::WM_ERROR_INVALID_PARAM)));
        return engine.CreateUndefined();
    }
    bool wakeUp = false;
    if (errCode == WmErrorCode::WM_OK) {
        NativeBoolean* nativeVal = ConvertNativeValueTo<NativeBoolean>(info.argv[0]);
        if (nativeVal == nullptr) {
            WLOGFE("[NAPI]Failed to convert parameter to keepScreenOn");
            engine.Throw(CreateJsError(engine, static_cast<int32_t>(WmErrorCode::WM_ERROR_INVALID_PARAM)));
            return engine.CreateUndefined();
        } else {
            wakeUp = static_cast<bool>(*nativeVal);
        }
    }

    windowToken_->SetTurnScreenOn(wakeUp);
    WLOGFD("[NAPI]Window [%{public}u, %{public}s] set wake up screen %{public}d end",
        windowToken_->GetWindowId(), windowToken_->GetWindowName().c_str(), wakeUp);
    return engine.CreateUndefined();
}

NativeValue* JsWindow::OnSetOutsideTouchable(NativeEngine& engine, NativeCallbackInfo& info)
{
    AsyncTask::CompleteCallback complete =
        [](NativeEngine& engine, AsyncTask& task, int32_t status) {
            task.Reject(engine, CreateJsError(engine, static_cast<int32_t>(WMError::WM_ERROR_DEVICE_NOT_SUPPORT)));
        };

    NativeValue* lastParam = (info.argc <= 1) ? nullptr :
        (info.argv[1]->TypeOf() == NATIVE_FUNCTION ? info.argv[1] : nullptr);
    NativeValue* result = nullptr;
    AsyncTask::Schedule("JsWindow::OnSetOutsideTouchable",
        engine, CreateAsyncTaskWithLastParam(engine, lastParam, nullptr, std::move(complete), &result));
    return result;
}

NativeValue* JsWindow::OnSetPrivacyMode(NativeEngine& engine, NativeCallbackInfo& info)
{
    WMError errCode = WMError::WM_OK;
    if (info.argc < 1 || info.argc > 2) { // 2: maximum params num
        WLOGFE("[NAPI]Argc is invalid: %{public}zu", info.argc);
        errCode = WMError::WM_ERROR_INVALID_PARAM;
    }
    bool isPrivacyMode = false;
    if (errCode == WMError::WM_OK) {
        NativeBoolean* nativeVal = ConvertNativeValueTo<NativeBoolean>(info.argv[0]);
        if (nativeVal == nullptr) {
            WLOGFE("[NAPI]Failed to convert parameter to isPrivacyMode");
            errCode = WMError::WM_ERROR_INVALID_PARAM;
        } else {
            isPrivacyMode = static_cast<bool>(*nativeVal);
        }
    }

    wptr<Window> weakToken(windowToken_);
    AsyncTask::CompleteCallback complete =
        [weakToken, isPrivacyMode, errCode](NativeEngine& engine, AsyncTask& task, int32_t status) {
            auto weakWindow = weakToken.promote();
            if (weakWindow == nullptr || errCode != WMError::WM_OK) {
                task.Reject(engine, CreateJsError(engine, static_cast<int32_t>(errCode), "Invalidate params"));
                return;
            }
            weakWindow->SetPrivacyMode(isPrivacyMode);
            task.Resolve(engine, engine.CreateUndefined());
            WLOGFD("[NAPI]Window [%{public}u, %{public}s] set privacy mode end, mode = %{public}u",
                weakWindow->GetWindowId(), weakWindow->GetWindowName().c_str(), isPrivacyMode);
        };

    NativeValue* lastParam = (info.argc <= 1) ? nullptr :
        (info.argv[1]->TypeOf() == NATIVE_FUNCTION ? info.argv[1] : nullptr);
    NativeValue* result = nullptr;
    AsyncTask::Schedule("JsWindow::OnSetPrivacyMode",
        engine, CreateAsyncTaskWithLastParam(engine, lastParam, nullptr, std::move(complete), &result));
    return result;
}

NativeValue* JsWindow::OnSetWindowPrivacyMode(NativeEngine& engine, NativeCallbackInfo& info)
{
    WmErrorCode errCode = WmErrorCode::WM_OK;
    if (info.argc < 1) { // 1: params num
        WLOGFE("[NAPI]Argc is invalid: %{public}zu", info.argc);
        errCode = WmErrorCode::WM_ERROR_INVALID_PARAM;
    }
    bool isPrivacyMode = false;
    if (errCode == WmErrorCode::WM_OK) {
        NativeBoolean* nativeVal = ConvertNativeValueTo<NativeBoolean>(info.argv[0]);
        if (nativeVal == nullptr) {
            WLOGFE("[NAPI]Failed to convert parameter to isPrivacyMode");
            errCode = WmErrorCode::WM_ERROR_INVALID_PARAM;
        } else {
            isPrivacyMode = static_cast<bool>(*nativeVal);
        }
    }
    if (errCode == WmErrorCode::WM_ERROR_INVALID_PARAM) {
        engine.Throw(CreateJsError(engine, static_cast<int32_t>(WmErrorCode::WM_ERROR_INVALID_PARAM)));
        return engine.CreateUndefined();
    }

    wptr<Window> weakToken(windowToken_);
    AsyncTask::CompleteCallback complete =
        [weakToken, isPrivacyMode](NativeEngine& engine, AsyncTask& task, int32_t status) {
            auto weakWindow = weakToken.promote();
            if (weakWindow == nullptr) {
                task.Reject(engine,
                    CreateJsError(engine, static_cast<int32_t>(WmErrorCode::WM_ERROR_STATE_ABNORMALLY),
                    "Invalidate params"));
                return;
            }
            weakWindow->SetPrivacyMode(isPrivacyMode);
            task.Resolve(engine, engine.CreateUndefined());
            WLOGFD("[NAPI]Window [%{public}u, %{public}s] set privacy mode end, mode = %{public}u",
                weakWindow->GetWindowId(), weakWindow->GetWindowName().c_str(), isPrivacyMode);
        };

    NativeValue* lastParam = (info.argc <= 1) ? nullptr :
        ((info.argv[1] != nullptr && info.argv[1]->TypeOf() == NATIVE_FUNCTION) ?
        info.argv[1] : nullptr);
    NativeValue* result = nullptr;
    AsyncTask::Schedule("JsWindow::OnSetWindowPrivacyMode",
        engine, CreateAsyncTaskWithLastParam(engine, lastParam, nullptr, std::move(complete), &result));
    return result;
}

NativeValue* JsWindow::OnSetTouchable(NativeEngine& engine, NativeCallbackInfo& info)
{
    WMError errCode = WMError::WM_OK;
    if (info.argc < 1 || info.argc > 2) { // 2: maximum params num
        WLOGFE("[NAPI]Argc is invalid: %{public}zu", info.argc);
        errCode = WMError::WM_ERROR_INVALID_PARAM;
    }
    bool touchable = true;
    if (errCode == WMError::WM_OK) {
        NativeBoolean* nativeVal = ConvertNativeValueTo<NativeBoolean>(info.argv[0]);
        if (nativeVal == nullptr) {
            WLOGFE("[NAPI]Failed to convert parameter to touchable");
            errCode = WMError::WM_ERROR_INVALID_PARAM;
        } else {
            touchable = static_cast<bool>(*nativeVal);
        }
    }

    wptr<Window> weakToken(windowToken_);
    AsyncTask::CompleteCallback complete =
        [weakToken, touchable, errCode](NativeEngine& engine, AsyncTask& task, int32_t status) {
            auto weakWindow = weakToken.promote();
            if (weakWindow == nullptr || errCode != WMError::WM_OK) {
                task.Reject(engine, CreateJsError(engine, static_cast<int32_t>(errCode), "Invalidate params."));
                return;
            }
            WMError ret = weakWindow->SetTouchable(touchable);
            if (ret == WMError::WM_OK) {
                task.Resolve(engine, engine.CreateUndefined());
            } else {
                task.Reject(engine, CreateJsError(engine, static_cast<int32_t>(ret), "Window set touchable failed"));
            }
            WLOGFD("[NAPI]Window [%{public}u, %{public}s] set touchable end",
                weakWindow->GetWindowId(), weakWindow->GetWindowName().c_str());
        };

    NativeValue* lastParam = (info.argc <= 1) ? nullptr :
        (info.argv[1]->TypeOf() == NATIVE_FUNCTION ? info.argv[1] : nullptr);
    NativeValue* result = nullptr;
    AsyncTask::Schedule("JsWindow::OnSetTouchable",
        engine, CreateAsyncTaskWithLastParam(engine, lastParam, nullptr, std::move(complete), &result));
    return result;
}

NativeValue* JsWindow::OnSetWindowTouchable(NativeEngine& engine, NativeCallbackInfo& info)
{
    WmErrorCode errCode = WmErrorCode::WM_OK;
    if (info.argc < 1) { // 1: params num
        WLOGFE("[NAPI]Argc is invalid: %{public}zu", info.argc);
        errCode = WmErrorCode::WM_ERROR_INVALID_PARAM;
    }
    bool touchable = true;
    if (errCode == WmErrorCode::WM_OK) {
        NativeBoolean* nativeVal = ConvertNativeValueTo<NativeBoolean>(info.argv[0]);
        if (nativeVal == nullptr) {
            WLOGFE("[NAPI]Failed to convert parameter to touchable");
            errCode = WmErrorCode::WM_ERROR_INVALID_PARAM;
        } else {
            touchable = static_cast<bool>(*nativeVal);
        }
    }
    if (errCode == WmErrorCode::WM_ERROR_INVALID_PARAM) {
        engine.Throw(CreateJsError(engine, static_cast<int32_t>(WmErrorCode::WM_ERROR_INVALID_PARAM)));
        return engine.CreateUndefined();
    }

    wptr<Window> weakToken(windowToken_);
    AsyncTask::CompleteCallback complete =
        [weakToken, touchable](NativeEngine& engine, AsyncTask& task, int32_t status) {
            auto weakWindow = weakToken.promote();
            if (weakWindow == nullptr) {
                task.Reject(engine,
                    CreateJsError(engine, static_cast<int32_t>(WmErrorCode::WM_ERROR_STATE_ABNORMALLY),
                    "Invalidate params."));
                return;
            }
            WmErrorCode ret = WM_JS_TO_ERROR_CODE_MAP.at(weakWindow->SetTouchable(touchable));
            if (ret == WmErrorCode::WM_OK) {
                task.Resolve(engine, engine.CreateUndefined());
            } else {
                task.Reject(engine, CreateJsError(engine, static_cast<int32_t>(ret), "Window set touchable failed"));
            }
            WLOGFD("[NAPI]Window [%{public}u, %{public}s] set touchable end",
                weakWindow->GetWindowId(), weakWindow->GetWindowName().c_str());
        };

    NativeValue* lastParam = (info.argc <= 1) ? nullptr :
        ((info.argv[1] != nullptr && info.argv[1]->TypeOf() == NATIVE_FUNCTION) ?
        info.argv[1] : nullptr);
    NativeValue* result = nullptr;
    AsyncTask::Schedule("JsWindow::OnSetWindowTouchable",
        engine, CreateAsyncTaskWithLastParam(engine, lastParam, nullptr, std::move(complete), &result));
    return result;
}

NativeValue* JsWindow::OnSetTransparent(NativeEngine& engine, NativeCallbackInfo& info)
{
    WMError errCode = WMError::WM_OK;
    if (info.argc < 1 || info.argc > 2) { // 2: maximum params num
        WLOGFE("[NAPI]Argc is invalid: %{public}zu", info.argc);
        errCode = WMError::WM_ERROR_INVALID_PARAM;
    }
    bool isTransparent = true;
    if (errCode == WMError::WM_OK) {
        NativeBoolean* nativeVal = ConvertNativeValueTo<NativeBoolean>(info.argv[0]);
        if (nativeVal == nullptr) {
            WLOGFE("[NAPI]Failed to convert parameter to isTransparent");
            errCode = WMError::WM_ERROR_INVALID_PARAM;
        } else {
            isTransparent = static_cast<bool>(*nativeVal);
        }
    }

    wptr<Window> weakToken(windowToken_);
    AsyncTask::CompleteCallback complete =
        [weakToken, isTransparent, errCode](NativeEngine& engine, AsyncTask& task, int32_t status) {
            auto weakWindow = weakToken.promote();
            if (weakWindow == nullptr || errCode != WMError::WM_OK) {
                task.Reject(engine, CreateJsError(engine, static_cast<int32_t>(errCode), "Invalidate params."));
                return;
            }
            WMError ret = weakWindow->SetTransparent(isTransparent);
            if (ret == WMError::WM_OK) {
                task.Resolve(engine, engine.CreateUndefined());
            } else {
                task.Reject(engine, CreateJsError(engine, static_cast<int32_t>(ret), "Window set transparent failed"));
            }
            WLOGFD("[NAPI]Window [%{public}u, %{public}s] set transparent end",
                weakWindow->GetWindowId(), weakWindow->GetWindowName().c_str());
        };

    NativeValue* lastParam = (info.argc <= 1) ? nullptr :
        (info.argv[1]->TypeOf() == NATIVE_FUNCTION ? info.argv[1] : nullptr);
    NativeValue* result = nullptr;
    AsyncTask::Schedule("JsWindow::OnSetTransparent",
        engine, CreateAsyncTaskWithLastParam(engine, lastParam, nullptr, std::move(complete), &result));
    return result;
}

NativeValue* JsWindow::OnSetCallingWindow(NativeEngine& engine, NativeCallbackInfo& info)
{
    WMError errCode = WMError::WM_OK;
    if (info.argc < 1 || info.argc > 2) { // 2: maximum params num
        WLOGFE("[NAPI]Argc is invalid: %{public}zu", info.argc);
        errCode = WMError::WM_ERROR_INVALID_PARAM;
    }
    uint32_t callingWindow = INVALID_WINDOW_ID;
    if (errCode == WMError::WM_OK) {
        NativeNumber* nativeVal = ConvertNativeValueTo<NativeNumber>(info.argv[0]);
        if (nativeVal == nullptr) {
            WLOGFE("[NAPI]Failed to convert parameter to touchable");
            errCode = WMError::WM_ERROR_INVALID_PARAM;
        } else {
            callingWindow = static_cast<uint32_t>(*nativeVal);
        }
    }

    wptr<Window> weakToken(windowToken_);
    AsyncTask::CompleteCallback complete =
        [weakToken, callingWindow, errCode](NativeEngine& engine, AsyncTask& task, int32_t status) {
            auto weakWindow = weakToken.promote();
            if (weakWindow == nullptr || errCode != WMError::WM_OK) {
                task.Reject(engine, CreateJsError(engine, static_cast<int32_t>(errCode), "Invalidate params."));
                return;
            }
            WMError ret = weakWindow->SetCallingWindow(callingWindow);
            if (ret == WMError::WM_OK) {
                task.Resolve(engine, engine.CreateUndefined());
            } else {
                task.Reject(engine, CreateJsError(engine, static_cast<int32_t>(ret),
                    "Window set calling window failed"));
            }
            WLOGFD("[NAPI]Window [%{public}u, %{public}s] set calling window end",
                weakWindow->GetWindowId(), weakWindow->GetWindowName().c_str());
        };

    NativeValue* lastParam = (info.argc <= 1) ? nullptr :
        (info.argv[1]->TypeOf() == NATIVE_FUNCTION ? info.argv[1] : nullptr);
    NativeValue* result = nullptr;
    AsyncTask::Schedule("JsWindow::OnSetCallingWindow",
        engine, CreateAsyncTaskWithLastParam(engine, lastParam, nullptr, std::move(complete), &result));
    return result;
}

NativeValue* JsWindow::OnDisableWindowDecor(NativeEngine& engine, NativeCallbackInfo& info)
{
    if (windowToken_ == nullptr) {
        return engine.CreateUndefined();
    }
    windowToken_->DisableAppWindowDecor();
    WLOGFD("[NAPI]Window [%{public}u, %{public}s] disable app window decor end",
        windowToken_->GetWindowId(), windowToken_->GetWindowName().c_str());
    return engine.CreateUndefined();
}

NativeValue* JsWindow::OnSetColorSpace(NativeEngine& engine, NativeCallbackInfo& info)
{
    WMError errCode = WMError::WM_OK;
    ColorSpace colorSpace = ColorSpace::COLOR_SPACE_DEFAULT;
    if (info.argc < 1 || info.argc > 2) { // 2: maximum params num
        WLOGFE("[NAPI]Argc is invalid: %{public}zu", info.argc);
        errCode = WMError::WM_ERROR_INVALID_PARAM;
    } else {
        NativeNumber* nativeType = ConvertNativeValueTo<NativeNumber>(info.argv[0]);
        if (nativeType == nullptr) {
            errCode = WMError::WM_ERROR_INVALID_PARAM;
            WLOGFE("[NAPI]Failed to convert parameter to ColorSpace");
        } else {
            colorSpace = static_cast<ColorSpace>(static_cast<uint32_t>(*nativeType));
            if (colorSpace > ColorSpace::COLOR_SPACE_WIDE_GAMUT) {
                WLOGFE("[NAPI]ColorSpace %{public}u invalid!", static_cast<uint32_t>(colorSpace));
                errCode = WMError::WM_ERROR_INVALID_PARAM;
            }
        }
    }

    wptr<Window> weakToken(windowToken_);
    AsyncTask::CompleteCallback complete =
        [weakToken, colorSpace, errCode](NativeEngine& engine, AsyncTask& task, int32_t status) {
            auto weakWindow = weakToken.promote();
            if (weakWindow == nullptr || errCode != WMError::WM_OK) {
                task.Reject(engine, CreateJsError(engine, static_cast<int32_t>(errCode), "OnSetColorSpace failed"));
                WLOGFE("[NAPI]window is nullptr or get invalid param");
                return;
            }
            weakWindow->SetColorSpace(colorSpace);
            task.Resolve(engine, engine.CreateUndefined());
            WLOGFD("[NAPI]Window [%{public}u, %{public}s] OnSetColorSpace end, colorSpace = %{public}u",
                weakWindow->GetWindowId(), weakWindow->GetWindowName().c_str(), static_cast<uint32_t>(colorSpace));
        };

    NativeValue* lastParam = (info.argc <= 1) ? nullptr :
        (info.argv[1]->TypeOf() == NATIVE_FUNCTION ? info.argv[1] : nullptr);
    NativeValue* result = nullptr;
    AsyncTask::Schedule("JsWindow::OnSetColorSpace",
        engine, CreateAsyncTaskWithLastParam(engine, lastParam, nullptr, std::move(complete), &result));
    return result;
}

NativeValue* JsWindow::OnSetWindowColorSpace(NativeEngine& engine, NativeCallbackInfo& info)
{
    WmErrorCode errCode = WmErrorCode::WM_OK;
    ColorSpace colorSpace = ColorSpace::COLOR_SPACE_DEFAULT;
    if (info.argc < 1) { // 1: params num
        WLOGFE("[NAPI]Argc is invalid: %{public}zu", info.argc);
        errCode = WmErrorCode::WM_ERROR_INVALID_PARAM;
    } else {
        NativeNumber* nativeType = ConvertNativeValueTo<NativeNumber>(info.argv[0]);
        if (nativeType == nullptr) {
            errCode = WmErrorCode::WM_ERROR_INVALID_PARAM;
            WLOGFE("[NAPI]Failed to convert parameter to ColorSpace");
        } else {
            colorSpace = static_cast<ColorSpace>(static_cast<uint32_t>(*nativeType));
            if (colorSpace > ColorSpace::COLOR_SPACE_WIDE_GAMUT) {
                WLOGFE("[NAPI]ColorSpace %{public}u invalid!", static_cast<uint32_t>(colorSpace));
                errCode = WmErrorCode::WM_ERROR_INVALID_PARAM;
            }
        }
    }
    if (errCode == WmErrorCode::WM_ERROR_INVALID_PARAM) {
        engine.Throw(CreateJsError(engine, static_cast<int32_t>(WmErrorCode::WM_ERROR_INVALID_PARAM)));
        return engine.CreateUndefined();
    }

    wptr<Window> weakToken(windowToken_);
    AsyncTask::CompleteCallback complete =
        [weakToken, colorSpace](NativeEngine& engine, AsyncTask& task, int32_t status) {
            auto weakWindow = weakToken.promote();
            if (weakWindow == nullptr) {
                WLOGFE("[NAPI]window is nullptr");
                task.Reject(engine,
                    CreateJsError(engine, static_cast<int32_t>(WmErrorCode::WM_ERROR_STATE_ABNORMALLY),
                    "OnSetWindowColorSpace failed"));
                return;
            }
            weakWindow->SetColorSpace(colorSpace);
            task.Resolve(engine, engine.CreateUndefined());
            WLOGFD("[NAPI]Window [%{public}u, %{public}s] OnSetWindowColorSpace end, colorSpace = %{public}u",
                weakWindow->GetWindowId(), weakWindow->GetWindowName().c_str(), static_cast<uint32_t>(colorSpace));
        };

    NativeValue* lastParam = (info.argc <= 1) ? nullptr :
        ((info.argv[1] != nullptr && info.argv[1]->TypeOf() == NATIVE_FUNCTION) ?
        info.argv[1] : nullptr);
    NativeValue* result = nullptr;
    AsyncTask::Schedule("JsWindow::OnSetWindowColorSpace",
        engine, CreateAsyncTaskWithLastParam(engine, lastParam, nullptr, std::move(complete), &result));
    return result;
}

NativeValue* JsWindow::OnGetColorSpace(NativeEngine& engine, NativeCallbackInfo& info)
{
    WMError errCode = WMError::WM_OK;
    if (info.argc > 1) {
        WLOGFE("[NAPI]Argc is invalid: %{public}zu", info.argc);
        errCode = WMError::WM_ERROR_INVALID_PARAM;
    }
    wptr<Window> weakToken(windowToken_);
    AsyncTask::CompleteCallback complete =
        [weakToken, errCode](NativeEngine& engine, AsyncTask& task, int32_t status) {
            auto weakWindow = weakToken.promote();
            if (weakWindow == nullptr || errCode != WMError::WM_OK) {
                task.Reject(engine, CreateJsError(engine, static_cast<int32_t>(errCode)));
                WLOGFE("[NAPI]window is nullptr or get invalid param");
                return;
            }
            ColorSpace colorSpace = weakWindow->GetColorSpace();
            task.Resolve(engine, CreateJsValue(engine, static_cast<uint32_t>(colorSpace)));
            WLOGFD("[NAPI]Window [%{public}u, %{public}s] OnGetColorSpace end, colorSpace = %{public}u",
                weakWindow->GetWindowId(), weakWindow->GetWindowName().c_str(), static_cast<uint32_t>(colorSpace));
        };

    NativeValue* lastParam = (info.argc == 0) ? nullptr :
        (info.argv[0]->TypeOf() == NATIVE_FUNCTION ? info.argv[0] : nullptr);
    NativeValue* result = nullptr;
    AsyncTask::Schedule("JsWindow::OnGetColorSpace",
        engine, CreateAsyncTaskWithLastParam(engine, lastParam, nullptr, std::move(complete), &result));
    return result;
}

NativeValue* JsWindow::OnGetWindowColorSpaceSync(NativeEngine& engine, NativeCallbackInfo& info)
{
    wptr<Window> weakToken(windowToken_);
    auto window = weakToken.promote();
    if (window == nullptr) {
        WLOGFE("[NAPI]window is nullptr");
        engine.Throw(CreateJsError(engine, static_cast<int32_t>(WmErrorCode::WM_ERROR_STATE_ABNORMALLY)));
        return engine.CreateUndefined();
    }
    ColorSpace colorSpace = window->GetColorSpace();
    WLOGFD("[NAPI]Window [%{public}u, %{public}s] OnGetColorSpace end, colorSpace = %{public}u",
        window->GetWindowId(), window->GetWindowName().c_str(), static_cast<uint32_t>(colorSpace));

    return CreateJsValue(engine, static_cast<uint32_t>(colorSpace));
}

NativeValue* JsWindow::OnDump(NativeEngine& engine, NativeCallbackInfo& info)
{
    WLOGFD("[NAPI]dump window start");
    if (info.argc < 1 || info.argc > 2) { // 2: maximum params num
        WLOGFE("[NAPI]Argc is invalid: %{public}zu", info.argc);
        return nullptr;
    }
    if (windowToken_ == nullptr) {
        WLOGFE("[NAPI]window is nullptr or get invalid param");
        return nullptr;
    }
    std::vector<std::string> params;
    if (!ConvertNativeValueToVector(engine, info.argv[0], params)) {
        WLOGFE("[NAPI]ConvertNativeValueToVector fail");
        return nullptr;
    }
    std::vector<std::string> dumpInfo;
    windowToken_->DumpInfo(params, dumpInfo);
    NativeValue* dumpInfoValue = CreateNativeArray(engine, dumpInfo);
    WLOGFD("[NAPI]Window [%{public}u, %{public}s] dump end",
        windowToken_->GetWindowId(), windowToken_->GetWindowName().c_str());
    return dumpInfoValue;
}

NativeValue* JsWindow::Snapshot(NativeEngine* engine, NativeCallbackInfo* info)
{
    WLOGFD("[NAPI]Snapshot");
    JsWindow* me = CheckParamsAndGetThis<JsWindow>(engine, info);
    return (me != nullptr) ? me->OnSnapshot(*engine, *info) : nullptr;
}

NativeValue* JsWindow::OnSetForbidSplitMove(NativeEngine& engine, NativeCallbackInfo& info)
{
    WmErrorCode errCode = WmErrorCode::WM_OK;
    if (info.argc < 1) { // 1: params num
        errCode = WmErrorCode::WM_ERROR_INVALID_PARAM;
    }
    bool isForbidSplitMove = false;
    if (errCode == WmErrorCode::WM_OK) {
        NativeBoolean* nativeVal = ConvertNativeValueTo<NativeBoolean>(info.argv[0]);
        if (nativeVal == nullptr) {
            errCode = WmErrorCode::WM_ERROR_INVALID_PARAM;
        } else {
            isForbidSplitMove = static_cast<bool>(*nativeVal);
        }
    }
    if (errCode == WmErrorCode::WM_ERROR_INVALID_PARAM) {
        engine.Throw(CreateJsError(engine, static_cast<int32_t>(WmErrorCode::WM_ERROR_INVALID_PARAM)));
        return engine.CreateUndefined();
    }
    wptr<Window> weakToken(windowToken_);
    AsyncTask::CompleteCallback complete =
        [weakToken, isForbidSplitMove](NativeEngine& engine, AsyncTask& task, int32_t status) {
            auto weakWindow = weakToken.promote();
            if (weakWindow == nullptr) {
                task.Reject(engine, CreateJsError(engine,
                    static_cast<int32_t>(WmErrorCode::WM_ERROR_STATE_ABNORMALLY), "Invalidate params."));
                return;
            }
            WmErrorCode ret;
            if (isForbidSplitMove) {
                ret = WM_JS_TO_ERROR_CODE_MAP.at(
                    weakWindow->AddWindowFlag(WindowFlag::WINDOW_FLAG_FORBID_SPLIT_MOVE));
            } else {
                ret = WM_JS_TO_ERROR_CODE_MAP.at(
                    weakWindow->RemoveWindowFlag(WindowFlag::WINDOW_FLAG_FORBID_SPLIT_MOVE));
            }
            if (ret == WmErrorCode::WM_OK) {
                task.Resolve(engine, engine.CreateUndefined());
            } else {
                task.Reject(engine, CreateJsError(engine,
                    static_cast<int32_t>(ret), "Window OnSetForbidSplitMove failed."));
            }
        };
    NativeValue* lastParam = (info.argc <= 1) ? nullptr :
        ((info.argv[1] != nullptr && info.argv[1]->TypeOf() == NATIVE_FUNCTION) ?
        info.argv[1] : nullptr);
    NativeValue* result = nullptr;
    AsyncTask::Schedule("JsWindow::OnSetForbidSplitMove",
        engine, CreateAsyncTaskWithLastParam(engine, lastParam, nullptr, std::move(complete), &result));
    return result;
}

NativeValue* JsWindow::OnSnapshot(NativeEngine& engine, NativeCallbackInfo& info)
{
    wptr<Window> weakToken(windowToken_);
    AsyncTask::CompleteCallback complete =
        [weakToken](NativeEngine& engine, AsyncTask& task, int32_t status) {
            auto weakWindow = weakToken.promote();
            if (weakWindow == nullptr) {
                WLOGFE("[NAPI]window is nullptr");
                task.Reject(engine, CreateJsError(engine,
                    static_cast<int32_t>(WmErrorCode::WM_ERROR_STATE_ABNORMALLY)));
                return;
            }

            std::shared_ptr<Media::PixelMap> pixelMap = weakWindow->Snapshot();
            if (pixelMap == nullptr) {
                task.Reject(engine, CreateJsError(engine,
                    static_cast<int32_t>(WmErrorCode::WM_ERROR_STATE_ABNORMALLY)));
                WLOGFE("[NAPI]window snapshot get pixelmap is null");
                return;
            }

            auto nativePixelMap = reinterpret_cast<NativeValue*>(
                Media::PixelMapNapi::CreatePixelMap(reinterpret_cast<napi_env>(&engine), pixelMap));
            if (nativePixelMap == nullptr) {
                WLOGFE("[NAPI]window snapshot get nativePixelMap is null");
            }
            task.Resolve(engine, nativePixelMap);
            WLOGFD("[NAPI]Window [%{public}u, %{public}s] OnSnapshot, WxH=%{public}dx%{public}d",
                weakWindow->GetWindowId(), weakWindow->GetWindowName().c_str(),
                pixelMap->GetWidth(), pixelMap->GetHeight());
        };

    NativeValue* lastParam = (info.argc == 0) ? nullptr :
        ((info.argv[0] != nullptr && info.argv[0]->TypeOf() == NATIVE_FUNCTION) ?
        info.argv[0] : nullptr);
    NativeValue* result = nullptr;
    AsyncTask::Schedule("JsWindow::OnSnapshot",
        engine, CreateAsyncTaskWithLastParam(engine, lastParam, nullptr, std::move(complete), &result));
    return result;
}

NativeValue* JsWindow::OnSetSnapshotSkip(NativeEngine& engine, NativeCallbackInfo& info)
{
    WmErrorCode errCode = WmErrorCode::WM_OK;
    if (info.argc < 1) { // 1: params num
        WLOGFE("[NAPI] inbalid param");
        errCode = WmErrorCode::WM_ERROR_INVALID_PARAM;
    }
    bool isSkip = false;
    if (errCode == WmErrorCode::WM_OK) {
        NativeBoolean* nativeVal = ConvertNativeValueTo<NativeBoolean>(info.argv[0]);
        if (nativeVal == nullptr) {
            errCode = WmErrorCode::WM_ERROR_INVALID_PARAM;
        } else {
            isSkip = static_cast<bool>(*nativeVal);
        }
    }
    if (errCode == WmErrorCode::WM_ERROR_INVALID_PARAM) {
        engine.Throw(CreateJsError(engine, static_cast<int32_t>(WmErrorCode::WM_ERROR_INVALID_PARAM)));
        return engine.CreateUndefined();
    }

    wptr<Window> weakToken(windowToken_);
    auto window = weakToken.promote();
    if (window == nullptr) {
        engine.Throw(CreateJsError(engine, static_cast<int32_t>(WmErrorCode::WM_ERROR_STATE_ABNORMALLY)));
        return engine.CreateUndefined();
    }

    window->SetSnapshotSkip(isSkip);
    WLOGFD("NAPI [%{public}u, %{public}s] set snapshotSkip end",
        window->GetWindowId(), window->GetWindowName().c_str());

    return engine.CreateUndefined();
}

NativeValue* JsWindow::OnOpacity(NativeEngine& engine, NativeCallbackInfo& info)
{
    if (info.argc < 1) {
        WLOGFE("[NAPI]Argc is invalid: %{public}zu", info.argc);
        engine.Throw(CreateJsError(engine, static_cast<int32_t>(WmErrorCode::WM_ERROR_INVALID_PARAM)));
        return engine.CreateUndefined();
    }
    if (windowToken_ == nullptr) {
        WLOGFE("[NAPI]WindowToken_ is nullptr");
        engine.Throw(CreateJsError(engine, static_cast<int32_t>(WmErrorCode::WM_ERROR_STATE_ABNORMALLY)));
        return engine.CreateUndefined();
    }
    if (!WindowHelper::IsSystemWindow(windowToken_->GetType())) {
        WLOGFE("[NAPI]Opacity is not allowed since window is not system window");
        engine.Throw(CreateJsError(engine, static_cast<int32_t>(WmErrorCode::WM_ERROR_INVALID_CALLING)));
        return engine.CreateUndefined();
    }
    NativeNumber* nativeVal = ConvertNativeValueTo<NativeNumber>(info.argv[0]);
    if (nativeVal == nullptr) {
        WLOGFE("[NAPI]Failed to convert parameter to alpha");
        engine.Throw(CreateJsError(engine, static_cast<int32_t>(WmErrorCode::WM_ERROR_INVALID_PARAM)));
        return engine.CreateUndefined();
    }
    if (MathHelper::LessNotEqual(*nativeVal, 0.0) || MathHelper::GreatNotEqual(*nativeVal, 1.0)) {
        WLOGFE("[NAPI]alpha should greater than 0 or smaller than 1.0");
        engine.Throw(CreateJsError(engine, static_cast<int32_t>(WmErrorCode::WM_ERROR_INVALID_PARAM)));
        return engine.CreateUndefined();
    }
    float alpha = static_cast<double>(*nativeVal);
    windowToken_->SetAlpha(alpha);
    WLOGFD("[NAPI]Window [%{public}u, %{public}s] Opacity end, alpha = %{public}f",
        windowToken_->GetWindowId(), windowToken_->GetWindowName().c_str(), alpha);
    return engine.CreateUndefined();
}

static bool IsPivotValid(double data)
{
    if (MathHelper::LessNotEqual(data, 0.0) || (MathHelper::GreatNotEqual(data, 1.0))) {
        return false;
    }
    return true;
}

static bool IsScaleValid(double data)
{
    if (!MathHelper::GreatNotEqual(data, 0.0)) {
        return false;
    }
    return true;
}

bool JsWindow::ParseScaleOption(NativeEngine& engine, NativeObject* jsObject, Transform& trans)
{
    double data = 0.0f;
    if (ParseJsValue(jsObject, engine, "pivotX", data)) {
        if (!IsPivotValid(data)) {
            return false;
        }
        trans.pivotX_ = data;
    }
    if (ParseJsValue(jsObject, engine, "pivotY", data)) {
        if (!IsPivotValid(data)) {
            return false;
        }
        trans.pivotY_ = data;
    }
    if (ParseJsValue(jsObject, engine, "x", data)) {
        if (!IsScaleValid(data)) {
            return false;
        }
        trans.scaleX_ = data;
    }
    if (ParseJsValue(jsObject, engine, "y", data)) {
        if (!IsScaleValid(data)) {
            return false;
        }
        trans.scaleY_ = data;
    }
    return true;
}

NativeValue* JsWindow::OnScale(NativeEngine& engine, NativeCallbackInfo& info)
{
    if (info.argc < 1) {
        WLOGFE("[NAPI]Argc is invalid: %{public}zu", info.argc);
        engine.Throw(CreateJsError(engine, static_cast<int32_t>(WmErrorCode::WM_ERROR_INVALID_PARAM)));
        return engine.CreateUndefined();
    }
    if (windowToken_ == nullptr) {
        WLOGFE("[NAPI]WindowToken_ is nullptr");
        engine.Throw(CreateJsError(engine, static_cast<int32_t>(WmErrorCode::WM_ERROR_STATE_ABNORMALLY)));
        return engine.CreateUndefined();
    }
    if (!WindowHelper::IsSystemWindow(windowToken_->GetType())) {
        WLOGFE("[NAPI]Scale is not allowed since window is not system window");
        engine.Throw(CreateJsError(engine, static_cast<int32_t>(WmErrorCode::WM_ERROR_INVALID_CALLING)));
        return engine.CreateUndefined();
    }
    NativeObject* nativeObj = ConvertNativeValueTo<NativeObject>(info.argv[0]);
    if (nativeObj == nullptr) {
        WLOGFE("[NAPI]Failed to convert object to ScaleOptions");
        engine.Throw(CreateJsError(engine, static_cast<int32_t>(WmErrorCode::WM_ERROR_INVALID_PARAM)));
        return engine.CreateUndefined();
    }
    auto trans = windowToken_->GetTransform();
    if (!ParseScaleOption(engine, nativeObj, trans)) {
        WLOGFE("[NAPI] PivotX or PivotY should between 0.0 ~ 1.0, scale should greater than 0.0");
        engine.Throw(CreateJsError(engine, static_cast<int32_t>(WmErrorCode::WM_ERROR_INVALID_PARAM)));
        return engine.CreateUndefined();
    }
    windowToken_->SetTransform(trans);
    WLOGFD("[NAPI]Window [%{public}u, %{public}s] Scale end",
        windowToken_->GetWindowId(), windowToken_->GetWindowName().c_str());
    WLOGFD("[NAPI]scaleX = %{public}f, scaleY = %{public}f, pivotX = %{public}f pivotY = %{public}f",
        trans.scaleX_, trans.scaleY_, trans.pivotX_, trans.pivotY_);
    return engine.CreateUndefined();
}

bool JsWindow::ParseRotateOption(NativeEngine& engine, NativeObject* jsObject, Transform& trans)
{
    double data = 0.0f;
    if (ParseJsValue(jsObject, engine, "pivotX", data)) {
        if (!IsPivotValid(data)) {
            return false;
        }
        trans.pivotX_ = data;
    }
    if (ParseJsValue(jsObject, engine, "pivotY", data)) {
        if (!IsPivotValid(data)) {
            return false;
        }
        trans.pivotY_ = data;
    }
    if (ParseJsValue(jsObject, engine, "x", data)) {
        trans.rotationX_ = data;
    }
    if (ParseJsValue(jsObject, engine, "y", data)) {
        trans.rotationY_ = data;
    }
    if (ParseJsValue(jsObject, engine, "z", data)) {
        trans.rotationZ_ = data;
    }
    return true;
}

NativeValue* JsWindow::OnRotate(NativeEngine& engine, NativeCallbackInfo& info)
{
    if (info.argc < 1) {
        WLOGFE("[NAPI]Argc is invalid: %{public}zu", info.argc);
        engine.Throw(CreateJsError(engine, static_cast<int32_t>(WmErrorCode::WM_ERROR_INVALID_PARAM)));
        return engine.CreateUndefined();
    }
    if (windowToken_ == nullptr) {
        WLOGFE("[NAPI]WindowToken_ is nullptr");
        engine.Throw(CreateJsError(engine, static_cast<int32_t>(WmErrorCode::WM_ERROR_STATE_ABNORMALLY)));
        return engine.CreateUndefined();
    }
    if (!WindowHelper::IsSystemWindow(windowToken_->GetType())) {
        WLOGFE("[NAPI]Rotate is not allowed since window is not system window");
        engine.Throw(CreateJsError(engine, static_cast<int32_t>(WmErrorCode::WM_ERROR_INVALID_CALLING)));
        return engine.CreateUndefined();
    }
    NativeObject* nativeObj = ConvertNativeValueTo<NativeObject>(info.argv[0]);
    if (nativeObj == nullptr) {
        WLOGFE("[NAPI]Failed to convert object to RotateOptions");
        engine.Throw(CreateJsError(engine, static_cast<int32_t>(WmErrorCode::WM_ERROR_INVALID_PARAM)));
        return engine.CreateUndefined();
    }
    // cannot use sync task since next transform base on current transform
    auto trans = windowToken_->GetTransform();
    if (!ParseRotateOption(engine, nativeObj, trans)) {
        WLOGFE("[NAPI] PivotX or PivotY should between 0.0 ~ 1.0");
        engine.Throw(CreateJsError(engine, static_cast<int32_t>(WmErrorCode::WM_ERROR_INVALID_PARAM)));
        return engine.CreateUndefined();
    }
    windowToken_->SetTransform(trans);
    WLOGFD("[NAPI]Window [%{public}u, %{public}s] Rotate end",
        windowToken_->GetWindowId(), windowToken_->GetWindowName().c_str());
    WLOGFD("[NAPI]rotateX = %{public}f, rotateY = %{public}f," \
        "rotateZ = %{public}f pivotX = %{public}f pivotY = %{public}f",
        trans.rotationX_, trans.rotationY_, trans.rotationZ_, trans.pivotX_, trans.pivotY_);
    return engine.CreateUndefined();
}

bool JsWindow::ParseTranslateOption(NativeEngine& engine, NativeObject* jsObject, Transform& trans)
{
    double data = 0.0f;
    if (ParseJsValue(jsObject, engine, "x", data)) {
        trans.translateX_ = data;
    }
    if (ParseJsValue(jsObject, engine, "y", data)) {
        trans.translateY_ = data;
    }
    if (ParseJsValue(jsObject, engine, "z", data)) {
        trans.translateZ_ = data;
    }
    return true;
}

NativeValue* JsWindow::OnTranslate(NativeEngine& engine, NativeCallbackInfo& info)
{
    if (info.argc < 1) {
        WLOGFE("[NAPI]Argc is invalid: %{public}zu", info.argc);
        engine.Throw(CreateJsError(engine, static_cast<int32_t>(WmErrorCode::WM_ERROR_INVALID_PARAM)));
        return engine.CreateUndefined();
    }
    if (windowToken_ == nullptr) {
        WLOGFE("[NAPI]WindowToken_ is nullptr");
        engine.Throw(CreateJsError(engine, static_cast<int32_t>(WmErrorCode::WM_ERROR_STATE_ABNORMALLY)));
        return engine.CreateUndefined();
    }
    if (!WindowHelper::IsSystemWindow(windowToken_->GetType())) {
        WLOGFE("[NAPI]Translate is not allowed since window is not system window");
        engine.Throw(CreateJsError(engine, static_cast<int32_t>(WmErrorCode::WM_ERROR_INVALID_CALLING)));
        return engine.CreateUndefined();
    }
    NativeObject* nativeObj = ConvertNativeValueTo<NativeObject>(info.argv[0]);
    if (nativeObj == nullptr) {
        WLOGFE("[NAPI]Failed to convert object to TranslateOptions");
        engine.Throw(CreateJsError(engine, static_cast<int32_t>(WmErrorCode::WM_ERROR_INVALID_PARAM)));
        return engine.CreateUndefined();
    }
    auto trans = windowToken_->GetTransform();
    if (!ParseTranslateOption(engine, nativeObj, trans)) {
        engine.Throw(CreateJsError(engine, static_cast<int32_t>(WmErrorCode::WM_ERROR_INVALID_PARAM)));
        return engine.CreateUndefined();
    }
    windowToken_->SetTransform(trans);
    WLOGFD("[NAPI]Window [%{public}u, %{public}s] Translate end," \
        "translateX = %{public}f, translateY = %{public}f, translateZ = %{public}f",
        windowToken_->GetWindowId(), windowToken_->GetWindowName().c_str(),
        trans.translateX_, trans.translateY_, trans.translateZ_);
    return engine.CreateUndefined();
}

void JsWindow::CreateTransitionController(NativeEngine& engine)
{
    if (windowToken_ == nullptr) {
        WLOGFE("[NAPI]windowToken_ is nullptr not match");
        return;
    }
    if (!WindowHelper::IsSystemWindow(windowToken_->GetType())) {
        WLOGFE("[NAPI]CreateTransitionController is not allowed since window is not system window");
        return;
    }
    NativeValue* objValue = engine.CreateObject();
    auto name = GetWindowName();
    std::shared_ptr<NativeReference> jsWindowObj = FindJsWindowObject(name);
    if (jsWindowObj == nullptr || jsWindowObj->Get() == nullptr) {
        return;
    }
    sptr<JsTransitionController> nativeController = new JsTransitionController(
        engine, jsWindowObj, windowToken_);
    NativeObject* object = ConvertNativeValueTo<NativeObject>(objValue);
    if (object == nullptr) {
        WLOGFE("[NAPI]Failed to convert to TransitionController Object");
        return;
    }
    object->SetNativePointer(new wptr<JsTransitionController>(nativeController),
        [](NativeEngine*, void* data, void*) {
            WLOGFE("Finalizer for wptr form native Transition Controller is called");
            delete static_cast<wptr<JsTransitionController>*>(data);
        }, nullptr);
    windowToken_->RegisterAnimationTransitionController(nativeController);
    jsTransControllerObj_.reset(engine.CreateReference(objValue, 1));
    nativeController->SetJsController(jsTransControllerObj_);
    WLOGFD("[NAPI]Window [%{public}u, %{public}s] CreateTransitionController end",
        windowToken_->GetWindowId(), windowToken_->GetWindowName().c_str());
}

NativeValue* JsWindow::OnGetTransitionController(NativeEngine& engine, NativeCallbackInfo& info)
{
    if (windowToken_ == nullptr) {
        WLOGFE("[NAPI]WindowToken_ is nullptr");
        engine.Throw(CreateJsError(engine, static_cast<int32_t>(WmErrorCode::WM_ERROR_STATE_ABNORMALLY)));
        return engine.CreateUndefined();
    }
    if (!WindowHelper::IsSystemWindow(windowToken_->GetType())) {
        WLOGFE("[NAPI]OnGetTransitionController is not allowed since window is not system window");
        engine.Throw(CreateJsError(engine, static_cast<int32_t>(WmErrorCode::WM_ERROR_INVALID_CALLING)));
        return engine.CreateUndefined();
    }
    if (jsTransControllerObj_ == nullptr || jsTransControllerObj_->Get() == nullptr) {
        CreateTransitionController(engine);
    }
    return jsTransControllerObj_ == nullptr ? nullptr : jsTransControllerObj_->Get();
}

NativeValue* JsWindow::OnSetCornerRadius(NativeEngine& engine, NativeCallbackInfo& info)
{
    if (!Permission::IsSystemCalling()) {
        WLOGFE("set corner radius permission denied!");
        engine.Throw(CreateJsError(engine, static_cast<int32_t>(WmErrorCode::WM_ERROR_INVALID_CALLING)));
        return engine.CreateUndefined();
    }
    if (info.argc < 1) {
        WLOGFE("[NAPI]Argc is invalid: %{public}zu", info.argc);
        engine.Throw(CreateJsError(engine, static_cast<int32_t>(WmErrorCode::WM_ERROR_INVALID_PARAM)));
        return engine.CreateUndefined();
    }
    if (windowToken_ == nullptr) {
        WLOGFE("[NAPI]WindowToken_ is nullptr");
        engine.Throw(CreateJsError(engine, static_cast<int32_t>(WmErrorCode::WM_ERROR_STATE_ABNORMALLY)));
        return engine.CreateUndefined();
    }
    if (!WindowHelper::IsSystemWindow(windowToken_->GetType())) {
        WLOGFE("[NAPI]SetCornerRadius is not allowed since window is not system window");
        engine.Throw(CreateJsError(engine, static_cast<int32_t>(WmErrorCode::WM_ERROR_INVALID_CALLING)));
        return engine.CreateUndefined();
    }
    NativeNumber* nativeVal = ConvertNativeValueTo<NativeNumber>(info.argv[0]);
    if (nativeVal == nullptr || MathHelper::LessNotEqual(static_cast<double>(*nativeVal), 0.0)) {
        WLOGFE("[NAPI]SetCornerRadius invalid radius");
        engine.Throw(CreateJsError(engine, static_cast<int32_t>(WmErrorCode::WM_ERROR_INVALID_PARAM)));
        return engine.CreateUndefined();
    }
    float radius = static_cast<double>(*nativeVal);
    WmErrorCode ret = WM_JS_TO_ERROR_CODE_MAP.at(windowToken_->SetCornerRadius(radius));
    if (ret != WmErrorCode::WM_OK) {
        WLOGFE("[NAPI]Window SetCornerRadius failed");
        engine.Throw(CreateJsError(engine, static_cast<int32_t>(ret)));
        return engine.CreateUndefined();
    }
    WLOGFD("[NAPI]Window [%{public}u, %{public}s] SetCornerRadius end, radius = %{public}f",
        windowToken_->GetWindowId(), windowToken_->GetWindowName().c_str(), radius);
    return engine.CreateUndefined();
}

NativeValue* JsWindow::OnSetShadow(NativeEngine& engine, NativeCallbackInfo& info)
{
    WmErrorCode ret = WmErrorCode::WM_OK;
    if (info.argc < 1) { // 1: min param num
        engine.Throw(CreateJsError(engine, static_cast<int32_t>(WmErrorCode::WM_ERROR_INVALID_PARAM)));
        return engine.CreateUndefined();
    }
    if (windowToken_ == nullptr) {
        engine.Throw(CreateJsError(engine, static_cast<int32_t>(WmErrorCode::WM_ERROR_STATE_ABNORMALLY)));
        return engine.CreateUndefined();
    }
    if (!WindowHelper::IsSystemWindow(windowToken_->GetType())) {
        engine.Throw(CreateJsError(engine, static_cast<int32_t>(WmErrorCode::WM_ERROR_INVALID_CALLING)));
        return engine.CreateUndefined();
    }

    { // parse the 1st param: radius
        NativeNumber* nativeVal = ConvertNativeValueTo<NativeNumber>(info.argv[0]);
        if (nativeVal == nullptr || MathHelper::LessNotEqual(static_cast<double>(*nativeVal), 0.0)) {
            engine.Throw(CreateJsError(engine, static_cast<int32_t>(WmErrorCode::WM_ERROR_INVALID_PARAM)));
            return engine.CreateUndefined();
        }
        ret = WM_JS_TO_ERROR_CODE_MAP.at(windowToken_->SetShadowRadius(static_cast<double>(*nativeVal)));
    }

    if ((ret == WmErrorCode::WM_OK) && (info.argc >= 2)) { // parse the 2nd param: color
        std::string color;
        if (!ConvertFromJsValue(engine, info.argv[1], color)) {
            engine.Throw(CreateJsError(engine, static_cast<int32_t>(WmErrorCode::WM_ERROR_INVALID_PARAM)));
            return engine.CreateUndefined();
        }
        ret = WM_JS_TO_ERROR_CODE_MAP.at(windowToken_->SetShadowColor(color));
    }

    if (ret != WmErrorCode::WM_OK) {
        engine.Throw(CreateJsError(engine, static_cast<int32_t>(ret)));
        return engine.CreateUndefined();
    }

    if (info.argc >= 3) { // parse the 3rd param: offsetX
        NativeNumber* nativeVal = ConvertNativeValueTo<NativeNumber>(info.argv[2]); // 2: the 3rd param
        if (nativeVal == nullptr) {
            engine.Throw(CreateJsError(engine, static_cast<int32_t>(WmErrorCode::WM_ERROR_INVALID_PARAM)));
            return engine.CreateUndefined();
        }
        windowToken_->SetShadowOffsetX(static_cast<double>(*nativeVal));
    }

    if (info.argc >= 4) { // parse the 4th param: offsetY
        NativeNumber* nativeVal = ConvertNativeValueTo<NativeNumber>(info.argv[3]); // 3: the 4th param
        if (nativeVal == nullptr) {
            engine.Throw(CreateJsError(engine, static_cast<int32_t>(WmErrorCode::WM_ERROR_INVALID_PARAM)));
            return engine.CreateUndefined();
        }
        windowToken_->SetShadowOffsetY(static_cast<double>(*nativeVal));
    }
    return engine.CreateUndefined();
}

NativeValue* JsWindow::OnSetBlur(NativeEngine& engine, NativeCallbackInfo& info)
{
    if (info.argc < 1) {
        WLOGFE("[NAPI]Argc is invalid: %{public}zu", info.argc);
        engine.Throw(CreateJsError(engine, static_cast<int32_t>(WmErrorCode::WM_ERROR_INVALID_PARAM)));
        return engine.CreateUndefined();
    }
    if (windowToken_ == nullptr) {
        WLOGFE("[NAPI]WindowToken_ is nullptr");
        engine.Throw(CreateJsError(engine, static_cast<int32_t>(WmErrorCode::WM_ERROR_STATE_ABNORMALLY)));
        return engine.CreateUndefined();
    }
    if (!WindowHelper::IsSystemWindow(windowToken_->GetType())) {
        WLOGFE("[NAPI]SetBlur is not allowed since window is not system window");
        engine.Throw(CreateJsError(engine, static_cast<int32_t>(WmErrorCode::WM_ERROR_INVALID_CALLING)));
        return engine.CreateUndefined();
    }
    NativeNumber* nativeVal = ConvertNativeValueTo<NativeNumber>(info.argv[0]);
    if (nativeVal == nullptr || MathHelper::LessNotEqual(static_cast<double>(*nativeVal), 0.0)) {
        WLOGFE("[NAPI]SetBlur invalid radius");
        engine.Throw(CreateJsError(engine, static_cast<int32_t>(WmErrorCode::WM_ERROR_INVALID_PARAM)));
        return engine.CreateUndefined();
    }
    float radius = static_cast<double>(*nativeVal);
    WmErrorCode ret = WM_JS_TO_ERROR_CODE_MAP.at(windowToken_->SetBlur(radius));
    if (ret != WmErrorCode::WM_OK) {
        WLOGFE("[NAPI]Window SetBlur failed");
        engine.Throw(CreateJsError(engine, static_cast<int32_t>(ret)));
        return engine.CreateUndefined();
    }
    WLOGFD("[NAPI]Window [%{public}u, %{public}s] SetBlur end, radius = %{public}f",
        windowToken_->GetWindowId(), windowToken_->GetWindowName().c_str(), radius);
    return engine.CreateUndefined();
}

NativeValue* JsWindow::OnSetBackdropBlur(NativeEngine& engine, NativeCallbackInfo& info)
{
    if (info.argc < 1) {
        WLOGFE("[NAPI]Argc is invalid: %{public}zu", info.argc);
        engine.Throw(CreateJsError(engine, static_cast<int32_t>(WmErrorCode::WM_ERROR_INVALID_PARAM)));
        return engine.CreateUndefined();
    }
    if (windowToken_ == nullptr) {
        WLOGFE("[NAPI]WindowToken_ is nullptr");
        engine.Throw(CreateJsError(engine, static_cast<int32_t>(WmErrorCode::WM_ERROR_STATE_ABNORMALLY)));
        return engine.CreateUndefined();
    }
    if (!WindowHelper::IsSystemWindow(windowToken_->GetType())) {
        WLOGFE("[NAPI]SetBackdropBlur is not allowed since window is not system window");
        engine.Throw(CreateJsError(engine, static_cast<int32_t>(WmErrorCode::WM_ERROR_INVALID_CALLING)));
        return engine.CreateUndefined();
    }
    NativeNumber* nativeVal = ConvertNativeValueTo<NativeNumber>(info.argv[0]);
    if (nativeVal == nullptr || MathHelper::LessNotEqual(static_cast<double>(*nativeVal), 0.0)) {
        WLOGFE("[NAPI]SetBackdropBlur invalid radius");
        engine.Throw(CreateJsError(engine, static_cast<int32_t>(WmErrorCode::WM_ERROR_INVALID_PARAM)));
        return engine.CreateUndefined();
    }
    float radius = static_cast<double>(*nativeVal);
    WmErrorCode ret = WM_JS_TO_ERROR_CODE_MAP.at(windowToken_->SetBackdropBlur(radius));
    if (ret != WmErrorCode::WM_OK) {
        WLOGFE("[NAPI]Window SetBackdropBlur failed");
        engine.Throw(CreateJsError(engine, static_cast<int32_t>(ret)));
        return engine.CreateUndefined();
    }
    WLOGFD("[NAPI]Window [%{public}u, %{public}s] SetBackdropBlur end, radius = %{public}f",
        windowToken_->GetWindowId(), windowToken_->GetWindowName().c_str(), radius);
    return engine.CreateUndefined();
}

NativeValue* JsWindow::OnSetBackdropBlurStyle(NativeEngine& engine, NativeCallbackInfo& info)
{
    if (info.argc < 1) {
        WLOGFE("[NAPI]Argc is invalid: %{public}zu", info.argc);
        engine.Throw(CreateJsError(engine, static_cast<int32_t>(WmErrorCode::WM_ERROR_INVALID_PARAM)));
        return engine.CreateUndefined();
    }
    if (windowToken_ == nullptr) {
        WLOGFE("[NAPI]WindowToken_ is nullptr");
        engine.Throw(CreateJsError(engine, static_cast<int32_t>(WmErrorCode::WM_ERROR_STATE_ABNORMALLY)));
        return engine.CreateUndefined();
    }
    if (!WindowHelper::IsSystemWindow(windowToken_->GetType())) {
        WLOGFE("[NAPI]SetBackdropBlurStyle is not allowed since window is not system window");
        engine.Throw(CreateJsError(engine, static_cast<int32_t>(WmErrorCode::WM_ERROR_INVALID_CALLING)));
        return engine.CreateUndefined();
    }

    NativeNumber* nativeMode = ConvertNativeValueTo<NativeNumber>(info.argv[0]);
    if (nativeMode == nullptr ||
        static_cast<uint32_t>(*nativeMode) > static_cast<uint32_t>(WindowBlurStyle::WINDOW_BLUR_THICK)) {
        WLOGFE("[NAPI]SetBackdropBlurStyle Invalid window blur style");
        engine.Throw(CreateJsError(engine, static_cast<int32_t>(WmErrorCode::WM_ERROR_INVALID_PARAM)));
        return engine.CreateUndefined();
    }
    WindowBlurStyle style = static_cast<WindowBlurStyle>(static_cast<uint32_t>(*nativeMode));
    WmErrorCode ret = WM_JS_TO_ERROR_CODE_MAP.at(windowToken_->SetBackdropBlurStyle(style));
    if (ret != WmErrorCode::WM_OK) {
        WLOGFE("[NAPI]Window SetBackdropBlurStyle failed");
        engine.Throw(CreateJsError(engine, static_cast<int32_t>(ret)));
        return engine.CreateUndefined();
    }

    WLOGFD("[NAPI]Window [%{public}u, %{public}s] SetBackdropBlurStyle end, style = %{public}u",
        windowToken_->GetWindowId(), windowToken_->GetWindowName().c_str(), style);
    return engine.CreateUndefined();
}

std::shared_ptr<NativeReference> FindJsWindowObject(std::string windowName)
{
    WLOGFD("[NAPI]Try to find window %{public}s in g_jsWindowMap", windowName.c_str());
    std::lock_guard<std::recursive_mutex> lock(g_mutex);
    if (g_jsWindowMap.find(windowName) == g_jsWindowMap.end()) {
        WLOGFD("[NAPI]Can not find window %{public}s in g_jsWindowMap", windowName.c_str());
        return nullptr;
    }
    return g_jsWindowMap[windowName];
}

NativeValue* CreateJsWindowObject(NativeEngine& engine, sptr<Window>& window)
{
    std::string windowName = window->GetWindowName();
    // avoid repeatedly create js window when getWindow
    std::shared_ptr<NativeReference> jsWindowObj = FindJsWindowObject(windowName);
    if (jsWindowObj != nullptr && jsWindowObj->Get() != nullptr) {
        WLOGFD("[NAPI]FindJsWindowObject %{public}s", windowName.c_str());
        return jsWindowObj->Get();
    }
    NativeValue* objValue = engine.CreateObject();
    NativeObject* object = ConvertNativeValueTo<NativeObject>(objValue);

    WLOGFD("[NAPI]CreateJsWindow %{public}s", windowName.c_str());
    std::unique_ptr<JsWindow> jsWindow = std::make_unique<JsWindow>(window);
    object->SetNativePointer(jsWindow.release(), JsWindow::Finalizer, nullptr);

    BindFunctions(engine, object, "JsWindow");

    std::shared_ptr<NativeReference> jsWindowRef;
    jsWindowRef.reset(engine.CreateReference(objValue, 1));
    std::lock_guard<std::recursive_mutex> lock(g_mutex);
    g_jsWindowMap[windowName] = jsWindowRef;
    return objValue;
}

void BindFunctions(NativeEngine& engine, NativeObject* object, const char *moduleName)
{
    BindNativeFunction(engine, *object, "show", moduleName, JsWindow::Show);
    BindNativeFunction(engine, *object, "showWindow", moduleName, JsWindow::ShowWindow);
    BindNativeFunction(engine, *object, "showWithAnimation", moduleName, JsWindow::ShowWithAnimation);
    BindNativeFunction(engine, *object, "destroy", moduleName, JsWindow::Destroy);
    BindNativeFunction(engine, *object, "destroyWindow", moduleName, JsWindow::DestroyWindow);
    BindNativeFunction(engine, *object, "hide", moduleName, JsWindow::Hide);
    BindNativeFunction(engine, *object, "hideWithAnimation", moduleName, JsWindow::HideWithAnimation);
    BindNativeFunction(engine, *object, "moveTo", moduleName, JsWindow::MoveTo);
    BindNativeFunction(engine, *object, "moveWindowTo", moduleName, JsWindow::MoveWindowTo);
    BindNativeFunction(engine, *object, "resetSize", moduleName, JsWindow::Resize);
    BindNativeFunction(engine, *object, "resize", moduleName, JsWindow::ResizeWindow);
    BindNativeFunction(engine, *object, "setWindowType", moduleName, JsWindow::SetWindowType);
    BindNativeFunction(engine, *object, "setWindowMode", moduleName, JsWindow::SetWindowMode);
    BindNativeFunction(engine, *object, "getProperties", moduleName, JsWindow::GetProperties);
    BindNativeFunction(engine, *object, "getWindowProperties", moduleName, JsWindow::GetWindowPropertiesSync);
    BindNativeFunction(engine, *object, "on", moduleName, JsWindow::RegisterWindowCallback);
    BindNativeFunction(engine, *object, "off", moduleName, JsWindow::UnregisterWindowCallback);
    BindNativeFunction(engine, *object, "bindDialogTarget", moduleName, JsWindow::BindDialogTarget);
    BindNativeFunction(engine, *object, "loadContent", moduleName, JsWindow::LoadContent);
    BindNativeFunction(engine, *object, "setUIContent", moduleName, JsWindow::SetUIContent);
    BindNativeFunction(engine, *object, "setFullScreen", moduleName, JsWindow::SetFullScreen);
    BindNativeFunction(engine, *object, "setLayoutFullScreen", moduleName, JsWindow::SetLayoutFullScreen);
    BindNativeFunction(engine, *object, "setWindowLayoutFullScreen", moduleName, JsWindow::SetWindowLayoutFullScreen);
    BindNativeFunction(engine, *object, "setSystemBarEnable", moduleName, JsWindow::SetSystemBarEnable);
    BindNativeFunction(engine, *object, "setWindowSystemBarEnable", moduleName, JsWindow::SetWindowSystemBarEnable);
    BindNativeFunction(engine, *object, "setSystemBarProperties", moduleName, JsWindow::SetSystemBarProperties);
    BindNativeFunction(engine, *object, "setWindowSystemBarProperties",
        moduleName, JsWindow::SetWindowSystemBarProperties);
    BindNativeFunction(engine, *object, "getAvoidArea", moduleName, JsWindow::GetAvoidArea);
    BindNativeFunction(engine, *object, "getWindowAvoidArea", moduleName, JsWindow::GetWindowAvoidAreaSync);
    BindNativeFunction(engine, *object, "isShowing", moduleName, JsWindow::IsShowing);
    BindNativeFunction(engine, *object, "isWindowShowing", moduleName, JsWindow::IsWindowShowingSync);
    BindNativeFunction(engine, *object, "isSupportWideGamut", moduleName, JsWindow::IsSupportWideGamut);
    BindNativeFunction(engine, *object, "isWindowSupportWideGamut", moduleName, JsWindow::IsWindowSupportWideGamut);
    BindNativeFunction(engine, *object, "setColorSpace", moduleName, JsWindow::SetColorSpace);
    BindNativeFunction(engine, *object, "setWindowColorSpace", moduleName, JsWindow::SetWindowColorSpace);
    BindNativeFunction(engine, *object, "getColorSpace", moduleName, JsWindow::GetColorSpace);
    BindNativeFunction(engine, *object, "getWindowColorSpace", moduleName, JsWindow::GetWindowColorSpaceSync);
    BindNativeFunction(engine, *object, "setBackgroundColor", moduleName, JsWindow::SetBackgroundColor);
    BindNativeFunction(engine, *object, "setWindowBackgroundColor", moduleName, JsWindow::SetWindowBackgroundColorSync);
    BindNativeFunction(engine, *object, "setBrightness", moduleName, JsWindow::SetBrightness);
    BindNativeFunction(engine, *object, "setWindowBrightness", moduleName, JsWindow::SetWindowBrightness);
    BindNativeFunction(engine, *object, "setDimBehind", moduleName, JsWindow::SetDimBehind);
    BindNativeFunction(engine, *object, "setFocusable", moduleName, JsWindow::SetFocusable);
    BindNativeFunction(engine, *object, "setWindowFocusable", moduleName, JsWindow::SetWindowFocusable);
    BindNativeFunction(engine, *object, "setKeepScreenOn", moduleName, JsWindow::SetKeepScreenOn);
    BindNativeFunction(engine, *object, "setWindowKeepScreenOn", moduleName, JsWindow::SetWindowKeepScreenOn);
    BindNativeFunction(engine, *object, "setWakeUpScreen", moduleName, JsWindow::SetWakeUpScreen);
    BindNativeFunction(engine, *object, "setOutsideTouchable", moduleName, JsWindow::SetOutsideTouchable);
    BindNativeFunction(engine, *object, "setPrivacyMode", moduleName, JsWindow::SetPrivacyMode);
    BindNativeFunction(engine, *object, "setWindowPrivacyMode", moduleName, JsWindow::SetWindowPrivacyMode);
    BindNativeFunction(engine, *object, "setTouchable", moduleName, JsWindow::SetTouchable);
    BindNativeFunction(engine, *object, "setWindowTouchable", moduleName, JsWindow::SetWindowTouchable);
    BindNativeFunction(engine, *object, "setTransparent", moduleName, JsWindow::SetTransparent);
    BindNativeFunction(engine, *object, "setCallingWindow", moduleName, JsWindow::SetCallingWindow);
    BindNativeFunction(engine, *object, "setSnapshotSkip", moduleName, JsWindow::SetSnapshotSkip);
    BindNativeFunction(engine, *object, "disableWindowDecor", moduleName, JsWindow::DisableWindowDecor);
    BindNativeFunction(engine, *object, "dump", moduleName, JsWindow::Dump);
    BindNativeFunction(engine, *object, "setForbidSplitMove", moduleName, JsWindow::SetForbidSplitMove);
    BindNativeFunction(engine, *object, "setPreferredOrientation", moduleName, JsWindow::SetPreferredOrientation);
    BindNativeFunction(engine, *object, "opacity", moduleName, JsWindow::Opacity);
    BindNativeFunction(engine, *object, "scale", moduleName, JsWindow::Scale);
    BindNativeFunction(engine, *object, "rotate", moduleName, JsWindow::Rotate);
    BindNativeFunction(engine, *object, "translate", moduleName, JsWindow::Translate);
    BindNativeFunction(engine, *object, "getTransitionController", moduleName, JsWindow::GetTransitionController);
    BindNativeFunction(engine, *object, "snapshot", moduleName, JsWindow::Snapshot);
    BindNativeFunction(engine, *object, "setCornerRadius", moduleName, JsWindow::SetCornerRadius);
    BindNativeFunction(engine, *object, "setShadow", moduleName, JsWindow::SetShadow);
    BindNativeFunction(engine, *object, "setBlur", moduleName, JsWindow::SetBlur);
    BindNativeFunction(engine, *object, "setBackdropBlur", moduleName, JsWindow::SetBackdropBlur);
    BindNativeFunction(engine, *object, "setBackdropBlurStyle", moduleName, JsWindow::SetBackdropBlurStyle);
}
}  // namespace Rosen
}  // namespace OHOS
