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

#include "window_node.h"
#include "window_helper.h"
#include "window_manager_hilog.h"

namespace OHOS {
namespace Rosen {
namespace {
    constexpr HiviewDFX::HiLogLabel LABEL = {LOG_CORE, HILOG_DOMAIN_WINDOW, "WindowNode"};
}

WindowNode::~WindowNode()
{
    WLOGFI("~WindowNode id:%{public}u", GetWindowId());
}

void WindowNode::SetDisplayId(DisplayId displayId)
{
    property_->SetDisplayId(displayId);
}

void WindowNode::SetEntireWindowTouchHotArea(const Rect& rect)
{
    entireWindowTouchHotArea_ = rect;
}

void WindowNode::SetEntireWindowPointerHotArea(const Rect& rect)
{
    entireWindowPointerHotArea_ = rect;
}

void WindowNode::SetWindowRect(const Rect& rect)
{
    property_->SetWindowRect(rect);
}

void WindowNode::SetDecoStatus(bool status)
{
    property_->SetDecoStatus(status);
}

void WindowNode::SetRequestRect(const Rect& rect)
{
    property_->SetRequestRect(rect);
}

void WindowNode::SetWindowProperty(const sptr<WindowProperty>& property)
{
    property_ = property;
}

void WindowNode::SetSystemBarProperty(WindowType type, const SystemBarProperty& property)
{
    property_->SetSystemBarProperty(type, property);
}

void WindowNode::SetWindowMode(WindowMode mode)
{
    property_->SetWindowMode(mode);
}

void WindowNode::SetBrightness(float brightness)
{
    property_->SetBrightness(brightness);
}

void WindowNode::SetFocusable(bool focusable)
{
    property_->SetFocusable(focusable);
}

void WindowNode::SetTouchable(bool touchable)
{
    property_->SetTouchable(touchable);
}

void WindowNode::SetTurnScreenOn(bool turnScreenOn)
{
    property_->SetTurnScreenOn(turnScreenOn);
}

void WindowNode::SetKeepScreenOn(bool keepScreenOn)
{
    property_->SetKeepScreenOn(keepScreenOn);
}

void WindowNode::SetCallingWindow(uint32_t windowId)
{
    property_->SetCallingWindow(windowId);
}

uint32_t WindowNode::GetCallingWindow() const
{
    return property_->GetCallingWindow();
}

void WindowNode::SetWindowSizeChangeReason(WindowSizeChangeReason reason)
{
    windowSizeChangeReason_ = reason;
}

void WindowNode::SetRequestedOrientation(Orientation orientation)
{
    property_->SetRequestedOrientation(orientation);
}

void WindowNode::SetShowingDisplays(const std::vector<DisplayId>& displayIdVec)
{
    showingDisplays_.clear();
    showingDisplays_.assign(displayIdVec.begin(), displayIdVec.end());
}

void WindowNode::SetModeSupportInfo(uint32_t modeSupportInfo)
{
    property_->SetModeSupportInfo(modeSupportInfo);
}

void WindowNode::ResetWindowSizeChangeReason()
{
    windowSizeChangeReason_ = WindowSizeChangeReason::UNDEFINED;
}

const sptr<IWindow>& WindowNode::GetWindowToken() const
{
    return windowToken_;
}

void WindowNode::SetWindowToken(sptr<IWindow> window)
{
    windowToken_ = window;
}

void WindowNode::SetInputEventCallingPid(int32_t pid)
{
    inputCallingPid_ = pid;
}

void WindowNode::SetCallingPid(int32_t pid)
{
    callingPid_ = pid;
    SetInputEventCallingPid(pid);
}

void WindowNode::SetCallingUid(int32_t uid)
{
    callingUid_ = uid;
}

void WindowNode::SetDragType(DragType dragType)
{
    property_->SetDragType(dragType);
}

void WindowNode::SetOriginRect(const Rect& rect)
{
    property_->SetOriginRect(rect);
}

void WindowNode::SetTouchHotAreas(const std::vector<Rect>& rects)
{
    touchHotAreas_ = rects;
}

void WindowNode::SetPointerHotAreas(const std::vector<Rect>& rects)
{
    pointerHotAreas_ = rects;
}

void WindowNode::SetWindowSizeLimits(const WindowSizeLimits& sizeLimits)
{
    property_->SetSizeLimits(sizeLimits);
}

void WindowNode::SetWindowUpdatedSizeLimits(const WindowSizeLimits& sizeLimits)
{
    property_->SetUpdatedSizeLimits(sizeLimits);
}

void WindowNode::ComputeTransform()
{
    property_->ComputeTransform();
}

void WindowNode::SetTransform(const Transform& trans)
{
    property_->SetTransform(trans);
}

Transform WindowNode::GetZoomTransform() const
{
    return property_->GetZoomTransform();
}

void WindowNode::UpdateZoomTransform(const Transform& trans, bool isDisplayZoomOn)
{
    property_->SetZoomTransform(trans);
    property_->SetDisplayZoomState(isDisplayZoomOn);
    if (windowToken_) {
        windowToken_->UpdateZoomTransform(trans, isDisplayZoomOn);
    }
}

WindowSizeLimits WindowNode::GetWindowSizeLimits() const
{
    return property_->GetSizeLimits();
}

WindowSizeLimits WindowNode::GetWindowUpdatedSizeLimits() const
{
    return property_->GetUpdatedSizeLimits();
}

DragType WindowNode::GetDragType() const
{
    return property_->GetDragType();
}

bool WindowNode::GetStretchable() const
{
    return property_->GetStretchable();
}

const Rect& WindowNode::GetOriginRect() const
{
    return property_->GetOriginRect();
}

DisplayId WindowNode::GetDisplayId() const
{
    return property_->GetDisplayId();
}

const std::string& WindowNode::GetWindowName() const
{
    return property_->GetWindowName();
}

uint32_t WindowNode::GetWindowId() const
{
    return property_->GetWindowId();
}

uint32_t WindowNode::GetParentId() const
{
    return property_->GetParentId();
}

Rect WindowNode::GetEntireWindowTouchHotArea() const
{
    return entireWindowTouchHotArea_;
}

Rect WindowNode::GetEntireWindowPointerHotArea() const
{
    return entireWindowPointerHotArea_;
}

Rect WindowNode::GetWindowRect() const
{
    return property_->GetWindowRect();
}

bool WindowNode::GetDecoStatus() const
{
    return property_->GetDecoStatus();
}

Rect WindowNode::GetRequestRect() const
{
    return property_->GetRequestRect();
}

WindowType WindowNode::GetWindowType() const
{
    return property_->GetWindowType();
}

WindowMode WindowNode::GetWindowMode() const
{
    return property_->GetWindowMode();
}

bool WindowNode::EnableDefaultAnimation(bool animationPlayed)
{
    // system config enabled && not in remote animation && not custom animation && not crash
    bool defaultAnimation = property_->GetAnimationFlag() == (static_cast<uint32_t>(WindowAnimation::DEFAULT));
    return ((!animationPlayed) && (defaultAnimation) && (!isAppCrash_));
}

float WindowNode::GetBrightness() const
{
    return property_->GetBrightness();
}

bool WindowNode::IsTurnScreenOn() const
{
    return property_->IsTurnScreenOn();
}

bool WindowNode::IsKeepScreenOn() const
{
    return property_->IsKeepScreenOn();
}

uint32_t WindowNode::GetWindowFlags() const
{
    return property_->GetWindowFlags();
}

const sptr<WindowProperty>& WindowNode::GetWindowProperty() const
{
    return property_;
}

int32_t WindowNode::GetInputEventCallingPid() const
{
    return inputCallingPid_;
}

int32_t WindowNode::GetCallingPid() const
{
    return callingPid_;
}

int32_t WindowNode::GetCallingUid() const
{
    return callingUid_;
}

const std::unordered_map<WindowType, SystemBarProperty>& WindowNode::GetSystemBarProperty() const
{
    return property_->GetSystemBarProperty();
}

bool WindowNode::IsSplitMode() const
{
    return (property_->GetWindowMode() == WindowMode::WINDOW_MODE_SPLIT_PRIMARY ||
        property_->GetWindowMode() == WindowMode::WINDOW_MODE_SPLIT_SECONDARY);
}

WindowSizeChangeReason WindowNode::GetWindowSizeChangeReason() const
{
    return windowSizeChangeReason_;
}

Orientation WindowNode::GetRequestedOrientation() const
{
    return property_->GetRequestedOrientation();
}

std::vector<DisplayId> WindowNode::GetShowingDisplays() const
{
    return showingDisplays_;
}

uint32_t WindowNode::GetModeSupportInfo() const
{
    return property_->GetModeSupportInfo();
}

void WindowNode::GetTouchHotAreas(std::vector<Rect>& rects) const
{
    rects = touchHotAreas_;
}

void WindowNode::GetPointerHotAreas(std::vector<Rect>& rects) const
{
    rects = pointerHotAreas_;
}

uint32_t WindowNode::GetAccessTokenId() const
{
    return property_->GetAccessTokenId();
}

void WindowNode::SetSnapshot(std::shared_ptr<Media::PixelMap> pixelMap)
{
    snapshot_ = pixelMap;
}

std::shared_ptr<Media::PixelMap> WindowNode::GetSnapshot()
{
    return snapshot_;
}
} // namespace Rosen
} // namespace OHOS
