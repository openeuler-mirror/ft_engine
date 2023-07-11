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

#include <memory>
#include <gbm.h>

#include "display_type.h"
#include "drm_common.h"

namespace oewm {
namespace drm {
struct FrameBufferInfo {
    uint32_t fbId = DRM_INVALID_OBJECT_ID;
    void *virAddr = nullptr;
    uint64_t usage = 0;
    uint32_t width = 0;
    uint32_t height = 0;
    uint32_t stride = 0; // framebuffer stride: how many bytes per line.
    uint64_t size = 0;   // framebuffer total size(in bytes).
};

class DrmFrameBuffer : NonCopyable {
public:
    static std::unique_ptr<DrmFrameBuffer> CreateAsDumb(int drmFd, uint32_t width, uint32_t height, uint64_t usage, bool cleanup = true);
    static std::unique_ptr<DrmFrameBuffer> CreateFromBufferHandle(int drmFd, const BufferHandle &handle);

    ~DrmFrameBuffer() noexcept;

    uint32_t GetFbId() const
    {
        return info_.fbId;
    }
    uint32_t GetFbWidth() const
    {
        return info_.width;
    }
    uint32_t GetFbHeight() const
    {
        return info_.height;
    }

private:
    // we hide these constructors to ensure that all DrmFrameBuffer instances are valid.
    DrmFrameBuffer(int drmFd, const FrameBufferInfo &fbInfo, uint32_t fbHandle);
    DrmFrameBuffer(int drmFd, const FrameBufferInfo &fbInfo);

    void Cleanup(uint32_t color);

private:
    // common
    int drmFd_ = INVALID_FD;
    FrameBufferInfo info_;
    bool isCreateFromBufferHandle_;

    // dumb fb object
    uint32_t fbHandle_ = DRM_INVALID_OBJECT_ID;
};
} // namespace drm
} // namespace oewm
