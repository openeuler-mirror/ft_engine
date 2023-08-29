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

#include "drm_mode_info.h"

#include "log.h"

namespace FT {
namespace drm {
DrmModeInfo::DrmModeInfo(const drmModeModeInfo &modeInfo, int id, int drmFd)
    : modeInfo_(modeInfo), id_(id), drmFd_(drmFd)
{
    // LOG_DEBUG << "DrmModeInfo::DrmModeInfo: modeId: " << id_ << ", vrefresh: " << modeInfo_.vrefresh
    //           << ", vdisplay: " << modeInfo_.vdisplay << ", hdisplay:" << modeInfo_.hdisplay;
    int ret = drmModeCreatePropertyBlob(drmFd, &modeInfo_, sizeof(modeInfo), &blobId_);
    if (ret != 0) {
        LOG_WARN("DrmModeInfo create property blob failed: %{public}s", ErrnoToString(errno).c_str());
    }
}

DrmModeInfo::~DrmModeInfo() noexcept
{
    int ret = drmModeDestroyPropertyBlob(drmFd_, blobId_);
    if (ret != 0) {
        LOG_WARN("DrmModeInfo destroy property blob failed: %{public}s", ErrnoToString(errno).c_str());
    }
}

DisplayModeInfo DrmModeInfo::ToHdiModeInfo() const
{
    DisplayModeInfo res;
    res.width = modeInfo_.hdisplay;
    res.height = modeInfo_.vdisplay;
    res.id = Id();
    res.freshRate = modeInfo_.vrefresh;
    return res;
}
} // namespace drm
} // namespace FT
