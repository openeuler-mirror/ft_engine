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

#include "timer.h"

#include <atomic>

namespace oewm {
namespace detail {
uint64_t GenSequenceId()
{
    static std::atomic<uint64_t> id(0);
    return id++;
}
} // namespace detail

Timer::Timer(TimerCallback callback, TimeStamp expireTime, TimeType interval)
    : cb_(std::move(callback)),
      expireTime_(expireTime),
      interval_(interval),
      repeat_(interval > 0),
      id_(detail::GenSequenceId(), this)
{}

void Timer::Execute()
{
    if (cb_ != nullptr) {
        cb_();
    }
}

void Timer::Restart(TimeStamp now)
{
    ASSERT(IsRepeat());
    expireTime_ = TimeAdd(now, interval_);
}
} // namespace oewm
