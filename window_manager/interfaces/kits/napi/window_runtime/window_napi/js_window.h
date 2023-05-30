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

#ifndef OHOS_JS_WINDOW_H
#define OHOS_JS_WINDOW_H

#include "js_runtime_utils.h"

#ifndef WINDOW_PREVIEW
#include "js_window_register_manager.h"
#else
#include "mock/js_window_register_manager.h"
#endif

#include "js_window_utils.h"
#include "native_engine/native_engine.h"
#include "native_engine/native_value.h"
#include "window.h"

namespace OHOS {
namespace Rosen {
NativeValue* CreateJsWindowObject(NativeEngine& engine, sptr<Window>& window);
std::shared_ptr<NativeReference> FindJsWindowObject(std::string windowName);
void BindFunctions(NativeEngine& engine, NativeObject* object, const char *moduleName);
class JsWindow final {
public:
    explicit JsWindow(const sptr<Window>& window);
    ~JsWindow();
    static void Finalizer(NativeEngine* engine, void* data, void* hint);
    static NativeValue* Show(NativeEngine* engine, NativeCallbackInfo* info);
    static NativeValue* ShowWindow(NativeEngine* engine, NativeCallbackInfo* info);
    static NativeValue* ShowWithAnimation(NativeEngine* engine, NativeCallbackInfo* info);
    static NativeValue* Destroy(NativeEngine* engine, NativeCallbackInfo* info);
    static NativeValue* DestroyWindow(NativeEngine* engine, NativeCallbackInfo* info);
    static NativeValue* Hide(NativeEngine* engine, NativeCallbackInfo* info);
    static NativeValue* HideWithAnimation(NativeEngine* engine, NativeCallbackInfo* info);
    static NativeValue* MoveTo(NativeEngine* engine, NativeCallbackInfo* info);
    static NativeValue* MoveWindowTo(NativeEngine* engine, NativeCallbackInfo* info);
    static NativeValue* Resize(NativeEngine* engine, NativeCallbackInfo* info);
    static NativeValue* ResizeWindow(NativeEngine* engine, NativeCallbackInfo* info);
    static NativeValue* SetWindowType(NativeEngine* engine, NativeCallbackInfo* info);
    static NativeValue* SetWindowMode(NativeEngine* engine, NativeCallbackInfo* info);
    static NativeValue* GetProperties(NativeEngine* engine, NativeCallbackInfo* info);
    static NativeValue* GetWindowPropertiesSync(NativeEngine* engine, NativeCallbackInfo* info);
    static NativeValue* RegisterWindowCallback(NativeEngine* engine, NativeCallbackInfo* info);
    static NativeValue* UnregisterWindowCallback(NativeEngine* engine, NativeCallbackInfo* info);
    static NativeValue* BindDialogTarget(NativeEngine* engine, NativeCallbackInfo* info);
    static NativeValue* LoadContent(NativeEngine* engine, NativeCallbackInfo* info);
    static NativeValue* SetUIContent(NativeEngine* engine, NativeCallbackInfo* info);
    static NativeValue* SetFullScreen(NativeEngine* engine, NativeCallbackInfo* info);
    static NativeValue* SetLayoutFullScreen(NativeEngine* engine, NativeCallbackInfo* info);
    static NativeValue* SetWindowLayoutFullScreen(NativeEngine* engine, NativeCallbackInfo* info);
    static NativeValue* SetSystemBarEnable(NativeEngine* engine, NativeCallbackInfo* info);
    static NativeValue* SetWindowSystemBarEnable(NativeEngine* engine, NativeCallbackInfo* info);
    static NativeValue* SetSystemBarProperties(NativeEngine* engine, NativeCallbackInfo* info);
    static NativeValue* SetWindowSystemBarProperties(NativeEngine* engine, NativeCallbackInfo* info);
    static NativeValue* GetAvoidArea(NativeEngine* engine, NativeCallbackInfo* info);
    static NativeValue* GetWindowAvoidAreaSync(NativeEngine* engine, NativeCallbackInfo* info);
    static NativeValue* IsShowing(NativeEngine* engine, NativeCallbackInfo* info);
    static NativeValue* IsWindowShowingSync(NativeEngine* engine, NativeCallbackInfo* info);
    static NativeValue* SetBackgroundColor(NativeEngine* engine, NativeCallbackInfo* info);
    static NativeValue* SetWindowBackgroundColorSync(NativeEngine* engine, NativeCallbackInfo* info);
    static NativeValue* SetBrightness(NativeEngine* engine, NativeCallbackInfo* info);
    static NativeValue* SetWindowBrightness(NativeEngine* engine, NativeCallbackInfo* info);
    static NativeValue* SetDimBehind(NativeEngine* engine, NativeCallbackInfo* info);
    static NativeValue* SetFocusable(NativeEngine* engine, NativeCallbackInfo* info);
    static NativeValue* SetWindowFocusable(NativeEngine* engine, NativeCallbackInfo* info);
    static NativeValue* SetKeepScreenOn(NativeEngine* engine, NativeCallbackInfo* info);
    static NativeValue* SetWindowKeepScreenOn(NativeEngine* engine, NativeCallbackInfo* info);
    static NativeValue* SetWakeUpScreen(NativeEngine* engine, NativeCallbackInfo* info);
    static NativeValue* SetOutsideTouchable(NativeEngine* engine, NativeCallbackInfo* info);
    static NativeValue* SetPrivacyMode(NativeEngine* engine, NativeCallbackInfo* info);
    static NativeValue* SetWindowPrivacyMode(NativeEngine* engine, NativeCallbackInfo* info);
    static NativeValue* SetTouchable(NativeEngine* engine, NativeCallbackInfo* info);
    static NativeValue* SetWindowTouchable(NativeEngine* engine, NativeCallbackInfo* info);
    static NativeValue* SetTransparent(NativeEngine* engine, NativeCallbackInfo* info);
    static NativeValue* SetCallingWindow(NativeEngine* engine, NativeCallbackInfo* info);
    static NativeValue* SetPreferredOrientation(NativeEngine* engine, NativeCallbackInfo* info);
    static NativeValue* DisableWindowDecor(NativeEngine* engine, NativeCallbackInfo* info);
    static NativeValue* SetSnapshotSkip(NativeEngine* engine, NativeCallbackInfo* info);
    // colorspace, gamut
    static NativeValue* IsSupportWideGamut(NativeEngine* engine, NativeCallbackInfo* info);
    static NativeValue* IsWindowSupportWideGamut(NativeEngine* engine, NativeCallbackInfo* info);
    static NativeValue* SetColorSpace(NativeEngine* engine, NativeCallbackInfo* info);
    static NativeValue* SetWindowColorSpace(NativeEngine* engine, NativeCallbackInfo* info);
    static NativeValue* GetColorSpace(NativeEngine* engine, NativeCallbackInfo* info);
    static NativeValue* GetWindowColorSpaceSync(NativeEngine* engine, NativeCallbackInfo* info);
    static NativeValue* Dump(NativeEngine* engine, NativeCallbackInfo* info);
    static NativeValue* SetForbidSplitMove(NativeEngine* engine, NativeCallbackInfo* info);
    static NativeValue* Snapshot(NativeEngine* engine, NativeCallbackInfo* info);

