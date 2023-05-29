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

#include "window_layout_policy_tile.h"
#include <ability_manager_client.h>
#include <hitrace_meter.h>

#include "minimize_app.h"
#include "window_helper.h"
#include "window_inner_manager.h"
#include "window_manager_hilog.h"

namespace OHOS {
namespace Rosen {
namespace {
    constexpr HiviewDFX::HiLogLabel LABEL = {LOG_CORE, HILOG_DOMAIN_WINDOW, "WindowLayoutPolicyTile"};
    constexpr uint32_t EDGE_INTERVAL = 48;
    constexpr uint32_t MID_INTERVAL = 24;
}

WindowLayoutPolicyTile::WindowLayoutPolicyTile(const sptr<DisplayGroupInfo>& displayGroupInfo,
    DisplayGroupWindowTree& displayGroupWindowTree)
    : WindowLayoutPolicy(displayGroupInfo, displayGroupWindowTree)
{
    for (auto& iter : displayGroupInfo_->GetAllDisplayRects()) {
        maxTileWinNumMap_.insert(std::make_pair(iter.first, static_cast<uint32_t>(1)));
    }
}

void WindowLayoutPolicyTile::Launch()
{
    // compute limit rect
    InitAllRects();
    // select app min win in queue, and minimize others
    InitForegroundNodeQueue();
    for (const auto& iter : displayGroupInfo_->GetAllDisplayRects()) {
        DisplayId displayId = iter.first;
        AssignNodePropertyForTileWindows(displayId);
        LayoutForegroundNodeQueue(displayId);
        auto& displayWindowTree = displayGroupWindowTree_[displayId];
        LayoutWindowNodesByRootType(*(displayWindowTree[WindowRootNodeType::BELOW_WINDOW_NODE]));
    }
    WLOGFI("WindowLayoutPolicyTile::Launch");
}

void WindowLayoutPolicyTile::InitAllRects()
{
    displayGroupLimitRect_ = displayGroupRect_;
    for (const auto& iter : displayGroupInfo_->GetAllDisplayRects()) {
        DisplayId displayId = iter.first;
        limitRectMap_[displayId] = iter.second;
        auto& displayWindowTree = displayGroupWindowTree_[displayId];
        LayoutWindowNodesByRootType(*(displayWindowTree[WindowRootNodeType::ABOVE_WINDOW_NODE]));
        InitTileWindowRects(displayId);
    }
}

uint32_t WindowLayoutPolicyTile::GetMaxTileWinNum(DisplayId displayId) const
{
    float virtualPixelRatio = GetVirtualPixelRatio(displayId);
    constexpr uint32_t half = 2;
    uint32_t edgeIntervalVp = static_cast<uint32_t>(EDGE_INTERVAL * half * virtualPixelRatio);
    uint32_t midIntervalVp = static_cast<uint32_t>(MID_INTERVAL * virtualPixelRatio);
    uint32_t minFloatingW = static_cast<uint32_t>(MIN_FLOATING_WIDTH * virtualPixelRatio);
    uint32_t drawableW = limitRectMap_[displayId].width_ - edgeIntervalVp + midIntervalVp;
    return static_cast<uint32_t>(drawableW / (minFloatingW + midIntervalVp));
}

void WindowLayoutPolicyTile::InitTileWindowRects(DisplayId displayId)
{
    float virtualPixelRatio = GetVirtualPixelRatio(displayId);
    uint32_t edgeIntervalVp = static_cast<uint32_t>(EDGE_INTERVAL * virtualPixelRatio);
    uint32_t midIntervalVp = static_cast<uint32_t>(MID_INTERVAL * virtualPixelRatio);

    constexpr float ratio = DEFAULT_ASPECT_RATIO;
    const Rect& limitRect = limitRectMap_[displayId];
    const Rect& displayRect = displayGroupInfo_->GetDisplayRect(displayId);
    constexpr int half = 2;
    maxTileWinNumMap_[displayId]  = GetMaxTileWinNum(displayId);
    WLOGFI("set max tile window num %{public}u", maxTileWinNumMap_[displayId]);
    auto& presetRects = presetRectsMap_[displayId];
    presetRects.clear();
    uint32_t w = displayRect.width_ * ratio;
    uint32_t h = displayRect.height_ * ratio;
    w = w > limitRect.width_ ? limitRect.width_ : w;
    h = h > limitRect.height_ ? limitRect.height_ : h;
    int x = limitRect.posX_ + ((limitRect.width_ - w) / half);
    int y = limitRect.posY_ + ((limitRect.height_ - h) / half);

    std::vector<Rect> single = {{ x, y, w, h }};
    presetRects.emplace_back(single);
    for (uint32_t num = 2; num <= maxTileWinNumMap_[displayId]; num++) { // start calc preset with 2 windows
        w = (limitRect.width_ - edgeIntervalVp * half - midIntervalVp * (num - 1)) / num;
        std::vector<Rect> curLevel;
        for (uint32_t i = 0; i < num; i++) {
            int curX = limitRect.posX_ + edgeIntervalVp + i * (w + midIntervalVp);
            Rect curRect = { curX, y, w, h };
            WLOGFI("presetRects: level %{public}u, id %{public}u, [%{public}d %{public}d %{public}u %{public}u]",
                num, i, curX, y, w, h);
            curLevel.emplace_back(curRect);
        }
        presetRects.emplace_back(curLevel);
    }
}

bool WindowLayoutPolicyTile::IsTileRectSatisfiedWithSizeLimits(const sptr<WindowNode>& node)
{
    if (!WindowHelper::IsMainWindow(node->GetWindowType())) {
        return true;
    }
    const auto& displayId = node->GetDisplayId();
    auto& foregroundNodes = foregroundNodesMap_[displayId];
    auto num = foregroundNodes.size();
    if (num > maxTileWinNumMap_[displayId] || maxTileWinNumMap_[displayId] == 0) {
        return false;
    }

    UpdateWindowSizeLimits(node);

    // find if node already exits in foreground nodes map
    auto iter = std::find_if(foregroundNodes.begin(), foregroundNodes.end(),
                             [node](sptr<WindowNode> foregroundNode) {
                                 return foregroundNode->GetWindowId() == node->GetWindowId();
                             });
    if (iter != foregroundNodes.end()) {
        return true;
    }

    const auto& presetRects = presetRectsMap_[displayId];
    Rect tileRect;
    // if size of foreground nodes is equal to or more than max tile window number
    if (num == maxTileWinNumMap_[displayId]) {
        tileRect = *(presetRects[num - 1].begin());
    } else {  // if size of foreground nodes is less than max tile window number
        tileRect = *(presetRects[num].begin());
    }
    WLOGFI("id %{public}u, tileRect: [%{public}d %{public}d %{public}u %{public}u]",
        node->GetWindowId(), tileRect.posX_, tileRect.posY_, tileRect.width_, tileRect.height_);
    return WindowHelper::IsRectSatisfiedWithSizeLimits(tileRect, node->GetWindowUpdatedSizeLimits());
}

void WindowLayoutPolicyTile::AddWindowNode(const sptr<WindowNode>& node)
{
    HITRACE_METER(HITRACE_TAG_WINDOW_MANAGER);

    if (WindowHelper::IsMainWindow(node->GetWindowType())) {
        DisplayId displayId = node->GetDisplayId();
        ForegroundNodeQueuePushBack(node, displayId);
        AssignNodePropertyForTileWindows(displayId);
        LayoutForegroundNodeQueue(displayId);
    } else {
        UpdateWindowNode(node); // currently, update and add do the same process
    }
}

void WindowLayoutPolicyTile::UpdateWindowNode(const sptr<WindowNode>& node, bool isAddWindow)
{
    HITRACE_METER(HITRACE_TAG_WINDOW_MANAGER);
    WindowLayoutPolicy::UpdateWindowNode(node);
    if (avoidTypes_.find(node->GetWindowType()) != avoidTypes_.end()) {
        DisplayId displayId = node->GetDisplayId();
        InitTileWindowRects(displayId);
        AssignNodePropertyForTileWindows(displayId);
        LayoutForegroundNodeQueue(displayId);
    }
}

void WindowLayoutPolicyTile::RemoveWindowNode(const sptr<WindowNode>& node)
{
    HITRACE_METER(HITRACE_TAG_WINDOW_MANAGER);
    WLOGFI("RemoveWindowNode %{public}u in tile", node->GetWindowId());
    auto type = node->GetWindowType();
    auto displayId = node->GetDisplayId();
    // affect other windows, trigger off global layout
    if (avoidTypes_.find(type) != avoidTypes_.end()) {
        LayoutWindowTree(displayId);
    } else {
        ForegroundNodeQueueRemove(node);
        AssignNodePropertyForTileWindows(displayId);
        LayoutForegroundNodeQueue(displayId);
    }
    UpdateClientRect(node->GetRequestRect(), node, WindowSizeChangeReason::HIDE);
}

void WindowLayoutPolicyTile::LayoutForegroundNodeQueue(DisplayId displayId)
{
    for (auto& node : foregroundNodesMap_[displayId]) {
        Rect winRect = node->GetRequestRect();
        Rect lastRect = node->GetWindowRect();
        node->SetWindowRect(winRect);
        CalcAndSetNodeHotZone(winRect, node);
        UpdateClientRect(winRect, node, node->GetWindowSizeChangeReason());
        UpdateSurfaceBounds(node, winRect, lastRect);
        for (auto& childNode : node->children_) {
            LayoutWindowNode(childNode);
        }
    }
}

void WindowLayoutPolicyTile::InitForegroundNodeQueue()
{
    for (const auto& iter : displayGroupInfo_->GetAllDisplayRects()) {
        DisplayId displayId = iter.first;
        foregroundNodesMap_[displayId].clear();
        const auto& appWindowNodes = *(displayGroupWindowTree_[displayId][WindowRootNodeType::APP_WINDOW_NODE]);
        for (auto& node : appWindowNodes) {
            if (WindowHelper::IsMainWindow(node->GetWindowType())) {
                ForegroundNodeQueuePushBack(node, displayId);
            }
        }
    }
}

void WindowLayoutPolicyTile::ForegroundNodeQueuePushBack(const sptr<WindowNode>& node, DisplayId displayId)
{
    if (node == nullptr) {
        return;
    }
    auto& foregroundNodes = foregroundNodesMap_[displayId];
    auto iter = std::find_if(foregroundNodes.begin(), foregroundNodes.end(),
                             [node](sptr<WindowNode> foregroundNode) {
                                 return foregroundNode->GetWindowId() == node->GetWindowId();
                             });
    if (iter != foregroundNodes.end()) {
        return;
    }

    if (!WindowHelper::IsWindowModeSupported(node->GetModeSupportInfo(), WindowMode::WINDOW_MODE_FLOATING)) {
        WLOGFD("window don't support tile mode, winId: %{public}d", node->GetWindowId());
        MinimizeApp::AddNeedMinimizeApp(node, MinimizeReason::LAYOUT_TILE);
        return;
    }
    WLOGFI("add win in tile, displayId: %{public}" PRIu64", winId: %{public}d", displayId, node->GetWindowId());
    while (!foregroundNodes.empty() && foregroundNodes.size() >= maxTileWinNumMap_[displayId]) {
        auto removeNode = foregroundNodes.front();
        foregroundNodes.pop_front();
        WLOGFI("pop win in queue head for add new win, windowId: %{public}d", removeNode->GetWindowId());
        MinimizeApp::AddNeedMinimizeApp(removeNode, MinimizeReason::LAYOUT_TILE);
    }
    foregroundNodes.push_back(node);
}

void WindowLayoutPolicyTile::ForegroundNodeQueueRemove(const sptr<WindowNode>& node)
{
    if (node == nullptr) {
        return;
    }
    DisplayId displayId = node->GetDisplayId();
    auto& foregroundNodes = foregroundNodesMap_[displayId];
    auto iter = std::find(foregroundNodes.begin(), foregroundNodes.end(), node);
    if (iter != foregroundNodes.end()) {
        WLOGFI("remove win in tile for win id: %{public}d", node->GetWindowId());
        foregroundNodes.erase(iter);
    }
}

void WindowLayoutPolicyTile::AssignNodePropertyForTileWindows(DisplayId displayId)
{
    // set rect for foreground windows
    auto& foregroundNodes = foregroundNodesMap_[displayId];
    uint32_t num = foregroundNodes.size();
    auto& presetRects = presetRectsMap_[displayId];
    if (num > maxTileWinNumMap_[displayId] || num > presetRects.size() || num == 0) {
        WLOGE("invalid tile queue");
        return;
    }
    std::vector<Rect>& presetRect = presetRects[num - 1];
    if (presetRect.size() != num) {
        WLOGE("invalid preset rects");
        return;
    }
    auto rectIt = presetRect.begin();
    std::vector<sptr<WindowNode>> needMinimizeNodes;
    std::vector<sptr<WindowNode>> needRecoverNodes;
    for (auto node : foregroundNodes) {
        auto& rect = (*rectIt);
        if (WindowHelper::IsWindowModeSupported(node->GetModeSupportInfo(), WindowMode::WINDOW_MODE_FLOATING) &&
            WindowHelper::IsRectSatisfiedWithSizeLimits(rect, node->GetWindowUpdatedSizeLimits())) {
            node->SetWindowMode(WindowMode::WINDOW_MODE_FLOATING);
            if (node->GetWindowToken()) {
                node->GetWindowToken()->UpdateWindowMode(WindowMode::WINDOW_MODE_FLOATING);
            }
            node->SetRequestRect(rect);
            node->SetDecoStatus(true);
            WLOGFI("set rect for qwin id: %{public}d [%{public}d %{public}d %{public}d %{public}d]",
                node->GetWindowId(), rect.posX_, rect.posY_, rect.width_, rect.height_);
            rectIt++;
        } else {
            // if foreground nodes is equal to max tileWinNum, means need recover one node before minimize cur node
            if (num == maxTileWinNumMap_[displayId]) {
                auto recoverNode = MinimizeApp::GetRecoverdNodeFromMinimizeList();
                if (recoverNode != nullptr) {
                    needRecoverNodes.push_back(recoverNode);
                }
            }
            needMinimizeNodes.push_back(node);
            MinimizeApp::AddNeedMinimizeApp(node, MinimizeReason::LAYOUT_TILE);
        }
    }
    for (auto& miniNode : needMinimizeNodes) {
        auto iter = std::find(foregroundNodes.begin(), foregroundNodes.end(), miniNode);
        if (iter != foregroundNodes.end()) {
            foregroundNodes.erase(iter);
        }
    }
    needMinimizeNodes.clear();

    for (auto& recNode : needRecoverNodes) {
        foregroundNodes.push_back(recNode);
    }
    needRecoverNodes.clear();
}

void WindowLayoutPolicyTile::UpdateLayoutRect(const sptr<WindowNode>& node)
{
    auto type = node->GetWindowType();
    auto mode = node->GetWindowMode();
    auto flags = node->GetWindowFlags();
    auto property = node->GetWindowProperty();
    if (property == nullptr) {
        WLOGFE("window property is nullptr.");
        return;
    }
    UpdateWindowSizeLimits(node);
    auto decorEnable = property->GetDecorEnable();
    bool needAvoid = (flags & static_cast<uint32_t>(WindowFlag::WINDOW_FLAG_NEED_AVOID));
    bool parentLimit = (flags & static_cast<uint32_t>(WindowFlag::WINDOW_FLAG_PARENT_LIMIT));
    bool subWindow = WindowHelper::IsSubWindow(type) || WindowHelper::IsSystemSubWindow(type);
    bool floatingWindow = (mode == WindowMode::WINDOW_MODE_FLOATING);
    const Rect lastRect = node->GetWindowRect();
    Rect limitRect = displayGroupInfo_->GetDisplayRect(node->GetDisplayId());
    ComputeDecoratedRequestRect(node);
    Rect winRect = node->GetRequestRect();

    WLOGFI("Id:%{public}u, avoid:%{public}d parLimit:%{public}d floating:%{public}d, sub:%{public}d, " \
        "deco:%{public}d, type:%{public}u, requestRect:[%{public}d, %{public}d, %{public}u, %{public}u]",
        node->GetWindowId(), needAvoid, parentLimit, floatingWindow, subWindow, decorEnable,
        static_cast<uint32_t>(type), winRect.posX_, winRect.posY_, winRect.width_, winRect.height_);
    if (needAvoid) {
        limitRect = limitRectMap_[node->GetDisplayId()];
    }

    if (!floatingWindow) { // fullscreen window
        winRect = limitRect;
    } else { // floating window
        if (subWindow && parentLimit && node->parent_) { // subwindow and limited by parent
            limitRect = node->parent_->GetWindowRect();
            UpdateFloatingLayoutRect(limitRect, winRect);
        }
    }

    LimitFloatingWindowSize(node, displayGroupInfo_->GetDisplayRect(node->GetDisplayId()), winRect);
    LimitMainFloatingWindowPosition(node, winRect);

    node->SetWindowRect(winRect);
    CalcAndSetNodeHotZone(winRect, node);
    // update Node Bounds before reset Reason
    UpdateSurfaceBounds(node, winRect, lastRect);
    UpdateClientRectAndResetReason(node, winRect);
}
} // Rosen
} // OHOS
