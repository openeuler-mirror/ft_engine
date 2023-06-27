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

#ifndef OHOS_ABILITY_BASE_WANT_H
#define OHOS_ABILITY_BASE_WANT_H

#include "parcel.h"
#include "iremote_object.h"

namespace OHOS {
namespace AAFwk {
class Want final : public Parcelable {
public:
    std::string GetAction() const
    {
        return "";
    }
    Want &SetAction(const std::string &action)
    {
        return *this;
    }
    Want& SetParam(const std::string& key, const sptr<IRemoteObject>& remoteObject)
    {
        return *this;
    }
    Want &SetParam(const std::string &key, int value)
    {
        return *this;
    }
    Want &SetParam(const std::string &key, const std::string &value)
    {
        return *this;
    }
    bool Marshalling(Parcel &parcel) const override
    {
        return true;
    }
};
}  // namespace AAFwk
}  // namespace OHOS

#endif  // OHOS_ABILITY_BASE_WANT_H