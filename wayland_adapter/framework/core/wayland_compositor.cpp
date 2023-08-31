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

#include "wayland_compositor.h"

#include "wayland_surface.h"
#include "wayland_objects_pool.h"
#include "version.h"

namespace FT {
namespace Wayland {
namespace {
    constexpr HiLogLabel LABEL = {LOG_CORE, HILOG_DOMAIN_WAYLAND, "WaylandCompositor"};
}

struct wl_compositor_interface IWaylandCompositor::impl_ = {
    .create_surface = CreateSurface,
    .create_region = CreateRegion};

void IWaylandCompositor::CreateSurface(struct wl_client *client, struct wl_resource *resource, uint32_t id)
{
    CAST_OBJECT_AND_CALL_FUNC(WaylandCompositorObject, resource,
        "IWaylandCompositor::CreateSurface: failed to find object.", CreateSurface, client, resource, id);
}

void IWaylandCompositor::CreateRegion(struct wl_client *client, struct wl_resource *resource, uint32_t id)
{
    CAST_OBJECT_AND_CALL_FUNC(WaylandCompositorObject, resource,
        "IWaylandCompositor::CreateRegion: failed to find object.", CreateRegion, client, resource, id);
}

OHOS::sptr<WaylandCompositor> WaylandCompositor::Create(struct wl_display *display)
{
    if (display == nullptr) {
        LOG_ERROR("display is nullptr");
        return nullptr;
    }

    return OHOS::sptr<WaylandCompositor>(new WaylandCompositor(display));
}

WaylandCompositor::WaylandCompositor(struct wl_display *display)
    : WaylandGlobal(display, &wl_compositor_interface, WL_COMPOSITOR_MAX_VERSION) {}

WaylandCompositor::~WaylandCompositor() noexcept {}

void WaylandCompositor::Bind(struct wl_client *client, uint32_t version, uint32_t id)
{
    auto object = OHOS::sptr<WaylandCompositorObject>(new WaylandCompositorObject(client, version, id));
    if (object == nullptr) {
        LOG_ERROR("no memory");
        return;
    }
    WaylandObjectsPool::GetInstance().AddObject(ObjectId(object->WlClient(), object->Id()), object);
}

WaylandCompositorObject::WaylandCompositorObject(struct wl_client *client, uint32_t version, uint32_t id)
    : WaylandResourceObject(client, &wl_compositor_interface, version, id, &IWaylandCompositor::impl_) {}

WaylandCompositorObject::~WaylandCompositorObject() noexcept {}

void WaylandCompositorObject::CreateSurface(struct wl_client *client, struct wl_resource *resource, uint32_t id)
{
    auto surface = WaylandSurface::Create(client, resource, wl_resource_get_version(resource), id);
    if (surface == nullptr) {
        LOG_ERROR("no memory");
        return;
    }
}

void WaylandCompositorObject::CreateRegion(struct wl_client *client, struct wl_resource *resource, uint32_t id)
{
}
} // namespace Wayland
} // namespace FT
