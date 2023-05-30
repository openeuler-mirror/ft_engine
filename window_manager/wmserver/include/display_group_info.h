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

#ifndef OHOS_ROSEN_DISPLAY_GROUP_INFO_H
#define OHOS_ROSEN_DISPLAY_GROUP_INFO_H

#include <map>
#include <refbase.h>

#include "display_info.h"
#include "wm_common.h"

namespace OHOS {
namespace Rosen {
class DisplayGroupInfo : public RefBase {
public:
    DisplayGroupInfo(ScreenId displayGroupId, const sptr<DisplayInfo>& displayInfo);
    ~DisplayGroupInfo() = default;
    void AddDisplayInfo(const sptr<DisplayInfo>& displayInfo);
    void RemoveDisplayInfo(DisplayId displayId);
    void UpdateLeftAndRightDisplayId();

    void SetDisplayRotation(DisplayId displayId, Rotation rotation);
    void SetDisplayVirtualPixelRatio(DisplayId displayId, float vpr);
    void SetDisplayRect(DisplayId displayId, Rect displayRect);

    Rotation GetDisplayRotation(DisplayId displayId) const;
    float GetDisplayVirtualPixelRatio(DisplayId displayId) const;
    std::map<DisplayId, Rect> GetAllDisplayRects() const;
    Rect GetDisplayRect(DisplayId displayId) const;
    sptr<DisplayInfo> GetDisplayInfo(DisplayId displayId) const;
    void UpdateDisplayInfo(sptr<DisplayInfo> displayInfo) const;
    std::vector<sptr<DisplayInfo>> GetAllDisplayInfo() const;
    DisplayId GetLeftDisplayId() const;
    DisplayId GetRightDisplayId() const;

private:
    ScreenId displayGroupId_;
    DisplayId leftDisplayId_ { 0 };
    DisplayId rightDisplayId_ { 0 };
    mutable std::map<DisplayId, sptr<DisplayInfo>> displayInfosMap_;
};
} // namespace Rosen
} // namespace OHOS
#endif // OHOS_ROSEN_DISPLAY_GROUP_INFO_H

