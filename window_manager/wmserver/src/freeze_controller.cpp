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

#include "freeze_controller.h"

#include <securec.h>

#include "display_manager_service_inner.h"
#include "pixel_map.h"
#include "surface_draw.h"
#include "window_manager_hilog.h"
#include "window_option.h"
#include "wm_common.h"

namespace OHOS {
namespace Rosen {
namespace {
    constexpr HiviewDFX::HiLogLabel LABEL = {LOG_CORE, HILOG_DOMAIN_WINDOW, "FreezeController"};
}

bool FreezeController::FreezeDisplay(DisplayId displayId)
{
    sptr<Window> window = CreateCoverWindow(displayId);
    if (window == nullptr) {
        return false;
    }

    WMError res = window->Show();
    if (res != WMError::WM_OK) {
        WLOGFE("Show window failed");
        return false;
    }
    std::shared_ptr<Media::PixelMap> pixelMap = DisplayManagerServiceInner::GetInstance().GetDisplaySnapshot(displayId);
    if (pixelMap == nullptr) {
        WLOGE("freeze display fail, pixel map is null. display %{public}" PRIu64"", displayId);
        return false;
    }
    return SurfaceDraw::DrawImage(window->GetSurfaceNode(), window->GetRect().width_,
        window->GetRect().height_, pixelMap);
}

bool FreezeController::UnfreezeDisplay(DisplayId displayId)
{
    auto iter = coverWindowMap_.find(displayId);
    if (iter == coverWindowMap_.end()) {
        WLOGW("unfreeze fail, no cover window. display %{public}" PRIu64"", displayId);
        return false;
    }
    sptr<Window> window = iter->second;
    if (window == nullptr) {
        WLOGW("unfreeze fail, window is null. display %{public}" PRIu64"", displayId);
        return false;
    }
    return WMError::WM_OK == window->Destroy();
}

sptr<Window> FreezeController::CreateCoverWindow(DisplayId displayId)
{
    sptr<WindowOption> option = new (std::nothrow) WindowOption();
    if (option == nullptr) {
        WLOGFE("window option is null");
        return nullptr;
    }
    option->SetWindowType(WindowType::WINDOW_TYPE_FREEZE_DISPLAY);
    option->SetFocusable(false);
    option->SetMainHandlerAvailable(false);
    option->RemoveWindowFlag(WindowFlag::WINDOW_FLAG_NEED_AVOID);
    option->SetDisplayId(displayId);
    sptr<Window> window = Window::Create("freeze" + std::to_string(displayId), option);
    if (window == nullptr) {
        WLOGFE("cover window is null");
        return nullptr;
    }
    coverWindowMap_[displayId] = window;
    return window;
}
} // Rosen
} // OHOS