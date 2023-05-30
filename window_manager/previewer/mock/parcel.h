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

#ifndef OHOS_UTILS_PARCEL_H
#define OHOS_UTILS_PARCEL_H

#include <string>
#include <vector>
#include "refbase.h"

namespace OHOS {
class Parcel;

class Parcelable : public virtual RefBase {
public:
    virtual ~Parcelable() = default;

    Parcelable();

    virtual bool Marshalling(Parcel &parcel) const = 0;
};

class Parcel {
public:
    Parcel();

    virtual ~Parcel();

    bool WriteInt32(int32_t value);

    bool ReadInt32(int32_t &value);

    int32_t ReadInt32();

    bool WriteUint32(uint32_t value);

    bool ReadUint32(uint32_t &value);

    uint32_t ReadUint32();

    bool WriteUint64(uint64_t value);

    uint64_t ReadUint64();

    bool WriteFloat(float value);

    float ReadFloat();

    bool WriteString(const std::string &value);

    const std::string ReadString();

    bool WriteBool(bool value);

    bool ReadBool();
    
    template<typename T>
    bool WriteObject(const sptr<T> &object)
    {
        return true;
    }
    
    template <typename T>
    sptr<T> ReadObject()
    {
        return nullptr;
    }
};
} // namespace OHOS
#endif // OHOS_UTILS_PARCEL_H
