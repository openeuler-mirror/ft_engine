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

#include "window_extension.h"

#include "js_window_extension.h"
#include "window_extension_context.h"
#include "window_manager_hilog.h"

namespace OHOS {
namespace Rosen {
namespace {
    constexpr HiviewDFX::HiLogLabel LABEL = {LOG_CORE, HILOG_DOMAIN_WINDOW, "WindowExtension"};
}

WindowExtension* WindowExtension::Create(const std::unique_ptr<AbilityRuntime::Runtime>& runtime)
{
    if (!runtime) {
        return new WindowExtension();
    }
    WLOGFI("create window extension");
    switch (runtime->GetLanguage()) {
        case AbilityRuntime::Runtime::Language::JS: {
            WLOGFI("create js windowExtension");
            return JsWindowExtension::Create(runtime);
        }
        default: {
            return new WindowExtension();
        }
    }
    return nullptr;
}

void WindowExtension::Init(const std::shared_ptr<AbilityRuntime::AbilityLocalRecord>& record,
    const std::shared_ptr<AbilityRuntime::OHOSApplication>& application,
    std::shared_ptr<AbilityRuntime::AbilityHandler>& handler,
    const sptr<IRemoteObject>& token)
{
    ExtensionBase<WindowExtensionContext>::Init(record, application, handler, token);
    WLOGFI("WindowExtension begin init");
}

std::shared_ptr<WindowExtensionContext> WindowExtension::CreateAndInitContext(
    const std::shared_ptr<AbilityRuntime::AbilityLocalRecord>& record,
    const std::shared_ptr<AbilityRuntime::OHOSApplication>& application,
    std::shared_ptr<AbilityRuntime::AbilityHandler>& handler,
    const sptr<IRemoteObject>& token)
{
    if (!record) {
        WLOGFE("WindowExtension::CreateAndInitContext record is nullptr");
    }
    return ExtensionBase<WindowExtensionContext>::CreateAndInitContext(record, application, handler, token);
}
} // namespace Rosen
} // namespace OHOS