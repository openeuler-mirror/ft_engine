/*
 * Copyright (c) 2021-2022 Huawei Device Co., Ltd.
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

#ifndef FOUNDATION_DMSERVER_SCREEN_GROUP_INFO_H
#define FOUNDATION_DMSERVER_SCREEN_GROUP_INFO_H

#include <parcel.h>

#include "screen_group.h"
#include "screen_info.h"

namespace OHOS::Rosen {
class ScreenGroupInfo : public ScreenInfo {
friend class AbstractScreenGroup;
public:
    ScreenGroupInfo() = default;
    ~ScreenGroupInfo() = default;
    WM_DISALLOW_COPY_AND_MOVE(ScreenGroupInfo);

    virtual bool Marshalling(Parcel& parcel) const override;
    static ScreenGroupInfo* Unmarshalling(Parcel& parcel);

    DEFINE_VAR_FUNC_GET_SET(std::vector<ScreenId>, Children, children);
    DEFINE_VAR_FUNC_GET_SET(std::vector<Point>, Position, position);
    DEFINE_VAR_DEFAULT_FUNC_GET(ScreenCombination, Combination, combination, ScreenCombination::SCREEN_ALONE);
private:
    bool InnerUnmarshalling(Parcel& parcel);
};
} // namespace OHOS::Rosen
#endif // FOUNDATION_DMSERVER_SCREEN_GROUP_INFO_H