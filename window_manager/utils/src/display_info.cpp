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

#include "display_info.h"

#include <new>
#include <parcel.h>


namespace OHOS::Rosen {
bool DisplayInfo::Marshalling(Parcel &parcel) const
{
    return parcel.WriteString(name_) && parcel.WriteUint64(id_) && parcel.WriteUint32(static_cast<uint32_t>(type_)) &&
        parcel.WriteInt32(width_) && parcel.WriteInt32(height_) &&
        parcel.WriteUint32(refreshRate_) && parcel.WriteUint64(screenId_) &&
        parcel.WriteFloat(virtualPixelRatio_) && parcel.WriteFloat(xDpi_) && parcel.WriteFloat(yDpi_) &&
        parcel.WriteUint32(static_cast<uint32_t>(rotation_)) &&
        parcel.WriteUint32(static_cast<uint32_t>(orientation_)) &&
        parcel.WriteInt32(offsetX_) && parcel.WriteInt32(offsetY_) &&
        parcel.WriteUint32(static_cast<uint32_t>(displayState_)) &&
        parcel.WriteBool(waterfallDisplayCompressionStatus_);
}

DisplayInfo *DisplayInfo::Unmarshalling(Parcel &parcel)
{
    DisplayInfo *displayInfo = new(std::nothrow) DisplayInfo();
    if (displayInfo == nullptr) {
        return nullptr;
    }
    uint32_t type = (uint32_t)DisplayType::DEFAULT;
    uint32_t rotation;
    uint32_t orientation;
    uint32_t displayState;
    bool res = parcel.ReadString(displayInfo->name_) &&
        parcel.ReadUint64(displayInfo->id_) && parcel.ReadUint32(type) &&
        parcel.ReadInt32(displayInfo->width_) && parcel.ReadInt32(displayInfo->height_) &&
        parcel.ReadUint32(displayInfo->refreshRate_) && parcel.ReadUint64(displayInfo->screenId_) &&
        parcel.ReadFloat(displayInfo->virtualPixelRatio_) &&
        parcel.ReadFloat(displayInfo->xDpi_) && parcel.ReadFloat(displayInfo->yDpi_) &&
        parcel.ReadUint32(rotation) && parcel.ReadUint32(orientation) &&
        parcel.ReadInt32(displayInfo->offsetX_) && parcel.ReadInt32(displayInfo->offsetY_) &&
        parcel.ReadUint32(displayState) && parcel.ReadBool(displayInfo->waterfallDisplayCompressionStatus_);
    if (!res) {
        delete displayInfo;
        return nullptr;
    }
    displayInfo->type_ = (DisplayType)type;
    displayInfo->rotation_ = static_cast<Rotation>(rotation);
    displayInfo->orientation_ = static_cast<Orientation>(orientation);
    displayInfo->displayState_ = static_cast<DisplayState>(displayState);
    return displayInfo;
}
} // namespace OHOS::Rosen