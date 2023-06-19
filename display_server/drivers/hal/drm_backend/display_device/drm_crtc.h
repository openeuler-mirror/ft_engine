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

#pragma once

#include "noncopyable.h"
#include "drm_common.h"
#include "drm_property.h"
#include "hdi_display.h"

namespace oewm {
namespace drm {
// prop for crtc
constexpr char PROP_MODEID[] = "MODE_ID";
constexpr char PROP_ACTIVE[] = "ACTIVE";
constexpr char PROP_OUTFENCE[] = "OUT_FENCE_PTR";
// typedef struct _drmModeCrtc {
//    uint32_t crtc_id;
//    uint32_t buffer_id; /**< FB id to connect to 0 = disconnect */
//
//    uint32_t x, y; /**< Position on the framebuffer */
//    uint32_t width, height;
//    int mode_valid;
//    drmModeModeInfo mode;
//
//    int gamma_size; /**< Number of gamma stops */
//
//} drmModeCrtc, *drmModeCrtcPtr;
class DrmCrtc : NonCopyable {
public:
    DrmCrtc(int drmFd, uint32_t crtcId, uint32_t pipe);
    ~DrmCrtc() noexcept;
    int Id() const
    {
        return id_;
    }
    uint32_t ModeIdPropId() const
    {
        return modeIdPropId_;
    }
    uint32_t ActivePropId() const
    {
        return activePropId_;
    }
    uint32_t OutFencePropId() const
    {
        return outFencePropId_;
    }
    bool BindToDisplay(uint32_t id);
    void UnBindDisplay(uint32_t id);
    bool CanBind();
    uint32_t Pipe() const
    {
        return pipe_;
    }

private:
    void ParseFrom(const drmModeCrtcPtr &crtc);

    int drmFd_ = INVALID_FD;
    int id_ = DRM_INVALID_OBJECT_ID;
    uint32_t modeIdPropId_ = DRM_INVALID_PROP_ID;
    uint32_t activePropId_ = DRM_INVALID_PROP_ID;
    uint32_t outFencePropId_ = DRM_INVALID_PROP_ID;
    uint32_t displayId_ = HDI::DISPLAY::INVALID_DISPLAY_ID;
    uint32_t pipe_ = 0;
};
} // namespace drm
} // namespace oewm
