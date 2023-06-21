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

#ifndef FOUNDATION_MARSHALLING_HELPER_H
#define FOUNDATION_MARSHALLING_HELPER_H

#include <climits>
#include <parcel.h>

namespace OHOS::Rosen {
class MarshallingHelper : public Parcelable {
public:
    MarshallingHelper() = delete;
    template<class T>
    static bool MarshallingVectorParcelableObj(Parcel &parcel, const std::vector<sptr<T>>& data)
    {
        if (data.size() > INT_MAX) {
            return false;
        }
        if (!parcel.WriteInt32(static_cast<int32_t>(data.size()))) {
            return false;
        }
        for (const auto &v : data) {
            if (!parcel.WriteParcelable(v)) {
                return false;
            }
        }
        return true;
    }

    template<class T>
    static bool UnmarshallingVectorParcelableObj(Parcel &parcel, std::vector<sptr<T>>& data)
    {
        int32_t len = parcel.ReadInt32();
        if (len < 0) {
            return false;
        }

        size_t readAbleSize = parcel.GetReadableBytes();
        size_t size = static_cast<size_t>(len);
        if ((size > readAbleSize) || (size > data.max_size())) {
            return false;
        }
        data.resize(size);
        if (data.size() < size) {
            return false;
        }
        size_t minDesireCapacity = sizeof(int32_t);
        for (size_t i = 0; i < size; i++) {
            readAbleSize = parcel.GetReadableBytes();
            if (minDesireCapacity > readAbleSize) {
                return false;
            }
            data[i] = parcel.ReadParcelable<T>();
        }
        return true;
    }

    template<class T>
    static bool MarshallingVectorObj(Parcel &parcel, const std::vector<T>& data,
        std::function<bool(Parcel&, const T&)> func)
    {
        if (data.size() > INT_MAX) {
            return false;
        }
        if (func == nullptr) {
            return false;
        }
        if (!parcel.WriteInt32(static_cast<int32_t>(data.size()))) {
            return false;
        }
        for (const auto &v : data) {
            if (!func(parcel, v)) {
                return false;
            }
        }
        return true;
    }

    template<class T>
    static bool UnmarshallingVectorObj(Parcel &parcel, std::vector<T>& data, std::function<bool(Parcel&, T&)> func)
    {
        if (func == nullptr) {
            return false;
        }
        int32_t len = parcel.ReadInt32();
        if (len < 0) {
            return false;
        }

        size_t readAbleSize = parcel.GetReadableBytes();
        size_t size = static_cast<size_t>(len);
        if ((size > readAbleSize) || (size > data.max_size())) {
            return false;
        }
        data.resize(size);
        if (data.size() < size) {
            return false;
        }

        for (size_t i = 0; i < size; i++) {
            if (!func(parcel, data[i])) {
                return false;
            }
        }
        return true;
    }
};
} // namespace OHOS::Rosen
#endif // FOUNDATION_MARSHALLING_HELPER_H