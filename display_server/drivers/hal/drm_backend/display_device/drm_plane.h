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
#include "drm_common.h"
#include "drm_property.h"

namespace FT {
namespace drm {
// prop for plane
constexpr char PROP_FBID[] = "FB_ID";
constexpr char PROP_CRTC_ID[] = "CRTC_ID";
constexpr char PROP_TYPE[] = "type";
constexpr char PROP_CRTC_X_ID[] = "CRTC_X";
constexpr char PROP_CRTC_Y_ID[] = "CRTC_Y";
constexpr char PROP_CRTC_W_ID[] = "CRTC_W";
constexpr char PROP_CRTC_H_ID[] = "CRTC_H";
constexpr char PROP_SRC_X_ID[] = "SRC_X";
constexpr char PROP_SRC_Y_ID[] = "SRC_Y";
constexpr char PROP_SRC_W_ID[] = "SRC_W";
constexpr char PROP_SRC_H_ID[] = "SRC_H";
// typedef struct _drmModePlane {
// 	uint32_t count_formats;
// 	uint32_t *formats;
// 	uint32_t plane_id;

// 	uint32_t crtc_id;
// 	uint32_t fb_id;

// 	uint32_t crtc_x, crtc_y;
// 	uint32_t x, y;

// 	uint32_t possible_crtcs;
// 	uint32_t gamma_size;
// } drmModePlane, *drmModePlanePtr;
class DrmPlane : NonCopyable {
public:
    DrmPlane(int drmFd, uint32_t planeId);
    ~DrmPlane() noexcept;
    uint64_t GetPlaneType() const
    {
        return type_;
    }
    uint32_t Id() const
    {
        return id_;
    }
    uint32_t FBPropId() const
    {
        return fbPropId_;
    }
    uint32_t CrtcPropId() const
    {
        return crtcPropId_;
    }
    uint32_t TypePropId() const
    {
        return typePropId_;
    }
    uint32_t CrtcXPropId() const
    {
        return crtcXPropId_;
    }
    uint32_t CrtcYPropId() const
    {
        return crtcYPropId_;
    }
    uint32_t CrtcWPropId() const
    {
        return crtcWPropId_;
    }
    uint32_t CrtcHPropId() const
    {
        return crtcHPropId_;
    }
    uint32_t SrcXPropId() const
    {
        return srcXPropId_;
    }
    uint32_t SrcYPropId() const
    {
        return srcYPropId_;
    }
    uint32_t SrcWPropId() const
    {
        return srcWPropId_;
    }
    uint32_t SrcHPropId() const
    {
        return srcHPropId_;
    }
    uint32_t GetPossibleCrtcs() const
    {
        return possibleCrtcs_;
    }

private:
    void ParseFrom(const drmModePlanePtr &plane);

    int drmFd_ = INVALID_FD;
    uint32_t id_ = DRM_INVALID_OBJECT_ID;
    uint32_t fbPropId_ = DRM_INVALID_PROP_ID;
    uint32_t crtcPropId_ = DRM_INVALID_PROP_ID;
    uint32_t typePropId_ = DRM_INVALID_PROP_ID;
    uint32_t crtcXPropId_ = DRM_INVALID_PROP_ID;
    uint32_t crtcYPropId_ = DRM_INVALID_PROP_ID;
    uint32_t crtcWPropId_ = DRM_INVALID_PROP_ID;
    uint32_t crtcHPropId_ = DRM_INVALID_PROP_ID;
    uint32_t srcXPropId_ = DRM_INVALID_PROP_ID;
    uint32_t srcYPropId_ = DRM_INVALID_PROP_ID;
    uint32_t srcWPropId_ = DRM_INVALID_PROP_ID;
    uint32_t srcHPropId_ = DRM_INVALID_PROP_ID;
    uint64_t type_ = DRM_INVLIAD_VALUE;
    uint32_t possibleCrtcs_ = 0;
};
} // namespace drm
} // namespace FT
