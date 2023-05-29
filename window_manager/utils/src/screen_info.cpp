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

#include "screen_info.h"

namespace OHOS::Rosen {
bool ScreenInfo::Marshalling(Parcel &parcel) const
{
    bool res = parcel.WriteString(name_) && parcel.WriteUint64(id_) &&
        parcel.WriteUint32(virtualWidth_) && parcel.WriteUint32(virtualHeight_) &&
        parcel.WriteFloat(virtualPixelRatio_) && parcel.WriteUint64(lastParent_) && parcel.WriteUint64(parent_) &&
        parcel.WriteBool(isScreenGroup_) && parcel.WriteUint32(static_cast<uint32_t>(rotation_)) &&
        parcel.WriteUint32(static_cast<uint32_t>(orientation_)) &&
        parcel.WriteUint32(static_cast<uint32_t>(type_)) &&
        parcel.WriteUint32(modeId_) && parcel.WriteUint32(static_cast<uint32_t>(modes_.size()));
    if (!res) {
        return false;
    }
    for (uint32_t modeIndex = 0; modeIndex < modes_.size(); modeIndex++) {
        if (parcel.WriteUint32(modes_[modeIndex]->height_) &&
            parcel.WriteUint32(modes_[modeIndex]->width_) &&
            parcel.WriteUint32(modes_[modeIndex]->refreshRate_)) {
            continue;
        }
        return false;
    }
    return true;
}

ScreenInfo* ScreenInfo::Unmarshalling(Parcel &parcel)
{
    ScreenInfo* info = new(std::nothrow) ScreenInfo();
    if (info == nullptr) {
        return info;
    }
    bool res = info->InnerUnmarshalling(parcel);
    if (res) {
        return info;
    }
    delete info;
    return nullptr;
}

bool ScreenInfo::InnerUnmarshalling(Parcel& parcel)
{
    uint32_t size = 0;
    uint32_t rotation;
    uint32_t orientation;
    uint32_t type;
    name_ = parcel.ReadString();
    bool res1 = parcel.ReadUint64(id_) &&
        parcel.ReadUint32(virtualWidth_) && parcel.ReadUint32(virtualHeight_) &&
        parcel.ReadFloat(virtualPixelRatio_) && parcel.ReadUint64(lastParent_) && parcel.ReadUint64(parent_) &&
        parcel.ReadBool(isScreenGroup_) && parcel.ReadUint32(rotation) &&
        parcel.ReadUint32(orientation) && parcel.ReadUint32(type) &&
        parcel.ReadUint32(modeId_) && parcel.ReadUint32(size);
    if (!res1) {
        return false;
    }
    modes_.clear();
    for (uint32_t modeIndex = 0; modeIndex < size; modeIndex++) {
        sptr<SupportedScreenModes> mode = new(std::nothrow) SupportedScreenModes();
        if (mode == nullptr) {
            return false;
        }
        if (parcel.ReadUint32(mode->height_) &&
            parcel.ReadUint32(mode->width_) &&
            parcel.ReadUint32(mode->refreshRate_)) {
            modes_.push_back(mode);
        } else {
            return false;
        }
    }
    rotation_ = static_cast<Rotation>(rotation);
    orientation_ = static_cast<Orientation>(orientation);
    type_ = static_cast<ScreenType>(type);
    return true;
}
} // namespace OHOS::Rosen