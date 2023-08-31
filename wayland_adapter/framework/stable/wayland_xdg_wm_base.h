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

#include <map>
#include <mutex>

#include <xdg-shell-server-protocol.h>
#include "wayland_global.h"

namespace FT {
namespace Wayland {
struct IWaylandXdgWmBase {
    static void CreatePositioner(struct wl_client *client, struct wl_resource *resource, uint32_t id);
    static void GetXdgSurface(struct wl_client *client, struct wl_resource *xdgWmBaseResource,
        uint32_t id, struct wl_resource *surfaceResource);
    static void Pong(struct wl_client *client, struct wl_resource *resource, uint32_t serial);
    static struct xdg_wm_base_interface impl_;
};

class WaylandXdgWmBase final : public WaylandGlobal {
public:
    static OHOS::sptr<WaylandXdgWmBase> Create(struct wl_display *display);
    ~WaylandXdgWmBase() noexcept override;

private:
    WaylandXdgWmBase(struct wl_display *display);
    void Bind(struct wl_client *client, uint32_t version, uint32_t id) override;
};

class WaylandXdgWmObject final : public WaylandResourceObject {
public:
    static OHOS::sptr<WaylandXdgWmObject> Create(struct wl_client *client, uint32_t version, uint32_t id);
    ~WaylandXdgWmObject() noexcept override;

private:
    WaylandXdgWmObject(struct wl_client *client, uint32_t version, uint32_t id);
    friend struct IWaylandXdgWmBase;

    void CreatePositioner(struct wl_client *client, uint32_t id);
    void GetXdgSurface(struct wl_client *client, struct wl_resource *xdgWmBaseResource,
        uint32_t id, struct wl_resource *surfaceResource);
    void Pong(struct wl_client *client, struct wl_resource *resource, uint32_t serial);
};
} // namespace Wayland
} // namespace FT
