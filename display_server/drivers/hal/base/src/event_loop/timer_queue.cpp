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

#include "timer_queue.h"

#include <sys/timerfd.h>

#include "event_loop.h"
#include "log.h"

namespace FT {
namespace detail {
int CreateTimerFd()
{
    int fd = TEMP_FAILURE_RETRY(::timerfd_create(CLOCK_MONOTONIC, TFD_CLOEXEC));
    if (IsInvalidFd(fd)) {
        LOG_FATAL("Create timerFd error: %{public}s", ErrnoToString(errno).c_str());
    }

    return fd;
}

itimerspec GenerateTimerSpec(TimeStamp dstTime)
{
    itimerspec newValue{};

    auto diffMicros = TimeDiff(dstTime, TimeStamp::Now());
    // set minimum diffMicros to 1 us to make sure the timer can be triggered.
    diffMicros = std::max(decltype(diffMicros)(1), diffMicros);

    newValue.it_value.tv_sec = diffMicros / MICRO_SECS_PER_SECOND;
    newValue.it_value.tv_nsec = (diffMicros * NANO_SECS_PER_MICROSECOND) % NANO_SECS_PER_SECOND;
    return newValue;
}
} // namespace detail

TimerQueue::TimerQueue(EventLoop *loop)
    : loop_(loop),
      timerFd_(detail::CreateTimerFd()),
      timerFdChannel_(std::make_unique<EventChannel>(timerFd_.Get(), loop_))
{
    timerFdChannel_->SetReadCallback([this](TimeStamp t) { HandleRead(t); });
    timerFdChannel_->EnableReading();
}

TimerQueue::~TimerQueue() noexcept
{
    timerFdChannel_->DisableAll();
}

TimerId TimerQueue::AddTimer(TimerCallback callback, TimeStamp expireTime, TimeType interval)
{
    auto future = loop_->Schedule([=, cb(std::move(callback))]() mutable -> TimerId {
        auto newTimer = std::make_unique<Timer>(std::move(cb), expireTime, interval);
        TimerId id = newTimer->Id();
        AddTimerInLoop(std::move(newTimer));
        return id;
    });
    return future.get();
}

void TimerQueue::AddTimerInLoop(std::unique_ptr<Timer> &&timer)
{
    ASSERT(timer != nullptr);
    AssertInLoopThread();

    TimeStamp expireTime = timer->ExpireTime();
    bool needToResetTimerFd = (timerEntries_.empty() || expireTime < timerEntries_.cbegin()->first);

    TimerId timerId = timer->Id();
    timerEntries_.insert(std::make_pair(expireTime, timerId));
    timers_[timerId] = std::move(timer);

    if (needToResetTimerFd) {
        TimerFdReset(expireTime);
    }
}

void TimerQueue::CancelTimer(const TimerId &timerId)
{
    loop_->RunInLoop([=]() { CancelTimerInLoop(timerId); });
}

void TimerQueue::CancelTimerInLoop(const TimerId &timerId)
{
    AssertInLoopThread();

    auto timer = timerId.timer;
    if (timer == nullptr) {
        return;
    }

    auto timerEntry = std::make_pair(timer->ExpireTime(), timerId);
    if (timers_.count(timerId) == 0 || timerEntries_.count(timerEntry) == 0) {
        return;
    }

    LOG_DEBUG("Cancel Timer(id: %{public}lu).", timerId.id);
    bool needToResetTimerFd = (timerEntry == *timerEntries_.cbegin());
    timers_.erase(timerId);
    timerEntries_.erase(timerEntry);

    if (needToResetTimerFd) {
        TimerFdReset(timerEntries_.cbegin()->first);
    }
}

void TimerQueue::AssertInLoopThread()
{
    ASSERT(loop_ != nullptr);
    loop_->AssertInLoopThread();
}

std::vector<TimerEntry> TimerQueue::GetExpiredTimers(TimeStamp receivedTime)
{
    std::vector<TimerEntry> expiredTimers;

    TimerEntry pivot = std::make_pair(receivedTime, TimerId(0, nullptr));
    auto it = timerEntries_.lower_bound(pivot);
    ASSERT(receivedTime <= it->first || it == timerEntries_.cend());
    std::copy(timerEntries_.begin(), it, std::back_inserter(expiredTimers));
    timerEntries_.erase(timerEntries_.begin(), it);

    return expiredTimers;
}

void TimerQueue::HandleRead(TimeStamp receivedTime)
{
    AssertInLoopThread();
    TimerFdRead();

    {
        auto expiredTimers = GetExpiredTimers(receivedTime);
        for (const auto &[timestamp, timerId] : expiredTimers) {
            ASSERT(timers_.count(timerId) > 0);
            auto &timer = timers_.at(timerId);
            ASSERT(timer != nullptr);
            if (timer->IsRepeat()) {
                // restart the timer and insert it back to the timer_queue.
                timer->Restart(timestamp);
                timerEntries_.insert(std::make_pair(timer->ExpireTime(), timerId));
            } else {
                timers_.erase(timerId);
            }

            timer->Execute();
        }
    }

    auto nextExpiredTime = timerEntries_.cbegin()->first;
    TimerFdReset(nextExpiredTime);
}

void TimerQueue::TimerFdRead()
{
    uint64_t one = 0;
    int len = TEMP_FAILURE_RETRY(::read(timerFd_.Get(), &one, sizeof(one)));
    if (len != sizeof(one)) {
        LOG_WARN("Read from timerFd(%{public}i) %{public}i bytes, should be %{public}lu bytes.",
            timerFd_.Get(), len, sizeof(one));
    }
}

void TimerQueue::TimerFdReset(TimeStamp expireTime)
{
    auto newValue = detail::GenerateTimerSpec(expireTime);
    int ret = TEMP_FAILURE_RETRY(::timerfd_settime(timerFd_.Get(), 0, &newValue, nullptr));
    if (ret != 0) {
        LOG_FATAL("TimerFd set time error: %{public}s", ErrnoToString(errno).c_str());
    }
}
} // namespace FT
