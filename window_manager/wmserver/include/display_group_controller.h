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

#ifndef OHOS_ROSEN_DISPLAY_GROUP_CONTROLLER_H
#define OHOS_ROSEN_DISPLAY_GROUP_CONTROLLER_H

#include <refbase.h>

#include "avoid_area_controller.h"
#include "display_group_info.h"
#include "display_info.h"
#include "display_manager_service_inner.h"
#include "window_layout_policy.h"
#include "window_manager.h"
#include "window_node.h"
#include "window_pair.h"
#include "wm_common.h"

namespace OHOS {
namespace Rosen {
using SysBarNodeMap = std::unordered_map<WindowType, sptr<WindowNode>>;
using SysBarTintMap = std::unordered_map<WindowType, SystemBarRegionTint>;
class WindowNodeContainer;

class DisplayGroupController : public RefBase {
public:
    DisplayGroupController(const sptr<WindowNodeContainer>& windowNodeContainer,
        const sptr<DisplayGroupInfo>& displayGroupInfo)
        : windowNodeContainer_(windowNodeContainer), displayGroupInfo_(displayGroupInfo) {}
    ~DisplayGroupController() = default;

    void InitNewDisplay(DisplayId displayId);
    void UpdateDisplayGroupWindowTree();
    void PreProcessWindowNode(const sptr<WindowNode>& node, WindowUpdateType type);
    void PostProcessWindowNode(const sptr<WindowNode>& node);
    void ProcessDisplayCreate(DisplayId defaultDisplayId, sptr<DisplayInfo> displayInfo,
                              const std::map<DisplayId, Rect>& displayRectMap);
    void ProcessDisplayDestroy(DisplayId defaultDisplayId, sptr<DisplayInfo> displayInfo,
                               const std::map<DisplayId, Rect>& displayRectMap,
                               std::vector<uint32_t>& windowIds);
    void ProcessDisplayChange(DisplayId defaultDisplayId, sptr<DisplayInfo> displayInfo,
                              const std::map<DisplayId, Rect>& displayRectMap,
                              DisplayStateChangeType type);
    sptr<WindowPair> GetWindowPairByDisplayId(DisplayId displayId);
    void SetDividerRect(DisplayId displayId, const Rect& rect);

    DisplayGroupWindowTree displayGroupWindowTree_;
    std::map<DisplayId, SysBarNodeMap> sysBarNodeMaps_;
    std::map<DisplayId, SysBarTintMap> sysBarTintMaps_;

private:
    std::vector<sptr<WindowNode>>* GetWindowNodesByDisplayIdAndRootType(DisplayId displayId, WindowRootNodeType type);
    void AddWindowNodeOnWindowTree(sptr<WindowNode>& node, WindowRootNodeType rootType);
    void ProcessNotCrossNodesOnDestroyedDisplay(DisplayId displayId, std::vector<uint32_t>& windowIds);
    void ProcessDisplaySizeChangeOrRotation(DisplayId defaultDisplayId, DisplayId displayId,
        const std::map<DisplayId, Rect>& displayRectMap, DisplayStateChangeType type);
    void ProcessCrossNodes(DisplayId defaultDisplayId, DisplayStateChangeType type);
    void MoveCrossNodeToTargetDisplay(const sptr<WindowNode>& node, DisplayId targetDisplayId);
    void MoveNotCrossNodeToDefaultDisplay(const sptr<WindowNode>& node, DisplayId displayId);
    void UpdateWindowDisplayIdIfNeeded(const sptr<WindowNode>& node);
    void UpdateWindowShowingDisplays(const sptr<WindowNode>& node);
    void UpdateWindowDisplayId(const sptr<WindowNode>& node, DisplayId newDisplayId);
    void ClearMapOfDestroyedDisplay(DisplayId displayId);
    void ChangeToRectInDisplayGroup(const sptr<WindowNode>& node, DisplayId displayId);
    void UpdateNodeSizeChangeReasonWithRotation(DisplayId displayId);

    sptr<WindowNodeContainer> windowNodeContainer_;
    sptr<DisplayGroupInfo> displayGroupInfo_;
    std::map<DisplayId, sptr<WindowPair>> windowPairMap_;
    DisplayId defaultDisplayId_ { 0 };
};
} // namespace Rosen
} // namespace OHOS
#endif // OHOS_ROSEN_DISPLAY_GROUP_CONTROLLER_H
