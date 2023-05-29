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
#ifndef OHOS_ROSEN_WINDOW_ROOT_H
#define OHOS_ROSEN_WINDOW_ROOT_H

#include <refbase.h>
#include <iremote_object.h>
#include <transaction/rs_interfaces.h>

#include "agent_death_recipient.h"
#include "display_manager_service_inner.h"
#include "parameters.h"
#include "window_node_container.h"
#include "zidl/window_manager_agent_interface.h"

namespace OHOS {
namespace Rosen {
enum class Event : uint32_t {
    REMOTE_DIED,
};

class WindowRoot : public RefBase {
using Callback = std::function<void (Event event, const sptr<IRemoteObject>& remoteObject)>;

public:
    explicit WindowRoot(Callback callback) : callback_(callback) {}
    ~WindowRoot() = default;

    sptr<WindowNodeContainer> GetOrCreateWindowNodeContainer(DisplayId displayId);
    sptr<WindowNodeContainer> GetWindowNodeContainer(DisplayId displayId);
    sptr<WindowNodeContainer> CreateWindowNodeContainer(sptr<DisplayInfo> displayInfo);
    sptr<WindowNode> GetWindowNode(uint32_t windowId) const;
    void GetBackgroundNodesByScreenId(ScreenId screenGroupId, std::vector<sptr<WindowNode>>& windowNodes) const;

    WMError SaveWindow(const sptr<WindowNode>& node);
    void AddDeathRecipient(sptr<WindowNode> node);
    sptr<WindowNode> FindWindowNodeWithToken(const sptr<IRemoteObject>& token) const;
    WMError AddWindowNode(uint32_t parentId, sptr<WindowNode>& node, bool fromStartingWin = false);
    WMError RemoveWindowNode(uint32_t windowId, bool fromAnimation = false);
    WMError DestroyWindow(uint32_t windowId, bool onlySelf);
    WMError UpdateWindowNode(uint32_t windowId, WindowUpdateReason reason);
    bool IsVerticalDisplay(sptr<WindowNode>& node) const;
    bool IsForbidDockSliceMove(DisplayId displayId) const;
    bool IsDockSliceInExitSplitModeArea(DisplayId displayId) const;
    void ExitSplitMode(DisplayId displayId);
    void NotifyWindowVisibilityChange(std::shared_ptr<RSOcclusionData> occlusionData);
    void AddSurfaceNodeIdWindowNodePair(uint64_t surfaceNodeId, sptr<WindowNode> node);

    WMError RequestFocus(uint32_t windowId);
    WMError RequestActiveWindow(uint32_t windowId);
    WMError MinimizeStructuredAppWindowsExceptSelf(sptr<WindowNode>& node);
    AvoidArea GetAvoidAreaByType(uint32_t windowId, AvoidAreaType avoidAreaType);
    WMError SetWindowMode(sptr<WindowNode>& node, WindowMode dstMode);
    WMError GetTopWindowId(uint32_t mainWinId, uint32_t& topWinId);
    void MinimizeAllAppWindows(DisplayId displayId);
    WMError ToggleShownStateForAllAppWindows();
    WMError SetWindowLayoutMode(DisplayId displayId, WindowLayoutMode mode);

    void ProcessWindowStateChange(WindowState state, WindowStateChangeReason reason);
    void ProcessDisplayChange(DisplayId defaultDisplayId, sptr<DisplayInfo> displayInfo,
        const std::map<DisplayId, sptr<DisplayInfo>>& displayInfoMap, DisplayStateChangeType type);
    void ProcessDisplayDestroy(DisplayId defaultDisplayId, sptr<DisplayInfo> displayInfo,
        const std::map<DisplayId, sptr<DisplayInfo>>& displayInfoMap);
    void ProcessDisplayCreate(DisplayId defaultDisplayId, sptr<DisplayInfo> displayInfo,
        const std::map<DisplayId, sptr<DisplayInfo>>& displayInfoMap);

