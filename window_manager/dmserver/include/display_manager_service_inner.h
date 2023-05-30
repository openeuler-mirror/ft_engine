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

#ifndef FOUNDATION_DMSERVER_DISPLAY_MANAGER_SERVICE_INNER_H
#define FOUNDATION_DMSERVER_DISPLAY_MANAGER_SERVICE_INNER_H

#include <vector>

#include <pixel_map.h>
#include <system_ability.h>

#include "abstract_display.h"
#include "cutout_info.h"
#include "display_change_listener.h"
#include "rsscreen_change_listener.h"
#include "singleton_delegator.h"
#include "wm_single_instance.h"
#include "window_info_queried_listener.h"

namespace OHOS::Rosen {
class DisplayManagerServiceInner {
WM_DECLARE_SINGLE_INSTANCE(DisplayManagerServiceInner);

public:
    std::vector<sptr<DisplayInfo>> GetAllDisplays() const;
    DisplayId GetDefaultDisplayId() const;
    sptr<DisplayInfo> GetDefaultDisplay() const;
    sptr<DisplayInfo> GetDisplayById(DisplayId displayId) const;
    std::vector<DisplayId> GetAllDisplayIds() const;
    uint32_t GetRSScreenNum() const;
    sptr<ScreenInfo> GetScreenInfoByDisplayId(DisplayId displayId) const;
    ScreenId GetScreenGroupIdByDisplayId(DisplayId displayId) const;
    sptr<SupportedScreenModes> GetScreenModesByDisplayId(DisplayId displayId) const;
    std::shared_ptr<Media::PixelMap> GetDisplaySnapshot(DisplayId) const;
    void UpdateRSTree(DisplayId displayId, DisplayId parentDisplayId, std::shared_ptr<RSSurfaceNode>& surfaceNode,
        bool isAdd, bool isMultiDisplay);
    void RegisterDisplayChangeListener(sptr<IDisplayChangeListener> listener);
    bool SetOrientationFromWindow(DisplayId displayId, Orientation orientation);
    bool SetRotationFromWindow(DisplayId displayId, Rotation targetRotation);
    void SetGravitySensorSubscriptionEnabled();
    void RegisterWindowInfoQueriedListener(const sptr<IWindowInfoQueriedListener>& listener);
    void RegisterRSScreenChangeListener(const sptr<IRSScreenChangeListener>& listener);
    sptr<CutoutInfo> GetCutoutInfo(DisplayId displayId) const;
};
} // namespace OHOS::Rosen

#endif // FOUNDATION_DMSERVER_DISPLAY_MANAGER_SERVICE_H