/*
 * Copyright (c) 2023 Huawei Technologies Co., Ltd.
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

#ifndef OHOS_ABILITY_RUNTIME_ABILITY_MANAGER_CLIENT_H
#define OHOS_ABILITY_RUNTIME_ABILITY_MANAGER_CLIENT_H

namespace OHOS {
namespace AbilityRuntime {
class AbilityContext {
public:
    sptr<IRemoteObject> GetToken()
    {
        return nullptr;
    }
    int32_t TerminateSelf()
    {
        return 0;
    }
    int32_t CloseAbility()
    {
        return 0;
    }
    std::shared_ptr<AppExecFwk::AbilityInfo> GetAbilityInfo() const
    {
        return nullptr;
    }
};

class Context {
public:
    template<class T>
    static std::shared_ptr<T> ConvertTo(const std::shared_ptr<Context>& context)
    {
        return nullptr;
    }
    std::string GetBundleName() const
    {
        return "";
    }
    sptr<IRemoteObject> GetToken()
    {
        return nullptr;
    }
};
}  // namespace AbilityRuntime
}  // namespace OHOS

namespace OHOS {
namespace AAFwk {
class AbilityManagerClient {
public:
    static std::shared_ptr<AbilityManagerClient> GetInstance()
    {
        return nullptr;
    }
    int32_t DoAbilityForeground(const sptr<IRemoteObject> &token, uint32_t flag)
    {
        return 0;
    }
    int32_t DoAbilityBackground(const sptr<IRemoteObject> &token, uint32_t flag)
    {
        return 0;
    }
    int32_t MinimizeAbility(const sptr<IRemoteObject> &token, bool fromUser = false)
    {
        return 0;
    }
};
}  // namespace AAFwk
}  // namespace OHOS
#endif  // OHOS_ABILITY_RUNTIME_ABILITY_MANAGER_CLIENT_H
