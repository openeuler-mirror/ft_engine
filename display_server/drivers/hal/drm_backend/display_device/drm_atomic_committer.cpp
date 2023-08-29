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

#include "drm_atomic_committer.h"

#include "drm_display.h"
#include "log.h"

namespace FT {
namespace drm {
DrmAtomicCommitter::DrmAtomicCommitter(int drmFd, int flags, void *userData)
    : drmFd_(drmFd), req_(drmModeAtomicAlloc()), flags_(flags), userData_(userData)
{}

DrmAtomicCommitter::~DrmAtomicCommitter() noexcept
{
    drmModeAtomicFree(req_);
}

void DrmAtomicCommitter::AddAtomicProperty(uint32_t objId, uint32_t propId, uint64_t value)
{
    if (OE_UNLIKELY(req_ == nullptr)) {
        LOG_ERROR("DrmAtomicCommitter::AddAtomicProperty: req_ is nullptr!");
        return;
    }

    int ret = drmModeAtomicAddProperty(req_, objId, propId, value);
    if (ret < 0) {
        LOG_WARN("drmModeAtomicAddProperty failed, err: %{public}s", ErrnoToString(errno).c_str());
    }
}

void DrmAtomicCommitter::Commit()
{
    LOG_DEBUG("DrmAtomicCommitter::Commit: drmFd: %{public}i, flags: %{public}i, userData: %{public}p", drmFd_, flags_, userData_);
    int ret = drmModeAtomicCommit(drmFd_, req_, flags_, userData_);
    if (ret < 0) {
        LOG_ERROR("DrmAtomicCommitter::Commit: failed, err: %{public}d, %{public}s", ret, ErrnoToString(errno).c_str());
    }
}
} // namespace drm
} // namespace FT
