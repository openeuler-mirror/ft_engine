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

#ifndef OHOS_JS_WINDOW_STAGE_H
#define OHOS_JS_WINDOW_STAGE_H
#include <map>
#include <refbase.h>
#include "native_engine/native_engine.h"
#include "native_engine/native_reference.h"
#include "native_engine/native_value.h"
#include "window_scene.h"
namespace OHOS {
namespace Rosen {
NativeValue* CreateJsWindowStage(NativeEngine& engine, std::shared_ptr<Rosen::WindowScene> windowScene);
class JsWindowStage {
public:
    explicit JsWindowStage(const std::shared_ptr<Rosen::WindowScene>& windowScene);
    ~JsWindowStage();
    static void Finalizer(NativeEngine* engine, void* data, void* hint);
    static NativeValue* SetUIContent(NativeEngine* engine, NativeCallbackInfo* info);
    static NativeValue* GetMainWindow(NativeEngine* engine, NativeCallbackInfo* info);
    static NativeValue* GetMainWindowSync(NativeEngine* engine, NativeCallbackInfo* info);
    static NativeValue* On(NativeEngine* engine, NativeCallbackInfo* info);
    static NativeValue* Off(NativeEngine* engine, NativeCallbackInfo* info);
    static NativeValue* LoadContent(NativeEngine* engine, NativeCallbackInfo* info);
    static NativeValue* GetWindowMode(NativeEngine* engine, NativeCallbackInfo* info);
    static NativeValue* CreateSubWindow(NativeEngine* engine, NativeCallbackInfo* info);
    static NativeValue* GetSubWindow(NativeEngine* engine, NativeCallbackInfo* info);
    static NativeValue* SetShowOnLockScreen(NativeEngine* engine, NativeCallbackInfo* info);
    static NativeValue* DisableWindowDecor(NativeEngine* engine, NativeCallbackInfo* info);

private:
    NativeValue* OnSetUIContent(NativeEngine& engine, NativeCallbackInfo& info);
    NativeValue* OnGetMainWindow(NativeEngine& engine, NativeCallbackInfo& info);
    NativeValue* OnGetMainWindowSync(NativeEngine& engine, NativeCallbackInfo& info);
    NativeValue* OnEvent(NativeEngine& engine, NativeCallbackInfo& info);
    NativeValue* OffEvent(NativeEngine& engine, NativeCallbackInfo& info);
    NativeValue* OnLoadContent(NativeEngine& engine, NativeCallbackInfo& info);
    NativeValue* OnGetWindowMode(NativeEngine& engine, NativeCallbackInfo& info);
    NativeValue* OnCreateSubWindow(NativeEngine& engine, NativeCallbackInfo& info);
    NativeValue* OnGetSubWindow(NativeEngine& engine, NativeCallbackInfo& info);
    NativeValue* OnSetShowOnLockScreen(NativeEngine& engine, NativeCallbackInfo& info);
    NativeValue* OnDisableWindowDecor(NativeEngine& engine, NativeCallbackInfo& info);

    std::weak_ptr<Rosen::WindowScene> windowScene_;
};
}  // namespace Rosen
}  // namespace OHOS
#endif  // OHOS_JS_WINDOW_STAGE_H
