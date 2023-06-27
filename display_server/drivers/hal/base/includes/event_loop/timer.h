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

#include <functional>

#include "noncopyable_hal.h"
#include "timestamp.h"

namespace oewm {
class Timer;

// NonCopyable
struct TimerId {
    TimerId() = default;
    TimerId(uint64_t id, Timer *timer) : id(id), timer(timer) {}

    uint64_t id = 0;
    Timer *timer = nullptr;
    bool operator!() const
    {
        return timer == nullptr;
    }
    bool operator==(std::nullptr_t) const
    {
        return timer == nullptr;
    }
    bool operator!=(std::nullptr_t) const
    {
        return timer != nullptr;
    }
    bool operator==(const TimerId &other) const
    {
        return id == other.id && timer == other.timer;
    }
    bool operator<(const TimerId &other) const
    {
        return id < other.id || timer < other.timer;
    }
};

using TimerCallback = std::function<void()>;

class Timer : NonCopyable {
public:
    // @callback: TimerCallback
    // @expireTime: expire TimeStamp
    // @interval: interval in micro seconds, 0 for only run once.
    Timer(TimerCallback callback, TimeStamp expireTime, TimeType interval = 0);
    ~Timer() noexcept = default;

    TimerId Id() const
    {
        return id_;
    }
    bool IsRepeat() const
    {
        return repeat_;
    }

    TimeStamp ExpireTime() const
    {
        return expireTime_;
    }

    // only valid when the timer is repeated.
    void Restart(TimeStamp now);
    void Execute();

private:
    TimerCallback cb_;
    TimeStamp expireTime_;
    TimeType interval_ = 0;
    bool repeat_ = false;
    TimerId id_;
};
} // namespace oewm

namespace std {
template <>
struct hash<typename oewm::TimerId> {
    std::size_t operator()(oewm::TimerId timerId) const
    {
        return static_cast<std::size_t>(timerId.id ^ reinterpret_cast<uintptr_t>(timerId.timer));
    }
};
} // namespace std
