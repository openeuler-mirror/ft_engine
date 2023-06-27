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

#include "types.h"

namespace oewm {
constexpr int MILLI_SECS_PER_SECOND = 1000;
constexpr int MICRO_SECS_PER_MILLISECOND = 1000;
constexpr int MICRO_SECS_PER_SECOND = MICRO_SECS_PER_MILLISECOND * MILLI_SECS_PER_SECOND;
constexpr int NANO_SECS_PER_MICROSECOND = 1000;
constexpr int NANO_SECS_PER_MILLISECOND = NANO_SECS_PER_MICROSECOND * MICRO_SECS_PER_MILLISECOND;
constexpr int NANO_SECS_PER_SECOND = NANO_SECS_PER_MILLISECOND * MILLI_SECS_PER_SECOND;

enum class TimePrecision { SECOND, MILLI, MICRO };

// copyable
class TimeStamp {
public:
    TimeStamp() noexcept = default;

    constexpr explicit TimeStamp(TimeType microSecondsSinceEpoch) noexcept
        : microSecondsSinceEpoch_(microSecondsSinceEpoch)
    {}

    static TimeStamp SystemStartTime() noexcept;
    static TimeStamp Now() noexcept;
    static TimeStamp Invalid() noexcept
    {
        return TimeStamp();
    }

    TimeType Get() const noexcept
    {
        return microSecondsSinceEpoch_;
    }
    TimeType Micros() const noexcept
    {
        return Get();
    }
    TimeType Nanos() const noexcept
    {
        return Micros() * NANO_SECS_PER_MICROSECOND;
    }
    TimeType Millis() const noexcept
    {
        return Micros() / MICRO_SECS_PER_MILLISECOND;
    }
    TimeType Seconds() const noexcept
    {
        return Millis() / MILLI_SECS_PER_SECOND;
    }

    std::string ToString(TimePrecision precision = TimePrecision::MILLI) const noexcept;
    std::string ToFormattedString(TimePrecision precision = TimePrecision::MILLI) const noexcept;

private:
    static TimeType systemStartTimePoint;
    TimeType microSecondsSinceEpoch_ = 0;
};

inline bool operator==(TimeStamp lhs, TimeStamp rhs)
{
    return lhs.Get() == rhs.Get();
}

inline bool operator<(TimeStamp lhs, TimeStamp rhs)
{
    return lhs.Get() < rhs.Get();
}

inline bool operator<=(TimeStamp lhs, TimeStamp rhs)
{
    return lhs.Get() <= rhs.Get();
}

inline bool operator>(TimeStamp lhs, TimeStamp rhs)
{
    return lhs.Get() > rhs.Get();
}

inline bool operator>=(TimeStamp lhs, TimeStamp rhs)
{
    return lhs.Get() >= rhs.Get();
}

inline bool operator!=(TimeStamp lhs, TimeStamp rhs)
{
    return lhs.Get() != rhs.Get();
}

// return two timeStamps' diff in micros
inline int64_t TimeDiff(TimeStamp lhs, TimeStamp rhs)
{
    return lhs.Get() - rhs.Get();
}

// @micros: add micros seconds to timestamp t.
// @return: timestamp t + micro seconds
inline TimeStamp TimeAdd(TimeStamp t, TimeType micros)
{
    return TimeStamp(t.Micros() + micros);
}
} // namespace oewm
