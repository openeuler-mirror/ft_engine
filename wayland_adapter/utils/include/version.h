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

#include <cstdint>

namespace FT {
/* Core*/
static const uint32_t WL_COMPOSITOR_MAX_VERSION = 4;          // child: wl_surface
static const uint32_t WL_SUBCOMPOSITOR_MAX_VERSION = 1;       // child: wl_subsurface
static const uint32_t WL_DATA_DEVICE_MANAGER_MAX_VERSION = 3; // child: wl_data_device
static const uint32_t WL_SHM_MAX_VERSION = 1;
static const uint32_t WL_OUTPUT_MAX_VERSION = 2;
static const uint32_t WL_SEAT_MAX_VERSION = 7; // child: wl_keyboard wl_pointer wl_touch
static const uint32_t WL_BUFFER_MAX_VERSION = 1;

/* Stable */
static const uint32_t XDG_WM_BASE_MAX_VERSION = 3; // child: xdg_surface

/* Unstable */
static const uint32_t ZXDG_OUTPUT_MANAGER_V1_MAX_VERSION = 2; // child: zxdg_output
static const uint32_t ZWP_LINUX_DMABUF_V1_MAX_VERSION = 3; // child: zwp_linux_buffer_params & zwp_linux_dmabuf_feedback
static const uint32_t ZXDG_SHELL_V6_MAX_VERSION = 1;
static const uint32_t ZXDG_DECORATION_MANAGER_V1_MAX_VERSION = 1;
static const uint32_t ZWLR_SCREENCOPY_MANAGER_V1_MAX_VERSION = 3;
static const uint32_t ZWLR_VIRTUAL_POINTER_MANAGER_V1_MAX_VERSION = 2;
static const uint32_t ZWP_VIRTUAL_KEYBOARD_MANAGER_V1_MAX_VERSION = 1;
static const uint32_t ZWLR_LAYER_SHELL_V1_MAX_VERSION = 4;
static const uint32_t ZWLR_DATA_CONTROL_MANAGER_V1_MAX_VERSION = 2;
} // namespace FT
