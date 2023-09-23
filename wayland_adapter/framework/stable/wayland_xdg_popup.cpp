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

#include "wayland_xdg_popup.h"

#include "wayland_objects_pool.h"
#include "wayland_xdg_toplevel.h"
#include "wayland_xdg_wm_base.h"

namespace FT {
namespace Wayland {
namespace {
    constexpr HiLogLabel LABEL = {LOG_CORE, HILOG_DOMAIN_WAYLAND, "WaylandXdgPopup"};
}

struct xdg_popup_interface WaylandXdgPopupInterface::impl_ = {
    .destroy = WaylandResourceObject::DefaultDestroyResource,
    .grab = WaylandXdgPopupInterface::Grab,
    .reposition = WaylandXdgPopupInterface::Reposition};

void WaylandXdgPopupInterface::Grab(
    struct wl_client *client,
    struct wl_resource *resource,
    struct wl_resource *seat,
    uint32_t serial)
{}

void WaylandXdgPopupInterface::Reposition(
    struct wl_client *client,
    struct wl_resource *resource,
    struct wl_resource *positioner,
    uint32_t token)
{}

OHOS::sptr<WaylandXdgPopup> WaylandXdgPopup::Create(
    const OHOS::sptr<WaylandXdgSurface> &xdgSurface,
    const OHOS::sptr<WaylandXdgSurface> &parentXdgSurface,
    const OHOS::sptr<WaylandXdgPositioner> &positioner,
    uint32_t id)
{
    if (xdgSurface == nullptr) {
        LOG_ERROR("WaylandXdgPopup::Create: xdgSurface is nullptr.");
        return nullptr;
    }

    if (parentXdgSurface == nullptr) {
        LOG_ERROR("WaylandXdgPopup::Create: parentXdgSurface is nullptr.");
        return nullptr;
    }

    if (positioner == nullptr) {
        LOG_ERROR("WaylandXdgPopup::Create: positioner is nullptr.");
        return nullptr;
    }

    auto xdgPopUp = OHOS::sptr<WaylandXdgPopup>(new WaylandXdgPopup(xdgSurface, parentXdgSurface, positioner, id));
    WaylandObjectsPool::GetInstance().AddObject(ObjectId(xdgPopUp->WlClient(), xdgPopUp->Id()), xdgPopUp);
    return xdgPopUp;
}

WaylandXdgPopup::WaylandXdgPopup(
    const OHOS::sptr<WaylandXdgSurface> &xdgSurface,
    const OHOS::sptr<WaylandXdgSurface> &parentXdgSurface,
    const OHOS::sptr<WaylandXdgPositioner> &positioner,
    uint32_t id)
    : WaylandResourceObject(
          xdgSurface->WlClient(),
          &xdg_popup_interface,
          xdgSurface->Version(),
          id,
          &WaylandXdgPopupInterface::impl_),
      xdgSurface_(xdgSurface),
      parentXdgSurface_(parentXdgSurface)
{
    // TODO: get information from positioner.
    LOG_DEBUG("WaylandXdgPopup  ctor.");
}

WaylandXdgPopup::~WaylandXdgPopup() noexcept
{
    LOG_DEBUG("WaylandXdgPopup  dtor.");
}

void WaylandXdgPopup::OnSurfaceCommitted() {}

void WaylandXdgPopup::Grab(struct wl_client *client, struct wl_resource *resource, struct wl_resource *seat, uint32_t serial)
{}

void WaylandXdgPopup::Reposition(
    struct wl_client *client,
    struct wl_resource *resource,
    struct wl_resource *positioner,
    uint32_t token)
{}

void WaylandXdgPopup::UpdateSize(int32_t x, int32_t y, uint32_t width, uint32_t height)
{
}

void WaylandXdgPopup::SetGeometry(int32_t x, int32_t y, uint32_t width, uint32_t height)
{
}
} // namespace Wayland
} // namespace FT
