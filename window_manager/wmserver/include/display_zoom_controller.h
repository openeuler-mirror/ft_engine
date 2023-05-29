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

#ifndef OHOS_ROSEN_DISPLAY_ZOOM_CONTROLLER_H
#define OHOS_ROSEN_DISPLAY_ZOOM_CONTROLLER_H

#include <refbase.h>

#include "window_node.h"
#include "window_root.h"
#include "window_node_container.h"
#include "wm_common.h"

namespace OHOS::Rosen {
class DisplayZoomController : public RefBase {
public:
    explicit DisplayZoomController(sptr<WindowRoot>& root) : windowRoot_(root) {}
    ~DisplayZoomController() = default;
    void SetAnchorAndScale(int32_t x, int32_t y, float scale);
    void SetAnchorOffset(int32_t deltaX, int32_t deltaY);
    void OffWindowZoom();
    void UpdateAllWindowsZoomInfo(DisplayId displayId);
    void UpdateWindowZoomInfo(uint32_t windowId);
    void ClearZoomTransform(std::vector<sptr<WindowNode>> nodes);
private:
    struct DisplayZoomInfo {
        int32_t pivotX;
        int32_t pivotY;
        float scale;
        int32_t translateX;
        int32_t translateY;
    };
    sptr<WindowRoot> windowRoot_;
    void ClearZoomTransformInner(sptr<WindowNode> node);
    bool UpdateZoomTranslateInfo(sptr<WindowNodeContainer> windowNodeContainer, DisplayId displayId,
        int32_t& deltaX, int32_t& deltaY);
    Transform CalcuAnimateZoomTrans(sptr<WindowNode> node);
    Transform CalcuZoomTransByZoomInfo(sptr<WindowNode> node);
    Transform CalcuZoomTrans(sptr<WindowNode> node, const DisplayZoomInfo& zoomInfo);
    void UpdateClientAndSurfaceZoomInfo(sptr<WindowNode> node, const Transform& zoomTrans);
    void HandleUpdateWindowZoomInfo(sptr<WindowNode> node);
    void TransformSurfaceNode(std::shared_ptr<RSSurfaceNode> surfaceNode, const Transform& trans);
    DisplayZoomInfo zoomInfo_ = {0, 0, 1.0, 0, 0}; // compared with original window rect
    std::unordered_set<WindowType> displayZoomWindowTypeSkipped_ {
        WindowType::WINDOW_TYPE_NAVIGATION_BAR,
        WindowType::WINDOW_TYPE_INPUT_METHOD_FLOAT,
    };
};
}
#endif // OHOS_ROSEN_DISPLAY_ZOOM_CONTROLLER_H
