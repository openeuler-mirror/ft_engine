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

#ifndef OHOS_ROSEN_WINDOW_LAYOUT_POLICY_TILE_H
#define OHOS_ROSEN_WINDOW_LAYOUT_POLICY_TILE_H

#include <map>
#include <queue>
#include <refbase.h>
#include <set>

#include "window_layout_policy.h"
#include "window_node.h"
#include "wm_common.h"

namespace OHOS {
namespace Rosen {
class WindowLayoutPolicyTile : public WindowLayoutPolicy {
public:
    WindowLayoutPolicyTile() = delete;
    WindowLayoutPolicyTile(const sptr<DisplayGroupInfo>& displayGroupInfo,
        DisplayGroupWindowTree& displayGroupWindowTree);
    ~WindowLayoutPolicyTile() = default;
    void Launch() override;
    void AddWindowNode(const sptr<WindowNode>& node) override;
    void UpdateWindowNode(const sptr<WindowNode>& node, bool isAddWindow = false) override;
    void RemoveWindowNode(const sptr<WindowNode>& node) override;
    void UpdateLayoutRect(const sptr<WindowNode>& node) override;
    bool IsTileRectSatisfiedWithSizeLimits(const sptr<WindowNode>& node) override;

private:
    std::map<DisplayId, uint32_t> maxTileWinNumMap_;
    std::map<DisplayId, std::vector<std::vector<Rect>>> presetRectsMap_;
    std::map<DisplayId, std::deque<sptr<WindowNode>>> foregroundNodesMap_;
    void InitAllRects();
    uint32_t GetMaxTileWinNum(DisplayId displayId) const;
    void InitTileWindowRects(DisplayId displayId);
    void AssignNodePropertyForTileWindows(DisplayId displayId);
    void LayoutForegroundNodeQueue(DisplayId displayId);
    void InitForegroundNodeQueue();
    void ForegroundNodeQueuePushBack(const sptr<WindowNode>& node, DisplayId displayId);
    void ForegroundNodeQueueRemove(const sptr<WindowNode>& node);
};
}
}
#endif // OHOS_ROSEN_WINDOW_LAYOUT_POLICY_TILE_H
