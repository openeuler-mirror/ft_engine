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

#include "window_root.h"
#include <ability_manager_client.h>
#include <cinttypes>
#include <display_power_mgr_client.h>
#include <hisysevent.h>
#include <hitrace_meter.h>
#include <transaction/rs_transaction.h>

#include "display_manager_service_inner.h"
#include "window_helper.h"
#include "window_inner_manager.h"
#include "window_manager_hilog.h"
#include "window_manager_service.h"
#include "window_manager_agent_controller.h"
#include "permission.h"

namespace OHOS {
namespace Rosen {
namespace {
    constexpr HiviewDFX::HiLogLabel LABEL = {LOG_CORE, HILOG_DOMAIN_WINDOW, "WindowRoot"};
}

uint32_t WindowRoot::GetTotalWindowNum() const
{
    return static_cast<uint32_t>(windowNodeMap_.size());
}

sptr<WindowNode> WindowRoot::GetWindowForDumpAceHelpInfo() const
{
    for (auto& iter : windowNodeMap_) {
        if (iter.second->GetWindowType() == WindowType::WINDOW_TYPE_DESKTOP ||
            iter.second->GetWindowType() == WindowType::WINDOW_TYPE_NAVIGATION_BAR ||
            iter.second->GetWindowType() == WindowType::WINDOW_TYPE_STATUS_BAR ||
            iter.second->GetWindowType() == WindowType::WINDOW_TYPE_KEYGUARD) {
            return iter.second;
        }
    }
    return nullptr;
}

ScreenId WindowRoot::GetScreenGroupId(DisplayId displayId, bool& isRecordedDisplay)
{
    for (auto iter : displayIdMap_) {
        auto displayIdVec = iter.second;
        if (std::find(displayIdVec.begin(), displayIdVec.end(), displayId) != displayIdVec.end()) {
            isRecordedDisplay = true;
            return iter.first;
        }
    }
    isRecordedDisplay = false;
    WLOGFE("Current display is not be recorded, displayId: %{public}" PRIu64 "", displayId);
    return DisplayManagerServiceInner::GetInstance().GetScreenGroupIdByDisplayId(displayId);
}

sptr<WindowNodeContainer> WindowRoot::GetOrCreateWindowNodeContainer(DisplayId displayId)
{
    auto container = GetWindowNodeContainer(displayId);
    if (container != nullptr) {
        return container;
    }

    // In case of have no container for default display, create container
    WLOGFD("Create container for current display, displayId: %{public}" PRIu64 "", displayId);
    sptr<DisplayInfo> displayInfo = DisplayManagerServiceInner::GetInstance().GetDisplayById(displayId);
    return CreateWindowNodeContainer(displayInfo);
}

sptr<WindowNodeContainer> WindowRoot::GetWindowNodeContainer(DisplayId displayId)
{
    bool isRecordedDisplay;
    ScreenId displayGroupId = GetScreenGroupId(displayId, isRecordedDisplay);
    auto iter = windowNodeContainerMap_.find(displayGroupId);
    if (iter != windowNodeContainerMap_.end()) {
        // if container exist for screenGroup and display is not be recorded, process expand display
        if (!isRecordedDisplay) {
            sptr<DisplayInfo> displayInfo = DisplayManagerServiceInner::GetInstance().GetDisplayById(displayId);
            // add displayId in displayId vector
            displayIdMap_[displayGroupId].push_back(displayId);
            auto displayRectMap = GetAllDisplayRectsByDMS(displayInfo);
            DisplayId defaultDisplayId = DisplayManagerServiceInner::GetInstance().GetDefaultDisplayId();
            ProcessExpandDisplayCreate(defaultDisplayId, displayInfo, displayRectMap);
        }
        return iter->second;
    }
    return nullptr;
}

sptr<WindowNodeContainer> WindowRoot::CreateWindowNodeContainer(sptr<DisplayInfo> displayInfo)
{
    if (displayInfo == nullptr || !CheckDisplayInfo(displayInfo)) {
        WLOGFE("get display failed or get invalid display info");
        return nullptr;
    }

    DisplayId displayId = displayInfo->GetDisplayId();
    ScreenId displayGroupId = displayInfo->GetScreenGroupId();
    WLOGFD("create new container for display, width: %{public}d, height: %{public}d, "
        "displayGroupId:%{public}" PRIu64", displayId:%{public}" PRIu64"", displayInfo->GetWidth(),
        displayInfo->GetHeight(), displayGroupId, displayId);
    sptr<WindowNodeContainer> container = new WindowNodeContainer(displayInfo, displayGroupId);
    windowNodeContainerMap_.insert(std::make_pair(displayGroupId, container));
    std::vector<DisplayId> displayVec = { displayId };
    displayIdMap_.insert(std::make_pair(displayGroupId, displayVec));
    if (container == nullptr) {
        WLOGFE("create container failed, displayId :%{public}" PRIu64 "", displayId);
        return nullptr;
    }
    container->GetLayoutPolicy()->SetSplitRatioConfig(splitRatioConfig_);
    return container;
}

bool WindowRoot::CheckDisplayInfo(const sptr<DisplayInfo>& display)
{
    const int32_t minWidth = 50;
    const int32_t minHeight = 50;
    const int32_t maxWidth = 7680;
    const int32_t maxHeight = 7680; // 8k resolution
    if (display->GetWidth() < minWidth || display->GetWidth() > maxWidth ||
        display->GetHeight() < minHeight || display->GetHeight() > maxHeight) {
        return false;
    }
    return true;
}

sptr<WindowNode> WindowRoot::GetWindowNode(uint32_t windowId) const
{
    auto iter = windowNodeMap_.find(windowId);
    if (iter == windowNodeMap_.end()) {
        return nullptr;
    }
    return iter->second;
}

void WindowRoot::GetBackgroundNodesByScreenId(ScreenId screenGroupId, std::vector<sptr<WindowNode>>& windowNodes) const
{
    for (const auto& it : windowNodeMap_) {
        if (it.second && screenGroupId == DisplayManagerServiceInner::GetInstance().GetScreenGroupIdByDisplayId(
            it.second->GetDisplayId()) && !it.second->currentVisibility_) {
            windowNodes.push_back(it.second);
        }
    }
}

sptr<WindowNode> WindowRoot::FindWindowNodeWithToken(const sptr<IRemoteObject>& token) const
{
    if (token == nullptr) {
        WLOGFE("token is null");
        return nullptr;
    }
    auto iter = std::find_if(windowNodeMap_.begin(), windowNodeMap_.end(),
        [token](const std::map<uint32_t, sptr<WindowNode>>::value_type& pair) {
            if (!(WindowHelper::IsSubWindow(pair.second->GetWindowType()))) {
                return pair.second->abilityToken_ == token;
            }
            return false;
        });
    if (iter == windowNodeMap_.end()) {
        WLOGFD("cannot find windowNode");
        return nullptr;
    }
    return iter->second;
}

void WindowRoot::AddDeathRecipient(sptr<WindowNode> node)
{
    if (node == nullptr) {
        WLOGFE("AddDeathRecipient failed, node is nullptr");
        return;
    }
    WLOGFD("Add for window: %{public}u", node->GetWindowId());

    auto remoteObject = node->GetWindowToken()->AsObject();
    windowIdMap_.insert(std::make_pair(remoteObject, node->GetWindowId()));

    if (windowDeath_ == nullptr) {
        WLOGFD("failed to create death Recipient ptr WindowDeathRecipient");
        return;
    }
    if (!remoteObject->AddDeathRecipient(windowDeath_)) {
        WLOGFD("failed to add death recipient");
    }
}

WMError WindowRoot::SaveWindow(const sptr<WindowNode>& node)
{
    if (node == nullptr) {
        WLOGFE("add window failed, node is nullptr");
        return WMError::WM_ERROR_NULLPTR;
    }

    WLOGFD("save windowId %{public}u", node->GetWindowId());
    windowNodeMap_.insert(std::make_pair(node->GetWindowId(), node));
    if (node->surfaceNode_ != nullptr) {
        surfaceIdWindowNodeMap_.insert(std::make_pair(node->surfaceNode_->GetId(), node));
    }
    if (node->GetWindowToken()) {
        AddDeathRecipient(node);
    }
    // Register FirstFrame Callback to rs, inform ability to get snapshot
    wptr<WindowNode> weak = node;
    auto firstFrameCompleteCallback = [weak]() {
        auto weakNode = weak.promote();
        if (weakNode == nullptr) {
            WLOGFE("windowNode is nullptr");
            return;
        }
        WindowInnerManager::GetInstance().CompleteFirstFrameDrawing(weakNode);
    };
    if (node->surfaceNode_ && WindowHelper::IsMainWindow(node->GetWindowType())) {
        node->surfaceNode_->SetBufferAvailableCallback(firstFrameCompleteCallback);
    }
    return WMError::WM_OK;
}

WMError WindowRoot::MinimizeStructuredAppWindowsExceptSelf(sptr<WindowNode>& node)
{
    HITRACE_METER_FMT(HITRACE_TAG_WINDOW_MANAGER, "root:MinimizeStructuredAppWindowsExceptSelf");
    auto container = GetOrCreateWindowNodeContainer(node->GetDisplayId());
    if (container == nullptr) {
        WLOGFE("MinimizeAbility failed, window container could not be found");
        return WMError::WM_ERROR_NULLPTR;
    }
    return container->MinimizeStructuredAppWindowsExceptSelf(node);
}

void WindowRoot::MinimizeTargetWindows(std::vector<uint32_t>& windowIds)
{
    for (auto& windowId : windowIds) {
        if (windowNodeMap_.count(windowId) != 0) {
            auto windowNode = windowNodeMap_[windowId];
            if (windowNode->GetWindowType() == WindowType::WINDOW_TYPE_APP_MAIN_WINDOW) {
                MinimizeApp::AddNeedMinimizeApp(windowNode, MinimizeReason::GESTURE_ANIMATION);
            } else {
                WLOGFE("Minimize window failed id: %{public}u, type: %{public}u",
                    windowNode->GetWindowId(), static_cast<uint32_t>(windowNode->GetWindowType()));
            }
        }
    }
}

std::vector<sptr<WindowNode>> WindowRoot::GetSplitScreenWindowNodes(DisplayId displayId)
{
    auto container = GetOrCreateWindowNodeContainer(displayId);
    if (container == nullptr) {
        return {};
    }
    auto displayGroupController = container->GetMultiDisplayController();
    if (displayGroupController == nullptr) {
        return {};
    }
    auto windowPair = displayGroupController->GetWindowPairByDisplayId(displayId);
    if (windowPair == nullptr) {
        return {};
    }
    return windowPair->GetPairedWindows();
}

bool WindowRoot::IsForbidDockSliceMove(DisplayId displayId) const
{
    auto container = const_cast<WindowRoot*>(this)->GetOrCreateWindowNodeContainer(displayId);
    if (container == nullptr) {
        WLOGFE("can't find container");
        return true;
    }
    return container->IsForbidDockSliceMove(displayId);
}

bool WindowRoot::IsDockSliceInExitSplitModeArea(DisplayId displayId) const
{
    auto container = const_cast<WindowRoot*>(this)->GetOrCreateWindowNodeContainer(displayId);
    if (container == nullptr) {
        WLOGFE("can't find container");
        return false;
    }
    return container->IsDockSliceInExitSplitModeArea(displayId);
}

void WindowRoot::ExitSplitMode(DisplayId displayId)
{
    auto container = GetOrCreateWindowNodeContainer(displayId);
    if (container == nullptr) {
        WLOGFE("can't find container");
        return;
    }
    container->ExitSplitMode(displayId);
}

void WindowRoot::AddSurfaceNodeIdWindowNodePair(uint64_t surfaceNodeId, sptr<WindowNode> node)
{
    surfaceIdWindowNodeMap_.insert(std::make_pair(surfaceNodeId, node));
}

void WindowRoot::GetVisibilityWindowInfo(std::vector<sptr<WindowVisibilityInfo>>& infos) const
{
    if (!Permission::IsSystemCalling()) {
        WLOGFE("Get Visible Window Permission Denied");
    }
    VisibleData& VisibleWindow = lastOcclusionData_->GetVisibleData();
    for (auto surfaceId : VisibleWindow) {
        auto iter = surfaceIdWindowNodeMap_.find(surfaceId);
        if (iter == surfaceIdWindowNodeMap_.end()) {
            continue;
        }
        sptr<WindowNode> node = iter->second;
        if (node == nullptr) {
            continue;
        }
        infos.emplace_back(new WindowVisibilityInfo(node->GetWindowId(), node->GetCallingPid(),
            node->GetCallingUid(), true, node->GetWindowType()));
    }
}

std::vector<std::pair<uint64_t, bool>> WindowRoot::GetWindowVisibilityChangeInfo(
    std::shared_ptr<RSOcclusionData> occlusionData)
{
    std::vector<std::pair<uint64_t, bool>> visibilityChangeInfo;
    VisibleData& currentVisibleWindow = occlusionData->GetVisibleData();
    std::sort(currentVisibleWindow.begin(), currentVisibleWindow.end());
    VisibleData& lastVisibleWindow = lastOcclusionData_->GetVisibleData();
    uint32_t i, j;
    i = j = 0;
    for (; i < lastVisibleWindow.size() && j < currentVisibleWindow.size();) {
        if (lastVisibleWindow[i] < currentVisibleWindow[j]) {
            visibilityChangeInfo.emplace_back(lastVisibleWindow[i], false);
            i++;
        } else if (lastVisibleWindow[i] > currentVisibleWindow[j]) {
            visibilityChangeInfo.emplace_back(currentVisibleWindow[j], true);
            j++;
        } else {
            i++;
            j++;
        }
    }
    for (; i < lastVisibleWindow.size(); ++i) {
        visibilityChangeInfo.emplace_back(lastVisibleWindow[i], false);
    }
    for (; j < currentVisibleWindow.size(); ++j) {
        visibilityChangeInfo.emplace_back(currentVisibleWindow[j], true);
    }
    lastOcclusionData_ = occlusionData;
    return visibilityChangeInfo;
}

void WindowRoot::NotifyWindowVisibilityChange(std::shared_ptr<RSOcclusionData> occlusionData)
{
    std::vector<std::pair<uint64_t, bool>> visibilityChangeInfo = GetWindowVisibilityChangeInfo(occlusionData);
    std::vector<sptr<WindowVisibilityInfo>> windowVisibilityInfos;
    bool hasAppWindowChange = false;
    for (const auto& elem : visibilityChangeInfo) {
        uint64_t surfaceId = elem.first;
        bool isVisible = elem.second;
        auto iter = surfaceIdWindowNodeMap_.find(surfaceId);
        if (iter == surfaceIdWindowNodeMap_.end()) {
            continue;
        }
        sptr<WindowNode> node = iter->second;
        if (node == nullptr) {
            continue;
        }
        node->isVisible_ = isVisible;
        WindowType winType = node->GetWindowType();
        hasAppWindowChange = (winType >= WindowType::APP_WINDOW_BASE && winType < WindowType::APP_WINDOW_END);
        windowVisibilityInfos.emplace_back(new WindowVisibilityInfo(node->GetWindowId(), node->GetCallingPid(),
            node->GetCallingUid(), isVisible, node->GetWindowType()));
        WLOGFD("NotifyWindowVisibilityChange: covered status changed window:%{public}u, isVisible:%{public}d",
            node->GetWindowId(), isVisible);
    }
    if (hasAppWindowChange) {
        SwitchRenderModeIfNeeded();
    }

    if (windowVisibilityInfos.size() != 0) {
        WindowManagerAgentController::GetInstance().UpdateWindowVisibilityInfo(windowVisibilityInfos);
    }
}

AvoidArea WindowRoot::GetAvoidAreaByType(uint32_t windowId, AvoidAreaType avoidAreaType)
{
    AvoidArea avoidArea;
    sptr<WindowNode> node = GetWindowNode(windowId);
    if (node == nullptr) {
        WLOGFE("could not find window");
        return avoidArea;
    }
    sptr<WindowNodeContainer> container = GetOrCreateWindowNodeContainer(node->GetDisplayId());
    if (container == nullptr) {
        WLOGFE("add window failed, window container could not be found");
        return avoidArea;
    }
    return container->GetAvoidAreaByType(node, avoidAreaType);
}

void WindowRoot::MinimizeAllAppWindows(DisplayId displayId)
{
    auto container = GetOrCreateWindowNodeContainer(displayId);
    if (container == nullptr) {
        WLOGFE("can't find window node container, failed!");
        return;
    }
    return container->MinimizeAllAppWindows(displayId);
}

WMError WindowRoot::ToggleShownStateForAllAppWindows()
{
    std::vector<DisplayId> displays = DisplayManagerServiceInner::GetInstance().GetAllDisplayIds();
    std::vector<sptr<WindowNodeContainer>> containers;
    bool isAllAppWindowsEmpty = true;
    for (auto displayId : displays) {
        auto container = GetOrCreateWindowNodeContainer(displayId);
        if (container == nullptr) {
            WLOGFE("can't find window node container, failed!");
            continue;
        }
        containers.emplace_back(container);
        isAllAppWindowsEmpty = isAllAppWindowsEmpty && container->IsAppWindowsEmpty();
    }
    WMError res = WMError::WM_OK;
    std::for_each(containers.begin(), containers.end(),
        [this, isAllAppWindowsEmpty, &res] (sptr<WindowNodeContainer> container) {
        auto restoreFunc = [this](uint32_t windowId, WindowMode mode) {
            auto windowNode = GetWindowNode(windowId);
            if (windowNode == nullptr) {
                return false;
            }
            if (!windowNode->GetWindowToken()) {
                return false;
            }
            auto property = windowNode->GetWindowToken()->GetWindowProperty();
            if (property == nullptr) {
                return false;
            }
            if (mode == WindowMode::WINDOW_MODE_SPLIT_PRIMARY ||
                mode == WindowMode::WINDOW_MODE_SPLIT_SECONDARY) {
                property->SetWindowMode(mode);
                windowNode->GetWindowToken()->RestoreSplitWindowMode(static_cast<uint32_t>(mode));
            }
            windowNode->GetWindowToken()->UpdateWindowState(WindowState::STATE_SHOWN);
            WindowManagerService::GetInstance().AddWindow(property);
            return true;
        };
        WMError tmpRes = tmpRes = container->ToggleShownStateForAllAppWindows(restoreFunc, isAllAppWindowsEmpty);
        res = (res == WMError::WM_OK) ? tmpRes : res;
    });
    return res;
}

void WindowRoot::DestroyLeakStartingWindow()
{
    WLOGFD("DestroyLeakStartingWindow is called");
    std::vector<uint32_t> destroyIds;
    for (auto& iter : windowNodeMap_) {
        if (iter.second->startingWindowShown_ && !iter.second->GetWindowToken()) {
            destroyIds.push_back(iter.second->GetWindowId());
        }
    }
    for (auto& id : destroyIds) {
        WLOGFD("Destroy Window id:%{public}u", id);
        DestroyWindow(id, false);
    }
}

WMError WindowRoot::PostProcessAddWindowNode(sptr<WindowNode>& node, sptr<WindowNode>& parentNode,
    sptr<WindowNodeContainer>& container)
{
    if (!node->currentVisibility_) {
        WLOGFD("window is invisible, do not need process");
        return WMError::WM_DO_NOTHING;
    }
    if (WindowHelper::IsSubWindow(node->GetWindowType())) {
        if (parentNode == nullptr) {
            WLOGFE("window type is invalid");
            return WMError::WM_ERROR_INVALID_TYPE;
        }
        sptr<WindowNode> parent = nullptr;
        container->RaiseZOrderForAppWindow(parentNode, parent);
    }
    if (node->GetWindowProperty()->GetFocusable()) {
        // when launcher reboot, the focus window should not change with showing a full screen window.
        sptr<WindowNode> focusWin = GetWindowNode(container->GetFocusWindow());
        if (focusWin == nullptr ||
            !(WindowHelper::IsFullScreenWindow(focusWin->GetWindowMode()) && focusWin->zOrder_ > node->zOrder_)) {
            container->SetFocusWindow(node->GetWindowId());
            needCheckFocusWindow = true;
        }
    }
    if (!WindowHelper::IsSystemBarWindow(node->GetWindowType())) {
        container->SetActiveWindow(node->GetWindowId(), false);
    }

    for (auto& child : node->children_) {
        if (child == nullptr || !child->currentVisibility_) {
            break;
        }
        HandleKeepScreenOn(child->GetWindowId(), child->IsKeepScreenOn());
    }
    HandleKeepScreenOn(node->GetWindowId(), node->IsKeepScreenOn());
    WLOGFD("windowId:%{public}u, name:%{public}s, orientation:%{public}u, type:%{public}u, isMainWindow:%{public}d",
        node->GetWindowId(), node->GetWindowName().c_str(), static_cast<uint32_t>(node->GetRequestedOrientation()),
        node->GetWindowType(), WindowHelper::IsMainWindow(node->GetWindowType()));
    if (WindowHelper::IsRotatableWindow(node->GetWindowType(), node->GetWindowMode())) {
        DisplayManagerServiceInner::GetInstance().
            SetOrientationFromWindow(node->GetDisplayId(), node->GetRequestedOrientation());
    }
    return WMError::WM_OK;
}

bool WindowRoot::NeedToStopAddingNode(sptr<WindowNode>& node, const sptr<WindowNodeContainer>& container)
{
    if (!WindowHelper::IsMainWindow(node->GetWindowType())) {
        return false;
    }
    // intercept the node which doesn't support floating mode at tile mode
    if (WindowHelper::IsInvalidWindowInTileLayoutMode(node->GetModeSupportInfo(), container->GetCurrentLayoutMode())) {
        WLOGFE("window doesn't support floating mode in tile, windowId: %{public}u", node->GetWindowId());
        return true;
    }
    // intercept the node that the tile rect can't be applied to
    WMError res = container->IsTileRectSatisfiedWithSizeLimits(node);
    if (res != WMError::WM_OK) {
        return true;
    }
    return false;
}

Rect WindowRoot::GetDisplayRectWithoutSystemBarAreas(DisplayId displayId)
{
    std::map<WindowType, std::pair<bool, Rect>> systemBarRects;
    for (const auto& it : windowNodeMap_) {
        auto& node = it.second;
        if (node && (node->GetDisplayId() == displayId) &&
            WindowHelper::IsSystemBarWindow(node->GetWindowType())) {
            systemBarRects[node->GetWindowType()] = std::make_pair(node->currentVisibility_, node->GetWindowRect());
        }
    }
    auto container = GetOrCreateWindowNodeContainer(displayId);
    if (container == nullptr) {
        WLOGFE("GetDisplayRectWithoutSystemBarAreas failed, window container could not be found");
        return {0, 0, 0, 0}; // empty rect
    }
    auto displayRect = container->GetDisplayRect(displayId);
    Rect targetRect = displayRect;
    bool isStatusShow = true;
    if (systemBarRects.count(WindowType::WINDOW_TYPE_STATUS_BAR)) {
        isStatusShow = systemBarRects[WindowType::WINDOW_TYPE_STATUS_BAR].first;
        targetRect.posY_ = displayRect.posY_ + static_cast<int32_t>(
            systemBarRects[WindowType::WINDOW_TYPE_STATUS_BAR].second.height_);
        targetRect.height_ -= systemBarRects[WindowType::WINDOW_TYPE_STATUS_BAR].second.height_;
        WLOGFD("after status bar winRect:[x:%{public}d, y:%{public}d, w:%{public}d, h:%{public}d]",
            targetRect.posX_, targetRect.posY_, targetRect.width_, targetRect.height_);
    }
    if (systemBarRects.count(WindowType::WINDOW_TYPE_NAVIGATION_BAR)) {
        if (isStatusShow && !(systemBarRects[WindowType::WINDOW_TYPE_NAVIGATION_BAR].first)) {
            return targetRect;
        }
        targetRect.height_ -= systemBarRects[WindowType::WINDOW_TYPE_NAVIGATION_BAR].second.height_;
        WLOGFD("after navi bar winRect:[x:%{public}d, y:%{public}d, w:%{public}d, h:%{public}d]",
            targetRect.posX_, targetRect.posY_, targetRect.width_, targetRect.height_);
    }
    return targetRect;
}

void WindowRoot::GetAllAnimationPlayingNodes(std::vector<wptr<WindowNode>>& windowNodes)
{
    for (const auto& it : windowNodeMap_) {
        if (it.second) {
            if (!WindowHelper::IsMainWindow(it.second->GetWindowType())) {
                continue;
            }
            WLOGFD("id:%{public}u state:%{public}u",
                it.second->GetWindowId(), static_cast<uint32_t>(it.second->stateMachine_.GetCurrentState()));
            if (it.second->stateMachine_.IsRemoteAnimationPlaying() ||
                it.second->stateMachine_.GetAnimationCount() > 0) {
                windowNodes.emplace_back(it.second);
            }
        }
    }
}

void WindowRoot::LayoutWhenAddWindowNode(sptr<WindowNode>& node, bool afterAnimation)
{
    if (node == nullptr) {
        WLOGFE("LayoutWhenAddWindowNode failed, node is nullptr");
        return;
    }
    auto container = GetOrCreateWindowNodeContainer(node->GetDisplayId());
    if (container == nullptr) {
        WLOGFE("add window failed, window container could not be found");
        return;
    }
    container->LayoutWhenAddWindowNode(node, afterAnimation);
    return;
}

WMError WindowRoot::AddWindowNode(uint32_t parentId, sptr<WindowNode>& node, bool fromStartingWin)
{
    if (node == nullptr) {
        WLOGFE("add window failed, node is nullptr");
        return WMError::WM_ERROR_NULLPTR;
    }

    auto container = GetOrCreateWindowNodeContainer(node->GetDisplayId());
    if (container == nullptr) {
        WLOGFE("add window failed, window container could not be found");
        return WMError::WM_ERROR_NULLPTR;
    }

    if (NeedToStopAddingNode(node, container)) { // true means stop adding
        return WMError::WM_ERROR_INVALID_WINDOW_MODE_OR_SIZE;
    }

    if (fromStartingWin) {
        if (node->GetWindowMode() == WindowMode::WINDOW_MODE_FULLSCREEN &&
            WindowHelper::IsAppWindow(node->GetWindowType()) && !node->isPlayAnimationShow_) {
            container->NotifyDockWindowStateChanged(node, false);
            WMError res = MinimizeStructuredAppWindowsExceptSelf(node);
            if (res != WMError::WM_OK) {
                WLOGFE("Minimize other structured window failed");
                MinimizeApp::ClearNodesWithReason(MinimizeReason::OTHER_WINDOW);
                return res;
            }
        }
        WMError res = container->ShowStartingWindow(node);
        if (res != WMError::WM_OK) {
            MinimizeApp::ClearNodesWithReason(MinimizeReason::OTHER_WINDOW);
        }
        SwitchRenderModeIfNeeded();
        return res;
    }
    // limit number of main window
    uint32_t mainWindowNumber = container->GetWindowCountByType(WindowType::WINDOW_TYPE_APP_MAIN_WINDOW);
    if (mainWindowNumber >= maxAppWindowNumber_ && node->GetWindowType() == WindowType::WINDOW_TYPE_APP_MAIN_WINDOW) {
        container->MinimizeOldestAppWindow();
    }

    auto parentNode = GetWindowNode(parentId);

    if (node->GetWindowType() == WindowType::WINDOW_TYPE_DIALOG) {
        sptr<WindowNode> callerNode = FindDialogCallerNode(node->GetWindowType(), node->dialogTargetToken_);
        parentNode = (callerNode != nullptr) ? callerNode : nullptr;
    }

    WMError res = container->AddWindowNode(node, parentNode);
    if (res != WMError::WM_OK) {
        WLOGFE("AddWindowNode failed with ret: %{public}u", static_cast<uint32_t>(res));
        return res;
    }
    SwitchRenderModeIfNeeded();
    return PostProcessAddWindowNode(node, parentNode, container);
}

WMError WindowRoot::RemoveWindowNode(uint32_t windowId, bool fromAnimation)
{
    auto node = GetWindowNode(windowId);
    if (node == nullptr) {
        WLOGFE("could not find window");
        return WMError::WM_ERROR_NULLPTR;
    }
    auto container = GetOrCreateWindowNodeContainer(node->GetDisplayId());
    if (container == nullptr) {
        WLOGFE("remove window failed, window container could not be found");
        return WMError::WM_ERROR_NULLPTR;
    }
    container->DropShowWhenLockedWindowIfNeeded(node);
    UpdateFocusWindowWithWindowRemoved(node, container);
    UpdateActiveWindowWithWindowRemoved(node, container);
    UpdateBrightnessWithWindowRemoved(windowId, container);
    WMError res = container->RemoveWindowNode(node, fromAnimation);
    if (res == WMError::WM_OK) {
        for (auto& child : node->children_) {
            if (child == nullptr) {
                break;
            }
            HandleKeepScreenOn(child->GetWindowId(), false);
        }
        HandleKeepScreenOn(windowId, false);
        SwitchRenderModeIfNeeded();
    }
    auto nextRotatableWindow = container->GetNextRotatableWindow(windowId);
    if (nextRotatableWindow != nullptr) {
        DisplayManagerServiceInner::GetInstance().SetOrientationFromWindow(nextRotatableWindow->GetDisplayId(),
            nextRotatableWindow->GetRequestedOrientation());
    }
    return res;
}

WMError WindowRoot::UpdateWindowNode(uint32_t windowId, WindowUpdateReason reason)
{
    auto node = GetWindowNode(windowId);
    if (node == nullptr) {
        WLOGFE("could not find window");
        return WMError::WM_ERROR_NULLPTR;
    }
    auto container = GetOrCreateWindowNodeContainer(node->GetDisplayId());
    if (container == nullptr) {
        WLOGFE("update window failed, window container could not be found");
        return WMError::WM_ERROR_NULLPTR;
    }
    return container->UpdateWindowNode(node, reason);
}

WMError WindowRoot::UpdateSizeChangeReason(uint32_t windowId, WindowSizeChangeReason reason)
{
    auto node = GetWindowNode(windowId);
    if (node == nullptr) {
        WLOGFE("could not find window");
        return WMError::WM_ERROR_NULLPTR;
    }
    auto container = GetOrCreateWindowNodeContainer(node->GetDisplayId());
    if (container == nullptr) {
        WLOGFE("update window size change reason failed, window container could not be found");
        return WMError::WM_ERROR_NULLPTR;
    }
    container->UpdateSizeChangeReason(node, reason);
    return WMError::WM_OK;
}

void WindowRoot::SetBrightness(uint32_t windowId, float brightness)
{
    auto node = GetWindowNode(windowId);
    if (node == nullptr) {
        WLOGFE("could not find window");
        return;
    }
    auto container = GetOrCreateWindowNodeContainer(node->GetDisplayId());
    if (container == nullptr) {
        WLOGFE("set brightness failed, window container could not be found");
        return;
    }
    if (!WindowHelper::IsAppWindow(node->GetWindowType())) {
        WLOGFD("non app window does not support set brightness");
        return;
    }
    if (windowId == container->GetActiveWindow()) {
        if (container->GetDisplayBrightness() != brightness) {
            WLOGFD("set brightness with value: %{public}u", container->ToOverrideBrightness(brightness));
            DisplayPowerMgr::DisplayPowerMgrClient::GetInstance().OverrideBrightness(
                container->ToOverrideBrightness(brightness));
            container->SetDisplayBrightness(brightness);
        }
        container->SetBrightnessWindow(windowId);
    }
}

void WindowRoot::HandleKeepScreenOn(uint32_t windowId, bool requireLock)
{
    auto node = GetWindowNode(windowId);
    if (node == nullptr) {
        WLOGFE("could not find window");
        return;
    }
    auto container = GetOrCreateWindowNodeContainer(node->GetDisplayId());
    if (container == nullptr) {
        WLOGFE("handle keep screen on failed, window container could not be found");
        return;
    }
    container->HandleKeepScreenOn(node, requireLock);
}

void WindowRoot::UpdateFocusableProperty(uint32_t windowId)
{
    auto node = GetWindowNode(windowId);
    if (node == nullptr) {
        WLOGFE("could not find window");
        return;
    }
    auto container = GetOrCreateWindowNodeContainer(node->GetDisplayId());
    if (container == nullptr) {
        WLOGFE("handle focusable failed, window container could not be found");
        return;
    }

    if (windowId != container->GetFocusWindow() || node->GetWindowProperty()->GetFocusable()) {
        return;
    }
    auto nextFocusableWindow = container->GetNextFocusableWindow(windowId);
    if (nextFocusableWindow != nullptr) {
        WLOGFD("adjust focus window, next focus window id: %{public}u", nextFocusableWindow->GetWindowId());
        container->SetFocusWindow(nextFocusableWindow->GetWindowId());
    }
}

WMError WindowRoot::SetWindowMode(sptr<WindowNode>& node, WindowMode dstMode)
{
    auto container = GetOrCreateWindowNodeContainer(node->GetDisplayId());
    if (container == nullptr) {
        WLOGFE("set window mode failed, window container could not be found");
        return WMError::WM_ERROR_NULLPTR;
    }
    WindowMode curWinMode = node->GetWindowMode();
    auto res = container->SetWindowMode(node, dstMode);
    if (res == WMError::WM_OK
        && (WindowHelper::IsSplitWindowMode(curWinMode) || WindowHelper::IsSplitWindowMode(dstMode))) {
        WLOGFD("SwitchRender: split mode changed");
        SwitchRenderModeIfNeeded();
    }
    if (WindowHelper::IsRotatableWindow(node->GetWindowType(), node->GetWindowMode())) {
        DisplayManagerServiceInner::GetInstance().
            SetOrientationFromWindow(node->GetDisplayId(), node->GetRequestedOrientation());
    }
    auto nextRotatableWindow = container->GetNextRotatableWindow(0);
    if (nextRotatableWindow != nullptr) {
        DisplayManagerServiceInner::GetInstance().SetOrientationFromWindow(nextRotatableWindow->GetDisplayId(),
            nextRotatableWindow->GetRequestedOrientation());
    }
    return res;
}

WMError WindowRoot::DestroyWindowSelf(sptr<WindowNode>& node, const sptr<WindowNodeContainer>& container)
{
    for (auto& child : node->children_) {
        if (child == nullptr) {
            continue;
        }
        child->parent_ = nullptr;
        if ((child->GetWindowToken() != nullptr) && (child->abilityToken_ != node->abilityToken_) &&
            (child->GetWindowType() == WindowType::WINDOW_TYPE_DIALOG)) {
            child->GetWindowToken()->NotifyDestroy();
        }
    }
    WMError res = container->RemoveWindowNode(node);
    if (res != WMError::WM_OK) {
        WLOGFE("RemoveWindowNode failed");
    }
    SwitchRenderModeIfNeeded();
    return DestroyWindowInner(node);
}

WMError WindowRoot::DestroyWindowWithChild(sptr<WindowNode>& node, const sptr<WindowNodeContainer>& container)
{
    auto token = node->abilityToken_;
    std::vector<uint32_t> windowIds;
    WMError res = container->DestroyWindowNode(node, windowIds);
    for (auto id : windowIds) {
        node = GetWindowNode(id);
        if (!node) {
            continue;
        }
        HandleKeepScreenOn(id, false);
        DestroyWindowInner(node);
        if ((node->GetWindowToken() != nullptr) && (node->abilityToken_ != token) &&
            (node->GetWindowType() == WindowType::WINDOW_TYPE_DIALOG)) {
            node->GetWindowToken()->NotifyDestroy();
        }
    }
    SwitchRenderModeIfNeeded();
    return res;
}

WMError WindowRoot::DestroyWindow(uint32_t windowId, bool onlySelf)
{
    auto node = GetWindowNode(windowId);
    if (node == nullptr) {
        WLOGFE("destroy window failed, because window node is not exist.");
        return WMError::WM_ERROR_NULLPTR;
    }
    WLOGFD("destroy window %{public}u, onlySelf:%{public}u.", windowId, onlySelf);
    auto container = GetOrCreateWindowNodeContainer(node->GetDisplayId());
    if (!container) {
        WLOGFW("destroy window failed, window container could not be found");
        return DestroyWindowInner(node);
    }

    UpdateFocusWindowWithWindowRemoved(node, container);
    UpdateActiveWindowWithWindowRemoved(node, container);
    UpdateBrightnessWithWindowRemoved(windowId, container);
    HandleKeepScreenOn(windowId, false);
    if (onlySelf) {
        return DestroyWindowSelf(node, container);
    } else {
        return DestroyWindowWithChild(node, container);
    }
}

WMError WindowRoot::DestroyWindowInner(sptr<WindowNode>& node)
{
    if (node == nullptr) {
        WLOGFE("window has been destroyed");
        return WMError::WM_ERROR_DESTROYED_OBJECT;
    }

    if (node->isVisible_) {
        std::vector<sptr<WindowVisibilityInfo>> windowVisibilityInfos;
        node->isVisible_ = false;
        windowVisibilityInfos.emplace_back(new WindowVisibilityInfo(node->GetWindowId(), node->GetCallingPid(),
            node->GetCallingUid(), false, node->GetWindowType()));
        WLOGFD("NotifyWindowVisibilityChange: covered status changed window:%{public}u, isVisible:%{public}d",
            node->GetWindowId(), node->isVisible_);
        WindowManagerAgentController::GetInstance().UpdateWindowVisibilityInfo(windowVisibilityInfos);
    }

    auto cmpFunc = [node](const std::map<uint64_t, sptr<WindowNode>>::value_type& pair) {
        if (pair.second == nullptr) {
            return false;
        }
        if (pair.second->GetWindowId() == node->GetWindowId()) {
            return true;
        }
        return false;
    };
    auto iter = std::find_if(surfaceIdWindowNodeMap_.begin(), surfaceIdWindowNodeMap_.end(), cmpFunc);
    if (iter != surfaceIdWindowNodeMap_.end()) {
        surfaceIdWindowNodeMap_.erase(iter);
    }

    sptr<IWindow> window = node->GetWindowToken();
    if ((window != nullptr) && (window->AsObject() != nullptr)) {
        if (windowIdMap_.count(window->AsObject()) == 0) {
            WLOGFD("window remote object has been destroyed");
            return WMError::WM_ERROR_DESTROYED_OBJECT;
        }

        if (window->AsObject() != nullptr) {
            window->AsObject()->RemoveDeathRecipient(windowDeath_);
        }
        windowIdMap_.erase(window->AsObject());
    }
    windowNodeMap_.erase(node->GetWindowId());
    WLOGFD("destroy window node use_count:%{public}d", node->GetSptrRefCount());
    return WMError::WM_OK;
}

void WindowRoot::UpdateFocusWindowWithWindowRemoved(const sptr<WindowNode>& node,
    const sptr<WindowNodeContainer>& container) const
{
    if (node == nullptr || container == nullptr) {
        WLOGFE("window is invalid");
        return;
    }
    if (node->GetWindowType() == WindowType::WINDOW_TYPE_DOCK_SLICE) {
        WLOGFD("window is divider, do not get next focus window.");
        return;
    }
    uint32_t windowId = node->GetWindowId();
    uint32_t focusedWindowId = container->GetFocusWindow();
    WLOGFD("current window: %{public}u, focus window: %{public}u", windowId, focusedWindowId);
    if (WindowHelper::IsMainWindow(node->GetWindowType())) {
        if (windowId != focusedWindowId) {
            auto iter = std::find_if(node->children_.begin(), node->children_.end(),
                                     [focusedWindowId](sptr<WindowNode> node) {
                                         return node->GetWindowId() == focusedWindowId;
                                     });
            if (iter == node->children_.end()) {
                return;
            }
        }
        if (!node->children_.empty()) {
            auto firstChild = node->children_.front();
            if (firstChild->priority_ < 0) {
                windowId = firstChild->GetWindowId();
            }
        }
    } else {
        if (windowId != focusedWindowId) {
            return;
        }
    }
    auto nextFocusableWindow = container->GetNextFocusableWindow(windowId);
    if (nextFocusableWindow != nullptr) {
        WLOGFD("adjust focus window, next focus window id: %{public}u", nextFocusableWindow->GetWindowId());
        container->SetFocusWindow(nextFocusableWindow->GetWindowId());
    }
}

void WindowRoot::UpdateActiveWindowWithWindowRemoved(const sptr<WindowNode>& node,
    const sptr<WindowNodeContainer>& container) const
{
    if (node == nullptr || container == nullptr) {
        WLOGFE("window is invalid");
        return;
    }
    uint32_t windowId = node->GetWindowId();
    uint32_t activeWindowId = container->GetActiveWindow();
    WLOGFD("current window: %{public}u, active window: %{public}u", windowId, activeWindowId);
    if (WindowHelper::IsMainWindow(node->GetWindowType())) {
        if (windowId != activeWindowId) {
            auto iter = std::find_if(node->children_.begin(), node->children_.end(),
                                     [activeWindowId](sptr<WindowNode> node) {
                                         return node->GetWindowId() == activeWindowId;
                                     });
            if (iter == node->children_.end()) {
                return;
            }
        }
        if (!node->children_.empty()) {
            auto firstChild = node->children_.front();
            if (firstChild->priority_ < 0) {
                windowId = firstChild->GetWindowId();
            }
        }
    } else {
        if (windowId != activeWindowId) {
            return;
        }
    }
    auto nextActiveWindow = container->GetNextActiveWindow(windowId);
    if (nextActiveWindow != nullptr) {
        WLOGFD("adjust active window, next active window id: %{public}u", nextActiveWindow->GetWindowId());
        container->SetActiveWindow(nextActiveWindow->GetWindowId(), true);
    }
}

void WindowRoot::UpdateBrightnessWithWindowRemoved(uint32_t windowId, const sptr<WindowNodeContainer>& container) const
{
    if (container == nullptr) {
        WLOGFE("window container could not be found");
        return;
    }
    if (windowId == container->GetBrightnessWindow()) {
        WLOGFD("adjust brightness window with active window: %{public}u", container->GetActiveWindow());
        container->UpdateBrightness(container->GetActiveWindow(), true);
    }
}

bool WindowRoot::IsVerticalDisplay(sptr<WindowNode>& node) const
{
    auto container = const_cast<WindowRoot*>(this)->GetOrCreateWindowNodeContainer(node->GetDisplayId());
    if (container == nullptr) {
        WLOGFE("get display direction failed, window container could not be found");
        return false;
    }
    return container->IsVerticalDisplay(node->GetDisplayId());
}

WMError WindowRoot::RequestFocus(uint32_t windowId)
{
    auto node = GetWindowNode(windowId);
    if (node == nullptr) {
        WLOGFE("could not find window");
        return WMError::WM_ERROR_NULLPTR;
    }
    if (!node->currentVisibility_) {
        WLOGFE("could not request focus before it does not be shown");
        return WMError::WM_ERROR_INVALID_OPERATION;
    }
    auto container = GetOrCreateWindowNodeContainer(node->GetDisplayId());
    if (container == nullptr) {
        WLOGFE("window container could not be found");
        return WMError::WM_ERROR_NULLPTR;
    }
    if (node->GetWindowProperty()->GetFocusable()) {
        return container->SetFocusWindow(windowId);
    }
    return WMError::WM_ERROR_INVALID_OPERATION;
}

WMError WindowRoot::RequestActiveWindow(uint32_t windowId)
{
    auto node = GetWindowNode(windowId);
    if (node == nullptr) {
        WLOGFE("could not find window");
        return WMError::WM_ERROR_NULLPTR;
    }
    if (WindowHelper::IsSystemBarWindow(node->GetWindowType())) {
        WLOGFE("window could not be active window");
        return WMError::WM_ERROR_INVALID_TYPE;
    }
    auto container = GetOrCreateWindowNodeContainer(node->GetDisplayId());
    if (container == nullptr) {
        WLOGFE("window container could not be found");
        return WMError::WM_ERROR_NULLPTR;
    }
    auto res = container->SetActiveWindow(windowId, false);
    WLOGFD("windowId:%{public}u, name:%{public}s, orientation:%{public}u, type:%{public}u, isMainWindow:%{public}d",
        windowId, node->GetWindowName().c_str(), static_cast<uint32_t>(node->GetRequestedOrientation()),
        node->GetWindowType(), WindowHelper::IsMainWindow(node->GetWindowType()));
    if (res == WMError::WM_OK &&
        WindowHelper::IsRotatableWindow(node->GetWindowType(), node->GetWindowMode())) {
        DisplayManagerServiceInner::GetInstance().
            SetOrientationFromWindow(node->GetDisplayId(), node->GetRequestedOrientation());
    }
    return res;
}

void WindowRoot::ProcessWindowStateChange(WindowState state, WindowStateChangeReason reason)
{
    for (auto& elem : windowNodeContainerMap_) {
        if (elem.second == nullptr) {
            continue;
        }
        elem.second->ProcessWindowStateChange(state, reason);
    }
}

void WindowRoot::NotifySystemBarTints()
{
    WLOGFD("notify current system bar tints");
    for (auto& it : windowNodeContainerMap_) {
        if (it.second != nullptr) {
            it.second->NotifySystemBarTints(displayIdMap_[it.first]);
        }
    }
}

WMError WindowRoot::RaiseZOrderForAppWindow(sptr<WindowNode>& node)
{
    if (node == nullptr) {
        WLOGFW("add window failed, node is nullptr");
        return WMError::WM_ERROR_NULLPTR;
    }
    if (node->GetWindowType() == WindowType::WINDOW_TYPE_DOCK_SLICE) {
        auto container = GetOrCreateWindowNodeContainer(node->GetDisplayId());
        if (container == nullptr) {
            WLOGFW("window container could not be found");
            return WMError::WM_ERROR_NULLPTR;
        }
        container->RaiseSplitRelatedWindowToTop(node);
        return WMError::WM_OK;
    }
    if (node->GetWindowType() == WindowType::WINDOW_TYPE_DIALOG) {
        auto container = GetOrCreateWindowNodeContainer(node->GetDisplayId());
        if (container == nullptr) {
            WLOGFW("window container could not be found");
            return WMError::WM_ERROR_NULLPTR;
        }
        sptr<WindowNode> parentNode = FindDialogCallerNode(node->GetWindowType(), node->dialogTargetToken_);
        if (parentNode != nullptr) {
            container->RaiseZOrderForAppWindow(node, parentNode);
        }
        return WMError::WM_OK;
    }

    if (!WindowHelper::IsAppWindow(node->GetWindowType())) {
        WLOGFW("window is not app window");
        return WMError::WM_ERROR_INVALID_TYPE;
    }
    auto container = GetOrCreateWindowNodeContainer(node->GetDisplayId());
    if (container == nullptr) {
        WLOGFW("add window failed, window container could not be found");
        return WMError::WM_ERROR_NULLPTR;
    }

    auto parentNode = GetWindowNode(node->GetParentId());
    return container->RaiseZOrderForAppWindow(node, parentNode);
}

uint32_t WindowRoot::GetWindowIdByObject(const sptr<IRemoteObject>& remoteObject)
{
    auto iter = windowIdMap_.find(remoteObject);
    return iter == std::end(windowIdMap_) ? INVALID_WINDOW_ID : iter->second;
}

void WindowRoot::OnRemoteDied(const sptr<IRemoteObject>& remoteObject)
{
    callback_(Event::REMOTE_DIED, remoteObject);
}

WMError WindowRoot::GetTopWindowId(uint32_t mainWinId, uint32_t& topWinId)
{
    if (windowNodeMap_.find(mainWinId) == windowNodeMap_.end()) {
        return WMError::WM_ERROR_INVALID_WINDOW;
    }
    auto node = windowNodeMap_[mainWinId];
    if (!node->currentVisibility_) {
        return WMError::WM_ERROR_INVALID_WINDOW;
    }
    if (!node->children_.empty()) {
        auto iter = node->children_.rbegin();
        if (WindowHelper::IsSubWindow((*iter)->GetWindowType()) ||
            WindowHelper::IsSystemSubWindow((*iter)->GetWindowType())) {
            topWinId = (*iter)->GetWindowId();
            return WMError::WM_OK;
        }
    }
    topWinId = mainWinId;
    return WMError::WM_OK;
}

WMError WindowRoot::SetWindowLayoutMode(DisplayId displayId, WindowLayoutMode mode)
{
    auto container = GetOrCreateWindowNodeContainer(displayId);
    if (container == nullptr) {
        WLOGFE("window container could not be found");
        return WMError::WM_ERROR_NULLPTR;
    }
    WMError ret = container->SwitchLayoutPolicy(mode, displayId, true);
    if (ret != WMError::WM_OK) {
        WLOGFW("set window layout mode failed displayId: %{public}" PRIu64 ", ret: %{public}d", displayId, ret);
    }
    return ret;
}

std::vector<DisplayId> WindowRoot::GetAllDisplayIds() const
{
    std::vector<DisplayId> displayIds;
    for (auto& it : windowNodeContainerMap_) {
        if (!it.second) {
            return {};
        }
        std::vector<DisplayId>& displayIdVec = const_cast<WindowRoot*>(this)->displayIdMap_[it.first];
        for (auto displayId : displayIdVec) {
            displayIds.push_back(displayId);
        }
    }
    return displayIds;
}

std::string WindowRoot::GenAllWindowsLogInfo() const
{
    std::ostringstream os;
    WindowNodeOperationFunc func = [&os](sptr<WindowNode> node) {
        if (node == nullptr) {
            WLOGE("WindowNode is nullptr");
            return false;
        }
        os<<"window_name:"<<node->GetWindowName()<<",id:"<<node->GetWindowId()<<
           ",focusable:"<<node->GetWindowProperty()->GetFocusable()<<";";
        return false;
    };

    for (auto& elem : windowNodeContainerMap_) {
        if (elem.second == nullptr) {
            continue;
        }
        std::vector<DisplayId>& displayIdVec = const_cast<WindowRoot*>(this)->displayIdMap_[elem.first];
        for (const auto& displayId : displayIdVec) {
            os << "Display " << displayId << ":";
        }
        elem.second->TraverseWindowTree(func, true);
    }
    return os.str();
}

void WindowRoot::FocusFaultDetection() const
{
    if (!needCheckFocusWindow) {
        return;
    }
    bool needReport = true;
    uint32_t focusWinId = INVALID_WINDOW_ID;
    for (auto& elem : windowNodeContainerMap_) {
        if (elem.second == nullptr) {
            continue;
        }
        focusWinId = elem.second->GetFocusWindow();
        if (focusWinId != INVALID_WINDOW_ID) {
            needReport = false;
            sptr<WindowNode> windowNode = GetWindowNode(focusWinId);
            if (windowNode == nullptr || !windowNode->currentVisibility_) {
                needReport = true;
                WLOGFE("The focus windowNode is nullptr or is invisible, focusWinId: %{public}u", focusWinId);
                break;
            }
        }
    }
    if (needReport) {
        std::string windowLog(GenAllWindowsLogInfo());
        WLOGFE("The focus window is faulty, focusWinId:%{public}u, %{public}s", focusWinId, windowLog.c_str());
        int32_t ret = OHOS::HiviewDFX::HiSysEvent::Write(
            OHOS::HiviewDFX::HiSysEvent::Domain::WINDOW_MANAGER,
            "NO_FOCUS_WINDOW",
            OHOS::HiviewDFX::HiSysEvent::EventType::FAULT,
            "PID", getpid(),
            "UID", getuid(),
            "PACKAGE_NAME", "foundation",
            "PROCESS_NAME", "foundation",
            "MSG", windowLog);
        if (ret != 0) {
            WLOGFE("Write HiSysEvent error, ret:%{public}d", ret);
        }
    }
}

void WindowRoot::ProcessExpandDisplayCreate(DisplayId defaultDisplayId, sptr<DisplayInfo> displayInfo,
    std::map<DisplayId, Rect>& displayRectMap)
{
    if (displayInfo == nullptr || !CheckDisplayInfo(displayInfo)) {
        WLOGFE("get display failed or get invalid display info");
        return;
    }
    DisplayId displayId = displayInfo->GetDisplayId();
    ScreenId displayGroupId = displayInfo->GetScreenGroupId();
    auto container = windowNodeContainerMap_[displayGroupId];
    if (container == nullptr) {
        WLOGFE("window node container is nullptr, displayId :%{public}" PRIu64 "", displayId);
        return;
    }

    WLOGFD("[Display Create] before add new display, displayId: %{public}" PRIu64"", displayId);
    container->GetMultiDisplayController()->ProcessDisplayCreate(defaultDisplayId, displayInfo, displayRectMap);
    WLOGFD("[Display Create] Container exist, add new display, displayId: %{public}" PRIu64"", displayId);
}

std::map<DisplayId, sptr<DisplayInfo>> WindowRoot::GetAllDisplayInfos(const std::vector<DisplayId>& displayIdVec)
{
    std::map<DisplayId, sptr<DisplayInfo>> displayInfoMap;
    for (auto& displayId : displayIdVec) {
        const sptr<DisplayInfo> displayInfo = DisplayManagerServiceInner::GetInstance().GetDisplayById(displayId);
        displayInfoMap.insert(std::make_pair(displayId, displayInfo));
        WLOGFD("Get latest displayInfo, displayId: %{public}" PRIu64"", displayId);
    }
    return displayInfoMap;
}

std::map<DisplayId, Rect> WindowRoot::GetAllDisplayRectsByDMS(sptr<DisplayInfo> displayInfo)
{
    std::map<DisplayId, Rect> displayRectMap;

    if (displayInfo == nullptr) {
        return displayRectMap;
    }

    for (auto& displayId : displayIdMap_[displayInfo->GetScreenGroupId()]) {
        auto info = DisplayManagerServiceInner::GetInstance().GetDisplayById(displayId);
        Rect displayRect = { info->GetOffsetX(), info->GetOffsetY(), info->GetWidth(), info->GetHeight() };
        displayRectMap.insert(std::make_pair(displayId, displayRect));

        WLOGFD("displayId: %{public}" PRIu64", displayRect: [ %{public}d, %{public}d, %{public}d, %{public}d]",
            displayId, displayRect.posX_, displayRect.posY_, displayRect.width_, displayRect.height_);
    }
    return displayRectMap;
}

std::map<DisplayId, Rect> WindowRoot::GetAllDisplayRectsByDisplayInfo(
    const std::map<DisplayId, sptr<DisplayInfo>>& displayInfoMap)
{
    std::map<DisplayId, Rect> displayRectMap;

    for (const auto& iter : displayInfoMap) {
        auto id = iter.first;
        auto info = iter.second;
        Rect displayRect = { info->GetOffsetX(), info->GetOffsetY(), info->GetWidth(), info->GetHeight() };
        displayRectMap.insert(std::make_pair(id, displayRect));

        WLOGFD("displayId: %{public}" PRIu64", displayRect: [ %{public}d, %{public}d, %{public}d, %{public}d]",
            id, displayRect.posX_, displayRect.posY_, displayRect.width_, displayRect.height_);
    }
    return displayRectMap;
}

void WindowRoot::ProcessDisplayCreate(DisplayId defaultDisplayId, sptr<DisplayInfo> displayInfo,
    const std::map<DisplayId, sptr<DisplayInfo>>& displayInfoMap)
{
    DisplayId displayId = (displayInfo == nullptr) ? DISPLAY_ID_INVALID : displayInfo->GetDisplayId();
    ScreenId displayGroupId = (displayInfo == nullptr) ? SCREEN_ID_INVALID : displayInfo->GetScreenGroupId();
    auto iter = windowNodeContainerMap_.find(displayGroupId);
    if (iter == windowNodeContainerMap_.end()) {
        CreateWindowNodeContainer(displayInfo);
        WLOGFD("[Display Create] Create new container for display, displayId: %{public}" PRIu64"", displayId);
    } else {
        auto& displayIdVec = displayIdMap_[displayGroupId];
        if (std::find(displayIdVec.begin(), displayIdVec.end(), displayId) != displayIdVec.end()) {
            WLOGFD("[Display Create] Current display is already exist, displayId: %{public}" PRIu64"", displayId);
            return;
        }
        // add displayId in displayId vector
        displayIdMap_[displayGroupId].push_back(displayId);
        auto displayRectMap = GetAllDisplayRectsByDisplayInfo(displayInfoMap);
        ProcessExpandDisplayCreate(defaultDisplayId, displayInfo, displayRectMap);
    }
}

void WindowRoot::MoveNotShowingWindowToDefaultDisplay(DisplayId defaultDisplayId, DisplayId displayId)
{
    for (auto& elem : windowNodeMap_) {
        auto& windowNode = elem.second;
        if (windowNode->GetDisplayId() == displayId && !windowNode->currentVisibility_) {
            std::vector<DisplayId> newShowingDisplays = { defaultDisplayId };
            windowNode->SetShowingDisplays(newShowingDisplays);
            windowNode->isShowingOnMultiDisplays_ = false;
            if (windowNode->GetWindowToken()) {
                windowNode->GetWindowToken()->UpdateDisplayId(windowNode->GetDisplayId(), defaultDisplayId);
            }
            windowNode->SetDisplayId(defaultDisplayId);
        }
    }
}

void WindowRoot::ProcessDisplayDestroy(DisplayId defaultDisplayId, sptr<DisplayInfo> displayInfo,
    const std::map<DisplayId, sptr<DisplayInfo>>& displayInfoMap)
{
    DisplayId displayId = (displayInfo == nullptr) ? DISPLAY_ID_INVALID : displayInfo->GetDisplayId();
    ScreenId displayGroupId = (displayInfo == nullptr) ? SCREEN_ID_INVALID : displayInfo->GetScreenGroupId();
    auto& displayIdVec = displayIdMap_[displayGroupId];

    auto iter = windowNodeContainerMap_.find(displayGroupId);
    if (iter == windowNodeContainerMap_.end() ||
        std::find(displayIdVec.begin(), displayIdVec.end(), displayId) == displayIdVec.end() ||
        displayInfoMap.find(displayId) == displayInfoMap.end()) {
        WLOGFE("[Display Destroy] could not find display, destroy failed, displayId: %{public}" PRIu64"", displayId);
        return;
    }

    // erase displayId in displayIdMap
    auto displayIter = std::remove(displayIdVec.begin(), displayIdVec.end(), displayId);
    displayIdVec.erase(displayIter, displayIdVec.end());

    // container process display destroy
    auto container = iter->second;
    if (container == nullptr) {
        WLOGFE("window node container is nullptr, displayId :%{public}" PRIu64 "", displayId);
        return;
    }
    WLOGFD("[Display Destroy] displayId: %{public}" PRIu64"", displayId);

    std::vector<uint32_t> needDestroyWindows;
    auto displayRectMap = GetAllDisplayRectsByDisplayInfo(displayInfoMap);
    // erase displayId in displayRectMap
    auto displayRectIter = displayRectMap.find(displayId);
    if (displayRectIter == displayRectMap.end()) {
        return;
    }
    displayRectMap.erase(displayRectIter);
    container->GetMultiDisplayController()->ProcessDisplayDestroy(
        defaultDisplayId, displayInfo, displayRectMap, needDestroyWindows);
    for (auto id : needDestroyWindows) {
        auto node = GetWindowNode(id);
        if (node != nullptr) {
            DestroyWindowInner(node);
        }
    }
    // move window which is not showing on destroyed display to default display
    MoveNotShowingWindowToDefaultDisplay(defaultDisplayId, displayId);
    WLOGFD("[Display Destroy] displayId: %{public}" PRIu64" ", displayId);
}

void WindowRoot::ProcessDisplayChange(DisplayId defaultDisplayId, sptr<DisplayInfo> displayInfo,
    const std::map<DisplayId, sptr<DisplayInfo>>& displayInfoMap, DisplayStateChangeType type)
{
    if (displayInfo == nullptr) {
        WLOGFE("get display failed");
        return;
    }
    DisplayId displayId = displayInfo->GetDisplayId();
    ScreenId displayGroupId = displayInfo->GetScreenGroupId();
    auto& displayIdVec = displayIdMap_[displayGroupId];
    auto iter = windowNodeContainerMap_.find(displayGroupId);
    if (iter == windowNodeContainerMap_.end() || std::find(displayIdVec.begin(),
        displayIdVec.end(), displayId) == displayIdVec.end()) {
        WLOGFE("[Display Change] could not find display, change failed, displayId: %{public}" PRIu64"", displayId);
        return;
    }
    // container process display change
    auto container = iter->second;
    if (container == nullptr) {
        WLOGFE("window node container is nullptr, displayId :%{public}" PRIu64 "", displayId);
        return;
    }

    auto displayRectMap = GetAllDisplayRectsByDisplayInfo(displayInfoMap);
    container->GetMultiDisplayController()->ProcessDisplayChange(defaultDisplayId, displayInfo, displayRectMap, type);
}

float WindowRoot::GetVirtualPixelRatio(DisplayId displayId) const
{
    auto container = const_cast<WindowRoot*>(this)->GetOrCreateWindowNodeContainer(displayId);
    if (container == nullptr) {
        WLOGFE("window container could not be found");
        return 1.0;  // Use DefaultVPR 1.0
    }
    return container->GetDisplayVirtualPixelRatio(displayId);
}

Rect WindowRoot::GetDisplayGroupRect(DisplayId displayId) const
{
    Rect fullDisplayRect;
    auto container = const_cast<WindowRoot*>(this)->GetOrCreateWindowNodeContainer(displayId);
    if (container == nullptr) {
        WLOGFE("window container could not be found");
        return fullDisplayRect;
    }
    return container->GetDisplayGroupRect();
}

bool WindowRoot::HasPrivateWindow(DisplayId displayId)
{
    auto container = GetWindowNodeContainer(displayId);
    return container != nullptr ? container->HasPrivateWindow() : false;
}

void WindowRoot::SetMaxAppWindowNumber(uint32_t windowNum)
{
    maxAppWindowNumber_ = windowNum;
}

void WindowRoot::SetMaxUniRenderAppWindowNumber(uint32_t uniAppWindowNum)
{
    maxUniRenderAppWindowNumber_ = uniAppWindowNum;
}

uint32_t WindowRoot::GetMaxUniRenderAppWindowNumber() const
{
    return maxUniRenderAppWindowNumber_;
}

void WindowRoot::SetSplitRatios(const std::vector<float>& splitRatioNumbers)
{
    auto& splitRatios = splitRatioConfig_.splitRatios;
    splitRatios.clear();
    splitRatios = splitRatioNumbers;
    for (auto iter = splitRatios.begin(); iter != splitRatios.end();) {
        if (*iter > 0 && *iter < 1) { // valid ratio range (0, 1)
            iter++;
        } else {
            iter = splitRatios.erase(iter);
        }
    }
    std::sort(splitRatios.begin(), splitRatios.end());
    auto iter = std::unique(splitRatios.begin(), splitRatios.end());
    splitRatios.erase(iter, splitRatios.end()); // remove duplicate ratios
}

void WindowRoot::SetExitSplitRatios(const std::vector<float>& exitSplitRatios)
{
    if (exitSplitRatios.size() != 2) {
        return;
    }
    if (exitSplitRatios[0] > 0 && exitSplitRatios[0] < DEFAULT_SPLIT_RATIO) {
        splitRatioConfig_.exitSplitStartRatio = exitSplitRatios[0];
    }
    if (exitSplitRatios[1] > DEFAULT_SPLIT_RATIO && exitSplitRatios[1] < 1) {
        splitRatioConfig_.exitSplitEndRatio = exitSplitRatios[1];
    }
}

WMError WindowRoot::GetModeChangeHotZones(DisplayId displayId,
    ModeChangeHotZones& hotZones, const ModeChangeHotZonesConfig& config)
{
    auto container = GetOrCreateWindowNodeContainer(displayId);
    if (container == nullptr) {
        WLOGFE("GetModeChangeHotZones failed, window container could not be found");
        return WMError::WM_ERROR_NULLPTR;
    }
    container->GetModeChangeHotZones(displayId, hotZones, config);
    return WMError::WM_OK;
}

void WindowRoot::RemoveSingleUserWindowNodes(int accountId)
{
    std::vector<DisplayId> displayIds = GetAllDisplayIds();
    for (auto id : displayIds) {
        sptr<WindowNodeContainer> container = GetOrCreateWindowNodeContainer(id);
        if (container == nullptr) {
            WLOGFD("get container failed %{public}" PRIu64"", id);
            continue;
        }
        container->RemoveSingleUserWindowNodes(accountId);
    }
}

WMError WindowRoot::UpdateRsTree(uint32_t windowId, bool isAdd)
{
    sptr<WindowNode> node = GetWindowNode(windowId);
    if (node == nullptr) {
        WLOGFE("could not find window");
        return WMError::WM_ERROR_NULLPTR;
    }
    auto container = GetOrCreateWindowNodeContainer(node->GetDisplayId());
    if (container == nullptr) {
        WLOGFE("window container could not be found");
        return WMError::WM_ERROR_NULLPTR;
    }
    for (auto& displayId : node->GetShowingDisplays()) {
        if (isAdd) {
            container->AddNodeOnRSTree(node, displayId, displayId, WindowUpdateType::WINDOW_UPDATE_ACTIVE);
        } else {
            container->RemoveNodeFromRSTree(node, displayId, displayId, WindowUpdateType::WINDOW_UPDATE_ACTIVE);
        }
    }
    RSTransaction::FlushImplicitTransaction();
    return WMError::WM_OK;
}

sptr<WindowNode> WindowRoot::FindDialogCallerNode(WindowType type, sptr<IRemoteObject> token)
{
    if (type != WindowType::WINDOW_TYPE_DIALOG) {
        return nullptr;
    }

    auto iter = std::find_if(windowNodeMap_.begin(), windowNodeMap_.end(),
        [token](const std::map<uint32_t, sptr<WindowNode>>::value_type& pair) {
            if (WindowHelper::IsMainWindow(pair.second->GetWindowType())) {
                return pair.second->abilityToken_ == token;
            }
            return false;
        });
    if (iter == windowNodeMap_.end()) {
        WLOGFD("cannot find windowNode");
        return nullptr;
    }
    return iter->second;
}

bool WindowRoot::CheckMultiDialogWindows(WindowType type, sptr<IRemoteObject> token)
{
    if (type != WindowType::WINDOW_TYPE_DIALOG) {
        return false;
    }

    sptr<WindowNode> newCaller, oriCaller;

    newCaller = FindDialogCallerNode(type, token);
    if (newCaller == nullptr) {
        return false;
    }

    for (auto& iter : windowNodeMap_) {
        if (iter.second->GetWindowType() == WindowType::WINDOW_TYPE_DIALOG) {
            oriCaller = FindDialogCallerNode(iter.second->GetWindowType(), iter.second->dialogTargetToken_);
            if (oriCaller == newCaller) {
                return true;
            }
        }
    }

    return false;
}

void WindowRoot::OnRenderModeChanged(bool isUniRender)
{
    WLOGFD("SwitchRender: render mode of RS has changed from %{public}u to %{public}s",
        static_cast<uint32_t>(renderMode_), std::to_string(isUniRender).c_str());
    if (isUniRender) {
        switch (renderMode_) {
            case RenderMode::SEPARATED:
            case RenderMode::UNIFYING:
                renderMode_ = RenderMode::UNIFIED;
                break;
            case RenderMode::SEPARATING:
                RSInterfaces::GetInstance().UpdateRenderMode(false);
                break;
            default:
                WLOGFE("SwitchRender: impossible code");
                break;
        }
    } else {
        switch (renderMode_) {
            case RenderMode::UNIFIED:
            case RenderMode::SEPARATING:
                renderMode_ = RenderMode::SEPARATED;
                break;
            case RenderMode::UNIFYING:
                RSInterfaces::GetInstance().UpdateRenderMode(true);
                break;
            default:
                WLOGFE("SwitchRender: impossible code");
                break;
        }
    }
}

void WindowRoot::SwitchRenderModeIfNeeded()
{
    if (displayIdMap_.empty()) {
        WLOGFE("WindowRoot::SwitchRenderModeIfNeeded: displayIdMap_ is empty");
        return;
    }
    if (displayIdMap_.size() != 1) {
        WLOGFE("WindowRoot::SwitchRenderModeIfNeeded: invalid screenGroup number");
        return;
    }
    uint32_t displayNum = displayIdMap_.begin()->second.size();
    if (displayNum > 1) {
        // switch to sperate render mode
        ChangeRSRenderModeIfNeeded(false);
        return;
    }

    bool exceed = IsAppWindowExceed();
    if (exceed) {
        // switch to sperate render mode
        ChangeRSRenderModeIfNeeded(false);
    } else {
        // switch to unified render mode
        ChangeRSRenderModeIfNeeded(true);
    }
}

void WindowRoot::ChangeRSRenderModeIfNeeded(bool isToUnified)
{
    switch (renderMode_) {
        case RenderMode::SEPARATED:
            if (isToUnified) {
                WLOGFD("SwitchRender: notify RS from separated to be unifying");
                renderMode_ = RenderMode::UNIFYING;
                RSInterfaces::GetInstance().UpdateRenderMode(true);
            }
            break;
        case RenderMode::UNIFIED:
            if (!isToUnified) {
                WLOGFD("SwitchRender: notify RS from unified to be separating");
                renderMode_ = RenderMode::SEPARATING;
                RSInterfaces::GetInstance().UpdateRenderMode(false);
            }
            break;
        case RenderMode::SEPARATING:
            if (isToUnified) {
                WLOGFD("SwitchRender: notify RS from separating to be unifying");
                renderMode_ = RenderMode::UNIFYING;
            }
            break;
        case RenderMode::UNIFYING:
            if (!isToUnified) {
                WLOGFD("SwitchRender: notify RS from unifying to be separating");
                renderMode_ = RenderMode::SEPARATING;
            }
            break;
        default:
            WLOGE("SwitchRender: impossible code");
            break;
    }
}

bool WindowRoot::IsAppWindowExceed() const
{
    uint32_t appWindowNum = 0;
    for (const auto& it : windowNodeMap_) {
        WindowType winType = it.second->GetWindowType();
        WindowMode winMode = it.second->GetWindowMode();
        if (winMode == WindowMode::WINDOW_MODE_SPLIT_PRIMARY || winMode == WindowMode::WINDOW_MODE_SPLIT_SECONDARY) {
            return false;
        }
        if (winType >= WindowType::APP_WINDOW_BASE && winType < WindowType::APP_WINDOW_END &&
            it.second->currentVisibility_) {
            appWindowNum++;
        }
    }
    WLOGFD("SwitchRender: the number of app window is %{public}u", maxUniRenderAppWindowNumber_);
    return (appWindowNum > maxUniRenderAppWindowNumber_);
}

sptr<WindowNode> WindowRoot::GetWindowNodeByAbilityToken(const sptr<IRemoteObject>& abilityToken)
{
    for (const auto& iter : windowNodeMap_) {
        if (iter.second != nullptr && iter.second->abilityToken_ == abilityToken) {
            return iter.second;
        }
    }
    WLOGFE("could not find required abilityToken!");
    return nullptr;
}

bool WindowRoot::TakeWindowPairSnapshot(DisplayId displayId)
{
    auto container = GetWindowNodeContainer(displayId);
    return container == nullptr ? false : container->TakeWindowPairSnapshot(displayId);
}

void WindowRoot::ClearWindowPairSnapshot(DisplayId displayId)
{
    auto container = GetWindowNodeContainer(displayId);
    if (container == nullptr) {
        WLOGFE("clear window pair snapshot failed, because container in null");
        return;
    }
    return container->ClearWindowPairSnapshot(displayId);
}
} // namespace Rosen
} // namespace OHOS
