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

#include <xdg-output-unstable-v1-server-protocol.h>
#include "wayland_global.h"
#include "wayland_zxdg_output_v1.h"

namespace FT {
namespace Wayland {
struct IWaylandZxdgOutputManagerV1 {
    static void GetZxdgOutput(
        struct wl_client *client,
        struct wl_resource *resource,
        uint32_t id,
        struct wl_resource *output);

    static struct zxdg_output_manager_v1_interface impl_;
};

class WaylandZxdgOutputManagerV1 final : public WaylandGlobal {
public:
    static OHOS::sptr<WaylandZxdgOutputManagerV1> Create(struct wl_display *display);
    ~WaylandZxdgOutputManagerV1() noexcept override;

private:
    WaylandZxdgOutputManagerV1(struct wl_display *display);
    void Bind(struct wl_client *client, uint32_t version, uint32_t id) override;
};

class WaylandZxdgOutputManagerObject final : public WaylandResourceObject {
public:
    WaylandZxdgOutputManagerObject(struct wl_client *client, uint32_t version, uint32_t id);
    ~WaylandZxdgOutputManagerObject() noexcept;

    void GetZxdgOutput(
        struct wl_client *client,
        struct wl_resource *resource,
        uint32_t id,
        struct wl_resource *output);
private:
    void Send(const OHOS::sptr<WaylandZxdgOutputV1> &xdgOutput);
};
} // namespace Wayland
} // namespace FT
