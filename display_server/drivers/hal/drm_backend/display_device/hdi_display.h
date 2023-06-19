/*
 * Copyright (c) 2021 Huawei Device Co., Ltd.
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

#include <queue>
#include <unordered_map>

#include "display_device.h"
#include "hdi_layer.h"

namespace oewm {
namespace HDI {
namespace DISPLAY {
using DisplayId = uint32_t;
constexpr DisplayId INVALID_DISPLAY_ID = ~0x0;
constexpr uint32_t DISPLAY_TYPE_DRM = (1 << 31);

class VsyncCallBack {
public:
    VsyncCallBack(VBlankCallback cb, void *data, uint32_t pipe);
    virtual void Vsync(unsigned int sequence, uint64_t ns);
    virtual ~VsyncCallBack() {}
    uint32_t GetPipe()
    {
        return mPipe;
    }

private:
    VBlankCallback mVBlankCb;
    void *mData;
    uint32_t mPipe;
};

class HdiDisplay {
public:
    HdiDisplay();
    virtual ~HdiDisplay() noexcept = default;

    virtual bool Init();
    virtual DisplayId Id() const = 0;
    virtual bool IsConnected() = 0;
    virtual int32_t GetDisplayCapability(DisplayCapability *info) = 0;
    virtual int32_t GetDisplaySupportedModes(uint32_t *num, DisplayModeInfo *modes) = 0;
    virtual int32_t GetDisplayMode(uint32_t *modeId) = 0;
    virtual int32_t SetDisplayMode(uint32_t modeId) = 0;
    virtual int32_t GetDisplayPowerStatus(DispPowerStatus *status) = 0;
    virtual int32_t SetDisplayPowerStatus(DispPowerStatus status) = 0;
    virtual int32_t GetDisplayBacklight(uint32_t *value) = 0;
    virtual int32_t SetDisplayBacklight(uint32_t value) = 0;
    virtual int32_t RegDisplayVBlankCallback(VBlankCallback cb, void *data) = 0;
    virtual void OnVSync(uint32_t sequence, uint64_t timeStamp) = 0;
    virtual int32_t SetDisplayVsyncEnabled(bool enabled) = 0;
    virtual int32_t GetDisplayReleaseFence(uint32_t *num, uint32_t *layers, int32_t *fences) = 0;
    virtual int32_t GetDisplaySupportedColorGamuts(uint32_t *num, ColorGamut *gamuts) = 0;
    virtual int32_t GetDisplayColorGamut(ColorGamut *gamut) = 0;
    virtual int32_t SetDisplayColorGamut(ColorGamut gamut) = 0;
    virtual int32_t GetDisplayGamutMap(GamutMap *gamutMap) = 0;
    virtual int32_t SetDisplayGamutMap(GamutMap gamutMap) = 0;
    virtual int32_t GetHDRCapabilityInfos(HDRCapability *info) = 0;
    virtual int32_t GetSupportedMetadataKey(uint32_t *num, HDRMetadataKey *keys) = 0;
    virtual int32_t SetDisplayClientBuffer(const BufferHandle *buffer, int32_t fence) = 0;

    virtual int32_t CreateLayer(const LayerInfo *layerInfo, LayerId *layerId);
    virtual int32_t CloseLayer(LayerId layerId);

    virtual int32_t PrepareDisplayLayers(bool *needFlushFb) = 0;
    virtual int32_t GetDisplayCompChange(uint32_t *num, uint32_t *layers, int32_t *type) = 0;
    virtual int32_t Commit(int32_t *fence) = 0;

    HdiLayer *GetHdiLayer(LayerId id);

protected:
    LayerId GenerateLayerId();
    virtual std::unique_ptr<HdiLayer> CreateHdiLayer(LayerId id, LayerType type);
    std::queue<LayerId> freeLayerIds_;
    LayerId maxLayerIdEver_ = 0;
    std::unordered_map<LayerId, std::unique_ptr<HdiLayer>> layers_;
    std::unique_ptr<HdiLayer> clientLayer_;
    std::vector<HdiLayer *> changeLayers_;
};
} // namespace DISPLAY
} // namespace HDI
using HdiDisplayId = HDI::DISPLAY::DisplayId;
using HdiDisplay = HDI::DISPLAY::HdiDisplay;
} // namespace oewm
