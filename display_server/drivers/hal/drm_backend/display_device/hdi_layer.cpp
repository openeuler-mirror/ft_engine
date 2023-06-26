/*
 * Copyright (c) 2021-2022 Huawei Technologies Co., Ltd.
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

#include "hdi_layer.h"

#include <libsync.h>

#include "refbase.h"
#include "sync_fence.h"
#include "log.h"
#include "types.h"

namespace oewm {
namespace HDI {
namespace DISPLAY {
HdiLayerBuffer::HdiLayerBuffer(const BufferHandle &handle)
    : handle_(handle),
      phyAddr_(handle.phyAddr),
      virAddr_(handle.virAddr),
      height_(handle.height),
      width_(handle.width),
      size_(handle.size),
      stride_(handle.stride),
      format_(handle.format)
{
    if (!IsInvalidFd(handle.fd)) {
        fd_ = OHOS::UniqueFd(::dup(handle.fd));
        if (fd_ < 0) {
            LOG_ERROR("Failed to dup from fd: %{public}i, err: %{public}s, errno: %{public}i", 
                handle.fd, ErrnoToString(errno).c_str(), errno);
        }
    }

    // LOG_DEBUG << "HdiLayerBuffer handle width: " << width_ << ", height: " << height_ << ", fd: " << fd_;
}

HdiLayerBuffer::~HdiLayerBuffer() noexcept {}

HdiLayer::HdiLayer(LayerId id, LayerType type) : id_(id), type_(type) {}

int32_t HdiLayer::SetSize(IRect *rect)
{
    if (rect == nullptr) {
        LOG_ERROR("HdiLayer::SetSize: rect is nullptr.");
        return DISPLAY_PARAM_ERR;
    }

    // LOG_DEBUG << "HdiLayer::SetSize: id: " << id_
    //           << Fmt(", rect x: %d y : %d w : %d h : %d", rect->x, rect->y, rect->w, rect->h);

    displayRect_ = *rect;
    return DISPLAY_SUCCESS;
}

int32_t HdiLayer::SetCrop(IRect *rect)
{
    if (rect == nullptr) {
        LOG_ERROR("HdiLayer::SetCrop: rect is nullptr.");
        return DISPLAY_PARAM_ERR;
    }

    // LOG_DEBUG << "HdiLayer::SetCrop: id: " << id_
    //           << Fmt(", crop x: %d y : %d w : %d h : %d", rect->x, rect->y, rect->w, rect->h);

    crop_ = *rect;
    return DISPLAY_SUCCESS;
}

int32_t HdiLayer::SetZOrder(uint32_t zOrder)
{
    // LOG_DEBUG << "HdiLayer::SetZOrder: id: " << id_ << ", zOrder: " << zOrder;
    zOrder_ = zOrder;
    return DISPLAY_SUCCESS;
}

int32_t HdiLayer::SetPreMulti(bool preMul)
{
    // LOG_DEBUG << "HdiLayer::SetPreMulti: id: " << id_ << ", preMul: " << preMul;
    preMulti_ = preMul;
    return DISPLAY_SUCCESS;
}

int32_t HdiLayer::SetAlpha(LayerAlpha *alpha)
{
    if (alpha == nullptr) {
        LOG_ERROR("HdiLayer::SetAlpha: alpha is nullptr.");
        return DISPLAY_PARAM_ERR;
    }

    // LOG_DEBUG << "HdiLayer::SetPreMulti: id: " << id_ << ", alpha: " << alpha->enGlobalAlpha
    //           << ", gAlpha: " << alpha->gAlpha;

    alpha_ = *alpha;
    return DISPLAY_SUCCESS;
}

int32_t HdiLayer::SetTransformMode(TransformType type)
{
    // LOG_DEBUG << "HdiLayer::TransformType: id: " << id_ << ", type: " << type;

    transformType_ = type;
    return DISPLAY_SUCCESS;
}

int32_t HdiLayer::SetDirtyRegion(IRect *region)
{
    if (region == nullptr) {
        LOG_ERROR("HdiLayer::SetDirtyRegion: region is nullptr.");
        return DISPLAY_PARAM_ERR;
    }

    // LOG_DEBUG << "HdiLayer::SetDirtyRegion: id: " << id_
    //           << Fmt(", region x: %d, y: %d, w: %d, h: %d", region->x, region->y, region->w, region->h);
    return DISPLAY_SUCCESS;
}

int32_t HdiLayer::SetVisibleRegion(uint32_t num, IRect *rect)
{
    if (rect == nullptr) {
        LOG_ERROR("HdiLayer::SetVisibleRegion: rect is nullptr.");
        return DISPLAY_PARAM_ERR;
    }
    // LOG_DEBUG << "HdiLayer::SetVisibleRegion: id: " << id_
    //           << Fmt(", rect x: %d, y: %d, w: %d, h: %d", rect->x, rect->y, rect->w, rect->h);
    return DISPLAY_SUCCESS;
}

int32_t HdiLayer::SetBuffer(const BufferHandle *handle, int32_t fence)
{
    if (handle == nullptr) {
        LOG_ERROR("HdiLayer::SetBuffer: handle is nullptr.");
        return DISPLAY_PARAM_ERR;
    }

    hdiBuffer_ = std::make_unique<HdiLayerBuffer>(*handle);
    acquireFence_ = OHOS::UniqueFd(dup(fence));
    return DISPLAY_SUCCESS;
}

int32_t HdiLayer::SetCompositionType(CompositionType type)
{
    // LOG_DEBUG << "HdiLayer::SetCompositionType: id: " << id_ << ", type: " << type;
    compositionType_ = type;
    return DISPLAY_SUCCESS;
}

int32_t HdiLayer::SetBlendType(BlendType type)
{
    // LOG_DEBUG << "HdiLayer::SetBlendType: id: " << id_ << ", type: " << type;
    blendType_ = type;
    return DISPLAY_SUCCESS;
}

void HdiLayer::SetPixel(const BufferHandle &handle, int x, int y, uint32_t color)
{
    const int32_t pixelBytes = 4;
    if (handle.format <= 0) {
        LOG_ERROR("HdiLayer::SetPixel: not supported format: %{public}i", handle.format);
        return;
    }

    if (handle.virAddr == nullptr) {
        LOG_ERROR("HdiLayer::SetPixel: virAddr is nullptr.");
        return;
    }

    if (x < 0 || x >= handle.width || y < 0 || y >= handle.height) {
        LOG_ERROR("HdiLayer::SetPixel: invalid parameter: x: %{public}d, bufWidth: %{public}d, y: %{public}d, bufHeight: %{public}d", 
            x, handle.width, y, handle.height);
        return;
    }

    int32_t position = y * handle.width + x;
    if ((position * pixelBytes) > handle.size) {
        LOG_ERROR("HdiLayer::SetPixel: position %{public}i is outside the buffer.", position);
        return;
    }
    uint32_t *pixel = reinterpret_cast<uint32_t *>(handle.virAddr) + position;
    *pixel = color;
}

void HdiLayer::ClearColor(uint32_t color)
{
    // LOG_DEBUG << "HdiLayer::ClearColor, color: " << color;
    const auto &handle = hdiBuffer_->GetBufferHandle();
    for (int32_t x = 0; x < handle.width; x++) {
        for (int32_t y = 0; y < handle.height; y++) {
            SetPixel(hdiBuffer_->GetBufferHandle(), x, y, color);
        }
    }
}

void HdiLayer::WaitAcquireFence()
{
    OHOS::sptr<OHOS::SyncFence> fence(new OHOS::SyncFence(acquireFence_.Release()));
    if (fence->IsValid()) {
        fence->Wait(fenceTimeOut_);
    }
}
} // namespace DISPLAY
} // namespace HDI
} // namespace oewm
