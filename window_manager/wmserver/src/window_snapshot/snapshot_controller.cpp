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

#include "snapshot_controller.h"

#include <hitrace_meter.h>

#include "surface_capture_future.h"
#include "surface_draw.h"
#include "window_manager_hilog.h"
#include "wm_common.h"

namespace OHOS {
namespace Rosen {
namespace {
    constexpr HiviewDFX::HiLogLabel LABEL = {LOG_CORE, HILOG_DOMAIN_DISPLAY, "SnapshotController"};
}

int32_t SnapshotController::GetSnapshot(const sptr<IRemoteObject> &token, Snapshot& snapshot)
{
    HITRACE_METER_FMT(HITRACE_TAG_WINDOW_MANAGER, "wms:GetSnapshot");
    if (token == nullptr) {
        WLOGFE("Get snapshot failed, because token is null.");
        return static_cast<int32_t>(WMError::WM_ERROR_NULLPTR);
    }
    if (handler_ == nullptr || windowRoot_ == nullptr) {
        WLOGFE("Get snapshot failed, because handler/root is null.");
        return static_cast<int32_t>(WMError::WM_ERROR_NULLPTR);
    }

    performReport_->start();
    // get snapshot cache in wms main handler
    std::shared_ptr<Media::PixelMap> pixelMap;
    std::shared_ptr<RSSurfaceNode> surfaceNode;
    auto task = [this, &pixelMap, &surfaceNode, token] () {
        auto targetNode = windowRoot_->GetWindowNodeByAbilityToken(token);
        if (targetNode != nullptr) {
            pixelMap = targetNode->GetSnapshot();
            targetNode->SetSnapshot(nullptr); // reset window snapshot after use
            surfaceNode = targetNode->surfaceNode_;
        }
    };
    handler_->PostSyncTask(task, AppExecFwk::EventQueue::Priority::IMMEDIATE);

    // do snapshot if no cache
    if (pixelMap == nullptr) {
        bool snapSucc = SurfaceDraw::GetSurfaceSnapshot(surfaceNode, pixelMap, 300); // snapshot time out 300ms
        if (!snapSucc) {
            return static_cast<int32_t>(WMError::WM_ERROR_NULLPTR);
        }
    }

    // success to snapshot
    snapshot.SetPixelMap(pixelMap);
    performReport_->end();
    return static_cast<int32_t>(WM_OK);
}
} // namespace Rosen
} // namespace OHOS