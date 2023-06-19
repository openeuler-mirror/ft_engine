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

#include "event_channel.h"

#include "event_loop.h"
#include "log.h"

namespace oewm {
EventChannel::EventChannel(int fd, EventLoop *eventLoop) : fd_(fd), eventLoop_(eventLoop)
{
    if (eventLoop_ == nullptr) {
        LOG_FATAL("EventChannel::ctor: EventLoop is null!");
    }
}

EventChannel::~EventChannel() noexcept {}

void EventChannel::AssertInLoopThread() const
{
    ASSERT(eventLoop_ != nullptr);
    eventLoop_->AssertInLoopThread();
}

void EventChannel::Remove()
{
    ASSERT(HasNoEvent());
    addedToLoop_ = false;
    eventLoop_->RemoveChannel(fd_);
}

void EventChannel::EnableReading(bool toUpdate)
{
    listeningEvents_ |= ECast(EventType::READ_EVENT);

    if (toUpdate) {
        Update();
    }
}

void EventChannel::DisableReading(bool toUpdate)
{
    listeningEvents_ &= (~ECast(EventType::READ_EVENT));

    if (toUpdate) {
        Update();
    }
}

void EventChannel::EnableWriting(bool toUpdate)
{
    listeningEvents_ |= ECast(EventType::WRITE_EVENT);

    if (toUpdate) {
        Update();
    }
}

void EventChannel::DisableWriting(bool toUpdate)
{
    listeningEvents_ &= (~ECast(EventType::WRITE_EVENT));

    if (toUpdate) {
        Update();
    }
}

void EventChannel::DisableAll(bool toRemove)
{
    listeningEvents_ = ECast(EventType::NONE);

    if (toRemove) {
        Update();
    }
}

void EventChannel::Update()
{
    if (HasNoEvent()) {
        Remove();
    } else {
        addedToLoop_ = true;
        eventLoop_->UpdateChannel(this);
    }
}

void EventChannel::HandleEvent(TimeStamp receivedTime)
{
    AssertInLoopThread();

    if (tied_) {
        // make sure the owner object will not be destroyed.
        std::shared_ptr<void> guard = ownerObj_.lock();
        if (guard == nullptr) {
            LOG_WARN("EventChannel::HandleEvent: the owner object was dead, do nothing.");
            return;
        }
        HandleEventInner(receivedTime);
    } else {
        HandleEventInner(receivedTime);
    }
}

void EventChannel::HandleEventInner(TimeStamp receivedTime)
{
    if ((receivedEvents_ & EPOLLHUP) && !(receivedEvents_ & EPOLLIN)) {
        LOG_DEBUG("close event in channel %{public}i.", fd_);
        if (closeCallback_ != nullptr) {
            closeCallback_();
            return;
        }
    }

    if (receivedEvents_ & EPOLLERR) {
        LOG_DEBUG("error event in channel %{public}i.", fd_);
        if (errorCallback_ != nullptr) {
            errorCallback_();
            return;
        }
    }

    if (receivedEvents_ & (EPOLLIN | EPOLLPRI | EPOLLRDHUP)) {
        // LOG_DEBUG("read event in channel %{public}i.", fd_);
        if (readCallback_ != nullptr) {
            readCallback_(receivedTime);
        }
    }

    if (receivedEvents_ & EPOLLOUT) {
        LOG_DEBUG("write event in channel %{public}i.", fd_);
        if (writeCallback_ != nullptr) {
            writeCallback_();
        }
    }
}
} // namespace oewm
