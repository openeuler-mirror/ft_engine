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

#include "wayland_region.h"
#include "wayland_objects_pool.h"

namespace FT {
namespace Wayland {
namespace {
    constexpr HiLogLabel LABEL = {LOG_CORE, HILOG_DOMAIN_WAYLAND, "WaylandRegion"};
}

struct wl_region_interface IWaylandRegion::impl_ = {
    .destroy = &WaylandResourceObject::DefaultDestroyResource,
    .add = Add,
    .subtract = Subtract};

void IWaylandRegion::Add(struct wl_client *client, struct wl_resource *resource,
    int32_t x, int32_t y, int32_t width, int32_t height)
{
    CAST_OBJECT_AND_CALL_FUNC(WaylandRegion, resource,
        "IWaylandRegion::Add: failed to find object.", Add, x, y, width, height);
}

void IWaylandRegion::Subtract(struct wl_client *client, struct wl_resource *resource,
    int32_t x, int32_t y, int32_t width, int32_t height)
{
    CAST_OBJECT_AND_CALL_FUNC(WaylandRegion, resource,
        "IWaylandRegion::Subtract: failed to find object.", Subtract, x, y, width, height);
}

OHOS::sptr<WaylandRegion> WaylandRegion::Create(struct wl_client *client,
    struct wl_resource *parent, uint32_t version, uint32_t id)
{
    if (client == nullptr) {
        return nullptr;
    }

    auto region = OHOS::sptr<WaylandRegion>(new WaylandRegion(client, parent, version, id));
    WaylandObjectsPool::GetInstance().AddObject(ObjectId(region->WlClient(), region->Id()), region);
    return region;
}

WaylandRegion::WaylandRegion(struct wl_client *client, struct wl_resource *parent, uint32_t version, uint32_t id)
    : WaylandResourceObject(client, &wl_region_interface, version, id, &IWaylandRegion::impl_),
      parent_(parent) {}

WaylandRegion::~WaylandRegion() noexcept {}

void WaylandRegion::Add(int32_t x, int32_t y, int32_t width, int32_t height)
{
    LOG_DEBUG("WaylandRegion::Add, cover");
    rect_.x = x;
    rect_.y = y;
    rect_.width = width;
    rect_.height = height;
}

void WaylandRegion::Subtract(int32_t x, int32_t y, int32_t width, int32_t height)
{
    LOG_DEBUG("WaylandRegion::Subtract, ignore");
}

Rect WaylandRegion::GetRect()
{
    return rect_;
}
} // namespace Wayland
} // namespace FT
