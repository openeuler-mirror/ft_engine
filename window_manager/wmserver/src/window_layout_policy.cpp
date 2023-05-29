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

#include "window_layout_policy.h"
#include "display_manager_service_inner.h"
#include "remote_animation.h"
#include "window_helper.h"
#include "window_manager_hilog.h"
#include "wm_common_inner.h"

namespace OHOS {
namespace Rosen {
namespace {
    constexpr HiviewDFX::HiLogLabel LABEL = {LOG_CORE, HILOG_DOMAIN_WINDOW, "WindowLayoutPolicy"};
}

uint32_t WindowLayoutPolicy::floatingBottomPosY_ = 0;

WindowLayoutPolicy::WindowLayoutPolicy(const sptr<DisplayGroupInfo>& displayGroupInfo,
    DisplayGroupWindowTree& displayGroupWindowTree)
    : displayGroupInfo_(displayGroupInfo), displayGroupWindowTree_(displayGroupWindowTree)
{
}

void WindowLayoutPolicy::Launch()
{
    WLOGFD("WindowLayoutPolicy::Launch");
}

void WindowLayoutPolicy::Clean()
{
    WLOGFD("WindowLayoutPolicy::Clean");
}

void WindowLayoutPolicy::Reorder()
{
    WLOGFD("WindowLayoutPolicy::Reorder");
}

std::vector<int32_t> WindowLayoutPolicy::GetExitSplitPoints(DisplayId displayId) const
{
    return {};
}

void WindowLayoutPolicy::LimitWindowToBottomRightCorner(const sptr<WindowNode>& node)
{
    Rect windowRect = node->GetRequestRect();
    Rect displayRect = displayGroupInfo_->GetDisplayRect(node->GetDisplayId());
    windowRect.posX_ = std::max(windowRect.posX_, displayRect.posX_);
    windowRect.posY_ = std::max(windowRect.posY_, displayRect.posY_);
    windowRect.width_ = std::min(windowRect.width_, displayRect.width_);
    windowRect.height_ = std::min(windowRect.height_, displayRect.height_);

    if (windowRect.posX_ + static_cast<int32_t>(windowRect.width_) >
        displayRect.posX_ + static_cast<int32_t>(displayRect.width_)) {
        windowRect.posX_ = displayRect.posX_ + static_cast<int32_t>(displayRect.width_) -
            static_cast<int32_t>(windowRect.width_);
    }

    if (windowRect.posY_ + static_cast<int32_t>(windowRect.height_) >
        displayRect.posY_ + static_cast<int32_t>(displayRect.height_)) {
        windowRect.posY_ = displayRect.posY_ + static_cast<int32_t>(displayRect.height_) -
            static_cast<int32_t>(windowRect.height_);
    }
    node->SetRequestRect(windowRect);

    WLOGFD("windowId: %{public}d, newRect: [%{public}d, %{public}d, %{public}d, %{public}d]",
        node->GetWindowId(), windowRect.posX_, windowRect.posY_, windowRect.width_, windowRect.height_);

    for (auto& childNode : node->children_) {
        LimitWindowToBottomRightCorner(childNode);
    }
}

void WindowLayoutPolicy::UpdateDisplayGroupRect()
{
    Rect newDisplayGroupRect = { 0, 0, 0, 0 };
    // current multi-display is only support left-right combination, maxNum is two
    for (auto& elem : displayGroupInfo_->GetAllDisplayRects()) {
        newDisplayGroupRect.posX_ = std::min(displayGroupRect_.posX_, elem.second.posX_);
        newDisplayGroupRect.posY_ = std::min(displayGroupRect_.posY_, elem.second.posY_);
        newDisplayGroupRect.width_ += elem.second.width_;
        int32_t maxHeight = std::max(newDisplayGroupRect.posY_ + static_cast<int32_t>(newDisplayGroupRect.height_),
                                     elem.second.posY_+ static_cast<int32_t>(elem.second.height_));
        newDisplayGroupRect.height_ = maxHeight - newDisplayGroupRect.posY_;
    }
    displayGroupRect_ = newDisplayGroupRect;
    WLOGFD("displayGroupRect_: [ %{public}d, %{public}d, %{public}d, %{public}d]",
        displayGroupRect_.posX_, displayGroupRect_.posY_, displayGroupRect_.width_, displayGroupRect_.height_);
}

void WindowLayoutPolicy::UpdateDisplayGroupLimitRect()
{
    auto firstDisplayLimitRect = limitRectMap_.begin()->second;
    Rect newDisplayGroupLimitRect = { firstDisplayLimitRect.posX_, firstDisplayLimitRect.posY_, 0, 0 };
    for (auto& elem : limitRectMap_) {
        newDisplayGroupLimitRect.posX_ = std::min(newDisplayGroupLimitRect.posX_, elem.second.posX_);
        newDisplayGroupLimitRect.posY_ = std::min(newDisplayGroupLimitRect.posY_, elem.second.posY_);

        int32_t maxWidth = std::max(newDisplayGroupLimitRect.posX_ +
                                    static_cast<int32_t>(newDisplayGroupLimitRect.width_),
                                    elem.second.posX_+ static_cast<int32_t>(elem.second.width_));

        int32_t maxHeight = std::max(newDisplayGroupLimitRect.posY_ +
                                     static_cast<int32_t>(newDisplayGroupLimitRect.height_),
                                     elem.second.posY_+ static_cast<int32_t>(elem.second.height_));
        newDisplayGroupLimitRect.width_  = static_cast<uint32_t>(maxWidth - newDisplayGroupLimitRect.posX_);
        newDisplayGroupLimitRect.height_ = static_cast<uint32_t>(maxHeight - newDisplayGroupLimitRect.posY_);
    }
    displayGroupLimitRect_ = newDisplayGroupLimitRect;
    WLOGFD("displayGroupLimitRect_: [ %{public}d, %{public}d, %{public}d, %{public}d]",
        displayGroupLimitRect_.posX_, displayGroupLimitRect_.posY_,
        displayGroupLimitRect_.width_, displayGroupLimitRect_.height_);
}

void WindowLayoutPolicy::UpdateRectInDisplayGroup(const sptr<WindowNode>& node,
                                                  const Rect& oriDisplayRect,
                                                  const Rect& newDisplayRect)
{
    Rect newRect = node->GetRequestRect();
    WLOGFD("before update rect in display group, windowId: %{public}d, rect: [%{public}d, %{public}d, "
        "%{public}d, %{public}d]", node->GetWindowId(), newRect.posX_, newRect.posY_, newRect.width_, newRect.height_);

    newRect.posX_ = newRect.posX_ - oriDisplayRect.posX_ + newDisplayRect.posX_;
    newRect.posY_ = newRect.posY_ - oriDisplayRect.posY_ + newDisplayRect.posY_;
    node->SetRequestRect(newRect);
    WLOGFD("after update rect in display group, windowId: %{public}d, newRect: [%{public}d, %{public}d, "
        "%{public}d, %{public}d]", node->GetWindowId(), newRect.posX_, newRect.posY_, newRect.width_, newRect.height_);

    for (auto& childNode : node->children_) {
        UpdateRectInDisplayGroup(childNode, oriDisplayRect, newDisplayRect);
    }
}

bool WindowLayoutPolicy::IsMultiDisplay()
{
    return isMultiDisplay_;
}

void WindowLayoutPolicy::UpdateMultiDisplayFlag()
{
    if (displayGroupInfo_->GetAllDisplayRects().size() > 1) {
        isMultiDisplay_ = true;
        WLOGFD("current mode is multi-display");
    } else {
        isMultiDisplay_ = false;
        WLOGFD("current mode is not multi-display");
    }
}

void WindowLayoutPolicy::UpdateRectInDisplayGroupForAllNodes(DisplayId displayId,
                                                             const Rect& oriDisplayRect,
                                                             const Rect& newDisplayRect)
{
    WLOGFD("displayId: %{public}" PRIu64", oriDisplayRect: [ %{public}d, %{public}d, %{public}d, %{public}d] "
        "newDisplayRect: [ %{public}d, %{public}d, %{public}d, %{public}d]",
        displayId, oriDisplayRect.posX_, oriDisplayRect.posY_, oriDisplayRect.width_, oriDisplayRect.height_,
        newDisplayRect.posX_, newDisplayRect.posY_, newDisplayRect.width_, newDisplayRect.height_);

    auto& displayWindowTree = displayGroupWindowTree_[displayId];
    for (auto& iter : displayWindowTree) {
        auto& nodeVector = *(iter.second);
        for (auto& node : nodeVector) {
            if (!node->isShowingOnMultiDisplays_) {
                UpdateRectInDisplayGroup(node, oriDisplayRect, newDisplayRect);
            }
            if (WindowHelper::IsMainFloatingWindow(node->GetWindowType(), node->GetWindowMode())) {
                LimitWindowToBottomRightCorner(node);
            }
        }
        WLOGFD("Recalculate window rect in display group, displayId: %{public}" PRIu64", rootType: %{public}d",
            displayId, iter.first);
    }
}

void WindowLayoutPolicy::UpdateDisplayRectAndDisplayGroupInfo(const std::map<DisplayId, Rect>& displayRectMap)
{
    for (auto& elem : displayRectMap) {
        auto& displayId = elem.first;
        auto& displayRect = elem.second;
        displayGroupInfo_->SetDisplayRect(displayId, displayRect);
    }
}

void WindowLayoutPolicy::PostProcessWhenDisplayChange()
{
    displayGroupInfo_->UpdateLeftAndRightDisplayId();
    UpdateMultiDisplayFlag();
    UpdateDisplayGroupRect();
    Launch();
}

void WindowLayoutPolicy::ProcessDisplayCreate(DisplayId displayId, const std::map<DisplayId, Rect>& displayRectMap)
{
    const auto& oriDisplayRectMap = displayGroupInfo_->GetAllDisplayRects();
    // check displayId and displayRectMap size
    if (oriDisplayRectMap.find(displayId) == oriDisplayRectMap.end() ||
        displayRectMap.size() != oriDisplayRectMap.size()) {
        WLOGFE("current display is exited or displayInfo map size is error, displayId: %{public}" PRIu64"", displayId);
        return;
    }
    for (auto& elem : displayRectMap) {
        auto iter = oriDisplayRectMap.find(elem.first);
        if (iter != oriDisplayRectMap.end()) {
            const auto& oriDisplayRect = iter->second;
            const auto& newDisplayRect = elem.second;
            UpdateRectInDisplayGroupForAllNodes(elem.first, oriDisplayRect, newDisplayRect);
        } else {
            if (elem.first != displayId) {
                WLOGFE("Wrong display, displayId: %{public}" PRIu64"", displayId);
                return;
            }
        }
    }
    UpdateDisplayRectAndDisplayGroupInfo(displayRectMap);
    PostProcessWhenDisplayChange();
    WLOGFD("Process display create, displayId: %{public}" PRIu64"", displayId);
}

void WindowLayoutPolicy::ProcessDisplayDestroy(DisplayId displayId, const std::map<DisplayId, Rect>& displayRectMap)
{
    const auto& oriDisplayRectMap = displayGroupInfo_->GetAllDisplayRects();
    // check displayId and displayRectMap size
    if (oriDisplayRectMap.find(displayId) != oriDisplayRectMap.end() ||
        displayRectMap.size() != oriDisplayRectMap.size()) {
        WLOGFE("can not find current display or displayInfo map size is error, displayId: %{public}" PRIu64"",
               displayId);
        return;
    }
    for (auto oriIter = oriDisplayRectMap.begin(); oriIter != oriDisplayRectMap.end();) {
        auto newIter = displayRectMap.find(oriIter->first);
        if (newIter != displayRectMap.end()) {
            const auto& oriDisplayRect = oriIter->second;
            const auto& newDisplayRect = newIter->second;
            UpdateRectInDisplayGroupForAllNodes(oriIter->first, oriDisplayRect, newDisplayRect);
        } else {
            if (oriIter->first != displayId) {
                WLOGFE("Wrong display, displayId: %{public}" PRIu64"", displayId);
                return;
            }
        }
        ++oriIter;
    }

    UpdateDisplayRectAndDisplayGroupInfo(displayRectMap);
    PostProcessWhenDisplayChange();
    WLOGFD("Process display destroy, displayId: %{public}" PRIu64"", displayId);
}

void WindowLayoutPolicy::ProcessDisplaySizeChangeOrRotation(DisplayId displayId,
                                                            const std::map<DisplayId, Rect>& displayRectMap)
{
    const auto& oriDisplayRectMap = displayGroupInfo_->GetAllDisplayRects();
    // check displayId and displayRectMap size
    if (oriDisplayRectMap.find(displayId) == oriDisplayRectMap.end() ||
        displayRectMap.size() != oriDisplayRectMap.size()) {
        WLOGFE("can not find current display or displayInfo map size is error, displayId: %{public}" PRIu64"",
               displayId);
        return;
    }

    for (auto& elem : displayRectMap) {
        auto iter = oriDisplayRectMap.find(elem.first);
        if (iter != oriDisplayRectMap.end()) {
            UpdateRectInDisplayGroupForAllNodes(elem.first, iter->second, elem.second);
        }
    }

    UpdateDisplayRectAndDisplayGroupInfo(displayRectMap);
    PostProcessWhenDisplayChange();
    WLOGFD("Process display change, displayId: %{public}" PRIu64"", displayId);
}

void WindowLayoutPolicy::LayoutWindowNodesByRootType(const std::vector<sptr<WindowNode>>& nodeVec)
{
    if (nodeVec.empty()) {
        WLOGE("The node vector is empty!");
        return;
    }
    for (auto& node : nodeVec) {
        LayoutWindowNode(node);
    }
}

void WindowLayoutPolicy::NotifyAnimationSizeChangeIfNeeded()
{
    if (!RemoteAnimation::CheckAnimationController()) {
        WLOGFD("no animation controller!");
        return;
    }
    std::vector<uint32_t> fullScreenWinIds;
    std::vector<uint32_t> floatMainIds;
    for (auto& iter : displayGroupWindowTree_) {
        auto& displayWindowTree = iter.second;
        auto& nodeVec = *(displayWindowTree[WindowRootNodeType::APP_WINDOW_NODE]);
        if (nodeVec.empty()) {
            WLOGE("The node vector is empty!");
            return;
        }
        for (auto& node : nodeVec) {
            // just has one fullscreen app node on foreground
            if (WindowHelper::IsMainFullScreenWindow(node->GetWindowType(), node->GetWindowMode())) {
                fullScreenWinIds.emplace_back(node->GetWindowId());
            }
            if (WindowHelper::IsMainFloatingWindow(node->GetWindowType(), node->GetWindowMode())) {
                floatMainIds.emplace_back(node->GetWindowId());
            }
        }
    }
    RemoteAnimation::NotifyAnimationTargetsUpdate(fullScreenWinIds, floatMainIds);
}

void WindowLayoutPolicy::LayoutWindowTree(DisplayId displayId)
{
    auto& displayWindowTree = displayGroupWindowTree_[displayId];
    limitRectMap_[displayId] = displayGroupInfo_->GetDisplayRect(displayId);
    // ensure that the avoid area windows are traversed first
    LayoutWindowNodesByRootType(*(displayWindowTree[WindowRootNodeType::ABOVE_WINDOW_NODE]));
    if (IsFullScreenRecentWindowExist(*(displayWindowTree[WindowRootNodeType::ABOVE_WINDOW_NODE]))) {
        WLOGFD("recent window on top, early exit layout tree");
        return;
    }
    LayoutWindowNodesByRootType(*(displayWindowTree[WindowRootNodeType::APP_WINDOW_NODE]));
    LayoutWindowNodesByRootType(*(displayWindowTree[WindowRootNodeType::BELOW_WINDOW_NODE]));
}

void WindowLayoutPolicy::LayoutWindowNode(const sptr<WindowNode>& node)
{
    if (node == nullptr) {
        return;
    }
    WLOGFD("LayoutWindowNode, window[%{public}u]", node->GetWindowId());
    if (node->parent_ != nullptr) { // isn't root node
        if (!node->currentVisibility_) {
            WLOGFD("window[%{public}u] currently not visible, no need layout", node->GetWindowId());
            return;
        }
        UpdateLayoutRect(node);
        if (avoidTypes_.find(node->GetWindowType()) != avoidTypes_.end()) {
            UpdateLimitRect(node, limitRectMap_[node->GetDisplayId()]);
            UpdateDisplayGroupLimitRect();
        }
    }
    for (auto& childNode : node->children_) {
        LayoutWindowNode(childNode);
    }
}

bool WindowLayoutPolicy::IsVerticalDisplay(DisplayId displayId) const
{
    return displayGroupInfo_->GetDisplayRect(displayId).width_ < displayGroupInfo_->GetDisplayRect(displayId).height_;
}

void WindowLayoutPolicy::UpdateClientRect(const Rect& rect, const sptr<WindowNode>& node, WindowSizeChangeReason reason)
{
    if (node->GetWindowToken()) {
        WLOGFD("notify client id: %{public}d, windowRect:[%{public}d, %{public}d, %{public}u, %{public}u], reason: "
            "%{public}u", node->GetWindowId(), rect.posX_, rect.posY_, rect.width_, rect.height_, reason);
        node->GetWindowToken()->UpdateWindowRect(rect, node->GetDecoStatus(), reason);
    }
    NotifyAnimationSizeChangeIfNeeded();
}

void WindowLayoutPolicy::UpdateClientRectAndResetReason(const sptr<WindowNode>& node, const Rect& winRect)
{
    auto reason = node->GetWindowSizeChangeReason();
    UpdateClientRect(winRect, node, reason);
    if ((reason != WindowSizeChangeReason::MOVE) && (node->GetWindowType() != WindowType::WINDOW_TYPE_DOCK_SLICE)) {
        node->ResetWindowSizeChangeReason();
    }
}

void WindowLayoutPolicy::RemoveWindowNode(const sptr<WindowNode>& node)
{
    auto type = node->GetWindowType();
    // affect other windows, trigger off global layout
    if (avoidTypes_.find(type) != avoidTypes_.end()) {
        LayoutWindowTree(node->GetDisplayId());
    } else if (type == WindowType::WINDOW_TYPE_DOCK_SLICE) { // split screen mode
        LayoutWindowTree(node->GetDisplayId());
    }
    UpdateClientRect(node->GetRequestRect(), node, WindowSizeChangeReason::HIDE);
}

void WindowLayoutPolicy::UpdateWindowNode(const sptr<WindowNode>& node, bool isAddWindow)
{
    auto type = node->GetWindowType();
    // affect other windows, trigger off global layout
    if (avoidTypes_.find(type) != avoidTypes_.end()) {
        LayoutWindowTree(node->GetDisplayId());
    } else if (type == WindowType::WINDOW_TYPE_DOCK_SLICE) { // split screen mode
        LayoutWindowTree(node->GetDisplayId());
    } else { // layout single window
        LayoutWindowNode(node);
    }
}

void WindowLayoutPolicy::UpdateFloatingLayoutRect(Rect& limitRect, Rect& winRect)
{
    winRect.width_ = std::min(limitRect.width_, winRect.width_);
    winRect.height_ = std::min(limitRect.height_, winRect.height_);
    winRect.posX_ = std::max(limitRect.posX_, winRect.posX_);
    winRect.posY_ = std::max(limitRect.posY_, winRect.posY_);
    winRect.posX_ = std::min(
        limitRect.posX_ + static_cast<int32_t>(limitRect.width_) - static_cast<int32_t>(winRect.width_),
        winRect.posX_);
    winRect.posY_ = std::min(
        limitRect.posY_ + static_cast<int32_t>(limitRect.height_) - static_cast<int32_t>(winRect.height_),
        winRect.posY_);
}

void WindowLayoutPolicy::ComputeDecoratedRequestRect(const sptr<WindowNode>& node) const
{
    auto property = node->GetWindowProperty();
    if (property == nullptr) {
        WLOGE("window property is nullptr");
        return;
    }
    auto reqRect = property->GetRequestRect();
    if (!property->GetDecorEnable() || property->GetDecoStatus() ||
        node->GetWindowSizeChangeReason() == WindowSizeChangeReason::MOVE) {
        return;
    }
    float virtualPixelRatio = GetVirtualPixelRatio(node->GetDisplayId());
    uint32_t winFrameW = static_cast<uint32_t>(WINDOW_FRAME_WIDTH * virtualPixelRatio);
    uint32_t winTitleBarH = static_cast<uint32_t>(WINDOW_TITLE_BAR_HEIGHT * virtualPixelRatio);

    Rect rect;
    rect.posX_ = reqRect.posX_;
    rect.posY_ = reqRect.posY_;
    rect.width_ = reqRect.width_ + winFrameW + winFrameW;
    rect.height_ = reqRect.height_ + winTitleBarH + winFrameW;
    property->SetRequestRect(rect);
    property->SetDecoStatus(true);
}

Rect WindowLayoutPolicy::CalcEntireWindowHotZone(const sptr<WindowNode>& node, const Rect& winRect, uint32_t hotZone,
    float vpr, TransformHelper::Vector2 hotZoneScale) const
{
    Rect rect = winRect;
    uint32_t hotZoneX = static_cast<uint32_t>(hotZone * vpr / hotZoneScale.x_);
    uint32_t hotZoneY = static_cast<uint32_t>(hotZone * vpr / hotZoneScale.y_);

    if (node->GetWindowType() == WindowType::WINDOW_TYPE_DOCK_SLICE) {
        if (rect.width_ < rect.height_) {
            rect.posX_ -= static_cast<int32_t>(hotZoneX);
            rect.width_ += (hotZoneX + hotZoneX);
        } else {
            rect.posY_ -= static_cast<int32_t>(hotZoneY);
            rect.height_ += (hotZoneY + hotZoneY);
        }
    } else if (node->GetWindowType() == WindowType::WINDOW_TYPE_LAUNCHER_RECENT) {
        rect = displayGroupInfo_->GetDisplayRect(node->GetDisplayId());
    } else if (WindowHelper::IsMainFloatingWindow(node->GetWindowType(), node->GetWindowMode())) {
        rect.posX_ -= static_cast<int32_t>(hotZoneX);
        rect.posY_ -= static_cast<int32_t>(hotZoneY);
        rect.width_ += (hotZoneX + hotZoneX);
        rect.height_ += (hotZoneY + hotZoneY);
    }
    return rect;
}

void WindowLayoutPolicy::CalcAndSetNodeHotZone(const Rect& winRect, const sptr<WindowNode>& node) const
{
    float virtualPixelRatio = GetVirtualPixelRatio(node->GetDisplayId());
    TransformHelper::Vector2 hotZoneScale(1, 1);
    if (node->GetWindowProperty()->isNeedComputerTransform()) {
        node->ComputeTransform();
        hotZoneScale = WindowHelper::CalculateHotZoneScale(node->GetWindowProperty()->GetTransformMat());
    }

    auto hotZoneRectTouch = CalcEntireWindowHotZone(node, winRect, HOTZONE_TOUCH, virtualPixelRatio, hotZoneScale);
    auto hotZoneRectPointer = CalcEntireWindowHotZone(node, winRect, HOTZONE_POINTER, virtualPixelRatio, hotZoneScale);

    node->SetEntireWindowTouchHotArea(hotZoneRectTouch);
    node->SetEntireWindowPointerHotArea(hotZoneRectPointer);

    std::vector<Rect> requestedHotAreas;
    node->GetWindowProperty()->GetTouchHotAreas(requestedHotAreas);
    std::vector<Rect> touchHotAreas;
    std::vector<Rect> pointerHotAreas;
    if (requestedHotAreas.empty()) {
        touchHotAreas.emplace_back(hotZoneRectTouch);
        pointerHotAreas.emplace_back(hotZoneRectPointer);
    } else {
        if (!WindowHelper::CalculateTouchHotAreas(winRect, requestedHotAreas, touchHotAreas)) {
            WLOGFW("some parameters in requestedHotAreas are abnormal");
        }
        pointerHotAreas = touchHotAreas;
    }
    node->SetTouchHotAreas(touchHotAreas);
    node->SetPointerHotAreas(pointerHotAreas);
}

void WindowLayoutPolicy::FixWindowSizeByRatioIfDragBeyondLimitRegion(const sptr<WindowNode>& node, Rect& winRect)
{
    const auto& sizeLimits = node->GetWindowUpdatedSizeLimits();
    if (sizeLimits.maxWidth_ == sizeLimits.minWidth_ &&
        sizeLimits.maxHeight_ == sizeLimits.minHeight_) {
        WLOGFD("window rect can not be changed");
        return;
    }
    if (winRect.height_ == 0) {
        WLOGFE("the height of window is zero");
        return;
    }
    float curRatio = static_cast<float>(winRect.width_) / static_cast<float>(winRect.height_);
    if (sizeLimits.minRatio_ <= curRatio && curRatio <= sizeLimits.maxRatio_) {
        WLOGFD("window ratio is satisfied with limit ratio, curRatio: %{public}f", curRatio);
        return;
    }

    float virtualPixelRatio = GetVirtualPixelRatio(node->GetDisplayId());
    uint32_t windowTitleBarH = static_cast<uint32_t>(WINDOW_TITLE_BAR_HEIGHT * virtualPixelRatio);
    Rect limitRect = isMultiDisplay_ ? displayGroupLimitRect_ : limitRectMap_[node->GetDisplayId()];
    int32_t limitMinPosX = limitRect.posX_ + static_cast<int32_t>(windowTitleBarH);
    int32_t limitMaxPosX = limitRect.posX_ + static_cast<int32_t>(limitRect.width_ - windowTitleBarH);
    int32_t limitMinPosY = limitRect.posY_;
    int32_t limitMaxPosY = limitRect.posY_ + static_cast<int32_t>(limitRect.height_ - windowTitleBarH);

    Rect dockWinRect;
    DockWindowShowState dockShownState = GetDockWindowShowState(node->GetDisplayId(), dockWinRect);
    if (dockShownState == DockWindowShowState::SHOWN_IN_BOTTOM) {
        WLOGFD("dock window show in bottom");
        limitMaxPosY = dockWinRect.posY_ - static_cast<int32_t>(windowTitleBarH);
    } else if (dockShownState == DockWindowShowState::SHOWN_IN_LEFT) {
        WLOGFD("dock window show in left");
        limitMinPosX = dockWinRect.posX_ + static_cast<int32_t>(dockWinRect.width_ + windowTitleBarH);
    } else if (dockShownState == DockWindowShowState::SHOWN_IN_RIGHT) {
        WLOGFD("dock window show in right");
        limitMaxPosX = dockWinRect.posX_ - static_cast<int32_t>(windowTitleBarH);
    }

    float newRatio = curRatio < sizeLimits.minRatio_ ? sizeLimits.minRatio_ : sizeLimits.maxRatio_;
    if ((winRect.posX_ + static_cast<int32_t>(winRect.width_) == limitMinPosX) || (winRect.posX_ == limitMaxPosX)) {
        // height can not be changed
        if (sizeLimits.maxHeight_ == sizeLimits.minHeight_) {
            return;
        }
        winRect.height_ = static_cast<uint32_t>(static_cast<float>(winRect.width_) / newRatio);
    }

    if ((winRect.posY_ == limitMinPosY) || (winRect.posX_ == limitMaxPosY)) {
        // width can not be changed
        if (sizeLimits.maxWidth_ == sizeLimits.minWidth_) {
            return;
        }
        winRect.width_ = static_cast<uint32_t>(static_cast<float>(winRect.height_) * newRatio);
    }
    WLOGFD("After limit by ratio if beyond limit region, winRect: %{public}d %{public}d %{public}u %{public}u",
        winRect.posX_, winRect.posY_, winRect.width_, winRect.height_);
}

WindowSizeLimits WindowLayoutPolicy::GetSystemSizeLimits(const sptr<WindowNode>& node,
    const Rect& displayRect, float virtualPixelRatio)
{
    WindowSizeLimits systemLimits;
    systemLimits.maxWidth_ = static_cast<uint32_t>(MAX_FLOATING_SIZE * virtualPixelRatio);
    systemLimits.maxHeight_ = static_cast<uint32_t>(MAX_FLOATING_SIZE * virtualPixelRatio);

    // Float camera window has a special limit:
    // if display sw <= 600dp, portrait: min width = display sw * 30%, landscape: min width = sw * 50%
    // if display sw > 600dp, portrait: min width = display sw * 12%, landscape: min width = sw * 30%
    if (node->GetWindowType() == WindowType::WINDOW_TYPE_FLOAT_CAMERA) {
        uint32_t smallWidth = displayRect.height_ <= displayRect.width_ ? displayRect.height_ : displayRect.width_;
        float hwRatio = static_cast<float>(displayRect.height_) / static_cast<float>(displayRect.width_);
        if (smallWidth <= static_cast<uint32_t>(600 * virtualPixelRatio)) { // sw <= 600dp
            if (displayRect.width_ <= displayRect.height_) {
                systemLimits.minWidth_ = static_cast<uint32_t>(smallWidth * 0.3); // min width = display sw * 0.3
            } else {
                systemLimits.minWidth_ = static_cast<uint32_t>(smallWidth * 0.5); // min width = display sw * 0.5
            }
        } else {
            if (displayRect.width_ <= displayRect.height_) {
                systemLimits.minWidth_ = static_cast<uint32_t>(smallWidth * 0.12); // min width = display sw * 0.12
            } else {
                systemLimits.minWidth_ = static_cast<uint32_t>(smallWidth * 0.3); // min width = display sw * 0.3
            }
        }
        systemLimits.minHeight_ = static_cast<uint32_t>(systemLimits.minWidth_ * hwRatio);
    } else {
        systemLimits.minWidth_ = static_cast<uint32_t>(MIN_FLOATING_WIDTH * virtualPixelRatio);
        systemLimits.minHeight_ = static_cast<uint32_t>(MIN_FLOATING_HEIGHT * virtualPixelRatio);
    }
    WLOGFD("[System SizeLimits] [maxWidth: %{public}u, minWidth: %{public}u, maxHeight: %{public}u, "
        "minHeight: %{public}u]", systemLimits.maxWidth_, systemLimits.minWidth_,
        systemLimits.maxHeight_, systemLimits.minHeight_);
    return systemLimits;
}

void WindowLayoutPolicy::UpdateWindowSizeLimits(const sptr<WindowNode>& node)
{
    const auto& displayRect = displayGroupInfo_->GetDisplayRect(node->GetDisplayId());
    const auto& virtualPixelRatio = GetVirtualPixelRatio(node->GetDisplayId());
    const auto& systemLimits = GetSystemSizeLimits(node, displayRect, virtualPixelRatio);
    const auto& customizedLimits = node->GetWindowSizeLimits();

    WindowSizeLimits newLimits = systemLimits;

    // configured limits of floating window
    uint32_t configuredMaxWidth = static_cast<uint32_t>(customizedLimits.maxWidth_ * virtualPixelRatio);
    uint32_t configuredMaxHeight = static_cast<uint32_t>(customizedLimits.maxHeight_ * virtualPixelRatio);
    uint32_t configuredMinWidth = static_cast<uint32_t>(customizedLimits.minWidth_ * virtualPixelRatio);
    uint32_t configuredMinHeight = static_cast<uint32_t>(customizedLimits.minHeight_ * virtualPixelRatio);

    // calculate new limit size
    if (systemLimits.minWidth_ <= configuredMaxWidth && configuredMaxWidth <= systemLimits.maxWidth_) {
        newLimits.maxWidth_ = configuredMaxWidth;
    }
    if (systemLimits.minHeight_ <= configuredMaxHeight && configuredMaxHeight <= systemLimits.maxHeight_) {
        newLimits.maxHeight_ = configuredMaxHeight;
    }
    if (systemLimits.minWidth_ <= configuredMinWidth && configuredMinWidth <= newLimits.maxWidth_) {
        newLimits.minWidth_ = configuredMinWidth;
    }
    if (systemLimits.minHeight_ <= configuredMinHeight && configuredMinHeight <= newLimits.maxHeight_) {
        newLimits.minHeight_ = configuredMinHeight;
    }

    // calculate new limit ratio
    newLimits.maxRatio_ = static_cast<float>(newLimits.maxWidth_) / static_cast<float>(newLimits.minHeight_);
    newLimits.minRatio_ = static_cast<float>(newLimits.minWidth_) / static_cast<float>(newLimits.maxHeight_);
    if (newLimits.minRatio_ <= customizedLimits.maxRatio_ && customizedLimits.maxRatio_ <= newLimits.maxRatio_) {
        newLimits.maxRatio_ = customizedLimits.maxRatio_;
    }
    if (newLimits.minRatio_ <= customizedLimits.minRatio_ && customizedLimits.minRatio_ <= newLimits.maxRatio_) {
        newLimits.minRatio_ = customizedLimits.minRatio_;
    }

    // recalculate limit size by new ratio
    uint32_t newMaxWidth = static_cast<uint32_t>(static_cast<float>(newLimits.maxHeight_) * newLimits.maxRatio_);
    newLimits.maxWidth_ = std::min(newMaxWidth, newLimits.maxWidth_);
    uint32_t newMinWidth = static_cast<uint32_t>(static_cast<float>(newLimits.minHeight_) * newLimits.minRatio_);
    newLimits.minWidth_ = std::max(newMinWidth, newLimits.minWidth_);
    uint32_t newMaxHeight = static_cast<uint32_t>(static_cast<float>(newLimits.maxWidth_) / newLimits.minRatio_);
    newLimits.maxHeight_ = std::min(newMaxHeight, newLimits.maxHeight_);
    uint32_t newMinHeight = static_cast<uint32_t>(static_cast<float>(newLimits.minWidth_) / newLimits.maxRatio_);
    newLimits.minHeight_ = std::max(newMinHeight, newLimits.minHeight_);

    WLOGFD("[Update SizeLimits] winId: %{public}u, Width: [max:%{public}u, min:%{public}u], Height: [max:%{public}u, "
        "min:%{public}u], Ratio: [max:%{public}f, min:%{public}f]", node->GetWindowId(), newLimits.maxWidth_,
        newLimits.minWidth_, newLimits.maxHeight_, newLimits.minHeight_, newLimits.maxRatio_, newLimits.minRatio_);
    node->SetWindowUpdatedSizeLimits(newLimits);
}

void WindowLayoutPolicy::UpdateFloatingWindowSizeForStretchableWindow(const sptr<WindowNode>& node,
    const Rect& displayRect, Rect& winRect) const
{
    if (node->GetWindowSizeChangeReason() == WindowSizeChangeReason::DRAG) {
        const Rect &originRect = node->GetOriginRect();
        if (originRect.height_ == 0 || originRect.width_ == 0) {
            WLOGE("invalid originRect. window id: %{public}u", node->GetWindowId());
            return;
        }
        auto dragType = node->GetDragType();
        if (dragType == DragType::DRAG_BOTTOM_OR_TOP) {
            // if drag height, use height to fix size.
            winRect.width_ = winRect.height_ * originRect.width_ / originRect.height_;
        } else if (dragType == DragType::DRAG_LEFT_TOP_CORNER || dragType == DragType::DRAG_RIGHT_TOP_CORNER ||
                   dragType == DragType::DRAG_LEFT_OR_RIGHT) {
            // if drag width or corner, use width to fix size.
            winRect.height_ = winRect.width_ * originRect.height_ / originRect.width_;
        }
    }
    // limit minimum size of window

    const auto& sizeLimits = node->GetWindowUpdatedSizeLimits();
    float scale = std::min(static_cast<float>(winRect.width_) / sizeLimits.minWidth_,
        static_cast<float>(winRect.height_) / sizeLimits.minHeight_);
    if (scale == 0) {
        WLOGE("invalid sizeLimits");
        return;
    }
    if (scale < 1.0f) {
        winRect.width_ = static_cast<uint32_t>(static_cast<float>(winRect.width_) / scale);
        winRect.height_ = static_cast<uint32_t>(static_cast<float>(winRect.height_) / scale);
    }
}

void WindowLayoutPolicy::UpdateFloatingWindowSizeBySizeLimits(const sptr<WindowNode>& node,
    const Rect& displayRect, Rect& winRect) const
{
    // get new limit config with the settings of system and app
    const auto& sizeLimits = node->GetWindowUpdatedSizeLimits();

    // limit minimum size of floating (not system type) window
    if (!WindowHelper::IsSystemWindow(node->GetWindowType()) ||
        node->GetWindowType() == WindowType::WINDOW_TYPE_FLOAT_CAMERA) {
        winRect.width_ = std::max(sizeLimits.minWidth_, winRect.width_);
        winRect.height_ = std::max(sizeLimits.minHeight_, winRect.height_);
    }
    winRect.width_ = std::min(sizeLimits.maxWidth_, winRect.width_);
    winRect.height_ = std::min(sizeLimits.maxHeight_, winRect.height_);
    WLOGFD("After limit by size, winRect: %{public}d %{public}d %{public}u %{public}u",
        winRect.posX_, winRect.posY_, winRect.width_, winRect.height_);

    // width and height can not be changed
    if (sizeLimits.maxWidth_ == sizeLimits.minWidth_ &&
        sizeLimits.maxHeight_ == sizeLimits.minHeight_) {
        winRect.width_ = sizeLimits.maxWidth_;
        winRect.height_ = sizeLimits.maxHeight_;
        WLOGFD("window rect can not be changed");
        return;
    }

    float curRatio = static_cast<float>(winRect.width_) / static_cast<float>(winRect.height_);
    // there is no need to fix size by ratio if this is not main floating window
    if (!WindowHelper::IsMainFloatingWindow(node->GetWindowType(), node->GetWindowMode()) ||
        (sizeLimits.minRatio_ <= curRatio && curRatio <= sizeLimits.maxRatio_)) {
        WLOGFD("window is system window or ratio is satisfied with limits, curSize: [%{public}d, %{public}d], "
            "curRatio: %{public}f", winRect.width_, winRect.height_, curRatio);
        return;
    }

    float newRatio = curRatio < sizeLimits.minRatio_ ? sizeLimits.minRatio_ : sizeLimits.maxRatio_;
    if (sizeLimits.maxWidth_ == sizeLimits.minWidth_) {
        winRect.height_ = static_cast<uint32_t>(static_cast<float>(winRect.width_) / newRatio);
        return;
    }
    if (sizeLimits.maxHeight_ == sizeLimits.minHeight_) {
        winRect.width_ = static_cast<uint32_t>(static_cast<float>(winRect.height_) * newRatio);
        return;
    }

    auto dragType = node->GetDragType();
    if (dragType == DragType::DRAG_BOTTOM_OR_TOP) {
        // if drag height, use height to fix size.
        winRect.width_ = static_cast<uint32_t>(static_cast<float>(winRect.height_) * newRatio);
    } else {
        // if drag width or corner, use width to fix size.
        winRect.height_ = static_cast<uint32_t>(static_cast<float>(winRect.width_) / newRatio);
    }
    WLOGFD("After limit by customize config, winRect: %{public}d %{public}d %{public}u %{public}u",
        winRect.posX_, winRect.posY_, winRect.width_, winRect.height_);
}

void WindowLayoutPolicy::LimitFloatingWindowSize(const sptr<WindowNode>& node,
                                                 const Rect& displayRect,
                                                 Rect& winRect) const
{
    if (node->GetWindowMode() != WindowMode::WINDOW_MODE_FLOATING
        || node->GetWindowType() == WindowType::WINDOW_TYPE_APP_COMPONENT) {
        return;
    }
    Rect oriWinRect = winRect;
    UpdateFloatingWindowSizeBySizeLimits(node, displayRect, winRect);

    if (node->GetStretchable() &&
        WindowHelper::IsMainFloatingWindow(node->GetWindowType(), node->GetWindowMode())) {
        UpdateFloatingWindowSizeForStretchableWindow(node, displayRect, winRect);
    }

    // fix size in case of moving window when dragging
    const auto& lastWinRect = node->GetWindowRect();
    if (node->GetWindowSizeChangeReason() == WindowSizeChangeReason::DRAG) {
        if (oriWinRect.posX_ != lastWinRect.posX_) {
            winRect.posX_ = oriWinRect.posX_ + static_cast<int32_t>(oriWinRect.width_) -
                static_cast<int32_t>(winRect.width_);
        }
        if (oriWinRect.posY_ != lastWinRect.posY_) {
            winRect.posY_ = oriWinRect.posY_ + static_cast<int32_t>(oriWinRect.height_) -
                static_cast<int32_t>(winRect.height_);
        }
    }
}

void WindowLayoutPolicy::LimitMainFloatingWindowPosition(const sptr<WindowNode>& node, Rect& winRect) const
{
    if (!WindowHelper::IsMainFloatingWindow(node->GetWindowType(), node->GetWindowMode())) {
        return;
    }

    auto reason = node->GetWindowSizeChangeReason();
    // if drag or move window, limit size and position
    if (reason == WindowSizeChangeReason::DRAG) {
        LimitWindowPositionWhenDrag(node, winRect);
        if (WindowHelper::IsMainFloatingWindow(node->GetWindowType(), node->GetWindowMode())) {
            const_cast<WindowLayoutPolicy*>(this)->FixWindowSizeByRatioIfDragBeyondLimitRegion(node, winRect);
        }
    } else {
        // Limit window position, such as init window rect when show
        LimitWindowPositionWhenInitRectOrMove(node, winRect);
    }
}

void WindowLayoutPolicy::LimitWindowPositionWhenDrag(const sptr<WindowNode>& node,
                                                     Rect& winRect) const
{
    float virtualPixelRatio = GetVirtualPixelRatio(node->GetDisplayId());
    uint32_t windowTitleBarH = static_cast<uint32_t>(WINDOW_TITLE_BAR_HEIGHT * virtualPixelRatio);
    const Rect& lastRect = node->GetWindowRect();
    Rect oriWinRect = winRect;

    Rect limitRect = isMultiDisplay_ ? displayGroupLimitRect_ : limitRectMap_[node->GetDisplayId()];
    int32_t limitMinPosX = limitRect.posX_ + static_cast<int32_t>(windowTitleBarH);
    int32_t limitMaxPosX = limitRect.posX_ + static_cast<int32_t>(limitRect.width_ - windowTitleBarH);
    int32_t limitMinPosY = limitRect.posY_;
    int32_t limitMaxPosY = limitRect.posY_ + static_cast<int32_t>(limitRect.height_ - windowTitleBarH);

    Rect dockWinRect;
    DockWindowShowState dockShownState = GetDockWindowShowState(node->GetDisplayId(), dockWinRect);
    if (dockShownState == DockWindowShowState::SHOWN_IN_BOTTOM) {
        WLOGFD("dock window show in bottom");
        limitMaxPosY = dockWinRect.posY_ - static_cast<int32_t>(windowTitleBarH);
    } else if (dockShownState == DockWindowShowState::SHOWN_IN_LEFT) {
        WLOGFD("dock window show in left");
        limitMinPosX = dockWinRect.posX_ + static_cast<int32_t>(dockWinRect.width_ + windowTitleBarH);
    } else if (dockShownState == DockWindowShowState::SHOWN_IN_RIGHT) {
        WLOGFD("dock window show in right");
        limitMaxPosX = dockWinRect.posX_ - static_cast<int32_t>(windowTitleBarH);
    }

    // limitMinPosX is minimum (x + width)
    if (oriWinRect.posX_ + static_cast<int32_t>(oriWinRect.width_) < limitMinPosX) {
        if (oriWinRect.width_ != lastRect.width_) {
            winRect.width_ = static_cast<uint32_t>(limitMinPosX - oriWinRect.posX_);
        }
    }
    // maximum position x
    if (oriWinRect.posX_ > limitMaxPosX) {
        winRect.posX_ = limitMaxPosX;
        if (oriWinRect.width_ != lastRect.width_) {
            winRect.width_ = static_cast<uint32_t>(
                oriWinRect.posX_ + static_cast<int32_t>(oriWinRect.width_) - winRect.posX_);
        }
    }
    // minimum position y
    if (oriWinRect.posY_ < limitMinPosY) {
        winRect.posY_ = limitMinPosY;
        if (oriWinRect.height_ != lastRect.height_) {
            winRect.height_ = static_cast<uint32_t>(
                oriWinRect.posY_ + static_cast<int32_t>(oriWinRect.height_) - winRect.posY_);
        }
    }
    // maximum position y
    if (winRect.posY_ > limitMaxPosY) {
        winRect.posY_ = limitMaxPosY;
        if (oriWinRect.height_ != lastRect.height_) {
            winRect.height_ = static_cast<uint32_t>(
                oriWinRect.posY_ + static_cast<int32_t>(oriWinRect.height_) - winRect.posY_);
        }
    }
    WLOGFD("After limit by position, winRect: %{public}d %{public}d %{public}u %{public}u",
        winRect.posX_, winRect.posY_, winRect.width_, winRect.height_);
}

void WindowLayoutPolicy::LimitWindowPositionWhenInitRectOrMove(const sptr<WindowNode>& node, Rect& winRect) const
{
    float virtualPixelRatio = GetVirtualPixelRatio(node->GetDisplayId());
    uint32_t windowTitleBarH = static_cast<uint32_t>(WINDOW_TITLE_BAR_HEIGHT * virtualPixelRatio);

    Rect limitRect;
    // if is cross-display window, the limit rect should be full limitRect
    if (node->isShowingOnMultiDisplays_) {
        limitRect = displayGroupLimitRect_;
    } else {
        limitRect = limitRectMap_[node->GetDisplayId()];
    }

    // limit position of the main floating window(window which support dragging)
    if (WindowHelper::IsMainFloatingWindow(node->GetWindowType(), node->GetWindowMode())) {
        Rect dockWinRect;
        DockWindowShowState dockShownState = GetDockWindowShowState(node->GetDisplayId(), dockWinRect);
        winRect.posY_ = std::max(limitRect.posY_, winRect.posY_);
        winRect.posY_ = std::min(limitRect.posY_ + static_cast<int32_t>(limitRect.height_ - windowTitleBarH),
                                 winRect.posY_);
        if (dockShownState == DockWindowShowState::SHOWN_IN_BOTTOM) {
            WLOGFD("dock window show in bottom");
            winRect.posY_ = std::min(dockWinRect.posY_ - static_cast<int32_t>(windowTitleBarH),
                                     winRect.posY_);
        }
        winRect.posX_ = std::max(limitRect.posX_ + static_cast<int32_t>(windowTitleBarH - winRect.width_),
                                 winRect.posX_);
        if (dockShownState == DockWindowShowState::SHOWN_IN_LEFT) {
            WLOGFD("dock window show in left");
            winRect.posX_ = std::max(static_cast<int32_t>(dockWinRect.width_ + windowTitleBarH - winRect.width_),
                                     winRect.posX_);
        }
        winRect.posX_ = std::min(limitRect.posX_ + static_cast<int32_t>(limitRect.width_ - windowTitleBarH),
                                 winRect.posX_);
        if (dockShownState == DockWindowShowState::SHOWN_IN_RIGHT) {
            WLOGFD("dock window show in right");
            winRect.posX_ = std::min(dockWinRect.posX_ - static_cast<int32_t>(windowTitleBarH),
                                     winRect.posX_);
        }
        auto reason = node->GetWindowSizeChangeReason();
        // if init window on pc, limit position
        if (floatingBottomPosY_ != 0 && reason == WindowSizeChangeReason::UNDEFINED) {
            uint32_t bottomPosY = static_cast<uint32_t>(floatingBottomPosY_ * virtualPixelRatio);
            if (winRect.posY_ + static_cast<int32_t>(winRect.height_) >= bottomPosY) {
                winRect.posY_ = limitRect.posY_;
            }
        }
    }
    WLOGFD("After limit by position if init or move, winRect: %{public}d %{public}d %{public}u %{public}u",
        winRect.posX_, winRect.posY_, winRect.width_, winRect.height_);
}

DockWindowShowState WindowLayoutPolicy::GetDockWindowShowState(DisplayId displayId, Rect& dockWinRect) const
{
    auto& displayWindowTree = displayGroupWindowTree_[displayId];
    auto& nodeVec = *(displayWindowTree[WindowRootNodeType::ABOVE_WINDOW_NODE]);
    for (auto& node : nodeVec) {
        if (node->GetWindowType() != WindowType::WINDOW_TYPE_LAUNCHER_DOCK) {
            continue;
        }

        dockWinRect = node->GetWindowRect();
        auto displayRect = displayGroupInfo_->GetDisplayRect(displayId);
        WLOGFD("begin dockWinRect :[%{public}d, %{public}d, %{public}u, %{public}u]",
            dockWinRect.posX_, dockWinRect.posY_, dockWinRect.width_, dockWinRect.height_);
        if (dockWinRect.height_ < dockWinRect.width_) {
            if (static_cast<uint32_t>(dockWinRect.posY_) + dockWinRect.height_ == displayRect.height_) {
                return DockWindowShowState::SHOWN_IN_BOTTOM;
            } else {
                return DockWindowShowState::NOT_SHOWN;
            }
        } else {
            if (dockWinRect.posX_ == 0) {
                return DockWindowShowState::SHOWN_IN_LEFT;
            } else if (static_cast<uint32_t>(dockWinRect.posX_) + dockWinRect.width_ == displayRect.width_) {
                return DockWindowShowState::SHOWN_IN_RIGHT;
            } else {
                return DockWindowShowState::NOT_SHOWN;
            }
        }
    }
    return DockWindowShowState::NOT_SHOWN;
}

AvoidPosType WindowLayoutPolicy::GetAvoidPosType(const Rect& rect, DisplayId displayId) const
{
    const auto& displayRectMap = displayGroupInfo_->GetAllDisplayRects();
    if (displayRectMap.find(displayId) == std::end(displayRectMap)) {
        WLOGFE("GetAvoidPosType fail. Get display fail. displayId: %{public}" PRIu64"", displayId);
        return AvoidPosType::AVOID_POS_UNKNOWN;
    }
    const auto& displayRect = displayGroupInfo_->GetDisplayRect(displayId);
    return WindowHelper::GetAvoidPosType(rect, displayRect);
}

void WindowLayoutPolicy::UpdateLimitRect(const sptr<WindowNode>& node, Rect& limitRect)
{
    const auto& layoutRect = node->GetWindowRect();
    int32_t limitH = static_cast<int32_t>(limitRect.height_);
    int32_t limitW = static_cast<int32_t>(limitRect.width_);
    int32_t layoutH = static_cast<int32_t>(layoutRect.height_);
    int32_t layoutW = static_cast<int32_t>(layoutRect.width_);
    if (node->GetWindowType() == WindowType::WINDOW_TYPE_STATUS_BAR ||
        node->GetWindowType() == WindowType::WINDOW_TYPE_NAVIGATION_BAR) {
        auto avoidPosType = GetAvoidPosType(layoutRect, node->GetDisplayId());
        int32_t offsetH = 0;
        int32_t offsetW = 0;
        switch (avoidPosType) {
            case AvoidPosType::AVOID_POS_TOP:
                offsetH = layoutRect.posY_ + layoutH - limitRect.posY_;
                limitRect.posY_ += offsetH;
                limitH -= offsetH;
                break;
            case AvoidPosType::AVOID_POS_BOTTOM:
                offsetH = limitRect.posY_ + limitH - layoutRect.posY_;
                limitH -= offsetH;
                break;
            case AvoidPosType::AVOID_POS_LEFT:
                offsetW = layoutRect.posX_ + layoutW - limitRect.posX_;
                limitRect.posX_ += offsetW;
                limitW -= offsetW;
                break;
            case AvoidPosType::AVOID_POS_RIGHT:
                offsetW = limitRect.posX_ + limitW - layoutRect.posX_;
                limitW -= offsetW;
                break;
            default:
                WLOGFE("invalid avoidPosType: %{public}d", avoidPosType);
        }
    }
    limitRect.height_ = static_cast<uint32_t>(limitH < 0 ? 0 : limitH);
    limitRect.width_ = static_cast<uint32_t>(limitW < 0 ? 0 : limitW);
    WLOGFD("Type: %{public}d, limitRect: %{public}d %{public}d %{public}u %{public}u",
        node->GetWindowType(), limitRect.posX_, limitRect.posY_, limitRect.width_, limitRect.height_);
}

void WindowLayoutPolicy::Reset()
{
}

float WindowLayoutPolicy::GetVirtualPixelRatio(DisplayId displayId) const
{
    float virtualPixelRatio = displayGroupInfo_->GetDisplayVirtualPixelRatio(displayId);
    WLOGFD("GetVirtualPixel success. displayId:%{public}" PRIu64", vpr:%{public}f", displayId, virtualPixelRatio);
    return virtualPixelRatio;
}

bool WindowLayoutPolicy::IsFullScreenRecentWindowExist(const std::vector<sptr<WindowNode>>& nodeVec) const
{
    for (auto& node : nodeVec) {
        if (node->GetWindowType() == WindowType::WINDOW_TYPE_LAUNCHER_RECENT &&
            node->GetWindowMode() == WindowMode::WINDOW_MODE_FULLSCREEN) {
            return true;
        }
    }
    return false;
}

static void SetBounds(const sptr<WindowNode>& node, const Rect& winRect, const Rect& preRect)
{
    if (node->GetWindowType() == WindowType::WINDOW_TYPE_APP_COMPONENT ||
        node->GetWindowSizeChangeReason() == WindowSizeChangeReason::TRANSFORM) {
        WLOGFD("not need to update bounds");
        return;
    }
    // set surface node gravity based on WindowSizeChangeReason
    if (node->GetWindowSizeChangeReason() == WindowSizeChangeReason::DRAG_START ||
        node->GetWindowSizeChangeReason() == WindowSizeChangeReason::DRAG ||
        node->GetWindowSizeChangeReason() == WindowSizeChangeReason::ROTATION) {
        if (node->surfaceNode_) {
            node->surfaceNode_->SetFrameGravity(Gravity::RESIZE);
        }
    } else {
        if (node->surfaceNode_) {
            node->surfaceNode_->SetFrameGravity(Gravity::TOP_LEFT);
        }
    }
    WLOGFD("name:%{public}s id:%{public}u preRect:[x:%{public}d, y:%{public}d, w:%{public}d, h:%{public}d]",
        node->GetWindowName().c_str(), node->GetWindowId(),
        preRect.posX_, preRect.posY_, preRect.width_, preRect.height_);
    WLOGFD("name:%{public}s id:%{public}u winRect:[x:%{public}d, y:%{public}d, w:%{public}d, h:%{public}d]",
        node->GetWindowName().c_str(), node->GetWindowId(),
        winRect.posX_, winRect.posY_, winRect.width_, winRect.height_);
    if (node->leashWinSurfaceNode_) {
        if (winRect != preRect) {
            // avoid animation interpreted when client coming
            node->leashWinSurfaceNode_->SetBounds(winRect.posX_, winRect.posY_, winRect.width_, winRect.height_);
        }
        if (node->startingWinSurfaceNode_) {
            node->startingWinSurfaceNode_->SetBounds(0, 0, winRect.width_, winRect.height_);
        }
        if (node->surfaceNode_) {
            node->surfaceNode_->SetBounds(0, 0, winRect.width_, winRect.height_);
        }
    } else if (node->surfaceNode_) {
        node->surfaceNode_->SetBounds(winRect.posX_, winRect.posY_, winRect.width_, winRect.height_);
    }
}

void WindowLayoutPolicy::UpdateSurfaceBounds(const sptr<WindowNode>& node, const Rect& winRect, const Rect& preRect)
{
    wptr<WindowNode> weakNode = node;
    auto SetBoundsFunc = [weakNode, winRect, preRect]() {
        auto winNode = weakNode.promote();
        if (winNode == nullptr) {
            WLOGFD("winNode is nullptr");
            return;
        }
        SetBounds(winNode, winRect, preRect);
    };

    switch (node->GetWindowSizeChangeReason()) {
        case WindowSizeChangeReason::MAXIMIZE:
            [[fallthrough]];
        case WindowSizeChangeReason::RECOVER: {
            const RSAnimationTimingProtocol timingProtocol(400); // animation time
            RSNode::Animate(timingProtocol, RSAnimationTimingCurve::EASE_OUT, SetBoundsFunc);
            break;
        }
        case WindowSizeChangeReason::ROTATION: {
            const RSAnimationTimingProtocol timingProtocol(600); // animation time
            const RSAnimationTimingCurve curve_ = RSAnimationTimingCurve::CreateCubicCurve(
                0.2, 0.0, 0.2, 1.0); // animation curve: cubic [0.2, 0.0, 0.2, 1.0]
            RSNode::Animate(timingProtocol, curve_, SetBoundsFunc);
            break;
        }
        case WindowSizeChangeReason::UNDEFINED:
            [[fallthrough]];
        default:
            SetBoundsFunc();
    }
}

Rect WindowLayoutPolicy::GetDisplayGroupRect() const
{
    return displayGroupRect_;
}

void WindowLayoutPolicy::SetSplitRatioConfig(const SplitRatioConfig& splitRatioConfig)
{
    splitRatioConfig_ = splitRatioConfig;
}

Rect WindowLayoutPolicy::GetDividerRect(DisplayId displayId) const
{
    return INVALID_EMPTY_RECT;
}

bool WindowLayoutPolicy::IsTileRectSatisfiedWithSizeLimits(const sptr<WindowNode>& node)
{
    return true;
}

void WindowLayoutPolicy::SetCascadeRectBottomPosYLimit(uint32_t floatingBottomPosY)
{
    floatingBottomPosY_ = floatingBottomPosY;
}
}
}
