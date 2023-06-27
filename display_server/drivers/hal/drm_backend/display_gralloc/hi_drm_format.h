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

#include "display_type.h"
#include "log.h"

#include <cstdint>

namespace DrmFormat {
#define DRM_MAX_PLANES 3

struct PlaneLayoutInfo {
    uint32_t numPlanes;
    uint32_t radio[DRM_MAX_PLANES];
};

struct DrmFormatInfo {
    uint32_t format;
    uint32_t bpp; // bits per pixel for first plane
    const PlaneLayoutInfo *planes;
};

const DrmFormatInfo *GetDrmFormatInfo(uint32_t format);
const char *GetDrmFmtStr(uint32_t format);
const char *GetHdiPixelFmtStr(PixelFormat format);
uint32_t ConvertPixelFormatToDrmFormat(PixelFormat fmtIn);
PixelFormat ConvertDrmFormatToPixelFormat(uint32_t fmtIn);
} // namespace DrmFormat
