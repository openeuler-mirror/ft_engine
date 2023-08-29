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

#include "hdi_display.h"

#include "log.h"

namespace FT {
namespace HDI {
namespace DISPLAY {
HdiDisplay::HdiDisplay() {}

bool HdiDisplay::Init()
{
    LOG_DEBUG("HdiDisplay::Init");

    auto id = GenerateLayerId();
    auto layer = CreateHdiLayer(id, LAYER_TYPE_GRAPHIC);
    if (layer.get() == nullptr) {
        LOG_ERROR("HdiDisplay::HdiDisplay CreateHdiLayer failed");
        return false;
    }
    clientLayer_ = std::move(layer);
    return true;
}

LayerId HdiDisplay::GenerateLayerId()
{
    if (!freeLayerIds_.empty()) {
        LayerId id = freeLayerIds_.front();
        freeLayerIds_.pop();
        return id;
    }

    return maxLayerIdEver_++;
}

std::unique_ptr<HdiLayer> HdiDisplay::CreateHdiLayer(LayerId id, LayerType type)
{
    return std::make_unique<HdiLayer>(id, type);
}

int32_t HdiDisplay::CreateLayer(const LayerInfo *layerInfo, LayerId *layerId)
{
    auto type = (layerInfo == nullptr ? LAYER_TYPE_GRAPHIC : layerInfo->type);
    auto id = GenerateLayerId();
    *layerId = id;
    layers_[id] = CreateHdiLayer(id, type);
    return DISPLAY_SUCCESS;
}

int32_t HdiDisplay::CloseLayer(LayerId layerId)
{
    auto ret = layers_.erase(layerId);
    if (ret == 0) {
        LOG_INFO("HdiDisplay::CloseLayer: can not find layer for id %{public}" PRIu32 " to close.", layerId);
        return DISPLAY_FAILURE;
    }
    freeLayerIds_.push(layerId);
    return DISPLAY_SUCCESS;
}

int32_t HdiDisplay::GetDisplayReleaseFence(uint32_t *num, uint32_t *layers, int32_t *fences)
{
    return DISPLAY_SUCCESS;
}

int32_t HdiDisplay::SetDisplayClientBuffer(const BufferHandle *buffer, int32_t fence)
{
    return DISPLAY_SUCCESS;
}

HdiLayer *HdiDisplay::GetHdiLayer(LayerId id)
{
    if (layers_.count(id) == 0) {
        return nullptr;
    }

    return layers_.at(id).get();
}
} // namespace DISPLAY
} // namespace HDI
} // namespace FT
