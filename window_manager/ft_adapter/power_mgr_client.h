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

#ifndef POWERMGR_POWER_MGR_CLIENT_H
#define POWERMGR_POWER_MGR_CLIENT_H

#include <string>

namespace OHOS {
namespace PowerMgr {
enum class WakeupDeviceType : uint32_t {
    WAKEUP_DEVICE_APPLICATION = 2,
};

enum class RunningLockType : uint32_t {
    RUNNINGLOCK_SCREEN = 0,
};

class PowerMgrClient {
public:
    static PowerMgrClient& GetInstance() {
        static PowerMgrClient client;
        return client;
    }
    int32_t WakeupDevice(WakeupDeviceType reason = WakeupDeviceType::WAKEUP_DEVICE_APPLICATION,
        const std::string& detail = std::string("app call"))
    {
        return 0;
    }
    bool IsScreenOn()
    {
        return true;
    }
    std::shared_ptr<RunningLock> CreateRunningLock(const std::string& name, RunningLockType type)
    {
        return nullptr;
    }
};
} // namespace PowerMgr
} // namespace OHOS
#endif // POWERMGR_POWER_MGR_CLIENT_H
