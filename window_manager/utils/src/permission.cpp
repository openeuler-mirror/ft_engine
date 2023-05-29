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

#include "permission.h"

#include <accesstoken_kit.h>
#include <bundle_constants.h>
#include <ipc_skeleton.h>
#include <bundle_mgr_proxy.h>
#include <system_ability_definition.h>
#include <iservice_registry.h>

#include "window_manager_hilog.h"

namespace OHOS {
namespace Rosen {
namespace {
    constexpr HiviewDFX::HiLogLabel LABEL = {LOG_CORE, HILOG_DOMAIN_WINDOW, "WMPermission"};
}

bool Permission::IsSystemServiceCalling(bool needPrintLog)
{
    Security::AccessToken::NativeTokenInfo tokenInfo;
    Security::AccessToken::AccessTokenKit::GetNativeTokenInfo(IPCSkeleton::GetCallingTokenID(), tokenInfo);
    if (tokenInfo.apl == Security::AccessToken::ATokenAplEnum::APL_SYSTEM_CORE ||
        tokenInfo.apl == Security::AccessToken::ATokenAplEnum::APL_SYSTEM_BASIC) {
        return true;
    }
    if (needPrintLog) {
        WLOGFE("Is not system service calling, native apl: %{public}d", tokenInfo.apl);
    }
    return false;
}

bool Permission::IsSystemCalling()
{
    if (IsSystemServiceCalling(false)) {
        return true;
    }
    int32_t uid = IPCSkeleton::GetCallingUid();
    if (uid < 0) {
        WLOGFE("Is not system calling, app caller uid is: %d,", uid);
        return false;
    }

    sptr<ISystemAbilityManager> systemAbilityManager =
        SystemAbilityManagerClient::GetInstance().GetSystemAbilityManager();
    if (systemAbilityManager == nullptr) {
        WLOGFE("Is not system calling, failed to get system ability mgr.");
        return false;
    }
    sptr<IRemoteObject> remoteObject = systemAbilityManager->GetSystemAbility(BUNDLE_MGR_SERVICE_SYS_ABILITY_ID);
    if (remoteObject == nullptr) {
        WLOGFE("Is not system calling, failed to get bundle manager proxy.");
        return false;
    }
    sptr<AppExecFwk::IBundleMgr> iBundleMgr = iface_cast<AppExecFwk::IBundleMgr>(remoteObject);
    if (iBundleMgr == nullptr) {
        WLOGFE("Is not system calling, iBundleMgr is nullptr");
        return false;
    }
    bool isSystemAppCalling = iBundleMgr->CheckIsSystemAppByUid(uid);
    if (!isSystemAppCalling) {
        WLOGFE("Is not system calling, UID:%{public}d  IsSystemApp:%{public}d", uid, isSystemAppCalling);
    }
    return isSystemAppCalling;
}

bool Permission::CheckCallingPermission(const std::string& permission)
{
    WLOGFI("permission:%{public}s", permission.c_str());

    if (Security::AccessToken::AccessTokenKit::VerifyAccessToken(IPCSkeleton::GetCallingTokenID(), permission) !=
        AppExecFwk::Constants::PERMISSION_GRANTED) {
        WLOGFI("permission denied!");
        return false;
    }
    WLOGFI("permission ok!");
    return true;
}

bool Permission::IsStartByHdcd()
{
    OHOS::Security::AccessToken::NativeTokenInfo info;
    if (Security::AccessToken::AccessTokenKit::GetNativeTokenInfo(IPCSkeleton::GetCallingTokenID(), info) != 0) {
        return false;
    }
    if (info.processName.compare("hdcd") == 0) {
        return true;
    }
    return false;
}
} // namespace Rosen
} // namespace OHOS