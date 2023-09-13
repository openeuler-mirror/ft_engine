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
#include "wayland_xdg_surface.h"

namespace FT {
namespace Wayland {
struct IWaylandXdgToplevel {
    static void SetParent(struct wl_client *client, struct wl_resource *resource, struct wl_resource *parent);
    static void SetTitle(struct wl_client *client, struct wl_resource *resource, const char *title);
    static void SetAppId(struct wl_client *client, struct wl_resource *resource, const char *app_id);
    static void ShowWindowMenu(struct wl_client *client, struct wl_resource *resource,
        struct wl_resource *seat, uint32_t serial, int32_t x, int32_t y);
    static void Move(struct wl_client *client, struct wl_resource *resource,
        struct wl_resource *seat, uint32_t serial);
    static void Resize(struct wl_client *client, struct wl_resource *resource,
        struct wl_resource *seat, uint32_t serial, uint32_t edges);
    static void SetMaxSize(struct wl_client *client, struct wl_resource *resource, int32_t width, int32_t height);
    static void SetMinSize(struct wl_client *client, struct wl_resource *resource, int32_t width, int32_t height);
    static void SetMaximized(struct wl_client *client, struct wl_resource *resource);
    static void UnsetMaximized(struct wl_client *client, struct wl_resource *resource);
    static void SetFullscreen(struct wl_client *client, struct wl_resource *resource, struct wl_resource *output);
    static void UnsetFullscreen(struct wl_client *client, struct wl_resource *resource);
    static void SetMinimized(struct wl_client *client, struct wl_resource *resource);
    static struct xdg_toplevel_interface impl_;
};

class WaylandXdgToplevel final : public WaylandResourceObject {
public:
    static OHOS::sptr<WaylandXdgToplevel> Create(const OHOS::sptr<WaylandXdgSurface> &xdgSurface, uint32_t id);
    ~WaylandXdgToplevel() noexcept override;

    void SetTitle(const char *title);
    void SetAppId(const char *appId);
    void SetMaxSize(int32_t width, int32_t height);
    void SetMinSize(int32_t width, int32_t height);
    void SetMaximized();
    void UnSetMaximized();
    void SetFullscreen();
    void UnSetFullscreen();
    void SetMinimized();
    void Move(uint32_t serial);
    void Resize(uint32_t serial, uint32_t edges);
    void SendConfigure();
    void HandleCommit();
    void SetRect(Rect rect);

private:
    WaylandXdgToplevel(const OHOS::sptr<WaylandXdgSurface> &xdgSurface, uint32_t id);
    friend struct IWaylandXdgToplevel;

    OHOS::wptr<WaylandXdgSurface> xdgSurface_;
    Rect rect_;
};
} // namespace Wayland
} // namespace FT
