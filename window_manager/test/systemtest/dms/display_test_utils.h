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

#ifndef FRAMEWORKS_DM_TEST_ST_DISPLAY_TEST_UTILS_H
#define FRAMEWORKS_DM_TEST_ST_DISPLAY_TEST_UTILS_H

#include <securec.h>

#include "display_manager.h"
#include "screen_manager.h"
#include "display_property.h"
#include "display.h"
#include "screen.h"
#include "display_info.h"
#include "wm_common.h"
#include "dm_common.h"
#include "window_manager_hilog.h"
#include "unique_fd.h"
#include "ui/rs_surface_node.h"
#include "ui/rs_display_node.h"

namespace OHOS {
namespace Rosen {
class DisplayTestUtils {
public:
    ~DisplayTestUtils();
    static bool SizeEqualToDisplay(const sptr<Display>& display, const Media::Size cur);
    static bool SizeEqual(const Media::Size dst, const Media::Size cur);
    void init();
    bool CreateSurface();
    void SetDefaultWH(const sptr<Display>& display);
    class BufferListener : public IBufferConsumerListener {
    public:
        explicit BufferListener(DisplayTestUtils &displayTestUtils): utils_(displayTestUtils)
        {
        }
        ~BufferListener() noexcept override = default;
        void OnBufferAvailable() override
        {
            utils_.OnVsync();
        }

    private:
        DisplayTestUtils &utils_;
    };
    friend class BufferListener;

    void OnVsync();
    uint32_t successCount_ = 0;
    uint32_t failCount_ = 0;
    uint32_t defaultWidth_ = 0;
    uint32_t defaultHeight_ = 0;
    sptr<IBufferConsumerListener> listener_ = nullptr;
    sptr<Surface> csurface_ = nullptr; // cosumer surface
    sptr<Surface> psurface_ = nullptr; // producer surface
    sptr<SurfaceBuffer> prevBuffer_ = nullptr;
    BufferHandle *bufferHandle_ = nullptr;

private:
    std::mutex mutex_;
};
} // namespace ROSEN
} // namespace OHOS
#endif // FRAMEWORKS_DM_TEST_ST_DISPLAY_TEST_UTILS_H
