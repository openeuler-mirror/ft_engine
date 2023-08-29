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

#pragma once

#include <unordered_set>
#include <memory>

#include "unique_fd.h"
#include "display_type.h"
#include "noncopyable_hal.h"

namespace FT {
namespace HDI {
namespace DISPLAY {
const uint32_t INVALID_VALUE = ~0x0;
using LayerId = uint32_t;
constexpr LayerId INVALID_LAYER_ID = ~0x0;
const int FENCE_TIMEOUT = 3000;

struct HdiLayerBuffer : NonCopyable {
public:
    explicit HdiLayerBuffer(const BufferHandle &handle);
    virtual ~HdiLayerBuffer() noexcept;

    const BufferHandle &GetBufferHandle() const
    {
        return handle_;
    }
    void *GetVirtualAddr() const
    {
        return virAddr_;
    }
    uint64_t GetPhysicalAddr() const
    {
        return phyAddr_;
    }
    int32_t GetHeight() const
    {
        return height_;
    }
    int32_t GetWight() const
    {
        return width_;
    }
    int32_t GetSize() const
    {
        return size_;
    }
    int32_t GetStride() const
    {
        return stride_;
    }
    int32_t GetFormat() const
    {
        return format_;
    }
    int Fd() const
    {
        return fd_;
    }

private:
    BufferHandle handle_;
    uint64_t phyAddr_ = 0;
    void *virAddr_ = nullptr;
    int32_t height_ = 0;
    int32_t width_ = 0;
    int32_t size_ = 0;
    int32_t stride_ = 0;
    int32_t format_ = 0;
    OHOS::UniqueFd fd_;
};

class HdiLayer : NonCopyable {
public:
    HdiLayer(LayerId id, LayerType type = LAYER_TYPE_GRAPHIC);
    virtual ~HdiLayer() noexcept = default;

    uint32_t GetId() const
    {
        return id_;
    }
    uint32_t GetZOrder() const
    {
        return zOrder_;
    }
    const IRect &GetLayerDisplayRect() const
    {
        return displayRect_;
    }
    const IRect &GetLayerCrop() const
    {
        return crop_;
    }
    bool GetLayerPreMulti() const
    {
        return preMulti_;
    }
    const LayerAlpha &GetAlpha() const
    {
        return alpha_;
    }
    LayerType GetType() const
    {
        return type_;
    }
    TransformType GetTransFormType() const
    {
        return transformType_;
    }
    BlendType GetLayerBlenType() const
    {
        return blendType_;
    }
    CompositionType GetCompositionType() const
    {
        return compositionType_;
    }
    void SetDeviceSelect(CompositionType type)
    {
        deviceSelect_ = type;
    }
    CompositionType GetDeviceSelect() const
    {
        return deviceSelect_;
    }
    int GetAcquireFenceFd()
    {
        return acquireFence_.Get();
    }
    int GetReleaseFenceFd()
    {
        return releaseFence_.Get();
    }
    void SetReleaseFence(int fd)
    {
        releaseFence_ = OHOS::UniqueFd(::dup(fd));
    };

    void ClearColor(uint32_t color);
    void SetPixel(const BufferHandle &handle, int x, int y, uint32_t color);

    void WaitAcquireFence();
    virtual int32_t SetSize(IRect *rect);
    virtual int32_t SetCrop(IRect *rect);
    virtual int32_t SetZOrder(uint32_t zOrder);
    virtual int32_t SetPreMulti(bool preMul);
    virtual int32_t SetAlpha(LayerAlpha *alpha);
    virtual int32_t SetTransformMode(TransformType type);
    virtual int32_t SetDirtyRegion(IRect *region);
    virtual int32_t SetVisibleRegion(uint32_t num, IRect *rect);
    virtual int32_t SetBuffer(const BufferHandle *handle, int32_t fence);
    virtual int32_t SetCompositionType(CompositionType type);
    virtual int32_t SetBlendType(BlendType type);
    virtual HdiLayerBuffer *GetCurrentBuffer()
    {
        return hdiBuffer_.get();
    }

    int32_t SetTunnelHandle(ExtDataHandle *handle)
    {
        tunnelHandle_ = handle;
        return DISPLAY_SUCCESS;
    }
    ExtDataHandle *tunnelHandle_ = nullptr;

private:
    uint32_t id_ = INVALID_LAYER_ID;
    LayerType type_ = LAYER_TYPE_GRAPHIC;
    OHOS::UniqueFd acquireFence_;
    OHOS::UniqueFd releaseFence_;

    IRect displayRect_ = {};
    IRect crop_ = {};
    uint32_t zOrder_ = INVALID_VALUE;
    bool preMulti_ = false;
    LayerAlpha alpha_ = {};
    int fenceTimeOut_ = FENCE_TIMEOUT;
    TransformType transformType_ = ROTATE_BUTT;
    CompositionType compositionType_ = COMPOSITION_CLIENT;
    CompositionType deviceSelect_ = COMPOSITION_CLIENT;
    BlendType blendType_ = BLEND_NONE;
    std::unique_ptr<HdiLayerBuffer> hdiBuffer_;
};

inline bool operator<(const HdiLayer &lhs, const HdiLayer &rhs)
{
    return lhs.GetZOrder() < rhs.GetZOrder();
}
inline bool operator<=(const HdiLayer &lhs, const HdiLayer &rhs)
{
    return lhs.GetZOrder() <= rhs.GetZOrder();
}
inline bool operator>(const HdiLayer &lhs, const HdiLayer &rhs)
{
    return !(lhs <= rhs);
}
inline bool operator>=(const HdiLayer &lhs, const HdiLayer &rhs)
{
    return !(lhs < rhs);
}
} // namespace DISPLAY
} // namespace HDI

namespace drm {
using HdiLayerId = HDI::DISPLAY::LayerId;
using HdiLayer = HDI::DISPLAY::HdiLayer;
using HdiLayerBuffer = HDI::DISPLAY::HdiLayerBuffer;
} // namespace drm
} // namespace FT
