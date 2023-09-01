/*
 * Copyright (c) 2023 Huawei Technologies Co., Ltd.
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

#include <string>

#include <include/core/SkImageInfo.h>
#include "wayland-server-protocol.h"

namespace FT {
namespace Wayland {
static constexpr uint32_t DEFAULT_WIDTH = 500;
static constexpr uint32_t DEFAULT_HEIGHT = 500;
struct XdgSurfaceState {
    uint32_t width = DEFAULT_WIDTH;
    uint32_t height = DEFAULT_HEIGHT;
    std::string title;
    std::string appId;
};

enum class XdgSurfaceRole : uint32_t {
    NONE = 0,
    TOPLEVEL,
    POPUP
};

static SkColorType ShmFormatToSkia(const uint32_t& shmFormat)
{
    switch (shmFormat) {
        case WL_SHM_FORMAT_RGBA8888:
            return SkColorType::kRGBA_8888_SkColorType;
        default:
            return SkColorType::kUnknown_SkColorType;
    }
}
} // namespace Wayland
} // namespace FT
