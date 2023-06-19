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

#include "drm_common.h"

namespace oewm {
namespace drm {
// RAII object for drm auto atomic committing.
class DrmAtomicCommitter : NonCopyable {
public:
    explicit DrmAtomicCommitter(
        int drmFd,
        int flags,
        void *userData = nullptr);
    ~DrmAtomicCommitter() noexcept;

    void AddAtomicProperty(uint32_t objId, uint32_t propId, uint64_t value);
    void Commit();

private:
    int drmFd_ = INVALID_FD;
    drmModeAtomicReqPtr req_;
    int flags_;
    void *userData_ = nullptr;
};
} // namespace drm
} // namespace oewm