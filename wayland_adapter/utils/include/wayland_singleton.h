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

#pragma once

#include "types.h"
#include "noncopyable_hal.h"

#include <mutex>

namespace FT {
template <typename T>
class Singleton : NonCopyable {
public:
    Singleton() = default;
    virtual ~Singleton() noexcept = default;

    static T &GetInstance()
    {
        std::call_once(onceFlag_, CreateInstance);
        ASSERT(instance_ != nullptr);
        return *instance_;
    }

private:
    template <typename U>
    struct HasFuncDontDestroyMe {
        template <typename V>
        static char check(decltype(&V::DontDestroyMe));
        template <typename V>
        static int32_t check(...);
        const static bool value = sizeof(check<U>(0)) == sizeof(char);
    };

    static void CreateInstance()
    {
        instance_ = new T();
        if (!HasFuncDontDestroyMe<T>::value) {
            ::atexit(DestoryInstance);
        }
    }
    static void DestoryInstance()
    {
        static_assert(sizeof(T) != 0, "Singleton type T must be complete type.");
        delete instance_;
        instance_ = nullptr;
    }
    static std::once_flag onceFlag_;
    static T *instance_;
};

template <typename T>
std::once_flag Singleton<T>::onceFlag_;

template <typename T>
T *Singleton<T>::instance_ = nullptr;

#define DECLARE_SINGLETON(T)                                                                                           \
private:                                                                                                               \
    friend class Singleton<T>;
} // namespace FT
