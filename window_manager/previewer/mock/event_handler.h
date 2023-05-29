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

#ifndef OHOS_EVENTHANDLER_H
#define OHOS_EVENTHANDLER_H

#include <string>
#include <memory>

#include "event_queue.h"
#include "event_runner.h"

namespace OHOS {
namespace AppExecFwk {
class EventHandler {
public:
    using Callback = std::function<void()>;
    using Priority = EventQueue::Priority;

    EventHandler() {};
    explicit EventHandler(const std::shared_ptr<EventRunner> &runner = nullptr) {};
    virtual ~EventHandler() {};

    static std::shared_ptr<EventHandler> Current()
    {
        return nullptr;
    }

    void RemoveTask(const std::string &name)
    {
        return;
    }
    
    inline bool PostTask(const Callback &callback, const std::string &name = std::string(), int64_t delayTime = 0,
        Priority priority = Priority::LOW)
    {
        return true;
    }
};
} // namespace AppExecFwk
} // namespace OHOS

#endif // OHOS_EVENTHANDLER_H
