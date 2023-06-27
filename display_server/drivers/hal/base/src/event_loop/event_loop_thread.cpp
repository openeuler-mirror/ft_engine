/*
 * Copyright (c) 2022 Huawei Technologies Co., Ltd.
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

#include "event_loop_thread.h"

#include "log.h"

namespace oewm {
EventLoopThread::EventLoopThread() : EventLoopThread("OEWMEventLoopThread") {}

EventLoopThread::EventLoopThread(std::string name) : name_(std::move(name)) {}

EventLoopThread::~EventLoopThread() noexcept
{
    {
        std::lock_guard<std::mutex> lock(mutex_);
        if (loop_ != nullptr) {
            loop_->Stop();
        }
    }

    if (thread_.joinable()) {
        thread_.join();
    }

    LOG_DEBUG("%{public}s Stopped.", name_.c_str());
}

EventLoop *EventLoopThread::Start()
{
    thread_ = std::thread([this]() { LoopThreadFunc(); });

    std::unique_lock<std::mutex> lock(mutex_);
    cond_.wait(lock, [this]() -> bool { return loop_ != nullptr; });
    return loop_;
}

void EventLoopThread::LoopThreadFunc()
{
    EventLoop loop;

    {
        std::lock_guard<std::mutex> lock(mutex_);
        loop_ = &loop;
        cond_.notify_one();
    }

    loop.Start();

    {
        std::lock_guard<std::mutex> lock(mutex_);
        loop_ = nullptr;
    }
}
} // namespace oewm
