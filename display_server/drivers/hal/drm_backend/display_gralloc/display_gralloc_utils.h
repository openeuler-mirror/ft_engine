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

#include "buffer_handle.h"

#include <optional>
#include <cstdint>

namespace GrallocUtils {

/**
 * @brief Internal func: Get the DRM fd from HdiSession Singleton
 *
 * @return int DRM fd
 */
int GetDrmFdFromSession();

/**
 * @brief Internal func: sync DMA-buffer for CPU access.
 *
 * For more information, please see comment of dma_buf_sync in <linux/dma-buf.h>
 * @param handle buffer handle
 * @param isStart start or stop of a map access session.
 * @return Display ErrCode
 */
int32_t DmaBufferSync(const BufferHandle *handle, bool isStart);

/**
 * @brief Reopen the DRM node.
 *
 * This function is aim to avoid GEM handle ref'counting issues
 * (ref: https://gitlab.freedesktop.org/mesa/drm/-/merge_requests/110)
 *
 * Generally, we'll get /dev/dri/renderD128 after reopen drm_fd /dev/dri/card0.
 * @param drmFd
 * @param enableRenderNode use render node if true. Otherwise, use primary node.
 * @return int new fd
 */
int ReopenDrmFd(int drmFd, bool enableRenderNode = true);

} // namespace GrallocUtils
