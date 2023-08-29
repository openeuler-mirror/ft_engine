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

#include "event_loop.h"

#include <fcntl.h>
#include <sys/eventfd.h>

#include "current_thread.h"
#include "log.h"

namespace FT {
namespace detail {
int CreateEventFdOrDie()
{
    int fd = ::eventfd(0, EFD_NONBLOCK | EFD_CLOEXEC);
    if (fd < 0) {
        LOG_FATAL("Create eventFd failed: %{public}s.", ErrnoToString(errno).c_str());
    }

    return fd;
}
} // namespace detail
__thread EventLoop *t_currLoop = nullptr; // current thread's event_loop

EventLoop::EventLoop()
    : tid_(CurrentThread::Tid()),
      poller_(std::make_unique<EventPoller>(this)),
      wakeUpFd_(detail::CreateEventFdOrDie()),
      wakeUpChannel_(std::make_unique<EventChannel>(wakeUpFd_.Get(), this)),
      timerQueue_(std::make_unique<TimerQueue>(this))
{
    if (t_currLoop != nullptr) {
        LOG_FATAL("Construct EventLoop failed: current thread already have a loop(%{public}p)!", &t_currLoop);
    }

    // wakeUpCallback do not need TimeStamp
    wakeUpChannel_->SetReadCallback([this](TimeStamp) { WakeUpCallback(); });
    wakeUpChannel_->EnableReading();
    t_currLoop = this;
}

EventLoop::~EventLoop() noexcept
{
    wakeUpChannel_->DisableAll();
    Stop();
    t_currLoop = nullptr;
}

void EventLoop::Stop() noexcept
{
    if (!running_) {
        return;
    }

    running_ = false;

    if (!IsInLoopThread()) {
        WakeUp();
    }
}

void EventLoop::UpdateChannel(EventChannel *channel)
{
    if (channel == nullptr) {
        LOG_WARN("EventLoop::%{public}s: channel is null!", __func__);
        return;
    }

    RunInLoop([this, channel]() mutable { poller_->UpdateChannel(channel); });
}

void EventLoop::RemoveChannel(int channelFd)
{
    RunInLoop([=]() { poller_->RemoveChannel(channelFd); });
}

void EventLoop::ExecPendingFunctors()
{
    AssertInLoopThread();

    executingPendingFunctors_ = true;
    std::vector<Functor> funcs;
    {
        std::lock_guard<std::mutex> lock(mutex_);
        funcs.swap(pendingFunctors_);
    }

    for (const auto &func : funcs) {
        func();
    }
    executingPendingFunctors_ = false;
}

void EventLoop::QueueToLoop(Functor func)
{
    {
        std::lock_guard<std::mutex> lock(mutex_);
        pendingFunctors_.push_back(std::move(func));
    }

    if (!IsInLoopThread() || executingPendingFunctors_) {
        WakeUp();
    }
}

void EventLoop::RunInLoop(Functor func)
{
    if (IsInLoopThread()) {
        func();
    } else {
        QueueToLoop(std::move(func));
    }
}

TimerId EventLoop::RunAt(Functor func, TimeStamp dstTime)
{
    return timerQueue_->AddTimer(std::move(func), dstTime);
}

TimerId EventLoop::RunAfter(Functor func, TimeType delay)
{
    return timerQueue_->AddTimer(std::move(func), TimeAdd(TimeStamp::Now(), delay));
}

TimerId EventLoop::RunEvery(Functor func, TimeType interval, TimeType delay)
{
    return timerQueue_->AddTimer(std::move(func), TimeAdd(TimeStamp::Now(), delay), interval);
}

void EventLoop::Cancel(const TimerId &timerId)
{
    timerQueue_->CancelTimer(timerId);
}

void EventLoop::Start()
{
    AssertInLoopThread();

    running_ = true;
    while (running_) {
        std::vector<EventChannel *> activeChannels;
        TimeStamp pollTime = poller_->PollOnce(activeChannels, -1);
        for (const auto &channel : activeChannels) {
            if (channel != nullptr) {
                channel->HandleEvent(pollTime);
            }
        }

        ExecPendingFunctors();
    }
}

bool EventLoop::IsInLoopThread() const
{
    return CurrentThread::Tid() == tid_;
}

void EventLoop::AssertInLoopThread() const
{
    if (OE_UNLIKELY(!IsInLoopThread())) {
        LOG_FATAL("assertInLoopThread failed!");
    }
}

void EventLoop::AssertNotInLoopThread() const
{
    if (OE_UNLIKELY(IsInLoopThread())) {
        LOG_FATAL("assertNotInLoopThread failed!");
    }
}

void EventLoop::WakeUp()
{
    uint64_t buf = 1;
    int len = TEMP_FAILURE_RETRY(::write(wakeUpChannel_->Fd(), &buf, sizeof(buf)));
    if (OE_UNLIKELY(len != sizeof(buf))) {
        LOG_WARN("should write %{public}lu bytes, but %{public}i wrote.", sizeof(buf), len);
    }
}

void EventLoop::WakeUpCallback()
{
    uint64_t buf = 0;
    int len = TEMP_FAILURE_RETRY(::read(wakeUpChannel_->Fd(), &buf, sizeof(buf)));
    if (OE_UNLIKELY(len != sizeof(buf))) {
        LOG_WARN("should read %{public}lu bytes, but %{public}i read.", sizeof(buf), len);
    }
}

EventLoop *EventLoop::EventLoopOfCurrThread()
{
    return t_currLoop;
}
} // namespace FT