    // animation config
    static NativeValue* Opacity(NativeEngine* engine, NativeCallbackInfo* info);
    static NativeValue* Scale(NativeEngine* engine, NativeCallbackInfo* info);
    static NativeValue* Rotate(NativeEngine* engine, NativeCallbackInfo* info);
    static NativeValue* Translate(NativeEngine* engine, NativeCallbackInfo* info);
    static NativeValue* GetTransitionController(NativeEngine* engine, NativeCallbackInfo* info);

    // window effect
    static NativeValue* SetCornerRadius(NativeEngine* engine, NativeCallbackInfo* info);
    static NativeValue* SetShadow(NativeEngine* engine, NativeCallbackInfo* info);
    static NativeValue* SetBlur(NativeEngine* engine, NativeCallbackInfo* info);
    static NativeValue* SetBackdropBlur(NativeEngine* engine, NativeCallbackInfo* info);
    static NativeValue* SetBackdropBlurStyle(NativeEngine* engine, NativeCallbackInfo* info);
private:
    std::string GetWindowName();
    static bool ParseScaleOption(NativeEngine& engine, NativeObject* jsObject, Transform& trans);
    static bool ParseRotateOption(NativeEngine& engine, NativeObject* jsObject, Transform& trans);
    static bool ParseTranslateOption(NativeEngine& engine, NativeObject* jsObject, Transform& trans);
    NativeValue* LoadContentScheduleOld(NativeEngine& engine, NativeCallbackInfo& info);
    NativeValue* LoadContentScheduleNew(NativeEngine& engine, NativeCallbackInfo& info);
    NativeValue* OnShow(NativeEngine& engine, NativeCallbackInfo& info);
    NativeValue* OnShowWindow(NativeEngine& engine, NativeCallbackInfo& info);
    NativeValue* OnShowWithAnimation(NativeEngine& engine, NativeCallbackInfo& info);
    NativeValue* OnDestroy(NativeEngine& engine, NativeCallbackInfo& info);
    NativeValue* OnDestroyWindow(NativeEngine& engine, NativeCallbackInfo& info);
    NativeValue* OnHide(NativeEngine& engine, NativeCallbackInfo& info);
    NativeValue* OnHideWithAnimation(NativeEngine& engine, NativeCallbackInfo& info);
    NativeValue* OnMoveTo(NativeEngine& engine, NativeCallbackInfo& info);
    NativeValue* OnMoveWindowTo(NativeEngine& engine, NativeCallbackInfo& info);
    NativeValue* OnResize(NativeEngine& engine, NativeCallbackInfo& info);
    NativeValue* OnResizeWindow(NativeEngine& engine, NativeCallbackInfo& info);
    NativeValue* OnSetWindowType(NativeEngine& engine, NativeCallbackInfo& info);
    NativeValue* OnSetWindowMode(NativeEngine& engine, NativeCallbackInfo& info);
    NativeValue* OnGetProperties(NativeEngine& engine, NativeCallbackInfo& info);
    NativeValue* OnGetWindowPropertiesSync(NativeEngine& engine, NativeCallbackInfo& info);
    NativeValue* OnRegisterWindowCallback(NativeEngine& engine, NativeCallbackInfo& info);
    NativeValue* OnUnregisterWindowCallback(NativeEngine& engine, NativeCallbackInfo& info);
    NativeValue* OnBindDialogTarget(NativeEngine& engine, NativeCallbackInfo& info);
    NativeValue* OnSetFullScreen(NativeEngine& engine, NativeCallbackInfo& info);
    NativeValue* OnSetLayoutFullScreen(NativeEngine& engine, NativeCallbackInfo& info);
    NativeValue* OnSetWindowLayoutFullScreen(NativeEngine& engine, NativeCallbackInfo& info);
    NativeValue* OnSetSystemBarEnable(NativeEngine& engine, NativeCallbackInfo& info);
    NativeValue* OnSetWindowSystemBarEnable(NativeEngine& engine, NativeCallbackInfo& info);
    NativeValue* OnSetSystemBarProperties(NativeEngine& engine, NativeCallbackInfo& info);
    NativeValue* OnSetWindowSystemBarProperties(NativeEngine& engine, NativeCallbackInfo& info);
    NativeValue* OnLoadContent(NativeEngine& engine, NativeCallbackInfo& info);
    NativeValue* OnSetUIContent(NativeEngine& engine, NativeCallbackInfo& info);
    NativeValue* OnGetAvoidArea(NativeEngine& engine, NativeCallbackInfo& info);
    NativeValue* OnGetWindowAvoidAreaSync(NativeEngine& engine, NativeCallbackInfo& info);
    NativeValue* OnIsShowing(NativeEngine& engine, NativeCallbackInfo& info);
    NativeValue* OnIsWindowShowingSync(NativeEngine& engine, NativeCallbackInfo& info);
    NativeValue* OnSetPreferredOrientation(NativeEngine& engine, NativeCallbackInfo& info);

