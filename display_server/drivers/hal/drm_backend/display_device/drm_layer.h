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

#pragma once

#include <deque>

#include "drm_frame_buffer.h"
#include "hdi_layer.h"

namespace oewm {
namespace drm {
class DrmLayer : public HdiLayer {
public:
    DrmLayer(HdiLayerId id, LayerType type) : HdiLayer(id, type) {}
    ~DrmLayer() noexcept override = default;

    DrmFrameBuffer *GetFrameBuffer(int drmFd);

private:
    std::unique_ptr<DrmFrameBuffer> lastFrameBuffer_;
    std::unique_ptr<DrmFrameBuffer> currentFrameBuffer_;
};
} // namespace drm
} // namespace oewm
