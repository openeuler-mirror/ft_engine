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

#include "gbm_allocator.h"

#include "display_gralloc_utils.h"
#include "display_gralloc_private.h"
#include "hi_gbm_format.h"
#include "display_type.h"
#include "log.h"
#include "types.h"

#include <drm.h>
#include <xf86drm.h>

namespace oewm {
namespace HDI {
namespace DISPLAY {
GbmAllocator::~GbmAllocator() noexcept
{
    if (gbmContext_ != nullptr) {
        gbm_device_destroy(gbmContext_);
        gbmContext_ = nullptr;
    }
}

int32_t GbmAllocator::Init()
{
    LOG_DEBUG("[Gralloc::GbmAllocator::Init] Initing...");

    /* Get DRM fd */
    int drmFd = GrallocUtils::GetDrmFdFromSession();
    if (IsInvalidFd(drmFd)) {
        LOG_ERROR("[Gralloc::GbmAllocator::Init] Failed to get DRM fd from HdiSession");
        return DISPLAY_FAILURE;
    }

    // TODO: reopen DRM fd

    /* create gbm device */
    gbmContext_ = gbm_create_device(drmFd);
    if (gbmContext_ == nullptr) {
        LOG_ERROR("[Gralloc::GbmAllocator::Init] Failed to create gbm device: %{public}s", ErrnoToString(errno).c_str());
        return DISPLAY_FAILURE;
    }

    LOG_INFO("[Gralloc::GbmAllocator::Init] Created GBM device with backend: %{public}s", 
        gbm_device_get_backend_name(gbmContext_));
    char *drmName = drmGetDeviceNameFromFd2(drmFd);
    LOG_INFO("[Gralloc::GbmAllocator::Init] Using DRM node: %{public}s", drmName);
    free(drmName);

    LOG_DEBUG("[Gralloc::GbmAllocator::Init] Init done.");

    return DISPLAY_SUCCESS;
}

int32_t GbmAllocator::AllocMem(const AllocInfo &info, BufferHandle **bufferPtr)
{
    LOG_DEBUG("[Gralloc::GbmAllocator::AllocMem] Alloc Mem.");

    if (bufferPtr == nullptr) {
        LOG_ERROR("[Gralloc::GbmAllocator::AllocMem] Get nullptr param: `buffer`");
        return DISPLAY_PARAM_ERR;
    }

    /* Decide flag depending on buffer usage */
    uint32_t gbmFlag = 0;
    if (info.usage & HBM_USE_MEM_FB) {
        gbmFlag |= GBM_BO_USE_RENDERING | GBM_BO_USE_SCANOUT;
    }

    // On some vm, it might failed to create GBM BO while allocating memory. In this case, we choose to create
    // Dumb buffer by `GBM_BO_USE_WRITE` flag.
    if (AllocMemWithUsage(info, bufferPtr, gbmFlag) != DISPLAY_SUCCESS) {
        LOG_WARN("[Gralloc::GbmAllocator::AllocMem] Gbm GL buffer allocation failed! Try to allocate dumb.");
        // Buffer can be used for gbm_bo_write.  This is guaranteed to work
        // with GBM_BO_USE_CURSOR, but may not work for other combinations.
        // **This will create Dumb Buffer!**
        gbmFlag |= GBM_BO_USE_WRITE;

        if (AllocMemWithUsage(info, bufferPtr, gbmFlag) != DISPLAY_SUCCESS) {
            LOG_ERROR("[Gralloc::GbmAllocator::AllocMem] Gbm Dumb buffer allocation failed!");
            return DISPLAY_FAILURE;
        }
    }

    LOG_DEBUG("[Gralloc::GbmAllocator::AllocMem] Alloc Mem done.");

    return DISPLAY_SUCCESS;
}

int32_t GbmAllocator::FreeMem(BufferHandle *buffer)
{
    LOG_DEBUG("[Gralloc::GbmAllocator::FreeMem] Free Mem.");

    if (buffer == nullptr) {
        LOG_ERROR("[Gralloc::GbmAllocator::FreeMem] Get nullptr param: `buffer`");
        return DISPLAY_NULL_PTR;
    }

    /* perform memory unmapping if mmap is not clear */
    if ((buffer->virAddr != nullptr) && Unmap(*buffer) != DISPLAY_SUCCESS) {
        LOG_WARN("[Gralloc::GbmAllocator::FreeMem] Failed to unmap buffer.");
    }

    /* close DMA-BUF fds*/
    // close current DMABUF fd
    if (buffer->fd >= 0) {
        ::close(buffer->fd);
        buffer->fd = -1;
    }
    // close reserved DMABUF fd
    const uint32_t reserveFds = buffer->reserveFds;
    for (uint32_t i = 0; i < reserveFds; i++) {
        if (buffer->reserve[i] >= 0) {
            ::close(buffer->reserve[i]);
            buffer->reserve[i] = -1;
        }
    }

    /* destroy gbm bo */
    int handle = buffer->key;
    if (DestroyGbmBo(handle) == false) {
        LOG_WARN("[Gralloc::GbmAllocator::FreeMem] Failed to destroy gbm bo.");
    }

    /* free buffer handle */
    delete buffer;

    LOG_DEBUG("[Gralloc::GbmAllocator::FreeMem] Free Mem done.");

    return DISPLAY_SUCCESS;
}

void *GbmAllocator::Mmap(BufferHandle &buffer)
{
    LOG_DEBUG("[Gralloc::GbmAllocator::Mmap] Mmap.");

    auto handle = static_cast<int32_t>(buffer.key);
    auto gbmBo = GetGbmBo(handle);
    if (gbmBo == nullptr) {
        LOG_ERROR("[Gralloc::GbmAllocator::Mmap] Get null gbmBo of this buffer!");
        return nullptr;
    }

    if (buffer.virAddr != nullptr) {
        LOG_WARN("[Gralloc::GbmAllocator::Mmap] `buffer.virAddr` is not null!");
        return buffer.virAddr;
    }

    // Decide mmap flags
    uint32_t transferFlags = 0;
    if (buffer.usage & HBM_USE_CPU_READ) {
        // Buffer contents read back (or accessed directly) at transfer create time.
        transferFlags |= GBM_BO_TRANSFER_READ;
    }
    if (buffer.usage & HBM_USE_CPU_WRITE) {
        // Buffer contents will be written back at unmap time (or modified as a result of being accessed directly).
        transferFlags |= GBM_BO_TRANSFER_WRITE;
    }

    // Check if current GBM buffer has impl `GBM_BO_USE_WRITE` flag.
    if (transferFlags == 0) {
        LOG_ERROR("[Gralloc::GbmAllocator::Mmap] GBM buffer without GBM_BO_USE_WRITE cannot be mmaped!");
        return nullptr;
    }

    uint32_t outStride = 0;
    void *mapData = nullptr;
    void *virAddr = gbm_bo_map(
        gbmBo,
        0,
        0,
        buffer.width,
        buffer.height,
        transferFlags,
        &outStride,
        &mapData); // map_data & return address is the same

    buffer.virAddr = virAddr;
    LOG_DEBUG("[Gralloc::GbmAllocator::Mmap] Mmap done.");
    return virAddr;
}

int32_t GbmAllocator::Unmap(BufferHandle &buffer)
{
    LOG_DEBUG("[Gralloc::GbmAllocator::Unmap] Unmap.");

    if (buffer.virAddr == nullptr) {
        LOG_ERROR("[Gralloc::GbmAllocator::Unmap] Get null buffer.virAddr!");
        return DISPLAY_NULL_PTR;
    }

    if (buffer.size < 0) {
        LOG_ERROR("[Gralloc::GbmAllocator::Unmap] Get st0 buffer.size!");
        return DISPLAY_PARAM_ERR;
    }

    auto handle = static_cast<int32_t>(buffer.key);
    auto gbmBo = GetGbmBo(handle);
    if (gbmBo == nullptr) {
        LOG_ERROR("[Gralloc::GbmAllocator::Unmap] Get null gbmBo of this buffer!");
        return DISPLAY_NULL_PTR;
    }

    gbm_bo_unmap(gbmBo, buffer.virAddr);
    buffer.virAddr = nullptr;
    
    LOG_DEBUG("[Gralloc::GbmAllocator::Unmap] Unmap done.");

    return DISPLAY_SUCCESS;
}

int32_t GbmAllocator::FlushCache(BufferHandle &buffer)
{
    LOG_DEBUG("[Gralloc::GbmAllocator::FlushCache] Flush Cache.");

    // TODO: FlushCache is not supported yet.
    UNUSED(buffer);

    LOG_DEBUG("[Gralloc::GbmAllocator::FlushCache] Flush Cache done.");
    return DISPLAY_SUCCESS;
}

int32_t GbmAllocator::InvalidateCache(BufferHandle &buffer)
{
    LOG_DEBUG("[Gralloc::GbmAllocator::InvalidateCache] Invalidate Cache.");

    // TODO: InvalidateCache is not supported yet.
    UNUSED(buffer);

    LOG_DEBUG("[Gralloc::GbmAllocator::InvalidateCache] Invalidate Cache done.");

    return DISPLAY_SUCCESS;
}

bool GbmAllocator::AddGbmBo(int32_t handle, struct gbm_bo *gbmBo)
{
    std::lock_guard<std::mutex> lock(mutex_);
    if (boMap_.count(handle) != 0) {
        return false;
    }

    boMap_.insert({handle, gbmBo});
    return true;
}

struct gbm_bo *GbmAllocator::GetGbmBo(int32_t handle)
{
    std::lock_guard<std::mutex> lock(mutex_);
    if (boMap_.count(handle) == 0) {
        return nullptr;
    }

