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
#include "display_zoom_controller.h"
#include "window_helper.h"

namespace OHOS::Rosen {
namespace {
    constexpr HiviewDFX::HiLogLabel LABEL = {LOG_CORE, HILOG_DOMAIN_WINDOW, "DisplayZoomController"};
}

void DisplayZoomController::SetAnchorAndScale(int32_t x, int32_t y, float scale)
{
    WLOGFD("DisplayZoom: On, anchor x:%{public}d, y:%{public}d, scale:%{public}f", x, y, scale);
    if (scale <= 0) {
        return;
    } else if (zoomInfo_.scale * scale < DISPLAY_ZOOM_MIN_SCALE) {
        scale = DISPLAY_ZOOM_MIN_SCALE / zoomInfo_.scale;
    } else if (zoomInfo_.scale * scale > DISPLAY_ZOOM_MAX_SCALE) {
        scale = DISPLAY_ZOOM_MAX_SCALE / zoomInfo_.scale;
    }
    DisplayId displayId = DisplayManagerServiceInner::GetInstance().GetDefaultDisplayId();
    sptr<WindowNodeContainer> windowNodeContainer = windowRoot_->GetOrCreateWindowNodeContainer(displayId);
    if (windowNodeContainer == nullptr) {
        return;
    }
    std::vector<sptr<WindowNode>> windowNodes;
    windowNodeContainer->TraverseContainer(windowNodes);
    bool isAlreadyCalcu = false;
    for (auto& node : windowNodes) {
        if (displayZoomWindowTypeSkipped_.find(node->GetWindowProperty()->GetWindowType()) !=
            displayZoomWindowTypeSkipped_.end()) {
            continue;
        }
        Transform zoomTrans;
        if (!isAlreadyCalcu) {
            zoomTrans = CalcuZoomTrans(node, {x, y, scale, 0, 0});
            zoomInfo_.scale *= scale;
            Rect rect = node->GetWindowRect();
            zoomInfo_.pivotX = rect.posX_ + zoomTrans.pivotX_ * rect.width_;
            zoomInfo_.pivotY = rect.posY_ + zoomTrans.pivotY_ * rect.height_;
            zoomInfo_.translateX = zoomTrans.translateX_;
            zoomInfo_.translateY = zoomTrans.translateY_;
            isAlreadyCalcu = true;
        } else {
            zoomTrans = CalcuZoomTransByZoomInfo(node);
        }
        UpdateClientAndSurfaceZoomInfo(node, zoomTrans);
    }
}

void DisplayZoomController::SetAnchorOffset(int32_t deltaX, int32_t deltaY)
{
    WLOGFD("DisplayZoom: SetAnchorOffset");
    DisplayId displayId = DisplayManagerServiceInner::GetInstance().GetDefaultDisplayId();
    sptr<WindowNodeContainer> windowNodeContainer = windowRoot_->GetOrCreateWindowNodeContainer(displayId);
    if (windowNodeContainer == nullptr) {
        return;
    }
    if (!UpdateZoomTranslateInfo(windowNodeContainer, displayId, deltaX, deltaY)) {
        return;
    }
    WindowNodeOperationFunc translateFunc = [this, deltaX, deltaY](sptr<WindowNode> node) {
        if (displayZoomWindowTypeSkipped_.find(node->GetWindowProperty()->GetWindowType()) !=
            displayZoomWindowTypeSkipped_.end()) {
            return false;
        }
        Transform zoomTrans = node->GetZoomTransform();
        zoomTrans.translateX_ += static_cast<float>(deltaX);
        zoomTrans.translateY_ += static_cast<float>(deltaY);
        UpdateClientAndSurfaceZoomInfo(node, zoomTrans);
        return false;
    };
    windowNodeContainer->TraverseWindowTree(translateFunc, false);
}

void DisplayZoomController::OffWindowZoom()
{
    WLOGFD("DisplayZoom: Off");
    DisplayId displayId = DisplayManagerServiceInner::GetInstance().GetDefaultDisplayId();
    sptr<WindowNodeContainer> windowNodeContainer = windowRoot_->GetOrCreateWindowNodeContainer(displayId);
    if (windowNodeContainer == nullptr) {
        return;
    }
    zoomInfo_ = {0, 0, 1.0, 0, 0};
    std::vector<sptr<WindowNode>> windowNodes;
    windowNodeContainer->TraverseContainer(windowNodes);
    for (auto& node : windowNodes) {
        if (displayZoomWindowTypeSkipped_.find(node->GetWindowProperty()->GetWindowType()) !=
            displayZoomWindowTypeSkipped_.end()) {
            continue;
        }
        ClearZoomTransformInner(node);
    }
}

void DisplayZoomController::UpdateAllWindowsZoomInfo(DisplayId displayId)
{
    if (zoomInfo_.scale == DISPLAY_ZOOM_OFF_SCALE) {
        return;
    }
    DisplayId defaultDisplayId = DisplayManagerServiceInner::GetInstance().GetDefaultDisplayId();
    if (defaultDisplayId != displayId) {
        return;
    }
    sptr<WindowNodeContainer> windowNodeContainer = windowRoot_->GetOrCreateWindowNodeContainer(displayId);
    if (windowNodeContainer == nullptr) {
        return;
    }
    int32_t deltaX, deltaY;
    deltaX = deltaY = 0;
    if (UpdateZoomTranslateInfo(windowNodeContainer, displayId, deltaX, deltaY)) {
        WLOGFD("Change ZoomInfo translation, deltaX:%{public}d, deltaY:%{public}d", deltaX, deltaY);
    }
    std::vector<sptr<WindowNode>> windowNodes;
    windowNodeContainer->TraverseContainer(windowNodes);
    for (auto& node: windowNodes) {
        HandleUpdateWindowZoomInfo(node);
    }
}

void DisplayZoomController::UpdateWindowZoomInfo(uint32_t windowId)
{
    if (zoomInfo_.scale == DISPLAY_ZOOM_OFF_SCALE) {
        return;
    }
    auto node = windowRoot_->GetWindowNode(windowId);
    if (node == nullptr) {
        return;
    }
    if (!node->currentVisibility_) {
        return;
    }
    DisplayId displayId = DisplayManagerServiceInner::GetInstance().GetDefaultDisplayId();
    if (node->GetDisplayId() != displayId) {
        return;
    }
    std::vector<sptr<WindowNode>> windowNodes;
    windowNodes.push_back(node);
    if (node->GetWindowType() == WindowType::WINDOW_TYPE_DOCK_SLICE) {
        windowNodes = windowRoot_->GetSplitScreenWindowNodes(node->GetDisplayId());
    }
    for (auto& windowNode: windowNodes) {
        HandleUpdateWindowZoomInfo(windowNode);
    }
}

void DisplayZoomController::ClearZoomTransform(std::vector<sptr<WindowNode>> nodes)
{
    if (zoomInfo_.scale == DISPLAY_ZOOM_OFF_SCALE) {
        return;
    }
    for (auto& node: nodes) {
        ClearZoomTransformInner(node);
    }
}

void DisplayZoomController::ClearZoomTransformInner(sptr<WindowNode> node)
{
    Transform recoverTrans;
    node->UpdateZoomTransform(recoverTrans, false);
    auto surfaceNode = node->leashWinSurfaceNode_ ? node->leashWinSurfaceNode_ : node->surfaceNode_;
    if (!node->GetWindowProperty()->IsAnimateWindow()) {
        TransformSurfaceNode(surfaceNode, recoverTrans);
    }
}

bool DisplayZoomController::UpdateZoomTranslateInfo(sptr<WindowNodeContainer> windowNodeContainer,
    DisplayId displayId, int32_t& deltaX, int32_t& deltaY)
{
    sptr<WindowNode> deskTop = windowNodeContainer->GetDeskTopWindow();
    if (deskTop == nullptr) {
        WLOGFE("DisplayZoom: can't find deskTop windowNode");
        return false;
    }
    Transform zoomTrans = deskTop->GetZoomTransform();
    Rect originalRect = deskTop->GetWindowRect();
    Rect zoomRect = originalRect;
    if (zoomTrans != Transform::Identity()) {
        deskTop->ComputeTransform();
        zoomRect = WindowHelper::TransformRect(deskTop->GetWindowProperty()->GetTransformMat(), originalRect);
    }
    sptr<DisplayInfo> displayInfo = windowNodeContainer->GetDisplayInfo(displayId);
    if (displayInfo == nullptr) {
        WLOGFE("DisplayZoom: can't get displayInfo");
        return false;
    }
    int32_t deltaXMax = displayInfo->GetOffsetX() - zoomRect.posX_;
    int32_t deltaXMin = displayInfo->GetOffsetX() + displayInfo->GetWidth() - zoomRect.posX_
        - static_cast<int32_t>(zoomRect.width_);
    int32_t deltaYMax = displayInfo->GetOffsetY() - zoomRect.posY_;
    int32_t deltaYMin = displayInfo->GetOffsetY() + displayInfo->GetHeight() - zoomRect.posY_
        - static_cast<int32_t>(zoomRect.height_);
    deltaX = MathHelper::Clamp(deltaX, deltaXMin, deltaXMax);
    deltaY = MathHelper::Clamp(deltaY, deltaYMin, deltaYMax);
    if (deltaX == 0 && deltaY == 0) {
        return false;
    }
    zoomInfo_.translateX += deltaX;
    zoomInfo_.translateY += deltaY;
    return true;
}

Transform DisplayZoomController::CalcuAnimateZoomTrans(sptr<WindowNode> node)
{
    Rect rect = node->GetWindowRect();
    if (rect.width_ == 0 || rect.height_ == 0) {
        return Transform::Identity();
    }
    Transform lastZoomTrans = CalcuZoomTransByZoomInfo(node);
    TransformHelper::Vector3 lastPivotPos = { rect.posX_ + lastZoomTrans.pivotX_ * rect.width_,
        rect.posY_ + lastZoomTrans.pivotY_ * rect.height_, 0 };
    TransformHelper::Matrix4 lastWorldMat = TransformHelper::CreateTranslation(-lastPivotPos) *
        WindowHelper::ComputeWorldTransformMat4(lastZoomTrans) *
        TransformHelper::CreateTranslation(lastPivotPos);

    Transform animateTrans = node->GetWindowProperty()->GetTransform();
    if (animateTrans.translateZ_ != 0.f) {
        node->GetWindowProperty()->ClearTransformZAxisOffset(animateTrans);
    }
    TransformHelper::Vector3 animatePivotPos = { rect.posX_ + animateTrans.pivotX_ * rect.width_,
        rect.posY_ + animateTrans.pivotY_ * rect.height_, 0 };
    TransformHelper::Matrix4 animateWorldMat = TransformHelper::CreateTranslation(-animatePivotPos) *
        WindowHelper::ComputeWorldTransformMat4(animateTrans) *
        TransformHelper::CreateTranslation(animatePivotPos);

    TransformHelper::Matrix4 finalWorldMat = animateWorldMat * lastWorldMat;
    Transform finalZoomTrans;
    finalZoomTrans.pivotX_ = (0 - rect.posX_) * 1.0 / rect.width_;
    finalZoomTrans.pivotY_ = (0 - rect.posY_) * 1.0 / rect.height_;
    TransformHelper::Vector3 scale = finalWorldMat.GetScale();
    TransformHelper::Vector3 translation = finalWorldMat.GetTranslation();
    finalZoomTrans.scaleX_ = scale.x_;
    finalZoomTrans.scaleY_ = scale.y_;
    finalZoomTrans.translateX_ = translation.x_;
    finalZoomTrans.translateY_ = translation.y_;
    finalZoomTrans.translateZ_ = translation.z_;
    finalZoomTrans.rotationX_ = animateTrans.rotationX_;
    finalZoomTrans.rotationY_ = animateTrans.rotationY_;
    finalZoomTrans.rotationZ_ = animateTrans.rotationZ_;

    return finalZoomTrans;
}

Transform DisplayZoomController::CalcuZoomTransByZoomInfo(sptr<WindowNode> node)
{
    Transform zoomTrans;
    Rect rect = node->GetWindowRect();
    if (rect.width_ == 0 || rect.height_ == 0) {
        return zoomTrans;
    }
    zoomTrans.pivotX_ = (zoomInfo_.pivotX - rect.posX_) * 1.0 / rect.width_;
    zoomTrans.pivotY_ = (zoomInfo_.pivotY - rect.posY_) * 1.0 / rect.height_;
    zoomTrans.scaleX_ = zoomTrans.scaleY_ = zoomInfo_.scale;
    zoomTrans.translateX_ = zoomInfo_.translateX;
    zoomTrans.translateY_ = zoomInfo_.translateY;
    return zoomTrans;
}

Transform DisplayZoomController::CalcuZoomTrans(sptr<WindowNode> node, const DisplayZoomInfo& zoomInfo)
{
    Rect rect = node->GetWindowRect();
    if (rect.width_ == 0 || rect.height_ == 0) {
        return Transform::Identity();
    }
    Transform lastZoomTrans = node->GetZoomTransform();
    TransformHelper::Vector3 lastPivotPos = { rect.posX_ + lastZoomTrans.pivotX_ * rect.width_,
        rect.posY_ + lastZoomTrans.pivotY_ * rect.height_, 0 };
    TransformHelper::Matrix4 lastWorldMat = TransformHelper::CreateTranslation(-lastPivotPos) *
        WindowHelper::ComputeWorldTransformMat4(lastZoomTrans) *
        TransformHelper::CreateTranslation(lastPivotPos);

    Transform zoomTrans;
    zoomTrans.scaleX_ = zoomTrans.scaleY_ = zoomInfo.scale;
    zoomTrans.translateX_ = zoomInfo.translateX;
    zoomTrans.translateY_ = zoomInfo.translateY;
    TransformHelper::Vector3 pivotPos = { zoomInfo.pivotX, zoomInfo.pivotY, 0 };
    TransformHelper::Matrix4 worldMat = TransformHelper::CreateTranslation(-pivotPos) *
        WindowHelper::ComputeWorldTransformMat4(zoomTrans) *
        TransformHelper::CreateTranslation(pivotPos);

    TransformHelper::Matrix4 finalWorldMat = lastWorldMat * worldMat;
    Transform finalZoomTrans;
    finalZoomTrans.pivotX_ = (0 - rect.posX_) * 1.0 / rect.width_;
    finalZoomTrans.pivotY_ = (0 - rect.posY_) * 1.0 / rect.height_;
    TransformHelper::Vector3 scale = finalWorldMat.GetScale();
    TransformHelper::Vector3 translation = finalWorldMat.GetTranslation();
    finalZoomTrans.scaleX_ = scale.x_;
    finalZoomTrans.scaleY_ = scale.y_;
    finalZoomTrans.translateX_ = translation.x_;
    finalZoomTrans.translateY_ = translation.y_;

    return finalZoomTrans;
}

void DisplayZoomController::UpdateClientAndSurfaceZoomInfo(sptr<WindowNode> node, const Transform& zoomTrans)
{
    node->UpdateZoomTransform(zoomTrans, true);
    auto surfaceNode = node->leashWinSurfaceNode_ ? node->leashWinSurfaceNode_ : node->surfaceNode_;
    if (!node->GetWindowProperty()->IsAnimateWindow()) {
        TransformSurfaceNode(surfaceNode, zoomTrans);
    }
    WLOGFD("%{public}s zoomTrans, pivotX:%{public}f, pivotY:%{public}f, scaleX:%{public}f, scaleY:%{public}f"
        ", transX:%{public}f, transY:%{public}f, transZ:%{public}f, rotateX:%{public}f, rotateY:%{public}f "
        "rotateZ:%{public}f", node->GetWindowName().c_str(), zoomTrans.pivotX_, zoomTrans.pivotY_, zoomTrans.scaleX_,
        zoomTrans.scaleY_, zoomTrans.translateX_, zoomTrans.translateY_, zoomTrans.translateZ_, zoomTrans.rotationX_,
        zoomTrans.rotationY_, zoomTrans.rotationZ_);
}

void DisplayZoomController::HandleUpdateWindowZoomInfo(sptr<WindowNode> node)
{
    if (displayZoomWindowTypeSkipped_.find(node->GetWindowProperty()->GetWindowType()) !=
        displayZoomWindowTypeSkipped_.end()) {
        return;
    }
    Transform zoomTrans;
    if (node->GetWindowProperty()->IsAnimateWindow()) {
        zoomTrans = CalcuAnimateZoomTrans(node);
    } else {
        zoomTrans = CalcuZoomTransByZoomInfo(node);
    }
    UpdateClientAndSurfaceZoomInfo(node, zoomTrans);
}

void DisplayZoomController::TransformSurfaceNode(std::shared_ptr<RSSurfaceNode> surfaceNode, const Transform& trans)
{
    if (surfaceNode == nullptr) {
        return;
    }
    surfaceNode->SetPivotX(trans.pivotX_);
    surfaceNode->SetPivotY(trans.pivotY_);
    surfaceNode->SetScaleX(trans.scaleX_);
    surfaceNode->SetScaleY(trans.scaleY_);
    surfaceNode->SetTranslateX(trans.translateX_);
    surfaceNode->SetTranslateY(trans.translateY_);
    surfaceNode->SetTranslateZ(trans.translateZ_);
    surfaceNode->SetRotationX(trans.rotationX_);
    surfaceNode->SetRotationY(trans.rotationY_);
    surfaceNode->SetRotation(trans.rotationZ_);
}
}