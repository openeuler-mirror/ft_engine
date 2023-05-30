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
#ifndef OHOS_ROSEN_AGENT_DEATH_RECIPIENT_H
#define OHOS_ROSEN_AGENT_DEATH_RECIPIENT_H

#include <iremote_object.h>

namespace OHOS {
namespace Rosen {
class AgentDeathRecipient : public IRemoteObject::DeathRecipient {
public:
    AgentDeathRecipient(std::function<void(sptr<IRemoteObject>&)> callback = nullptr) : callback_(callback) {}
    ~AgentDeathRecipient() = default;

    virtual void OnRemoteDied(const wptr<IRemoteObject>& wptrDeath) override;

private:
    std::function<void(sptr<IRemoteObject>&)> callback_;
};
}
}
#endif // OHOS_ROSEN_AGENT_DEATH_RECIPIENT_H