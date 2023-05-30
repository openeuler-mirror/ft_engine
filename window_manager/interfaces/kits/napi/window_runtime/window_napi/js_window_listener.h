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

#ifndef OHOS_JS_WINDOW_LISTENER_H
#define OHOS_JS_WINDOW_LISTENER_H

#include <map>
#include <mutex>

#include "class_var_definition.h"
#include "js_window_utils.h"
#include "native_engine/native_engine.h"
#include "native_engine/native_value.h"
#include "refbase.h"
#include "window.h"
#include "window_manager.h"
#include "wm_common.h"

namespace OHOS {
namespace Rosen {
const std::string WINDOW_SIZE_CHANGE_CB = "windowSizeChange";
const std::string SYSTEM_BAR_TINT_CHANGE_CB = "systemBarTintChange";
const std::string SYSTEM_AVOID_AREA_CHANGE_CB = "systemAvoidAreaChange";
const std::string AVOID_AREA_CHANGE_CB = "avoidAreaChange";
const std::string LIFECYCLE_EVENT_CB = "lifeCycleEvent";
const std::string WINDOW_STAGE_EVENT_CB = "windowStageEvent";
const std::string KEYBOARD_HEIGHT_CHANGE_CB = "keyboardHeightChange";
const std::string TOUCH_OUTSIDE_CB = "touchOutside";
const std::string SCREENSHOT_EVENT_CB = "screenshot";
const std::string DIALOG_TARGET_TOUCH_CB = "dialogTargetTouch";
const std::string DIALOG_DEATH_RECIPIENT_CB = "dialogDeathRecipient";

class JsWindowListener : public IWindowChangeListener,
                         public ISystemBarChangedListener,
                         public IAvoidAreaChangedListener,
                         public IWindowLifeCycle,
                         public IOccupiedAreaChangeListener,
                         public ITouchOutsideListener,
                         public IScreenshotListener,
                         public IDialogTargetTouchListener,
                         public IDialogDeathRecipientListener {
public:
    JsWindowListener(NativeEngine* engine, std::shared_ptr<NativeReference> callback)
        : engine_(engine), jsCallBack_(callback), weakRef_(wptr<JsWindowListener> (this)) {}
    ~JsWindowListener();
    void OnSystemBarPropertyChange(DisplayId displayId, const SystemBarRegionTints& tints) override;
    void OnSizeChange(Rect rect, WindowSizeChangeReason reason) override;
    void OnModeChange(WindowMode mode) override;
    void OnAvoidAreaChanged(const AvoidArea avoidArea, AvoidAreaType type) override;
    void AfterForeground() override;
    void AfterBackground() override;
    void AfterFocused() override;
    void AfterUnfocused() override;
    void OnSizeChange(const sptr<OccupiedAreaChangeInfo>& info) override;
    void OnTouchOutside() const override;
    void OnScreenshot() override;
    void OnDialogTargetTouch() const override;
    void OnDialogDeathRecipient() const override;
    void CallJsMethod(const char* methodName, NativeValue* const* argv = nullptr, size_t argc = 0);
private:
    void LifeCycleCallBack(LifeCycleEventType eventType);
    NativeEngine* engine_ = nullptr;
    std::shared_ptr<NativeReference> jsCallBack_ = nullptr;
    wptr<JsWindowListener> weakRef_  = nullptr;
    DEFINE_VAR_DEFAULT_FUNC_SET(bool, IsDeprecatedInterface, isDeprecatedInterface, false)
};
}  // namespace Rosen
}  // namespace OHOS
#endif /* OHOS_JS_WINDOW_LISTENER_H */
