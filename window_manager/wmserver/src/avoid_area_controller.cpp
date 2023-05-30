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

#include "avoid_area_controller.h"

#include <hitrace_meter.h>

#include "display_manager_service_inner.h"
#include "window_helper.h"
#include "window_manager_hilog.h"

namespace OHOS {
namespace Rosen {
namespace {
    constexpr HiviewDFX::HiLogLabel LABEL = {LOG_CORE, HILOG_DOMAIN_WINDOW, "AvoidAreaController"};
}

void AvoidAreaController::UpdateAvoidAreaListener(sptr<WindowNode>& windowNode, bool isRegisterListener)
{
    WLOGFE("haveAvoidAreaListener %{public}d", isRegisterListener);
    if (windowNode == nullptr) {
        WLOGFE("windowNode is nullptr.");
        return;
    }
    if (isRegisterListener) {
        avoidAreaListenerNodes_.insert(windowNode);
    } else {
        lastUpdatedAvoidArea_.erase(windowNode->GetWindowId());
        avoidAreaListenerNodes_.erase(windowNode);
    }
}

void AvoidAreaController::ProcessWindowChange(const sptr<WindowNode>& windowNode, AvoidControlType avoidType,
    const std::function<bool(sptr<WindowNode>)>& checkFunc)
{
    if (isForbidProcessingWindowChange_) {
        WLOGFI("do not process window change.");
        return;
    }
    if (windowNode == nullptr || windowNode->GetWindowToken() == nullptr) {
        WLOGFE("invalid WindowNode.");
        return;
    }
    switch (avoidType) {
        case AvoidControlType::AVOID_NODE_ADD:
        case AvoidControlType::AVOID_NODE_REMOVE:
            AddOrRemoveOverlayWindowIfNeed(windowNode, avoidType == AvoidControlType::AVOID_NODE_ADD);
            break;
        case AvoidControlType::AVOID_NODE_UPDATE:
            UpdateOverlayWindowIfNeed(windowNode, checkFunc);
            break;
        default:
            break;
    }
}

void AvoidAreaController::AddOrRemoveOverlayWindowIfNeed(const sptr<WindowNode>& overlayNode, bool isAdding)
{
    if (!WindowHelper::IsOverlayWindow(overlayNode->GetWindowType())) {
        WLOGFE("IsOverlayWindow Failed.");
        return;
    }
    HITRACE_METER(HITRACE_TAG_WINDOW_MANAGER);

    uint32_t overlayId = overlayNode->GetWindowId();
    bool isRecorded = (overlayWindowMap_.find(overlayId) != overlayWindowMap_.end());
    if (isAdding == isRecorded) {
        WLOGFE("error occured in overlay. overlayId %{public}u isAdding %{public}d record flag %{public}d",
            overlayId, isAdding, isRecorded);
        return;
    }
    if (isAdding) {
        overlayWindowMap_.insert(std::make_pair(overlayId, overlayNode));
    } else {
        overlayWindowMap_.erase(overlayId);
    }

    if (overlayNode->GetWindowType() == WindowType::WINDOW_TYPE_INPUT_METHOD_FLOAT) {
        AddOrRemoveKeyboard(overlayNode, isAdding);
        return;
    }

    for (auto& node : avoidAreaListenerNodes_) {
        AvoidArea systemAvoidArea = GetAvoidAreaByType(node, AvoidAreaType::TYPE_SYSTEM);
        UpdateAvoidAreaIfNeed(systemAvoidArea, node, AvoidAreaType::TYPE_SYSTEM);
    }
}

void AvoidAreaController::AddOrRemoveKeyboard(const sptr<WindowNode>& keyboardNode, bool isAdding)
{
    const uint32_t callingWindowId = keyboardNode->GetCallingWindow();
    sptr<WindowNode> callingWindow = nullptr;
    sptr<WindowNode> focusWindow = nullptr;
    sptr<WindowNode> lastKeyboardAreaUpdatedWindow = nullptr;
    for (auto window : avoidAreaListenerNodes_) {
        if (window == nullptr || window->GetWindowToken() == nullptr) {
            continue;
        }
        if (window->GetWindowId() == callingWindowId) {
            callingWindow = window;
        }
        if (window->GetWindowId() == focusedWindow_) {
            focusWindow = window;
        }
        if (window->GetWindowId() == lastSoftInputKeyboardAreaUpdatedWindowId_) {
            lastKeyboardAreaUpdatedWindow = window;
        }
    }
    if (callingWindow == nullptr) {
        callingWindow = focusWindow;
    }
    if (lastKeyboardAreaUpdatedWindow != nullptr && lastKeyboardAreaUpdatedWindow != callingWindow) {
        const WindowMode windowMode = lastKeyboardAreaUpdatedWindow->GetWindowMode();
        if (windowMode == WindowMode::WINDOW_MODE_FULLSCREEN || windowMode == WindowMode::WINDOW_MODE_SPLIT_PRIMARY ||
            windowMode == WindowMode::WINDOW_MODE_SPLIT_SECONDARY) {
            auto avoidArea = GetAvoidAreaByType(lastKeyboardAreaUpdatedWindow, AvoidAreaType::TYPE_KEYBOARD);
            UpdateAvoidAreaIfNeed(avoidArea, lastKeyboardAreaUpdatedWindow, AvoidAreaType::TYPE_KEYBOARD);
        }
    }
    if (callingWindow == nullptr) {
        WLOGFE("callingWindow: %{public}u is nullptr, focusWindow: %{public}u is nullptr.",
            callingWindowId, focusedWindow_);
        return;
    }
    const WindowMode callingWindowMode = callingWindow->GetWindowMode();
    if (callingWindowMode == WindowMode::WINDOW_MODE_FULLSCREEN ||
        callingWindowMode == WindowMode::WINDOW_MODE_SPLIT_PRIMARY ||
        callingWindowMode == WindowMode::WINDOW_MODE_SPLIT_SECONDARY) {
        auto avoidArea = GetAvoidAreaByType(callingWindow, AvoidAreaType::TYPE_KEYBOARD);
        bool res = UpdateAvoidAreaIfNeed(avoidArea, callingWindow, AvoidAreaType::TYPE_KEYBOARD);
        if (res) {
            lastSoftInputKeyboardAreaUpdatedWindowId_ = callingWindow->GetWindowId();
        }
        return;
    }
    WLOGFE("does not have correct callingWindowMode for input method window");
}

void AvoidAreaController::UpdateOverlayWindowIfNeed(const sptr<WindowNode>& node,
    const std::function<bool(sptr<WindowNode>)>& checkFunc)
{
    HITRACE_METER(HITRACE_TAG_WINDOW_MANAGER);
    if (WindowHelper::IsOverlayWindow(node->GetWindowType())) {
        AvoidAreaType type = WindowHelper::IsSystemBarWindow(node->GetWindowType()) ?
            AvoidAreaType::TYPE_SYSTEM : AvoidAreaType::TYPE_KEYBOARD;
        for (auto& appNode : avoidAreaListenerNodes_) {
            if (checkFunc != nullptr && checkFunc(appNode)) {
                bool res = UpdateAvoidAreaIfNeed(GetAvoidAreaByType(appNode, type), appNode, type);
                if (type == AvoidAreaType::TYPE_KEYBOARD && res) {
                    lastSoftInputKeyboardAreaUpdatedWindowId_ = appNode->GetWindowId();
                }
            }
        }
    } else {
        if (avoidAreaListenerNodes_.find(node) == avoidAreaListenerNodes_.end()) {
            WLOGE("window: %{public}u is not in avoidAreaListenerNodes, don't update avoid area.", node->GetWindowId());
            return;
        }
        uint32_t start = static_cast<uint32_t>(AvoidAreaType::TYPE_SYSTEM);
        uint32_t end = static_cast<uint32_t>(AvoidAreaType::TYPE_KEYBOARD);
        for (uint32_t type = start; type <= end; type++) {
            AvoidArea systemAvoidArea = GetAvoidAreaByType(node, static_cast<AvoidAreaType>(type));
            bool res = UpdateAvoidAreaIfNeed(systemAvoidArea, node, static_cast<AvoidAreaType>(type));
            if (res && type == static_cast<uint32_t>(AvoidAreaType::TYPE_KEYBOARD)) {
                lastSoftInputKeyboardAreaUpdatedWindowId_ = node->GetWindowId();
            }
        }
    }
}

bool AvoidAreaController::UpdateAvoidAreaIfNeed(const AvoidArea& avoidArea, const sptr<WindowNode>& node,
    AvoidAreaType avoidAreaType)
{
    auto iter = lastUpdatedAvoidArea_.find(node->GetWindowId());
    bool needUpdate = true;
    if (iter != lastUpdatedAvoidArea_.end()) {
        auto avoidAreaIter = iter->second.find(avoidAreaType);
        if (avoidAreaIter != iter->second.end()) {
            needUpdate = avoidAreaIter->second != avoidArea;
        } else {
            if (avoidArea.isEmptyAvoidArea()) {
                needUpdate = false;
            }
        }
    } else {
        if (avoidArea.isEmptyAvoidArea()) {
            needUpdate = false;
        }
    }
    if (needUpdate) {
        lastUpdatedAvoidArea_[node->GetWindowId()][avoidAreaType] = avoidArea;
        node->GetWindowToken()->UpdateAvoidArea(new AvoidArea(avoidArea), avoidAreaType);
    }
    return needUpdate;
}

AvoidPosType AvoidAreaController::CalculateOverlayRect(const sptr<WindowNode>& node,
    const sptr<WindowNode>& overlayNode, Rect& overlayRect) const
{
    if (node->GetWindowId() == overlayNode->GetWindowId()) {
        WLOGE("overlay not support self. windowId %{public}u", node->GetWindowId());
        return AvoidPosType::AVOID_POS_UNKNOWN;
    }
    const Rect rect = node->GetWindowRect();
    overlayRect = WindowHelper::GetOverlap(overlayNode->GetWindowRect(), rect, rect.posX_, rect.posY_);
    return  GetAvoidPosType(rect, overlayRect);
}

AvoidPosType AvoidAreaController::GetAvoidPosType(const Rect& windowRect, const Rect& overlayRect) const
{
    if (windowRect.width_ == 0 || windowRect.height_ == 0) {
        return AvoidPosType::AVOID_POS_UNKNOWN;
    }
    uint32_t centerX = overlayRect.posX_ + (overlayRect.width_ >> 1);
    uint32_t centerY = overlayRect.posY_ + (overlayRect.height_ >> 1);
    float res1 = float(centerY) - float(windowRect.height_) / float(windowRect.width_) * float(centerX);
    float res2 = float(centerY) + float(windowRect.height_) / float(windowRect.width_)  * float(centerX)
        - float(windowRect.height_);
    if (res1 < 0) {
        if (res2 < 0) {
            return AvoidPosType::AVOID_POS_TOP;
        }
        return AvoidPosType::AVOID_POS_RIGHT;
    }
    if (res2 < 0) {
        return AvoidPosType::AVOID_POS_LEFT;
    }
    return AvoidPosType::AVOID_POS_BOTTOM;
}

void AvoidAreaController::SetAvoidAreaRect(AvoidArea& avoidArea, const Rect& rect, AvoidPosType type) const
{
    switch (type) {
        case AvoidPosType::AVOID_POS_TOP : {
            avoidArea.topRect_ = rect;
            break;
        }
        case AvoidPosType::AVOID_POS_LEFT : {
            avoidArea.leftRect_ = rect;
            break;
        }
        case AvoidPosType::AVOID_POS_RIGHT : {
            avoidArea.rightRect_ = rect;
            break;
        }
        case AvoidPosType::AVOID_POS_BOTTOM : {
            avoidArea.bottomRect_ = rect;
            break;
        }
        default : {
            WLOGFI("default type: %{public}u", type);
        }
    }
}

AvoidArea AvoidAreaController::GetAvoidAreaByType(const sptr<WindowNode>& node, AvoidAreaType avoidAreaType) const
{
    WLOGFI("avoidAreaType: %{public}u", avoidAreaType);
    if (node == nullptr) {
        WLOGFE("invalid WindowNode.");
        return {};
    }
    WindowMode windowMode = node->GetWindowMode();
    if (avoidAreaType != AvoidAreaType::TYPE_KEYBOARD &&
        windowMode != WindowMode::WINDOW_MODE_FULLSCREEN &&
        windowMode != WindowMode::WINDOW_MODE_SPLIT_PRIMARY &&
        windowMode != WindowMode::WINDOW_MODE_SPLIT_SECONDARY) {
        WLOGFI("avoidAreaType: %{public}u, windowMode: %{public}u, return default avoid area.",
            avoidAreaType, windowMode);
        return {};
    }
    switch (avoidAreaType) {
        case AvoidAreaType::TYPE_SYSTEM : {
            return GetAvoidAreaSystemType(node);
        }
        case AvoidAreaType::TYPE_KEYBOARD : {
            return GetAvoidAreaKeyboardType(node);
        }
        case AvoidAreaType::TYPE_CUTOUT : {
            sptr<CutoutInfo> cutoutInfo = DisplayManagerServiceInner::GetInstance().GetCutoutInfo(node->GetDisplayId());
            if (cutoutInfo == nullptr) {
                WLOGFE("there is no cutoutInfo");
                return {};
            }
            std::vector<DMRect> cutoutAreas = cutoutInfo->GetBoundingRects();
            if (cutoutAreas.empty()) {
                WLOGFE("there is no cutout");
                return {};
            }
            // 0 means the index in the vector.
            Rect cutoutAreaRect { cutoutAreas[0].posX_, cutoutAreas[0].posY_,
                cutoutAreas[0].width_, cutoutAreas[0].height_ };
            auto rect = node->GetWindowRect();
            Rect overlayRect = WindowHelper::GetOverlap(cutoutAreaRect, rect, rect.posX_, rect.posY_);
            auto type = GetAvoidPosType(rect, overlayRect);
            AvoidArea avoidArea;
            SetAvoidAreaRect(avoidArea, overlayRect, type);
            return avoidArea;
        }
        default : {
            WLOGFI("cannot find avoidAreaType: %{public}u", avoidAreaType);
            return {};
        }
    }
}

AvoidArea AvoidAreaController::GetAvoidAreaSystemType(const sptr<WindowNode>& node) const
{
    AvoidArea systemAvoidArea;
    Rect statusBarAvoidArea;
    AvoidPosType statusBarAvoidPosType = AvoidPosType::AVOID_POS_UNKNOWN;
    Rect navigationBarAvoidArea;
    AvoidPosType navigationBarAvoidPosType = AvoidPosType::AVOID_POS_UNKNOWN;
    for (auto& iter : overlayWindowMap_) {
        if (iter.second != nullptr) {
            if (iter.second->GetWindowType() == WindowType::WINDOW_TYPE_STATUS_BAR) {
                statusBarAvoidPosType = CalculateOverlayRect(node, iter.second, statusBarAvoidArea);
            }
            if (iter.second->GetWindowType() == WindowType::WINDOW_TYPE_NAVIGATION_BAR) {
                navigationBarAvoidPosType = CalculateOverlayRect(node, iter.second, navigationBarAvoidArea);
            }
        }
    }
    SetAvoidAreaRect(systemAvoidArea, statusBarAvoidArea, statusBarAvoidPosType);
    SetAvoidAreaRect(systemAvoidArea, navigationBarAvoidArea, navigationBarAvoidPosType);
    return systemAvoidArea;
}

AvoidArea AvoidAreaController::GetAvoidAreaKeyboardType(const sptr<WindowNode>& node) const
{
    for (auto& iter : overlayWindowMap_) {
        if (iter.second != nullptr &&
            iter.second->GetWindowType() == WindowType::WINDOW_TYPE_INPUT_METHOD_FLOAT) {
            const uint32_t callingWindowId = iter.second->GetCallingWindow();
            if (callingWindowId != node->GetWindowId() && focusedWindow_ != node->GetWindowId()) {
                WLOGFI("windowId: %{public}u is not focusedWindow: %{public}u or callingWindow: %{public}u",
                       node->GetWindowId(), focusedWindow_, callingWindowId);
                continue;
            }
            Rect avoidAreaRect { 0, 0, 0, 0 };
            AvoidPosType avoidPosType = CalculateOverlayRect(node, iter.second, avoidAreaRect);
            AvoidArea avoidArea;
            SetAvoidAreaRect(avoidArea, avoidAreaRect, avoidPosType);
            return avoidArea;
        }
    }
    return {};
}
}
}