    return boMap_.at(handle);
}

bool GbmAllocator::DestroyGbmBo(int32_t handle)
{
    std::lock_guard<std::mutex> lock(mutex_);
    if (boMap_.count(handle) == 0) {
        return false;
    }

    auto gbmBo = boMap_.at(handle);
    boMap_.erase(handle);
    if (gbmBo != nullptr) {
        gbm_bo_destroy(gbmBo);
    }

    return true;
}

int32_t GbmAllocator::AllocMemWithUsage(const AllocInfo &info, BufferHandle **bufferPtr, uint32_t usage)
{
    /* Get format info */
    LOG_DEBUG("[Gralloc::GbmAllocator::AllocMem] Get format info");
    uint32_t gbmFmt = GbmFormat::ConvertPixelFormatToGbmFormat(info.format);

    /* CORE: gbm bo */
    LOG_DEBUG("[Gralloc::GbmAllocator::AllocMemWithUsage] create GBM buffer object.");
    struct gbm_bo *gbmBo = gbm_bo_create(
        gbmContext_,
        info.width,
        info.height,
        gbmFmt,
        usage);
    if (gbmBo == nullptr) {
        LOG_ERROR("[Gralloc::GbmAllocator::AllocMemWithUsage] Failed to create GBM buffer object.");
        return DISPLAY_FAILURE;
    }

    /* Set buffer handle rst */
    LOG_DEBUG("[Gralloc::GbmAllocator::AllocMemWithUsage] Set buffer handle rst");
    PriBufferHandle *priBuffer = new PriBufferHandle();
    auto boHandle = static_cast<int32_t>(gbm_bo_get_handle(gbmBo).u32);

    priBuffer->hdl.fd = gbm_bo_get_fd(gbmBo); // TODO: use planes' fd
    if (priBuffer->hdl.fd < 0) {
        LOG_WARN("[Gralloc::GbmAllocator::AllocMemWithUsage] Failed to get fd from gbm bo.");
        // return DISPLAY_FAILURE;
    }
    priBuffer->hdl.stride = gbm_bo_get_stride(gbmBo);
    priBuffer->hdl.width = info.width;
    priBuffer->hdl.height = info.height;
    priBuffer->hdl.usage = info.usage;
    priBuffer->hdl.format = info.format;
    priBuffer->hdl.virAddr = nullptr;
    priBuffer->hdl.size = priBuffer->hdl.stride * priBuffer->hdl.height;
    priBuffer->hdl.key = boHandle;
    // set to default value
    priBuffer->hdl.phyAddr = 0;
    priBuffer->hdl.reserveFds = 0;
    priBuffer->hdl.reserveInts = 0;

    *bufferPtr = &priBuffer->hdl;

    AddGbmBo(boHandle, gbmBo);

    return DISPLAY_SUCCESS;
}
} // namespace DISPLAY
} // namespace HDI
} // namespace oewm
