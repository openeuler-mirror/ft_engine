/*
 * Copyright (c) 2023 Huawei Technologies Co., Ltd.
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

#ifndef POINTER_DRAW_H
#define POINTER_DRAW_H

#include "graphic_common.h"
#include "wm_single_instance.h"
#include "ui/rs_display_node.h"
#include "ui/rs_surface_node.h"
#include "event_handler.h"

namespace FangTian {
class PointerDraw {
WM_DECLARE_SINGLE_INSTANCE_BASE(PointerDraw);
public:
    OHOS::WMError Init();
    void AsyncMove(int32_t x, int32_t y);

private:
    PointerDraw() = default;
    virtual ~PointerDraw() = default;
    using Task = std::function<void()>;

    OHOS::WMError InitDisplayInfo();
    OHOS::WMError InitLayerNode();
    OHOS::WMError InitDisplayNode();
    OHOS::WMError MoveTo(int32_t x, int32_t y);
    void PostAsyncTask(Task task);

    int32_t displayWidth_ = 0;
    int32_t displayHeight_ = 0;
    uint64_t displayId_ = 0;
    OHOS::Rosen::RSDisplayNode::SharedPtr displayNode_ = nullptr;
    OHOS::Rosen::RSSurfaceNode::SharedPtr surfaceNode_ = nullptr;
    std::shared_ptr<OHOS::Rosen::RSSurface> rsSurface_ = nullptr;
    std::shared_ptr<OHOS::AppExecFwk::EventRunner> runner_ = nullptr;
    std::shared_ptr<OHOS::AppExecFwk::EventHandler> handler_ = nullptr;
};
} // namespace FangTian
#endif // POINTER_DRAW_H
