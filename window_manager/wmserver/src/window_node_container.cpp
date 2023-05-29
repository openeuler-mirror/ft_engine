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

#include "window_node_container.h"

#include <ability_manager_client.h>
#include <algorithm>
#include <cinttypes>
#include <cmath>
#include <ctime>
#include <display_power_mgr_client.h>
#include <hitrace_meter.h>
#include <limits>
#include <power_mgr_client.h>
#include <transaction/rs_interfaces.h>
#include <transaction/rs_transaction.h>

#include "common_event_manager.h"
#include "dm_common.h"
#include "remote_animation.h"
#include "starting_window.h"
#include "window_helper.h"
#include "window_inner_manager.h"
#include "window_layout_policy_cascade.h"
#include "window_layout_policy_tile.h"
#include "window_manager_agent_controller.h"
#include "window_manager_hilog.h"
#include "window_manager_service.h"
#include "wm_common.h"
#include "wm_common_inner.h"

namespace OHOS {
namespace Rosen {
namespace {
    constexpr HiviewDFX::HiLogLabel LABEL = {LOG_CORE, HILOG_DOMAIN_WINDOW, "WindowNodeContainer"};
    constexpr int WINDOW_NAME_MAX_LENGTH = 10;
    constexpr uint32_t MAX_BRIGHTNESS = 255;
    constexpr uint32_t SPLIT_WINDOWS_CNT = 2;
    constexpr uint32_t EXIT_SPLIT_POINTS_NUMBER = 2;
    constexpr int UID_TRANSFROM_DIVISOR = 200000;
    constexpr int UID_MIN = 100;
}
AnimationConfig WindowNodeContainer::animationConfig_;

WindowNodeContainer::WindowNodeContainer(const sptr<DisplayInfo>& displayInfo, ScreenId displayGroupId)
{
    DisplayId displayId = displayInfo->GetDisplayId();

    // create and displayGroupInfo and displayGroupController
    displayGroupInfo_ = new DisplayGroupInfo(displayGroupId, displayInfo);
    displayGroupController_ = new DisplayGroupController(this, displayGroupInfo_);
    displayGroupController_->InitNewDisplay(displayId);

    // init layout policy
    layoutPolicies_[WindowLayoutMode::CASCADE] = new WindowLayoutPolicyCascade(displayGroupInfo_,
        displayGroupController_->displayGroupWindowTree_);
    layoutPolicies_[WindowLayoutMode::TILE] = new WindowLayoutPolicyTile(displayGroupInfo_,
        displayGroupController_->displayGroupWindowTree_);
    layoutPolicy_ = layoutPolicies_[WindowLayoutMode::CASCADE];
    layoutPolicy_->Launch();

    Rect initalDividerRect = layoutPolicies_[WindowLayoutMode::CASCADE]->GetDividerRect(displayId);
    displayGroupController_->SetDividerRect(displayId, initalDividerRect);
    // init avoidAreaController
    avoidController_ = new AvoidAreaController(focusedWindow_);
}

WindowNodeContainer::~WindowNodeContainer()
{
    Destroy();
}

uint32_t WindowNodeContainer::GetWindowCountByType(WindowType windowType)
{
    uint32_t windowNumber = 0;
    auto counter = [&windowNumber, &windowType](sptr<WindowNode>& windowNode) {
        if (windowNode->GetWindowType() == windowType && !windowNode->startingWindowShown_) ++windowNumber;
    };
    std::for_each(belowAppWindowNode_->children_.begin(), belowAppWindowNode_->children_.end(), counter);
    std::for_each(appWindowNode_->children_.begin(), appWindowNode_->children_.end(), counter);
    std::for_each(aboveAppWindowNode_->children_.begin(), aboveAppWindowNode_->children_.end(), counter);
    return windowNumber;
}

WMError WindowNodeContainer::AddWindowNodeOnWindowTree(sptr<WindowNode>& node, const sptr<WindowNode>& parentNode)
{
    sptr<WindowNode> root = FindRoot(node->GetWindowType());
    if (root == nullptr && !(WindowHelper::IsSystemSubWindow(node->GetWindowType()) &&
        parentNode != nullptr)) {
        WLOGFE("root is nullptr!");
        return WMError::WM_ERROR_NULLPTR;
    }
    node->requestedVisibility_ = true;
    if (parentNode != nullptr) { // subwindow
        if (WindowHelper::IsSystemSubWindow(node->GetWindowType()) ||
            node->GetWindowType() == WindowType::WINDOW_TYPE_APP_COMPONENT) {
            if (WindowHelper::IsSubWindow(parentNode->GetWindowType()) ||
                WindowHelper::IsSystemSubWindow(parentNode->GetWindowType()) ||
                parentNode->GetWindowType() == WindowType::WINDOW_TYPE_APP_COMPONENT ||
                parentNode->GetWindowType() == WindowType::WINDOW_TYPE_DIALOG) {
                // some times, dialog is a child window, so exclude
                WLOGFE("the parent of window cannot be any sub window");
                return WMError::WM_ERROR_INVALID_PARAM;
            }
        } else {
            if (parentNode->parent_ != root &&
                !((parentNode->GetWindowFlags() & static_cast<uint32_t>(WindowFlag::WINDOW_FLAG_SHOW_WHEN_LOCKED)) &&
                (parentNode->parent_ == aboveAppWindowNode_))) {
                WLOGFE("window type and parent window not match \
                    or try to add subwindow to subwindow, which is forbidden");
                return WMError::WM_ERROR_INVALID_PARAM;
            }
        }
        node->currentVisibility_ = parentNode->currentVisibility_;
        node->parent_ = parentNode;
    } else { // mainwindow
        node->parent_ = root;
        node->currentVisibility_ = true;
        for (auto& child : node->children_) {
            child->currentVisibility_ = child->requestedVisibility_;
        }
        if (WindowHelper::IsSystemBarWindow(node->GetWindowType())) {
            displayGroupController_->sysBarNodeMaps_[node->GetDisplayId()][node->GetWindowType()] = node;
        }
    }
    return WMError::WM_OK;
}

WMError WindowNodeContainer::ShowStartingWindow(sptr<WindowNode>& node)
{
    if (node->currentVisibility_) {
        WLOGFE("current window is visible, windowId: %{public}u", node->GetWindowId());
        return WMError::WM_ERROR_INVALID_OPERATION;
    }

    WMError res = AddWindowNodeOnWindowTree(node, nullptr);
    if (res != WMError::WM_OK) {
        return res;
    }
    UpdateWindowTree(node);
    displayGroupController_->PreProcessWindowNode(node, WindowUpdateType::WINDOW_UPDATE_ADDED);
    StartingWindow::AddNodeOnRSTree(node, animationConfig_, layoutPolicy_->IsMultiDisplay());
    AssignZOrder();
    layoutPolicy_->AddWindowNode(node);
    WLOGFD("ShowStartingWindow windowId: %{public}u end", node->GetWindowId());
    return WMError::WM_OK;
}

WMError WindowNodeContainer::IsTileRectSatisfiedWithSizeLimits(sptr<WindowNode>& node)
{
    if (layoutMode_ == WindowLayoutMode::TILE &&
        !layoutPolicy_->IsTileRectSatisfiedWithSizeLimits(node)) {
        WLOGFE("layoutMode is tile, default rect is not satisfied with size limits of window, windowId: %{public}u",
            node->GetWindowId());
        return WMError::WM_ERROR_INVALID_WINDOW_MODE_OR_SIZE;
    }
    return WMError::WM_OK;
}

AnimationConfig& WindowNodeContainer::GetAnimationConfigRef()
{
    return animationConfig_;
}

void WindowNodeContainer::LayoutWhenAddWindowNode(sptr<WindowNode>& node, bool afterAnimation)
{
    if (afterAnimation) {
        layoutPolicy_->AddWindowNode(node);
        return;
    }
    WLOGFD("AddWindowNode windowId:%{public}u, name:%{public}s currState:%{public}u",
        node->GetWindowId(), node->GetWindowName().c_str(),
        static_cast<uint32_t>(node->stateMachine_.GetCurrentState()));
    if (WindowHelper::IsMainWindow(node->GetWindowType()) &&
        RemoteAnimation::IsRemoteAnimationEnabledAndFirst(node->GetDisplayId()) &&
        node->stateMachine_.IsShowAnimationPlaying()) {
        // for first frame callback
        auto winRect = node->GetWindowRect();
        if (node->surfaceNode_) {
            node->surfaceNode_->SetBounds(0, 0, winRect.width_, winRect.height_);
            WLOGFD("notify client and SetBounds id:%{public}u, rect:[%{public}d, %{public}d, %{public}u, %{public}u]",
                node->GetWindowId(), winRect.posX_, winRect.posY_, winRect.width_, winRect.height_);
            layoutPolicy_->UpdateClientRect(node->GetWindowRect(), node, WindowSizeChangeReason::UNDEFINED);
        }
    } else {
        if (node->GetWindowProperty()->GetAnimationFlag() == static_cast<uint32_t>(WindowAnimation::CUSTOM) &&
            WindowHelper::IsSystemWindow(node->GetWindowType())) {
                node->SetWindowSizeChangeReason(WindowSizeChangeReason::CUSTOM_ANIMATION_SHOW);
        }
        layoutPolicy_->AddWindowNode(node);
    }
}

WMError WindowNodeContainer::AddWindowNode(sptr<WindowNode>& node, sptr<WindowNode>& parentNode, bool afterAnimation)
{
    if (!node->startingWindowShown_) { // window except main Window
        WMError res = AddWindowNodeOnWindowTree(node, parentNode);
        if (res != WMError::WM_OK) {
            return res;
        }
        UpdateWindowTree(node);
        displayGroupController_->PreProcessWindowNode(node, WindowUpdateType::WINDOW_UPDATE_ADDED);
        // add node on RSTree
        for (auto& displayId : node->GetShowingDisplays()) {
            AddNodeOnRSTree(node, displayId, displayId, WindowUpdateType::WINDOW_UPDATE_ADDED,
                node->isPlayAnimationShow_);
        }
    } else { // only main app window has starting window
        node->isPlayAnimationShow_ = false;
        node->startingWindowShown_ = false;
        AddAppSurfaceNodeOnRSTree(node);
        ReZOrderShowWhenLockedWindowIfNeeded(node);
        RaiseZOrderForAppWindow(node, parentNode);
    }
    auto windowPair = displayGroupController_->GetWindowPairByDisplayId(node->GetDisplayId());
    if (windowPair == nullptr) {
        WLOGFE("Window pair is nullptr");
        return WMError::WM_ERROR_NULLPTR;
    }
    windowPair->UpdateIfSplitRelated(node);
    if (node->IsSplitMode()) {
        // raise the z-order of window pair
        RaiseSplitRelatedWindowToTop(node);
    }
    AssignZOrder();
    LayoutWhenAddWindowNode(node, afterAnimation);
    NotifyIfAvoidAreaChanged(node, AvoidControlType::AVOID_NODE_ADD);
    DumpScreenWindowTree();
    UpdateCameraFloatWindowStatus(node, true);
    if (WindowHelper::IsMainWindow(node->GetWindowType())) {
        backupWindowIds_.clear();
    }

    if (node->GetWindowType() == WindowType::WINDOW_TYPE_KEYGUARD) {
        isScreenLocked_ = true;
        SetBelowScreenlockVisible(node, false);
    }
    if (node->GetWindowType() == WindowType::WINDOW_TYPE_WALLPAPER) {
        RemoteAnimation::NotifyAnimationUpdateWallpaper(node);
    }
    WLOGFD("AddWindowNode windowId: %{public}u end", node->GetWindowId());
    RSInterfaces::GetInstance().SetAppWindowNum(GetAppWindowNum());
    return WMError::WM_OK;
}

void WindowNodeContainer::UpdateRSTreeWhenShowingDisplaysChange(sptr<WindowNode>& node,
    const std::vector<DisplayId>& lastShowingDisplays)
{
    if (!layoutPolicy_->IsMultiDisplay()) {
        return;
    }

    // Update RSTree
    auto curShowingDisplays = node->GetShowingDisplays();
    for (auto& displayId : lastShowingDisplays) {
        if (std::find(curShowingDisplays.begin(), curShowingDisplays.end(), displayId) == curShowingDisplays.end()) {
            RemoveNodeFromRSTree(node, displayId, *(curShowingDisplays.begin()),
                WindowUpdateType::WINDOW_UPDATE_ACTIVE);
            WLOGFD("remove from RSTree : %{public}" PRIu64"", displayId);
        }
    }

    for (auto& displayId : curShowingDisplays) {
        if (std::find(lastShowingDisplays.begin(), lastShowingDisplays.end(), displayId) == lastShowingDisplays.end()) {
            AddNodeOnRSTree(node, displayId, displayId, WindowUpdateType::WINDOW_UPDATE_ACTIVE);
            WLOGFD("add on RSTree : %{public}" PRIu64"", displayId);
        }
    }
}

WMError WindowNodeContainer::UpdateWindowNode(sptr<WindowNode>& node, WindowUpdateReason reason)
{
    // Get last displayId and last showing displays before layout
    auto lastShowingDisplays = node->GetShowingDisplays();

    // PreProcess window node and layout node
    displayGroupController_->PreProcessWindowNode(node, WindowUpdateType::WINDOW_UPDATE_ACTIVE);
    if (WindowHelper::IsMainWindow(node->GetWindowType()) && WindowHelper::IsSwitchCascadeReason(reason)) {
        SwitchLayoutPolicy(WindowLayoutMode::CASCADE, node->GetDisplayId());
    }
    layoutPolicy_->UpdateWindowNode(node);
    displayGroupController_->PostProcessWindowNode(node);
    // Get current displayId and showing displays, update RSTree and displayGroupWindowTree
    UpdateRSTreeWhenShowingDisplaysChange(node, lastShowingDisplays);
    NotifyIfAvoidAreaChanged(node, AvoidControlType::AVOID_NODE_UPDATE);
    DumpScreenWindowTree();
    WLOGFD("UpdateWindowNode windowId: %{public}u end", node->GetWindowId());
    return WMError::WM_OK;
}

void WindowNodeContainer::RemoveWindowNodeFromWindowTree(sptr<WindowNode>& node)
{
    // remove this node from parent
    auto iter = std::find(node->parent_->children_.begin(), node->parent_->children_.end(), node);
    if (iter != node->parent_->children_.end()) {
        node->parent_->children_.erase(iter);
    } else {
        WLOGFE("can't find this node in parent");
    }
    node->parent_ = nullptr;
}

void WindowNodeContainer::RemoveFromRsTreeWhenRemoveWindowNode(sptr<WindowNode>& node, bool fromAnimation)
{
    if (fromAnimation || (RemoteAnimation::IsRemoteAnimationEnabledAndFirst(node->GetDisplayId()) &&
        node->stateMachine_.IsHideAnimationPlaying())) {
            WLOGFD("not remove from rs tree id:%{public}u", node->GetWindowId());
            return;
    }
    // When RemoteAnimation exists, remove node from rs tree after animation
    WLOGFD("remove from rs tree id:%{public}u, node->isPlayAnimationHide_:%{public}u", node->GetWindowId(),
        static_cast<uint32_t>(node->isPlayAnimationHide_));
    // subwindow or no remote animation also exit with animation
    for (auto& displayId : node->GetShowingDisplays()) {
        RemoveNodeFromRSTree(node, displayId, displayId, WindowUpdateType::WINDOW_UPDATE_REMOVED,
            node->isPlayAnimationHide_);
    }
}

void WindowNodeContainer::SetSurfaceNodeVisible(sptr<WindowNode>& node, int32_t topPriority, bool visible)
{
    if (node == nullptr) {
        return;
    }
    if (node->parent_ != nullptr && node->currentVisibility_) {
        if (node->priority_ < topPriority && !WindowHelper::IsShowWhenLocked(node->GetWindowFlags()) &&
            !WindowHelper::IsShowWhenLocked(node->parent_->GetWindowFlags())) {
            auto surfaceNode = node->leashWinSurfaceNode_ != nullptr ? node->leashWinSurfaceNode_ : node->surfaceNode_;
            if (surfaceNode) {
                surfaceNode->SetVisible(visible);
            }
        }
    }
    for (auto& childNode : node->children_) {
        SetSurfaceNodeVisible(childNode, topPriority, visible);
    }
}

void WindowNodeContainer::SetBelowScreenlockVisible(sptr<WindowNode>& node, bool visible)
{
    int32_t topPriority = zorderPolicy_->GetWindowPriority(WindowType::WINDOW_TYPE_KEYGUARD);
    std::vector<sptr<WindowNode>> rootNodes = { belowAppWindowNode_, appWindowNode_, aboveAppWindowNode_ };
    for (auto& node : rootNodes) {
        SetSurfaceNodeVisible(node, topPriority, visible);
    }
}

WMError WindowNodeContainer::RemoveWindowNode(sptr<WindowNode>& node, bool fromAnimation)
{
    if (node == nullptr) {
        WLOGFE("window node or surface node is nullptr, invalid");
        return WMError::WM_ERROR_DESTROYED_OBJECT;
    }
    if (node->parent_ == nullptr) {
        WLOGFW("can't find parent of this node");
    } else {
        RemoveWindowNodeFromWindowTree(node);
    }

    node->requestedVisibility_ = false;
    node->currentVisibility_ = false;
    RemoveFromRsTreeWhenRemoveWindowNode(node, fromAnimation);
    node->isPlayAnimationHide_ = false;
    displayGroupController_->UpdateDisplayGroupWindowTree();

    layoutPolicy_->RemoveWindowNode(node);
    WindowMode lastMode = node->GetWindowMode();
    if (HandleRemoveWindow(node) != WMError::WM_OK) {
        return WMError::WM_ERROR_NULLPTR;
    }
    if (!WindowHelper::IsFloatingWindow(lastMode)) {
        NotifyDockWindowStateChanged(node, true);
    }
    NotifyIfAvoidAreaChanged(node, AvoidControlType::AVOID_NODE_REMOVE);
    DumpScreenWindowTree();
    UpdateCameraFloatWindowStatus(node, false);
    if (node->GetWindowType() == WindowType::WINDOW_TYPE_KEYGUARD) {
        isScreenLocked_ = false;
        SetBelowScreenlockVisible(node, true);
    }
    if (node->GetWindowType() == WindowType::WINDOW_TYPE_BOOT_ANIMATION) {
        DisplayManagerServiceInner::GetInstance().SetGravitySensorSubscriptionEnabled();
    }
    WLOGFD("RemoveWindowNode windowId: %{public}u end", node->GetWindowId());
    RSInterfaces::GetInstance().SetAppWindowNum(GetAppWindowNum());
    return WMError::WM_OK;
}

uint32_t WindowNodeContainer::GetAppWindowNum()
{
    uint32_t num = 0;
    for (auto& child : appWindowNode_->children_) {
        if (WindowHelper::IsAppWindow(child->GetWindowType())) {
            num++;
        }
    }
    return num;
}

WMError WindowNodeContainer::HandleRemoveWindow(sptr<WindowNode>& node)
{
    auto windowPair = displayGroupController_->GetWindowPairByDisplayId(node->GetDisplayId());
    if (windowPair == nullptr) {
        WLOGFE("Window pair is nullptr");
        return WMError::WM_ERROR_NULLPTR;
    }
    windowPair->HandleRemoveWindow(node);
    auto dividerWindow = windowPair->GetDividerWindow();
    auto type = node->GetWindowType();
    if ((type == WindowType::WINDOW_TYPE_STATUS_BAR || type == WindowType::WINDOW_TYPE_NAVIGATION_BAR) &&
        dividerWindow != nullptr) {
        UpdateWindowNode(dividerWindow, WindowUpdateReason::UPDATE_RECT);
    }
    return WMError::WM_OK;
}

WMError WindowNodeContainer::DestroyWindowNode(sptr<WindowNode>& node, std::vector<uint32_t>& windowIds)
{
    WMError ret = RemoveWindowNode(node);
    if (ret != WMError::WM_OK) {
        WLOGFE("RemoveWindowNode failed");
        return ret;
    }
    StartingWindow::ReleaseStartWinSurfaceNode(node);
    node->surfaceNode_ = nullptr;
    windowIds.push_back(node->GetWindowId());
    for (auto& child : node->children_) { // destroy sub window if exists
        windowIds.push_back(child->GetWindowId());
        child->parent_ = nullptr;
        if (child->surfaceNode_ != nullptr) {
            WLOGFD("child surfaceNode set nullptr");
            child->surfaceNode_ = nullptr;
        }
    }

    // clear vector cache completely, swap with empty vector
    auto emptyVector = std::vector<sptr<WindowNode>>();
    node->children_.swap(emptyVector);
    if (node->GetWindowType() == WindowType::WINDOW_TYPE_WALLPAPER) {
        RemoteAnimation::NotifyAnimationUpdateWallpaper(nullptr);
    }
    WLOGFD("DestroyWindowNode windowId: %{public}u end", node->GetWindowId());
    return WMError::WM_OK;
}

void WindowNodeContainer::UpdateSizeChangeReason(sptr<WindowNode>& node, WindowSizeChangeReason reason)
{
    if (!node->GetWindowToken()) {
        WLOGFE("windowToken is null");
        return;
    }
    if (node->GetWindowType() == WindowType::WINDOW_TYPE_DOCK_SLICE) {
        for (auto& childNode : appWindowNode_->children_) {
            if (childNode->IsSplitMode()) {
                layoutPolicy_->UpdateClientRect(childNode->GetWindowRect(), childNode, reason);
                childNode->ResetWindowSizeChangeReason();
                WLOGFD("Notify split window that the drag action is start or end, windowId: %{public}d, "
                    "reason: %{public}u", childNode->GetWindowId(), reason);
            }
        }
    } else {
        layoutPolicy_->UpdateClientRect(node->GetWindowRect(), node, reason);
        node->ResetWindowSizeChangeReason();
        WLOGFD("Notify window that the drag action is start or end, windowId: %{public}d, "
            "reason: %{public}u", node->GetWindowId(), reason);
    }
}

void WindowNodeContainer::UpdateWindowTree(sptr<WindowNode>& node)
{
    HITRACE_METER(HITRACE_TAG_WINDOW_MANAGER);
    node->priority_ = zorderPolicy_->GetWindowPriority(node->GetWindowType());
    RaiseInputMethodWindowPriorityIfNeeded(node);
    RaiseShowWhenLockedWindowIfNeeded(node);
    auto parentNode = node->parent_;
    if (parentNode == nullptr) {
        WLOGFD("Current window node has no parent: %{public}u", node->GetWindowId());
        return;
    }
    auto iter = std::find(parentNode->children_.begin(), parentNode->children_.end(), node);
    if (iter != parentNode->children_.end()) {
        WLOGFD("node %{public}u is already on window tree, no need to update!", node->GetWindowId());
        return;
    }
    auto position = parentNode->children_.end();
    int splitWindowCnt = 0;
    for (auto child = parentNode->children_.begin(); child < parentNode->children_.end(); ++child) {
        if (node->GetWindowType() == WindowType::WINDOW_TYPE_DOCK_SLICE && splitWindowCnt == SPLIT_WINDOWS_CNT) {
            position = child;
            break;
        }
        if (WindowHelper::IsSplitWindowMode((*child)->GetWindowMode())) {
            splitWindowCnt++;
        }
        if ((*child)->priority_ > node->priority_) {
            position = child;
            break;
        }
    }
    parentNode->children_.insert(position, node);
}

bool WindowNodeContainer::AddAppSurfaceNodeOnRSTree(sptr<WindowNode>& node)
{
    /*
     * App main window must has starting window, and show after starting window
     * Starting Window has already update leashWindowSurfaceNode and starting window surfaceNode on RSTree
     * Just need add appSurface Node as child of leashWindowSurfaceNode
     */
    HITRACE_METER_FMT(HITRACE_TAG_WINDOW_MANAGER, "AddAppSurfaceNodeOnRSTree");
    if (!WindowHelper::IsMainWindow(node->GetWindowType())) {
        WLOGFE("id:%{public}u not main app window but has start window", node->GetWindowId());
        return false;
    }
    if (!node->leashWinSurfaceNode_ || !node->surfaceNode_) {
        WLOGFE("id:%{public}u leashWinSurfaceNode or surfaceNode is null but has start window!", node->GetWindowId());
        return false;
    }
    WLOGFD("AddAppSurfaceNodeOnRSTree windowId: %{public}d", node->GetWindowId());
    if (!node->currentVisibility_) {
        WLOGFD("window: %{public}d is invisible, do not need update RS tree", node->GetWindowId());
        return false;
    }
    node->leashWinSurfaceNode_->AddChild(node->surfaceNode_, -1);
    return true;
}

bool WindowNodeContainer::AddNodeOnRSTree(sptr<WindowNode>& node, DisplayId displayId, DisplayId parentDisplayId,
    WindowUpdateType type, bool animationPlayed)
{
    HITRACE_METER(HITRACE_TAG_WINDOW_MANAGER);
    if (node->GetWindowProperty()->GetAnimationFlag() == static_cast<uint32_t>(WindowAnimation::CUSTOM) ||
        node->GetWindowType() == WindowType::WINDOW_TYPE_APP_COMPONENT) {
        WLOGFD("not need to update RSTree");
        return true;
    }
    bool isMultiDisplay = layoutPolicy_->IsMultiDisplay();
    WLOGFD("AddNodeOnRSTree windowId: %{public}d, displayId: %{public}" PRIu64", parentDisplayId: %{public}" PRIu64", "
        "isMultiDisplay: %{public}d, animationPlayed: %{public}d",
        node->GetWindowId(), displayId, parentDisplayId, isMultiDisplay, animationPlayed);
    auto updateRSTreeFunc = [&]() {
        auto& dms = DisplayManagerServiceInner::GetInstance();
        if (!node->currentVisibility_) {
            WLOGFD("window: %{public}d is invisible, do not need update RS tree", node->GetWindowId());
            return;
        }
        auto& surfaceNode = node->leashWinSurfaceNode_ != nullptr ? node->leashWinSurfaceNode_ : node->surfaceNode_;
        dms.UpdateRSTree(displayId, parentDisplayId, surfaceNode, true, isMultiDisplay);
        for (auto& child : node->children_) {
            if (child->currentVisibility_) {
                dms.UpdateRSTree(displayId, parentDisplayId, child->surfaceNode_, true, isMultiDisplay);
            }
        }
    };

    if (type != WindowUpdateType::WINDOW_UPDATE_ADDED && type != WindowUpdateType::WINDOW_UPDATE_REMOVED) {
        updateRSTreeFunc();
        return true;
    }

    if (node->EnableDefaultAnimation(animationPlayed)) {
        WLOGFD("add window with animation");
        StartTraceArgs(HITRACE_TAG_WINDOW_MANAGER, "Animate(%u)", node->GetWindowId());
        RSNode::Animate(animationConfig_.windowAnimationConfig_.animationTiming_.timingProtocol_,
            animationConfig_.windowAnimationConfig_.animationTiming_.timingCurve_, updateRSTreeFunc);
        FinishTrace(HITRACE_TAG_WINDOW_MANAGER);
    } else if (node->GetWindowType() == WindowType::WINDOW_TYPE_INPUT_METHOD_FLOAT &&
        !animationPlayed) { // add keyboard with animation
        ProcessInputMethodWindowAddAnimation(node, updateRSTreeFunc);
    } else {
        WLOGFD("add window without animation");
        updateRSTreeFunc();
    }
    return true;
}

void WindowNodeContainer::ProcessInputMethodWindowAddAnimation(sptr<WindowNode>& node,
    std::function<void()> updateRSTreeFunc)
{
    static sptr<WindowNode> imeNode = nullptr;
    if (imeNode == node && imeNode != nullptr && imeNode->surfaceNode_ != nullptr) {
        WLOGFD("imeNode SetAppFreeze(true)");
        imeNode->surfaceNode_->SetAppFreeze(true);
    }
    sptr<WindowNode> launcherNode = nullptr;
    for (auto& windowNode : belowAppWindowNode_->children_) {
        if (windowNode->GetWindowType() == WindowType::WINDOW_TYPE_DESKTOP) {
            launcherNode = windowNode;
            break;
        }
    }
    if (launcherNode != nullptr && launcherNode->surfaceNode_ != nullptr) {
        WLOGFD("launcherNode SetAppFreeze(true)");
        launcherNode->surfaceNode_->SetAppFreeze(true);
    }
    auto timingProtocol = animationConfig_.keyboardAnimationConfig_.durationIn_;
    RSNode::Animate(timingProtocol, animationConfig_.keyboardAnimationConfig_.curve_, updateRSTreeFunc,
        [ime = imeNode, node, launcherNode]() {
        if (ime == node && ime != nullptr && ime->surfaceNode_ != nullptr) {
            WLOGFD("imeNode SetAppFreeze(false)");
            ime->surfaceNode_->SetAppFreeze(false);
        }
        if (launcherNode != nullptr && launcherNode->surfaceNode_ != nullptr) {
            WLOGFD("launcherNode SetAppFreeze(false)");
            launcherNode->surfaceNode_->SetAppFreeze(false);
        }
        auto transactionProxy = RSTransactionProxy::GetInstance();
        if (transactionProxy != nullptr) {
            transactionProxy->FlushImplicitTransaction();
        }
    });
    imeNode = node;
}

bool WindowNodeContainer::RemoveNodeFromRSTree(sptr<WindowNode>& node, DisplayId displayId, DisplayId parentDisplayId,
    WindowUpdateType type, bool animationPlayed)
{
    HITRACE_METER(HITRACE_TAG_WINDOW_MANAGER);
    if (node->GetWindowProperty()->GetAnimationFlag() == static_cast<uint32_t>(WindowAnimation::CUSTOM) ||
        node->GetWindowType() == WindowType::WINDOW_TYPE_APP_COMPONENT) {
        WLOGFD("not need to update RSTree");
        return true;
    }
    bool isMultiDisplay = layoutPolicy_->IsMultiDisplay();
    WLOGFD("RemoveNodeFromRSTree  windowId: %{public}d, displayId: %{public}" PRIu64", isMultiDisplay: %{public}d, "
        "parentDisplayId: %{public}" PRIu64", animationPlayed: %{public}d",
        node->GetWindowId(), displayId, isMultiDisplay, parentDisplayId, animationPlayed);
    auto updateRSTreeFunc = [&]() {
        auto& dms = DisplayManagerServiceInner::GetInstance();
        auto& surfaceNode = node->leashWinSurfaceNode_ != nullptr ? node->leashWinSurfaceNode_ : node->surfaceNode_;
        dms.UpdateRSTree(displayId, parentDisplayId, surfaceNode, false, isMultiDisplay);
        for (auto& child : node->children_) {
            if (child->currentVisibility_) {
                dms.UpdateRSTree(displayId, parentDisplayId, child->surfaceNode_, false, isMultiDisplay);
            }
        }
    };

    if (type != WindowUpdateType::WINDOW_UPDATE_ADDED && type != WindowUpdateType::WINDOW_UPDATE_REMOVED) {
        updateRSTreeFunc();
        return true;
    }

    if (node->EnableDefaultAnimation(animationPlayed)) {
        WLOGFD("remove window with animation");
        StartTraceArgs(HITRACE_TAG_WINDOW_MANAGER, "Animate(%u)", node->GetWindowId());
        if (node->surfaceNode_) {
            node->surfaceNode_->SetAppFreeze(true);
        }
	wptr<WindowNode> weakNode(node);
        RSNode::Animate(animationConfig_.windowAnimationConfig_.animationTiming_.timingProtocol_,
            animationConfig_.windowAnimationConfig_.animationTiming_.timingCurve_, updateRSTreeFunc, [weakNode]() {
	    auto weakWindow = weakNode.promote();
            if (weakWindow && weakWindow->surfaceNode_) {
                weakWindow->surfaceNode_->SetAppFreeze(false);
            }
        });
        FinishTrace(HITRACE_TAG_WINDOW_MANAGER);
    } else if (node->GetWindowType() == WindowType::WINDOW_TYPE_INPUT_METHOD_FLOAT &&
        !animationPlayed) { // remove keyboard with animation
        auto timingProtocol = animationConfig_.keyboardAnimationConfig_.durationOut_;
        RSNode::Animate(timingProtocol, animationConfig_.keyboardAnimationConfig_.curve_, updateRSTreeFunc);
    } else {
        WLOGFD("remove window without animation");
        updateRSTreeFunc();
    }
    return true;
}

const std::vector<uint32_t>& WindowNodeContainer::Destroy()
{
    // clear vector cache completely, swap with empty vector
    auto emptyVector = std::vector<uint32_t>();
    removedIds_.swap(emptyVector);
    for (auto& node : belowAppWindowNode_->children_) {
        DestroyWindowNode(node, removedIds_);
    }
    for (auto& node : appWindowNode_->children_) {
        DestroyWindowNode(node, removedIds_);
    }
    for (auto& node : aboveAppWindowNode_->children_) {
        DestroyWindowNode(node, removedIds_);
    }
    return removedIds_;
}

sptr<WindowNode> WindowNodeContainer::FindRoot(WindowType type) const
{
    if (WindowHelper::IsAppWindow(type) || type == WindowType::WINDOW_TYPE_DOCK_SLICE ||
        type == WindowType::WINDOW_TYPE_APP_COMPONENT || type == WindowType::WINDOW_TYPE_PLACEHOLDER ||
        type == WindowType::WINDOW_TYPE_DIALOG) {
        return appWindowNode_;
    }
    if (WindowHelper::IsBelowSystemWindow(type)) {
        return belowAppWindowNode_;
    }
    if (WindowHelper::IsAboveSystemWindow(type)) {
        return aboveAppWindowNode_;
    }
    return nullptr;
}

sptr<WindowNode> WindowNodeContainer::FindWindowNodeById(uint32_t id) const
{
    std::vector<sptr<WindowNode>> rootNodes = { aboveAppWindowNode_, appWindowNode_, belowAppWindowNode_ };
    for (const auto& rootNode : rootNodes) {
        for (auto& node : rootNode->children_) {
            if (node->GetWindowId() == id) {
                return node;
            }
            for (auto& subNode : node->children_) {
                if (subNode->GetWindowId() == id) {
                    return subNode;
                }
            }
        }
    }
    return nullptr;
}

void WindowNodeContainer::UpdateFocusStatus(uint32_t id, bool focused)
{
    auto node = FindWindowNodeById(id);
    if (node == nullptr) {
        WLOGFW("cannot find focused window id:%{public}d", id);
        return;
    }
    if (focused) {
        focusedPid_ = node->GetCallingPid();
    }

    if (node->GetCallingPid() == 0) {
        WLOGFW("focused window is starting window, no need notify");
        return;
    }

    if (focused && node->GetWindowProperty() != nullptr) {
        AbilityInfo info = node->GetWindowProperty()->GetAbilityInfo();
        WLOGFW("current focus window: windowId: %{public}d, windowName: %{public}s, bundleName: %{public}s,"
            " abilityName: %{public}s, pid: %{public}d, uid: %{public}d", id,
            node->GetWindowProperty()->GetWindowName().c_str(), info.bundleName_.c_str(), info.abilityName_.c_str(),
            node->GetCallingPid(), node->GetCallingUid());
        FocusAppInfo appInfo = { node->GetCallingPid(), node->GetCallingUid(), info.bundleName_, info.abilityName_ };
        RSInterfaces::GetInstance().SetFocusAppInfo(appInfo);
    }
    if (node->GetWindowToken()) {
        node->GetWindowToken()->UpdateFocusStatus(focused);
    }
    if (node->abilityToken_ == nullptr) {
        WLOGFD("abilityToken is null, window : %{public}d", id);
    }
    sptr<FocusChangeInfo> focusChangeInfo = new FocusChangeInfo(node->GetWindowId(), node->GetDisplayId(),
        node->GetCallingPid(), node->GetCallingUid(), node->GetWindowType(), node->abilityToken_);
    WindowManagerAgentController::GetInstance().UpdateFocusChangeInfo(
        focusChangeInfo, focused);
}

void WindowNodeContainer::UpdateActiveStatus(uint32_t id, bool isActive)
{
    auto node = FindWindowNodeById(id);
    if (node == nullptr) {
        WLOGFE("cannot find active window id: %{public}d", id);
        return;
    }
    if (isActive) {
        activePid_ = node->GetCallingPid();
    }
    if (node->GetWindowToken()) {
        node->GetWindowToken()->UpdateActiveStatus(isActive);
    }
}

void WindowNodeContainer::UpdateBrightness(uint32_t id, bool byRemoved)
{
    auto node = FindWindowNodeById(id);
    if (node == nullptr) {
        WLOGFE("cannot find active window id: %{public}d", id);
        return;
    }

    if (!byRemoved) {
        if (!WindowHelper::IsAppWindow(node->GetWindowType())) {
            return;
        }
    }
    WLOGFD("brightness: [%{public}f, %{public}f]", GetDisplayBrightness(), node->GetBrightness());
    if (std::fabs(node->GetBrightness() - UNDEFINED_BRIGHTNESS) < std::numeric_limits<float>::min()) {
        if (GetDisplayBrightness() != node->GetBrightness()) {
            WLOGFD("adjust brightness with default value");
            DisplayPowerMgr::DisplayPowerMgrClient::GetInstance().RestoreBrightness();
            SetDisplayBrightness(UNDEFINED_BRIGHTNESS); // UNDEFINED_BRIGHTNESS means system default brightness
        }
        SetBrightnessWindow(INVALID_WINDOW_ID);
    } else {
        if (GetDisplayBrightness() != node->GetBrightness()) {
            WLOGFD("adjust brightness with value: %{public}u", ToOverrideBrightness(node->GetBrightness()));
            DisplayPowerMgr::DisplayPowerMgrClient::GetInstance().OverrideBrightness(
                ToOverrideBrightness(node->GetBrightness()));
            SetDisplayBrightness(node->GetBrightness());
        }
        SetBrightnessWindow(node->GetWindowId());
    }
}

void WindowNodeContainer::AssignZOrder()
{
    zOrder_ = 0;
    WindowNodeOperationFunc func = [this](sptr<WindowNode> node) {
        if (!node->leashWinSurfaceNode_ && !node->surfaceNode_ && !node->startingWinSurfaceNode_) {
            ++zOrder_;
            WLOGFE("Id: %{public}u has no surface nodes", node->GetWindowId());
            return false;
        }
        if (node->leashWinSurfaceNode_ != nullptr) {
            ++zOrder_;
            node->leashWinSurfaceNode_->SetPositionZ(zOrder_);
        }

        if (node->surfaceNode_ != nullptr) {
            ++zOrder_;
            node->surfaceNode_->SetPositionZ(zOrder_);
            node->zOrder_ = zOrder_;
        }
        // make sure starting window above app
        if (node->startingWinSurfaceNode_ != nullptr) {
            ++zOrder_;
            node->startingWinSurfaceNode_->SetPositionZ(zOrder_);
        }
        return false;
    };
    TraverseWindowTree(func, false);
    displayGroupController_->UpdateDisplayGroupWindowTree();
}

WMError WindowNodeContainer::SetFocusWindow(uint32_t windowId)
{
    if (focusedWindow_ == windowId) {
        WLOGFD("focused window no change, id: %{public}u, %{public}d", windowId, focusedPid_);
        // StartingWindow can be focused and this pid is 0, then notify info in UpdateFocusStatus.
        // This info is invalid, so we must notify again when first frame callback.
        if (focusedPid_ == 0) {
            UpdateFocusStatus(windowId, true);
        }
        return WMError::WM_DO_NOTHING;
    }
    UpdateFocusStatus(focusedWindow_, false);
    focusedWindow_ = windowId;
    UpdateFocusStatus(focusedWindow_, true);
    return WMError::WM_OK;
}

uint32_t WindowNodeContainer::GetFocusWindow() const
{
    return focusedWindow_;
}

WMError WindowNodeContainer::SetActiveWindow(uint32_t windowId, bool byRemoved)
{
    if (activeWindow_ == windowId) {
        WLOGFD("active window not change, id: %{public}u, %{public}d", windowId, activePid_);
        if (activePid_ == 0) {
            UpdateActiveStatus(windowId, true);
        }
        return WMError::WM_DO_NOTHING;
    }
    UpdateActiveStatus(activeWindow_, false);
    activeWindow_ = windowId;
    UpdateActiveStatus(activeWindow_, true);
    UpdateBrightness(activeWindow_, byRemoved);
    return WMError::WM_OK;
}

void WindowNodeContainer::SetDisplayBrightness(float brightness)
{
    displayBrightness_ = brightness;
}

float WindowNodeContainer::GetDisplayBrightness() const
{
    return displayBrightness_;
}

void WindowNodeContainer::SetBrightnessWindow(uint32_t windowId)
{
    brightnessWindow_ = windowId;
}

uint32_t WindowNodeContainer::GetBrightnessWindow() const
{
    return brightnessWindow_;
}

uint32_t WindowNodeContainer::ToOverrideBrightness(float brightness)
{
    return static_cast<uint32_t>(brightness * MAX_BRIGHTNESS);
}

uint32_t WindowNodeContainer::GetActiveWindow() const
{
    return activeWindow_;
}

sptr<WindowLayoutPolicy> WindowNodeContainer::GetLayoutPolicy() const
{
    return layoutPolicy_;
}

sptr<AvoidAreaController> WindowNodeContainer::GetAvoidController() const
{
    return avoidController_;
}

sptr<DisplayGroupController> WindowNodeContainer::GetMultiDisplayController() const
{
    return displayGroupController_;
}

sptr<WindowNode> WindowNodeContainer::GetRootNode(WindowRootNodeType type) const
{
    if (type == WindowRootNodeType::ABOVE_WINDOW_NODE) {
        return aboveAppWindowNode_;
    } else if (type == WindowRootNodeType::APP_WINDOW_NODE) {
        return appWindowNode_;
    } else if (type == WindowRootNodeType::BELOW_WINDOW_NODE) {
        return belowAppWindowNode_;
    }
    return nullptr;
}

void WindowNodeContainer::HandleKeepScreenOn(const sptr<WindowNode>& node, bool requireLock)
{
    if (requireLock && node->keepScreenLock_ == nullptr) {
        // reset ipc identity
        std::string identity = IPCSkeleton::ResetCallingIdentity();
        node->keepScreenLock_ = PowerMgr::PowerMgrClient::GetInstance().CreateRunningLock(node->GetWindowName(),
            PowerMgr::RunningLockType::RUNNINGLOCK_SCREEN);
        // set ipc identity to raw
        IPCSkeleton::SetCallingIdentity(identity);
    }
    if (node->keepScreenLock_ == nullptr) {
        return;
    }
    WLOGFD("handle keep screen on: [%{public}s, %{public}d]", node->GetWindowName().c_str(), requireLock);
    HITRACE_METER_FMT(HITRACE_TAG_WINDOW_MANAGER, "container:HandleKeepScreenOn(%s, %d)",
        node->GetWindowName().c_str(), requireLock);
    ErrCode res;
    // reset ipc identity
    std::string identity = IPCSkeleton::ResetCallingIdentity();
    if (requireLock) {
        res = node->keepScreenLock_->Lock();
    } else {
        res = node->keepScreenLock_->UnLock();
    }
    // set ipc identity to raw
    IPCSkeleton::SetCallingIdentity(identity);
    if (res != ERR_OK) {
        WLOGFE("handle keep screen running lock failed: [operation: %{public}d, err: %{public}d]", requireLock, res);
    }
}

bool WindowNodeContainer::IsAboveSystemBarNode(sptr<WindowNode> node) const
{
    int32_t curPriority = zorderPolicy_->GetWindowPriority(node->GetWindowType());
    if ((curPriority > zorderPolicy_->GetWindowPriority(WindowType::WINDOW_TYPE_STATUS_BAR)) &&
        (curPriority > zorderPolicy_->GetWindowPriority(WindowType::WINDOW_TYPE_NAVIGATION_BAR))) {
        return true;
    }
    return false;
}

bool WindowNodeContainer::IsSplitImmersiveNode(sptr<WindowNode> node) const
{
    auto type = node->GetWindowType();
    return node->IsSplitMode() || type == WindowType::WINDOW_TYPE_DOCK_SLICE;
}

std::unordered_map<WindowType, SystemBarProperty> WindowNodeContainer::GetExpectImmersiveProperty() const
{
    std::unordered_map<WindowType, SystemBarProperty> sysBarPropMap {
        { WindowType::WINDOW_TYPE_STATUS_BAR,     SystemBarProperty() },
        { WindowType::WINDOW_TYPE_NAVIGATION_BAR, SystemBarProperty() },
    };

    std::vector<sptr<WindowNode>> rootNodes = { aboveAppWindowNode_, appWindowNode_, belowAppWindowNode_ };
    for (const auto& node : rootNodes) {
        for (auto iter = node->children_.rbegin(); iter < node->children_.rend(); ++iter) {
            auto& sysBarPropMapNode = (*iter)->GetSystemBarProperty();
            if (IsAboveSystemBarNode(*iter)) {
                continue;
            }
            if (WindowHelper::IsFullScreenWindow((*iter)->GetWindowMode())
	        && (*iter)->GetWindowType() != WindowType::WINDOW_TYPE_PANEL) {
                WLOGFD("Top immersive window id: %{public}d. Use full immersive prop", (*iter)->GetWindowId());
                for (auto it : sysBarPropMap) {
                    sysBarPropMap[it.first] = (sysBarPropMapNode.find(it.first))->second;
                }
                return sysBarPropMap;
            } else if (IsSplitImmersiveNode(*iter)) {
                WLOGFD("Top split window id: %{public}d. Use split immersive prop", (*iter)->GetWindowId());
                for (auto it : sysBarPropMap) {
                    sysBarPropMap[it.first] = (sysBarPropMapNode.find(it.first))->second;
                    sysBarPropMap[it.first].enable_ = false;
                }
                return sysBarPropMap;
            }
        }
    }

    WLOGFD("No immersive window on top. Use default systembar Property");
    return sysBarPropMap;
}

void WindowNodeContainer::NotifyIfAvoidAreaChanged(const sptr<WindowNode>& node,
    const AvoidControlType avoidType) const
{
    auto checkFunc = [this](sptr<WindowNode> node) {
        return CheckWindowNodeWhetherInWindowTree(node);
    };
    avoidController_->ProcessWindowChange(node, avoidType, checkFunc);
    if (WindowHelper::IsSystemBarWindow(node->GetWindowType())) {
        NotifyIfSystemBarRegionChanged(node->GetDisplayId());
    } else {
        NotifyIfSystemBarTintChanged(node->GetDisplayId());
    }

    NotifyIfKeyboardRegionChanged(node, avoidType);
}

void WindowNodeContainer::BeforeProcessWindowAvoidAreaChangeWhenDisplayChange() const
{
    avoidController_->SetFlagForProcessWindowChange(true);
}

void WindowNodeContainer::ProcessWindowAvoidAreaChangeWhenDisplayChange() const
{
    avoidController_->SetFlagForProcessWindowChange(false);
    auto checkFunc = [this](sptr<WindowNode> node) {
        return CheckWindowNodeWhetherInWindowTree(node);
    };
    WindowNodeOperationFunc func = [avoidController = avoidController_, &checkFunc](sptr<WindowNode> node) {
        avoidController->ProcessWindowChange(node, AvoidControlType::AVOID_NODE_UPDATE, checkFunc);
        return false;
    };
    TraverseWindowTree(func, true);
}

void WindowNodeContainer::NotifyIfSystemBarTintChanged(DisplayId displayId) const
{
    HITRACE_METER(HITRACE_TAG_WINDOW_MANAGER);
    auto expectSystemBarProp = GetExpectImmersiveProperty();
    SystemBarRegionTints tints;
    SysBarTintMap& sysBarTintMap = displayGroupController_->sysBarTintMaps_[displayId];
    for (auto it : sysBarTintMap) {
        auto expectProp = expectSystemBarProp.find(it.first)->second;
        if (it.second.prop_ == expectProp) {
            continue;
        }
        WLOGFD("System bar prop update, Type: %{public}d, Visible: %{public}d, Color: %{public}x | %{public}x",
            static_cast<int32_t>(it.first), expectProp.enable_, expectProp.backgroundColor_, expectProp.contentColor_);
        sysBarTintMap[it.first].prop_ = expectProp;
        sysBarTintMap[it.first].type_ = it.first;
        tints.emplace_back(sysBarTintMap[it.first]);
    }
    WindowManagerAgentController::GetInstance().UpdateSystemBarRegionTints(displayId, tints);
}

void WindowNodeContainer::NotifyIfSystemBarRegionChanged(DisplayId displayId) const
{
    HITRACE_METER(HITRACE_TAG_WINDOW_MANAGER);
    SystemBarRegionTints tints;
    SysBarTintMap& sysBarTintMap = displayGroupController_->sysBarTintMaps_[displayId];
    SysBarNodeMap& sysBarNodeMap = displayGroupController_->sysBarNodeMaps_[displayId];
    for (auto it : sysBarTintMap) { // split screen mode not support yet
        auto sysNode = sysBarNodeMap[it.first];
        if (sysNode == nullptr || it.second.region_ == sysNode->GetWindowRect()) {
            continue;
        }
        const Rect& newRegion = sysNode->GetWindowRect();
        sysBarTintMap[it.first].region_ = newRegion;
        sysBarTintMap[it.first].type_ = it.first;
        tints.emplace_back(sysBarTintMap[it.first]);
        WLOGFD("system bar region update, type: %{public}d" \
            "region: [%{public}d, %{public}d, %{public}d, %{public}d]",
            static_cast<int32_t>(it.first), newRegion.posX_, newRegion.posY_, newRegion.width_, newRegion.height_);
    }
    WindowManagerAgentController::GetInstance().UpdateSystemBarRegionTints(displayId, tints);
}

void WindowNodeContainer::NotifyIfKeyboardRegionChanged(const sptr<WindowNode>& node,
    const AvoidControlType avoidType) const
{
    if (node->GetWindowType() != WindowType::WINDOW_TYPE_INPUT_METHOD_FLOAT) {
        WLOGFD("windowType: %{public}u", node->GetWindowType());
        return;
    }

    auto callingWindow = FindWindowNodeById(node->GetCallingWindow());
    if (callingWindow == nullptr) {
        WLOGFD("callingWindow: %{public}u does not be set", node->GetCallingWindow());
        callingWindow = FindWindowNodeById(GetFocusWindow());
    }
    if (callingWindow == nullptr || callingWindow->GetWindowToken() == nullptr) {
        WLOGFE("does not have correct callingWindow for input method window");
        return;
    }
    const WindowMode callingWindowMode = callingWindow->GetWindowMode();
    if (callingWindowMode == WindowMode::WINDOW_MODE_FULLSCREEN ||
        callingWindowMode == WindowMode::WINDOW_MODE_SPLIT_PRIMARY ||
        callingWindowMode == WindowMode::WINDOW_MODE_SPLIT_SECONDARY) {
        const Rect keyRect = node->GetWindowRect();
        const Rect callingRect = callingWindow->GetWindowRect();
        if (WindowHelper::IsEmptyRect(WindowHelper::GetOverlap(callingRect, keyRect, 0, 0))) {
            WLOGFD("no overlap between two windows");
            return;
        }
        Rect overlapRect = { 0, 0, 0, 0 };
        if (avoidType == AvoidControlType::AVOID_NODE_ADD || avoidType == AvoidControlType::AVOID_NODE_UPDATE) {
            overlapRect = WindowHelper::GetOverlap(keyRect, callingRect, callingRect.posX_, callingRect.posY_);
        }

        WLOGFD("keyboard size change callingWindow: [%{public}s, %{public}u], " \
        "overlap rect: [%{public}d, %{public}d, %{public}u, %{public}u]",
            callingWindow->GetWindowName().c_str(), callingWindow->GetWindowId(),
            overlapRect.posX_, overlapRect.posY_, overlapRect.width_, overlapRect.height_);
        sptr<OccupiedAreaChangeInfo> info = new OccupiedAreaChangeInfo(OccupiedAreaType::TYPE_INPUT, overlapRect);
        callingWindow->GetWindowToken()->UpdateOccupiedAreaChangeInfo(info);
        return;
    }
    WLOGFE("does not have correct callingWindowMode for input method window");
}

void WindowNodeContainer::NotifySystemBarTints(std::vector<DisplayId> displayIdVec)
{
    if (displayIdVec.size() != displayGroupController_->sysBarTintMaps_.size()) {
        WLOGE("[Immersive] the number of display is error");
    }

    for (auto displayId : displayIdVec) {
        SystemBarRegionTints tints;
        SysBarTintMap& sysBarTintMap = displayGroupController_->sysBarTintMaps_[displayId];
        for (auto it : sysBarTintMap) {
            WLOGFD("[Immersive] system bar cur notify, T: %{public}d, " \
                "V: %{public}d, C: %{public}x | %{public}x, " \
                "R: [%{public}d, %{public}d, %{public}d, %{public}d]",
                static_cast<int32_t>(it.first),
                sysBarTintMap[it.first].prop_.enable_,
                sysBarTintMap[it.first].prop_.backgroundColor_, sysBarTintMap[it.first].prop_.contentColor_,
                sysBarTintMap[it.first].region_.posX_, sysBarTintMap[it.first].region_.posY_,
                sysBarTintMap[it.first].region_.width_, sysBarTintMap[it.first].region_.height_);
            tints.push_back(sysBarTintMap[it.first]);
        }
        WindowManagerAgentController::GetInstance().UpdateSystemBarRegionTints(displayId, tints);
    }
}

void WindowNodeContainer::NotifyDockWindowStateChanged(sptr<WindowNode>& node, bool isEnable)
{
    HITRACE_METER(HITRACE_TAG_WINDOW_MANAGER);
    WLOGFD("[Immersive] begin isEnable: %{public}d", isEnable);
    if (isEnable) {
        for (auto& windowNode : appWindowNode_->children_) {
            if (windowNode->GetWindowId() == node->GetWindowId()) {
                continue;
            }
            if (!WindowHelper::IsFloatingWindow(windowNode->GetWindowMode())) {
                return;
            }
        }
    }
    SystemBarProperty prop;
    prop.enable_ = isEnable;
    SystemBarRegionTint tint;
    tint.type_ = WindowType::WINDOW_TYPE_LAUNCHER_DOCK;
    tint.prop_ = prop;
    SystemBarRegionTints tints;
    tints.push_back(tint);
    WindowManagerAgentController::GetInstance().UpdateSystemBarRegionTints(node->GetDisplayId(), tints);
}

void WindowNodeContainer::UpdateAvoidAreaListener(sptr<WindowNode>& windowNode, bool haveAvoidAreaListener)
{
    avoidController_->UpdateAvoidAreaListener(windowNode, haveAvoidAreaListener);
}

bool WindowNodeContainer::IsTopWindow(uint32_t windowId, sptr<WindowNode>& rootNode) const
{
    if (rootNode->children_.empty()) {
        WLOGFE("root does not have any node");
        return false;
    }
    auto node = *(rootNode->children_.rbegin());
    if (node == nullptr) {
        WLOGFE("window tree does not have any node");
        return false;
    }

    for (auto iter = node->children_.rbegin(); iter < node->children_.rend(); iter++) {
        if ((*iter)->priority_ > 0) {
            return (*iter)->GetWindowId() == windowId;
        } else {
            break;
        }
    }
    return node->GetWindowId() == windowId;
}

void WindowNodeContainer::RaiseOrderedWindowToTop(std::vector<sptr<WindowNode>>& orderedNodes,
    std::vector<sptr<WindowNode>>& windowNodes)
{
    for (auto iter = appWindowNode_->children_.begin(); iter != appWindowNode_->children_.end();) {
        uint32_t wid = (*iter)->GetWindowId();
        auto orderedIter = std::find_if(orderedNodes.begin(), orderedNodes.end(),
            [wid] (sptr<WindowNode> orderedNode) { return orderedNode->GetWindowId() == wid; });
        if (orderedIter != orderedNodes.end()) {
            iter = windowNodes.erase(iter);
        } else {
            iter++;
        }
    }
    for (auto iter = orderedNodes.begin(); iter != orderedNodes.end(); iter++) {
        UpdateWindowTree(*iter);
    }
    return;
}

void WindowNodeContainer::RaiseWindowToTop(uint32_t windowId, std::vector<sptr<WindowNode>>& windowNodes)
{
    if (windowNodes.empty()) {
        WLOGFE("windowNodes is empty!");
        return;
    }
    auto iter = std::find_if(windowNodes.begin(), windowNodes.end(),
                             [windowId](sptr<WindowNode> node) {
                                 return node->GetWindowId() == windowId;
                             });
    // raise app node window to top
    if (iter != windowNodes.end()) {
        sptr<WindowNode> node = *iter;
        windowNodes.erase(iter);
        UpdateWindowTree(node);
        WLOGFD("raise window to top %{public}u", node->GetWindowId());
    }
}

void WindowNodeContainer::TraverseContainer(std::vector<sptr<WindowNode>>& windowNodes) const
{
    for (auto& node : belowAppWindowNode_->children_) {
        TraverseWindowNode(node, windowNodes);
    }
    for (auto& node : appWindowNode_->children_) {
        TraverseWindowNode(node, windowNodes);
    }
    for (auto& node : aboveAppWindowNode_->children_) {
        TraverseWindowNode(node, windowNodes);
    }
    std::reverse(windowNodes.begin(), windowNodes.end());
}

void WindowNodeContainer::TraverseWindowNode(sptr<WindowNode>& node, std::vector<sptr<WindowNode>>& windowNodes) const
{
    if (node == nullptr) {
        return;
    }
    auto iter = node->children_.begin();
    for (; iter < node->children_.end(); ++iter) {
        if ((*iter)->priority_ < 0) {
            windowNodes.emplace_back(*iter);
        } else {
            break;
        }
    }
    windowNodes.emplace_back(node);
    for (; iter < node->children_.end(); ++iter) {
        windowNodes.emplace_back(*iter);
    }
}

AvoidArea WindowNodeContainer::GetAvoidAreaByType(const sptr<WindowNode>& node, AvoidAreaType avoidAreaType) const
{
    if (CheckWindowNodeWhetherInWindowTree(node)) {
        return avoidController_->GetAvoidAreaByType(node, avoidAreaType);
    }
    return {};
}

bool WindowNodeContainer::CheckWindowNodeWhetherInWindowTree(const sptr<WindowNode>& node) const
{
    bool isInWindowTree = false;
    WindowNodeOperationFunc func = [&node, &isInWindowTree](sptr<WindowNode> windowNode) {
        if (node->GetWindowId() == windowNode->GetWindowId()) {
            isInWindowTree = true;
            return true;
        }
        return false;
    };
    TraverseWindowTree(func, true);
    return isInWindowTree;
}

void WindowNodeContainer::DumpScreenWindowTree()
{
    WLOGFD("-------- dump window info begin---------");
    WLOGFD("WindowName DisplayId WinId Type Mode Flag ZOrd Orientation abilityToken [   x    y    w    h]");
    uint32_t zOrder = zOrder_;
    WindowNodeOperationFunc func = [&zOrder](sptr<WindowNode> node) {
        Rect rect = node->GetWindowRect();
        const std::string& windowName = node->GetWindowName().size() < WINDOW_NAME_MAX_LENGTH ?
            node->GetWindowName() : node->GetWindowName().substr(0, WINDOW_NAME_MAX_LENGTH);
        WLOGFD("DumpScreenWindowTree: %{public}10s %{public}9" PRIu64" %{public}5u %{public}4u %{public}4u %{public}4u "
            "%{public}4u %{public}11u %{public}12d [%{public}4d %{public}4d %{public}4u %{public}4u]",
            windowName.c_str(), node->GetDisplayId(), node->GetWindowId(), node->GetWindowType(), node->GetWindowMode(),
            node->GetWindowFlags(), --zOrder, static_cast<uint32_t>(node->GetRequestedOrientation()),
            node->abilityToken_ != nullptr, rect.posX_, rect.posY_, rect.width_, rect.height_);
        return false;
    };
    TraverseWindowTree(func, true);
    WLOGFD("-------- dump window info end  ---------");
}

Rect WindowNodeContainer::GetDisplayRect(DisplayId displayId) const
{
    return displayGroupInfo_->GetDisplayRect(displayId);
}

bool WindowNodeContainer::IsVerticalDisplay(DisplayId displayId) const
{
    return displayGroupInfo_->GetDisplayRect(displayId).width_ < displayGroupInfo_->GetDisplayRect(displayId).height_;
}

void WindowNodeContainer::ProcessWindowStateChange(WindowState state, WindowStateChangeReason reason)
{
    switch (reason) {
        case WindowStateChangeReason::KEYGUARD: {
            int32_t topPriority = zorderPolicy_->GetWindowPriority(WindowType::WINDOW_TYPE_KEYGUARD);
            TraverseAndUpdateWindowState(state, topPriority);
            break;
        }
        default:
            return;
    }
}

void WindowNodeContainer::TraverseAndUpdateWindowState(WindowState state, int32_t topPriority)
{
    std::vector<sptr<WindowNode>> rootNodes = { belowAppWindowNode_, appWindowNode_, aboveAppWindowNode_ };
    for (auto& node : rootNodes) {
        UpdateWindowState(node, topPriority, state);
    }
}

void WindowNodeContainer::UpdateWindowState(sptr<WindowNode> node, int32_t topPriority, WindowState state)
{
    if (node == nullptr) {
        return;
    }
    if (node->parent_ != nullptr && node->currentVisibility_) {
        if (node->priority_ < topPriority && !WindowHelper::IsShowWhenLocked(node->GetWindowFlags()) &&
            !WindowHelper::IsShowWhenLocked(node->parent_->GetWindowFlags())) {
            if (node->GetWindowToken()) {
                node->GetWindowToken()->UpdateWindowState(state);
            }
            HandleKeepScreenOn(node, state);
        }
    }
    for (auto& childNode : node->children_) {
        UpdateWindowState(childNode, topPriority, state);
    }
}

void WindowNodeContainer::HandleKeepScreenOn(const sptr<WindowNode>& node, WindowState state)
{
    if (node == nullptr) {
        WLOGFE("window is invalid");
        return;
    }
    if (state == WindowState::STATE_FROZEN) {
        HandleKeepScreenOn(node, false);
    } else if (state == WindowState::STATE_UNFROZEN) {
        HandleKeepScreenOn(node, node->IsKeepScreenOn());
    } else {
        // do nothing
    }
}

sptr<WindowNode> WindowNodeContainer::FindDividerNode() const
{
    for (auto iter = appWindowNode_->children_.begin(); iter != appWindowNode_->children_.end(); iter++) {
        if ((*iter)->GetWindowType() == WindowType::WINDOW_TYPE_DOCK_SLICE) {
            return *iter;
        }
    }
    return nullptr;
}

void WindowNodeContainer::RaiseSplitRelatedWindowToTop(sptr<WindowNode>& node)
{
    if (node == nullptr) {
        return;
    }
    auto windowPair = displayGroupController_->GetWindowPairByDisplayId(node->GetDisplayId());
    if (windowPair == nullptr) {
        WLOGFE("Window pair is nullptr");
        return;
    }
    std::vector<sptr<WindowNode>> orderedPair = windowPair->GetOrderedPair(node);
    RaiseOrderedWindowToTop(orderedPair, appWindowNode_->children_);
    AssignZOrder();
    return;
}

WMError WindowNodeContainer::RaiseZOrderForAppWindow(sptr<WindowNode>& node, sptr<WindowNode>& parentNode)
{
    if (node == nullptr) {
        return WMError::WM_ERROR_NULLPTR;
    }
    if (IsTopWindow(node->GetWindowId(), appWindowNode_) || IsTopWindow(node->GetWindowId(), aboveAppWindowNode_)) {
        WLOGFD("it is already top app window, id: %{public}u", node->GetWindowId());
        return WMError::WM_ERROR_INVALID_TYPE;
    }

    if (WindowHelper::IsSubWindow(node->GetWindowType()) ||
        (node->GetWindowType() == WindowType::WINDOW_TYPE_DIALOG)) {
        if (parentNode == nullptr) {
            WLOGFE("window type is invalid");
            return WMError::WM_ERROR_NULLPTR;
        }
        RaiseWindowToTop(node->GetWindowId(), parentNode->children_); // raise itself
        if (parentNode->IsSplitMode()) {
            RaiseSplitRelatedWindowToTop(parentNode);
        } else if (parentNode->parent_ != nullptr) {
            RaiseWindowToTop(parentNode->GetWindowId(), parentNode->parent_->children_); // raise parent window
        }
    } else if (WindowHelper::IsMainWindow(node->GetWindowType())) {
        if (node->IsSplitMode()) {
            RaiseSplitRelatedWindowToTop(node);
        } else {
            // remote animation continuous start and exit allow parent is nullptr
            if (node->parent_ == nullptr) {
                WLOGFW("node parent is nullptr");
                return WMError::WM_OK;
            }
            RaiseWindowToTop(node->GetWindowId(), node->parent_->children_);
        }
    } else {
        // do nothing
    }
    AssignZOrder();
    WLOGFD("RaiseZOrderForAppWindow finished");
    DumpScreenWindowTree();
    return WMError::WM_OK;
}

sptr<WindowNode> WindowNodeContainer::GetNextFocusableWindow(uint32_t windowId) const
{
    sptr<WindowNode> nextFocusableWindow;
    bool previousFocusedWindowFound = false;
    WindowNodeOperationFunc func = [windowId, &nextFocusableWindow, &previousFocusedWindowFound](
        sptr<WindowNode> node) {
        if (previousFocusedWindowFound && node->GetWindowProperty()->GetFocusable() && node->currentVisibility_) {
            nextFocusableWindow = node;
            return true;
        }
        if (node->GetWindowId() == windowId) {
            previousFocusedWindowFound = true;
        }
        return false;
    };
    TraverseWindowTree(func, true);
    return nextFocusableWindow;
}

sptr<WindowNode> WindowNodeContainer::GetNextRotatableWindow(uint32_t windowId) const
{
    sptr<WindowNode> nextRotatableWindow;
    WindowNodeOperationFunc func = [windowId, &nextRotatableWindow](
        sptr<WindowNode> node) {
        if (windowId != node->GetWindowId() &&
            WindowHelper::IsRotatableWindow(node->GetWindowType(), node->GetWindowMode())) {
            nextRotatableWindow = node;
            return true;
        }
        return false;
    };
    TraverseWindowTree(func, true);
    return nextRotatableWindow;
}

sptr<WindowNode> WindowNodeContainer::GetNextActiveWindow(uint32_t windowId) const
{
    auto currentNode = FindWindowNodeById(windowId);
    if (currentNode == nullptr) {
        WLOGFE("cannot find window id: %{public}u by tree", windowId);
        return nullptr;
    }
    WLOGFD("current window: [%{public}u, %{public}u]", windowId, static_cast<uint32_t>(currentNode->GetWindowType()));
    if (WindowHelper::IsSystemWindow(currentNode->GetWindowType())) {
        for (auto& node : appWindowNode_->children_) {
            if (node->GetWindowType() == WindowType::WINDOW_TYPE_DOCK_SLICE) {
                continue;
            }
            return node;
        }
        for (auto& node : belowAppWindowNode_->children_) {
            if (node->GetWindowType() == WindowType::WINDOW_TYPE_DESKTOP) {
                return node;
            }
        }
    } else if (WindowHelper::IsAppWindow(currentNode->GetWindowType())) {
        std::vector<sptr<WindowNode>> windowNodes;
        TraverseContainer(windowNodes);
        auto iter = std::find_if(windowNodes.begin(), windowNodes.end(), [windowId](sptr<WindowNode>& node) {
            return node->GetWindowId() == windowId;
            });
        if (iter == windowNodes.end()) {
            WLOGFE("could not find this window");
            return nullptr;
        }
        int index = std::distance(windowNodes.begin(), iter);
        for (size_t i = static_cast<size_t>(index) + 1; i < windowNodes.size(); i++) {
            if (windowNodes[i]->GetWindowType() == WindowType::WINDOW_TYPE_DOCK_SLICE
                || !windowNodes[i]->currentVisibility_) {
                continue;
            }
            return windowNodes[i];
        }
    } else {
        // do nothing
    }
    WLOGFE("could not get next active window");
    return nullptr;
}

bool WindowNodeContainer::IsForbidDockSliceMove(DisplayId displayId) const
{
    auto windowPair = displayGroupController_->GetWindowPairByDisplayId(displayId);
    if (windowPair == nullptr) {
        WLOGFE("window pair is nullptr");
        return true;
    }
    if (windowPair->IsForbidDockSliceMove()) {
        return true;
    }
    return false;
}

bool WindowNodeContainer::IsDockSliceInExitSplitModeArea(DisplayId displayId) const
{
    auto windowPair = displayGroupController_->GetWindowPairByDisplayId(displayId);
    if (windowPair == nullptr) {
        WLOGFE("window pair is nullptr");
        return false;
    }
    std::vector<int32_t> exitSplitPoints = layoutPolicy_->GetExitSplitPoints(displayId);
    if (exitSplitPoints.size() != EXIT_SPLIT_POINTS_NUMBER) {
        return false;
    }
    return windowPair->IsDockSliceInExitSplitModeArea(exitSplitPoints);
}

void WindowNodeContainer::ExitSplitMode(DisplayId displayId)
{
    auto windowPair = displayGroupController_->GetWindowPairByDisplayId(displayId);
    if (windowPair == nullptr) {
        WLOGFE("window pair is nullptr");
        return;
    }
    windowPair->ExitSplitMode();
}

void WindowNodeContainer::MinimizeAllAppWindows(DisplayId displayId)
{
    WMError ret = MinimizeAppNodeExceptOptions(MinimizeReason::MINIMIZE_ALL);
    SwitchLayoutPolicy(WindowLayoutMode::CASCADE, displayId);
    if (ret != WMError::WM_OK) {
        WLOGFE("Minimize all app window failed");
    }
    return;
}

sptr<WindowNode> WindowNodeContainer::GetDeskTopWindow()
{
    sptr<WindowNode> deskTop;
    WindowNodeOperationFunc findDeskTopFunc = [this, &deskTop](sptr<WindowNode> node) {
        if (node->GetWindowProperty()->GetWindowType() == WindowType::WINDOW_TYPE_DESKTOP) {
            deskTop = node;
            return true;
        }
        return false;
    };
    TraverseWindowTree(findDeskTopFunc, false);
    return deskTop;
}

bool WindowNodeContainer::HasPrivateWindow()
{
    std::vector<sptr<WindowNode>> windowNodes;
    TraverseContainer(windowNodes);
    for (const auto& node : windowNodes) {
        if (node->isVisible_ && node->GetWindowProperty()->GetPrivacyMode()) {
            WLOGFD("window name %{public}s", node->GetWindowName().c_str());
            return true;
        }
    }
    return false;
}

void WindowNodeContainer::MinimizeOldestAppWindow()
{
    for (auto& appNode : appWindowNode_->children_) {
        if (appNode->GetWindowType() == WindowType::WINDOW_TYPE_APP_MAIN_WINDOW) {
            MinimizeApp::AddNeedMinimizeApp(appNode, MinimizeReason::MAX_APP_COUNT);
            return;
        }
    }
    for (auto& appNode : aboveAppWindowNode_->children_) {
        if (appNode->GetWindowType() == WindowType::WINDOW_TYPE_APP_MAIN_WINDOW) {
            MinimizeApp::AddNeedMinimizeApp(appNode, MinimizeReason::MAX_APP_COUNT);
            return;
        }
    }
    WLOGFD("no window needs to minimize");
}

WMError WindowNodeContainer::ToggleShownStateForAllAppWindows(
    std::function<bool(uint32_t, WindowMode)> restoreFunc, bool restore)
{
    WLOGFD("ToggleShownStateForAllAppWindows");
    for (auto node : aboveAppWindowNode_->children_) {
        if (node->GetWindowType() == WindowType::WINDOW_TYPE_LAUNCHER_RECENT &&
            node->GetWindowMode() == WindowMode::WINDOW_MODE_FULLSCREEN && restore) {
            return WMError::WM_DO_NOTHING;
        }
    }
    // to do, backup reentry: 1.ToggleShownStateForAllAppWindows fast; 2.this display should reset backupWindowIds_.
    if (!restore && appWindowNode_->children_.empty() && !backupWindowIds_.empty()) {
        backupWindowIds_.clear();
        backupWindowMode_.clear();
        backupDisplaySplitWindowMode_.clear();
        backupDividerWindowRect_.clear();
    }
    if (!restore && !appWindowNode_->children_.empty() && backupWindowIds_.empty()) {
        WLOGFD("backup");
        BackUpAllAppWindows();
    } else if (restore && !backupWindowIds_.empty()) {
        WLOGFD("restore");
        RestoreAllAppWindows(restoreFunc);
    } else {
        WLOGFD("do nothing because shown app windows is empty or backup windows is empty.");
    }
    return WMError::WM_OK;
}

void WindowNodeContainer::BackUpAllAppWindows()
{
    std::set<DisplayId> displayIdSet;
    backupWindowMode_.clear();
    backupDisplaySplitWindowMode_.clear();
    std::vector<sptr<WindowNode>> children = appWindowNode_->children_;
    for (auto& appNode : children) {
        if (!WindowHelper::IsMainWindow(appNode->GetWindowType())) {
            continue;
        }
        auto windowMode = appNode->GetWindowMode();
        backupWindowMode_[appNode->GetWindowId()] = windowMode;
        if (WindowHelper::IsSplitWindowMode(windowMode)) {
            backupDisplaySplitWindowMode_[appNode->GetDisplayId()].insert(windowMode);
        }
        displayIdSet.insert(appNode->GetDisplayId());
    }
    for (auto& appNode : children) {
        // exclude exceptional window
        if (!WindowHelper::IsMainWindow(appNode->GetWindowType())) {
            WLOGFE("is not main window, windowId:%{public}u", appNode->GetWindowId());
            continue;
        }
        // minimize window
        WLOGFD("minimize window, windowId:%{public}u", appNode->GetWindowId());
        backupWindowIds_.emplace_back(appNode->GetWindowId());
        WindowManagerService::GetInstance().RemoveWindow(appNode->GetWindowId());
        wptr<IRemoteObject> abilityToken = appNode->abilityToken_;
        WindowInnerManager::GetInstance().PostTask([abilityToken]() {
            auto token = abilityToken.promote();
            if (token == nullptr) {
                WLOGFW("Ability token is null");
                return;
            }
            AAFwk::AbilityManagerClient::GetInstance()->DoAbilityBackground(token,
                static_cast<uint32_t>(WindowStateChangeReason::TOGGLING));
        });
    }
    backupDividerWindowRect_.clear();
    for (auto displayId : displayIdSet) {
        auto windowPair = displayGroupController_->GetWindowPairByDisplayId(displayId);
        if (windowPair == nullptr || windowPair->GetDividerWindow() == nullptr) {
            continue;
        }
        backupDividerWindowRect_[displayId] = windowPair->GetDividerWindow()->GetWindowRect();
    }
}

void WindowNodeContainer::RestoreAllAppWindows(std::function<bool(uint32_t, WindowMode)> restoreFunc)
{
    std::vector<uint32_t> backupWindowIds(backupWindowIds_);
    auto displayIds = DisplayManagerServiceInner::GetInstance().GetAllDisplayIds();
    std::vector<sptr<WindowPair>> windowPairs;
    for (auto displayId : displayIds) {
        auto windowPair = displayGroupController_->GetWindowPairByDisplayId(displayId);
        if (windowPair != nullptr) {
            if (backupDisplaySplitWindowMode_[displayId].count(WindowMode::WINDOW_MODE_SPLIT_PRIMARY) > 0 &&
                backupDisplaySplitWindowMode_[displayId].count(WindowMode::WINDOW_MODE_SPLIT_SECONDARY) > 0) {
                windowPair->SetAllSplitAppWindowsRestoring(true);
            }
            windowPairs.emplace_back(windowPair);
        }
    }
    for (auto windowId: backupWindowIds) {
        if (!restoreFunc(windowId, backupWindowMode_[windowId])) {
            WLOGFE("restore %{public}u failed", windowId);
            continue;
        }
        WLOGFD("restore %{public}u", windowId);
    }
    for (auto windowPair : windowPairs) {
        windowPair->SetAllSplitAppWindowsRestoring(false);
    }
    layoutPolicy_->SetSplitDividerWindowRects(backupDividerWindowRect_);
    backupWindowIds_.clear();
    backupWindowMode_.clear();
    backupDividerWindowRect_.clear();
}

bool WindowNodeContainer::IsAppWindowsEmpty() const
{
    return appWindowNode_->children_.empty();
}

WMError WindowNodeContainer::MinimizeAppNodeExceptOptions(MinimizeReason reason,
    const std::vector<uint32_t> &exceptionalIds, const std::vector<WindowMode> &exceptionalModes)
{
    if (appWindowNode_->children_.empty()) {
        return WMError::WM_OK;
    }
    for (auto& appNode : appWindowNode_->children_) {
        // exclude exceptional window
        if (std::find(exceptionalIds.begin(), exceptionalIds.end(), appNode->GetWindowId()) != exceptionalIds.end() ||
            std::find(exceptionalModes.begin(), exceptionalModes.end(),
                appNode->GetWindowMode()) != exceptionalModes.end() ||
                appNode->GetWindowType() != WindowType::WINDOW_TYPE_APP_MAIN_WINDOW) {
            continue;
        }
        MinimizeApp::AddNeedMinimizeApp(appNode, reason);
    }
    return WMError::WM_OK;
}

WMError WindowNodeContainer::MinimizeStructuredAppWindowsExceptSelf(const sptr<WindowNode>& node)
{
    std::vector<uint32_t> exceptionalIds = { node->GetWindowId() };
    std::vector<WindowMode> exceptionalModes = { WindowMode::WINDOW_MODE_FLOATING, WindowMode::WINDOW_MODE_PIP };
    return MinimizeAppNodeExceptOptions(MinimizeReason::OTHER_WINDOW, exceptionalIds, exceptionalModes);
}

void WindowNodeContainer::ResetLayoutPolicy()
{
    layoutPolicy_->Reset();
}

WMError WindowNodeContainer::SwitchLayoutPolicy(WindowLayoutMode dstMode, DisplayId displayId, bool reorder)
{
    WLOGFD("SwitchLayoutPolicy src: %{public}d dst: %{public}d, reorder: %{public}d, displayId: %{public}" PRIu64"",
        static_cast<uint32_t>(layoutMode_), static_cast<uint32_t>(dstMode), static_cast<uint32_t>(reorder), displayId);
    if (dstMode < WindowLayoutMode::BASE || dstMode >= WindowLayoutMode::END) {
        WLOGFE("invalid layout mode");
        return WMError::WM_ERROR_INVALID_PARAM;
    }
    auto windowPair = displayGroupController_->GetWindowPairByDisplayId(displayId);
    if (windowPair == nullptr) {
        WLOGFE("Window pair is nullptr");
        return WMError::WM_ERROR_NULLPTR;
    }
    if (layoutMode_ != dstMode) {
        if (layoutMode_ == WindowLayoutMode::CASCADE) {
            layoutPolicy_->Reset();
            windowPair->Clear();
        }
        layoutMode_ = dstMode;
        layoutPolicy_->Clean();
        layoutPolicy_ = layoutPolicies_[dstMode];
        layoutPolicy_->Launch();
        DumpScreenWindowTree();
    } else {
        WLOGFD("Current layout mode is already: %{public}d", static_cast<uint32_t>(dstMode));
    }
    if (reorder) {
        windowPair->Clear();
        layoutPolicy_->Reorder();
        DumpScreenWindowTree();
    }
    NotifyIfSystemBarTintChanged(displayId);
    return WMError::WM_OK;
}

void WindowNodeContainer::UpdateModeSupportInfoWhenKeyguardChange(const sptr<WindowNode>& node, bool up)
{
    if (!WindowHelper::IsWindowModeSupported(node->GetWindowProperty()->GetRequestModeSupportInfo(),
                                             WindowMode::WINDOW_MODE_SPLIT_PRIMARY)) {
        WLOGFD("window doesn't support split mode, winId: %{public}d", node->GetWindowId());
        return;
    }
    uint32_t modeSupportInfo;
    if (up) {
        modeSupportInfo = node->GetModeSupportInfo() & (~WindowModeSupport::WINDOW_MODE_SUPPORT_SPLIT_PRIMARY);
    } else {
        modeSupportInfo = node->GetModeSupportInfo() | WindowModeSupport::WINDOW_MODE_SUPPORT_SPLIT_PRIMARY;
    }
    node->SetModeSupportInfo(modeSupportInfo);
    if (node->GetWindowToken() != nullptr) {
        node->GetWindowToken()->UpdateWindowModeSupportInfo(modeSupportInfo);
    }
}

void WindowNodeContainer::RaiseInputMethodWindowPriorityIfNeeded(const sptr<WindowNode>& node) const
{
    if (node->GetWindowType() != WindowType::WINDOW_TYPE_INPUT_METHOD_FLOAT || !isScreenLocked_) {
        return;
    }

    WLOGFD("raise input method float window priority.");
    node->priority_ = zorderPolicy_->GetWindowPriority(
        WindowType::WINDOW_TYPE_KEYGUARD) + 2; // 2: higher than keyguard and show when locked window
}

void WindowNodeContainer::ReZOrderShowWhenLockedWindows(bool up)
{
    WLOGFD("Keyguard change %{public}u, re-zorder showWhenLocked window", up);
    std::vector<sptr<WindowNode>> needReZOrderNodes;
    auto& srcRoot = up ? appWindowNode_ : aboveAppWindowNode_;
    auto& dstRoot = up ? aboveAppWindowNode_ : appWindowNode_;

    auto dstPriority = up ? zorderPolicy_->GetWindowPriority(WindowType::WINDOW_TYPE_KEYGUARD) + 1 :
        zorderPolicy_->GetWindowPriority(WindowType::WINDOW_TYPE_APP_MAIN_WINDOW);

    for (auto iter = srcRoot->children_.begin(); iter != srcRoot->children_.end();) {
        if ((*iter)->GetWindowFlags() & static_cast<uint32_t>(WindowFlag::WINDOW_FLAG_SHOW_WHEN_LOCKED)) {
            needReZOrderNodes.emplace_back(*iter);
            iter = srcRoot->children_.erase(iter);
        } else {
            iter++;
        }
    }

    for (auto& needReZOrderNode : needReZOrderNodes) {
        needReZOrderNode->priority_ = dstPriority;
        needReZOrderNode->parent_ = dstRoot;
        auto parentNode = needReZOrderNode->parent_;
        auto position = parentNode->children_.end();
        for (auto iter = parentNode->children_.begin(); iter < parentNode->children_.end(); ++iter) {
            if ((*iter)->priority_ > needReZOrderNode->priority_) {
                position = iter;
                break;
            }
        }

        UpdateModeSupportInfoWhenKeyguardChange(needReZOrderNode, up);

        parentNode->children_.insert(position, needReZOrderNode);
        if (up && WindowHelper::IsSplitWindowMode(needReZOrderNode->GetWindowMode())) {
            needReZOrderNode->GetWindowProperty()->ResumeLastWindowMode();
            if (needReZOrderNode->GetWindowToken() != nullptr) {
                needReZOrderNode->GetWindowToken()->UpdateWindowMode(needReZOrderNode->GetWindowMode());
            }
            auto windowPair = displayGroupController_->GetWindowPairByDisplayId(needReZOrderNode->GetDisplayId());
            if (windowPair == nullptr) {
                WLOGFE("Window pair is nullptr");
                return;
            }
            windowPair->UpdateIfSplitRelated(needReZOrderNode);
        }
        WLOGFD("window %{public}u re-zorder when keyguard change %{public}u", needReZOrderNode->GetWindowId(), up);
    }
}

void WindowNodeContainer::ReZOrderShowWhenLockedWindowIfNeeded(const sptr<WindowNode>& node)
{
    if (!(node->GetWindowFlags() & static_cast<uint32_t>(WindowFlag::WINDOW_FLAG_SHOW_WHEN_LOCKED)) ||
        !isScreenLocked_) {
        return;
    }

    WLOGFD("ShowWhenLocked window %{public}u re-zorder to up", node->GetWindowId());
    ReZOrderShowWhenLockedWindows(true);
}

void WindowNodeContainer::RaiseShowWhenLockedWindowIfNeeded(const sptr<WindowNode>& node)
{
    // if keyguard window show, raise show when locked windows
    if (node->GetWindowType() == WindowType::WINDOW_TYPE_KEYGUARD) {
        ReZOrderShowWhenLockedWindows(true);
        return;
    }

    // if show when locked window show, raise itself when exist keyguard
    if (!(node->GetWindowFlags() & static_cast<uint32_t>(WindowFlag::WINDOW_FLAG_SHOW_WHEN_LOCKED)) ||
        !isScreenLocked_) {
        return;
    }

    WLOGFD("ShowWhenLocked window %{public}u raise itself", node->GetWindowId());
    node->priority_ = zorderPolicy_->GetWindowPriority(WindowType::WINDOW_TYPE_KEYGUARD) + 1;
    node->parent_ = aboveAppWindowNode_;
    if (WindowHelper::IsSplitWindowMode(node->GetWindowMode())) {
        node->GetWindowProperty()->ResumeLastWindowMode();
        if (node->GetWindowToken() != nullptr) {
            node->GetWindowToken()->UpdateWindowMode(node->GetWindowMode());
        }
    }
}

void WindowNodeContainer::DropShowWhenLockedWindowIfNeeded(const sptr<WindowNode>& node)
{
    // if keyguard window hide, drop show when locked windows
    if (node->GetWindowType() == WindowType::WINDOW_TYPE_KEYGUARD) {
        ReZOrderShowWhenLockedWindows(false);
        AssignZOrder();
    }
}

void WindowNodeContainer::TraverseWindowTree(const WindowNodeOperationFunc& func, bool isFromTopToBottom) const
{
    std::vector<sptr<WindowNode>> rootNodes = { belowAppWindowNode_, appWindowNode_, aboveAppWindowNode_ };
    if (isFromTopToBottom) {
        std::reverse(rootNodes.begin(), rootNodes.end());
    }

    for (const auto& node : rootNodes) {
        if (isFromTopToBottom) {
            for (auto iter = node->children_.rbegin(); iter != node->children_.rend(); ++iter) {
                if (TraverseFromTopToBottom(*iter, func)) {
                    return;
                }
            }
        } else {
            for (auto iter = node->children_.begin(); iter != node->children_.end(); ++iter) {
                if (TraverseFromBottomToTop(*iter, func)) {
                    return;
                }
            }
        }
    }
}

bool WindowNodeContainer::TraverseFromTopToBottom(sptr<WindowNode> node, const WindowNodeOperationFunc& func) const
{
    if (node == nullptr) {
        return false;
    }
    auto iterBegin = node->children_.rbegin();
    for (; iterBegin != node->children_.rend(); ++iterBegin) {
        if ((*iterBegin)->priority_ <= 0) {
            break;
        }
        if (func(*iterBegin)) {
            return true;
        }
    }
    if (func(node)) {
        return true;
    }
    for (; iterBegin != node->children_.rend(); ++iterBegin) {
        if (func(*iterBegin)) {
            return true;
        }
    }
    return false;
}

bool WindowNodeContainer::TraverseFromBottomToTop(sptr<WindowNode> node, const WindowNodeOperationFunc& func) const
{
    if (node == nullptr) {
        return false;
    }
    auto iterBegin = node->children_.begin();
    for (; iterBegin != node->children_.end(); ++iterBegin) {
        if ((*iterBegin)->priority_ >= 0) {
            break;
        }
        if (func(*iterBegin)) {
            return true;
        }
    }
    if (func(node)) {
        return true;
    }
    for (; iterBegin != node->children_.end(); ++iterBegin) {
        if (func(*iterBegin)) {
            return true;
        }
    }
    return false;
}

float WindowNodeContainer::GetVirtualPixelRatio(DisplayId displayId) const
{
    return layoutPolicy_->GetVirtualPixelRatio(displayId);
}

Rect WindowNodeContainer::GetDisplayGroupRect() const
{
    return layoutPolicy_->GetDisplayGroupRect();
}

WMError WindowNodeContainer::SetWindowMode(sptr<WindowNode>& node, WindowMode dstMode)
{
    if (node == nullptr) {
        WLOGFE("could not find window");
        return WMError::WM_ERROR_NULLPTR;
    }
    WindowMode srcMode = node->GetWindowMode();
    if (srcMode == dstMode) {
        return WMError::WM_OK;
    }

    if (WindowHelper::IsSplitWindowMode(dstMode) && isScreenLocked_ &&
        (node->GetWindowFlags() & static_cast<uint32_t>(WindowFlag::WINDOW_FLAG_SHOW_WHEN_LOCKED))) {
        return WMError::WM_ERROR_INVALID_PARAM;
    }

    WMError res = WMError::WM_OK;
    if ((srcMode == WindowMode::WINDOW_MODE_FULLSCREEN) && (dstMode == WindowMode::WINDOW_MODE_FLOATING)) {
        node->SetWindowSizeChangeReason(WindowSizeChangeReason::RECOVER);
    } else if (dstMode == WindowMode::WINDOW_MODE_FULLSCREEN) {
        node->SetWindowSizeChangeReason(WindowSizeChangeReason::MAXIMIZE);
        if (srcMode == WindowMode::WINDOW_MODE_FLOATING) {
            node->SetRequestRect(node->GetWindowRect());
        }
    } else {
        node->SetWindowSizeChangeReason(WindowSizeChangeReason::RESIZE);
    }
    node->SetWindowMode(dstMode);
    auto windowPair = displayGroupController_->GetWindowPairByDisplayId(node->GetDisplayId());
    if (windowPair == nullptr) {
        WLOGFE("Window pair is nullptr");
        return WMError::WM_ERROR_NULLPTR;
    }
    windowPair->UpdateIfSplitRelated(node);
    if (WindowHelper::IsMainWindow(node->GetWindowType())) {
        if (WindowHelper::IsFloatingWindow(node->GetWindowMode())) {
            NotifyDockWindowStateChanged(node, true);
        } else {
            NotifyDockWindowStateChanged(node, false);
        }
    }

    if (node->GetWindowMode() == WindowMode::WINDOW_MODE_FULLSCREEN &&
        WindowHelper::IsAppWindow(node->GetWindowType())) {
        // minimize other app window
        res = MinimizeStructuredAppWindowsExceptSelf(node);
        if (res != WMError::WM_OK) {
            return res;
        }
    }
    if (node->GetWindowToken() != nullptr) {
        node->GetWindowToken()->UpdateWindowMode(node->GetWindowMode());
    }
    res = UpdateWindowNode(node, WindowUpdateReason::UPDATE_MODE);
    if (res != WMError::WM_OK) {
        WLOGFE("Set window mode failed, update node failed");
        return res;
    }
    return WMError::WM_OK;
}

void WindowNodeContainer::GetModeChangeHotZones(DisplayId displayId, ModeChangeHotZones& hotZones,
    const ModeChangeHotZonesConfig& config)
{
    const auto& displayRect = displayGroupInfo_->GetDisplayRect(displayId);

    hotZones.fullscreen_.width_ = displayRect.width_;
    hotZones.fullscreen_.height_ = config.fullscreenRange_;

    hotZones.primary_.width_ = config.primaryRange_;
    hotZones.primary_.height_ = displayRect.height_;

    hotZones.secondary_.posX_ = static_cast<int32_t>(displayRect.width_) - config.secondaryRange_;
    hotZones.secondary_.width_ = config.secondaryRange_;
    hotZones.secondary_.height_ = displayRect.height_;
}

float WindowNodeContainer::GetDisplayVirtualPixelRatio(DisplayId displayId) const
{
    return displayGroupInfo_->GetDisplayVirtualPixelRatio(displayId);
}

sptr<DisplayInfo> WindowNodeContainer::GetDisplayInfo(DisplayId displayId)
{
    return displayGroupInfo_->GetDisplayInfo(displayId);
}

void WindowNodeContainer::UpdateDisplayInfo(sptr<DisplayInfo> displayInfo)
{
    displayGroupInfo_->UpdateDisplayInfo(displayInfo);
}

std::vector<sptr<DisplayInfo>> WindowNodeContainer::GetAllDisplayInfo()
{
    return displayGroupInfo_->GetAllDisplayInfo();
}

void WindowNodeContainer::UpdateCameraFloatWindowStatus(const sptr<WindowNode>& node, bool isShowing)
{
    if (node->GetWindowType() == WindowType::WINDOW_TYPE_FLOAT_CAMERA) {
        WindowManagerAgentController::GetInstance().UpdateCameraFloatWindowStatus(node->GetAccessTokenId(), isShowing);
    }
}

WindowLayoutMode WindowNodeContainer::GetCurrentLayoutMode() const
{
    return layoutMode_;
}

void WindowNodeContainer::RemoveSingleUserWindowNodes(int accountId)
{
    std::vector<sptr<WindowNode>> windowNodes;
    TraverseContainer(windowNodes);
    WLOGFD("%{public}d", accountId);
    for (auto& windowNode : windowNodes) {
        int windowAccountId = windowNode->GetCallingUid() / UID_TRANSFROM_DIVISOR;
        if (windowAccountId < UID_MIN || windowAccountId == accountId) {
            WLOGFD("skiped window %{public}s, windowId %{public}d uid %{public}d",
                windowNode->GetWindowName().c_str(), windowNode->GetWindowId(), windowNode->GetCallingUid());
            continue;
        }
        WLOGFD("remove window %{public}s, windowId %{public}d uid %{public}d",
            windowNode->GetWindowName().c_str(), windowNode->GetWindowId(), windowNode->GetCallingUid());
        windowNode->GetWindowProperty()->SetAnimationFlag(static_cast<uint32_t>(WindowAnimation::NONE));
        if (windowNode->GetWindowToken()) {
            if (windowNode->surfaceNode_ != nullptr) {
                windowNode->surfaceNode_->SetVisible(true);
            }
            windowNode->GetWindowToken()->UpdateWindowState(WindowState::STATE_HIDDEN);
        }
    }
}

bool WindowNodeContainer::TakeWindowPairSnapshot(DisplayId displayId)
{
    auto windowPair = displayGroupController_->GetWindowPairByDisplayId(displayId);
    return windowPair == nullptr ? false : windowPair->TakePairSnapshot();
}

void WindowNodeContainer::ClearWindowPairSnapshot(DisplayId displayId)
{
    auto windowPair = displayGroupController_->GetWindowPairByDisplayId(displayId);
    if (windowPair == nullptr) {
        WLOGFE("Window pair is nullptr");
        return;
    }
    windowPair->ClearPairSnapshot();
}

bool WindowNodeContainer::IsScreenLocked()
{
    return isScreenLocked_;
}
} // namespace Rosen
} // namespace OHOS
