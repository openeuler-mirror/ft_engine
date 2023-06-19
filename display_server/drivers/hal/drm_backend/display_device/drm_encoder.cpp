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

#include "drm_encoder.h"

#include <memory>
#include <vector>

#include "log.h"

namespace oewm {
namespace drm {
DrmEncoder::DrmEncoder(int drmFd, uint32_t encoderId) : drmFd_(drmFd), id_(encoderId)
{
    auto encoder = drmModeGetEncoder(drmFd_, id_);
    if (encoder == nullptr) {
        LOG_ERROR("Failed to get drm encoder for encoder id: %{public}" PRIu32, id_);
        return;
    }
    ParseFrom(encoder);
    drmModeFreeEncoder(encoder);
}

DrmEncoder::~DrmEncoder() noexcept {}

bool DrmEncoder::FindCrtcId(const IdMapPtr<DrmCrtc> &crtcs, uint32_t &crtcId)
{
    if (crtcs.count(crtcId_) != 0 && crtcs.at(crtcId_)->CanBind()) {
        crtcId = crtcId_;
        return true;
    }

    auto res = drmModeGetResources(drmFd_);
    if (res == nullptr) {
        return false;
    }
    bool found = false;
    for (int i = 0; i != res->count_crtcs; ++i) {
        auto id = res->crtcs[i];
        if (crtcs.count(id) == 0) {
            continue;
        }
        const auto &crtc = crtcs.at(id);
        if (possibleCrtcs_ & (1 << i) && crtc->CanBind()) {
            crtcId = id;
            found = true;
            break;
        }
    }
    drmModeFreeResources(res);

    return found;
}

void DrmEncoder::ParseFrom(drmModeEncoderPtr const &encoder)
{
    crtcId_ = encoder->crtc_id;
    possibleCrtcs_ = encoder->possible_crtcs;
}
} // namespace drm
} // namespace oewm
