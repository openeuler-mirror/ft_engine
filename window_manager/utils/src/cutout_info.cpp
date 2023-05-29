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

#include "cutout_info.h"

namespace OHOS::Rosen {
CutoutInfo::CutoutInfo(const std::vector<DMRect>& boundingRects,
    WaterfallDisplayAreaRects waterfallDisplayAreaRects) : waterfallDisplayAreaRects_(waterfallDisplayAreaRects),
    boundingRects_(boundingRects)
{
}

bool CutoutInfo::Marshalling(Parcel &parcel) const
{
    return parcel.WriteInt32(waterfallDisplayAreaRects_.left.posX_) &&
        parcel.WriteInt32(waterfallDisplayAreaRects_.left.posY_) &&
        parcel.WriteUint32(waterfallDisplayAreaRects_.left.width_) &&
        parcel.WriteUint32(waterfallDisplayAreaRects_.left.height_) &&
        parcel.WriteInt32(waterfallDisplayAreaRects_.top.posX_) &&
        parcel.WriteInt32(waterfallDisplayAreaRects_.top.posY_) &&
        parcel.WriteUint32(waterfallDisplayAreaRects_.top.width_) &&
        parcel.WriteUint32(waterfallDisplayAreaRects_.top.height_) &&
        parcel.WriteInt32(waterfallDisplayAreaRects_.right.posX_) &&
        parcel.WriteInt32(waterfallDisplayAreaRects_.right.posY_) &&
        parcel.WriteUint32(waterfallDisplayAreaRects_.right.width_) &&
        parcel.WriteUint32(waterfallDisplayAreaRects_.right.height_) &&
        parcel.WriteInt32(waterfallDisplayAreaRects_.bottom.posX_) &&
        parcel.WriteInt32(waterfallDisplayAreaRects_.bottom.posY_) &&
        parcel.WriteUint32(waterfallDisplayAreaRects_.bottom.width_) &&
        parcel.WriteUint32(waterfallDisplayAreaRects_.bottom.height_) &&
        WriteBoundingRectsVector(boundingRects_, parcel);
}

CutoutInfo *CutoutInfo::Unmarshalling(Parcel &parcel)
{
    WaterfallDisplayAreaRects waterfallDisplayAreaRects;
    std::vector<DMRect> boundingRects;
    ReadWaterfallDisplayAreaRects(waterfallDisplayAreaRects, parcel);
    ReadBoundingRectsVector(boundingRects, parcel);
    CutoutInfo *cutoutInfo = new CutoutInfo(boundingRects, waterfallDisplayAreaRects);
    return cutoutInfo;
}

bool CutoutInfo::WriteBoundingRectsVector(const std::vector<DMRect>& boundingRects, Parcel &parcel) const
{
    if (!parcel.WriteUint32(static_cast<uint32_t>(boundingRects.size()))) {
        return false;
    }
    for (DMRect rect : boundingRects) {
        if (!(parcel.WriteInt32(rect.posX_) && parcel.WriteInt32(rect.posY_) &&
            parcel.WriteUint32(rect.width_) && parcel.WriteUint32(rect.height_))) {
            return false;
        }
    }
    return true;
}

bool CutoutInfo::ReadBoundingRectsVector(std::vector<DMRect>& unmarBoundingRects, Parcel &parcel)
{
    uint32_t size;
    if (!parcel.ReadUint32(size)) {
        return false;
    }
    for (uint32_t index = 0; index < size; index++) {
        int32_t posX;
        int32_t posY;
        uint32_t width;
        uint32_t height;
        if (!(parcel.ReadInt32(posX) && parcel.ReadInt32(posY) &&
            parcel.ReadUint32(width) && parcel.ReadUint32(height))) {
            return false;
        }
        DMRect rect = {posX, posY, width, height};
        unmarBoundingRects.push_back(rect);
    }
    return true;
}

bool CutoutInfo::ReadWaterfallDisplayAreaRects(WaterfallDisplayAreaRects& waterfallDisplayAreaRects, Parcel &parcel)
{
    if (!(parcel.ReadInt32(waterfallDisplayAreaRects.left.posX_) &&
        parcel.ReadInt32(waterfallDisplayAreaRects.left.posY_) &&
        parcel.ReadUint32(waterfallDisplayAreaRects.left.width_) &&
        parcel.ReadUint32(waterfallDisplayAreaRects.left.height_) &&
        parcel.ReadInt32(waterfallDisplayAreaRects.top.posX_) &&
        parcel.ReadInt32(waterfallDisplayAreaRects.top.posY_) &&
        parcel.ReadUint32(waterfallDisplayAreaRects.top.width_) &&
        parcel.ReadUint32(waterfallDisplayAreaRects.top.height_) &&
        parcel.ReadInt32(waterfallDisplayAreaRects.right.posX_) &&
        parcel.ReadInt32(waterfallDisplayAreaRects.right.posY_) &&
        parcel.ReadUint32(waterfallDisplayAreaRects.right.width_) &&
        parcel.ReadUint32(waterfallDisplayAreaRects.right.height_) &&
        parcel.ReadInt32(waterfallDisplayAreaRects.bottom.posX_) &&
        parcel.ReadInt32(waterfallDisplayAreaRects.bottom.posY_) &&
        parcel.ReadUint32(waterfallDisplayAreaRects.bottom.width_) &&
        parcel.ReadUint32(waterfallDisplayAreaRects.bottom.height_))) {
        return false;
    }
    return true;
}
} // namespace OHOS::Rosen