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
#include "drm_crtc.h"

namespace oewm {
namespace drm {
class DrmEncoder : NonCopyable {
public:
    DrmEncoder(int drmFd, uint32_t encoderId);
    ~DrmEncoder() noexcept;
    bool FindCrtcId(const IdMapPtr<DrmCrtc> &crtcs, uint32_t &crtcId);

private:
    void ParseFrom(const drmModeEncoderPtr &encoder);

    int drmFd_ = INVALID_FD;
    uint32_t id_ = DRM_INVALID_OBJECT_ID;
    uint32_t crtcId_ = DRM_INVALID_OBJECT_ID;
    uint32_t possibleCrtcs_ = DRM_INVALID_OBJECT_ID;
};
} // namespace drm
} // namespace oewm
