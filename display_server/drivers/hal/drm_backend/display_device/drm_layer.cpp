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

#include "drm_layer.h"

#include "log.h"

namespace oewm {
namespace drm {
DrmFrameBuffer *DrmLayer::GetFrameBuffer(int drmFd)
{
    if (IsInvalidFd(drmFd)) {
        LOG_ERROR("DrmLayer::GetFrameBuffer: invalid drm fd");
        return nullptr;
    }

    HdiLayerBuffer *layerBuffer = GetCurrentBuffer();
    if (layerBuffer == nullptr) {
        LOG_ERROR("DrmLayer::GetFrameBuffer: GetCurrentBuffer is nullptr");
        return nullptr;
    }

    lastFrameBuffer_ = std::move(currentFrameBuffer_);

    const auto &bufferHandle = layerBuffer->GetBufferHandle();
    currentFrameBuffer_ = DrmFrameBuffer::CreateFromBufferHandle(drmFd, bufferHandle);
    if (currentFrameBuffer_ == nullptr) {
        LOG_ERROR("DrmLayer::GetFrameBuffer: create framebuffer from BufferHandle failed.");
        return nullptr;
    }

    return currentFrameBuffer_.get();
}
} // namespace drm
} // namespace oewm
