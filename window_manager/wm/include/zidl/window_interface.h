/*
 * Copyright (c) 2021 Huawei Device Co., Ltd.
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

#ifndef OHOS_WINDOW_INTERFACE_H
#define OHOS_WINDOW_INTERFACE_H

#include "iremote_broker.h"
#include "window.h"
#include "window_property.h"
#include "wm_common.h"
#include "wm_common_inner.h"

namespace OHOS {
namespace Rosen {
class IWindow : public IRemoteBroker {
public:
    DECLARE_INTERFACE_DESCRIPTOR(u"OHOS.IWindow");

    enum class WindowMessage : uint32_t {
        TRANS_ID_UPDATE_WINDOW_RECT,
        TRANS_ID_UPDATE_WINDOW_MODE,
        TRANS_ID_UPDATE_MODE_SUPPORT_INFO,
        TRANS_ID_UPDATE_FOCUS_STATUS,
        TRANS_ID_UPDATE_AVOID_AREA,
        TRANS_ID_UPDATE_WINDOW_STATE,
        TRANS_ID_UPDATE_DRAG_EVENT,
        TRANS_ID_UPDATE_DISPLAY_ID,
        TRANS_ID_UPDATE_OCCUPIED_AREA,
        TRANS_ID_UPDATE_ACTIVE_STATUS,
        TRANS_ID_GET_WINDOW_PROPERTY,
        TRANS_ID_NOTIFY_OUTSIDE_PRESSED,
        TRANS_ID_NOTIFY_SCREEN_SHOT,
        TRANS_ID_DUMP_INFO,
        TRANS_ID_NOTIFY_DESTROY,
        TRANS_ID_NOTIFY_FOREGROUND,
        TRANS_ID_NOTIFY_BACKGROUND,
        TRANS_ID_NOTIFY_CLIENT_POINT_UP,
        TRANS_ID_UPDATE_ZOOM_TRANSFORM,
        TRANS_ID_RESTORE_SPLIT_WINDOW_MODE,
    };

    virtual WMError UpdateWindowRect(const struct Rect& rect, bool decoStatus, WindowSizeChangeReason reason) = 0;
    virtual WMError UpdateWindowMode(WindowMode mode) = 0;
    virtual WMError UpdateWindowModeSupportInfo(uint32_t modeSupportInfo) = 0;
    virtual WMError UpdateFocusStatus(bool focused) = 0;
    virtual WMError UpdateAvoidArea(const sptr<AvoidArea>& avoidArea, AvoidAreaType type) = 0;
    virtual WMError UpdateWindowState(WindowState state) = 0;
    virtual WMError UpdateWindowDragInfo(const PointInfo& point, DragEvent event) = 0;
    virtual WMError UpdateDisplayId(DisplayId from, DisplayId to) = 0;
    virtual WMError UpdateOccupiedAreaChangeInfo(const sptr<OccupiedAreaChangeInfo>& info) = 0;
    virtual WMError UpdateActiveStatus(bool isActive) = 0;
    virtual sptr<WindowProperty> GetWindowProperty() = 0;
    virtual WMError NotifyTouchOutside() = 0;
    virtual WMError NotifyScreenshot() = 0;
    virtual WMError DumpInfo(const std::vector<std::string>& params) = 0;
    virtual WMError NotifyDestroy(void) = 0;
    virtual WMError NotifyForeground(void) = 0;
    virtual WMError NotifyBackground(void) = 0;
    virtual WMError NotifyWindowClientPointUp(const std::shared_ptr<MMI::PointerEvent>& pointerEvent) = 0;
    virtual WMError UpdateZoomTransform(const Transform& trans, bool isDisplayZoomOn) = 0;
    virtual WMError RestoreSplitWindowMode(uint32_t mode) = 0;
};
} // namespace Rosen
} // namespace OHOS
#endif // OHOS_WINDOW_INTERFACE_H
