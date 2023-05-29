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

#ifndef FRAMEWORKS_WM_TEST_UT_MOCK_WINDOW_ADAPTER_H
#define FRAMEWORKS_WM_TEST_UT_MOCK_WINDOW_ADAPTER_H
#include <gmock/gmock.h>

#include "window_adapter.h"

namespace OHOS {
namespace Rosen {
class MockWindowAdapter : public WindowAdapter {
public:
    MOCK_METHOD5(CreateWindow, WMError(sptr<IWindow>& window, sptr<WindowProperty>& windowProperty,
        std::shared_ptr<RSSurfaceNode> surfaceNode, uint32_t& windowId, const sptr<IRemoteObject>& token));
    MOCK_METHOD1(AddWindow, WMError(sptr<WindowProperty>& windowProperty));
    MOCK_METHOD1(RemoveWindow, WMError(uint32_t windowId));
    MOCK_METHOD0(ClearWindowAdapter, void());
    MOCK_METHOD1(DestroyWindow, WMError(uint32_t windowId));
    MOCK_METHOD2(UpdateProperty, WMError(sptr<WindowProperty>& windowProperty, PropertyChangeAction action));
    MOCK_METHOD2(GetTopWindowId, WMError(uint32_t mainWinId, uint32_t& topWinId));
    MOCK_METHOD3(GetAvoidAreaByType, WMError(uint32_t windowId, AvoidAreaType type, AvoidArea& avoidArea));
    MOCK_METHOD2(BindDialogTarget, WMError(uint32_t& windowId, sptr<IRemoteObject> targetToken));
    MOCK_METHOD2(UpdateRsTree, WMError(uint32_t windowId, bool isAdd));
    MOCK_METHOD1(MinimizeAllAppWindows, void(DisplayId displayId));
    MOCK_METHOD0(ToggleShownStateForAllAppWindows, WMError());
    MOCK_METHOD2(ProcessPointDown, void(uint32_t windowId, bool isPointDown));
    MOCK_METHOD1(ProcessPointUp, void(uint32_t windowId));
    MOCK_METHOD1(RequestFocus, WMError(uint32_t windowId));
    MOCK_METHOD2(NotifyWindowTransition, WMError(sptr<WindowTransitionInfo> from, sptr<WindowTransitionInfo> to));
    MOCK_METHOD2(GetModeChangeHotZones, WMError(DisplayId displayId, ModeChangeHotZones& hotZones));
    MOCK_METHOD3(NotifyServerReadyToMoveOrDrag, void(uint32_t windowId, sptr<WindowProperty>& windowProperty,
        sptr<MoveDragProperty>& moveDragProperty));
    MOCK_METHOD2(RegisterWindowManagerAgent, bool(WindowManagerAgentType type,
                                                const sptr<IWindowManagerAgent>& windowManagerAgent));
    MOCK_METHOD2(UnregisterWindowManagerAgent, bool(WindowManagerAgentType type,
                                                const sptr<IWindowManagerAgent>& windowManagerAgent));
    MOCK_METHOD1(GetVisibilityWindowInfo, WMError(std::vector<sptr<WindowVisibilityInfo>>& infos));
    MOCK_METHOD1(GetAccessibilityWindowInfo, WMError(std::vector<sptr<AccessibilityWindowInfo>>& infos));
};
}
} // namespace OHOS

#endif // FRAMEWORKS_WM_TEST_UT_MOCK_WINDOW_ADAPTER_H