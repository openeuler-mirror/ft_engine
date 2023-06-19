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

#include "display_gralloc_utils.h"

#include "log.h"
#include "hdi_session.h"

// driver
#include <linux/dma-buf.h>

namespace GrallocUtils {
int GetDrmFdFromSession()
{
    const auto &session = oewm::HDI::DISPLAY::HdiSession::GetInstance();
    const auto &device = session.GetDisplayDevice();
    if (device == nullptr) {
        LOG_ERROR("[Gralloc] Get empty DRM device from HdiSession.");
        return -1;
    }

    return device->Fd();
}

int32_t DmaBufferSync(const BufferHandle *handle, bool isStart)
{
    // LOG_DEBUG << "[Gralloc] Sync DMA-BUF mmap cache : " << (isStart ? "invalidate" : "flush");
    const int RETRY_TIMES = 6;

    struct dma_buf_sync syncFlag = {.flags = 0};

    /* set read/write flag */
    if (handle->usage & HBM_USE_CPU_WRITE) {
        syncFlag.flags |= DMA_BUF_SYNC_WRITE;
    }
    if (handle->usage & HBM_USE_CPU_READ) {
        syncFlag.flags |= DMA_BUF_SYNC_READ;
    }

    /* set start/end flag */
    if (isStart) {
        syncFlag.flags |= DMA_BUF_SYNC_START;
    } else {
        syncFlag.flags |= DMA_BUF_SYNC_END;
    }

    /* try DMA-BUF ioctl sync */
    int retry = RETRY_TIMES;
    int ret;
    do {
        ret = ioctl(handle->fd, DMA_BUF_IOCTL_SYNC, &syncFlag);
    } while ((retry--) && (ret != -EAGAIN) && (ret != -EINTR));
    // error code ref: https://docs.kernel.org/driver-api/dma-buf.html

    if (ret < 0) {
        LOG_ERROR("[Gralloc] Failed to sync DMA-BUF");
        return DISPLAY_SYS_BUSY;
    }
    return DISPLAY_SUCCESS;
}

int ReopenDrmFd(int drmFd, bool enableRenderNode /*= true*/)
{
    /* Try to open render node */
    char *deviceName = nullptr;
    if (enableRenderNode == true) {
        deviceName = drmGetRenderDeviceNameFromFd(drmFd);
    }

    /* Try to open primary node if previous operation failed */
    if (deviceName == nullptr) {
        // Either the DRM device has no render node, either the caller wants
        // a primary node
        deviceName = drmGetDeviceNameFromFd2(drmFd);
        if (deviceName == nullptr) {
            LOG_ERROR("[Gralloc] Failed to get device name from DRM fd: %{public}i", drmFd);
            return -1;
        }
    }

    /* Reopen DRM fd */
    int newFd = ::open(deviceName, O_RDWR | O_CLOEXEC);
    if (newFd < 0) {
        LOG_ERROR("[Gralloc] Failed to reopen DRM node: %{public}s", deviceName);
        free(deviceName);
        return -1;
    }

    LOG_INFO("[Gralloc::GbmUtils::ReopenDrmFd] Reopen new device '%{public}s'(%{public}d) from DRM fd(%{public}d).", 
        deviceName, newFd, drmFd);

    /* Clear */
    free(deviceName);

    /**
     * Auth for new DRM fd permission
     *
     * If we're using a DRM primary node (e.g. because we're running under the
     * to use the legacy DRM authentication mechanism to have the permission to
     * manipulate buffers.
     */
    if (drmGetNodeTypeFromFd(newFd) == DRM_NODE_PRIMARY) {
        drm_magic_t magic;
        if (drmGetMagic(newFd, &magic) < 0) {
            LOG_ERROR("[Gralloc] Failed to get DRM magic.");
            ::close(newFd);
            return -1;
        }

        if (drmAuthMagic(drmFd, magic) < 0) {
            LOG_ERROR("[Gralloc] Failed to auth DRM magic.");
            ::close(newFd);
            return -1;
        }
    }

    return newFd;
}
} // namespace GrallocUtils
