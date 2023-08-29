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

#include "drm_crtc.h"

#include "hdi_display.h"
#include "log.h"
#include "drm_property.h"

namespace FT {
namespace drm {
DrmCrtc::DrmCrtc(int drmFd, uint32_t crtcId, uint32_t pipe) : drmFd_(drmFd), id_(crtcId), pipe_(pipe)
{
    auto crtc = drmModeGetCrtc(drmFd_, id_);
    if (crtc == nullptr) {
        LOG_ERROR("Failed to get drm crtc for crtc id: %{public}i", id_);
        return;
    }
    ParseFrom(crtc);
    drmModeFreeCrtc(crtc);
}

DrmCrtc::~DrmCrtc() noexcept {}

bool DrmCrtc::BindToDisplay(uint32_t id)
{
    if (displayId_ != HDI::DISPLAY::INVALID_DISPLAY_ID) {
        LOG_ERROR("the crtc has bind to display id: %{public}" PRIu32 ".", displayId_);
        return false;
    }
    displayId_ = id;
    LOG_DEBUG("the crtc : %i bind to display id: %u", id_, displayId_);
    return true;
}

void DrmCrtc::UnBindDisplay(uint32_t id)
{
    if (displayId_ == id) {
        displayId_ = HDI::DISPLAY::INVALID_DISPLAY_ID;
    } else {
        LOG_ERROR("can not unbind");
    }
}

bool DrmCrtc::CanBind()
{
    return (displayId_ == HDI::DISPLAY::INVALID_DISPLAY_ID);
}

void DrmCrtc::ParseFrom(drmModeCrtcPtr const &crtc)
{
    DrmObjectPropertyFetcher crtcPropFetcher(drmFd_, id_, DRM_MODE_OBJECT_CRTC);
    modeIdPropId_ = crtcPropFetcher.GetPropId(PROP_MODEID);
    activePropId_ = crtcPropFetcher.GetPropId(PROP_ACTIVE);
    outFencePropId_ = crtcPropFetcher.GetPropId(PROP_OUTFENCE);
}
} // namespace drm
} // namespace FT
