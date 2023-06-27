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

#include "dumb_allocator.h"

// std
#include <sys/mman.h> // mmap
#include <mutex>
// drm
#include <drm.h>
#include <drm_mode.h>
#include <drm_fourcc.h>
#include <xf86drm.h>

#include "display_gralloc_utils.h"
#include "hi_drm_format.h"
#include "display_gralloc.h"
#include "display_type.h"
#include "display_gralloc_private.h"
#include "hdi_session.h"
#include "log.h"

namespace oewm {
namespace HDI {
namespace DISPLAY {

int32_t DumbAllocator::Init()
{
    LOG_DEBUG("[Gralloc::DumbAllocator::Init] Initing...");

    // Get DRM fd from HDI Session
    int drmFd = GrallocUtils::GetDrmFdFromSession();
    if (IsInvalidFd(drmFd)) {
        LOG_ERROR("[Gralloc::DumbAllocator::Init] Failed to get DRM fd from HdiSession!");
        return DISPLAY_FD_ERR;
    }
    drmFd_ = drmFd;

    // TODO: enable reopen DrmFd
    // int newFd = GrallocUtils::ReopenDrmFd(drmFd);
    // if (IsInvalidFd(newFd)) {
    //     LOG_ERROR << "[Gralloc::DumbAllocator::Init] Failed to reopen DRM fd!";
    //     return DISPLAY_FD_ERR;
    // }
    // drmFd_ = newFd;

    LOG_DEBUG("[Gralloc::DumbAllocator::Init] Init done.");

    return DISPLAY_SUCCESS;
}

int32_t DumbAllocator::AllocMem(const AllocInfo &info, BufferHandle **bufferPtr)
{
    if (bufferPtr == nullptr) {
        LOG_ERROR("[Gralloc::DumbAllocator::AllocMem] Get nullptr param: `buffer`");
        return DISPLAY_PARAM_ERR;
    }

    /* Get format info */
    LOG_DEBUG("[Gralloc::DumbAllocator::AllocMem] Get format info");
    uint32_t drmFmt = DrmFormat::ConvertPixelFormatToDrmFormat(info.format);
    if (drmFmt == DRM_FORMAT_INVALID) {
        LOG_DEBUG("[Gralloc::DumbAllocator::AllocMem] Create dumb buffer");
    }
    const DrmFormat::DrmFormatInfo *fmtInfo = DrmFormat::GetDrmFormatInfo(drmFmt);

    {
        std::lock_guard<std::mutex> lock(mutex_);

        /* DRM RAW: Create dumb buffer */
        LOG_DEBUG("[Gralloc::DumbAllocator::AllocMem] Create dumb buffer");
        struct drm_mode_create_dumb create {};
        create.height = info.height;
        create.width = info.width;
        create.bpp = fmtInfo->bpp; // bits per pixel

        if (drmIoctl(drmFd_, DRM_IOCTL_MODE_CREATE_DUMB, &create) != 0) {
            LOG_ERROR("[Gralloc::DumbAllocator::AllocMem] Failed to create DRM dumb buffer: %{public}s"
                , ErrnoToString(errno).c_str());
            return DISPLAY_NOMEM;
        }

        /* Convert GEM dumb buffer handle to DMA-BUF prime fd
         *
         * tips: If DRM doesn't support "DRM_CAP_ADDFB2_MODIFIERS" capability, "drmPrimeHandleToFD()" function will
         *       cause crash. Thus, we have to check "supportGbmModifiers_" of DrmDevice which represents
         *       "DRM_CAP_ADDFB2_MODIFIERS" cap support.
         */
        int primeFd = -1;
        PriBufferHandle *priBuffer = new PriBufferHandle();
        LOG_DEBUG("[Gralloc::DumbAllocator::AllocMem] convert GEM dumb buffer handle to DMA-BUF prime fd");
        if (drmPrimeHandleToFD(drmFd_, create.handle, DRM_CLOEXEC, &primeFd) != 0) {
            LOG_WARN("[Gralloc::DumbAllocator::AllocMem] Failed to convert GEM handle %{public}u on DRM %{public}d : %{public}s",
                create.handle, drmFd_, ErrnoToString(errno).c_str());
            // return DISPLAY_FD_ERR; // For now, DumbBufferAllocator won't promise fd is available.
        }

        /* Set buffer handle rst */
        LOG_DEBUG("[Gralloc::DumbAllocator::AllocMem] set buffer handle rst");
        priBuffer->hdl.fd = primeFd;
        priBuffer->hdl.width = create.width;
        priBuffer->hdl.stride = create.pitch;
        priBuffer->hdl.height = create.height;
        priBuffer->hdl.size = create.size;
        priBuffer->hdl.format = info.format;
        priBuffer->hdl.usage = info.usage;
        priBuffer->hdl.virAddr = nullptr;
        priBuffer->hdl.key = static_cast<int32_t>(create.handle); // set DMA handle to key
        // set to default value
        priBuffer->hdl.phyAddr = 0;
        priBuffer->hdl.reserveFds = 0;
        priBuffer->hdl.reserveInts = 0;

        *bufferPtr = &priBuffer->hdl;
    }

    return DISPLAY_SUCCESS;
}

int32_t DumbAllocator::FreeMem(BufferHandle *buffer)
{
    if (buffer == nullptr) {
        LOG_ERROR("Get nullptr param: `buffer`");
        return DISPLAY_PARAM_ERR;
    }

    /* Perform memory unmapping if mmap is not clear */
    if ((buffer->virAddr != nullptr) && Unmap(*buffer) != DISPLAY_SUCCESS) {
        LOG_ERROR("[Gralloc::DumbAllocator::FreeMem] Failed to unmap buffer");
    }

    /* Close Dumb buffer fds & handle*/
    // close current Dumb buffer fd
    if (buffer->fd >= 0) {
        ::close(buffer->fd);
        buffer->fd = -1;
    }
    // close reserved Dumb buffer fd
    const uint32_t reserveFds = buffer->reserveFds;
    for (uint32_t i = 0; i < reserveFds; i++) {
        if (buffer->reserve[i] >= 0) {
            close(buffer->reserve[i]);
            buffer->reserve[i] = -1;
        }
    }
    // close current Dumb buffer handle
    DestroyGemDumbHandle(static_cast<unsigned int>(buffer->key));

    /* Free BufferHandle */
    delete buffer;

    return DISPLAY_SUCCESS;
}

void *DumbAllocator::Mmap(BufferHandle &buffer)
{
    if (buffer.virAddr != nullptr) {
        LOG_ERROR("Get err param: buffer.virAddr is not null");
        return nullptr;
    }

    /**
     * Get GEM Dumb buffer handle.
     *
     * tips: In some case, "drmPrimeHandleToFD()" function will cause crash. When converion failed, we will directly
     *       use handle in BufferHandle.key. This behavior might cause "MAP DUMB" failed when the buffer handle is
     *       passed across processes.
     */
    uint32_t handle;
    LOG_DEBUG("[Gralloc::DumbAllocator::Mmap] Try DMA-BUF prime fd to GEM dumb buffer handle");
    if (drmPrimeFDToHandle(drmFd_, buffer.fd, &handle) != 0) {
        LOG_WARN("[Gralloc::DumbAllocator::Mmap] Failed to convert DMA-BUF prime fd: %{public}i", buffer.fd);
        handle = buffer.key;
    }

    /* Prepare buffer for mmap */
    LOG_DEBUG("[Gralloc::DumbAllocator::Mmap] prepare buffer for mmap. handle = %{public}u", handle);
    struct drm_mode_map_dumb map {};
    map.handle = handle;
    if (drmIoctl(drmFd_, DRM_IOCTL_MODE_MAP_DUMB, &map) != 0) {
        LOG_ERROR("[Gralloc::DumbAllocator::Mmap] Failed to map DRM dumb buffer: %{public}s", ErrnoToString(errno).c_str());
        return nullptr;
    }

    /* Perform memory mapping */
    LOG_DEBUG("[Gralloc::DumbAllocator::Mmap] perform memory mapping");
    void *newVirAddr =
        mmap(static_cast<void *>(0), buffer.size, PROT_READ | PROT_WRITE, MAP_SHARED, drmFd_, map.offset);
    if (newVirAddr == MAP_FAILED) {
        LOG_ERROR("[Gralloc::DumbAllocator::Mmap] Failed to map buffer on DRM fd(%{public}d): %{public}s",
            drmFd_, ErrnoToString(errno).c_str());
        return nullptr;
    }

    /* Set new map to buffer struct */
    LOG_DEBUG("[Gralloc::DumbAllocator::Mmap] set new map to buffer struct");
    buffer.virAddr = newVirAddr;

    return newVirAddr;
}

int32_t DumbAllocator::Unmap(BufferHandle &buffer)
{
    if (buffer.virAddr == nullptr) {
        LOG_ERROR("[Gralloc::DumbAllocator::Mmap] Get null buffer.virAddr");
        return DISPLAY_NULL_PTR;
    }

    if (buffer.size < 0) {
        LOG_ERROR("[Gralloc::DumbAllocator::Mmap] Get st0 buffer.size");
        return DISPLAY_PARAM_ERR;
    }

    if (munmap(buffer.virAddr, buffer.size) != 0) {
        LOG_ERROR("[Gralloc::DumbAllocator::Mmap] Failed to do munmap(): %{public}s", ErrnoToString(errno).c_str());
        return DISPLAY_FAILURE;
    }
    buffer.virAddr = nullptr;

    return DISPLAY_SUCCESS;
}

int32_t DumbAllocator::FlushCache(BufferHandle &buffer)
{
    // TODO: FlushCache is not supported yet.
    // return GrallocUtils::DmaBufferSync(&buffer, false);
    UNUSED(buffer);
    return DISPLAY_SUCCESS;
}

int32_t DumbAllocator::InvalidateCache(BufferHandle &buffer)
{
    // TODO: FlushCache is not supported yet.
    // return GrallocUtils::DmaBufferSync(&buffer, true);
    UNUSED(buffer);
    return DISPLAY_SUCCESS;
}

int32_t DumbAllocator::DestroyGemDumbHandle(unsigned int handle)
{
    struct drm_mode_destroy_dumb destroy = {.handle = handle};
    if (drmIoctl(drmFd_, DRM_IOCTL_MODE_DESTROY_DUMB, &destroy) != 0) {
        LOG_WARN("[Gralloc::DumbAllocator::DestroyGemDumbHandle] Failed to destory GEM handle: %{public}u", handle);
        return DISPLAY_FAILURE;
    }

    return DISPLAY_SUCCESS;
}

bool DumbAllocator::GetSupportGbmModifyFromSession()
{
    auto &session = oewm::HDI::DISPLAY::HdiSession::GetInstance();
    const std::shared_ptr<oewm::drm::DrmDevice> device = session.GetDisplayDevice();
    if (device == nullptr) {
        LOG_ERROR("[Gralloc::DumbAllocator::DestroyGemDumbHandle] Get empty DRM device from HdiSession.");
        return false;
    }
    return device->SupportGbmModifiers();
}
} // namespace DISPLAY
} // namespace HDI
} // namespace oewm
