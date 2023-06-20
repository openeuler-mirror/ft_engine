/*
 * Copyright (c) 2021-2023 Huawei Device Co., Ltd.
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

#ifndef INTERFACES_INNER_KITS_ACCESSTOKEN_KIT_H
#define INTERFACES_INNER_KITS_ACCESSTOKEN_KIT_H

#include <string>

#include "bundle_constants.h"

namespace OHOS {
namespace Security {
namespace AccessToken {
typedef unsigned int AccessTokenID;

typedef enum TypeATokenAplEnum {
    APL_INVALID = 0,
    APL_NORMAL = 1,
    APL_SYSTEM_BASIC = 2,
    APL_SYSTEM_CORE = 3,
} ATokenAplEnum;

class NativeTokenInfo {
public:
    ATokenAplEnum apl;
    std::string processName;
};

class AccessTokenKit {
public:
    static int GetNativeTokenInfo(AccessTokenID tokenID, NativeTokenInfo& nativeTokenInfoRes)
    {
        nativeTokenInfoRes.apl = APL_SYSTEM_BASIC;
        nativeTokenInfoRes.processName = "openEuler";
        return 0;
    }

    static int VerifyAccessToken(AccessTokenID tokenID, const std::string& permissionName)
    {
        return AppExecFwk::Constants::PERMISSION_GRANTED;
    }
};
} // namespace AccessToken
} // namespace Security
} // namespace OHOS
#endif
