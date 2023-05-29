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

#ifndef OHOS_ROSEN_MINIMIZE_APP_H
#define OHOS_ROSEN_MINIMIZE_APP_H

#include <map>
#include <vector>

#include <refbase.h>
#include "wm_common.h"
#include "window_node.h"

namespace OHOS {
namespace Rosen {
enum MinimizeReason : uint32_t {
    MINIMIZE_BUTTON = 1,
    MINIMIZE_ALL = 1 << 1,
    LAYOUT_TILE = 1 << 2,
    LAYOUT_CASCADE = 1 << 3,
    MAX_APP_COUNT = 1 << 4,
    SPLIT_REPLACE = 1 << 5,
    SPLIT_QUIT = 1 << 6,
    GESTURE_ANIMATION = 1 << 7,
    OTHER_WINDOW = 1 << 8,
};

class MinimizeApp : public RefBase {
public:
    MinimizeApp() = delete;
    ~MinimizeApp() = default;

    static void AddNeedMinimizeApp(const sptr<WindowNode>& node, MinimizeReason reason);
    static void ExecuteMinimizeAll();
    static void ExecuteMinimizeTargetReasons(uint32_t reasons);
    static void SetMinimizedByOtherConfig(bool isMinimizedByOther);
    static void ClearNodesWithReason(MinimizeReason reason);
    static bool IsNodeNeedMinimize(const sptr<WindowNode>& node);
    static std::vector<wptr<WindowNode>> GetNeedMinimizeAppNodesWithReason(MinimizeReason reason);
    static sptr<WindowNode> GetRecoverdNodeFromMinimizeList();
    static bool IsNodeNeedMinimizeWithReason(const sptr<WindowNode>& node, MinimizeReason reason);
    static bool EnableMinimize(MinimizeReason reason);
private:
    static inline bool IsFromUser(MinimizeReason reason)
    {
        return (reason == MinimizeReason::MINIMIZE_ALL || reason == MinimizeReason::MINIMIZE_BUTTON ||
            reason == MinimizeReason::MAX_APP_COUNT || reason == MinimizeReason::LAYOUT_TILE ||
            reason == MinimizeReason::SPLIT_REPLACE || reason == MinimizeReason::SPLIT_QUIT);
    }

    static std::map<MinimizeReason, std::vector<wptr<WindowNode>>> needMinimizeAppNodes_;
    static bool isMinimizedByOtherWindow_;
    static std::recursive_mutex mutex_;
};
} // Rosen
} // OHOS
#endif // OHOS_ROSEN_MINIMIZE_APP_H
