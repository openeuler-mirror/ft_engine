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

#ifndef OHOS_ROSEN_CLIENT_AGENT_MANAGER_H
#define OHOS_ROSEN_CLIENT_AGENT_MANAGER_H

#include <map>
#include <mutex>
#include <set>
#include "agent_death_recipient.h"
#include "window_manager_hilog.h"

namespace OHOS {
namespace Rosen {
template <typename T1, typename T2>
class ClientAgentContainer {
public:
    ClientAgentContainer();
    virtual ~ClientAgentContainer() = default;

    bool RegisterAgent(const sptr<T1>& agent, T2 type);
    bool UnregisterAgent(const sptr<T1>& agent, T2 type);
    std::set<sptr<T1>> GetAgentsByType(T2 type);

private:
    void RemoveAgent(const sptr<IRemoteObject>& remoteObject);
    bool UnregisterAgentLocked(std::set<sptr<T1>>& agents, const sptr<IRemoteObject>& agent);

    static constexpr HiviewDFX::HiLogLabel LABEL = {LOG_CORE, HILOG_DOMAIN_WINDOW, "ClientAgentContainer"};

    struct finder_t {
        finder_t(sptr<IRemoteObject> remoteObject) : remoteObject_(remoteObject) {}

        bool operator()(sptr<T1> agent)
        {
            return agent->AsObject() == remoteObject_;
        }

        sptr<IRemoteObject> remoteObject_;
    };

    std::recursive_mutex mutex_;
    std::map<T2, std::set<sptr<T1>>> agentMap_;
    sptr<AgentDeathRecipient> deathRecipient_;
};

template<typename T1, typename T2>
ClientAgentContainer<T1, T2>::ClientAgentContainer() : deathRecipient_(
    new AgentDeathRecipient(std::bind(&ClientAgentContainer<T1, T2>::RemoveAgent, this, std::placeholders::_1))) {}

template<typename T1, typename T2>
bool ClientAgentContainer<T1, T2>::RegisterAgent(const sptr<T1>& agent, T2 type)
{
    std::lock_guard<std::recursive_mutex> lock(mutex_);
    auto iter = std::find_if(agentMap_[type].begin(), agentMap_[type].end(), finder_t(agent->AsObject()));
    if (iter != agentMap_[type].end()) {
        WLOGFW("failed to register agent");
        return false;
    }
    agentMap_[type].insert(agent);
    if (deathRecipient_ == nullptr || !agent->AsObject()->AddDeathRecipient(deathRecipient_)) {
        WLOGFI("failed to add death recipient");
    }
    return true;
}

template<typename T1, typename T2>
bool ClientAgentContainer<T1, T2>::UnregisterAgent(const sptr<T1>& agent, T2 type)
{
    std::lock_guard<std::recursive_mutex> lock(mutex_);
    if (agent == nullptr || agentMap_.count(type) == 0) {
        WLOGFE("agent or type is invalid");
        return false;
    }
    auto& agents = agentMap_.at(type);
    bool ret = UnregisterAgentLocked(agents, agent->AsObject());
    agent->AsObject()->RemoveDeathRecipient(deathRecipient_);
    return ret;
}

template<typename T1, typename T2>
std::set<sptr<T1>> ClientAgentContainer<T1, T2>::GetAgentsByType(T2 type)
{
    std::lock_guard<std::recursive_mutex> lock(mutex_);
    if (agentMap_.count(type) == 0) {
        WLOGFI("no such type of agent registered! type:%{public}u", type);
        return std::set<sptr<T1>>();
    }
    return agentMap_.at(type);
}

template<typename T1, typename T2>
bool ClientAgentContainer<T1, T2>::UnregisterAgentLocked(std::set<sptr<T1>>& agents,
    const sptr<IRemoteObject>& agent)
{
    auto iter = std::find_if(agents.begin(), agents.end(), finder_t(agent));
    if (iter == agents.end()) {
        WLOGFW("could not find this agent");
        return false;
    }
    agents.erase(iter);
    WLOGFI("agent unregistered");
    return true;
}

template<typename T1, typename T2>
void ClientAgentContainer<T1, T2>::RemoveAgent(const sptr<IRemoteObject>& remoteObject)
{
    WLOGFI("RemoveAgent");
    std::lock_guard<std::recursive_mutex> lock(mutex_);
    for (auto& elem : agentMap_) {
        if (UnregisterAgentLocked(elem.second, remoteObject)) {
            break;
        }
    }
    remoteObject->RemoveDeathRecipient(deathRecipient_);
}
}
}
#endif // OHOS_ROSEN_CLIENT_AGENT_MANAGER_H
