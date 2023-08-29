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

#include "drm_frame_buffer.h"

#include <drm_fourcc.h>
#include <sys/mman.h>

#include "log.h"

namespace FT {
namespace drm {
namespace detail {
// add fbId to fbInfo
bool AddFb(int drmFd, uint32_t fbHandle, FrameBufferInfo &fbInfo)
{
    ASSERT(!IsInvalidFd(drmFd));
    ASSERT(fbHandle != DRM_INVALID_OBJECT_ID);

    uint32_t handles[4] = {};
    uint32_t pitches[4] = {};
    uint32_t offsets[4] = {};

    handles[0] = fbHandle;
    pitches[0] = fbInfo.stride;
    offsets[0] = 0;

    printf("AddFb: fd=%d, width=%u, height=%u, pixel_format=DRM_FORMAT_XRGB8888, handle=%u, pitch=%u, offset=%u, fbId=%u\n",
        drmFd, fbInfo.width, fbInfo.height, handles[0], pitches[0], offsets[0], fbInfo.fbId);
    if (drmModeAddFB2(drmFd, fbInfo.width, fbInfo.height, DRM_FORMAT_XRGB8888, // need use DRM_FORMAT_XRGB8888
                      handles, pitches, offsets, &fbInfo.fbId, 0) != 0) {
        printf("drmModeAddFB2 failed, error: %s\n", ErrnoToString(errno).c_str());
        return false;
    }

    return true;
}

bool RemoveFb(int drmFd, uint32_t fbId)
{
    ASSERT(!IsInvalidFd(drmFd));
    ASSERT(fbId != DRM_INVALID_OBJECT_ID);

    if (::drmModeRmFB(drmFd, fbId) != 0) {
        LOG_ERROR("drmModeRmFB failed, error: %{public}s", ErrnoToString(errno).c_str());
        return false;
    }

    return true;
}

bool DestroyDumbHandle(int drmFd, uint32_t fbHandle)
{
    ASSERT(!IsInvalidFd(drmFd));
    ASSERT(fbHandle != DRM_INVALID_OBJECT_ID);

    struct drm_mode_destroy_dumb dumb {};
    dumb.handle = fbHandle;
    if (::drmIoctl(drmFd, DRM_IOCTL_MODE_DESTROY_DUMB, &dumb) != 0) {
        LOG_ERROR("DRM_IOCTL_MODE_DESTROY_DUMB error: %{public}s", ErrnoToString(errno).c_str());
        return false;
    }

    return true;
}
} // namespace detail

std::unique_ptr<DrmFrameBuffer> DrmFrameBuffer::CreateAsDumb(
    int drmFd,
    uint32_t width,
    uint32_t height,
    uint64_t usage,
    bool cleanup/* = true */)
{
    if (IsInvalidFd(drmFd)) {
        LOG_ERROR("DrmFrameBuffer::CreateAsDumb error: invalid drm fd!");
        return nullptr;
    }

    if (width == 0 || height == 0) {
        LOG_ERROR("DrmFrameBuffer::CreateAsDumb error: invalid width(%{public}" PRIu32 ") or height(%{public}" PRIu32 ")!", width, height);
        return nullptr;
    }

    // Create Dumb Buffer as the buffer of fb
    struct drm_mode_create_dumb create {};
    create.width = width;
    create.height = height;
    create.bpp = 32;
    create.flags = 0;
    if (drmIoctl(drmFd, DRM_IOCTL_MODE_CREATE_DUMB, &create) != 0) {
        LOG_ERROR("DrmFrameBuffer::CreateAsDumb error: failed to create DRM dumb: %{public}s", ErrnoToString(errno).c_str());
        return nullptr;
    }

    // Create fb class
    FrameBufferInfo fbInfo{};
    fbInfo.width = width;
    fbInfo.height = height;
    fbInfo.usage = usage;
    fbInfo.stride = create.pitch;
    fbInfo.size = create.size;
    if (!detail::AddFb(drmFd, create.handle, fbInfo)) {
        LOG_ERROR("DrmFrameBuffer::CreateAsDumb: AddFb failed!");
        (void)detail::DestroyDumbHandle(drmFd, create.handle);
        return nullptr;
    }
    auto fb = std::unique_ptr<DrmFrameBuffer>(new DrmFrameBuffer(drmFd, fbInfo, create.handle));

    // Perform cleanup as white
    if (cleanup == true) {
        fb->Cleanup(0xffffffff);
    }

    return fb;
}

std::unique_ptr<DrmFrameBuffer> DrmFrameBuffer::CreateFromBufferHandle(int drmFd, const BufferHandle &handle)
{
    if (IsInvalidFd(drmFd)) {
        LOG_ERROR("DrmFrameBuffer::CreateFromBufferHandle error: invalid drm fd!");
        return nullptr;
    }

    if (handle.width <= 0 || handle.height <= 0) {
        LOG_ERROR("DrmFrameBuffer::CreateFromBufferHandle error: invalid width(%{public}" PRIu32 ") or height(%{public}" PRIu32 ")!", handle.width, handle.height);
        return nullptr;
    }

    FrameBufferInfo fbInfo{};
    fbInfo.width = static_cast<uint32_t>(handle.width);
    fbInfo.height = static_cast<uint32_t>(handle.height);
    fbInfo.usage = handle.usage;
    fbInfo.stride = handle.stride;
    fbInfo.size = static_cast<uint64_t>(handle.size);
    fbInfo.virAddr = handle.virAddr;
    auto fbHandle = static_cast<uint32_t>(handle.key);
    if (!detail::AddFb(drmFd, fbHandle, fbInfo)) {
        LOG_ERROR("DrmFrameBuffer::CreateFromBufferHandle: AddFb failed!");
        return nullptr;
    }

    return std::unique_ptr<DrmFrameBuffer>(new DrmFrameBuffer(drmFd, fbInfo));
}

DrmFrameBuffer::~DrmFrameBuffer() noexcept
{
    if (IsInvalidFd(drmFd_)) {
        return;
    }

    std::string funInfo = "DrmFrameBuffer::~DrmFrameBuffer(fbId: " + std::to_string(info_.fbId) + "): ";
    // LOG_DEBUG("%{public}s Removing fb...", funInfo.c_str());

    // Remove fb
    if (info_.fbId != DRM_INVALID_OBJECT_ID) {
        if (!detail::RemoveFb(drmFd_, info_.fbId)) {
            LOG_ERROR("%{public}s free fdId failed, error: %{public}s", funInfo.c_str(), ErrnoToString(errno).c_str());
        }
    }

    // Free dumb buffer
    if (isCreateFromBufferHandle_ == false && fbHandle_ != DRM_INVALID_OBJECT_ID) {
        // unmap
        if (info_.virAddr != nullptr) {
            if (::munmap(info_.virAddr, info_.size) != 0) {
                LOG_ERROR("%{public}s munmap failed, error: %{public}s", funInfo.c_str(), ErrnoToString(errno).c_str());
            }
        }
        // destroy dumb
        if (!detail::DestroyDumbHandle(drmFd_, fbHandle_)) {
            LOG_ERROR("%{public}s DestroyDumbHandle failed, error: %{public}s", funInfo.c_str(), ErrnoToString(errno).c_str());
        }
    }
}

DrmFrameBuffer::DrmFrameBuffer(int drmFd, const FrameBufferInfo &fbInfo, uint32_t fbHandle)
    : drmFd_(drmFd), info_(fbInfo), isCreateFromBufferHandle_(false), fbHandle_(fbHandle)
{}

DrmFrameBuffer::DrmFrameBuffer(int drmFd, const FrameBufferInfo &fbInfo)
    : drmFd_(drmFd), info_(fbInfo), isCreateFromBufferHandle_(true)
{}

void DrmFrameBuffer::Cleanup(uint32_t color)
{
    if (isCreateFromBufferHandle_ == true) {
        LOG_WARN("DrmFrameBuffer::Clean: Cannot cleanup fb created from buffer handle!");
        return;
    }

    // Mmap
    if (info_.virAddr == nullptr) {
        void *virAddr = nullptr;

        if (fbHandle_ != DRM_INVALID_OBJECT_ID) {
            struct drm_mode_map_dumb map {};
            map.handle = fbHandle_;
            if (drmIoctl(drmFd_, DRM_IOCTL_MODE_MAP_DUMB, &map) != 0) {
                LOG_ERROR("DrmFrameBuffer::Clean: Failed to map DRM dumb buffer: %{public}s", ErrnoToString(errno).c_str());
                return;
            }
            virAddr = ::mmap(virAddr, info_.size, PROT_READ | PROT_WRITE, MAP_SHARED, drmFd_, map.offset);
            if (virAddr == MAP_FAILED) {
                LOG_ERROR("DrmFrameBuffer::Clean: Failed to mmap DRM dumb buffer: %{public}s", ErrnoToString(errno).c_str());
                return;
            }
        }

        info_.virAddr = virAddr;
    }

    // Fill with given color
    if (info_.virAddr != nullptr) {
        std::fill(
            reinterpret_cast<uint8_t *>(info_.virAddr), reinterpret_cast<uint8_t *>(info_.virAddr) + info_.size, color);
    }

    // Unmap
    if (info_.virAddr != nullptr) {
        if (::munmap(info_.virAddr, info_.size) != 0) {
            LOG_ERROR("DrmFrameBuffer::Clean: munmap failed, error: %{public}s", ErrnoToString(errno).c_str());
        }
        info_.virAddr = nullptr;
    }
}
} // namespace drm
} // namespace FT
