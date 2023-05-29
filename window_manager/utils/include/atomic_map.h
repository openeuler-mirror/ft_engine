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
#ifndef ATOMIC_MAP_H
#define ATOMIC_MAP_H

#include <atomic>
#include <map>
#include "nocopyable.h"

namespace OHOS {
namespace Rosen {
template<class Key, class Value>
class AtomicMap {
public:
    void insert(const std::pair<Key, Value>& kv)
    {
        locked();
        data_.insert(kv);
        unlocked();
    }

    void erase(const Key& key)
    {
        locked();
        data_.erase(key);
        unlocked();
    }

    auto find(Key k)
    {
        locked();
        auto key = data_.find(k);
        unlocked();
        return key;
    }

    int count(Key k)
    {
        locked();
        int size = data_.count(k);
        unlocked();
        return size;
    }

    bool isExistAndRemove(Key k, uint32_t value)
    {
        locked();
        if (data_.count(k) <= 0) {
            unlocked();
            return false;
        }
        if (data_[k] == value) {
            data_.erase(k);
            unlocked();
            return true;
        }
        unlocked();
        return false;
    }

    bool isExist(Key k, uint32_t value)
    {
        locked();
        if (data_.count(k) <= 0) {
            unlocked();
            return false;
        }
        if (data_[k] == value) {
            unlocked();
            return true;
        }
        unlocked();
        return false;
    }
private:
    void locked()
    {
        bool expect = false;
        while (!isWritingOrReading_.compare_exchange_weak(expect, true, std::memory_order_relaxed)) {
            expect = false;
        }
    }

    void unlocked()
    {
        isWritingOrReading_.store(false);
    }

    std::map<Key, Value> data_;
    std::atomic<bool> isWritingOrReading_ { false };
};
} // Rosen
} // OHOS
#endif // ATOMIC_MAP_H