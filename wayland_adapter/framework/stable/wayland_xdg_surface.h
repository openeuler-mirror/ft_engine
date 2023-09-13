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

#include <xdg-shell-server-protocol.h>
#include "wayland_resource_object.h"
#include "wayland_surface.h"
#include "wayalnd_utils.h"

namespace FT {
namespace Wayland {
class WaylandXdgWmObject;
class WaylandXdgToplevel;
struct IWaylandXdgSurface {
    static void GetToplevel(struct wl_client *client, struct wl_resource *resource, uint32_t id);
    static void GetPopup(struct wl_client *client, struct wl_resource *resource,
        uint32_t id, struct wl_resource *parent, struct wl_resource *positioner);
    static void SetWindowGeometry(struct wl_client *client, struct wl_resource *resource,
        int32_t x, int32_t y, int32_t width, int32_t height);
    static void AckConfigure(struct wl_client *client, struct wl_resource *resource, uint32_t serial);
    static struct xdg_surface_interface impl_;
};

class WaylandXdgSurface : public WaylandResourceObject {
public:
    static OHOS::sptr<WaylandXdgSurface> Create(const OHOS::sptr<WaylandXdgWmObject> &xdgWm,
        const OHOS::sptr<WaylandSurface> &surface, uint32_t id);
    WaylandXdgSurface(const OHOS::sptr<WaylandXdgWmObject> &xdgWm, const OHOS::sptr<WaylandSurface> &surface, uint32_t id);
    ~WaylandXdgSurface() noexcept override;

    void StartMove();

private:
    friend struct IWaylandXdgSurface;

    void GetToplevel(uint32_t id);
    void GetPopup(uint32_t id, struct wl_resource *parent, struct wl_resource *positioner);
    void SetWindowGeometry(int32_t x, int32_t y, int32_t width, int32_t height);
    void AckConfigure(uint32_t serial);
    void OnSurfaceCommit();
    void OnSurfaceRect(Rect rect);

    SurfaceRole role_ = SurfaceRole::NONE;
    OHOS::wptr<WaylandXdgWmObject> xdgWm_;
    OHOS::wptr<WaylandSurface> surface_;
    OHOS::wptr<WaylandXdgToplevel> toplevel_;
};
} // namespace Wayland
} // namespace FT
