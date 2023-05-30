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

#ifndef FOUNDATION_DMSERVER_CUTOUT_INFO_H
#define FOUNDATION_DMSERVER_CUTOUT_INFO_H

#include <parcel.h>

#include "class_var_definition.h"
#include "display.h"
#include "dm_common.h"

namespace OHOS::Rosen {
struct DMRect {
    int32_t posX_;
    int32_t posY_;
    uint32_t width_;
    uint32_t height_;

    bool operator==(const DMRect& a) const
    {
        return (posX_ == a.posX_ && posY_ == a.posY_ && width_ == a.width_ && height_ == a.height_);
    }

    bool operator!=(const DMRect& a) const
    {
        return !this->operator==(a);
    }

    bool isUninitializedRect() const
    {
        return (posX_ == 0 && posY_ == 0 && width_ == 0 && height_ == 0);
    }

    bool IsInsideOf(const DMRect& a) const
    {
        return (posX_ >= a.posX_ && posY_ >= a.posY_ &&
            posX_ + width_ <= a.posX_ + a.width_ && posY_ + height_ <= a.posY_ + a.height_);
    }
};

struct WaterfallDisplayAreaRects {
    DMRect left;
    DMRect top;
    DMRect right;
    DMRect bottom;

    bool isUninitialized() const
    {
        return (left.isUninitializedRect() && top.isUninitializedRect() && right.isUninitializedRect() &&
            bottom.isUninitializedRect());
    }
};

class CutoutInfo : public Parcelable {
public:
    CutoutInfo() = default;
    CutoutInfo(const std::vector<DMRect>& boundingRects, WaterfallDisplayAreaRects waterfallDisplayAreaRects);
    ~CutoutInfo() = default;
    WM_DISALLOW_COPY_AND_MOVE(CutoutInfo);

    virtual bool Marshalling(Parcel& parcel) const override;
    static CutoutInfo *Unmarshalling(Parcel& parcel);

    DEFINE_VAR_FUNC_GET_SET(WaterfallDisplayAreaRects, WaterfallDisplayAreaRects, waterfallDisplayAreaRects);
    DEFINE_VAR_FUNC_GET_SET(std::vector<DMRect>, BoundingRects, boundingRects);
private:
    bool WriteBoundingRectsVector(const std::vector<DMRect>& boundingRects, Parcel &parcel) const;
    static bool ReadBoundingRectsVector(std::vector<DMRect>& unmarBoundingRects, Parcel &parcel);
    static bool ReadWaterfallDisplayAreaRects(WaterfallDisplayAreaRects& waterfallDisplayAreaRects, Parcel &parcel);
};
} // namespace OHOS::Rosen
#endif // FOUNDATION_DMSERVER_CUTOUT_INFO_H