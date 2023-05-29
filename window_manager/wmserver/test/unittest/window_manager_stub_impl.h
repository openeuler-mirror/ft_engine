/*
 * Copyright (c) 2022-2022 Huawei Device Co., Ltd.
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

#include "window_manager_stub.h"
#include "window_manager_agent.h"
#include <rs_iwindow_animation_controller.h>
#include "window_impl.h"
#include "window_agent.h"

namespace OHOS {
namespace Rosen {
class WindowManagerStubImpl : public WindowManagerStub {
WMError CreateWindow(sptr<IWindow>& window, sptr<WindowProperty>& property,
    const std::shared_ptr<RSSurfaceNode>& surfaceNode,
    uint32_t& windowId, sptr<IRemoteObject> token)
{
    return WMError::WM_OK;
};
WMError AddWindow(sptr<WindowProperty>& property)
{
    return WMError::WM_OK;
};
WMError RemoveWindow(uint32_t windowId)
{
    return WMError::WM_OK;
};
WMError DestroyWindow(uint32_t windowId, bool onlySelf = false)
{
    return WMError::WM_OK;
};
WMError RequestFocus(uint32_t windowId)
{
    return WMError::WM_OK;
};
AvoidArea GetAvoidAreaByType(uint32_t windowId, AvoidAreaType type)
{
    AvoidArea area;
    return area;
};
WMError GetTopWindowId(uint32_t mainWinId, uint32_t& topWinId)
{
    return WMError::WM_OK;
};
void NotifyServerReadyToMoveOrDrag(uint32_t windowId, sptr<WindowProperty>& windowProperty,
    sptr<MoveDragProperty>& moveDragProperty){};
void ProcessPointDown(uint32_t windowId, bool isPointDown){};
void ProcessPointUp(uint32_t windowId){};
void MinimizeAllAppWindows(DisplayId displayId){};
WMError ToggleShownStateForAllAppWindows()
{
    return WMError::WM_OK;
};
WMError SetWindowLayoutMode(WindowLayoutMode mode)
{
    return WMError::WM_OK;
};
WMError UpdateProperty(sptr<WindowProperty>& windowProperty, PropertyChangeAction action,
    bool isAsyncTask = false)
{
    return WMError::WM_OK;
};
bool RegisterWindowManagerAgent(WindowManagerAgentType type,
    const sptr<IWindowManagerAgent>& windowManagerAgent)
{
    return true;
};
bool UnregisterWindowManagerAgent(WindowManagerAgentType type,
    const sptr<IWindowManagerAgent>& windowManagerAgent)
{
    return true;
};
WMError GetAccessibilityWindowInfo(std::vector<sptr<AccessibilityWindowInfo>>& infos)
{
    return WMError::WM_OK;
};
WMError GetVisibilityWindowInfo(std::vector<sptr<WindowVisibilityInfo>>& infos)
{
    return WMError::WM_OK;
};
WMError SetWindowAnimationController(const sptr<RSIWindowAnimationController>& controller)
{
    return WMError::WM_OK;
};
WMError GetSystemConfig(SystemConfig& systemConfig)
{
    return WMError::WM_OK;
};
WMError NotifyWindowTransition(sptr<WindowTransitionInfo>& from, sptr<WindowTransitionInfo>& to,
    bool isFromClient = false)
{
    return WMError::WM_OK;
};
WMError GetModeChangeHotZones(DisplayId displayId, ModeChangeHotZones& hotZones)
{
    return WMError::WM_OK;
};
void MinimizeWindowsByLauncher(std::vector<uint32_t> windowIds, bool isAnimated,
    sptr<RSIWindowAnimationFinishedCallback>& finishCallback){};
WMError UpdateAvoidAreaListener(uint32_t windowId, bool haveListener)
{
    return WMError::WM_OK;
};
WMError UpdateRsTree(uint32_t windowId, bool isAdd)
{
    return WMError::WM_OK;
};
WMError BindDialogTarget(uint32_t& windowId, sptr<IRemoteObject> targetToken)
{
    return WMError::WM_OK;
};
void SetAnchorAndScale(int32_t x, int32_t y, float scale){};
void SetAnchorOffset(int32_t deltaX, int32_t deltaY){};
void OffWindowZoom(){};
std::shared_ptr<Media::PixelMap> GetSnapshot(int32_t windowId) override
{
    return nullptr;
}
};
}
}