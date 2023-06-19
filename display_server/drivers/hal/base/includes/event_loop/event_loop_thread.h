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

#pragma once

#include <mutex>
#include <condition_variable>
#include <thread>

#include "event_loop.h"

namespace oewm {
class EventLoopThread : NonCopyable {
public:
    EventLoopThread();
    explicit EventLoopThread(std::string name);
    ~EventLoopThread() noexcept;

    EventLoop *Start();
    const std::string &Name() const
    {
        return name_;
    }

private:
    void LoopThreadFunc();

    mutable std::mutex mutex_;
    std::condition_variable cond_;
    std::string name_;
    std::thread thread_;
    EventLoop *loop_ = nullptr;
};
} // namespace oewm
