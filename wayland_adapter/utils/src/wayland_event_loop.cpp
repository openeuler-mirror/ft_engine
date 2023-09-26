/*
 * Copyright (c) 2023 Huawei Technologies Co., Ltd.
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

#include "wayland_event_loop.h"

namespace FT {
namespace Wayland {

WaylandEventLoop::WaylandEventLoop()
{
    loop_ = std::make_shared<EventLoop>();
}

WaylandEventLoop::~WaylandEventLoop()
{
    loop_ = nullptr;
}

void WaylandEventLoop::QueueToLoop(Functor func)
{
    if (loop_) {
        loop_->QueueToLoop(func);
    }
}

EventLoop *WaylandEventLoop::GetEventLoopPtr()
{
    if (loop_) {
        return loop_.get();
    }
    return nullptr;
}

void WaylandEventLoop::Start()
{
    if (loop_) {
        loop_->Start();
    }
}
} // namespace Wayland
} // namespace FT
