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
#ifndef OHOS_JS_WINDOW_REGISTER_MANAGER_H
#define OHOS_JS_WINDOW_REGISTER_MANAGER_H
#include <map>
#include <mutex>
#include "js_window_listener.h"
#include "native_engine/native_engine.h"
#include "native_engine/native_reference.h"
#include "native_engine/native_value.h"
#include "refbase.h"
#include "window.h"
namespace OHOS {
namespace Rosen {
enum class CaseType {
    CASE_WINDOW_MANAGER = 0,
    CASE_WINDOW,
    CASE_STAGE
};
class JsWindowRegisterManager {
public:
    JsWindowRegisterManager();
    ~JsWindowRegisterManager();
    void RegisterListener(sptr<Window> window, std::string type,
        CaseType caseType, NativeEngine& engine, NativeValue* value);
    void UnregisterListener(sptr<Window> window, std::string type,
        CaseType caseType, NativeValue* value);
private:
    bool IsCallbackRegistered(std::string type, NativeValue* jsListenerObject);
    bool ProcessWindowChangeRegister(sptr<JsWindowListener> listener, sptr<Window> window, bool isRegister);
    bool ProcessSystemAvoidAreaChangeRegister(sptr<JsWindowListener> listener, sptr<Window> window, bool isRegister);
    bool ProcessAvoidAreaChangeRegister(sptr<JsWindowListener> listener, sptr<Window> window, bool isRegister);
    bool ProcessLifeCycleEventRegister(sptr<JsWindowListener> listener, sptr<Window> window, bool isRegister);
    bool ProcessOccupiedAreaChangeRegister(sptr<JsWindowListener> listener, sptr<Window> window, bool isRegister);
    bool ProcessSystemBarChangeRegister(sptr<JsWindowListener> listener, sptr<Window> window, bool isRegister);
    bool ProcessTouchOutsideRegister(sptr<JsWindowListener> listener, sptr<Window> window, bool isRegister);
    bool ProcessScreenshotRegister(sptr<JsWindowListener> listener, sptr<Window> window, bool isRegister);
    bool ProcessDialogTargetTouchRegister(sptr<JsWindowListener> listener, sptr<Window> window, bool isRegister);
    bool ProcessDialogDeathRecipientRegister(sptr<JsWindowListener> listener, sptr<Window> window, bool isRegister);
    using Func_t = bool(JsWindowRegisterManager::*)(sptr<JsWindowListener>, sptr<Window> window, bool);
    std::map<std::string, std::map<std::shared_ptr<NativeReference>, sptr<JsWindowListener>>> jsCbMap_;
    std::mutex mtx_;
    std::map<CaseType, std::map<std::string, Func_t>> listenerProcess_;
};
} // namespace Rosen
} // namespace OHOS

#endif