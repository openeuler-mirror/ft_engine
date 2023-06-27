/*
 * Copyright (c) 2022 Huawei Technologies Co., Ltd.
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

#pragma once

#include "noncopyable_hal.h"

#include "display_type.h"
#include "drm_common.h"

namespace oewm {
namespace drm {
// typedef struct _drmModeModeInfo {
// 	uint32_t clock;
// 	uint16_t hdisplay, hsync_start, hsync_end, htotal, hskew;
// 	uint16_t vdisplay, vsync_start, vsync_end, vtotal, vscan;

// 	uint32_t vrefresh;

// 	uint32_t flags;
// 	uint32_t type;
// 	char name[DRM_DISPLAY_MODE_LEN];
// } drmModeModeInfo, *drmModeModeInfoPtr;
class DrmModeInfo : NonCopyable {
public:
    DrmModeInfo(const drmModeModeInfo &modeInfo, int id, int drmFd);
    ~DrmModeInfo() noexcept;

    int Id() const
    {
        return id_;
    }
    uint32_t BlobId() const
    {
        return blobId_;
    }
    drmModeModeInfo ModeInfo() const
    {
        return modeInfo_;
    }

    DisplayModeInfo ToHdiModeInfo() const;

private:
    drmModeModeInfo modeInfo_;
    int id_ = -1;
    int drmFd_ = INVALID_FD;
    uint32_t blobId_ = DRM_INVALID_OBJECT_ID;
};
} // namespace drm
} // namespace oewm
