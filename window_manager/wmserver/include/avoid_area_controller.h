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

#ifndef OHOS_ROSEN_AVOID_AREA_CONTROLLER_H
#define OHOS_ROSEN_AVOID_AREA_CONTROLLER_H

#include <map>
#include <set>
#include <unordered_map>
#include <vector>

#include <refbase.h>

#include "class_var_definition.h"
#include "window_node.h"
#include "wm_common.h"
#include "wm_common_inner.h"

namespace OHOS {
namespace Rosen {
enum class AvoidControlType : uint32_t {
    AVOID_NODE_ADD,
    AVOID_NODE_UPDATE,
    AVOID_NODE_REMOVE,
    AVOID_NODE_UNKNOWN,
};

class AvoidAreaController : public RefBase {
public:
    AvoidAreaController(uint32_t& focusedWindow) : focusedWindow_(focusedWindow) {};
    ~AvoidAreaController() = default;

    void UpdateAvoidAreaListener(sptr<WindowNode>& windowNode, bool isRegisterListener);
    void ProcessWindowChange(const sptr<WindowNode>& windowNode, AvoidControlType avoidType,
        const std::function<bool(sptr<WindowNode>)>& checkFunc);
    AvoidArea GetAvoidAreaByType(const sptr<WindowNode>& node, AvoidAreaType avoidAreaType) const;

private:
    void AddOrRemoveOverlayWindowIfNeed(const sptr<WindowNode>& overlayNode, bool isAdding);
    void AddOrRemoveKeyboard(const sptr<WindowNode>& keyboardNode, bool isAdding);
    void UpdateOverlayWindowIfNeed(const sptr<WindowNode>& node,
        const std::function<bool(sptr<WindowNode>)>& checkFunc);
    AvoidPosType CalculateOverlayRect(const sptr<WindowNode>& node,
        const sptr<WindowNode>& overlayNode, Rect& overlayRect) const;
    AvoidPosType GetAvoidPosType(const Rect& windowRect, const Rect& overlayRect) const;
    void SetAvoidAreaRect(AvoidArea& avoidArea, const Rect& rect, AvoidPosType type) const;
    bool UpdateAvoidAreaIfNeed(const AvoidArea& avoidArea, const sptr<WindowNode>& node, AvoidAreaType avoidAreaType);
    AvoidArea GetAvoidAreaSystemType(const sptr<WindowNode>& node) const;
    AvoidArea GetAvoidAreaKeyboardType(const sptr<WindowNode>& node) const;

    uint32_t& focusedWindow_;
    std::unordered_map<uint32_t, sptr<WindowNode>> overlayWindowMap_;
    std::set<sptr<WindowNode>> avoidAreaListenerNodes_;
    std::map<uint32_t, std::map<AvoidAreaType, AvoidArea>> lastUpdatedAvoidArea_;
    uint32_t lastSoftInputKeyboardAreaUpdatedWindowId_ { 0 };
    DEFINE_VAR_DEFAULT_FUNC_SET(bool, FlagForProcessWindowChange, isForbidProcessingWindowChange, false)
};
}
}
#endif // OHOS_ROSEN_AVOID_AREA_CONTROLLER_H
