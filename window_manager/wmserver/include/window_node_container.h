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

#ifndef OHOS_ROSEN_WINDOW_NODE_CONTAINER_H
#define OHOS_ROSEN_WINDOW_NODE_CONTAINER_H

#include "animation_config.h"
#include "avoid_area_controller.h"
#include "display_info.h"
#include "minimize_app.h"
#include "display_group_controller.h"
#include "display_group_info.h"
#include "window_layout_policy.h"
#include "window_manager.h"
#include "window_node.h"
#include "window_zorder_policy.h"
#include "wm_common.h"
#include "wm_common_inner.h"
#include "window_pair.h"

namespace OHOS {
namespace Rosen {
using WindowNodeOperationFunc = std::function<bool(sptr<WindowNode>)>; // return true indicates to stop traverse
class WindowNodeContainer : public RefBase {
public:
    WindowNodeContainer(const sptr<DisplayInfo>& displayInfo, ScreenId displayGroupId);
    ~WindowNodeContainer();
    WMError ShowStartingWindow(sptr<WindowNode>& node);
    WMError AddWindowNode(sptr<WindowNode>& node, sptr<WindowNode>& parentNode, bool afterAnimation = false);
    WMError RemoveWindowNode(sptr<WindowNode>& node, bool fromAnimation = false);
    WMError HandleRemoveWindow(sptr<WindowNode>& node);
    WMError UpdateWindowNode(sptr<WindowNode>& node, WindowUpdateReason reason);
    WMError DestroyWindowNode(sptr<WindowNode>& node, std::vector<uint32_t>& windowIds);
    const std::vector<uint32_t>& Destroy();
    void AssignZOrder();
    WMError SetFocusWindow(uint32_t windowId);
    uint32_t GetFocusWindow() const;
    WMError SetActiveWindow(uint32_t windowId, bool byRemoved);
    uint32_t GetActiveWindow() const;
    void SetDisplayBrightness(float brightness);
    float GetDisplayBrightness() const;
    void SetBrightnessWindow(uint32_t windowId);
    uint32_t GetBrightnessWindow() const;
    uint32_t ToOverrideBrightness(float brightness);
    void UpdateBrightness(uint32_t id, bool byRemoved);
    void HandleKeepScreenOn(const sptr<WindowNode>& node, bool requireLock);
    AvoidArea GetAvoidAreaByType(const sptr<WindowNode>& node, AvoidAreaType avoidAreaType) const;
    WMError MinimizeStructuredAppWindowsExceptSelf(const sptr<WindowNode>& node);
    void TraverseContainer(std::vector<sptr<WindowNode>>& windowNodes) const;
    Rect GetDisplayRect(DisplayId displayId) const;
    std::unordered_map<WindowType, SystemBarProperty> GetExpectImmersiveProperty() const;
    uint32_t GetWindowCountByType(WindowType windowType);
    bool IsForbidDockSliceMove(DisplayId displayId) const;
    bool IsDockSliceInExitSplitModeArea(DisplayId displayId) const;
    void ExitSplitMode(DisplayId displayId);

    bool IsVerticalDisplay(DisplayId displayId) const;
    WMError RaiseZOrderForAppWindow(sptr<WindowNode>& node, sptr<WindowNode>& parentNode);
    sptr<WindowNode> GetNextFocusableWindow(uint32_t windowId) const;
    sptr<WindowNode> GetNextRotatableWindow(uint32_t windowId) const;
    sptr<WindowNode> GetNextActiveWindow(uint32_t windowId) const;
    void MinimizeAllAppWindows(DisplayId displayId);
    void MinimizeOldestAppWindow();
    WMError ToggleShownStateForAllAppWindows(std::function<bool(uint32_t, WindowMode)> restoreFunc, bool restore);
    void BackUpAllAppWindows();
    void RestoreAllAppWindows(std::function<bool(uint32_t, WindowMode)> restoreFunc);
    bool IsAppWindowsEmpty() const;
    void SetSurfaceNodeVisible(sptr<WindowNode>& node, int32_t topPriority, bool visible);
    void SetBelowScreenlockVisible(sptr<WindowNode>& node, bool visible);
    void ProcessWindowStateChange(WindowState state, WindowStateChangeReason reason);
    void NotifySystemBarTints(std::vector<DisplayId> displayIdVec);
    WMError MinimizeAppNodeExceptOptions(MinimizeReason reason, const std::vector<uint32_t> &exceptionalIds = {},
                                         const std::vector<WindowMode> &exceptionalModes = {});
    WMError SetWindowMode(sptr<WindowNode>& node, WindowMode dstMode);
    WMError SwitchLayoutPolicy(WindowLayoutMode mode, DisplayId displayId, bool reorder = false);
    void RaiseSplitRelatedWindowToTop(sptr<WindowNode>& node);
    float GetVirtualPixelRatio(DisplayId displayId) const;
    Rect GetDisplayGroupRect() const;
    void TraverseWindowTree(const WindowNodeOperationFunc& func, bool isFromTopToBottom = true) const;
    void UpdateSizeChangeReason(sptr<WindowNode>& node, WindowSizeChangeReason reason);
    void DropShowWhenLockedWindowIfNeeded(const sptr<WindowNode>& node);

