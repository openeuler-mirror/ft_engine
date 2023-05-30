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

#include "input_window_monitor.h"

#include <ipc_skeleton.h>
#include <ability_manager_client.h>

#include "display_manager_service_inner.h"
#include "dm_common.h"
#include "window_helper.h"
#include "window_manager_hilog.h"

namespace OHOS {
namespace Rosen {
namespace {
    constexpr HiviewDFX::HiLogLabel LABEL = {LOG_CORE, HILOG_DOMAIN_WINDOW, "InputWindowMonitor"};
}
static inline void convertRectsToMmiRects(const std::vector<Rect>& rects, std::vector<MMI::Rect>& mmiRects)
{
    for (const auto& rect : rects) {
        mmiRects.emplace_back(
            MMI::Rect{ rect.posX_, rect.posY_, static_cast<int32_t>(rect.width_), static_cast<int32_t>(rect.height_) });
    }
}

void InputWindowMonitor::UpdateInputWindow(uint32_t windowId)
{
    if (windowRoot_ == nullptr) {
        WLOGFE("windowRoot is null.");
        return;
    }
    sptr<WindowNode> windowNode = windowRoot_->GetWindowNode(windowId);
    if (windowNode == nullptr) {
        WLOGFE("window node could not be found.");
        return;
    }
    if (INPUT_WINDOW_TYPE_SKIPPED.find(windowNode->GetWindowProperty()->GetWindowType()) !=
        INPUT_WINDOW_TYPE_SKIPPED.end()) {
        return;
    }
    DisplayId displayId = windowNode->GetDisplayId();
    UpdateInputWindowByDisplayId(displayId);
}

MMI::DisplayGroupInfo InputWindowMonitor::GetDisplayInfo(uint32_t windowId)
{
    if (windowRoot_ == nullptr) {
        WLOGFE("windowRoot is null.");
        return displayGroupInfo_;
    }
    sptr<WindowNode> windowNode = windowRoot_->GetWindowNode(windowId);
    if (windowNode == nullptr) {
        WLOGFE("window node could not be found.");
        return displayGroupInfo_;
    }
    if (INPUT_WINDOW_TYPE_SKIPPED.find(windowNode->GetWindowProperty()->GetWindowType()) !=
        INPUT_WINDOW_TYPE_SKIPPED.end()) {
        return displayGroupInfo_;
    }
    DisplayId displayId = windowNode->GetDisplayId();
    HandleDisplayInfo(displayId);
    return displayGroupInfo_;
}

void InputWindowMonitor::HandleDisplayInfo(DisplayId displayId)
{
    if (displayId == DISPLAY_ID_INVALID) {
        return;
    }
    auto container = windowRoot_->GetOrCreateWindowNodeContainer(displayId);
    if (container == nullptr) {
        WLOGFE("can not get window node container.");
        return;
    }

    auto displayInfos = container->GetAllDisplayInfo();
    if (displayInfos.empty()) {
        return;
    }

    UpdateDisplayGroupInfo(container, displayGroupInfo_);
    UpdateDisplayInfo(displayInfos, displayGroupInfo_.displaysInfo);
    std::vector<sptr<WindowNode>> windowNodes;
    container->TraverseContainer(windowNodes);
    TraverseWindowNodes(windowNodes, displayGroupInfo_.windowsInfo);
}

void InputWindowMonitor::UpdateInputWindowByDisplayId(DisplayId displayId)
{
    HandleDisplayInfo(displayId);
    WLOGFI("update display info to IMS, displayId: %{public}" PRIu64"", displayId);
    MMI::InputManager::GetInstance()->UpdateDisplayInfo(displayGroupInfo_);
}

void InputWindowMonitor::UpdateDisplayGroupInfo(const sptr<WindowNodeContainer>& windowNodeContainer,
                                                MMI::DisplayGroupInfo& displayGroupInfo)
{
    const Rect&& rect = windowNodeContainer->GetDisplayGroupRect();
    displayGroupInfo.width = static_cast<int32_t>(rect.width_);
    displayGroupInfo.height = static_cast<int32_t>(rect.height_);
    displayGroupInfo.focusWindowId = static_cast<int32_t>(windowNodeContainer->GetFocusWindow());
    displayGroupInfo.windowsInfo.clear();
    displayGroupInfo.displaysInfo.clear();
}

void InputWindowMonitor::UpdateDisplayInfo(const std::vector<sptr<DisplayInfo>>& displayInfos,
                                           std::vector<MMI::DisplayInfo>& displayInfoVector)
{
    for (auto& displayInfo : displayInfos) {
        if (displayInfo == nullptr) {
            continue;
        }
        uint32_t displayWidth = static_cast<uint32_t>(displayInfo->GetWidth());
        uint32_t displayHeight = static_cast<uint32_t>(displayInfo->GetHeight());
        int32_t offsetX = displayInfo->GetOffsetX();
        int32_t offsetY = displayInfo->GetOffsetY();
        if (displayInfo->GetWaterfallDisplayCompressionStatus()) {
            displayWidth = static_cast<uint32_t>(
                static_cast<int32_t>(displayWidth) + offsetX * 2); // 2: Get full width;
            displayHeight = static_cast<uint32_t>(
                static_cast<int32_t>(displayHeight) + offsetY * 2); // 2: Get full height;
            offsetX = 0;
            offsetY = 0;
        }
        if (displayInfo->GetRotation() == Rotation::ROTATION_90 ||
            displayInfo->GetRotation() == Rotation::ROTATION_270) {
            std::swap(displayWidth, displayHeight);
        }
        MMI::DisplayInfo display = {
            .id = static_cast<int32_t>(displayInfo->GetDisplayId()),
            .x = offsetX,
            .y = offsetY,
            .width = static_cast<int32_t>(displayWidth),
            .height = static_cast<int32_t>(displayHeight),
            .name = (std::stringstream("display ")<<displayInfo->GetDisplayId()).str(),
            .uniq = "default" + std::to_string(displayInfo->GetDisplayId()),
            .direction = GetDisplayDirectionForMmi(displayInfo->GetRotation()),
        };
        auto displayIter = std::find_if(displayInfoVector.begin(), displayInfoVector.end(),
            [&display](MMI::DisplayInfo& displayInfoTmp) {
            return displayInfoTmp.id == display.id;
        });
        if (displayIter != displayInfoVector.end()) {
            *displayIter = display;
        } else {
            displayInfoVector.emplace_back(display);
        }
        WLOGFD("UpdateDisplayInfo, displayId: %{public}d, displayRect: "
            "[%{public}d, %{public}d, %{public}u, %{public}u]",
            display.id, display.x, display.y, display.width, display.height);
    }
}

void InputWindowMonitor::TransformWindowRects(const sptr<WindowNode>& windowNode, Rect& areaRect,
                                              std::vector<Rect>& touchHotAreas, std::vector<Rect>& pointerHotAreas)
{
    if (windowNode->GetWindowProperty()->isNeedComputerTransform()) {
        windowNode->ComputeTransform();
        for (Rect& rect : touchHotAreas) {
            rect = WindowHelper::TransformRect(windowNode->GetWindowProperty()->GetTransformMat(), rect);
        }
        for (Rect& rect : pointerHotAreas) {
            rect = WindowHelper::TransformRect(windowNode->GetWindowProperty()->GetTransformMat(), rect);
        }
        WLOGFI("Area rect before tranform: [%{public}d, %{public}d, %{public}u, %{public}u]",
            areaRect.posX_, areaRect.posY_, areaRect.width_, areaRect.height_);
        areaRect = WindowHelper::TransformRect(windowNode->GetWindowProperty()->GetTransformMat(), areaRect);
        WLOGFI("Area rect after tranform: [%{public}d, %{public}d, %{public}u, %{public}u]",
            areaRect.posX_, areaRect.posY_, areaRect.width_, areaRect.height_);
    }
}

void InputWindowMonitor::TraverseWindowNodes(const std::vector<sptr<WindowNode>> &windowNodes,
                                             std::vector<MMI::WindowInfo>& windowsInfo)
{
    std::map<uint32_t, sptr<WindowNode>> dialogWindowMap;
    for (const auto& windowNode: windowNodes) {
        sptr<WindowNode> callerNode =
            windowRoot_->FindDialogCallerNode(windowNode->GetWindowType(), windowNode->dialogTargetToken_);
        if (callerNode != nullptr) {
            dialogWindowMap.insert(std::make_pair(callerNode->GetWindowId(), windowNode));
        }
    }
    for (const auto& windowNode: windowNodes) {
        if (INPUT_WINDOW_TYPE_SKIPPED.find(windowNode->GetWindowType()) != INPUT_WINDOW_TYPE_SKIPPED.end()) {
            WLOGFI("skip node[id:%{public}u, type:%{public}d]", windowNode->GetWindowId(), windowNode->GetWindowType());
            continue;
        }

        std::vector<Rect> touchHotAreas;
        std::vector<Rect> pointerHotAreas;
        windowNode->GetTouchHotAreas(touchHotAreas);
        windowNode->GetPointerHotAreas(pointerHotAreas);
        Rect areaRect = windowNode->GetWindowRect();

        TransformWindowRects(windowNode, areaRect, touchHotAreas, pointerHotAreas);

        MMI::WindowInfo windowInfo = {
            .id = static_cast<int32_t>(windowNode->GetWindowId()),
            .pid = windowNode->GetInputEventCallingPid(),
            .uid = windowNode->GetCallingUid(),
            .area = MMI::Rect { areaRect.posX_, areaRect.posY_,
                static_cast<int32_t>(areaRect.width_), static_cast<int32_t>(areaRect.height_) },
            .agentWindowId = static_cast<int32_t>(windowNode->GetWindowId()),
        };

        auto iter = (windowNode->GetParentId() == INVALID_WINDOW_ID) ?
            dialogWindowMap.find(windowNode->GetWindowId()) : dialogWindowMap.find(windowNode->GetParentId());
        if (iter != dialogWindowMap.end()) {
            windowInfo.pid = iter->second->GetCallingPid();
            windowInfo.uid = iter->second->GetCallingUid();
            windowInfo.agentWindowId = static_cast<int32_t>(iter->second->GetWindowId());
        }
        convertRectsToMmiRects(touchHotAreas, windowInfo.defaultHotAreas);
        convertRectsToMmiRects(pointerHotAreas, windowInfo.pointerHotAreas);
        if (!windowNode->GetWindowProperty()->GetTouchable()) {
            WLOGFI("window is not touchable: %{public}u", windowNode->GetWindowId());
            windowInfo.flags |= MMI::WindowInfo::FLAG_BIT_UNTOUCHABLE;
        }
        windowsInfo.emplace_back(windowInfo);
    }
}

MMI::Direction InputWindowMonitor::GetDisplayDirectionForMmi(Rotation rotation)
{
    MMI::Direction direction = MMI::Direction0;
    switch (rotation) {
        case Rotation::ROTATION_0:
            direction = MMI::Direction0;
            break;
        case Rotation::ROTATION_90:
            direction = MMI::Direction90;
            break;
        case Rotation::ROTATION_180:
            direction = MMI::Direction180;
            break;
        case Rotation::ROTATION_270:
            direction = MMI::Direction270;
            break;
        default:
            break;
    }
    return direction;
}
}
}