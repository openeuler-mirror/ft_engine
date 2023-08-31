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

#include "wayland_xdg_wm_base.h"

#include "wayland_objects_pool.h"
#include "wayland_xdg_surface.h"
#include "version.h"

namespace FT {
namespace Wayland {
namespace {
    constexpr HiLogLabel LABEL = {LOG_CORE, HILOG_DOMAIN_WAYLAND, "WaylandXdgWmBase"};
}

struct xdg_wm_base_interface IWaylandXdgWmBase::impl_ = {
    .destroy = &WaylandResourceObject::DefaultDestroyResource,
    .create_positioner = CreatePositioner,
    .get_xdg_surface = GetXdgSurface,
    .pong = Pong,
};

void IWaylandXdgWmBase::CreatePositioner(struct wl_client *client, struct wl_resource *resource, uint32_t id)
{
    CAST_OBJECT_AND_CALL_FUNC(WaylandXdgWmObject, resource,
        "IWaylandXdgWmBase::CreatePositioner: failed to find object.",
        CreatePositioner, client, id);
}

void IWaylandXdgWmBase::GetXdgSurface(struct wl_client *client, struct wl_resource *xdgWmBaseResource,
    uint32_t id, struct wl_resource *surfaceResource)
{
    CAST_OBJECT_AND_CALL_FUNC(WaylandXdgWmObject, xdgWmBaseResource,
        "IWaylandXdgWmBase::GetXdgSurface: failed to find object.",
        GetXdgSurface, client, xdgWmBaseResource, id, surfaceResource);
}

void IWaylandXdgWmBase::Pong(struct wl_client *client, struct wl_resource *resource, uint32_t serial)
{
    CAST_OBJECT_AND_CALL_FUNC(WaylandXdgWmObject, resource,
        "IWaylandXdgWmBase::Pong: failed to find object.", Pong, client, resource, serial);
}

OHOS::sptr<WaylandXdgWmObject> WaylandXdgWmObject::Create(struct wl_client *client, uint32_t version, uint32_t id)
{
    if (client == nullptr) {
        LOG_WARN("client is nullptr");
        return nullptr;
    }

    auto xdgWm = OHOS::sptr<WaylandXdgWmObject>(new WaylandXdgWmObject(client, version, id));
    WaylandObjectsPool::GetInstance().AddObject(ObjectId(xdgWm->WlClient(), xdgWm->Id()), xdgWm);
    return xdgWm;
}

WaylandXdgWmObject::WaylandXdgWmObject(struct wl_client *client, uint32_t version, uint32_t id)
    : WaylandResourceObject(client, &xdg_wm_base_interface, version, id, &IWaylandXdgWmBase::impl_) {}

WaylandXdgWmObject::~WaylandXdgWmObject() noexcept {}

void WaylandXdgWmObject::CreatePositioner(struct wl_client *client, uint32_t id)
{
}

void WaylandXdgWmObject::GetXdgSurface(struct wl_client *client, struct wl_resource *xdgWmBaseResource,
    uint32_t id, struct wl_resource *surfaceResource)
{
    if (client != client_) {
        LOG_WARN("client conflict");
        return;
    }

    auto surface = CastFromResource<WaylandSurface>(surfaceResource);
    if (surface == nullptr) {
        LOG_WARN("waylandSurface is nullptr");
        return;
    }

    auto xdgSurface = WaylandXdgSurface::Create(this, surface, id);
    if (xdgSurface == nullptr) {
        LOG_ERROR("no memory");
        return;
    }
}

void WaylandXdgWmObject::Pong(struct wl_client *client, struct wl_resource *resource, uint32_t serial) {}

OHOS::sptr<WaylandXdgWmBase> WaylandXdgWmBase::Create(struct wl_display *display)
{
    if (display == nullptr) {
        LOG_ERROR("display is nullptr");
        return nullptr;
    }

    return OHOS::sptr<WaylandXdgWmBase>(new WaylandXdgWmBase(display));
}

WaylandXdgWmBase::WaylandXdgWmBase(struct wl_display *display)
    : WaylandGlobal(display, &xdg_wm_base_interface, XDG_WM_BASE_MAX_VERSION) {}

WaylandXdgWmBase::~WaylandXdgWmBase() noexcept {}

void WaylandXdgWmBase::Bind(struct wl_client *client, uint32_t version, uint32_t id)
{
    auto xdgWm = WaylandXdgWmObject::Create(client, version, id);
    if (xdgWm == nullptr) {
        LOG_ERROR("no memory");
    }
}
} // namespace Wayland
} // namespace FT