    void SetMinimizedByOther(bool isMinimizedByOther);
    void GetModeChangeHotZones(DisplayId displayId,
        ModeChangeHotZones& hotZones, const ModeChangeHotZonesConfig& config);
    sptr<DisplayInfo> GetDisplayInfo(DisplayId displayId);
    void UpdateDisplayInfo(sptr<DisplayInfo> displayInfo);
    std::vector<sptr<DisplayInfo>> GetAllDisplayInfo();
    float GetDisplayVirtualPixelRatio(DisplayId displayId) const;

    // parentDisplayId is the same as displayId in single-display mode
    bool AddNodeOnRSTree(sptr<WindowNode>& node, DisplayId displayId, DisplayId parentDisplayId,
        WindowUpdateType type, bool animationPlayed = false);
    // parentDisplayId is the same as displayId in single-display mode
    bool RemoveNodeFromRSTree(sptr<WindowNode>& node, DisplayId displayId, DisplayId parentDisplayId,
        WindowUpdateType type, bool animationPlayed = false);
    bool AddAppSurfaceNodeOnRSTree(sptr<WindowNode>& node);

    sptr<WindowLayoutPolicy> GetLayoutPolicy() const;
    sptr<AvoidAreaController> GetAvoidController() const;
    sptr<DisplayGroupController> GetMultiDisplayController() const;
    sptr<WindowNode> GetRootNode(WindowRootNodeType type) const;
    void NotifyDockWindowStateChanged(sptr<WindowNode>& node, bool isEnable);
    void UpdateCameraFloatWindowStatus(const sptr<WindowNode>& node, bool isShowing);
    void UpdateAvoidAreaListener(sptr<WindowNode>& windowNode, bool haveAvoidAreaListener);
    void BeforeProcessWindowAvoidAreaChangeWhenDisplayChange() const;
    void ProcessWindowAvoidAreaChangeWhenDisplayChange() const;
    WindowLayoutMode GetCurrentLayoutMode() const;
    void RemoveSingleUserWindowNodes(int accountId);
    WMError IsTileRectSatisfiedWithSizeLimits(sptr<WindowNode>& node);
    bool HasPrivateWindow();
    sptr<WindowNode> GetDeskTopWindow();
    static AnimationConfig& GetAnimationConfigRef();
    bool TakeWindowPairSnapshot(DisplayId displayId);
    void ClearWindowPairSnapshot(DisplayId displayId);
    bool IsScreenLocked();
    void LayoutWhenAddWindowNode(sptr<WindowNode>& node, bool afterAnimation = false);
private:
    void TraverseWindowNode(sptr<WindowNode>& root, std::vector<sptr<WindowNode>>& windowNodes) const;
    sptr<WindowNode> FindRoot(WindowType type) const;
    sptr<WindowNode> FindWindowNodeById(uint32_t id) const;
    void UpdateFocusStatus(uint32_t id, bool focused);
    void UpdateActiveStatus(uint32_t id, bool isActive);
    void NotifyIfAvoidAreaChanged(const sptr<WindowNode>& node, const AvoidControlType avoidType) const;
    void NotifyIfSystemBarTintChanged(DisplayId displayId) const;
    void NotifyIfSystemBarRegionChanged(DisplayId displayId) const;
    void NotifyIfKeyboardRegionChanged(const sptr<WindowNode>& node, const AvoidControlType avoidType) const;
    void TraverseAndUpdateWindowState(WindowState state, int32_t topPriority);
    void UpdateWindowTree(sptr<WindowNode>& node);
    void UpdateWindowState(sptr<WindowNode> node, int32_t topPriority, WindowState state);
    void HandleKeepScreenOn(const sptr<WindowNode>& node, WindowState state);
    bool IsTopWindow(uint32_t windowId, sptr<WindowNode>& rootNode) const;
    sptr<WindowNode> FindDividerNode() const;
    void RaiseWindowToTop(uint32_t windowId, std::vector<sptr<WindowNode>>& windowNodes);
    void ResetLayoutPolicy();
    bool IsAboveSystemBarNode(sptr<WindowNode> node) const;
    bool IsSplitImmersiveNode(sptr<WindowNode> node) const;
    bool TraverseFromTopToBottom(sptr<WindowNode> node, const WindowNodeOperationFunc& func) const;
    bool TraverseFromBottomToTop(sptr<WindowNode> node, const WindowNodeOperationFunc& func) const;
    void RaiseOrderedWindowToTop(std::vector<sptr<WindowNode>>& orderedNodes,
        std::vector<sptr<WindowNode>>& windowNodes);
    void DumpScreenWindowTree();
    void RaiseInputMethodWindowPriorityIfNeeded(const sptr<WindowNode>& node) const;
    void ReZOrderShowWhenLockedWindowIfNeeded(const sptr<WindowNode>& node);
    void RaiseShowWhenLockedWindowIfNeeded(const sptr<WindowNode>& node);
    void ReZOrderShowWhenLockedWindows(bool up);

