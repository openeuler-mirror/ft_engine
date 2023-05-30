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

#include "static_call.h"

namespace OHOS {
namespace Rosen {
WM_IMPLEMENT_SINGLE_INSTANCE(StaticCall)

sptr<Window> StaticCall::CreateWindow(const std::string& windowName,
    sptr<WindowOption>& option, std::shared_ptr<AbilityRuntime::Context> context)
{
    return Window::Create(windowName, option, context);
}

std::vector<sptr<Window>> StaticCall::GetSubWindow(uint32_t parentId)
{
    return Window::GetSubWindow(parentId);
}
} // namespace Rosen
} // namespace OHOS