    void NotifySystemBarTints();
    WMError RaiseZOrderForAppWindow(sptr<WindowNode>& node);
    void FocusFaultDetection() const;
    float GetVirtualPixelRatio(DisplayId displayId) const;
    Rect GetDisplayGroupRect(DisplayId displayId) const;
    WMError UpdateSizeChangeReason(uint32_t windowId, WindowSizeChangeReason reason);
    void SetBrightness(uint32_t windowId, float brightness);
    void HandleKeepScreenOn(uint32_t windowId, bool requireLock);
    void UpdateFocusableProperty(uint32_t windowId);
    void SetMaxAppWindowNumber(uint32_t windowNum);
    void SetMaxUniRenderAppWindowNumber(uint32_t uniAppWindowNum);
    uint32_t GetMaxUniRenderAppWindowNumber() const;
    WMError GetModeChangeHotZones(DisplayId displayId,
        ModeChangeHotZones& hotZones, const ModeChangeHotZonesConfig& config);
    std::vector<DisplayId> GetAllDisplayIds() const;
    uint32_t GetTotalWindowNum() const;
    uint32_t GetWindowIdByObject(const sptr<IRemoteObject>& remoteObject);
    sptr<WindowNode> GetWindowForDumpAceHelpInfo() const;
    void DestroyLeakStartingWindow();
    void SetSplitRatios(const std::vector<float>& splitRatioNumbers);
    std::vector<sptr<WindowNode>> GetSplitScreenWindowNodes(DisplayId displayId);
    void SetExitSplitRatios(const std::vector<float>& exitSplitRatios);
    void MinimizeTargetWindows(std::vector<uint32_t>& windowIds);
    WMError UpdateRsTree(uint32_t windowId, bool isAdd);
    void RemoveSingleUserWindowNodes(int accountId);
    sptr<WindowNode> FindDialogCallerNode(WindowType type, sptr<IRemoteObject> token);
    bool CheckMultiDialogWindows(WindowType type, sptr<IRemoteObject> token);
    bool HasPrivateWindow(DisplayId displayId);
    Rect GetDisplayRectWithoutSystemBarAreas(DisplayId displayId);
    void SwitchRenderModeIfNeeded();
    void OnRenderModeChanged(bool isUniRender);
    sptr<WindowNode> GetWindowNodeByAbilityToken(const sptr<IRemoteObject>& abilityToken);
    bool TakeWindowPairSnapshot(DisplayId displayId);
    void ClearWindowPairSnapshot(DisplayId displayId);
    bool IsUniRender()
    {
        return renderMode_ == RenderMode::UNIFIED;
    }
    void LayoutWhenAddWindowNode(sptr<WindowNode>& node, bool afterAnimation = false);
    void GetAllAnimationPlayingNodes(std::vector<wptr<WindowNode>>& windowNodes);
    void GetVisibilityWindowInfo(std::vector<sptr<WindowVisibilityInfo>>& infos) const;
private:
    enum class RenderMode : uint8_t {
        SEPARATED,
        UNIFIED,
        SEPARATING,
        UNIFYING,
    };

    void OnRemoteDied(const sptr<IRemoteObject>& remoteObject);
    WMError DestroyWindowInner(sptr<WindowNode>& node);
    WMError DestroyWindowSelf(sptr<WindowNode>& node, const sptr<WindowNodeContainer>& container);
    WMError DestroyWindowWithChild(sptr<WindowNode>& node, const sptr<WindowNodeContainer>& container);
    void UpdateFocusWindowWithWindowRemoved(const sptr<WindowNode>& node,
        const sptr<WindowNodeContainer>& container) const;
    void UpdateActiveWindowWithWindowRemoved(const sptr<WindowNode>& node,
        const sptr<WindowNodeContainer>& container) const;
    void UpdateBrightnessWithWindowRemoved(uint32_t windowId, const sptr<WindowNodeContainer>& container) const;
    std::string GenAllWindowsLogInfo() const;
    bool CheckDisplayInfo(const sptr<DisplayInfo>& display);
    ScreenId GetScreenGroupId(DisplayId displayId, bool& isRecordedDisplay);
    void ProcessExpandDisplayCreate(DisplayId defaultDisplayId, sptr<DisplayInfo> displayInfo,
        std::map<DisplayId, Rect>& displayRectMap);
    std::map<DisplayId, sptr<DisplayInfo>> GetAllDisplayInfos(const std::vector<DisplayId>& displayIdVec);
    std::map<DisplayId, Rect> GetAllDisplayRectsByDMS(sptr<DisplayInfo> displayInfo);
    std::map<DisplayId, Rect> GetAllDisplayRectsByDisplayInfo(
        const std::map<DisplayId, sptr<DisplayInfo>>& displayInfoMap);
    void MoveNotShowingWindowToDefaultDisplay(DisplayId defaultDisplayId, DisplayId displayId);
    WMError PostProcessAddWindowNode(sptr<WindowNode>& node, sptr<WindowNode>& parentNode,
        sptr<WindowNodeContainer>& container);
    std::vector<std::pair<uint64_t, bool>> GetWindowVisibilityChangeInfo(
        std::shared_ptr<RSOcclusionData> occlusionData);
    bool NeedToStopAddingNode(sptr<WindowNode>& node, const sptr<WindowNodeContainer>& container);
    void ChangeRSRenderModeIfNeeded(bool isToUnified);
    bool IsAppWindowExceed() const;

    std::map<uint32_t, sptr<WindowNode>> windowNodeMap_;
    std::map<sptr<IRemoteObject>, uint32_t> windowIdMap_;
    std::map<uint64_t, sptr<WindowNode>> surfaceIdWindowNodeMap_;
    std::shared_ptr<RSOcclusionData> lastOcclusionData_ = std::make_shared<RSOcclusionData>();
    std::map<ScreenId, sptr<WindowNodeContainer>> windowNodeContainerMap_;
    std::map<ScreenId, std::vector<DisplayId>> displayIdMap_;

    bool needCheckFocusWindow = false;

    std::map<WindowManagerAgentType, std::vector<sptr<IWindowManagerAgent>>> windowManagerAgents_;

    sptr<AgentDeathRecipient> windowDeath_ = new AgentDeathRecipient(std::bind(&WindowRoot::OnRemoteDied,
        this, std::placeholders::_1));
    Callback callback_;
    uint32_t maxAppWindowNumber_ = 100;
    uint32_t maxUniRenderAppWindowNumber_ { maxAppWindowNumber_ };
    SplitRatioConfig splitRatioConfig_ = {0.1, 0.9, {}};
    RenderMode renderMode_ { RenderMode::UNIFIED };
};
}
}
#endif // OHOS_ROSEN_WINDOW_ROOT_H
