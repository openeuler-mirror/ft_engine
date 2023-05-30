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

#include "display_cutout_controller.h"
#include <screen_manager/screen_types.h>
#include "display_manager_service_inner.h"

namespace OHOS {
namespace Rosen {
namespace {
    constexpr HiviewDFX::HiLogLabel LABEL = {LOG_CORE, HILOG_DOMAIN_DISPLAY, "DisplayCutoutController"};
    const uint32_t NO_WATERFALL_DISPLAY_COMPRESSION_SIZE = 0;
}

bool DisplayCutoutController::isWaterfallDisplay_ = false;
bool DisplayCutoutController::isWaterfallAreaCompressionEnableWhenHorizontal_ = false;
uint32_t DisplayCutoutController::waterfallAreaCompressionSizeWhenHorizontal_ = 0;

void DisplayCutoutController::SetBuiltInDisplayCutoutSvgPath(const std::string& svgPath)
{
    SetCutoutSvgPath(0, svgPath);
}

void DisplayCutoutController::SetIsWaterfallDisplay(bool isWaterfallDisplay)
{
    WLOGFI("Set isWaterfallDisplay: %{public}u", isWaterfallDisplay);
    isWaterfallDisplay_ = isWaterfallDisplay;
}

bool DisplayCutoutController::IsWaterfallDisplay()
{
    return isWaterfallDisplay_;
}

void DisplayCutoutController::SetCurvedScreenBoundary(std::vector<int> curvedScreenBoundary)
{
    while (curvedScreenBoundary.size() < 4) { // 4 directions.
        curvedScreenBoundary.emplace_back(0);
    }
    WLOGFI("Set curvedScreenBoundary");
    curvedScreenBoundary_ = curvedScreenBoundary;
}

void DisplayCutoutController::SetCutoutSvgPath(DisplayId displayId, const std::string& svgPath)
{
    WLOGFI("Set SvgPath: %{public}s", svgPath.c_str());
    if (svgPaths_.count(displayId) == 1) {
        svgPaths_[displayId].emplace_back(svgPath);
    } else {
        std::vector<std::string> pathVec;
        pathVec.emplace_back(svgPath);
        svgPaths_[displayId] = pathVec;
    }
    DMRect boundingRect = CalcCutoutBoundingRect(svgPath);
    if (boundingRects_.count(displayId) == 1) {
        boundingRects_[displayId].emplace_back(boundingRect);
    } else {
        std::vector<DMRect> rectVec;
        rectVec.emplace_back(boundingRect);
        boundingRects_[displayId] = rectVec;
    }
}

sptr<CutoutInfo> DisplayCutoutController::GetCutoutInfo(DisplayId displayId)
{
    WLOGFD("Get Cutout Info");
    std::vector<DMRect> boundingRects;
    WaterfallDisplayAreaRects waterfallDisplayAreaRects;
    if (boundingRects_.count(displayId) == 1) {
        TransferBoundingRectsByRotation(displayId, boundingRects);
    }
    if (displayId == DisplayManagerServiceInner::GetInstance().GetDefaultDisplayId()) {
        CalcBuiltInDisplayWaterfallRects();
        waterfallDisplayAreaRects = waterfallDisplayAreaRects_;
    }
    CutoutInfo *cutoutInfo = new CutoutInfo(boundingRects, waterfallDisplayAreaRects);
    return cutoutInfo;
}

void DisplayCutoutController::CheckBoundingRectsBoundary(DisplayId displayId, std::vector<DMRect>& boundingRects)
{
    sptr<SupportedScreenModes> modes =
        DisplayManagerServiceInner::GetInstance().GetScreenModesByDisplayId(displayId);
    if (modes == nullptr) {
        WLOGFE("DisplayId is invalid");
        return;
    }
    uint32_t displayHeight = modes->height_;
    uint32_t displayWidth = modes->width_;
    for (auto iter = boundingRects.begin(); iter != boundingRects.end();) {
        DMRect boundingRect = *iter;
        if (boundingRect.posX_ < 0 || boundingRect.posY_ < 0 ||
            static_cast<int32_t>(boundingRect.width_) + boundingRect.posX_ > static_cast<int32_t>(displayWidth) ||
            static_cast<int32_t>(boundingRect.height_) + boundingRect.posY_ > static_cast<int32_t>(displayHeight) ||
            boundingRect.width_ > displayWidth || boundingRect.height_ > displayHeight ||
            boundingRect.isUninitializedRect()) {
            WLOGFE("boundingRect boundary is invalid");
            iter = boundingRects.erase(iter);
        } else {
            iter++;
        }
    }
}

DMRect DisplayCutoutController::CalcCutoutBoundingRect(std::string svgPath)
{
    DMRect emptyRect = {0, 0, 0, 0};
    SkPath skCutoutSvgPath;
    if (!SkParsePath::FromSVGString(svgPath.c_str(), &skCutoutSvgPath)) {
        WLOGFE("Parse svg string path failed.");
        return emptyRect;
    }
    SkRect skRect = skCutoutSvgPath.computeTightBounds();
    if (skRect.isEmpty()) {
        WLOGFW("Get empty skRect");
        return emptyRect;
    }
    SkIRect skiRect = skRect.roundOut();
    if (skiRect.isEmpty()) {
        WLOGFW("Get empty skiRect");
        return emptyRect;
    }
    int32_t left = static_cast<int32_t>(skiRect.left());
    int32_t top = static_cast<int32_t>(skiRect.top());
    uint32_t width = static_cast<uint32_t>(skiRect.width());
    uint32_t height = static_cast<uint32_t>(skiRect.height());
    WLOGFI("calc rect from path,[%{public}d, %{public}d, %{public}u, %{public}u]", left, top, width, height);
    DMRect cutoutMinOuterRect = {.posX_ = left, .posY_ = top, .width_ = width, .height_ = height};
    return cutoutMinOuterRect;
}

void DisplayCutoutController::CalcBuiltInDisplayWaterfallRects()
{
    WaterfallDisplayAreaRects emptyRects = {};
    if (!isWaterfallDisplay_) {
        WLOGFI("not waterfall display");
        waterfallDisplayAreaRects_ = emptyRects;
        return;
    }
    if (curvedScreenBoundary_.empty()) {
        WLOGFI("curved screen boundary is empty");
        waterfallDisplayAreaRects_ = emptyRects;
        return;
    }
    uint32_t left = static_cast<uint32_t>(curvedScreenBoundary_[0]);
    uint32_t top = static_cast<uint32_t>(curvedScreenBoundary_[1]);
    uint32_t right = static_cast<uint32_t>(curvedScreenBoundary_[2]);
    uint32_t bottom = static_cast<uint32_t>(curvedScreenBoundary_[3]);
    if (left == 0 && top == 0 && right == 0 && bottom == 0) {
        waterfallDisplayAreaRects_ = emptyRects;
        return;
    }
    sptr<SupportedScreenModes> modes =
        DisplayManagerServiceInner::GetInstance().GetScreenModesByDisplayId(
            DisplayManagerServiceInner::GetInstance().GetDefaultDisplayId());
    if (!modes) {
        WLOGE("support screen modes get failed");
        waterfallDisplayAreaRects_ = emptyRects;
        return;
    }
    uint32_t displayHeight = modes->height_;
    uint32_t displayWidth = modes->width_;

    if ((left > displayWidth / 2) || (right > displayWidth / 2) || // invalid if more than 1/2 width
        (top > displayHeight / 2) || (bottom > displayHeight / 2)) { // invalid if more than 1/2 height
        WLOGFE("Curved screen boundary data is not valid.");
        waterfallDisplayAreaRects_ = emptyRects;
        return;
    }
    CalcBuiltInDisplayWaterfallRectsByRotation(
        DisplayManagerServiceInner::GetInstance().GetDefaultDisplay()->GetRotation(),
        displayHeight, displayWidth);
}

void DisplayCutoutController::CalcBuiltInDisplayWaterfallRectsByRotation(
    Rotation rotation, uint32_t displayHeight, uint32_t displayWidth)
{
    uint32_t left = static_cast<uint32_t>(curvedScreenBoundary_[0]);
    uint32_t top = static_cast<uint32_t>(curvedScreenBoundary_[1]);
    uint32_t right = static_cast<uint32_t>(curvedScreenBoundary_[2]);
    uint32_t bottom = static_cast<uint32_t>(curvedScreenBoundary_[3]);
    switch (rotation) {
        case Rotation::ROTATION_0: {
            DMRect leftRect = CreateWaterfallRect(0, 0, left, displayHeight);
            DMRect topRect = CreateWaterfallRect(0, 0, displayWidth, top);
            DMRect rightRect = CreateWaterfallRect(displayWidth - right, 0, right, displayHeight);
            DMRect bottomRect = CreateWaterfallRect(0, displayHeight - bottom, displayWidth, bottom);
            waterfallDisplayAreaRects_ = WaterfallDisplayAreaRects {leftRect, topRect, rightRect, bottomRect};
            return;
        }
        case Rotation::ROTATION_90: {
            DMRect leftRect = CreateWaterfallRect(0, 0, bottom, displayWidth);
            DMRect topRect = CreateWaterfallRect(0, 0, displayHeight, left);
            DMRect rightRect = CreateWaterfallRect(displayHeight - top, 0, top, displayWidth);
            DMRect bottomRect = CreateWaterfallRect(0, displayWidth - right, displayHeight, right);
            waterfallDisplayAreaRects_ = WaterfallDisplayAreaRects {leftRect, topRect, rightRect, bottomRect};
            return;
        }
        case Rotation::ROTATION_180: {
            DMRect leftRect = CreateWaterfallRect(0, 0, right, displayHeight);
            DMRect topRect = CreateWaterfallRect(0, 0, bottom, displayWidth);
            DMRect rightRect = CreateWaterfallRect(displayWidth - left, 0, left, displayHeight);
            DMRect bottomRect = CreateWaterfallRect(0, displayHeight - top, displayWidth, top);
            waterfallDisplayAreaRects_ = WaterfallDisplayAreaRects {leftRect, topRect, rightRect, bottomRect};
            return;
        }
        case Rotation::ROTATION_270: {
            DMRect leftRect = CreateWaterfallRect(0, 0, top, displayWidth);
            DMRect topRect = CreateWaterfallRect(0, 0, displayHeight, right);
            DMRect rightRect = CreateWaterfallRect(displayHeight - bottom, 0, bottom, displayWidth);
            DMRect bottomRect = CreateWaterfallRect(0, displayWidth - left, displayHeight, left);
            waterfallDisplayAreaRects_ = WaterfallDisplayAreaRects {leftRect, topRect, rightRect, bottomRect};
            return;
        }
        default: {
        }
    }
}

void DisplayCutoutController::TransferBoundingRectsByRotation(DisplayId displayId, std::vector<DMRect>& boundingRects)
{
    std::vector<DMRect> resultVec;
    if (boundingRects_.count(displayId) == 0) {
        boundingRects = resultVec;
        return;
    }
    std::vector<DMRect> displayBoundingRects = boundingRects_[displayId];
    if (displayBoundingRects.empty()) {
        boundingRects = resultVec;
        return;
    }
    sptr<DisplayInfo> displayInfo = DisplayManagerServiceInner::GetInstance().GetDisplayById(displayId);
    if (!displayInfo) {
        WLOGFE("display invaild");
        return;
    }
    Rotation currentRotation = displayInfo->GetRotation();
    CheckBoundingRectsBoundary(displayId, displayBoundingRects);
    if (currentRotation == Rotation::ROTATION_0) {
        boundingRects = displayBoundingRects;
        return;
    }
    sptr<SupportedScreenModes> modes =
        DisplayManagerServiceInner::GetInstance().GetScreenModesByDisplayId(displayId);
    if (!modes) {
        WLOGE("support screen modes get failed");
        return;
    }
    uint32_t displayHeight = modes->height_;
    uint32_t displayWidth = modes->width_;
    
    switch (currentRotation) {
        case Rotation::ROTATION_90: {
            for (DMRect rect : displayBoundingRects) {
                resultVec.emplace_back(DMRect {.posX_ = displayHeight - rect.posY_ - rect.height_,
                    .posY_ = rect.posX_, .width_ = rect.height_, .height_ = rect.width_});
            }
            break;
        }
        case Rotation::ROTATION_180: {
            for (DMRect rect : displayBoundingRects) {
                resultVec.emplace_back(DMRect {displayWidth - rect.posX_ - rect.width_,
                    displayHeight - rect.posY_ - rect.height_, rect.width_, rect.height_});
            }
            break;
        }
        case Rotation::ROTATION_270: {
            for (DMRect rect : displayBoundingRects) {
                resultVec.emplace_back(DMRect {rect.posY_, displayWidth - rect.posX_ - rect.width_,
                    rect.height_, rect.width_});
            }
            break;
        }
        default: {
        }
    }
    boundingRects = resultVec;
}

DMRect DisplayCutoutController::CreateWaterfallRect(uint32_t left, uint32_t top, uint32_t width, uint32_t height)
{
    if (width == 0 || height == 0) {
        return DMRect {0, 0, 0, 0};
    }
    return DMRect {left, top, width, height};
}

void DisplayCutoutController::SetWaterfallAreaCompressionEnableWhenHorzontal(bool isEnable)
{
    isWaterfallAreaCompressionEnableWhenHorizontal_ = isEnable;
}

void DisplayCutoutController::SetWaterfallAreaCompressionSizeWhenHorizontal(uint32_t size)
{
    waterfallAreaCompressionSizeWhenHorizontal_ = size;
}

bool DisplayCutoutController::IsWaterfallAreaCompressionEnableWhenHorizontal()
{
    return isWaterfallDisplay_ && isWaterfallAreaCompressionEnableWhenHorizontal_;
}

uint32_t DisplayCutoutController::GetWaterfallAreaCompressionSizeWhenHorizontal()
{
    if (!isWaterfallDisplay_ || !isWaterfallAreaCompressionEnableWhenHorizontal_) {
        WLOGFW("Not waterfall display or not enable waterfall compression");
        return NO_WATERFALL_DISPLAY_COMPRESSION_SIZE;
    }
    return waterfallAreaCompressionSizeWhenHorizontal_;
}
} // Rosen
} // OHOS
