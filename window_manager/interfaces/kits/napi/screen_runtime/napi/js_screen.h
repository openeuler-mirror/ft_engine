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

#ifndef OHOS_JS_SCREEN_H
#define OHOS_JS_SCREEN_H
#include <js_runtime_utils.h>
#include <native_engine/native_engine.h>
#include <native_engine/native_value.h>
#include <refbase.h>

#include "screen.h"

namespace OHOS {
namespace Rosen {
NativeValue* CreateJsScreenObject(NativeEngine& engine, sptr<Screen>& screen);
NativeValue* CreateJsScreenModeArrayObject(NativeEngine& engine, std::vector<sptr<SupportedScreenModes>> screenModes);
NativeValue* CreateJsScreenModeObject(NativeEngine &engine, const sptr<SupportedScreenModes>& mode);
class JsScreen final {
public:
    explicit JsScreen(const sptr<Screen>& screen);
    ~JsScreen();
    static void Finalizer(NativeEngine* engine, void* data, void* hint);
    static NativeValue* SetScreenActiveMode(NativeEngine* engine, NativeCallbackInfo* info);
    static NativeValue* SetOrientation(NativeEngine* engine, NativeCallbackInfo* info);
    static NativeValue* SetDensityDpi(NativeEngine* engine, NativeCallbackInfo* info);

private:
    sptr<Screen> screen_ = nullptr;
    NativeValue* OnSetOrientation(NativeEngine& engine, NativeCallbackInfo& info);
    NativeValue* OnSetScreenActiveMode(NativeEngine& engine, NativeCallbackInfo& info);
    NativeValue* OnSetDensityDpi(NativeEngine& engine, NativeCallbackInfo& info);
};
}  // namespace Rosen
}  // namespace OHOS
#endif