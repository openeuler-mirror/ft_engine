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

#include <xf86drm.h>
#include <xf86drmMode.h>
#include <unordered_map>
#include <memory>

#include "noncopyable_hal.h"
#include "types.h"

namespace FT {
namespace drm {
constexpr uint32_t DRM_INVALID_OBJECT_ID = 0;
constexpr uint32_t DRM_INVALID_OBJECT_TYPE = 0;
constexpr uint32_t DRM_INVALID_PROP_ID = 0;
constexpr uint32_t DRM_INVLIAD_SIZE = 0;
constexpr uint64_t DRM_INVLIAD_VALUE = ~(0x0);

template <typename T>
using IdMapPtr = std::unordered_map<uint32_t, std::shared_ptr<T>>;

inline std::string DrmObjTypeToString(uint32_t objType)
{
    switch (objType) {
        case DRM_MODE_OBJECT_CRTC:
            return "DRM_MODE_OBJECT_CRTC";
        case DRM_MODE_OBJECT_CONNECTOR:
            return "DRM_MODE_OBJECT_CONNECTOR";
        case DRM_MODE_OBJECT_ENCODER:
            return "DRM_MODE_OBJECT_ENCODER";
        case DRM_MODE_OBJECT_MODE:
            return "DRM_MODE_OBJECT_MODE";
        case DRM_MODE_OBJECT_PROPERTY:
            return "DRM_MODE_OBJECT_PROPERTY";
        case DRM_MODE_OBJECT_FB:
            return "DRM_MODE_OBJECT_FB";
        case DRM_MODE_OBJECT_BLOB:
            return "DRM_MODE_OBJECT_BLOB";
        case DRM_MODE_OBJECT_PLANE:
            return "DRM_MODE_OBJECT_PLANE";
        case DRM_MODE_OBJECT_ANY:
            return "DRM_MODE_OBJECT_ANY";
        default:
            return "DRM_MODE_OBJECT_UNKNOWN";
    }
}

inline std::string ObjInfoDump(uint32_t objId, uint32_t objType)
{
    return "DrmObject [id: " + std::to_string(objId) + ", type: " + DrmObjTypeToString(objType) + "]";
}
} // namespace drm
} // namespace FT
