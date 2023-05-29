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

#ifndef OHOS_ROSEN_WINDOW_LAYOUT_POLICY_H
#define OHOS_ROSEN_WINDOW_LAYOUT_POLICY_H

#include <map>
#include <refbase.h>
#include <set>

#include "display_group_info.h"
#include "display_info.h"
#include "window_node.h"
#include "wm_common.h"


namespace OHOS {
namespace Rosen {
using DisplayGroupWindowTree = std::map<DisplayId,
    std::map<WindowRootNodeType, std::unique_ptr<std::vector<sptr<WindowNode>>>>>;
enum class DockWindowShowState : uint32_t {
    NOT_SHOWN = 0,
    SHOWN_IN_BOTTOM = 1,
    SHOWN_IN_LEFT = 2,
    SHOWN_IN_RIGHT = 3,
    SHOWN_IN_TOP = 4,
};
class WindowLayoutPolicy : public RefBase {
public:
    WindowLayoutPolicy() = delete;
    WindowLayoutPolicy(const sptr<DisplayGroupInfo>& displayGroupInfo, DisplayGroupWindowTree& displayGroupWindowTree);
    ~WindowLayoutPolicy() = default;
    virtual void Launch();
    virtual void Clean();
    virtual void Reset();
    virtual void Reorder();
    virtual void AddWindowNode(const sptr<WindowNode>& node) = 0;
    virtual void LayoutWindowTree(DisplayId displayId);
    virtual void RemoveWindowNode(const sptr<WindowNode>& node);
    virtual void UpdateWindowNode(const sptr<WindowNode>& node, bool isAddWindow = false);
    virtual void UpdateLayoutRect(const sptr<WindowNode>& node) = 0;
    virtual void SetSplitDividerWindowRects(std::map<DisplayId, Rect> dividerWindowRects) {};
    virtual Rect GetDividerRect(DisplayId displayId) const;
    virtual std::vector<int32_t> GetExitSplitPoints(DisplayId displayId) const;
    float GetVirtualPixelRatio(DisplayId displayId) const;
    void UpdateClientRectAndResetReason(const sptr<WindowNode>& node, const Rect& winRect);
    void UpdateClientRect(const Rect& rect, const sptr<WindowNode>& node, WindowSizeChangeReason reason);
    Rect GetDisplayGroupRect() const;
    bool IsMultiDisplay();
    void ProcessDisplayCreate(DisplayId displayId, const std::map<DisplayId, Rect>& displayRectMap);
    void ProcessDisplayDestroy(DisplayId displayId, const std::map<DisplayId, Rect>& displayRectMap);
    void ProcessDisplaySizeChangeOrRotation(DisplayId displayId, const std::map<DisplayId, Rect>& displayRectMap);
    void SetSplitRatioConfig(const SplitRatioConfig& splitRatioConfig);
    virtual bool IsTileRectSatisfiedWithSizeLimits(const sptr<WindowNode>& node);
    // methods for setting bottom posY limit for cascade rect on pc
    static void SetCascadeRectBottomPosYLimit(uint32_t floatingBottomPosY);

protected:
    void UpdateFloatingLayoutRect(Rect& limitRect, Rect& winRect);
    void UpdateLimitRect(const sptr<WindowNode>& node, Rect& limitRect);
    virtual void LayoutWindowNode(const sptr<WindowNode>& node);
    AvoidPosType GetAvoidPosType(const Rect& rect, DisplayId displayId) const;
    void CalcAndSetNodeHotZone(const Rect& winRect, const sptr<WindowNode>& node) const;
    void ComputeDecoratedRequestRect(const sptr<WindowNode>& node) const;
    bool IsVerticalDisplay(DisplayId displayId) const;
    bool IsFullScreenRecentWindowExist(const std::vector<sptr<WindowNode>>& nodeVec) const;
    void LayoutWindowNodesByRootType(const std::vector<sptr<WindowNode>>& nodeVec);
    virtual void UpdateSurfaceBounds(const sptr<WindowNode>& node, const Rect& winRect, const Rect& preRect);
    void UpdateRectInDisplayGroupForAllNodes(DisplayId displayId,
        const Rect& oriDisplayRect, const Rect& newDisplayRect);
    void UpdateRectInDisplayGroup(const sptr<WindowNode>& node,
        const Rect& oriDisplayRect, const Rect& newDisplayRect);
    void LimitWindowToBottomRightCorner(const sptr<WindowNode>& node);
    void UpdateDisplayGroupRect();
    void UpdateDisplayGroupLimitRect();
    void UpdateMultiDisplayFlag();
    void PostProcessWhenDisplayChange();
    void UpdateDisplayRectAndDisplayGroupInfo(const std::map<DisplayId, Rect>& displayRectMap);
    DockWindowShowState GetDockWindowShowState(DisplayId displayId, Rect& dockWinRect) const;
    void LimitFloatingWindowSize(const sptr<WindowNode>& node, const Rect& displayRect, Rect& winRect) const;
    void LimitMainFloatingWindowPosition(const sptr<WindowNode>& node, Rect& winRect) const;

    void UpdateFloatingWindowSizeForStretchableWindow(const sptr<WindowNode>& node,
        const Rect& displayRect, Rect& winRect) const;
    void UpdateFloatingWindowSizeBySizeLimits(const sptr<WindowNode>& node,
        const Rect& displayRect, Rect& winRect) const;
    void LimitWindowPositionWhenInitRectOrMove(const sptr<WindowNode>& node, Rect& winRect) const;
    void LimitWindowPositionWhenDrag(const sptr<WindowNode>& node, Rect& winRect) const;
    void FixWindowSizeByRatioIfDragBeyondLimitRegion(const sptr<WindowNode>& node, Rect& winRect);
    void UpdateWindowSizeLimits(const sptr<WindowNode>& node);
    WindowSizeLimits GetSystemSizeLimits(const sptr<WindowNode>& node,
        const Rect& displayRect, float virtualPixelRatio);
    Rect CalcEntireWindowHotZone(const sptr<WindowNode>& node, const Rect& winRect, uint32_t hotZone,
        float vpr, TransformHelper::Vector2 hotZoneScale) const;
    void NotifyAnimationSizeChangeIfNeeded();
    const std::set<WindowType> avoidTypes_ {
        WindowType::WINDOW_TYPE_STATUS_BAR,
        WindowType::WINDOW_TYPE_NAVIGATION_BAR,
    };
    struct LayoutRects {
        Rect primaryRect_;
        Rect secondaryRect_;
        Rect primaryLimitRect_;
        Rect secondaryLimitRect_;
        Rect dividerRect_;
        Rect firstCascadeRect_;
        std::vector<int32_t> exitSplitPoints_; // 2 element, first element < second element
        std::vector<int32_t> splitRatioPoints_;
    };
    sptr<DisplayGroupInfo> displayGroupInfo_;
    mutable std::map<DisplayId, Rect> limitRectMap_;
    DisplayGroupWindowTree& displayGroupWindowTree_;
    Rect displayGroupRect_;
    Rect displayGroupLimitRect_;
    bool isMultiDisplay_ = false;
    SplitRatioConfig splitRatioConfig_;
    // bottom posY limit for cascade rect on pc
    static uint32_t floatingBottomPosY_;
};
}
}
#endif // OHOS_ROSEN_WINDOW_LAYOUT_POLICY_H
