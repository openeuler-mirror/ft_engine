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

#include "starting_window.h"
#include <ability_manager_client.h>
#include <common/rs_common_def.h>
#include <display_manager_service_inner.h>
#include <hitrace_meter.h>
#include <transaction/rs_transaction.h>
#include "remote_animation.h"
#include "window_helper.h"
#include "window_inner_manager.h"
#include "window_manager_hilog.h"
#include "window_manager_service.h"

namespace OHOS {
namespace Rosen {
namespace {
    constexpr HiviewDFX::HiLogLabel LABEL = {LOG_CORE, HILOG_DOMAIN_WINDOW, "StartingWindow"};
}

std::recursive_mutex StartingWindow::mutex_;
WindowMode StartingWindow::defaultMode_ = WindowMode::WINDOW_MODE_FULLSCREEN;

sptr<WindowNode> StartingWindow::CreateWindowNode(const sptr<WindowTransitionInfo>& info, uint32_t winId)
{
    sptr<WindowProperty> property = new(std::nothrow) WindowProperty();
    if (property == nullptr || info == nullptr) {
        return nullptr;
    }

    property->SetRequestRect(info->GetWindowRect());
    if (WindowHelper::IsValidWindowMode(info->GetWindowMode())) {
        property->SetWindowMode(info->GetWindowMode());
    } else {
        property->SetWindowMode(defaultMode_);
    }

    property->SetDisplayId(info->GetDisplayId());
    property->SetWindowType(info->GetWindowType());
    property->AddWindowFlag(WindowFlag::WINDOW_FLAG_NEED_AVOID);
    if (info->GetShowFlagWhenLocked()) {
        property->AddWindowFlag(WindowFlag::WINDOW_FLAG_SHOW_WHEN_LOCKED);
    }
    property->SetWindowId(winId);
    sptr<WindowNode> node = new(std::nothrow) WindowNode(property);
    if (node == nullptr) {
        return nullptr;
    }
    // test
    node->stateMachine_.SetWindowId(winId);
    node->abilityToken_ = info->GetAbilityToken();
    node->SetWindowSizeLimits(info->GetWindowSizeLimits());
    node->abilityInfo_.missionId_ = info->GetMissionId();
    node->abilityInfo_.bundleName_ = info->GetBundleName();
    node->abilityInfo_.abilityName_ = info->GetAbilityName();
    uint32_t modeSupportInfo = WindowHelper::ConvertSupportModesToSupportInfo(info->GetWindowSupportModes());
    node->SetModeSupportInfo(modeSupportInfo);

    if (CreateLeashAndStartingSurfaceNode(node) != WMError::WM_OK) {
        return nullptr;
    }
    node->stateMachine_.TransitionTo(WindowNodeState::STARTING_CREATED);
    return node;
}

WMError StartingWindow::CreateLeashAndStartingSurfaceNode(sptr<WindowNode>& node)
{
    struct RSSurfaceNodeConfig rsSurfaceNodeConfig;
    rsSurfaceNodeConfig.SurfaceNodeName = "leashWindow" + std::to_string(node->GetWindowId());
    node->leashWinSurfaceNode_ = RSSurfaceNode::Create(rsSurfaceNodeConfig, RSSurfaceNodeType::LEASH_WINDOW_NODE);
    if (node->leashWinSurfaceNode_ == nullptr) {
        WLOGFE("create leashWinSurfaceNode failed");
        return WMError::WM_ERROR_NULLPTR;
    }

    rsSurfaceNodeConfig.SurfaceNodeName = "startingWindow" + std::to_string(node->GetWindowId());
    node->startingWinSurfaceNode_ = RSSurfaceNode::Create(rsSurfaceNodeConfig, RSSurfaceNodeType::STARTING_WINDOW_NODE);
    if (node->startingWinSurfaceNode_ == nullptr) {
        WLOGFE("create startingWinSurfaceNode failed");
        node->leashWinSurfaceNode_ = nullptr;
        return WMError::WM_ERROR_NULLPTR;
    }
    WLOGFI("Create leashWinSurfaceNode and startingWinSurfaceNode success with id:%{public}u!", node->GetWindowId());
    return WMError::WM_OK;
}

WMError StartingWindow::DrawStartingWindow(sptr<WindowNode>& node,
    std::shared_ptr<Media::PixelMap> pixelMap, uint32_t bkgColor, bool isColdStart)
{
    if (node == nullptr) {
        return WMError::WM_ERROR_NULLPTR;
    }
    // using snapshot to support hot start since node destroy when hide
    HITRACE_METER_FMT(HITRACE_TAG_WINDOW_MANAGER, "wms:DrawStartingWindow(%u)", node->GetWindowId());
    Rect rect = node->GetWindowRect();
    if (RemoteAnimation::CheckRemoteAnimationEnabled(node->GetDisplayId()) && node->leashWinSurfaceNode_) {
        // hides this node until RSProxyNode send valid context alpha/matrix
        node->leashWinSurfaceNode_->ResetContextAlpha();
    }
    if (!isColdStart) {
        return WMError::WM_OK;
    }
    if (node->startingWinSurfaceNode_ == nullptr) {
        WLOGFE("no starting Window SurfaceNode!");
        return WMError::WM_ERROR_NULLPTR;
    }
    if (pixelMap == nullptr) {
        SurfaceDraw::DrawColor(node->startingWinSurfaceNode_, rect.width_, rect.height_, bkgColor);
        return WMError::WM_OK;
    }

    WLOGFD("draw background in sperate");
    SurfaceDraw::DrawImageRect(node->startingWinSurfaceNode_, rect, pixelMap, bkgColor);
    return WMError::WM_OK;
}

void StartingWindow::HandleClientWindowCreate(sptr<WindowNode>& node, sptr<IWindow>& window,
    uint32_t& windowId, const std::shared_ptr<RSSurfaceNode>& surfaceNode, sptr<WindowProperty>& property,
    int32_t pid, int32_t uid)
{
    if (node == nullptr) {
        WLOGFE("node is nullptr");
        return;
    }
    node->surfaceNode_ = surfaceNode;
    node->SetWindowToken(window);
    node->SetCallingPid(pid);
    node->SetCallingUid(uid);
    windowId = node->GetWindowId();
    // test
    node->stateMachine_.SetWindowId(windowId);
    node->stateMachine_.SetWindowType(property->GetWindowType());
    WLOGFI("after set Id:%{public}u, requestRect:[%{public}d, %{public}d, %{public}u, %{public}u]",
        node->GetWindowId(), node->GetRequestRect().posX_, node->GetRequestRect().posY_,
        node->GetRequestRect().width_, node->GetRequestRect().height_);
    // Register FirstFrame Callback to rs, replace startwin
    wptr<WindowNode> weak = node;
    auto firstFrameCompleteCallback = [weak]() {
        WindowManagerService::GetInstance().PostAsyncTask([weak]() {
            FinishAsyncTraceArgs(HITRACE_TAG_WINDOW_MANAGER, static_cast<int32_t>(TraceTaskId::STARTING_WINDOW),
                "wms:async:ShowStartingWindow");
            auto weakNode = weak.promote();
            if (weakNode == nullptr || weakNode->leashWinSurfaceNode_ == nullptr) {
                WLOGFE("windowNode or leashWinSurfaceNode_ is nullptr");
                return;
            }
            WLOGFI("StartingWindow::Replace surfaceNode, id: %{public}u", weakNode->GetWindowId());
            weakNode->leashWinSurfaceNode_->RemoveChild(weakNode->startingWinSurfaceNode_);
            WindowInnerManager::GetInstance().CompleteFirstFrameDrawing(weakNode);
            RSTransaction::FlushImplicitTransaction();
            weakNode->firstFrameAvaliable_ = true;
            weakNode->startingWinSurfaceNode_ = nullptr;
        });
    };
    node->surfaceNode_->SetBufferAvailableCallback(firstFrameCompleteCallback);
    RSTransaction::FlushImplicitTransaction();
}

void StartingWindow::ReleaseStartWinSurfaceNode(sptr<WindowNode>& node)
{
    std::lock_guard<std::recursive_mutex> lock(mutex_);
    if (!node->leashWinSurfaceNode_) {
        WLOGFI("cannot release leashwindow since leash is null, id:%{public}u", node->GetWindowId());
        return;
    }
    node->leashWinSurfaceNode_->RemoveChild(node->startingWinSurfaceNode_);
    node->leashWinSurfaceNode_->RemoveChild(node->surfaceNode_);
    node->leashWinSurfaceNode_ = nullptr;
    node->startingWinSurfaceNode_ = nullptr;
    WLOGFI("Release startwindow surfaceNode end id: %{public}u, [leashWinSurface]: use_count: %{public}ld, \
        [startWinSurface]: use_count: %{public}ld ", node->GetWindowId(),
        node->leashWinSurfaceNode_.use_count(), node->startingWinSurfaceNode_.use_count());
    RSTransaction::FlushImplicitTransaction();
}

void StartingWindow::AddNodeOnRSTree(sptr<WindowNode>& node, const AnimationConfig& animationConfig,
    bool isMultiDisplay)
{
    auto updateRSTreeFunc = [&]() {
        auto& dms = DisplayManagerServiceInner::GetInstance();
        DisplayId displayId = node->GetDisplayId();
        if (!node->surfaceNode_) { // cold start
            if (!WindowHelper::IsMainWindow(node->GetWindowType())) {
                WLOGFE("window id:%{public}d type: %{public}u is not Main Window!",
                    node->GetWindowId(), static_cast<uint32_t>(node->GetWindowType()));
            }
            dms.UpdateRSTree(displayId, displayId, node->leashWinSurfaceNode_, true, isMultiDisplay);
            node->leashWinSurfaceNode_->AddChild(node->startingWinSurfaceNode_, -1);
        } else { // hot start
            const auto& displayIdVec = node->GetShowingDisplays();
            for (auto& shownDisplayId : displayIdVec) {
                if (node->leashWinSurfaceNode_) { // to app
                    dms.UpdateRSTree(shownDisplayId, shownDisplayId, node->leashWinSurfaceNode_, true, isMultiDisplay);
                } else { // to launcher
                    dms.UpdateRSTree(shownDisplayId, shownDisplayId, node->surfaceNode_, true, isMultiDisplay);
                }
                for (auto& child : node->children_) {
                    if (child->currentVisibility_) {
                        dms.UpdateRSTree(shownDisplayId, shownDisplayId, child->surfaceNode_, true, isMultiDisplay);
                    }
                }
            }
        }
    };
    wptr<WindowNode> weakNode = node;
    auto finishCallBack = [weakNode]() {
        auto weak = weakNode.promote();
        if (weak == nullptr) {
            return;
        }
        auto winRect = weak->GetWindowRect();
        WLOGFI("before setBounds windowRect: %{public}d, %{public}d, %{public}d, %{public}d",
            winRect.posX_, winRect.posY_, winRect.width_, winRect.height_);
        if (weak->leashWinSurfaceNode_) {
            weak->leashWinSurfaceNode_->SetBounds(winRect.posX_, winRect.posY_, winRect.width_, winRect.height_);
            weak->leashWinSurfaceNode_->SetAnimationFinished();
        }
        RSTransaction::FlushImplicitTransaction();
    };
    if (!RemoteAnimation::CheckAnimationController()) {
        RSNode::Animate(animationConfig.windowAnimationConfig_.animationTiming_.timingProtocol_,
            animationConfig.windowAnimationConfig_.animationTiming_.timingCurve_, updateRSTreeFunc, finishCallBack);
    } else {
        // add or remove window without animation
        updateRSTreeFunc();
    }
}

void StartingWindow::SetDefaultWindowMode(WindowMode defaultMode)
{
    defaultMode_ = defaultMode;
}
} // Rosen
} // OHOS
