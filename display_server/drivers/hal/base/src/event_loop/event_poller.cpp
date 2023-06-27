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

#include "event_poller.h"

#include "types.h"
#include "event_loop.h"

#include "log.h"

namespace oewm {
namespace detail {
std::string EpollOperationToString(int operation)
{
    switch (operation) {
        case EPOLL_CTL_ADD:
            return "EpollCtlAdd";
        case EPOLL_CTL_MOD:
            return "EpollCtlMod";
        case EPOLL_CTL_DEL:
            return "EpollCtlDel";
        default:
            return "UnknownEpollCtl";
    }
}
} // namespace detail

std::size_t EventPoller::eventSize_ = 32;

EventPoller::EventPoller(EventLoop *eventLoop)
    : eventLoop_(eventLoop), epollFd_(::epoll_create1(EPOLL_CLOEXEC)), activeEvents_(eventSize_)
{
    if (eventLoop_ == nullptr) {
        LOG_FATAL("EventLoop is null!");
    }
}

EventPoller::~EventPoller() noexcept {}

TimeStamp EventPoller::PollOnce(std::vector<EventChannel *> &activeChannels, int timeOutMs)
{
    auto cnt = TEMP_FAILURE_RETRY(::epoll_wait(epollFd_.Get(), activeEvents_.data(), eventSize_, timeOutMs));
    auto pollTime = TimeStamp::Now();
    if (cnt < 0) {
        LOG_WARN("epoll_wait error: %{public}s.", ErrnoToString(errno).c_str());
    } else {
        for (int i = 0; i < cnt; ++i) {
            const auto &event = activeEvents_[i];
            int fd = event.data.fd;
            if (channels_.count(fd) == 0 && channels_.at(fd) == nullptr) {
                LOG_WARN("epoll_wait returned a channel that is not in poller(%{public}i).", epollFd_.Get());
                continue;
            }

            const auto &channel = channels_.at(fd);
            channel->SetReceivedEvents(event.events);
            activeChannels.emplace_back(channel);
        }

        if (static_cast<std::size_t>(cnt) == eventSize_) {
            eventSize_ *= 2;
            activeEvents_.resize(eventSize_);
        }
    }

    return pollTime;
}

void EventPoller::AssertInLoopThread()
{
    ASSERT(eventLoop_ != nullptr);
    eventLoop_->AssertInLoopThread();
}

void EventPoller::EpollCtl(EventChannel *channel, int operation)
{
    ASSERT(channel != nullptr);
    int fd = channel->Fd();
    epoll_event epollEvent;
    epollEvent.events = channel->ListeningEvents();
    epollEvent.data.fd = fd;
    int ret = TEMP_FAILURE_RETRY(::epoll_ctl(epollFd_.Get(), operation, fd, &epollEvent));
    if (ret < 0) {
        LOG_ERROR("%{public}s failed for EventPoller(fd: %{public}i): %{public}s.", 
            detail::EpollOperationToString(operation).c_str(), fd, ErrnoToString(errno).c_str());
    }
}

void EventPoller::UpdateChannel(EventChannel *channel)
{
    if (channel == nullptr) {
        return;
    }

    eventLoop_->AssertInLoopThread();

    int fd = channel->Fd();
    if (channel->HasNoEvent()) {
        eventLoop_->RemoveChannel(fd);
        return;
    }

    if (channels_.count(fd) == 0) {
        // new channel
        EpollCtl(channel, EPOLL_CTL_ADD);
        channels_[fd] = channel;
    } else {
        // modify channel
        EpollCtl(channels_[fd], EPOLL_CTL_MOD);
    }
}

void EventPoller::RemoveChannel(int fd)
{
    eventLoop_->AssertInLoopThread();

    if (channels_.count(fd) == 0) {
        LOG_WARN("Can't find channel %{public}i in poller %{public}i.", fd, epollFd_.Get());
        return;
    }

    EpollCtl(channels_[fd], EPOLL_CTL_DEL);
    channels_.erase(fd);
}
} // namespace oewm