    // colorspace, gamut
    NativeValue* OnIsSupportWideGamut(NativeEngine& engine, NativeCallbackInfo& info);
    NativeValue* OnIsWindowSupportWideGamut(NativeEngine& engine, NativeCallbackInfo& info);
    NativeValue* OnSetColorSpace(NativeEngine& engine, NativeCallbackInfo& info);
    NativeValue* OnSetWindowColorSpace(NativeEngine& engine, NativeCallbackInfo& info);
    NativeValue* OnGetColorSpace(NativeEngine& engine, NativeCallbackInfo& info);
    NativeValue* OnGetWindowColorSpaceSync(NativeEngine& engine, NativeCallbackInfo& info);
    NativeValue* OnSetBackgroundColor(NativeEngine& engine, NativeCallbackInfo& info);
    NativeValue* OnSetWindowBackgroundColorSync(NativeEngine& engine, NativeCallbackInfo& info);
    NativeValue* OnSetBrightness(NativeEngine& engine, NativeCallbackInfo& info);
    NativeValue* OnSetWindowBrightness(NativeEngine& engine, NativeCallbackInfo& info);
    NativeValue* OnSetDimBehind(NativeEngine& engine, NativeCallbackInfo& info);
    NativeValue* OnSetFocusable(NativeEngine& engine, NativeCallbackInfo& info);
    NativeValue* OnSetWindowFocusable(NativeEngine& engine, NativeCallbackInfo& info);
    NativeValue* OnSetKeepScreenOn(NativeEngine& engine, NativeCallbackInfo& info);
    NativeValue* OnSetWindowKeepScreenOn(NativeEngine& engine, NativeCallbackInfo& info);
    NativeValue* OnSetWakeUpScreen(NativeEngine& engine, NativeCallbackInfo& info);
    NativeValue* OnSetOutsideTouchable(NativeEngine& engine, NativeCallbackInfo& info);
    NativeValue* OnSetPrivacyMode(NativeEngine& engine, NativeCallbackInfo& info);
    NativeValue* OnSetWindowPrivacyMode(NativeEngine& engine, NativeCallbackInfo& info);
    NativeValue* OnSetTouchable(NativeEngine& engine, NativeCallbackInfo& info);
    NativeValue* OnSetWindowTouchable(NativeEngine& engine, NativeCallbackInfo& info);
    NativeValue* OnSetTransparent(NativeEngine& engine, NativeCallbackInfo& info);
    NativeValue* OnSetCallingWindow(NativeEngine& engine, NativeCallbackInfo& info);
    NativeValue* OnDisableWindowDecor(NativeEngine& engine, NativeCallbackInfo& info);
    NativeValue* OnDump(NativeEngine& engine, NativeCallbackInfo& info);
    NativeValue* OnSetForbidSplitMove(NativeEngine& engine, NativeCallbackInfo& info);
    NativeValue* OnSnapshot(NativeEngine& engine, NativeCallbackInfo& info);
    NativeValue* OnSetSnapshotSkip(NativeEngine& engine, NativeCallbackInfo& info);

