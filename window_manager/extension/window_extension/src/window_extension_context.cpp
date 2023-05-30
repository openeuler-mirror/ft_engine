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

#include "window_extension_context.h"

#include "ability_info.h"
#include "native_engine/native_reference.h"
#include "window_manager_hilog.h"

namespace OHOS {
namespace Rosen {

namespace {
    constexpr HiviewDFX::HiLogLabel LABEL = {LOG_CORE, HILOG_DOMAIN_WINDOW, "WindowExtensionContext"};
}

const size_t WindowExtensionContext::CONTEXT_TYPE_ID(std::hash<const char*> {} ("WindowExtensionContext"));
int WindowExtensionContext::ILLEGAL_REQUEST_CODE(-1);

WMError WindowExtensionContext::StartAbility(const AAFwk::Want &want, const AAFwk::StartOptions &startOptions) const
{
    WLOGFD("Start ability begin, ability:%{public}s.", want.GetElement().GetAbilityName().c_str());
    ErrCode err = AAFwk::AbilityManagerClient::GetInstance()->StartAbility(want, startOptions, token_,
        ILLEGAL_REQUEST_CODE);
    if (err != ERR_OK) {
        WLOGFE("WindowContext::StartAbility is failed %{public}d", err);
        return WMError::WM_ERROR_START_ABILITY_FAILED;
    }
    return WMError::WM_OK;
}
}  // namespace Rosen
}  // namespace OHOS
