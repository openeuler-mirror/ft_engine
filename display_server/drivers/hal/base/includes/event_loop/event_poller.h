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

#pragma once

#include <unordered_map>
#include <vector>

#include "unique_fd.h"
#include "event_channel.h"

namespace FT {
class EventLoop;

class EventPoller : NonCopyable {
public:
    explicit EventPoller(EventLoop *eventLoop);
    ~EventPoller() noexcept;

    TimeStamp PollOnce(std::vector<EventChannel *> &activeChannels, int timeOutMs);
    void UpdateChannel(EventChannel *channel);
    void RemoveChannel(int fd);

private:
    void AssertInLoopThread();

    void EpollCtl(EventChannel *channel, int operation);
    EventLoop *eventLoop_ = nullptr;
    OHOS::UniqueFd epollFd_;
    static std::size_t eventSize_;
    std::vector<epoll_event> activeEvents_; // to receive events from epoll_wait.
    std::unordered_map<int, EventChannel *> channels_;
};
} // namespace FT
