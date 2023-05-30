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

#include "parcel.h"
namespace OHOS {
Parcelable::Parcelable()
{
}

Parcel::Parcel()
{
}

Parcel::~Parcel()
{
}

bool Parcel::WriteInt32(int32_t value)
{
    return true;
}

bool Parcel::ReadInt32(int32_t &value)
{
    return true;
}

int32_t Parcel::ReadInt32()
{
    return 0;
}

bool Parcel::WriteUint32(uint32_t value)
{
    return true;
}

bool Parcel::ReadUint32(uint32_t &value)
{
    return true;
}

uint32_t Parcel::ReadUint32()
{
    return 0;
}

bool Parcel::WriteUint64(uint64_t value)
{
    return true;
}

uint64_t Parcel::ReadUint64()
{
    return 0;
}

bool Parcel::WriteFloat(float value)
{
    return true;
}

float Parcel::ReadFloat()
{
    return 0;
}

bool Parcel::WriteString(const std::string &value)
{
    return true;
}

const std::string Parcel::ReadString()
{
    return "";
}

bool Parcel::WriteBool(bool value)
{
    return true;
}

bool Parcel::ReadBool()
{
    return true;
}
}  // namespace OHOS
