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

#include "wayland_xdg_toplevel.h"

#include "wayland_objects_pool.h"

namespace FT {
namespace Wayland {
namespace {
    constexpr HiLogLabel LABEL = {LOG_CORE, HILOG_DOMAIN_WAYLAND, "WaylandXdgToplevel"};
}

struct xdg_toplevel_interface IWaylandXdgToplevel::impl_ = {
    .destroy = &WaylandResourceObject::DefaultDestroyResource,
    .set_parent = SetParent,
    .set_title = SetTitle,
    .set_app_id = SetAppId,
    .show_window_menu = ShowWindowMenu,
    .move = Move,
    .resize = Resize,
    .set_max_size = SetMaxSize,
    .set_min_size = SetMinSize,
    .set_maximized = SetMaximized,
    .unset_maximized = UnsetMaximized,
    .set_fullscreen = SetFullscreen,
    .unset_fullscreen = UnsetFullscreen,
    .set_minimized = SetMinimized};

void IWaylandXdgToplevel::SetParent(struct wl_client *client, struct wl_resource *resource,
    struct wl_resource *parent) {}

void IWaylandXdgToplevel::SetTitle(struct wl_client *client, struct wl_resource *resource, const char *title)
{
    CAST_OBJECT_AND_CALL_FUNC(WaylandXdgToplevel, resource,
        "IWaylandXdgToplevel::SetTitle: failed to find object.", SetTitle, title);
}

void IWaylandXdgToplevel::SetAppId(struct wl_client *client, struct wl_resource *resource, const char *app_id)
{
    CAST_OBJECT_AND_CALL_FUNC(WaylandXdgToplevel, resource,
        "IWaylandXdgToplevel::SetTitle: failed to find object.", SetAppId, app_id);
}

void IWaylandXdgToplevel::ShowWindowMenu(struct wl_client *client, struct wl_resource *resource,
    struct wl_resource *seat, uint32_t serial, int32_t x, int32_t y) {}

void IWaylandXdgToplevel::Move(struct wl_client *client, struct wl_resource *resource,
    struct wl_resource *seat, uint32_t serial)
{
    CAST_OBJECT_AND_CALL_FUNC(WaylandXdgToplevel, resource,
        "IWaylandXdgToplevel::Move: failed to find object.", Move, serial);
}

void IWaylandXdgToplevel::Resize(struct wl_client *client, struct wl_resource *resource,
    struct wl_resource *seat, uint32_t serial, uint32_t edges)
{
    CAST_OBJECT_AND_CALL_FUNC(WaylandXdgToplevel, resource,
        "IWaylandXdgToplevel::Resize: failed to find object.", Resize, serial, edges);
}

void IWaylandXdgToplevel::SetMaxSize(struct wl_client *client, struct wl_resource *resource,
    int32_t width, int32_t height)
{
    CAST_OBJECT_AND_CALL_FUNC(WaylandXdgToplevel, resource,
        "IWaylandXdgToplevel::SetMaxSize: failed to find object.", SetMaxSize, width, height);
}

void IWaylandXdgToplevel::SetMinSize(struct wl_client *client, struct wl_resource *resource,
    int32_t width, int32_t height)
{
    CAST_OBJECT_AND_CALL_FUNC(WaylandXdgToplevel, resource,
        "IWaylandXdgToplevel::SetMinSize: failed to find object.", SetMinSize, width, height);
}

void IWaylandXdgToplevel::SetMaximized(struct wl_client *client, struct wl_resource *resource)
{
    CAST_OBJECT_AND_CALL_FUNC(WaylandXdgToplevel, resource,
        "IWaylandXdgToplevel::SetMaximized: failed to find object.", SetMaximized);
}

void IWaylandXdgToplevel::UnsetMaximized(struct wl_client *client, struct wl_resource *resource)
{
    CAST_OBJECT_AND_CALL_FUNC(WaylandXdgToplevel, resource,
        "IWaylandXdgToplevel::UnsetMaximized: failed to find object.", UnSetMaximized);
}

void IWaylandXdgToplevel::SetFullscreen(struct wl_client *client, struct wl_resource *resource,
    struct wl_resource *output)
{
    CAST_OBJECT_AND_CALL_FUNC(WaylandXdgToplevel, resource,
        "IWaylandXdgToplevel::SetFullscreen: failed to find object.", SetFullscreen);
}

void IWaylandXdgToplevel::UnsetFullscreen(struct wl_client *client, struct wl_resource *resource)
{
    CAST_OBJECT_AND_CALL_FUNC(WaylandXdgToplevel, resource,
        "IWaylandXdgToplevel::UnsetFullscreen: failed to find object.", UnSetFullscreen);
}

void IWaylandXdgToplevel::SetMinimized(struct wl_client *client, struct wl_resource *resource)
{
    CAST_OBJECT_AND_CALL_FUNC(WaylandXdgToplevel, resource,
        "IWaylandXdgToplevel::SetMinimized: failed to find object.", SetMinimized);
}

OHOS::sptr<WaylandXdgToplevel> WaylandXdgToplevel::Create(const OHOS::sptr<WaylandXdgSurface> &xdgSurface, uint32_t id)
{
    if (xdgSurface == nullptr) {
        LOG_WARN("xdgSurface is nullptr");
        return nullptr;
    }

    auto xdgTopLevel = OHOS::sptr<WaylandXdgToplevel>(new WaylandXdgToplevel(xdgSurface, id));
    WaylandObjectsPool::GetInstance().AddObject(ObjectId(xdgTopLevel->WlClient(), xdgTopLevel->Id()), xdgTopLevel);
    return xdgTopLevel;
}

WaylandXdgToplevel::WaylandXdgToplevel(const OHOS::sptr<WaylandXdgSurface> &xdgSurface, uint32_t id)
    : WaylandResourceObject(xdgSurface->WlClient(), &xdg_toplevel_interface, xdgSurface->Version(),
      id, &IWaylandXdgToplevel::impl_),
      xdgSurface_(xdgSurface) {}

WaylandXdgToplevel::~WaylandXdgToplevel() noexcept
{
}

void WaylandXdgToplevel::SetTitle(const char *title)
{
}

void WaylandXdgToplevel::Move(uint32_t serial)
{
    xdgSurface_->StartMove();
}

void WaylandXdgToplevel::Resize(uint32_t serial, uint32_t edges)
{
}

void WaylandXdgToplevel::SetAppId(const char *appId)
{
}

void WaylandXdgToplevel::SetMaxSize(int32_t width, int32_t height)
{
}

void WaylandXdgToplevel::SetMinSize(int32_t width, int32_t height)
{
}

void WaylandXdgToplevel::SetMaximized()
{
}

void WaylandXdgToplevel::UnSetMaximized()
{
}

void WaylandXdgToplevel::SetFullscreen()
{
}

void WaylandXdgToplevel::UnSetFullscreen()
{
}

void WaylandXdgToplevel::SetMinimized()
{
}

void WaylandXdgToplevel::SendConfigure()
{
    struct wl_array states;
    wl_array_init(&states);
    xdg_toplevel_send_configure(WlResource(), rect_.width, rect_.height, &states);
    wl_array_release(&states);
}

void WaylandXdgToplevel::HandleCommit()
{
    SendConfigure();
}
} // namespace Wayland
} // namespace FT
