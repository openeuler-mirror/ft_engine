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

#include "shm_allocator.h"

#include "display_gralloc_private.h"
#include "display_gralloc_utils.h"
#include "log.h"
#include "types.h"
#include "hi_drm_format.h" // TMP

#include <cinttypes>
#include <sys/mman.h> // for shm_xxx

namespace oewm {
namespace HDI {
namespace DISPLAY {

#define RANDNAME_PATTERN "/oewm-shm-XXXXXX"

int32_t ShmAllocator::AllocMem(const AllocInfo &info, BufferHandle **bufferPtr)
{
    if (bufferPtr == nullptr) {
        LOG_ERROR("[Gralloc::ShmAllocator::AllocMem] Get nullptr param: `buffer`");
        return DISPLAY_PARAM_ERR;
    }

    PriBufferHandle *priBuffer = new PriBufferHandle();

    /* Get format info */
    // TODO: only get bpp from PixelFormat
    LOG_DEBUG("[Gralloc::ShmAllocator::AllocMem] Get format info for bpp.");
    uint32_t drmFmt = DrmFormat::ConvertPixelFormatToDrmFormat(info.format);
    const DrmFormat::DrmFormatInfo *fmtInfo = DrmFormat::GetDrmFormatInfo(drmFmt);

    uint32_t bytesPerPixel = fmtInfo->bpp / 8;
    int32_t stride = static_cast<int32_t>(info.width * bytesPerPixel); // TODO: align?
    int32_t size = stride * static_cast<int32_t>(info.height);

    if (size != static_cast<int32_t>(info.expectedSize)) {
        LOG_WARN("[Gralloc::ShmAllocator::AllocMem] size(%{public}d) is not equal to user expected size(%{public}u)!",
            size, info.expectedSize);
    }

    {
        std::lock_guard<std::mutex> lock(mutex_);

        /* Create shm anonymous file */
        int32_t shmFd = static_cast<int32_t>(AllocateShmFile(static_cast<size_t>(size)));
        if (shmFd < 0) {
            LOG_ERROR("[Gralloc::ShmAllocator::AllocMem] Failed to create shm file for %{public}" PRId32 " Bytes", size);
            return DISPLAY_FD_ERR;
        }

        /* Set buffer handle rst */
        priBuffer->hdl.fd = shmFd;
        priBuffer->hdl.reserveFds = 0;
        priBuffer->hdl.reserveInts = 0;
        priBuffer->hdl.stride = stride;
        priBuffer->hdl.width = info.width;
        priBuffer->hdl.height = info.height;
        priBuffer->hdl.usage = info.usage;
        priBuffer->hdl.format = info.format;
        priBuffer->hdl.virAddr = nullptr;
        priBuffer->hdl.size = size;

        *bufferPtr = &priBuffer->hdl;
    }

    return DISPLAY_SUCCESS;
}

int32_t ShmAllocator::FreeMem(BufferHandle *buffer)
{
    if (buffer == nullptr) {
        LOG_ERROR("[Gralloc::ShmAllocator::FreeMem] Get nullptr param: `buffer`");
        return DISPLAY_PARAM_ERR;
    }

    /* perform memory unmapping if mmap is not clear */
    if ((buffer->virAddr != nullptr) && Unmap(*buffer) != DISPLAY_SUCCESS) {
        LOG_ERROR("[Gralloc::ShmAllocator::FreeMem] Failed to free shm buffer mmap!");
    }

    /* close shm fd */
    if (buffer->fd >= 0) {
        ::close(buffer->fd);
    }

    return DISPLAY_SUCCESS;
}

void *ShmAllocator::Mmap(BufferHandle &buffer)
{
    if (buffer.size < 0) {
        LOG_ERROR("[Gralloc::ShmAllocator::Mmap] Get st0 buffer.size");
        return nullptr;
    }
    if (buffer.fd < 0) {
        LOG_ERROR("[Gralloc::ShmAllocator::Mmap] Get st0 buffer.fd");
        return nullptr;
    }
    if (buffer.virAddr != nullptr) {
        LOG_WARN("[Gralloc::ShmAllocator::Mmap] buffer.virAddr is not empty");
        return buffer.virAddr;
    }

    void *data = mmap(nullptr, buffer.size, PROT_READ | PROT_WRITE, MAP_SHARED, buffer.fd, 0);
    if (data == MAP_FAILED) {
        LOG_ERROR("[Gralloc::ShmAllocator::Mmap] Failed to mmap for shm fd: %{public}i", buffer.fd);
        return nullptr;
    }

    buffer.virAddr = data;

    return data;
}

int32_t ShmAllocator::Unmap(BufferHandle &buffer)
{
    if (buffer.virAddr == nullptr) {
        LOG_ERROR("[Gralloc::ShmAllocator::Unmap] Get null buffer.virAddr");
        return DISPLAY_NULL_PTR;
    }

    if (buffer.size < 0) {
        LOG_ERROR("[Gralloc::ShmAllocator::Unmap] Get st0 buffer.size");
        return DISPLAY_PARAM_ERR;
    }

    if (munmap(buffer.virAddr, static_cast<size_t>(buffer.size)) != 0) {
        LOG_ERROR("[Gralloc::ShmAllocator::Unmap] Failed to unmap shm buffer (%{public}p)", buffer.virAddr);
        return DISPLAY_FAILURE;
    }

    return DISPLAY_SUCCESS;
}

int32_t ShmAllocator::FlushCache(BufferHandle &buffer)
{
    // TODO: FlushCache is not supported yet.
    UNUSED(buffer);
    return DISPLAY_FAILURE;
}

int32_t ShmAllocator::InvalidateCache(BufferHandle &buffer)
{
    // TODO: InvalidateCache is not supported yet.
    UNUSED(buffer);
    return DISPLAY_FAILURE;
}

void ShmAllocator::Randname(char *buf)
{
    struct timespec ts;
    clock_gettime(CLOCK_REALTIME, &ts);
    long r = ts.tv_nsec;
    for (int i = 0; i < 6; ++i) {
        buf[i] = 'A' + (r & 15) + (r & 16) * 2;
        r >>= 5;
    }
}

int ShmAllocator::ExclShmOpen(char *name)
{
    int retries = 100;
    do {
        Randname(name + strlen(RANDNAME_PATTERN) - 6);

        --retries;
        // CLOEXEC is guaranteed to be set by shm_open
        int fd = shm_open(name, O_RDWR | O_CREAT | O_EXCL, 0600);
        if (fd >= 0) {
            return fd;
        }
    } while (retries > 0 && errno == EEXIST);

    return -1;
}

int ShmAllocator::AllocateShmFile(size_t size)
{
    char name[] = RANDNAME_PATTERN;
    int fd = ExclShmOpen(name);
    if (fd < 0) {
        return -1;
    }
    shm_unlink(name);

    int ret;
    do {
        ret = ftruncate(fd, size);
    } while (ret < 0 && errno == EINTR);
    if (ret < 0) {
        close(fd);
        return -1;
    }

    return fd;
}

} // namespace DISPLAY
} // namespace HDI
} // namespace oewm
