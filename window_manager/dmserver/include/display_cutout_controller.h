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

#ifndef OHOS_ROSEN_DISPLAY_CUTOUT_CONTROLLER_H
#define OHOS_ROSEN_DISPLAY_CUTOUT_CONTROLLER_H

#include <mutex>
#include <map>
#include <refbase.h>
#include <vector>

#include "include/core/SkPath.h"
#include "include/core/SkMatrix.h"
#include "include/core/SkPathMeasure.h"
#include "include/utils/SkParsePath.h"

#include "cutout_info.h"
#include "dm_common.h"
#include "noncopyable.h"
#include "window_manager_hilog.h"

namespace OHOS {
namespace Rosen {
class DisplayCutoutController : public RefBase {
public:
    DisplayCutoutController() {};
    virtual ~DisplayCutoutController() = default;

    void SetCutoutSvgPath(DisplayId displayId, const std::string& svgPath);
    sptr<CutoutInfo> GetCutoutInfo(DisplayId displayId);

    // For built-in display
    void SetBuiltInDisplayCutoutSvgPath(const std::string& svgPath);
    static void SetIsWaterfallDisplay(bool isWaterfallDisplay);
    static bool IsWaterfallDisplay();
    void SetCurvedScreenBoundary(std::vector<int> curvedScreenBoundary);

    // For waterfall display curved area compression.
    static void SetWaterfallAreaCompressionEnableWhenHorzontal(bool isEnable);
    static void SetWaterfallAreaCompressionSizeWhenHorizontal(uint32_t size);
    static bool IsWaterfallAreaCompressionEnableWhenHorizontal();
    static uint32_t GetWaterfallAreaCompressionSizeWhenHorizontal();
private:
    DMRect CalcCutoutBoundingRect(std::string svgPath);
    void CheckBoundingRectsBoundary(DisplayId displayId, std::vector<DMRect>& boundingRects);
    void CalcBuiltInDisplayWaterfallRects();
    void CalcBuiltInDisplayWaterfallRectsByRotation(Rotation rotation, uint32_t displayHeight, uint32_t displayWidth);
    void TransferBoundingRectsByRotation(DisplayId displayId, std::vector<DMRect>& boundingRects);
    DMRect CreateWaterfallRect(uint32_t left, uint32_t top, uint32_t width, uint32_t height);

    // Raw data
    std::map<DisplayId, std::vector<std::string>> svgPaths_;
    static bool isWaterfallDisplay_;
    std::vector<int> curvedScreenBoundary_; // Order: left top right bottom

    // Calulated data
    WaterfallDisplayAreaRects waterfallDisplayAreaRects_ = {};
    std::map<DisplayId, std::vector<DMRect>> boundingRects_;

    // For waterfall display curved area compression.
    static bool isWaterfallAreaCompressionEnableWhenHorizontal_;
    static uint32_t waterfallAreaCompressionSizeWhenHorizontal_; // The unit is vp.
};
} // Rosen
} // OHOS
#endif // OHOS_ROSEN_DISPLAY_CUTOUT_CONTROLLER_H