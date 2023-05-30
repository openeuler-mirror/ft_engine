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

#include "abstract_display.h"

#include "abstract_screen_controller.h"
#include "display_manager_config.h"
#include "display_manager_service.h"
#include "window_manager_hilog.h"

namespace OHOS::Rosen {
namespace {
    constexpr HiviewDFX::HiLogLabel LABEL = {LOG_CORE, HILOG_DOMAIN_DISPLAY, "AbstractDisplay"};
    constexpr int32_t PAD_SCREEN_WIDTH = 2560;
    constexpr int32_t PHONE_SCREEN_WIDTH = 2160;
}

AbstractDisplay::AbstractDisplay(DisplayId id, std::string name,
    sptr<SupportedScreenModes>& info, sptr<AbstractScreen>& absScreen)
    : id_(id),
      name_(name),
      screenId_(absScreen->dmsId_),
      screenGroupId_(absScreen->groupDmsId_),
      width_(info->width_),
      height_(info->height_),
      refreshRate_(info->refreshRate_),
      orientation_(absScreen->orientation_)
{
    RequestRotation(absScreen->rotation_);
    auto numbersConfig = DisplayManagerConfig::GetIntNumbersConfig();
    if (numbersConfig.count("dpi") != 0) {
        uint32_t densityDpi = static_cast<uint32_t>(numbersConfig["dpi"][0]);
        if (densityDpi >= DOT_PER_INCH_MINIMUM_VALUE && densityDpi <= DOT_PER_INCH_MAXIMUM_VALUE) {
            virtualPixelRatio_ = static_cast<float>(densityDpi) / BASELINE_DENSITY;
            absScreen->SetVirtualPixelRatio(virtualPixelRatio_);
            return;
        }
    }
    if ((info->width_ >= PHONE_SCREEN_WIDTH) || (info->height_ >= PHONE_SCREEN_WIDTH)) {
        if ((info->width_ == PAD_SCREEN_WIDTH) || (info->height_ == PAD_SCREEN_WIDTH)) {
            virtualPixelRatio_ = 2.0f; // Pad is 2.0
        } else {
            virtualPixelRatio_ = 3.0f; // Phone is 3.0
        }
    } else {
        virtualPixelRatio_ = 1.0f; // Other is 1.0
    }
    absScreen->SetVirtualPixelRatio(virtualPixelRatio_);
}

DisplayId AbstractDisplay::GetId() const
{
    return id_;
}

int32_t AbstractDisplay::GetWidth() const
{
    return width_;
}

int32_t AbstractDisplay::GetHeight() const
{
    return height_;
}

uint32_t AbstractDisplay::GetRefreshRate() const
{
    return refreshRate_;
}

float AbstractDisplay::GetVirtualPixelRatio() const
{
    return virtualPixelRatio_;
}

int32_t AbstractDisplay::GetOffsetX() const
{
    return offsetX_;
}

int32_t AbstractDisplay::GetOffsetY() const
{
    return offsetY_;
}

void AbstractDisplay::SetOffsetX(int32_t offsetX)
{
    offsetX_ = offsetX;
}

void AbstractDisplay::SetOffsetY(int32_t offsetY)
{
    offsetY_ = offsetY;
}

void AbstractDisplay::SetWidth(int32_t width)
{
    width_ = width;
}

void AbstractDisplay::SetHeight(int32_t height)
{
    height_ = height;
}

void AbstractDisplay::SetOffset(int32_t offsetX, int32_t offsetY)
{
    offsetX_ = offsetX;
    offsetY_ = offsetY;
}

void AbstractDisplay::SetRefreshRate(uint32_t refreshRate)
{
    refreshRate_ = refreshRate;
}

void AbstractDisplay::SetVirtualPixelRatio(float virtualPixelRatio)
{
    virtualPixelRatio_ = virtualPixelRatio;
}

void AbstractDisplay::SetId(DisplayId id)
{
    id_ = id;
}

void AbstractDisplay::SetOrientation(Orientation orientation)
{
    orientation_ = orientation;
}

bool AbstractDisplay::RequestRotation(Rotation rotation)
{
    WLOGD("request rotation from %{public}u to %{public}u, display %{public}" PRIu64"", rotation_, rotation, id_);
    if (rotation_ == rotation) {
        WLOGFE("rotation not change %{public}u", rotation);
        return false;
    }
    if (IsVertical(rotation) != IsVertical(rotation_)) {
        std::swap(width_, height_);
    }
    rotation_ = rotation;
    return true;
}

Rotation AbstractDisplay::GetRotation() const
{
    return rotation_;
}

Orientation AbstractDisplay::GetOrientation() const
{
    return orientation_;
}

void AbstractDisplay::SetFreezeFlag(FreezeFlag freezeFlag)
{
    freezeFlag_ = freezeFlag;
}

FreezeFlag AbstractDisplay::GetFreezeFlag() const
{
    return freezeFlag_;
}

bool AbstractDisplay::BindAbstractScreen(sptr<AbstractScreen> abstractScreen)
{
    if (abstractScreen == nullptr) {
        WLOGE("display bind screen error, cannot get screen. display:%{public}" PRIu64"", id_);
        return false;
    }
    ScreenId dmsScreenId = abstractScreen->dmsId_;
    sptr<SupportedScreenModes> info = abstractScreen->GetActiveScreenMode();
    if (info == nullptr) {
        WLOGE("display bind screen error, cannot get info. display:%{public}" PRIu64", screen:%{public}" PRIu64"",
            id_, dmsScreenId);
        return false;
    }

    Point point = abstractScreen->GetGroup()->GetChildPosition(dmsScreenId);
    offsetX_ = point.posX_;
    offsetY_ = point.posY_;
    width_ = static_cast<int32_t>(info->width_);
    height_ = static_cast<int32_t>(info->height_);
    refreshRate_ = info->refreshRate_;
    screenId_ = dmsScreenId;
    WLOGD("display bind to screen. display:%{public}" PRIu64", screen:%{public}" PRIu64"", id_, dmsScreenId);
    return true;
}

ScreenId AbstractDisplay::GetAbstractScreenId() const
{
    return screenId_;
}

ScreenId AbstractDisplay::GetAbstractScreenGroupId() const
{
    return screenGroupId_;
}

sptr<DisplayInfo> AbstractDisplay::ConvertToDisplayInfo() const
{
    sptr<DisplayInfo> displayInfo = new(std::nothrow) DisplayInfo();
    if (displayInfo == nullptr) {
        return displayInfo;
    }
    displayInfo->name_ = name_;
    displayInfo->SetOffsetX(offsetX_);
    displayInfo->SetOffsetY(offsetY_);
    displayInfo->SetWidth(width_);
    displayInfo->SetHeight(height_);
    displayInfo->SetDisplayId(id_);
    displayInfo->SetRefreshRate(refreshRate_);
    displayInfo->SetScreenId(screenId_);
    displayInfo->SetScreenGroupId(screenGroupId_);
    displayInfo->SetVirtualPixelRatio(virtualPixelRatio_);
    displayInfo->SetRotation(rotation_);
    displayInfo->SetOrientation(orientation_);
    displayInfo->displayState_ = displayState_;
    displayInfo->SetWaterfallDisplayCompressionStatus(waterfallDisplayCompressionStatus_);
    return displayInfo;
}
} // namespace OHOS::Rosen