    WMError AddWindowNodeOnWindowTree(sptr<WindowNode>& node, const sptr<WindowNode>& parentNode);
    void RemoveWindowNodeFromWindowTree(sptr<WindowNode>& node);
    void UpdateRSTreeWhenShowingDisplaysChange(sptr<WindowNode>& node,
        const std::vector<DisplayId>& lastShowingDisplays);
    bool CheckWindowNodeWhetherInWindowTree(const sptr<WindowNode>& node) const;
    void UpdateModeSupportInfoWhenKeyguardChange(const sptr<WindowNode>& node, bool up);
    void RemoveFromRsTreeWhenRemoveWindowNode(sptr<WindowNode>& node, bool fromAnimation);
    void ProcessInputMethodWindowAddAnimation(sptr<WindowNode>& node, std::function<void()> updateRSTreeFunc);
    uint32_t GetAppWindowNum();

    float displayBrightness_ = UNDEFINED_BRIGHTNESS;
    uint32_t brightnessWindow_ = INVALID_WINDOW_ID;
    uint32_t zOrder_ { 0 };
    uint32_t focusedWindow_ { INVALID_WINDOW_ID };
    int32_t focusedPid_ = -1;
    uint32_t activeWindow_ = INVALID_WINDOW_ID;
    int32_t activePid_ = -1;
    bool isScreenLocked_ = false;

    std::vector<uint32_t> backupWindowIds_;
    std::map<uint32_t, WindowMode> backupWindowMode_;
    std::map<DisplayId, Rect> backupDividerWindowRect_;
    std::map<DisplayId, std::set<WindowMode>> backupDisplaySplitWindowMode_;
    sptr<WindowZorderPolicy> zorderPolicy_ = new WindowZorderPolicy();
    std::unordered_map<WindowLayoutMode, sptr<WindowLayoutPolicy>> layoutPolicies_;
    WindowLayoutMode layoutMode_ = WindowLayoutMode::CASCADE;
    std::vector<Rect> currentCoveredArea_;
    std::vector<uint32_t> removedIds_;
    static AnimationConfig animationConfig_;

    sptr<WindowNode> belowAppWindowNode_ = new WindowNode();
    sptr<WindowNode> appWindowNode_ = new WindowNode();
    sptr<WindowNode> aboveAppWindowNode_ = new WindowNode();
    sptr<WindowLayoutPolicy> layoutPolicy_;
    sptr<AvoidAreaController> avoidController_;
    sptr<DisplayGroupController> displayGroupController_;
    sptr<DisplayGroupInfo> displayGroupInfo_;
};
} // namespace Rosen
} // namespace OHOS
#endif // OHOS_ROSEN_WINDOW_NODE_CONTAINER_H
