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

#include "timestamp.h"

#include <chrono>
#include <iomanip>
#include <sstream>

namespace FT {
namespace detail {
// Class std::chrono::steady_clock represents a monotonic clock. The time points of this clock cannot decrease as
// physical time moves forward and the time between ticks of this clock is constant. This clock is not related to wall
// clock time (for example, it can be time since last reboot), and is most suitable for measuring intervals.
TimeType SteadyClockMicroSeconds()
{
    auto tp = std::chrono::time_point_cast<std::chrono::microseconds>(std::chrono::steady_clock::now());
    return tp.time_since_epoch().count();
}

// Class std::chrono::system_clock represents the system-wide real time wall clock.
// It may not be monotonic: on most systems, the system time can be adjusted at any mo/ment.
TimeType SystemClockMicroSeconds()
{
    auto tp = std::chrono::time_point_cast<std::chrono::microseconds>(std::chrono::system_clock::now());
    return tp.time_since_epoch().count();
}
} // namespace detail

TimeType TimeStamp::systemStartTimePoint = detail::SystemClockMicroSeconds() - detail::SteadyClockMicroSeconds();

TimeStamp TimeStamp::Now() noexcept
{
    return TimeStamp(detail::SteadyClockMicroSeconds() + systemStartTimePoint);
}

TimeStamp TimeStamp::SystemStartTime() noexcept
{
    return TimeStamp(systemStartTimePoint);
}

std::string TimeStamp::ToString(TimePrecision precision) const noexcept
{
    TimeType seconds = microSecondsSinceEpoch_ / MICRO_SECS_PER_SECOND;
    TimeType micros = microSecondsSinceEpoch_ % MICRO_SECS_PER_SECOND;

    std::stringstream ss;
    ss << seconds << "." << std::setfill('0');
    switch (precision) {
        case TimePrecision::SECOND: {
            ss << std::setw(3) << 0;
            break;
        }
        case TimePrecision::MILLI: {
            TimeType millis = micros / MICRO_SECS_PER_MILLISECOND;
            ss << std::setw(3) << millis;
            break;
        }
        case TimePrecision::MICRO: {
            ss << std::setw(6) << micros;
            break;
        }
        default:
            break;
    }

    return ss.str();
}

std::string TimeStamp::ToFormattedString(TimePrecision precision) const noexcept
{
    TimeType seconds = microSecondsSinceEpoch_ / MICRO_SECS_PER_SECOND;
    TimeType micros = microSecondsSinceEpoch_ % MICRO_SECS_PER_SECOND;

    std::stringstream ss;
    ss << std::put_time(::localtime(&seconds), "%F %X") << "." << std::setfill('0');
    switch (precision) {
        case TimePrecision::SECOND: {
            ss << std::setw(3) << 0;
            break;
        }
        case TimePrecision::MILLI: {
            TimeType millis = micros / MICRO_SECS_PER_MILLISECOND;
            ss << std::setw(3) << millis;
            break;
        }
        case TimePrecision::MICRO: {
            ss << std::setw(6) << micros;
            break;
        }
        default:
            break;
    }

    return ss.str();
}
} // namespace FT
