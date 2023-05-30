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

#ifndef BASE_EVENTHANDLER_INTERFACES_INNER_API_EVENT_RUNNER_H
#define BASE_EVENTHANDLER_INTERFACES_INNER_API_EVENT_RUNNER_H

#include "event_queue.h"

namespace OHOS {
namespace AppExecFwk {
class EventRunner final {
public:
    EventRunner() {};
    virtual ~EventRunner() {};
    static std::shared_ptr<EventRunner> Create(bool inNewThread = true)
    {
        return nullptr;
    }
    static std::shared_ptr<EventRunner> Create(const std::string &threadName)
    {
        return nullptr;
    }
    static inline std::shared_ptr<EventRunner> Create(const char *threadName)
    {
        return nullptr;
    }

    static std::shared_ptr<EventRunner> GetMainEventRunner()
    {
        return nullptr;
    }
    
    uint64_t GetThreadId()
    {
        return 0;
    }

    explicit EventRunner(bool deposit);

    friend class EventHandler;
};
}  // namespace AppExecFwk
}  // namespace OHOS

#endif  // #ifndef BASE_EVENTHANDLER_INTERFACES_INNER_API_EVENT_RUNNER_H
