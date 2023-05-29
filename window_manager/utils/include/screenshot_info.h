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

#ifndef FOUNDATION_DMSERVER_SNAPSHOT_INFO_H
#define FOUNDATION_DMSERVER_SNAPSHOT_INFO_H

#include <cstdint>
#include <parcel.h>

#include "class_var_definition.h"
#include "dm_common.h"
#include "noncopyable.h"

namespace OHOS::Rosen {
class ScreenshotInfo : public Parcelable {
public:
    ScreenshotInfo() = default;
    ~ScreenshotInfo() = default;

    virtual bool Marshalling(Parcel& parcel) const override;
    static ScreenshotInfo *Unmarshalling(Parcel& parcel);

    DEFINE_VAR_DEFAULT_FUNC_GET_SET(std::string, Trigger, trigger, "");
    DEFINE_VAR_DEFAULT_FUNC_GET_SET(DisplayId, DisplayId, displayId, DISPLAY_ID_INVALID);
};
} // namespace OHOS::Rosen
#endif // FOUNDATION_DMSERVER_DISPLAY_INFO_H