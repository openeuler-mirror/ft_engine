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

#include "wayland_xdg_surface.h"

#include "wayland_objects_pool.h"
#include "wayland_xdg_toplevel.h"
#include "wayland_xdg_wm_base.h"

namespace FT {
namespace Wayland {
namespace {
    constexpr HiLogLabel LABEL = {LOG_CORE, HILOG_DOMAIN_WAYLAND, "WaylandXdgSurface"};
}

struct xdg_surface_interface IWaylandXdgSurface::impl_ = {
    .destroy = &WaylandResourceObject::DefaultDestroyResource,
    .get_toplevel = GetToplevel,
    .get_popup = GetPopup,
    .set_window_geometry = SetWindowGeometry,
    .ack_configure = AckConfigure};

void IWaylandXdgSurface::GetToplevel(struct wl_client *client, struct wl_resource *resource, uint32_t id)
{
    CAST_OBJECT_AND_CALL_FUNC(WaylandXdgSurface, resource,
        "WaylandXdgSurface::GetToplevel: failed to find object.", GetToplevel, id);
}

void IWaylandXdgSurface::GetPopup(struct wl_client *client, struct wl_resource *resource,
    uint32_t id, struct wl_resource *parent, struct wl_resource *positioner)
{
    CAST_OBJECT_AND_CALL_FUNC(WaylandXdgSurface, resource,
        "WaylandXdgSurface::GetPopup: failed to find object.", GetPopup, id, parent, positioner);
}

void IWaylandXdgSurface::SetWindowGeometry(struct wl_client *client, struct wl_resource *resource,
    int32_t x, int32_t y, int32_t width, int32_t height)
{
    CAST_OBJECT_AND_CALL_FUNC(WaylandXdgSurface, resource,
        "WaylandXdgSurface::SetWindowGeometry: failed to find object.", SetWindowGeometry, x, y, width, height);
}

void IWaylandXdgSurface::AckConfigure(struct wl_client *client, struct wl_resource *resource, uint32_t serial)
{
}

OHOS::sptr<WaylandXdgSurface> WaylandXdgSurface::Create(const OHOS::sptr<WaylandXdgWmObject> &xdgWm,
    const OHOS::sptr<WaylandSurface> &surface, uint32_t id)
{
    if (xdgWm == nullptr) {
        LOG_WARN("xdgWm is nullptr");
        return nullptr;
    }
    if (surface == nullptr) {
        LOG_WARN("surface is nullptr");
        return nullptr;
    }

    auto xdgSurface = OHOS::sptr<WaylandXdgSurface>(new WaylandXdgSurface(xdgWm, surface, id));
    WaylandObjectsPool::GetInstance().AddObject(ObjectId(xdgSurface->WlClient(), xdgSurface->Id()), xdgSurface);
    return xdgSurface;
}

WaylandXdgSurface::WaylandXdgSurface(const OHOS::sptr<WaylandXdgWmObject> &xdgWm,
    const OHOS::sptr<WaylandSurface> &surface, uint32_t id)
    : WaylandResourceObject(xdgWm->WlClient(), &xdg_surface_interface, xdgWm->Version(),
      id, &IWaylandXdgSurface::impl_),
      xdgWm_(xdgWm),
      surface_(surface)
{
    surface->AddCommitCallback([this]() { OnSurfaceCommit(); });
    surface->AddRectCallback([this](Rect rect) { OnSurfaceRect(rect); });
}

WaylandXdgSurface::~WaylandXdgSurface() noexcept {}

void WaylandXdgSurface::GetToplevel(uint32_t id)
{
    LOG_DEBUG("Window %{public}s.", windowTitle_.c_str());
    toplevel_ = WaylandXdgToplevel::Create(this, id);
    if (toplevel_ == nullptr) {
        LOG_ERROR("no memory");
        return;
    }

    role_ = SurfaceRole::XDG_TOPLEVEL;
    surface_->WithTopLevel(true);
}

void WaylandXdgSurface::GetPopup(uint32_t id, struct wl_resource *parent, struct wl_resource *positioner)
{
    LOG_DEBUG("Window %{public}s.", windowTitle_.c_str());
}

void WaylandXdgSurface::SetWindowGeometry(int32_t x, int32_t y, int32_t width, int32_t height)
{
}

void WaylandXdgSurface::AckConfigure(uint32_t serial)
{
}

void WaylandXdgSurface::StartMove()
{
    LOG_DEBUG("Window %{public}s.", windowTitle_.c_str());
    auto surface = surface_.promote();
    if (surface != nullptr) {
        surface->StartMove();
    }
}

void WaylandXdgSurface::SetWindowMode(OHOS::Rosen::WindowMode mode)
{
    LOG_DEBUG("Window %{public}s.", windowTitle_.c_str());
    auto surface = surface_.promote();
    if (surface != nullptr) {
        surface->SetWindowMode(mode);
    }
}

void WaylandXdgSurface::SetWindowType(OHOS::Rosen::WindowType type)
{
    LOG_DEBUG("Window %{public}s.", windowTitle_.c_str());
    auto surface = surface_.promote();
    if (surface != nullptr) {
        surface->SetWindowType(type);
    }
}

void WaylandXdgSurface::OnSurfaceCommit()
{
    if (role_ == SurfaceRole::XDG_TOPLEVEL) {
        auto topLevel = toplevel_.promote();
        if (topLevel != nullptr) {
            topLevel->HandleCommit();
        }
    }

    xdg_surface_send_configure(WlResource(), wl_display_next_serial(WlDisplay()));
}

void WaylandXdgSurface::OnSurfaceRect(Rect rect)
{
    LOG_DEBUG("Window %{public}s.", windowTitle_.c_str());
    if (role_ == SurfaceRole::XDG_TOPLEVEL) {
        auto topLevel = toplevel_.promote();
        if (topLevel != nullptr) {
            topLevel->SetRect(rect);
            xdg_surface_send_configure(WlResource(), wl_display_next_serial(WlDisplay()));
        }
    }
}

void WaylandXdgSurface::SetTitle(const char *title)
{
    LOG_DEBUG("Window %{public}s, set Title %{public}s.", windowTitle_.c_str(), title);
    auto surface = surface_.promote();
    if (surface != nullptr) {
        surface->SetTitle(title);
    }
    windowTitle_ = title;
}

void WaylandXdgSurface::Resize(uint32_t serial, uint32_t edges)
{
    LOG_DEBUG("Window %{public}s.", windowTitle_.c_str());
    auto surface = surface_.promote();
    if (surface != nullptr) {
        surface->Resize(serial, edges);
    }
}

void WaylandXdgSurface::SetMaxSize(int32_t width, int32_t height)
{
    LOG_DEBUG("Window %{public}s.", windowTitle_.c_str());
    auto surface = surface_.promote();
    if (surface != nullptr) {
        surface->SetMaxSize(width, height);
    }
}

void WaylandXdgSurface::SetMinSize(int32_t width, int32_t height)
{
    LOG_DEBUG("Window %{public}s.", windowTitle_.c_str());
    auto surface = surface_.promote();
    if (surface != nullptr) {
        surface->SetMinSize(width, height);
    }
}

void WaylandXdgSurface::SetMaximized()
{
    LOG_DEBUG("Window %{public}s.", windowTitle_.c_str());
    auto surface = surface_.promote();
    if (surface != nullptr) {
        surface->SetMaximized();
    }
}

void WaylandXdgSurface::UnSetMaximized()
{
    LOG_DEBUG("Window %{public}s.", windowTitle_.c_str());
    auto surface = surface_.promote();
    if (surface != nullptr) {
        surface->UnSetMaximized();
    }
}

void WaylandXdgSurface::SetFullscreen()
{
    LOG_DEBUG("Window %{public}s.", windowTitle_.c_str());
    auto surface = surface_.promote();
    if (surface != nullptr) {
        surface->SetFullscreen();
    }
}

void WaylandXdgSurface::UnSetFullscreen()
{
    LOG_DEBUG("Window %{public}s.", windowTitle_.c_str());
    auto surface = surface_.promote();
    if (surface != nullptr) {
        surface->UnSetFullscreen();
    }
}

void WaylandXdgSurface::SetMinimized()
{
    LOG_DEBUG("Window %{public}s.", windowTitle_.c_str());
    auto surface = surface_.promote();
    if (surface != nullptr) {
        surface->SetMinimized();
    }
}

void WaylandXdgSurface::Close()
{
    LOG_DEBUG("Window %{public}s.", windowTitle_.c_str());
    auto surface = surface_.promote();
    if (surface != nullptr) {
        surface->Close();
    }
}
} // namespace Wayland
} // namespace FT
