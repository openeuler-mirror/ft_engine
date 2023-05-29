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

#include "window_extension_module_loader.h"

#include "window_extension.h"
#include "window_manager_hilog.h"

namespace OHOS::Rosen {
namespace {
    constexpr HiviewDFX::HiLogLabel LABEL = {LOG_CORE, HILOG_DOMAIN_WINDOW, "WindowExtensionStub"};
}
WindowExtensionModuleLoader::WindowExtensionModuleLoader() = default;
WindowExtensionModuleLoader::~WindowExtensionModuleLoader() = default;

AbilityRuntime::Extension *WindowExtensionModuleLoader::Create(
    const std::unique_ptr<AbilityRuntime::Runtime>& runtime) const
{
    WLOGFI("called");
    return WindowExtension::Create(runtime);
}

std::map<std::string, std::string> WindowExtensionModuleLoader::GetParams()
{
    std::map<std::string, std::string> params;
    // type means extension type in ExtensionAbilityType of extension_ability_info.h, 10 means window.
    params.insert(std::pair<std::string, std::string>("type", "10"));
    // extension name
    params.insert(std::pair<std::string, std::string>("name", "WindowExtension"));
    WLOGFI("called");
    return params;
}

extern "C" __attribute__((visibility("default"))) void* OHOS_EXTENSION_GetExtensionModule()
{
    WLOGFI("called");
    return &WindowExtensionModuleLoader::GetInstance();
}
} // namespace OHOS::Window

