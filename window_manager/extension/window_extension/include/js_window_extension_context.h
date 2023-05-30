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

#ifndef JS_WINDOW_EXTENSION_CONTEXT_H
#define JS_WINDOW_EXTENSION_CONTEXT_H

#include <ability_connect_callback.h>
#include <event_handler.h>
#include <extension_context.h>
#include <native_engine/native_engine.h>

#include "window.h"
#include "window_extension_context.h"

namespace OHOS {
namespace Rosen {
class JsWindowConnectCallback : public AbilityRuntime::AbilityConnectCallback {
public:
    JsWindowConnectCallback() = default;
    ~JsWindowConnectCallback() = default;
    virtual void OnAbilityConnectDone(const AppExecFwk::ElementName& element,
        const sptr<IRemoteObject>& remoteObject, int resultCode) override;
    virtual void OnAbilityDisconnectDone(const AppExecFwk::ElementName& element, int resultCode) override;
};

NativeValue* CreateJsWindowExtensionContext(NativeEngine& engine,
    const std::shared_ptr<Rosen::WindowExtensionContext>& context);
} // namespace Rosen
} // namespace OHOS
#endif // JS_WINDOW_EXTENSION_CONTEXT_H