    // animation Config
    NativeValue* OnOpacity(NativeEngine& engine, NativeCallbackInfo& info);
    NativeValue* OnScale(NativeEngine& engine, NativeCallbackInfo& info);
    NativeValue* OnRotate(NativeEngine& engine, NativeCallbackInfo& info);
    NativeValue* OnTranslate(NativeEngine& engine, NativeCallbackInfo& info);
    NativeValue* OnGetTransitionController(NativeEngine& engine, NativeCallbackInfo& info);
    void CreateTransitionController(NativeEngine& engine);

    // window effect
    NativeValue* OnSetCornerRadius(NativeEngine& engine, NativeCallbackInfo& info);
    NativeValue* OnSetShadow(NativeEngine& engine, NativeCallbackInfo& info);
    NativeValue* OnSetBlur(NativeEngine& engine, NativeCallbackInfo& info);
    NativeValue* OnSetBackdropBlur(NativeEngine& engine, NativeCallbackInfo& info);
    NativeValue* OnSetBackdropBlurStyle(NativeEngine& engine, NativeCallbackInfo& info);

    sptr<Window> windowToken_ = nullptr;
    std::unique_ptr<JsWindowRegisterManager> registerManager_ = nullptr;
    std::shared_ptr<NativeReference> jsTransControllerObj_ = nullptr;
};
}  // namespace Rosen
}  // namespace OHOS
#endif