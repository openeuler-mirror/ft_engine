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


#include "iremote_broker.h"
#include "window.h"
#include "window_property.h"
#include "wm_common.h"
#include "wm_common_inner.h"
#include <gmock/gmock.h>

namespace OHOS {
namespace Rosen {
class IWindowMocker : public IWindow {
public:
    IWindowMocker() {};
    ~IWindowMocker() {};
    MOCK_METHOD3(UpdateWindowRect, WMError(const struct Rect& rect, bool decoStatus, WindowSizeChangeReason reason));
    MOCK_METHOD1(UpdateWindowMode, WMError(WindowMode mode));
    MOCK_METHOD1(UpdateWindowModeSupportInfo, WMError(uint32_t modeSupportInfo));
    MOCK_METHOD1(UpdateFocusStatus, WMError(bool focused));
    MOCK_METHOD2(UpdateAvoidArea, WMError(const sptr<AvoidArea>& avoidArea, AvoidAreaType type));
    MOCK_METHOD1(UpdateWindowState, WMError(WindowState state));
    MOCK_METHOD2(UpdateWindowDragInfo, WMError(const PointInfo& point, DragEvent event));
    MOCK_METHOD2(UpdateDisplayId, WMError(DisplayId from, DisplayId to));
    MOCK_METHOD1(UpdateOccupiedAreaChangeInfo, WMError(const sptr<OccupiedAreaChangeInfo>& info));
    MOCK_METHOD1(UpdateActiveStatus, WMError(bool isActive));
    MOCK_METHOD0(GetWindowProperty, sptr<WindowProperty>());
    MOCK_METHOD0(NotifyTouchOutside, WMError());
    MOCK_METHOD0(NotifyScreenshot, WMError());
    MOCK_METHOD1(DumpInfo, WMError(const std::vector<std::string>& params));
    MOCK_METHOD0(NotifyDestroy, WMError(void));
    MOCK_METHOD0(NotifyForeground, WMError(void));
    MOCK_METHOD0(NotifyBackground, WMError(void));
    MOCK_METHOD1(NotifyWindowClientPointUp, WMError(const std::shared_ptr<MMI::PointerEvent>& pointerEvent));
    MOCK_METHOD2(UpdateZoomTransform, WMError(const Transform& trans, bool isDisplayZoomOn));
    MOCK_METHOD1(RestoreSplitWindowMode, WMError(uint32_t mode));
    sptr<IRemoteObject> AsObject() override
    {
        return nullptr;
    };
};
} // namespace Rosen
} // namespace OHOS
