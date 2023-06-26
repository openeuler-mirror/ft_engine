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

#include <cstring>
#include <random>
#include <string>
#include <type_traits>
#include <unistd.h>

#define UNUSED(val) (void)val;
#ifndef NDEBUG
#include <cassert>
#define ASSERT(exp) assert((exp))
#else // NDEBUG
#define ASSERT(exp) UNUSED((exp))
#endif // NDEBUG

#ifdef __cplusplus
#define OE_LIKELY(x) (__builtin_expect(!!(x), true))
#define OE_UNLIKELY(x) (__builtin_expect(!!(x), false))
#else
#define OE_LIKELY(x) (__builtin_expect(!!(x), 1))
#define OE_UNLIKELY(x) (__builtin_expect(!!(x), 0))
#endif

namespace oewm {
template <typename EnumType>
inline constexpr typename std::underlying_type<EnumType>::type ECast(EnumType e)
{
    return static_cast<typename std::underlying_type<EnumType>::type>(e);
}

using ThreadId = pid_t;
using ProcessId = pid_t;
using FileSize = std::uintmax_t;
using TimeType = int64_t;

inline constexpr bool IsInvalidFd(int fd)
{
    return fd < 0;
}

constexpr int INVALID_FD = -1;

template <typename Derived, typename Base>
inline Derived DownCast(Base base)
{
    return static_cast<Derived>(base);
}

template <typename... Args>
inline void AppendFormat(std::string &out, const char *fmt, Args &&...args)
{
    static constexpr int STRING_BUFFER_SIZE = 4096;
    char buf[STRING_BUFFER_SIZE] = {0};
    ::snprintf(buf, sizeof(buf) - 1, fmt, args...);
    out += buf;
}

inline std::string ErrnoToString(int errorNum)
{
    static constexpr int BUFFER_LEN = 256;
    char buf[BUFFER_LEN] = {0};
    return strerror_r(errorNum, buf, sizeof(buf));
}

inline double Random(double low, double high)
{
    static std::random_device rd;
    std::mt19937 mt(rd());
    std::uniform_real_distribution<double> dist(low, high);
    return dist(mt);
}

template <typename T>
// Remove const, volatile and reference qualifiers from type T
using RemoveCVRefType = std::remove_cv_t<std::remove_reference_t<T>>;

// Extract type traits of FromType and ToType.
template <typename FromType, typename ToType>
struct ConvertTypeTraits {
    // FromType traits.
    using RawFromType = RemoveCVRefType<FromType>;
    using FromTypeLimits = std::numeric_limits<RawFromType>;
    constexpr static auto fromTypeMax = FromTypeLimits::max();
    constexpr static auto fromTypeMin = FromTypeLimits::lowest();

    // ToType traits.
    using RawToType = RemoveCVRefType<ToType>;
    using ToTypeLimits = std::numeric_limits<RawToType>;
    constexpr static auto toTypeMax = ToTypeLimits::max();
    constexpr static auto toTypeMin = ToTypeLimits::lowest();

    // CommonType traits.
    using CommonType = std::common_type_t<RawFromType, RawToType>;
    using CommonTypeLimits = std::numeric_limits<CommonType>;
    constexpr static auto commonTypeMax = CommonTypeLimits::max();
    constexpr static auto commonTypeMin = CommonTypeLimits::lowest();
    constexpr static bool isCommonTypeInteger = CommonTypeLimits::is_integer;
};

// Ensure that a value of FromType can be represented for ToType,
// and clamp the FromType value to the ToType's range if necessary.
template <typename ToType, typename FromType>
inline constexpr RemoveCVRefType<ToType> clamp(FromType value)
{
    using TypeTraits = ConvertTypeTraits<FromType, ToType>;
    using CommonType = typename TypeTraits::CommonType;
    constexpr auto commonClampMax =
        std::min(static_cast<CommonType>(TypeTraits::fromTypeMax), static_cast<CommonType>(TypeTraits::toTypeMax));
    constexpr auto commonClampMin =
        std::max(static_cast<CommonType>(TypeTraits::fromTypeMin), static_cast<CommonType>(TypeTraits::toTypeMin));
    constexpr bool needClamp =
        (commonClampMax != TypeTraits::fromTypeMax) || (commonClampMin != TypeTraits::fromTypeMin);
    if constexpr (!needClamp) {
        return static_cast<typename TypeTraits::RawToType>(value);
    }

    // Do clamp.
    const auto toClampMax = static_cast<typename TypeTraits::RawToType>(commonClampMax);
    const auto toClampMin = static_cast<typename TypeTraits::RawToType>(commonClampMin);
    if (static_cast<typename TypeTraits::RawToType>(value) <= toClampMin) {
        return toClampMin;
    }
    if (static_cast<typename TypeTraits::RawToType>(value) >= toClampMax) {
        return toClampMax;
    }

    return static_cast<typename TypeTraits::RawToType>(value);
}

template <typename T>
inline void HashCombineOne(size_t &combinedHash, const T &value)
{
    combinedHash = (sizeof(size_t) * 8 - 1) * combinedHash + std::hash<T>{}(value);
}

template <typename... Args>
inline size_t HashCombine(const Args &...args)
{
    size_t hash = 0;
    (HashCombineOne(hash, args), ...);
    return hash;
}
} // namespace oewm
