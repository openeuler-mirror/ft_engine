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
#include "display_group_controller.h"

#include "window_inner_manager.h"
#include "window_manager_hilog.h"
#include "window_node_container.h"

namespace OHOS {
namespace Rosen {
namespace {
    constexpr HiviewDFX::HiLogLabel LABEL = {LOG_CORE, HILOG_DOMAIN_WINDOW, "DisplayGroupController"};
}

void DisplayGroupController::InitNewDisplay(DisplayId displayId)
{
    // system bar map for display
    SysBarNodeMap sysBarNodeMap {
        { WindowType::WINDOW_TYPE_STATUS_BAR,     nullptr },
        { WindowType::WINDOW_TYPE_NAVIGATION_BAR, nullptr },
    };
    sysBarNodeMaps_.insert(std::make_pair(displayId, sysBarNodeMap));

    SysBarTintMap sysBarTintMap {
        { WindowType::WINDOW_TYPE_STATUS_BAR,     SystemBarRegionTint() },
        { WindowType::WINDOW_TYPE_NAVIGATION_BAR, SystemBarRegionTint() },
    };
    sysBarTintMaps_.insert(std::make_pair(displayId, sysBarTintMap));

    // window node maps for display
    std::map<WindowRootNodeType, std::unique_ptr<std::vector<sptr<WindowNode>>>> displayWindowTree;
    displayWindowTree.insert(std::make_pair(WindowRootNodeType::APP_WINDOW_NODE,
        std::make_unique<std::vector<sptr<WindowNode>>>()));
    displayWindowTree.insert(std::make_pair(WindowRootNodeType::ABOVE_WINDOW_NODE,
        std::make_unique<std::vector<sptr<WindowNode>>>()));
    displayWindowTree.insert(std::make_pair(WindowRootNodeType::BELOW_WINDOW_NODE,
        std::make_unique<std::vector<sptr<WindowNode>>>()));
    displayGroupWindowTree_.insert(std::make_pair(displayId, std::move(displayWindowTree)));

    // window pair for display
    auto windowPair = new WindowPair(displayId);
    windowPairMap_.insert(std::make_pair(displayId, windowPair));
}

std::vector<sptr<WindowNode>>* DisplayGroupController::GetWindowNodesByDisplayIdAndRootType(DisplayId displayId,
                                                                                            WindowRootNodeType type)
{
    if (displayGroupWindowTree_.find(displayId) != displayGroupWindowTree_.end()) {
        auto& displayWindowTree = displayGroupWindowTree_[displayId];
        if (displayWindowTree.find(type) != displayWindowTree.end()) {
            return displayWindowTree[type].get();
        }
    }
    return nullptr;
}

void DisplayGroupController::AddWindowNodeOnWindowTree(sptr<WindowNode>& node, WindowRootNodeType rootType)
{
    std::vector<sptr<WindowNode>>* rootNodeVectorPtr = GetWindowNodesByDisplayIdAndRootType(node->GetDisplayId(),
        rootType);
    if (rootNodeVectorPtr != nullptr) {
        rootNodeVectorPtr->push_back(node);
        WLOGFD("add node in node vector of root, displayId: %{public}" PRIu64" windowId: %{public}d, "
            "rootType: %{public}d", node->GetDisplayId(), node->GetWindowId(), rootType);
    } else {
        WLOGFE("add node failed, rootNode vector is empty, windowId: %{public}d, rootType: %{public}d",
            node->GetWindowId(), rootType);
    }
}

void DisplayGroupController::UpdateDisplayGroupWindowTree()
{
    // clear ori window tree of displayGroup
    for (auto& elem : displayGroupWindowTree_) {
        for (auto& nodeVec : elem.second) {
            auto emptyVector = std::vector<sptr<WindowNode>>();
            nodeVec.second->swap(emptyVector);
        }
    }
    std::vector<WindowRootNodeType> rootNodeType = {
        WindowRootNodeType::ABOVE_WINDOW_NODE,
        WindowRootNodeType::APP_WINDOW_NODE,
        WindowRootNodeType::BELOW_WINDOW_NODE
    };
    for (auto& rootType : rootNodeType) {
        auto rootNode = windowNodeContainer_->GetRootNode(rootType);
        if (rootNode == nullptr) {
            WLOGFE("rootNode is nullptr, %{public}d", rootType);
            continue;
        }
        for (auto& node : rootNode->children_) {
            AddWindowNodeOnWindowTree(node, rootType);
        }
    }
}

void DisplayGroupController::ProcessCrossNodes(DisplayId defaultDisplayId, DisplayStateChangeType type)
{
    defaultDisplayId_ = defaultDisplayId;
    for (auto& iter : displayGroupWindowTree_) {
        auto nodeVec = *(iter.second[WindowRootNodeType::APP_WINDOW_NODE]);
        for (auto node : nodeVec) {
            if (node->isShowingOnMultiDisplays_) {
                WLOGFD("process cross node, windowId: %{public}u, displayId: %{public}" PRIu64"",
                    node->GetWindowId(), node->GetDisplayId());
                auto showingDisplays = node->GetShowingDisplays();

                DisplayId newDisplayId;
                if (type == DisplayStateChangeType::SIZE_CHANGE || type == DisplayStateChangeType::UPDATE_ROTATION ||
                    type == DisplayStateChangeType::DISPLAY_COMPRESS) {
                    newDisplayId = node->GetDisplayId();
                } else {
                    newDisplayId = defaultDisplayId;
                }

                for (auto& displayId : showingDisplays) {
                    if (displayId == newDisplayId) {
                        continue;
                    }
                    windowNodeContainer_->RemoveNodeFromRSTree(node, displayId, newDisplayId,
                        WindowUpdateType::WINDOW_UPDATE_ACTIVE);
                }
                // update shown displays and displayId
                MoveCrossNodeToTargetDisplay(node, newDisplayId);
            }
        }
    }
}

void DisplayGroupController::UpdateWindowShowingDisplays(const sptr<WindowNode>& node)
{
    auto leftDisplayId = displayGroupInfo_->GetLeftDisplayId();
    auto rightDisplayId = displayGroupInfo_->GetRightDisplayId();
    auto displayRectMap = displayGroupInfo_->GetAllDisplayRects();
    auto showingDisplays = std::vector<DisplayId>();
    const auto& winRect = node->GetWindowRect();
    for (auto& elem : displayRectMap) {
        auto& curDisplayRect = elem.second;

        // if window is showing in display region
        if (((winRect.posX_ + static_cast<int32_t>(winRect.width_)) > curDisplayRect.posX_) &&
            (winRect.posX_ < (curDisplayRect.posX_ + static_cast<int32_t>(curDisplayRect.width_)))) {
            showingDisplays.push_back(elem.first);
        }
    }

    // if window is not showing on any display, maybe in the left of minPosX display, or the right of maxPosX display
    if (showingDisplays.empty()) {
        if (((winRect.posX_ + static_cast<int32_t>(winRect.width_)) <=
            displayRectMap[leftDisplayId].posX_)) {
            showingDisplays.push_back(leftDisplayId);
        }
        if (winRect.posX_ >=
            (displayRectMap[rightDisplayId].posX_ + static_cast<int32_t>(displayRectMap[rightDisplayId].width_))) {
            showingDisplays.push_back(rightDisplayId);
        }
    }

    // mean that this is cross-display window
    if (showingDisplays.size() > 1) {
        node->isShowingOnMultiDisplays_ = true;
    } else {
        node->isShowingOnMultiDisplays_ = false;
    }
    node->SetShowingDisplays(showingDisplays);
}

void DisplayGroupController::UpdateWindowDisplayIdIfNeeded(const sptr<WindowNode>& node)
{
    // current multi-display is only support left-right combination, maxNum is two
    DisplayId newDisplayId = node->GetDisplayId();
    const auto& curShowingDisplays = node->GetShowingDisplays();
    if (curShowingDisplays.empty()) {
        WLOGFE("id:%{public}u not show on any display!", node->GetWindowId());
        return;
    }
    const auto& winRect = node->GetWindowRect();
    if (curShowingDisplays.size() == 1) {
        newDisplayId = *(curShowingDisplays.begin());
    } else {
        // if more than half width of the window is showing on the display, means the window belongs to this display
        int32_t halfWidth = static_cast<int32_t>(winRect.width_ * 0.5);
        const auto& displayRectMap = displayGroupInfo_->GetAllDisplayRects();
        for (auto& elem : displayRectMap) {
            auto& displayRect = elem.second;
            if ((winRect.posX_ < displayRect.posX_) &&
                (winRect.posX_ + static_cast<int32_t>(winRect.width_) >
                displayRect.posX_ + static_cast<int32_t>(displayRect.width_))) { // window covers whole display region
                newDisplayId = elem.first;
                break;
            }
            if (winRect.posX_ >= displayRect.posX_) { // current display is default display
                if ((displayRect.posX_ + static_cast<int32_t>(displayRect.width_) - winRect.posX_) >= halfWidth) {
                    newDisplayId = elem.first;
                    break;
                }
            } else { // current display is expand display
                if ((winRect.posX_ + static_cast<int32_t>(winRect.width_) - displayRect.posX_) >= halfWidth) {
                    newDisplayId = elem.first;
                    break;
                }
            }
        }
    }

    // update displayId if needed
    if (node->GetDisplayId() != newDisplayId) {
        UpdateWindowDisplayId(node, newDisplayId);
        UpdateDisplayGroupWindowTree();
    }
}

void DisplayGroupController::ChangeToRectInDisplayGroup(const sptr<WindowNode>& node, DisplayId displayId)
{
    Rect requestRect = node->GetRequestRect();
    const Rect& displayRect = displayGroupInfo_->GetDisplayRect(displayId);
    requestRect.posX_ += displayRect.posX_;
    requestRect.posY_ += displayRect.posY_;
    node->SetRequestRect(requestRect);

    std::vector<DisplayId> curShowingDisplays = { node->GetDisplayId() };
    node->SetShowingDisplays(curShowingDisplays);
}

void DisplayGroupController::PreProcessWindowNode(const sptr<WindowNode>& node, WindowUpdateType type)
{
    if (!windowNodeContainer_->GetLayoutPolicy()->IsMultiDisplay()) {
        if (type == WindowUpdateType::WINDOW_UPDATE_ADDED) {
            std::vector<DisplayId> curShowingDisplays = { node->GetDisplayId() };
            node->SetShowingDisplays(curShowingDisplays);
            for (auto& childNode : node->children_) {
                PreProcessWindowNode(childNode, type);
            }
        }
        WLOGFD("Current mode is not multi-display");
        return;
    }

    switch (type) {
        case WindowUpdateType::WINDOW_UPDATE_ADDED: {
            if (!node->isShowingOnMultiDisplays_) {
                // change rect to rect in display group
                ChangeToRectInDisplayGroup(node, node->GetDisplayId());
            }
            WLOGFD("preprocess node when add window");
            break;
        }
        case WindowUpdateType::WINDOW_UPDATE_ACTIVE: {
            // MoveTo can be called by user, calculate rect in display group if the reason is move
            if (node->GetWindowSizeChangeReason() == WindowSizeChangeReason::MOVE) {
                ChangeToRectInDisplayGroup(node, defaultDisplayId_);
            }
            WLOGFD("preprocess node when update window");
            break;
        }
        default:
            break;
    }

    for (auto& childNode : node->children_) {
        PreProcessWindowNode(childNode, type);
    }
}

void DisplayGroupController::PostProcessWindowNode(const sptr<WindowNode>& node)
{
    if (!windowNodeContainer_->GetLayoutPolicy()->IsMultiDisplay()) {
        WLOGFD("Current mode is not multi-display");
        return;
    }

    UpdateWindowShowingDisplays(node);
    UpdateWindowDisplayIdIfNeeded(node);
}

void DisplayGroupController::UpdateWindowDisplayId(const sptr<WindowNode>& node, DisplayId newDisplayId)
{
    WLOGFD("update node displayId, srcDisplayId: %{public}" PRIu64", newDisplayId: %{public}" PRIu64"",
        node->GetDisplayId(), newDisplayId);
    if (node->GetWindowToken()) {
        node->GetWindowToken()->UpdateDisplayId(node->GetDisplayId(), newDisplayId);
    }
    node->SetDisplayId(newDisplayId);
}

void DisplayGroupController::MoveCrossNodeToTargetDisplay(const sptr<WindowNode>& node, DisplayId targetDisplayId)
{
    node->isShowingOnMultiDisplays_ = false;
    // update showing display
    std::vector<DisplayId> newShowingDisplays = { targetDisplayId };
    node->SetShowingDisplays(newShowingDisplays);
    // update new displayId
    if (node->GetDisplayId() != targetDisplayId) {
        UpdateWindowDisplayId(node, targetDisplayId);
    }

    for (auto& childNode : node->children_) {
        MoveCrossNodeToTargetDisplay(childNode, targetDisplayId);
    }
}

void DisplayGroupController::MoveNotCrossNodeToDefaultDisplay(const sptr<WindowNode>& node, DisplayId displayId)
{
    WLOGFD("windowId: %{public}d, displayId: %{public}" PRIu64"", node->GetWindowId(), displayId);
    // update new rect in display group
    const Rect& srcDisplayRect = displayGroupInfo_->GetDisplayRect(displayId);
    const Rect& dstDisplayRect = displayGroupInfo_->GetDisplayRect(defaultDisplayId_);
    Rect newRect = node->GetRequestRect();
    if (node->GetWindowType() == WindowType::WINDOW_TYPE_POINTER) {
        newRect.posX_ = static_cast<int32_t>(dstDisplayRect.width_ / 2); // default pointerX : displayRect.width / 2
        newRect.posY_ = static_cast<int32_t>(dstDisplayRect.height_ / 2); // default pointerY : displayRect.height / 2
    } else {
        newRect.posX_ = newRect.posX_ - srcDisplayRect.posX_ + dstDisplayRect.posX_;
        newRect.posY_ = newRect.posY_ - srcDisplayRect.posY_ + dstDisplayRect.posY_;
    }

    node->SetRequestRect(newRect);
    // update showing display
    std::vector<DisplayId> newShowingDisplays = { defaultDisplayId_ };
    node->SetShowingDisplays(newShowingDisplays);
    // update new displayId
    UpdateWindowDisplayId(node, defaultDisplayId_);

    for (auto& childNode : node->children_) {
        MoveNotCrossNodeToDefaultDisplay(childNode, displayId);
    }
}

void DisplayGroupController::ProcessNotCrossNodesOnDestroyedDisplay(DisplayId displayId,
                                                                    std::vector<uint32_t>& windowIds)
{
    if (displayId == defaultDisplayId_) {
        WLOGFE("Move window nodes failed, displayId is the same as defaultDisplayId");
        return;
    }
    if (displayGroupWindowTree_.find(displayId) == displayGroupWindowTree_.end()) {
        WLOGFE("displayId: %{public}" PRIu64" not in display group window tree", displayId);
        return;
    }
    WLOGFI("move window nodes for display destroy, displayId: %{public}" PRIu64"", displayId);

    std::vector<WindowRootNodeType> rootNodeType = {
        WindowRootNodeType::ABOVE_WINDOW_NODE,
        WindowRootNodeType::APP_WINDOW_NODE,
        WindowRootNodeType::BELOW_WINDOW_NODE
    };
    for (const auto& type : rootNodeType) {
        if (displayGroupWindowTree_[displayId].find(type) == displayGroupWindowTree_[displayId].end()) {
            continue;
        }
        auto nodesVec = *(displayGroupWindowTree_[displayId][type]);
        for (auto node : nodesVec) {
            WLOGFD("node on destroied display, windowId: %{public}d, isShowingOnMulti: %{public}d",
                node->GetWindowId(), node->isShowingOnMultiDisplays_);
            if (node->GetDisplayId() != displayId || node->isShowingOnMultiDisplays_) {
                continue;
            }
            // destroy status and navigation bar
            if (node->GetWindowType() == WindowType::WINDOW_TYPE_STATUS_BAR ||
                node->GetWindowType() == WindowType::WINDOW_TYPE_NAVIGATION_BAR) {
                windowNodeContainer_->DestroyWindowNode(node, windowIds);
                WLOGFW("destroy status or navigation bar on destroyed display, windowId: %{public}d",
                    node->GetWindowId());
                continue;
            }
            // move not cross-display nodes to default display
            MoveNotCrossNodeToDefaultDisplay(node, displayId);

            // update RS tree
            windowNodeContainer_->RemoveNodeFromRSTree(node, displayId, defaultDisplayId_,
                WindowUpdateType::WINDOW_UPDATE_ACTIVE);
            windowNodeContainer_->AddNodeOnRSTree(node, defaultDisplayId_, defaultDisplayId_,
                WindowUpdateType::WINDOW_UPDATE_ACTIVE);
        }
    }
}

void DisplayGroupController::ProcessDisplayCreate(DisplayId defaultDisplayId, sptr<DisplayInfo> displayInfo,
                                                  const std::map<DisplayId, Rect>& displayRectMap)
{
    WindowInnerManager::GetInstance().NotifyDisplayChange(displayRectMap);
    defaultDisplayId_ = defaultDisplayId;
    WLOGFI("defaultDisplay, displayId: %{public}" PRIu64"", defaultDisplayId);

    DisplayId displayId = displayInfo->GetDisplayId();

    InitNewDisplay(displayId);

    // add displayInfo in displayGroupInfo
    displayGroupInfo_->AddDisplayInfo(displayInfo);

    // modify RSTree and window tree of displayGroup for cross-display nodes
    ProcessCrossNodes(defaultDisplayId, DisplayStateChangeType::CREATE);
    UpdateDisplayGroupWindowTree();
    const auto& layoutPolicy = windowNodeContainer_->GetLayoutPolicy();
    layoutPolicy->ProcessDisplayCreate(displayId, displayRectMap);
    Rect initialDividerRect = layoutPolicy->GetDividerRect(displayId);
    SetDividerRect(displayId, initialDividerRect);
}

void DisplayGroupController::ProcessDisplayDestroy(DisplayId defaultDisplayId, sptr<DisplayInfo> displayInfo,
                                                   const std::map<DisplayId, Rect>& displayRectMap,
                                                   std::vector<uint32_t>& windowIds)
{
    WindowInnerManager::GetInstance().NotifyDisplayChange(displayRectMap);
    DisplayId displayId = displayInfo->GetDisplayId();

    // delete nodes and map element of deleted display
    ProcessNotCrossNodesOnDestroyedDisplay(displayId, windowIds);
    // modify RSTree and window tree of displayGroup for cross-display nodes
    ProcessCrossNodes(defaultDisplayId, DisplayStateChangeType::DESTROY);
    UpdateDisplayGroupWindowTree();
    ClearMapOfDestroyedDisplay(displayId);
    windowNodeContainer_->GetLayoutPolicy()->ProcessDisplayDestroy(displayId, displayRectMap);
}

void DisplayGroupController::UpdateNodeSizeChangeReasonWithRotation(DisplayId displayId)
{
    std::vector<WindowRootNodeType> rootNodeType = {
        WindowRootNodeType::ABOVE_WINDOW_NODE,
        WindowRootNodeType::APP_WINDOW_NODE,
        WindowRootNodeType::BELOW_WINDOW_NODE
    };
    for (auto& rootType : rootNodeType) {
        std::vector<sptr<WindowNode>>* rootNodeVectorPtr = GetWindowNodesByDisplayIdAndRootType(displayId, rootType);
        if (rootNodeVectorPtr == nullptr) {
            WLOGFE("rootNodeVectorPtr is nullptr, %{public}d, displayId: %{public}" PRIu64, rootType, displayId);
            return;
        }
        for (auto& node : (*rootNodeVectorPtr)) {
            // DOCK_SLICE not need do rotation animation
            if (node->GetWindowType() == WindowType::WINDOW_TYPE_DOCK_SLICE) {
                continue;
            }
            node->SetWindowSizeChangeReason(WindowSizeChangeReason::ROTATION);
        }
    }
}

void DisplayGroupController::ProcessDisplayChange(DisplayId defaultDisplayId, sptr<DisplayInfo> displayInfo,
                                                  const std::map<DisplayId, Rect>& displayRectMap,
                                                  DisplayStateChangeType type)
{
    WindowInnerManager::GetInstance().NotifyDisplayChange(displayRectMap);
    DisplayId displayId = displayInfo->GetDisplayId();
    WLOGFI("display change, displayId: %{public}" PRIu64", type: %{public}d", displayId, type);
    switch (type) {
        case DisplayStateChangeType::UPDATE_ROTATION: {
            displayGroupInfo_->SetDisplayRotation(displayId, displayInfo->GetRotation());
            [[fallthrough]];
        }
        case DisplayStateChangeType::DISPLAY_COMPRESS:
        case DisplayStateChangeType::SIZE_CHANGE: {
            ProcessDisplaySizeChangeOrRotation(defaultDisplayId, displayId, displayRectMap, type);
            break;
        }
        case DisplayStateChangeType::VIRTUAL_PIXEL_RATIO_CHANGE: {
            displayGroupInfo_->SetDisplayVirtualPixelRatio(displayId, displayInfo->GetVirtualPixelRatio());
            windowNodeContainer_->GetLayoutPolicy()->LayoutWindowTree(displayId);
            break;
        }
        default: {
            break;
        }
    }
}

void DisplayGroupController::ProcessDisplaySizeChangeOrRotation(DisplayId defaultDisplayId, DisplayId displayId,
    const std::map<DisplayId, Rect>& displayRectMap, DisplayStateChangeType type)
{
    // modify RSTree and window tree of displayGroup for cross-display nodes
    ProcessCrossNodes(defaultDisplayId, type);
    UpdateDisplayGroupWindowTree();
    const auto& layoutPolicy = windowNodeContainer_->GetLayoutPolicy();
    if (layoutPolicy == nullptr) {
        return;
    }
    // update reason after process cross Nodes to get correct display attribution
    UpdateNodeSizeChangeReasonWithRotation(displayId);
    layoutPolicy->ProcessDisplaySizeChangeOrRotation(displayId, displayRectMap);
    Rect curDividerRect = layoutPolicy->GetDividerRect(displayId);
    if (windowPairMap_[displayId] != nullptr) {
        windowPairMap_[displayId]->RotateDividerWindow(curDividerRect);
    }
}

void DisplayGroupController::ClearMapOfDestroyedDisplay(DisplayId displayId)
{
    sysBarTintMaps_.erase(displayId);
    sysBarNodeMaps_.erase(displayId);
    displayGroupWindowTree_.erase(displayId);
    displayGroupInfo_->RemoveDisplayInfo(displayId);
    windowPairMap_.erase(displayId);
}

sptr<WindowPair> DisplayGroupController::GetWindowPairByDisplayId(DisplayId displayId)
{
    if (windowPairMap_.find(displayId) != windowPairMap_.end()) {
        return windowPairMap_[displayId];
    }
    return nullptr;
}

void DisplayGroupController::SetDividerRect(DisplayId displayId, const Rect& rect)
{
    if (windowPairMap_.find(displayId) != windowPairMap_.end()) {
        windowPairMap_[displayId]->SetDividerRect(rect);
    }
}
} // namespace Rosen
} // namespace OHOS
