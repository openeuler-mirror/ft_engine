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

#ifndef COMMON_EVENT_DATA_H
#define COMMON_EVENT_DATA_H

#include "parcel.h"
#include "want.h"

namespace OHOS {
namespace EventFwk {
using Want = OHOS::AAFwk::Want;

class CommonEventData : public Parcelable {
public:
    void SetWant(const Want &want) {}
    const Want &GetWant() const
    {
        static Want want;
        return want;
    }
    int32_t GetCode() const
    {
        return 0;
    }
    bool Marshalling(Parcel &parcel) const override
    {
        return true;
    }
};
}  // namespace EventFwk
}  // namespace OHOS

#endif  // COMMON_EVENT_DATA_H