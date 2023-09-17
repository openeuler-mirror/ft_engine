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

#include "wayland_subsurface.h"

#include "wayland_objects_pool.h"
#include "wayland_surface.h"

namespace FT {
namespace Wayland {
namespace {
    constexpr HiLogLabel LABEL = {LOG_CORE, HILOG_DOMAIN_WAYLAND, "WaylandSubSurface"};
}

struct wl_subsurface_interface IWaylandSubSurface::impl_ = {
    .destroy = WaylandResourceObject::DefaultDestroyResource,
    .set_position = IWaylandSubSurface::SetPosition,
    .place_above = IWaylandSubSurface::PlaceAbove,
    .place_below = IWaylandSubSurface::PlaceBelow,
    .set_sync = IWaylandSubSurface::SetSync,
    .set_desync = IWaylandSubSurface::SetDesync,
};

void IWaylandSubSurface::SetPosition(struct wl_client *client, struct wl_resource *resource, int32_t x, int32_t y)
{
    CAST_OBJECT_AND_CALL_FUNC(WaylandSubSurface, resource,
    "IWaylandSubSurface::SetPosition: failed to find object.", SetPosition, resource, x, y);
}

void IWaylandSubSurface::PlaceAbove(struct wl_client *client, struct wl_resource *resource,
    struct wl_resource *sibling) {}

void IWaylandSubSurface::PlaceBelow(struct wl_client *client, struct wl_resource *resource,
    struct wl_resource *sibling) {}

void IWaylandSubSurface::SetSync(struct wl_client *client, struct wl_resource *resource) {}

void IWaylandSubSurface::SetDesync(struct wl_client *client, struct wl_resource *resource) {}

OHOS::sptr<WaylandSubSurface> WaylandSubSurface::Create(struct wl_client *client, uint32_t version,
    uint32_t id, struct wl_resource *surface, struct wl_resource *parent)
{
    if (client == nullptr) {
        return nullptr;
    }

    auto subSurface = OHOS::sptr<WaylandSubSurface>(new WaylandSubSurface(client, version, id, surface, parent));
    WaylandObjectsPool::GetInstance().AddObject(ObjectId(subSurface->WlClient(), subSurface->Id()), subSurface);
    return subSurface;
}

WaylandSubSurface::WaylandSubSurface(struct wl_client *client, uint32_t version, uint32_t id,
    struct wl_resource *surface, struct wl_resource *parent)
    : WaylandResourceObject(client, &wl_subsurface_interface, version, id, &IWaylandSubSurface::impl_)
{
    parent_ = parent;
    child_ = surface;
}

void WaylandSubSurface::SetPosition(struct wl_resource *resource, int32_t x, int32_t y)
{
    if ((positionX_ != x) || (positionY_ != y)) {
        LOG_INFO("SetPosition X:%{public}d, Y:%{public}d", x, y);
        auto surfaceParent = CastFromResource<WaylandSurface>(parent_);
        auto surfaceChild = CastFromResource<WaylandSurface>(child_);
        surfaceParent->AddChild(child_, x, y);
        surfaceChild->AddParent(parent_);
        positionX_ = x;
        positionY_ = y;
    }

}

WaylandSubSurface::~WaylandSubSurface() noexcept {}
} // namespace Wayland
} // namespace FT
