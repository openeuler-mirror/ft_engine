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

#include "window_agent.h"
#include "window_manager_hilog.h"
#include "wm_common.h"

namespace OHOS {
namespace Rosen {
namespace {
    constexpr HiviewDFX::HiLogLabel LABEL = {LOG_CORE, HILOG_DOMAIN_WINDOW, "WindowAgent"};
}

WindowAgent::WindowAgent(sptr<WindowImpl>& windowImpl)
{
    window_ = windowImpl;
}

WMError WindowAgent::UpdateWindowRect(const struct Rect& rect, bool decoStatus, WindowSizeChangeReason reason)
{
    if (window_ == nullptr) {
        WLOGFE("window_ is nullptr");
        return WMError::WM_ERROR_NULLPTR;
    }
    window_->UpdateRect(rect, decoStatus, reason);
    return WMError::WM_OK;
}

WMError WindowAgent::UpdateWindowMode(WindowMode mode)
{
    if (window_ == nullptr) {
        WLOGFE("window_ is nullptr");
        return WMError::WM_ERROR_NULLPTR;
    }
    window_->UpdateMode(mode);
    return WMError::WM_OK;
}

WMError WindowAgent::UpdateWindowModeSupportInfo(uint32_t modeSupportInfo)
{
    if (window_ == nullptr) {
        WLOGFE("window_ is nullptr");
        return WMError::WM_ERROR_NULLPTR;
    }
    window_->UpdateModeSupportInfo(modeSupportInfo);
    return WMError::WM_OK;
}

WMError WindowAgent::UpdateFocusStatus(bool focused)
{
    if (window_ == nullptr) {
        WLOGFE("window_ is nullptr");
        return WMError::WM_ERROR_NULLPTR;
    }
    window_->UpdateFocusStatus(focused);
    return WMError::WM_OK;
}

WMError WindowAgent::UpdateAvoidArea(const sptr<AvoidArea>& avoidArea, AvoidAreaType type)
{
    if (window_ == nullptr || avoidArea == nullptr) {
        WLOGFE("window_ or avoidArea is nullptr.");
        return WMError::WM_ERROR_NULLPTR;
    }
    window_->UpdateAvoidArea(avoidArea, type);
    return WMError::WM_OK;
}

WMError WindowAgent::UpdateWindowState(WindowState state)
{
    if (window_ == nullptr) {
        WLOGFE("window_ is nullptr");
        return WMError::WM_ERROR_NULLPTR;
    }
    window_->UpdateWindowState(state);
    return WMError::WM_OK;
}

WMError WindowAgent::UpdateWindowDragInfo(const PointInfo& point, DragEvent event)
{
    if (window_ == nullptr) {
        WLOGFE("window is null");
        return WMError::WM_ERROR_NULLPTR;
    }
    window_->UpdateDragEvent(point, event);
    return WMError::WM_OK;
}

WMError WindowAgent::UpdateDisplayId(DisplayId from, DisplayId to)
{
    if (window_ == nullptr) {
        WLOGFE("window is null");
        return WMError::WM_ERROR_NULLPTR;
    }
    window_->UpdateDisplayId(from, to);
    return WMError::WM_OK;
}

WMError WindowAgent::UpdateOccupiedAreaChangeInfo(const sptr<OccupiedAreaChangeInfo>& info)
{
    if (window_ == nullptr) {
        WLOGFE("window is null");
        return WMError::WM_ERROR_NULLPTR;
    }
    window_->UpdateOccupiedAreaChangeInfo(info);
    return WMError::WM_OK;
}

WMError WindowAgent::UpdateActiveStatus(bool isActive)
{
    if (window_ == nullptr) {
        WLOGFE("window is null");
        return WMError::WM_ERROR_NULLPTR;
    }
    window_->UpdateActiveStatus(isActive);
    return WMError::WM_OK;
}

sptr<WindowProperty> WindowAgent::GetWindowProperty()
{
    if (window_ == nullptr) {
        WLOGFE("window_ is nullptr");
        return nullptr;
    }
    return window_->GetWindowProperty();
}

WMError WindowAgent::RestoreSplitWindowMode(uint32_t mode)
{
    if (window_ == nullptr) {
        WLOGFE("window_ is nullptr");
        return WMError::WM_ERROR_NULLPTR;
    }
    window_->RestoreSplitWindowMode(mode);
    return WMError::WM_OK;
}

WMError WindowAgent::NotifyTouchOutside()
{
    if (window_ == nullptr) {
        WLOGFI("window is null");
        return WMError::WM_ERROR_NULLPTR;
    }
    WLOGFD("called");
    window_->NotifyTouchOutside();
    return WMError::WM_OK;
}

WMError WindowAgent::NotifyScreenshot()
{
    if (window_ == nullptr) {
        WLOGFE("notify screenshot failed: window is null.");
        return WMError::WM_ERROR_NULLPTR;
    }
    WLOGFI("called");
    window_->NotifyScreenshot();
    return WMError::WM_OK;
}

WMError WindowAgent::DumpInfo(const std::vector<std::string>& params)
{
    if (window_ == nullptr) {
        WLOGFE("window_ is nullptr");
        return WMError::WM_ERROR_NULLPTR;
    }
    std::vector<std::string> info;
    window_->DumpInfo(params, info);
    return WMError::WM_OK;
}

WMError WindowAgent::UpdateZoomTransform(const Transform& trans, bool isDisplayZoomOn)
{
    if (window_ == nullptr) {
        WLOGFE("window_ is nullptr");
        return WMError::WM_ERROR_NULLPTR;
    }
    window_->UpdateZoomTransform(trans, isDisplayZoomOn);
    return WMError::WM_OK;
}

WMError WindowAgent::NotifyDestroy(void)
{
    if (window_ == nullptr) {
        WLOGFE("window_ is nullptr");
        return WMError::WM_ERROR_NULLPTR;
    }
    window_->NotifyDestroy();
    return WMError::WM_OK;
}

WMError WindowAgent::NotifyForeground(void)
{
    if (window_ == nullptr) {
        WLOGFE("window_ is nullptr");
        return WMError::WM_ERROR_NULLPTR;
    }
    window_->NotifyForeground();
    return WMError::WM_OK;
}

WMError WindowAgent::NotifyBackground(void)
{
    if (window_ == nullptr) {
        WLOGFE("window_ is nullptr");
        return WMError::WM_ERROR_NULLPTR;
    }
    window_->NotifyBackground();
    return WMError::WM_OK;
}

WMError WindowAgent::NotifyWindowClientPointUp(const std::shared_ptr<MMI::PointerEvent>& pointerEvent)
{
    if (window_ == nullptr) {
        WLOGFE("window_ is nullptr");
        return WMError::WM_ERROR_NULLPTR;
    }
    window_->ConsumePointerEvent(pointerEvent);
    return WMError::WM_OK;
}
} // namespace Rosen
} // namespace OHOS
