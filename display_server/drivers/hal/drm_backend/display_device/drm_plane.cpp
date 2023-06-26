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

#include "drm_plane.h"

#include "drm_mode.h"
#include "log.h"

namespace oewm {
namespace drm {
DrmPlane::DrmPlane(int drmFd, uint32_t planeId) : drmFd_(drmFd), id_(planeId)
{
    auto plane = drmModeGetPlane(drmFd_, id_);
    if (plane == nullptr) {
        LOG_ERROR("Failed to get drm plane for plane id: %{public}" PRIu32, id_);
        return;
    }
    ParseFrom(plane);
    drmModeFreePlane(plane);
}

DrmPlane::~DrmPlane() noexcept {}

void DrmPlane::ParseFrom(drmModePlanePtr const &plane)
{
    DrmObjectPropertyFetcher planePropFetcher(drmFd_, id_, DRM_MODE_OBJECT_PLANE);
    fbPropId_ = planePropFetcher.GetPropId(PROP_FBID);
    crtcPropId_ = planePropFetcher.GetPropId(PROP_CRTC_ID);
    typePropId_ = planePropFetcher.GetPropId(PROP_TYPE);
    crtcXPropId_ = planePropFetcher.GetPropId(PROP_CRTC_X_ID);
    crtcYPropId_ = planePropFetcher.GetPropId(PROP_CRTC_Y_ID);
    crtcWPropId_ = planePropFetcher.GetPropId(PROP_CRTC_W_ID);
    crtcHPropId_ = planePropFetcher.GetPropId(PROP_CRTC_H_ID);
    srcXPropId_ = planePropFetcher.GetPropId(PROP_SRC_X_ID);
    srcYPropId_ = planePropFetcher.GetPropId(PROP_SRC_Y_ID);
    srcWPropId_ = planePropFetcher.GetPropId(PROP_SRC_W_ID);
    srcHPropId_ = planePropFetcher.GetPropId(PROP_SRC_H_ID);
    type_ = planePropFetcher.GetPropValue(PROP_TYPE);
    possibleCrtcs_ = plane->possible_crtcs;
}
} // namespace drm
} // namespace oewm
