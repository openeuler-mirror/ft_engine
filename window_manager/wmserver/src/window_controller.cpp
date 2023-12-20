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

#include "window_controller.h"
#include <ability_manager_client.h>
#include <chrono>
#include <cstdint>
#include <hisysevent.h>
#include <hitrace_meter.h>
#include <parameters.h>
#include <power_mgr_client.h>
#include <rs_window_animation_finished_callback.h>
#include <transaction/rs_transaction.h>
#include <sstream>

#include "display_manager_service_inner.h"
#include "minimize_app.h"
#include "remote_animation.h"
#include "starting_window.h"
#include "window_inner_manager.h"
#include "window_manager_hilog.h"
#include "window_helper.h"
#include "wm_common.h"

namespace OHOS {
namespace Rosen {
namespace {
    constexpr HiviewDFX::HiLogLabel LABEL = {LOG_CORE, HILOG_DOMAIN_WINDOW, "WindowController"};
    constexpr uint32_t TOUCH_HOT_AREA_MAX_NUM = 10;
    constexpr float MASKING_SURFACE_NODE_Z_ORDER = 9999;
}

uint32_t WindowController::GenWindowId()
{
    return ++windowId_;
}

void WindowController::StartingWindow(sptr<WindowTransitionInfo> info, std::shared_ptr<Media::PixelMap> pixelMap,
    uint32_t bkgColor, bool isColdStart)
{
    if (!info || info->GetAbilityToken() == nullptr) {
        WLOGFE("info or AbilityToken is nullptr!");
        return;
    }
    StartAsyncTraceArgs(HITRACE_TAG_WINDOW_MANAGER, static_cast<int32_t>(TraceTaskId::STARTING_WINDOW),
        "wms:async:ShowStartingWindow");
    auto node = windowRoot_->FindWindowNodeWithToken(info->GetAbilityToken());
    if (node == nullptr) {
        if (!isColdStart) {
            WLOGFE("no windowNode exists but is hot start!");
            return;
        }
        node = StartingWindow::CreateWindowNode(info, GenWindowId());
        if (node == nullptr) {
            return;
        }
        if (windowRoot_->SaveWindow(node) != WMError::WM_OK) {
            return;
        }
        if (!RemoteAnimation::CheckAnimationController()) {
            UpdateWindowAnimation(node);
        }
    } else {
        if (node->stateMachine_.IsWindowNodeShownOrShowing()) {
            WLOGFD("current window is visible, windowId:%{public}u state:%{public}u!",
                node->GetWindowId(), static_cast<uint32_t>(node->stateMachine_.GetCurrentState()));
            return;
        }
        if (WindowHelper::IsValidWindowMode(info->GetWindowMode()) &&
            (node->GetWindowMode() != info->GetWindowMode())) {
            WLOGFW("set starting window mode. starting mode is: %{public}u, window mode is:%{public}u.",
                node->GetWindowMode(), info->GetWindowMode());
            node->SetWindowMode(info->GetWindowMode());
        }
    }

    if (!WindowHelper::CheckSupportWindowMode(node->GetWindowMode(), node->GetModeSupportInfo(), info)) {
        WLOGFE("need to cancel starting window");
        return;
    }

    if (windowRoot_->AddWindowNode(0, node, true) != WMError::WM_OK) {
        return;
    }
    StartingWindow::DrawStartingWindow(node, pixelMap, bkgColor, isColdStart);
    FlushWindowInfo(node->GetWindowId());
    node->startingWindowShown_ = true;
    WLOGFD("StartingWindow show success with id:%{public}u!", node->GetWindowId());
}

void WindowController::CancelStartingWindow(sptr<IRemoteObject> abilityToken)
{
    auto node = windowRoot_->FindWindowNodeWithToken(abilityToken);
    if (node == nullptr) {
        WLOGFD("cannot find windowNode!");
        return;
    }
    if (!node->startingWindowShown_) {
        WLOGFE("CancelStartingWindow failed because client window has shown id:%{public}u", node->GetWindowId());
        return;
    }
    HITRACE_METER_FMT(HITRACE_TAG_WINDOW_MANAGER, "wms:CancelStartingWindow(%u)", node->GetWindowId());
    FinishAsyncTraceArgs(HITRACE_TAG_WINDOW_MANAGER, static_cast<int32_t>(TraceTaskId::STARTING_WINDOW),
        "wms:async:ShowStartingWindow");
    WLOGFD("CancelStartingWindow with id:%{public}u!", node->GetWindowId());
    node->isAppCrash_ = true;
    WMError res = DestroyWindow(node->GetWindowId(), false);
    if (res != WMError::WM_OK) {
        WLOGFE("DestroyWindow failed!");
    }
}

WMError WindowController::NotifyWindowTransition(sptr<WindowTransitionInfo>& srcInfo,
    sptr<WindowTransitionInfo>& dstInfo)
{
    WLOGFD("NotifyWindowTransition begin!");
    sptr<WindowNode> dstNode = nullptr;
    sptr<WindowNode> srcNode = nullptr;
    if (srcInfo) {
        srcNode = windowRoot_->FindWindowNodeWithToken(srcInfo->GetAbilityToken());
    }
    if (dstInfo) {
        dstNode = windowRoot_->FindWindowNodeWithToken(dstInfo->GetAbilityToken());
    }
    if (!RemoteAnimation::CheckTransition(srcInfo, srcNode, dstInfo, dstNode)) {
        return WMError::WM_ERROR_NO_REMOTE_ANIMATION;
    }
    StartAsyncTraceArgs(HITRACE_TAG_WINDOW_MANAGER, static_cast<int32_t>(TraceTaskId::REMOTE_ANIMATION),
        "wms:async:ShowRemoteAnimation");
    auto transitionEvent = RemoteAnimation::GetTransitionEvent(srcInfo, dstInfo, srcNode, dstNode);
    switch (transitionEvent) {
        case TransitionEvent::APP_TRANSITION: {
            return RemoteAnimation::NotifyAnimationTransition(srcInfo, dstInfo, srcNode, dstNode);
        }
        case TransitionEvent::MINIMIZE:
            return RemoteAnimation::NotifyAnimationMinimize(srcInfo, srcNode);
        case TransitionEvent::CLOSE:
            return RemoteAnimation::NotifyAnimationClose(srcInfo, srcNode, TransitionEvent::CLOSE);
        case TransitionEvent::BACK_TRANSITION:
            return RemoteAnimation::NotifyAnimationBackTransition(srcInfo, dstInfo, srcNode, dstNode);
        default:
            return WMError::WM_ERROR_NO_REMOTE_ANIMATION;
    }
    return WMError::WM_OK;
}

WMError WindowController::GetFocusWindowNode(DisplayId displayId, sptr<WindowNode>& windowNode)
{
    auto windowNodeContainer = windowRoot_->GetOrCreateWindowNodeContainer(displayId);
    if (windowNodeContainer == nullptr) {
        WLOGFE("windowNodeContainer is null, displayId: %{public}" PRIu64"", displayId);
        return WMError::WM_ERROR_NULLPTR;
    }
    uint32_t focusWindowId = windowNodeContainer->GetFocusWindow();
    WLOGFD("focusWindowId: %{public}u", focusWindowId);
    auto thisWindowNode = windowRoot_->GetWindowNode(focusWindowId);
    if (thisWindowNode == nullptr || !thisWindowNode->currentVisibility_) {
        WLOGFE("focusWindowNode is null or invisible, focusWindowId: %{public}u", focusWindowId);
        return WMError::WM_ERROR_INVALID_WINDOW;
    }
    windowNode = thisWindowNode;
    return WMError::WM_OK;
}

WMError WindowController::GetFocusWindowInfo(sptr<IRemoteObject>& abilityToken)
{
    DisplayId displayId = DisplayManagerServiceInner::GetInstance().GetDefaultDisplayId();
    sptr<WindowNode> windowNode;
    WMError res = GetFocusWindowNode(displayId, windowNode);
    if (res == WMError::WM_OK) {
        abilityToken = windowNode->abilityToken_;
    }
    return res;
}

WMError WindowController::CreateWindow(sptr<IWindow>& window, sptr<WindowProperty>& property,
    const std::shared_ptr<RSSurfaceNode>& surfaceNode, uint32_t& windowId, sptr<IRemoteObject> token,
    int32_t pid, int32_t uid)
{
    uint32_t parentId = property->GetParentId();
    if ((parentId != INVALID_WINDOW_ID) &&
        !WindowHelper::IsSubWindow(property->GetWindowType()) &&
        !WindowHelper::IsSystemSubWindow(property->GetWindowType())) {
        WLOGFE("create window failed, type is error");
        return WMError::WM_ERROR_INVALID_TYPE;
    }

    if (windowRoot_->CheckMultiDialogWindows(property->GetWindowType(), token)) {
        return WMError::WM_ERROR_INVALID_WINDOW;
    }

    sptr<WindowNode> node = windowRoot_->FindWindowNodeWithToken(token);
    if (node != nullptr && WindowHelper::IsMainWindow(property->GetWindowType()) && node->startingWindowShown_) {
        StartingWindow::HandleClientWindowCreate(node, window, windowId, surfaceNode, property, pid, uid);
        windowRoot_->AddDeathRecipient(node);
        windowRoot_->AddSurfaceNodeIdWindowNodePair(surfaceNode->GetId(), node);
        return WMError::WM_OK;
    }

    windowId = GenWindowId();
    sptr<WindowProperty> windowProperty = new WindowProperty(property);
    windowProperty->SetWindowId(windowId);
    node = new WindowNode(windowProperty, window, surfaceNode, pid, uid);
    node->abilityToken_ = token;
    node->dialogTargetToken_ = token;
    UpdateWindowAnimation(node);
    WLOGFD("createWindow name:%{public}u, windowName:%{public}s",
        windowId, node->GetWindowName().c_str());
    // test
    node->stateMachine_.SetWindowId(windowId);
    node->stateMachine_.SetWindowType(property->GetWindowType());
    return windowRoot_->SaveWindow(node);
}

void WindowController::NotifyAfterAddWindow(sptr<WindowNode>& node)
{
    std::vector<sptr<WindowNode>> nodes;
    nodes.emplace_back(node);
    for (auto& child : node->children_) {
        if (child->currentVisibility_) {
            nodes.emplace_back(child);
        }
    }
    for (auto& iter : nodes) {
        if ((iter->GetWindowType() == WindowType::WINDOW_TYPE_DIALOG) &&
            (node->abilityToken_ != iter->abilityToken_)) {
            iter->GetWindowToken()->NotifyForeground();
        }
    }
    accessibilityConnection_->NotifyAccessibilityWindowInfo(node->GetDisplayId(), nodes,
        WindowUpdateType::WINDOW_UPDATE_ADDED);
}

WMError WindowController::AddWindowNode(sptr<WindowProperty>& property)
{
    auto node = windowRoot_->GetWindowNode(property->GetWindowId());
    if (node == nullptr) {
        WLOGFE("could not find window");
        return WMError::WM_ERROR_NULLPTR;
    }

    if (node->currentVisibility_ && !node->startingWindowShown_) {
        WLOGFE("current window is visible, windowId: %{public}u", node->GetWindowId());
        return WMError::WM_ERROR_INVALID_OPERATION;
    }

    // using starting window rect if client rect is empty
    if (WindowHelper::IsEmptyRect(property->GetRequestRect()) && node->startingWindowShown_) { // for tile and cascade
        property->SetRequestRect(node->GetRequestRect());
        property->SetWindowRect(node->GetWindowRect());
        property->SetDecoStatus(true);
    }
    node->GetWindowProperty()->CopyFrom(property);
    // Need 'check permission'
    // Need 'adjust property'
    UpdateWindowAnimation(node);
    WMError res = windowRoot_->AddWindowNode(property->GetParentId(), node);
    if (res != WMError::WM_OK) {
        MinimizeApp::ClearNodesWithReason(MinimizeReason::OTHER_WINDOW);
        return res;
    }
    windowRoot_->FocusFaultDetection();
    RelayoutKeyboard(node);
    FlushWindowInfo(property->GetWindowId());
    NotifyAfterAddWindow(node);
    HandleTurnScreenOn(node);

    if (WindowHelper::IsSystemBarWindow(node->GetWindowType())) {
        sysBarWinId_[node->GetWindowType()] = node->GetWindowId();
    }
    if (node->GetWindowType() == WindowType::WINDOW_TYPE_INPUT_METHOD_FLOAT) {
        ResizeSoftInputCallingWindowIfNeed(node);
    }
    StopBootAnimationIfNeed(node);
    // when hide with remote animation first and show with default animation, need transform state
    // minimize should execute in finish callback when remote animation enabled
    if (!node->stateMachine_.IsShowAnimationPlaying()) {
        if (WindowHelper::IsMainWindow(node->GetWindowType())) {
            MinimizeApp::ExecuteMinimizeAll();
            WLOGFD("id:%{public}u execute minimize all", node->GetWindowId());
        }
        node->stateMachine_.TransitionTo(WindowNodeState::SHOWN); // for normal show which not use remote animation
    } else if (WindowHelper::IsMainWindow(node->GetWindowType())) {
        MinimizeApp::ExecuteMinimizeTargetReasons(~MinimizeReason::OTHER_WINDOW);
    }
    return WMError::WM_OK;
}

void WindowController::RelayoutKeyboard(const sptr<WindowNode>& node)
{
    if (node == nullptr) {
        WLOGFE("could not find window");
        return;
    }
    if (node->GetWindowType() != WindowType::WINDOW_TYPE_INPUT_METHOD_FLOAT) {
        return;
    }

    auto container = windowRoot_->GetOrCreateWindowNodeContainer(node->GetDisplayId());
    if (container == nullptr) {
        WLOGFE("window node container is null");
        return;
    }

    uint32_t navigationBarHeight = 0;
    bool hasFullScreenKeyGuardWindow = false;
    WindowNodeOperationFunc func = [&navigationBarHeight, &hasFullScreenKeyGuardWindow](sptr<WindowNode> windowNode) {
        if (windowNode->GetWindowType() == WindowType::WINDOW_TYPE_KEYGUARD &&
            windowNode->GetWindowMode() == WindowMode::WINDOW_MODE_FULLSCREEN) {
                hasFullScreenKeyGuardWindow = true;
        }
        if (windowNode->GetWindowType() == WindowType::WINDOW_TYPE_NAVIGATION_BAR && windowNode->isVisible_) {
            navigationBarHeight = windowNode->GetWindowRect().height_;
            if (hasFullScreenKeyGuardWindow) {
                WLOGFW("RelayoutKeyboard: The navigation bar is overlaid by the keyguard window and is invisible");
                navigationBarHeight = 0;
            }
            return true;
        }
        return false;
    };
    container->TraverseWindowTree(func, true); // FromTopToBottom

    sptr<DisplayInfo> defaultDisplayInfo = DisplayManagerServiceInner::GetInstance().GetDefaultDisplay();
    if (defaultDisplayInfo == nullptr) {
        WLOGFE("defaultDisplayInfo is null");
        return;
    }

    auto previousRect = node->GetWindowRect();
    WLOGFD("relayout keyboard window with navigationBarHeight: %{public}u", navigationBarHeight);
    Rect requestedRect = { previousRect.posX_,
        static_cast<int32_t>(defaultDisplayInfo->GetHeight() - previousRect.height_ - navigationBarHeight),
        previousRect.width_, previousRect.height_ };
    ResizeRect(node->GetWindowId(), requestedRect, WindowSizeChangeReason::MOVE);
}

void WindowController::ResizeSoftInputCallingWindowIfNeed(const sptr<WindowNode>& node)
{
    auto callingWindowId = node->GetCallingWindow();
    auto callingWindow = windowRoot_->GetWindowNode(callingWindowId);
    if (callingWindow == nullptr) {
        auto windowNodeContainer = windowRoot_->GetOrCreateWindowNodeContainer(node->GetDisplayId());
        if (windowNodeContainer == nullptr) {
            WLOGFE("windowNodeContainer is null, displayId:%{public}" PRIu64"", node->GetDisplayId());
            return;
        }
        callingWindowId = windowNodeContainer->GetFocusWindow();
        callingWindow = windowRoot_->GetWindowNode(callingWindowId);
    }
    if (callingWindow == nullptr || !callingWindow->currentVisibility_ ||
        callingWindow->GetWindowMode() != WindowMode::WINDOW_MODE_FLOATING) {
        WLOGFE("callingWindow is null or invisible or not float window, callingWindowId:%{public}u", callingWindowId);
        return;
    }
    Rect softInputWindowRect = node->GetWindowRect();
    Rect callingWindowRect = callingWindow->GetWindowRect();
    Rect rect = WindowHelper::GetOverlap(softInputWindowRect, callingWindowRect, 0, 0);
    if (WindowHelper::IsEmptyRect(rect)) {
        WLOGFE("there is no overlap");
        return;
    }
    Rect requestedRect = callingWindowRect;
    requestedRect.posY_ = softInputWindowRect.posY_ - static_cast<int32_t>(requestedRect.height_);
    Rect statusBarWindowRect = { 0, 0, 0, 0 };
    auto statusbarWindow = windowRoot_->GetWindowNode(sysBarWinId_[WindowType::WINDOW_TYPE_STATUS_BAR]);
    if (statusbarWindow != nullptr && statusbarWindow->parent_ != nullptr) {
        statusBarWindowRect = statusbarWindow->GetWindowRect();
    }
    int32_t posY = std::max(requestedRect.posY_, static_cast<int32_t>(statusBarWindowRect.height_));
    if (posY != requestedRect.posY_) {
        requestedRect.height_ = static_cast<uint32_t>(softInputWindowRect.posY_ - posY);
        requestedRect.posY_ = posY;
    }
    callingWindowRestoringRect_ = callingWindowRect;
    callingWindowId_ = callingWindow->GetWindowId();
    ResizeRectAndFlush(callingWindowId_, requestedRect, WindowSizeChangeReason::DRAG);
}

void WindowController::RestoreCallingWindowSizeIfNeed()
{
    auto callingWindow = windowRoot_->GetWindowNode(callingWindowId_);
    if (!WindowHelper::IsEmptyRect(callingWindowRestoringRect_) && callingWindow != nullptr &&
        callingWindow->GetWindowMode() == WindowMode::WINDOW_MODE_FLOATING) {
        ResizeRectAndFlush(callingWindowId_, callingWindowRestoringRect_, WindowSizeChangeReason::DRAG);
    }
    callingWindowRestoringRect_ = { 0, 0, 0, 0 };
    callingWindowId_ = 0u;
}

void WindowController::HandleTurnScreenOn(const sptr<WindowNode>& node)
{
    if (node == nullptr) {
        WLOGFE("window is invalid");
        return;
    }
    WLOGFD("handle turn screen on: [%{public}s, %{public}d]", node->GetWindowName().c_str(), node->IsTurnScreenOn());
    // reset ipc identity
    std::string identity = IPCSkeleton::ResetCallingIdentity();
    if (node->IsTurnScreenOn() && !PowerMgr::PowerMgrClient::GetInstance().IsScreenOn()) {
        WLOGFD("turn screen on");
        PowerMgr::PowerMgrClient::GetInstance().WakeupDevice();
    }
    // set ipc identity to raw
    IPCSkeleton::SetCallingIdentity(identity);
}

WMError WindowController::RemoveWindowNode(uint32_t windowId, bool fromAnimation)
{
    auto windowNode = windowRoot_->GetWindowNode(windowId);
    if (windowNode == nullptr) {
        WLOGFE("windowNode is nullptr");
        return WMError::WM_ERROR_NULLPTR;
    }
    auto removeFunc = [this, windowId, windowNode, fromAnimation]() {
        WMError res = windowRoot_->RemoveWindowNode(windowId, fromAnimation);
        if (res != WMError::WM_OK) {
            WLOGFE("RemoveWindowNode failed");
            return res;
        }
        windowRoot_->FocusFaultDetection();
        FlushWindowInfo(windowId);
        std::vector<sptr<WindowNode>> nodes;
        nodes.emplace_back(windowNode);
        for (auto& child : windowNode->children_) {
            nodes.emplace_back(child);
        }
        for (auto& iter : nodes) {
            if ((iter->GetWindowType() == WindowType::WINDOW_TYPE_DIALOG) &&
                (windowNode->abilityToken_ != iter->abilityToken_)) {
                iter->GetWindowToken()->NotifyBackground();
            }
        }
        displayZoomController_->ClearZoomTransform(nodes);
        accessibilityConnection_->NotifyAccessibilityWindowInfo(windowNode->GetDisplayId(), nodes,
            WindowUpdateType::WINDOW_UPDATE_REMOVED);
        return res;
    };
    WMError res = WMError::WM_ERROR_NO_REMOTE_ANIMATION;
    if (windowNode->GetWindowType() == WindowType::WINDOW_TYPE_KEYGUARD) {
        if (RemoteAnimation::NotifyAnimationScreenUnlock(removeFunc) == WMError::WM_OK) {
            WLOGFD("NotifyAnimationScreenUnlock with remote animation");
            res = WMError::WM_OK;
        }
    }
    if (res != WMError::WM_OK) {
        res = removeFunc();
    }
    if (windowNode->GetWindowType() == WindowType::WINDOW_TYPE_INPUT_METHOD_FLOAT) {
        RestoreCallingWindowSizeIfNeed();
    }
    if (!windowNode->stateMachine_.IsHideAnimationPlaying()) {
        windowNode->stateMachine_.TransitionTo(WindowNodeState::HIDDEN);
    }
    return res;
}

WMError WindowController::DestroyWindow(uint32_t windowId, bool onlySelf)
{
    DisplayId displayId = DISPLAY_ID_INVALID;
    auto node = windowRoot_->GetWindowNode(windowId);
    if (node == nullptr) {
        WLOGFE("destroy window id:%{public}u failed, because window node is not exist.", windowId);
        return WMError::WM_ERROR_NULLPTR;
    }
    displayId = node->GetDisplayId();
    WMError res = windowRoot_->DestroyWindow(windowId, onlySelf);
    if (res != WMError::WM_OK) {
        return res;
    }
    windowRoot_->FocusFaultDetection();
    FlushWindowInfoWithDisplayId(displayId);
    std::vector<sptr<WindowNode>> nodes;
    nodes.emplace_back(node);
    for (auto& child : node->children_) {
        nodes.emplace_back(child);
    }
    accessibilityConnection_->NotifyAccessibilityWindowInfo(node->GetDisplayId(), nodes,
        WindowUpdateType::WINDOW_UPDATE_REMOVED);
    node->stateMachine_.TransitionTo(WindowNodeState::DESTROYED);
    return res;
}

WMError WindowController::ResizeRect(uint32_t windowId, const Rect& rect, WindowSizeChangeReason reason)
{
    auto node = windowRoot_->GetWindowNode(windowId);
    if (node == nullptr) {
        WLOGFE("could not find window");
        return WMError::WM_ERROR_NULLPTR;
    }
    if (node->GetWindowMode() != WindowMode::WINDOW_MODE_FLOATING) {
        WLOGFE("fullscreen window could not resize");
        return WMError::WM_ERROR_INVALID_OPERATION;
    }
    auto property = node->GetWindowProperty();
    node->SetWindowSizeChangeReason(reason);
    Rect lastRect = property->GetWindowRect();
    Rect newRect;
    if (reason == WindowSizeChangeReason::MOVE) {
        newRect = { rect.posX_, rect.posY_, lastRect.width_, lastRect.height_ };
        if (node->GetWindowType() == WindowType::WINDOW_TYPE_DOCK_SLICE) {
            if (windowRoot_->IsForbidDockSliceMove(node->GetDisplayId())) {
                WLOGFD("dock slice is forbidden to move");
                newRect = lastRect;
            } else if (windowRoot_->IsVerticalDisplay(node)) {
                newRect.posX_ = lastRect.posX_;
            } else {
                newRect.posY_ = lastRect.posY_;
            }
        }
    } else if (reason == WindowSizeChangeReason::RESIZE) {
        newRect = { lastRect.posX_, lastRect.posY_, rect.width_, rect.height_ };
    } else if (reason == WindowSizeChangeReason::DRAG || reason == WindowSizeChangeReason::ROTATION) {
        newRect = rect;
    }
    property->SetRequestRect(newRect);
    WMError res = windowRoot_->UpdateWindowNode(windowId, WindowUpdateReason::UPDATE_RECT);
    if (res != WMError::WM_OK) {
        return res;
    }
    accessibilityConnection_->NotifyAccessibilityWindowInfo(node, WindowUpdateType::WINDOW_UPDATE_PROPERTY);
    return WMError::WM_OK;
}

WMError WindowController::ResizeRectAndFlush(uint32_t windowId, const Rect& rect, WindowSizeChangeReason reason)
{
    WMError res = ResizeRect(windowId, rect, reason);
    if (res != WMError::WM_OK) {
        return res;
    } else {
        FlushWindowInfo(windowId);
        return WMError::WM_OK;
    }
}

WMError WindowController::RequestFocus(uint32_t windowId)
{
    if (windowRoot_ == nullptr) {
        return WMError::WM_ERROR_NULLPTR;
    }
    WMError res = windowRoot_->RequestFocus(windowId);
    FlushWindowInfo(windowId);
    accessibilityConnection_->NotifyAccessibilityWindowInfo(windowRoot_->GetWindowNode(windowId),
        WindowUpdateType::WINDOW_UPDATE_FOCUSED);
    return res;
}

WMError WindowController::SetWindowMode(uint32_t windowId, WindowMode dstMode)
{
    HITRACE_METER(HITRACE_TAG_WINDOW_MANAGER);
    auto node = windowRoot_->GetWindowNode(windowId);
    if (node == nullptr) {
        WLOGFE("could not find window");
        return WMError::WM_ERROR_NULLPTR;
    }
    WMError ret = windowRoot_->SetWindowMode(node, dstMode);
    if (ret != WMError::WM_OK) {
        return ret;
    }
    FlushWindowInfo(windowId);
    accessibilityConnection_->NotifyAccessibilityWindowInfo(node, WindowUpdateType::WINDOW_UPDATE_PROPERTY);
    if (!node->stateMachine_.IsShowAnimationPlaying()) {
        if (WindowHelper::IsMainWindow(node->GetWindowType())) {
            MinimizeApp::ExecuteMinimizeAll();
            WLOGFD("id:%{public}u execute minimize all", node->GetWindowId());
        }
    }
    return WMError::WM_OK;
}

void WindowController::NotifyDisplayStateChange(DisplayId defaultDisplayId, sptr<DisplayInfo> displayInfo,
    const std::map<DisplayId, sptr<DisplayInfo>>& displayInfoMap, DisplayStateChangeType type)
{
    WLOGFD("NotifyDisplayStateChange start: %{public}u", type);
    switch (type) {
        case DisplayStateChangeType::BEFORE_SUSPEND: {
            isScreenLocked_ = true;
            windowRoot_->ProcessWindowStateChange(WindowState::STATE_FROZEN, WindowStateChangeReason::KEYGUARD);
            break;
        }
        case DisplayStateChangeType::BEFORE_UNLOCK: {
            windowRoot_->ProcessWindowStateChange(WindowState::STATE_UNFROZEN, WindowStateChangeReason::KEYGUARD);
            isScreenLocked_ = false;
            break;
        }
        case DisplayStateChangeType::CREATE: {
            SetDefaultDisplayInfo(defaultDisplayId, displayInfo);
            windowRoot_->ProcessDisplayCreate(defaultDisplayId, displayInfo, displayInfoMap);
            FlushWindowInfoWithDisplayId(displayInfo->GetDisplayId());
            break;
        }
        case DisplayStateChangeType::DESTROY: {
            windowRoot_->ProcessDisplayDestroy(defaultDisplayId, displayInfo, displayInfoMap);
            FlushWindowInfoWithDisplayId(defaultDisplayId);
            break;
        }
        case DisplayStateChangeType::DISPLAY_COMPRESS:
        case DisplayStateChangeType::SIZE_CHANGE:
        case DisplayStateChangeType::UPDATE_ROTATION:
        case DisplayStateChangeType::VIRTUAL_PIXEL_RATIO_CHANGE: {
            ProcessDisplayChange(defaultDisplayId, displayInfo, displayInfoMap, type);
            /*
             * Window tile num may change when display rotate or change size, need to execute minimize
             */
            MinimizeApp::ExecuteMinimizeTargetReasons(MinimizeReason::LAYOUT_TILE);
            break;
        }
        default: {
            WLOGFE("unknown DisplayStateChangeType:%{public}u", type);
            return;
        }
    }
    WLOGFD("NotifyDisplayStateChange end, type: %{public}u", type);
}

void WindowController::SetDefaultDisplayInfo(DisplayId defaultDisplayId, sptr<DisplayInfo> displayInfo)
{
    if (displayInfo == nullptr) {
        WLOGFE("display is null");
        return;
    }
    if (displayInfo->GetDisplayId() != defaultDisplayId) {
        return;
    }
    WLOGFD("get default display info");
    auto displayWidth = static_cast<uint32_t>(displayInfo->GetWidth());
    auto displayHeight = static_cast<uint32_t>(displayInfo->GetHeight());
    defaultDisplayRect_ = { 0, 0, displayWidth, displayHeight };
}

void WindowController::ProcessSystemBarChange(const sptr<DisplayInfo>& displayInfo)
{
    DisplayId displayId = displayInfo->GetDisplayId();
    auto width = static_cast<uint32_t>(displayInfo->GetWidth());
    auto height = static_cast<uint32_t>(displayInfo->GetHeight());
    const auto& statusBarNode = windowRoot_->GetWindowNode(sysBarWinId_[WindowType::WINDOW_TYPE_STATUS_BAR]);
    if (statusBarNode != nullptr && statusBarNode->GetDisplayId() == displayId) {
        auto statusBarHeight = statusBarNode->GetWindowRect().height_;
        Rect newRect = { 0, 0, width, statusBarHeight };
        ResizeRect(sysBarWinId_[WindowType::WINDOW_TYPE_STATUS_BAR], newRect, WindowSizeChangeReason::ROTATION);
    }
    const auto& naviBarNode = windowRoot_->GetWindowNode(sysBarWinId_[WindowType::WINDOW_TYPE_NAVIGATION_BAR]);
    if (naviBarNode != nullptr && naviBarNode->GetDisplayId() == displayId) {
        auto naviBarHeight = naviBarNode->GetWindowRect().height_;
        Rect newRect = { 0, static_cast<int32_t>(height - naviBarHeight), width, naviBarHeight };
        ResizeRect(sysBarWinId_[WindowType::WINDOW_TYPE_NAVIGATION_BAR], newRect, WindowSizeChangeReason::ROTATION);
    }
}

void WindowController::ProcessDisplayChange(DisplayId defaultDisplayId, sptr<DisplayInfo> displayInfo,
    const std::map<DisplayId, sptr<DisplayInfo>>& displayInfoMap, DisplayStateChangeType type)
{
    if (displayInfo == nullptr) {
        WLOGFE("get display failed");
        return;
    }
    auto windowNodeContainer = windowRoot_->GetOrCreateWindowNodeContainer(displayInfo->GetDisplayId());
    if (windowNodeContainer != nullptr) {
        windowNodeContainer->BeforeProcessWindowAvoidAreaChangeWhenDisplayChange();
        windowNodeContainer->UpdateDisplayInfo(displayInfo);
    }
    switch (type) {
        case DisplayStateChangeType::DISPLAY_COMPRESS:
            ProcessDisplayCompression(defaultDisplayId, displayInfo);
            [[fallthrough]];
        case DisplayStateChangeType::SIZE_CHANGE:
        case DisplayStateChangeType::UPDATE_ROTATION:
            windowRoot_->ProcessDisplayChange(defaultDisplayId, displayInfo, displayInfoMap, type);
            ProcessSystemBarChange(displayInfo);
            break;
        case DisplayStateChangeType::VIRTUAL_PIXEL_RATIO_CHANGE: {
            windowRoot_->ProcessDisplayChange(defaultDisplayId, displayInfo, displayInfoMap, type);
            break;
        }
        default: {
            WLOGFE("unknown DisplayStateChangeType:%{public}u", type);
            return;
        }
    }
    auto displayId = displayInfo->GetDisplayId();
    displayZoomController_->UpdateAllWindowsZoomInfo(displayId);
    FlushWindowInfoWithDisplayId(displayId);
    accessibilityConnection_->NotifyAccessibilityWindowInfo(displayId, WindowUpdateType::WINDOW_UPDATE_PROPERTY);
    if (windowNodeContainer != nullptr) {
        windowNodeContainer->ProcessWindowAvoidAreaChangeWhenDisplayChange();
    }
}

void WindowController::ProcessDisplayCompression(DisplayId defaultDisplayId, const sptr<DisplayInfo>& displayInfo)
{
    WLOGFD("Enter processDisplayCompress");
    DisplayId displayId = displayInfo->GetDisplayId();
    if (displayId != defaultDisplayId) {
        WLOGFD("Not default display");
        return;
    }
    auto& dms = DisplayManagerServiceInner::GetInstance();
    if (!displayInfo->GetWaterfallDisplayCompressionStatus()) {
        if (maskingSurfaceNode_ == nullptr) {
            WLOGFD("MaskingSurfaceNode is not created");
            return;
        } else {
            WLOGFD("Remove maskingSurfaceNode");
            dms.UpdateRSTree(displayId, displayId, maskingSurfaceNode_, false, false);
            maskingSurfaceNode_ = nullptr;
            return;
        }
    }
    WLOGFD("Add maskingSurfaceNode");
    struct RSSurfaceNodeConfig rsSurfaceNodeConfig;
    rsSurfaceNodeConfig.SurfaceNodeName = "maskingSurface";
    maskingSurfaceNode_ = RSSurfaceNode::Create(rsSurfaceNodeConfig);
    if (maskingSurfaceNode_ == nullptr) {
        WLOGFE("Create maskingSurfaceNode failed");
        return;
    }
    auto displayWidth = displayInfo->GetWidth();
    auto displayHeight = displayInfo->GetHeight();
    auto maskingSizeX = displayInfo->GetOffsetX();
    auto maskingSizeY = displayInfo->GetOffsetY();
    auto fullDisplayWidth = displayWidth + maskingSizeX * 2; // *2: Get full width.
    auto fullDisplayHeight = displayHeight + maskingSizeY * 2; // *2: Get full height.

    Rect screenRect = Rect {0, 0, fullDisplayWidth, fullDisplayHeight};
    Rect transparentRect = Rect {maskingSizeX, maskingSizeY, displayWidth, displayHeight};
    WLOGFD("ScreenRect: fullDisplayWidth: %{public}d, fullDisplayHeight: %{public}d",
        fullDisplayWidth, fullDisplayHeight);
    WLOGFD("TransparentRect: X: %{public}u, Y: %{public}u, Width: %{public}d, Height: %{public}d",
        maskingSizeX, maskingSizeY, displayWidth, displayHeight);

    maskingSurfaceNode_->SetPositionZ(MASKING_SURFACE_NODE_Z_ORDER);

    if (!SurfaceDraw::DrawMasking(maskingSurfaceNode_, screenRect, transparentRect)) {
        WLOGFE("Draw masking surface failed");
        return;
    }
    maskingSurfaceNode_->SetBounds(0, 0, fullDisplayWidth, fullDisplayHeight);
    dms.UpdateRSTree(displayId, displayId, maskingSurfaceNode_, true, false);
}

void WindowController::StopBootAnimationIfNeed(const sptr<WindowNode>& node)
{
    if (isBootAnimationStopped_) {
        return;
    }
    if (node == nullptr) {
        WLOGFE("could not find window");
        return;
    }
    if (node->GetDisplayId() != DisplayManagerServiceInner::GetInstance().GetDefaultDisplayId()) {
        return;
    }
    auto windowNodeContainer = windowRoot_->GetOrCreateWindowNodeContainer(node->GetDisplayId());
    if (windowNodeContainer == nullptr) {
        WLOGFE("window node container is null");
        return;
    }
    std::vector<sptr<WindowNode>> windowNodes;
    windowNodeContainer->TraverseContainer(windowNodes);
    WmOcclusion::Rect defaultDisplayRect = { defaultDisplayRect_.posX_, defaultDisplayRect_.posY_,
        defaultDisplayRect_.posX_ + static_cast<int32_t>(defaultDisplayRect_.width_),
        defaultDisplayRect_.posY_ + static_cast<int32_t>(defaultDisplayRect_.height_)};
    WmOcclusion::Region defaultDisplayRegion(defaultDisplayRect);
    WmOcclusion::Region allRegion; // Counts the area of all shown windows
    for (auto& node : windowNodes) {
        if (node->GetWindowType() == WindowType::WINDOW_TYPE_BOOT_ANIMATION) {
            continue;
        }
        auto windowRect = node->GetWindowRect();
        WmOcclusion::Rect curRect = { windowRect.posX_, windowRect.posY_,
            windowRect.posX_ + static_cast<int32_t>(windowRect.width_),
            windowRect.posY_ + static_cast<int32_t>(windowRect.height_) };
        WmOcclusion::Region curRegion(curRect);
        allRegion = curRegion.Or(allRegion);
        WmOcclusion::Region subResult = defaultDisplayRegion.Sub(allRegion);
        if (subResult.GetSize() == 0) {
            WLOGFD("stop boot animation");
            system::SetParameter("bootevent.wms.fullscreen.ready", "true");
            isBootAnimationStopped_ = true;
            RecordBootAnimationEvent();
        }
    }
}

void WindowController::RecordBootAnimationEvent() const
{
    uint64_t time = static_cast<uint64_t>(std::chrono::time_point_cast<std::chrono::seconds>
        (std::chrono::steady_clock::now()).time_since_epoch().count());
    WLOGFD("boot animation done duration(s): %{public}" PRIu64"", time);
    std::ostringstream os;
    os << "boot animation done duration(s): " << time <<";";
    int32_t ret = OHOS::HiviewDFX::HiSysEvent::Write(
        OHOS::HiviewDFX::HiSysEvent::Domain::WINDOW_MANAGER,
        "WINDOW_BOOT_ANIMATION_DONE",
        OHOS::HiviewDFX::HiSysEvent::EventType::BEHAVIOR,
        "MSG", os.str());
    if (ret != 0) {
        WLOGFE("Write HiSysEvent error, ret:%{public}d", ret);
    }
}

WMError WindowController::SetWindowType(uint32_t windowId, WindowType type)
{
    auto node = windowRoot_->GetWindowNode(windowId);
    if (node == nullptr) {
        WLOGFE("could not find window");
        return WMError::WM_ERROR_NULLPTR;
    }
    auto property = node->GetWindowProperty();
    property->SetWindowType(type);
    UpdateWindowAnimation(node);
    WMError res = windowRoot_->UpdateWindowNode(windowId, WindowUpdateReason::UPDATE_TYPE);
    if (res != WMError::WM_OK) {
        return res;
    }
    FlushWindowInfo(windowId);
    accessibilityConnection_->NotifyAccessibilityWindowInfo(node, WindowUpdateType::WINDOW_UPDATE_PROPERTY);
    WLOGFD("SetWindowType end");
    return res;
}

WMError WindowController::SetWindowFlags(uint32_t windowId, uint32_t flags)
{
    auto node = windowRoot_->GetWindowNode(windowId);
    if (node == nullptr) {
        WLOGFE("could not find window");
        return WMError::WM_ERROR_NULLPTR;
    }
    auto property = node->GetWindowProperty();
    uint32_t oldFlags = property->GetWindowFlags();
    property->SetWindowFlags(flags);
    // only forbid_split_move flag change, just set property
    if ((oldFlags ^ flags) == static_cast<uint32_t>(WindowFlag::WINDOW_FLAG_FORBID_SPLIT_MOVE)) {
        return WMError::WM_OK;
    }
    WMError res = windowRoot_->UpdateWindowNode(windowId, WindowUpdateReason::UPDATE_FLAGS);
    if (res != WMError::WM_OK) {
        return res;
    }
    FlushWindowInfo(windowId);
    accessibilityConnection_->NotifyAccessibilityWindowInfo(node, WindowUpdateType::WINDOW_UPDATE_PROPERTY);
    WLOGFD("SetWindowFlags end");
    return res;
}

WMError WindowController::SetSystemBarProperty(uint32_t windowId, WindowType type, const SystemBarProperty& property)
{
    auto node = windowRoot_->GetWindowNode(windowId);
    if (node == nullptr) {
        WLOGFE("could not find window");
        return WMError::WM_ERROR_NULLPTR;
    }
    node->SetSystemBarProperty(type, property);
    WMError res = windowRoot_->UpdateWindowNode(windowId, WindowUpdateReason::UPDATE_OTHER_PROPS);
    if (res != WMError::WM_OK) {
        return res;
    }
    FlushWindowInfo(windowId);
    accessibilityConnection_->NotifyAccessibilityWindowInfo(node, WindowUpdateType::WINDOW_UPDATE_PROPERTY);
    WLOGFD("SetSystemBarProperty end");
    return res;
}

void WindowController::NotifySystemBarTints()
{
    windowRoot_->NotifySystemBarTints();
}

WMError WindowController::SetWindowAnimationController(const sptr<RSIWindowAnimationController>& controller)
{
    return RemoteAnimation::SetWindowAnimationController(controller);
}

AvoidArea WindowController::GetAvoidAreaByType(uint32_t windowId, AvoidAreaType avoidAreaType) const
{
    return windowRoot_->GetAvoidAreaByType(windowId, avoidAreaType);
}

WMError WindowController::ChangeMouseStyle(uint32_t windowId, sptr<MoveDragProperty>& moveDragProperty)
{
    auto node = windowRoot_->GetWindowNode(windowId);
    if (node->GetWindowType() == WindowType::WINDOW_TYPE_DOCK_SLICE) {
        if (node->GetWindowRect().width_ > node->GetWindowRect().height_) {
            MMI::InputManager::GetInstance()->SetPointerStyle(windowId, MMI::MOUSE_ICON::NORTH_SOUTH);
        } else {
            MMI::InputManager::GetInstance()->SetPointerStyle(windowId, MMI::MOUSE_ICON::WEST_EAST);
        }
        return WMError::WM_OK;
    }
    MMI::InputManager::GetInstance()->SetPointerStyle(windowId, STYLEID_MAP.at(moveDragProperty->dragType_));
    return WMError::WM_OK;
}

WMError WindowController::NotifyServerReadyToMoveOrDrag(uint32_t windowId, sptr<MoveDragProperty>& moveDragProperty)
{
    auto node = windowRoot_->GetWindowNode(windowId);
    if (node == nullptr) {
        WLOGFW("could not find window");
        return WMError::WM_ERROR_NULLPTR;
    }
    if (!node->currentVisibility_) {
        WLOGFE("[NotifyServerReadyToMoveOrDrag] window is not visible, windowId: %{public}u", windowId);
        return WMError::WM_ERROR_INVALID_OPERATION;
    }

    // if start dragging or start moving dock_slice, need to update size change reason
    if ((moveDragProperty->startMoveFlag_ && node->GetWindowType() == WindowType::WINDOW_TYPE_DOCK_SLICE) ||
        moveDragProperty->startDragFlag_) {
        WMError res = windowRoot_->UpdateSizeChangeReason(windowId, WindowSizeChangeReason::DRAG_START);
        ChangeMouseStyle(windowId, moveDragProperty);
        return res;
    }
    return WMError::WM_OK;
}

WMError WindowController::ProcessPointDown(uint32_t windowId, bool isPointDown)
{
    auto node = windowRoot_->GetWindowNode(windowId);
    if (node == nullptr) {
        WLOGFW("could not find window");
        return WMError::WM_ERROR_NULLPTR;
    }
    if (!node->currentVisibility_) {
        WLOGFE("[ProcessPointDown] window is not visible, windowId: %{public}u", windowId);
        return WMError::WM_ERROR_INVALID_OPERATION;
    }

    /*
     * If not point down, no need to notify touch outside
     */
    if (isPointDown) {
        NotifyTouchOutside(node);
        if (node->GetWindowType() == WindowType::WINDOW_TYPE_DOCK_SLICE) {
            windowRoot_->TakeWindowPairSnapshot(node->GetDisplayId());
        }
    }

    WLOGFD("process point down, windowId: %{public}u", windowId);
    WMError zOrderRes = windowRoot_->RaiseZOrderForAppWindow(node);
    WMError focusRes = windowRoot_->RequestFocus(windowId);
    windowRoot_->RequestActiveWindow(windowId);
    windowRoot_->FocusFaultDetection();
    if (zOrderRes == WMError::WM_OK || focusRes == WMError::WM_OK) {
        FlushWindowInfo(windowId);
        accessibilityConnection_->NotifyAccessibilityWindowInfo(windowRoot_->GetWindowNode(windowId),
            WindowUpdateType::WINDOW_UPDATE_FOCUSED);
        WLOGFD("ProcessPointDown end");
        return WMError::WM_OK;
    }
    return WMError::WM_ERROR_INVALID_OPERATION;
}

WMError WindowController::ProcessPointUp(uint32_t windowId)
{
    auto node = windowRoot_->GetWindowNode(windowId);
    if (node == nullptr) {
        WLOGFW("could not find window");
        return WMError::WM_ERROR_NULLPTR;
    }
    if (node->GetWindowType() == WindowType::WINDOW_TYPE_DOCK_SLICE) {
        DisplayId displayId = node->GetDisplayId();
        if (windowRoot_->IsDockSliceInExitSplitModeArea(displayId)) {
            windowRoot_->ExitSplitMode(displayId);
        } else {
            windowRoot_->ClearWindowPairSnapshot(node->GetDisplayId());
            auto property = node->GetWindowProperty();
            node->SetWindowSizeChangeReason(WindowSizeChangeReason::DRAG_END);
            property->SetRequestRect(property->GetWindowRect());
            WMError res = windowRoot_->UpdateWindowNode(windowId, WindowUpdateReason::UPDATE_RECT);
            if (res == WMError::WM_OK) {
                FlushWindowInfo(windowId);
                accessibilityConnection_->NotifyAccessibilityWindowInfo(node, WindowUpdateType::WINDOW_UPDATE_PROPERTY);
            }
        }
    }
    WMError res = windowRoot_->UpdateSizeChangeReason(windowId, WindowSizeChangeReason::DRAG_END);
    if (res != WMError::WM_OK) {
        return res;
    }
    return WMError::WM_OK;
}

WMError WindowController::InterceptInputEventToServer(uint32_t windowId)
{
    auto node = windowRoot_->GetWindowNode(windowId);
    if (node == nullptr) {
        WLOGFW("could not find window");
        return WMError::WM_ERROR_NULLPTR;
    }
    auto inputPidInServer = WindowInnerManager::GetInstance().GetPid();
    WLOGFD("InterceptInputEventToServer, windowId: %{public}u, inputPid: %{public}u", windowId, inputPidInServer);
    node->SetInputEventCallingPid(static_cast<int32_t>(inputPidInServer));
    FlushWindowInfo(windowId);
    return WMError::WM_OK;
}

WMError WindowController::RecoverInputEventToClient(uint32_t windowId)
{
    auto node = windowRoot_->GetWindowNode(windowId);
    if (node == nullptr) {
        WLOGFW("could not find window");
        return WMError::WM_ERROR_NULLPTR;
    }
    if (node->GetInputEventCallingPid() == node->GetCallingPid()) {
        WLOGFD("There is no need to recover input event to client");
        return WMError::WM_OK;
    }
    node->SetInputEventCallingPid(node->GetCallingPid());
    RecoverDefaultMouseStyle(windowId);
    AsyncFlushInputInfo(windowId);
    return WMError::WM_OK;
}

void WindowController::AsyncFlushInputInfo(uint32_t windowId)
{
    WLOGFD("AsyncFlushWindowInfo");
    displayZoomController_->UpdateWindowZoomInfo(windowId);
    RSTransaction::FlushImplicitTransaction();
    MMI::DisplayGroupInfo displayInfo_ = inputWindowMonitor_->GetDisplayInfo(windowId);
    auto task = [this, displayInfo_]() {
        MMI::InputManager::GetInstance()->UpdateDisplayInfo(displayInfo_);
    };
    WindowInnerManager::GetInstance().PostTask(task, "UpdateDisplayInfo");
}

void WindowController::RecoverDefaultMouseStyle(uint32_t windowId)
{
    // asynchronously calls SetMouseStyle of MultiModalInput
    auto task = [this, windowId]() {
        MMI::InputManager::GetInstance()->SetPointerStyle(windowId, MMI::MOUSE_ICON::DEFAULT);
    };
    WindowInnerManager::GetInstance().PostTask(task, "RecoverDefaultMouseStyle");
}

WMError WindowController::NotifyWindowClientPointUp(uint32_t windowId,
    const std::shared_ptr<MMI::PointerEvent>& pointerEvent)
{
    auto node = windowRoot_->GetWindowNode(windowId);
    if (node == nullptr) {
        WLOGFW("could not find window");
        return WMError::WM_ERROR_NULLPTR;
    }
    if (node->GetWindowToken() != nullptr) {
        WLOGFD("notify client when receive point_up event, windowId: %{public}u", windowId);
        node->GetWindowToken()->NotifyWindowClientPointUp(pointerEvent);
    }
    return WMError::WM_OK;
}

void WindowController::MinimizeAllAppWindows(DisplayId displayId)
{
    windowRoot_->MinimizeAllAppWindows(displayId);
    if (RemoteAnimation::NotifyAnimationByHome() != WMError::WM_OK) {
        MinimizeApp::ExecuteMinimizeAll();
    }
}

WMError WindowController::ToggleShownStateForAllAppWindows()
{
    if (isScreenLocked_) {
        return WMError::WM_DO_NOTHING;
    }
    return windowRoot_->ToggleShownStateForAllAppWindows();
}

WMError WindowController::GetTopWindowId(uint32_t mainWinId, uint32_t& topWinId)
{
    return windowRoot_->GetTopWindowId(mainWinId, topWinId);
}

void WindowController::FlushWindowInfo(uint32_t windowId)
{
    WLOGFD("FlushWindowInfo");
    displayZoomController_->UpdateWindowZoomInfo(windowId);
    RSTransaction::FlushImplicitTransaction();
    inputWindowMonitor_->UpdateInputWindow(windowId);
}

void WindowController::FlushWindowInfoWithDisplayId(DisplayId displayId)
{
    WLOGFD("FlushWindowInfoWithDisplayId, displayId: %{public}" PRIu64"", displayId);
    RSTransaction::FlushImplicitTransaction();
    inputWindowMonitor_->UpdateInputWindowByDisplayId(displayId);
}

void WindowController::UpdateWindowAnimation(const sptr<WindowNode>& node)
{
    if (node == nullptr || (node->leashWinSurfaceNode_ == nullptr && node->surfaceNode_ == nullptr)) {
        WLOGFE("windowNode or surfaceNode is nullptr");
        return;
    }
    const auto& windowAnimationConfig = WindowNodeContainer::GetAnimationConfigRef().windowAnimationConfig_;

    uint32_t animationFlag = node->GetWindowProperty()->GetAnimationFlag();
    uint32_t windowId = node->GetWindowProperty()->GetWindowId();
    WLOGFD("windowId: %{public}u, animationFlag: %{public}u", windowId, animationFlag);
    std::shared_ptr<const RSTransitionEffect> effect = nullptr;
    if (animationFlag == static_cast<uint32_t>(WindowAnimation::DEFAULT)) {
        effect = RSTransitionEffect::Create()
            ->Scale(windowAnimationConfig.scale_)
            ->Rotate(windowAnimationConfig.rotation_)
            ->Translate(windowAnimationConfig.translate_)
            ->Opacity(windowAnimationConfig.opacity_);
    } else if (animationFlag == static_cast<uint32_t>(WindowAnimation::INPUTE)) {
        float translateY = static_cast<float>(node->GetWindowRect().height_);
        if (!node->GetWindowRect().height_) {
            translateY = static_cast<float>(node->GetRequestRect().height_);
        }
        effect = RSTransitionEffect::Create()->Translate(Vector3f(0, translateY, 0))->Opacity(0.0f);
    };
    if (node->leashWinSurfaceNode_) {
        node->leashWinSurfaceNode_->SetTransitionEffect(effect);
    }
    if (node->surfaceNode_) {
        node->surfaceNode_->SetTransitionEffect(effect);
    }
}

WMError WindowController::SetWindowLayoutMode(WindowLayoutMode mode)
{
    WMError res = WMError::WM_OK;
    auto displayIds = windowRoot_->GetAllDisplayIds();
    for (auto displayId : displayIds) {
        res = windowRoot_->SetWindowLayoutMode(displayId, mode);
        if (res != WMError::WM_OK) {
            return res;
        }
        displayZoomController_->UpdateAllWindowsZoomInfo(displayId);
        FlushWindowInfoWithDisplayId(displayId);
        accessibilityConnection_->NotifyAccessibilityWindowInfo(displayId, WindowUpdateType::WINDOW_UPDATE_PROPERTY);
    }
    MinimizeApp::ExecuteMinimizeAll();
    return res;
}

WMError WindowController::UpdateProperty(sptr<WindowProperty>& property, PropertyChangeAction action)
{
    if (property == nullptr) {
        WLOGFE("property is invalid");
        return WMError::WM_ERROR_NULLPTR;
    }
    uint32_t windowId = property->GetWindowId();
    auto node = windowRoot_->GetWindowNode(windowId);
    if (node == nullptr) {
        WLOGFE("window is invalid");
        return WMError::WM_ERROR_NULLPTR;
    }
    WLOGFD("window: [%{public}s, %{public}u] update property for action: %{public}u", node->GetWindowName().c_str(),
        node->GetWindowId(), static_cast<uint32_t>(action));
    WMError ret = WMError::WM_OK;
    switch (action) {
        case PropertyChangeAction::ACTION_UPDATE_RECT: {
            node->SetDecoStatus(property->GetDecoStatus());
            node->SetOriginRect(property->GetOriginRect());
            node->SetDragType(property->GetDragType());
            ret = ResizeRectAndFlush(windowId, property->GetRequestRect(), property->GetWindowSizeChangeReason());
            if (node->GetWindowMode() == WindowMode::WINDOW_MODE_FLOATING && ret == WMError::WM_OK &&
                callingWindowId_ == windowId && !WindowHelper::IsEmptyRect(callingWindowRestoringRect_)) {
                if (property->GetWindowSizeChangeReason() != WindowSizeChangeReason::MOVE) {
                    callingWindowId_ = 0u;
                    callingWindowRestoringRect_ = { 0, 0, 0, 0 };
                } else {
                    auto windowRect = node->GetWindowRect();
                    callingWindowRestoringRect_.posX_ = windowRect.posX_;
                    callingWindowRestoringRect_.posY_ = windowRect.posY_;
                }
            }
            break;
        }
        case PropertyChangeAction::ACTION_UPDATE_MODE: {
            ret = SetWindowMode(windowId, property->GetWindowMode());
            break;
        }
        case PropertyChangeAction::ACTION_UPDATE_FLAGS: {
            ret = SetWindowFlags(windowId, property->GetWindowFlags());
            break;
        }
        case PropertyChangeAction::ACTION_UPDATE_OTHER_PROPS: {
            auto& props = property->GetSystemBarProperty();
            for (auto& iter : props) {
                SetSystemBarProperty(windowId, iter.first, iter.second);
            }
            break;
        }
        case PropertyChangeAction::ACTION_UPDATE_FOCUSABLE: {
            node->SetFocusable(property->GetFocusable());
            windowRoot_->UpdateFocusableProperty(windowId);
            FlushWindowInfo(windowId);
            accessibilityConnection_->NotifyAccessibilityWindowInfo(node, WindowUpdateType::WINDOW_UPDATE_PROPERTY);
            break;
        }
        case PropertyChangeAction::ACTION_UPDATE_TOUCHABLE: {
            node->SetTouchable(property->GetTouchable());
            FlushWindowInfo(windowId);
            accessibilityConnection_->NotifyAccessibilityWindowInfo(node, WindowUpdateType::WINDOW_UPDATE_PROPERTY);
            break;
        }
        case PropertyChangeAction::ACTION_UPDATE_CALLING_WINDOW: {
            node->SetCallingWindow(property->GetCallingWindow());
            break;
        }
        case PropertyChangeAction::ACTION_UPDATE_ORIENTATION: {
            node->SetRequestedOrientation(property->GetRequestedOrientation());
            if (WindowHelper::IsRotatableWindow(node->GetWindowType(), node->GetWindowMode())) {
                DisplayManagerServiceInner::GetInstance().
                    SetOrientationFromWindow(node->GetDisplayId(), property->GetRequestedOrientation());
            }
            break;
        }
        case PropertyChangeAction::ACTION_UPDATE_TURN_SCREEN_ON: {
            node->SetTurnScreenOn(property->IsTurnScreenOn());
            HandleTurnScreenOn(node);
            break;
        }
        case PropertyChangeAction::ACTION_UPDATE_KEEP_SCREEN_ON: {
            node->SetKeepScreenOn(property->IsKeepScreenOn());
            windowRoot_->HandleKeepScreenOn(node->GetWindowId(), node->IsKeepScreenOn());
            break;
        }
        case PropertyChangeAction::ACTION_UPDATE_SET_BRIGHTNESS: {
            node->SetBrightness(property->GetBrightness());
            windowRoot_->SetBrightness(node->GetWindowId(), node->GetBrightness());
            break;
        }
        case PropertyChangeAction::ACTION_UPDATE_MODE_SUPPORT_INFO: {
            node->SetModeSupportInfo(property->GetModeSupportInfo());
            break;
        }
        case PropertyChangeAction::ACTION_UPDATE_TOUCH_HOT_AREA: {
            std::vector<Rect> rects;
            property->GetTouchHotAreas(rects);
            ret = UpdateTouchHotAreas(node, rects);
            break;
        }
        case PropertyChangeAction::ACTION_UPDATE_ANIMATION_FLAG: {
            node->GetWindowProperty()->SetAnimationFlag(property->GetAnimationFlag());
            UpdateWindowAnimation(node);
            break;
        }
        case PropertyChangeAction::ACTION_UPDATE_TRANSFORM_PROPERTY: {
            node->SetTransform(property->GetTransform());
            node->SetWindowSizeChangeReason(WindowSizeChangeReason::TRANSFORM);
            node->GetWindowProperty()->SetAnimateWindowFlag(true);
            ret = UpdateTransform(windowId);
            break;
        }
        case PropertyChangeAction::ACTION_UPDATE_PRIVACY_MODE: {
            node->GetWindowProperty()->SetPrivacyMode(property->GetPrivacyMode());
            break;
        }
        case PropertyChangeAction::ACTION_UPDATE_LIMIT_SIZE: {
            node->GetWindowProperty()->SetSizeLimits(property->GetSizeLimits());
            break;
        }
        default:
            break;
    }
    return ret;
}

WMError WindowController::GetAccessibilityWindowInfo(std::vector<sptr<AccessibilityWindowInfo>>& infos) const
{
    accessibilityConnection_->GetAccessibilityWindowInfo(infos);
    return WMError::WM_OK;
}

WMError WindowController::GetVisibilityWindowInfo(std::vector<sptr<WindowVisibilityInfo>>& infos) const
{
    windowRoot_->GetVisibilityWindowInfo(infos);
    return WMError::WM_OK;
}

WMError WindowController::GetModeChangeHotZones(DisplayId displayId,
    ModeChangeHotZones& hotZones, const ModeChangeHotZonesConfig& config)
{
    return windowRoot_->GetModeChangeHotZones(displayId, hotZones, config);
}

WMError WindowController::UpdateTouchHotAreas(const sptr<WindowNode>& node, const std::vector<Rect>& rects)
{
    std::ostringstream oss;
    int index = 0;
    for (const auto& rect : rects) {
        oss << "[ " << rect.posX_ << ", " << rect.posY_ << ", " << rect.width_ << ", " << rect.height_ << " ]";
        index++;
        if (index < static_cast<int32_t>(rects.size())) {
            oss <<", ";
        }
    }
    WLOGFD("windowId: %{public}u, size: %{public}d, rects: %{public}s",
        node->GetWindowId(), static_cast<int32_t>(rects.size()), oss.str().c_str());
    if (rects.size() > TOUCH_HOT_AREA_MAX_NUM) {
        WLOGFE("the number of touch hot areas exceeds the maximum");
        return WMError::WM_ERROR_INVALID_PARAM;
    }

    std::vector<Rect> touchHotAreas;
    std::vector<Rect> pointerHotAreas;
    if (rects.empty()) {
        touchHotAreas.emplace_back(node->GetEntireWindowTouchHotArea());
        pointerHotAreas.emplace_back(node->GetEntireWindowPointerHotArea());
    } else {
        Rect windowRect = node->GetWindowRect();
        if (!WindowHelper::CalculateTouchHotAreas(windowRect, rects, touchHotAreas)) {
            WLOGFE("the requested touch hot areas are incorrect");
            return WMError::WM_ERROR_INVALID_PARAM;
        }
        pointerHotAreas = touchHotAreas;
    }
    node->GetWindowProperty()->SetTouchHotAreas(rects);
    node->SetTouchHotAreas(touchHotAreas);
    node->SetPointerHotAreas(pointerHotAreas);
    FlushWindowInfo(node->GetWindowId());
    accessibilityConnection_->NotifyAccessibilityWindowInfo(node, WindowUpdateType::WINDOW_UPDATE_PROPERTY);
    return WMError::WM_OK;
}

WMError WindowController::UpdateTransform(uint32_t windowId)
{
    WMError res = windowRoot_->UpdateWindowNode(windowId, WindowUpdateReason::UPDATE_TRANSFORM);
    if (res != WMError::WM_OK) {
        return res;
    }
    FlushWindowInfo(windowId);
    accessibilityConnection_->NotifyAccessibilityWindowInfo(windowRoot_->GetWindowNode(windowId),
        WindowUpdateType::WINDOW_UPDATE_PROPERTY);
    return WMError::WM_OK;
}

void WindowController::NotifyTouchOutside(const sptr<WindowNode>& node)
{
    auto windowNodeContainer = windowRoot_->GetOrCreateWindowNodeContainer(node->GetDisplayId());
    if (windowNodeContainer == nullptr) {
        WLOGFE("window node container is null");
        return;
    }

    std::vector<sptr<WindowNode>> windowNodes;
    windowNodeContainer->TraverseContainer(windowNodes);
    uint32_t skipNodeId = GetEmbedNodeId(windowNodes, node);
    for (const auto& windowNode : windowNodes) {
        if (windowNode == nullptr || windowNode->GetWindowToken() == nullptr ||
            windowNode->GetWindowId() == skipNodeId ||
            windowNode->GetWindowId() == node->GetWindowId()) {
            WLOGFD("continue %{public}s", windowNode == nullptr ? "nullptr" : windowNode->GetWindowName().c_str());
            continue;
        }
        WLOGFD("notify %{public}s id %{public}d", windowNode->GetWindowName().c_str(), windowNode->GetWindowId());
        windowNode->GetWindowToken()->NotifyTouchOutside();
    }
}

uint32_t WindowController::GetEmbedNodeId(const std::vector<sptr<WindowNode>>& windowNodes,
    const sptr<WindowNode>& node)
{
    if (node->GetWindowType() != WindowType::WINDOW_TYPE_APP_COMPONENT) {
        return 0;
    }

    Rect nodeRect = node->GetWindowRect();
    bool isSkip = true;
    for (auto& windowNode : windowNodes) {
        if (windowNode == nullptr) {
            continue;
        }
        if (windowNode->GetWindowId() == node->GetWindowId()) {
            isSkip = false;
            continue;
        }
        if (isSkip) {
            continue;
        }
        if (nodeRect.IsInsideOf(windowNode->GetWindowRect())) {
            WLOGI("TouchOutside window type is component %{public}s windowNode %{public}d",
                windowNode->GetWindowName().c_str(), windowNode->GetWindowId());
            return windowNode->GetWindowId();
        }
    }
    return 0;
}

void WindowController::MinimizeWindowsByLauncher(std::vector<uint32_t>& windowIds, bool isAnimated,
    sptr<RSIWindowAnimationFinishedCallback>& finishCallback)
{
    windowRoot_->MinimizeTargetWindows(windowIds);
    auto func = []() {
        MinimizeApp::ExecuteMinimizeTargetReasons(MinimizeReason::GESTURE_ANIMATION);
    };
    if (!isAnimated) {
        func();
    } else {
        finishCallback = RemoteAnimation::CreateAnimationFinishedCallback(func);
        if (finishCallback == nullptr) {
            return;
        }
    }
}

void WindowController::OnScreenshot(DisplayId displayId)
{
    sptr<WindowNode> windowNode;
    WMError res = GetFocusWindowNode(displayId, windowNode);
    if (res != WMError::WM_OK) {
        return;
    }
    auto windowToken = windowNode->GetWindowToken();
    if (windowToken == nullptr) {
        WLOGFE("notify screenshot failed: window token is null.");
        return;
    }
    windowToken->NotifyScreenshot();
}

void WindowController::SetAnchorOffset(int32_t deltaX, int32_t deltaY)
{
    displayZoomController_->SetAnchorOffset(deltaX, deltaY);
    DisplayId displayId = DisplayManagerServiceInner::GetInstance().GetDefaultDisplayId();
    FlushWindowInfoWithDisplayId(displayId);
}

void WindowController::OffWindowZoom()
{
    displayZoomController_->OffWindowZoom();
    DisplayId displayId = DisplayManagerServiceInner::GetInstance().GetDefaultDisplayId();
    FlushWindowInfoWithDisplayId(displayId);
}

void WindowController::SetAnchorAndScale(int32_t x, int32_t y, float scale)
{
    displayZoomController_->SetAnchorAndScale(x, y, scale);
    DisplayId displayId = DisplayManagerServiceInner::GetInstance().GetDefaultDisplayId();
    FlushWindowInfoWithDisplayId(displayId);
}

WMError WindowController::BindDialogTarget(uint32_t& windowId, sptr<IRemoteObject> targetToken)
{
    auto node = windowRoot_->GetWindowNode(windowId);
    if (node == nullptr) {
        WLOGFE("could not find window");
        return WMError::WM_ERROR_NULLPTR;
    }
    if (windowRoot_->CheckMultiDialogWindows(node->GetWindowType(), targetToken)) {
        return WMError::WM_ERROR_INVALID_WINDOW;
    }

    node->dialogTargetToken_ = targetToken;

    sptr<WindowNode> parentNode = windowRoot_->FindDialogCallerNode(node->GetWindowType(), node->dialogTargetToken_);
    if (parentNode != nullptr) {
        auto position = parentNode->children_.end();
        for (auto iter = parentNode->children_.begin(); iter < parentNode->children_.end(); ++iter) {
            if ((*iter)->priority_ > node->priority_) {
                position = iter;
                break;
            }
        }
        parentNode->children_.insert(position, node);
    }

    return WMError::WM_OK;
}
} // namespace OHOS
} // namespace Rosen
