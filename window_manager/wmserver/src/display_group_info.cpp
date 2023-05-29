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

#include "display_group_info.h"

#include <cinttypes>
#include "window_manager_hilog.h"

namespace OHOS {
namespace Rosen {
namespace {
    constexpr HiviewDFX::HiLogLabel LABEL = {LOG_CORE, HILOG_DOMAIN_WINDOW, "DisplayGroupInfo"};
}

DisplayGroupInfo::DisplayGroupInfo(ScreenId displayGroupId, const sptr<DisplayInfo>& displayInfo)
{
    displayGroupId_ = displayGroupId;
    AddDisplayInfo(displayInfo);
}

void DisplayGroupInfo::AddDisplayInfo(const sptr<DisplayInfo>& displayInfo)
{
    DisplayId displayId = displayInfo->GetDisplayId();
    if (displayInfosMap_.find(displayId) != displayInfosMap_.end()) {
        WLOGFE("current display is exits, displayId: %{public}" PRIu64"", displayId);
        return;
    }
    displayInfosMap_.insert(std::make_pair(displayId, displayInfo));
}

void DisplayGroupInfo::RemoveDisplayInfo(DisplayId displayId)
{
    if (displayInfosMap_.find(displayId) == displayInfosMap_.end()) {
        WLOGFE("current display is not exits, displayId: %{public}" PRIu64"", displayId);
        return;
    }
    displayInfosMap_.erase(displayId);
}

void DisplayGroupInfo::UpdateLeftAndRightDisplayId()
{
    auto displayRectMap = GetAllDisplayRects();
    leftDisplayId_ = displayRectMap.begin()->first;
    rightDisplayId_ = displayRectMap.begin()->first;
    for (auto& elem : displayRectMap) {
        auto& curDisplayRect = elem.second;
        if (curDisplayRect.posX_ < displayRectMap[leftDisplayId_].posX_) {
            leftDisplayId_ = elem.first;
        }
        if ((curDisplayRect.posX_ + static_cast<int32_t>(curDisplayRect.width_)) >
            (displayRectMap[rightDisplayId_].posX_ + static_cast<int32_t>(displayRectMap[rightDisplayId_].width_))) {
            rightDisplayId_ = elem.first;
        }
    }
    WLOGFI("max posX displayId: %{public}" PRIu64", min posX displayId: %{public}" PRIu64"",
        rightDisplayId_, leftDisplayId_);
}

void DisplayGroupInfo::SetDisplayRotation(DisplayId displayId, Rotation rotation)
{
    if (displayInfosMap_.find(displayId) == displayInfosMap_.end()) {
        WLOGFE("current display is not exits, displayId: %{public}" PRIu64"", displayId);
        return;
    }
    displayInfosMap_[displayId]->SetRotation(rotation);
}

void DisplayGroupInfo::SetDisplayVirtualPixelRatio(DisplayId displayId, float vpr)
{
    if (displayInfosMap_.find(displayId) == displayInfosMap_.end()) {
        WLOGFE("current display is not exits, displayId: %{public}" PRIu64"", displayId);
        return;
    }
    displayInfosMap_[displayId]->SetVirtualPixelRatio(vpr);
}

void DisplayGroupInfo::SetDisplayRect(DisplayId displayId, Rect displayRect)
{
    if (displayInfosMap_.find(displayId) == displayInfosMap_.end()) {
        WLOGFE("current display is not exits, displayId: %{public}" PRIu64"", displayId);
        return;
    }
    auto& displayInfo = displayInfosMap_[displayId];
    displayInfo->SetOffsetX(displayRect.posX_);
    displayInfo->SetOffsetY(displayRect.posY_);
    displayInfo->SetWidth(displayRect.width_);
    displayInfo->SetHeight(displayRect.height_);
}

Rotation DisplayGroupInfo::GetDisplayRotation(DisplayId displayId) const
{
    Rotation rotation = Rotation::ROTATION_0;
    if (displayInfosMap_.find(displayId) != displayInfosMap_.end()) {
        auto& displayInfo = displayInfosMap_[displayId];
        rotation = displayInfo->GetRotation();
    }
    return rotation;
}

float DisplayGroupInfo::GetDisplayVirtualPixelRatio(DisplayId displayId) const
{
    float vpr = 1.0;
    if (displayInfosMap_.find(displayId) != displayInfosMap_.end()) {
        auto& displayInfo = displayInfosMap_[displayId];
        vpr = displayInfo->GetVirtualPixelRatio();
    }
    return vpr;
}

std::map<DisplayId, Rect> DisplayGroupInfo::GetAllDisplayRects() const
{
    std::map<DisplayId, Rect> displayRectMap;
    for (auto elem : displayInfosMap_) {
        auto& displayInfo = elem.second;
        Rect displayRect = { displayInfo->GetOffsetX(), displayInfo->GetOffsetY(),
            displayInfo->GetWidth(), displayInfo->GetHeight() };
        displayRectMap.insert(std::make_pair(elem.first, displayRect));
    }
    return displayRectMap;
}

Rect DisplayGroupInfo::GetDisplayRect(DisplayId displayId) const
{
    Rect rect;
    if (displayInfosMap_.find(displayId) != displayInfosMap_.end()) {
        auto& displayInfo = displayInfosMap_[displayId];
        rect = { displayInfo->GetOffsetX(), displayInfo->GetOffsetY(),
            displayInfo->GetWidth(), displayInfo->GetHeight() };
    }
    return rect;
}

sptr<DisplayInfo> DisplayGroupInfo::GetDisplayInfo(DisplayId displayId) const
{
    if (displayInfosMap_.find(displayId) != displayInfosMap_.end()) {
        return displayInfosMap_[displayId];
    }
    return nullptr;
}

void DisplayGroupInfo::UpdateDisplayInfo(sptr<DisplayInfo> displayInfo) const
{
    DisplayId displayId = displayInfo->GetDisplayId();
    if (displayInfosMap_.find(displayId) != displayInfosMap_.end()) {
        displayInfosMap_[displayId] = displayInfo;
        WLOGFD("Update displayInfo");
    }
}

std::vector<sptr<DisplayInfo>> DisplayGroupInfo::GetAllDisplayInfo() const
{
    std::vector<sptr<DisplayInfo>> displayInfos;
    for (auto& iter : displayInfosMap_) {
        displayInfos.push_back(iter.second);
    }
    return displayInfos;
}

DisplayId DisplayGroupInfo::GetLeftDisplayId() const
{
    return leftDisplayId_;
}

DisplayId DisplayGroupInfo::GetRightDisplayId() const
{
    return rightDisplayId_;
}
} // namespace Rosen
} // namespace OHOS
