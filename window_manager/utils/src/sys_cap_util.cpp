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

#include "sys_cap_util.h"
#include "string_util.h"
#include "window_manager_hilog.h"

#include <accesstoken_kit.h>
#include <bundle_mgr_interface.h>
#include <ipc_skeleton.h>
#include <iservice_registry.h>
#include <system_ability_definition.h>

namespace OHOS {
namespace Rosen {
namespace {
    constexpr HiviewDFX::HiLogLabel LABEL = {LOG_CORE, HILOG_DOMAIN_WINDOW, "WmSysCapUtil"};
}

std::string SysCapUtil::GetClientName()
{
    std::string bn = GetBundleName();
    if (!bn.empty()) {
        WLOGFD("bundle name [%{public}s]", bn.c_str());
        return bn;
    }

    std::string pn = GetProcessName();
    if (!pn.empty()) {
        WLOGFD("process name [%{public}s]", pn.c_str());
        return pn;
    }

    WLOGFD("unknow name");
    return "unknown";
}

std::string SysCapUtil::GetBundleName()
{
    OHOS::sptr<OHOS::ISystemAbilityManager> systemAbilityManager =
        OHOS::SystemAbilityManagerClient::GetInstance().GetSystemAbilityManager();
    OHOS::sptr<OHOS::IRemoteObject> remoteObject =
        systemAbilityManager->GetSystemAbility(BUNDLE_MGR_SERVICE_SYS_ABILITY_ID);
    sptr<AppExecFwk::IBundleMgr> iBundleMgr = OHOS::iface_cast<AppExecFwk::IBundleMgr>(remoteObject);
    if (iBundleMgr == nullptr) {
        WLOGFW("IBundleMgr is null");
        return "";
    }

    std::string bundleName = "";
    iBundleMgr->GetBundleNameForUid(IPCSkeleton::GetCallingUid(), bundleName);
    return StringUtil::Trim(bundleName);
}

std::string SysCapUtil::GetProcessName()
{
    OHOS::Security::AccessToken::NativeTokenInfo info;
    if (Security::AccessToken::AccessTokenKit::GetNativeTokenInfo(IPCSkeleton::GetCallingTokenID(), info) != 0) {
        WLOGFW("get token info failed");
        return "";
    }
    return StringUtil::Trim(info.processName);
}
} // namespace Rosen
} // namespace OHOS