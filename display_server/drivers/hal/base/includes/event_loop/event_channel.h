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

#include <atomic>
#include <functional>
#include <memory>
#include <mutex>
#include <sys/epoll.h>

#include "noncopyable.h"
#include "timestamp.h"

namespace oewm {
using EventCallback = std::function<void()>;
using ReadCallback = std::function<void(TimeStamp)>;

class EventPoller;
class EventLoop;

// use EPOLL_EVENTS to define channel event types.
enum class EventType : uint32_t {
    NONE = 0x0,
    READ_EVENT = EPOLLIN | EPOLLPRI,
    WRITE_EVENT = EPOLLOUT,
    EDGE_EVENT = EPOLLET,
};

// The EventChannel class not own the fd, so we must make sure that
// the fd is valid when an EventChannel object holds it.
class EventChannel : NonCopyable {
public:
    EventChannel(int fd, EventLoop *eventLoop);
    ~EventChannel() noexcept;

    void HandleEvent(TimeStamp receivedTime);

    int Fd() const
    {
        return fd_;
    }
    EventLoop *GetOwnerLoop() const
    {
        return eventLoop_;
    }

    // not thread safe.
    void SetReadCallback(ReadCallback cb)
    {
        readCallback_ = std::move(cb);
    }
    // not thread safe.
    void SetWriteCallback(EventCallback cb)
    {
        writeCallback_ = std::move(cb);
    }
    // not thread safe.
    void SetErrorCallback(EventCallback cb)
    {
        errorCallback_ = std::move(cb);
    }
    // not thread safe.
    void SetCloseCallback(EventCallback cb)
    {
        closeCallback_ = std::move(cb);
    }

    bool HasNoEvent() const
    {
        return ListeningEvents() == ECast(EventType::NONE);
    }
    bool IsWriting() const
    {
        return ListeningEvents() & ECast(EventType::WRITE_EVENT);
    }
    bool IsReading() const
    {
        return ListeningEvents() & ECast(EventType::READ_EVENT);
    }

    // @toUpdate: whether to update the channel in poller or not, true by default.
    void EnableReading(bool toUpdate = true);
    // @toUpdate: whether to update the channel in poller or not, true by default.
    void DisableReading(bool toUpdate = true);
    // @toUpdate: whether to update the channel in poller or not, true by default.
    void EnableWriting(bool toUpdate = true);
    // @toUpdate: whether to update the channel in poller or not, true by default.
    void DisableWriting(bool toUpdate = true);
    // @toRemove: whether to remove the channel in poller or not, true by default.
    void DisableAll(bool toRemove = true);

    void Update();
    void Remove();

    // the owner object should call this function
    // to prevent itself from being destroyed in handleEvent.
    void Tie(std::weak_ptr<void> ownerObj)
    {
        ownerObj_ = ownerObj;
        tied_ = true;
    }

protected:
    // will abort if not in loop thread.
    void AssertInLoopThread() const;

    void HandleEventInner(TimeStamp receivedTime);

    friend class EventPoller;
    uint32_t ListeningEvents() const
    {
        return listeningEvents_;
    }
    void SetReceivedEvents(uint32_t events)
    {
        receivedEvents_ = events;
    }

    int fd_ = -1;
    EventLoop *eventLoop_ = nullptr;
    std::atomic<bool> addedToLoop_{false};

    uint32_t listeningEvents_ = ECast(EventType::NONE);
    uint32_t receivedEvents_ = ECast(EventType::NONE);

    ReadCallback readCallback_;
    EventCallback writeCallback_;
    EventCallback errorCallback_;
    EventCallback closeCallback_;

    std::weak_ptr<void> ownerObj_;
    std::atomic<bool> tied_{false};
};
} // namespace oewm
