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

#include <set>
#include <unordered_map>
#include <vector>

#include "unique_fd.h"
#include "event_channel.h"
#include "timer.h"

namespace oewm {
class EventLoop;

using TimerPtr = std::unique_ptr<Timer>;
using TimerMap = std::unordered_map<TimerId, TimerPtr>; // To hold timers, manager the timers' ownership.
using TimerEntry = std::pair<TimeStamp, TimerId>;       // Make sure every TimerEntry is unique.
using TimerEntrySet = std::set<TimerEntry>;             // To sort timers ordered by expireTime

class TimerQueue : NonCopyable {
public:
    explicit TimerQueue(EventLoop *loop);
    ~TimerQueue() noexcept;

    // @callback: TimerCallback
    // @expireTime: expire TimeStamp
    // @interval: interval in micro seconds, 0 for only run once.
    // @return: TimerId
    TimerId AddTimer(TimerCallback callback, TimeStamp expireTime, TimeType interval = 0);

    // @timerId: TimerId to cancel
    // can only be called in loop thread
    void CancelTimer(const TimerId &timerId);

private:
    void AssertInLoopThread();

    void AddTimerInLoop(std::unique_ptr<Timer> &&timer);
    void CancelTimerInLoop(const TimerId &timerId);
    std::vector<TimerEntry> GetExpiredTimers(TimeStamp receivedTime);

    void HandleRead(TimeStamp receivedTime);
    void TimerFdRead();
    void TimerFdReset(TimeStamp expireTime);

    EventLoop *loop_ = nullptr;
    OHOS::UniqueFd timerFd_;
    std::unique_ptr<EventChannel> timerFdChannel_;

    TimerMap timers_;
    TimerEntrySet timerEntries_;
};
} // namespace oewm
