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

#include <future>
#include <type_traits>

#include "event_poller.h"
#include "timer_queue.h"

namespace FT {
using Functor = std::function<void()>;

namespace detail {
template <typename Callable>
class PackagedTask : NonCopyable {
    using Return = std::invoke_result_t<Callable>;

public:
    ~PackagedTask() noexcept = default;

    static auto Create(Callable &&cb)
    {
        std::shared_ptr<PackagedTask<Callable>> t(new PackagedTask(std::forward<Callable &&>(cb)));
        return std::make_pair(t, t->cb_.get_future());
    }

    void Run()
    {
        cb_();
    }

private:
    explicit PackagedTask(Callable &&cb) : cb_(std::move(cb)) {}

    std::packaged_task<Return()> cb_;
};
} // namespace detail

class EventLoop : NonCopyable {
public:
    EventLoop();
    ~EventLoop() noexcept;
    void Start();
    void Stop() noexcept;
    void UpdateChannel(EventChannel *channel);
    void RemoveChannel(int channelFd);

    template <typename Task, typename Ret = std::invoke_result_t<Task>>
    std::future<Ret> Schedule(Task task)
    {
        auto [packagedTask, future] = detail::PackagedTask<Task>::Create(std::move(task));
        RunInLoop([t(std::move(packagedTask))]() { t->Run(); });
        return std::move(future);
    }

    // run func immediately if in loop thread, or call queueToLoop() if in other thread.
    void RunInLoop(Functor func);

    // add this func to the last of the loop's pending functors.
    void QueueToLoop(Functor func);

    TimerId RunAt(Functor func, TimeStamp dstTime);

    // delay in micro seconds, 0 means run immediately
    TimerId RunAfter(Functor func, TimeType delay);

    // delay in micro seconds, 0 means run immediately
    // interval in micro seconds, 0 means only run once.
    TimerId RunEvery(Functor func, TimeType interval, TimeType delay = 0);

    void Cancel(const TimerId &timerId);

    static EventLoop *EventLoopOfCurrThread();

    bool IsInLoopThread() const;

    // will abort if not in loop thread.
    void AssertInLoopThread() const;
    // will abort if in loop thread.
    void AssertNotInLoopThread() const;

private:
    void WakeUp();
    void WakeUpCallback();

    void ExecPendingFunctors();

    ThreadId tid_ = -1; // indicates which thread is this loop in.

    mutable std::mutex mutex_;
    std::atomic<bool> running_{false};

    std::unique_ptr<EventPoller> poller_;

    OHOS::UniqueFd wakeUpFd_;
    std::unique_ptr<EventChannel> wakeUpChannel_;

    std::atomic<bool> executingPendingFunctors_{false};
    std::vector<Functor> pendingFunctors_;

    std::unique_ptr<TimerQueue> timerQueue_;
};
} // namespace FT
