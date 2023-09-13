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

#include "wayland_subcompositor.h"

#include "wayland_subsurface.h"
#include "wayland_objects_pool.h"
#include "version.h"

namespace FT {
namespace Wayland {
namespace {
    constexpr HiLogLabel LABEL = {LOG_CORE, HILOG_DOMAIN_WAYLAND, "WaylandSubCompositor"};
}

struct wl_subcompositor_interface IWaylandSubCompositor::impl_ = {
    .destroy = WaylandResourceObject::DefaultDestroyResource,
    .get_subsurface = IWaylandSubCompositor::GetSubSurface};

void IWaylandSubCompositor::GetSubSurface(struct wl_client *client, struct wl_resource *resource,
    uint32_t id, struct wl_resource *surface, struct wl_resource *parent)
{
    CAST_OBJECT_AND_CALL_FUNC(WaylandSubCompositorObject, resource,
        "OECompositorInterface::CreateRegion: failed to find object.", CreateSubSurface, id, surface, parent);
}

OHOS::sptr<WaylandSubCompositor> WaylandSubCompositor::Create(struct wl_display *display)
{
    if (display == nullptr) {
        LOG_ERROR("display is nullptr");
        return nullptr;
    }

    return OHOS::sptr<WaylandSubCompositor>(new WaylandSubCompositor(display));
}

WaylandSubCompositor::WaylandSubCompositor(struct wl_display *display)
    : WaylandGlobal(display, &wl_subcompositor_interface, WL_SUBCOMPOSITOR_MAX_VERSION) {}

WaylandSubCompositor::~WaylandSubCompositor() noexcept {}

void WaylandSubCompositor::Bind(struct wl_client *client, uint32_t version, uint32_t id)
{
    auto object = OHOS::sptr<WaylandSubCompositorObject>(new WaylandSubCompositorObject(client, version, id));
    if (object == nullptr) {
        LOG_ERROR("no memory");
        return;
    }
    WaylandObjectsPool::GetInstance().AddObject(ObjectId(object->WlClient(), object->Id()), object);
}

WaylandSubCompositorObject::WaylandSubCompositorObject(struct wl_client *client,
    uint32_t version, uint32_t id)
    : WaylandResourceObject(client, &wl_subcompositor_interface, version, id, &IWaylandSubCompositor::impl_) {}

WaylandSubCompositorObject::~WaylandSubCompositorObject() noexcept {}

void WaylandSubCompositorObject::CreateSubSurface(uint32_t id, struct wl_resource *surface, struct wl_resource *parent)
{
    auto subSurface = WaylandSubSurface::Create(WlClient(), wl_resource_get_version(WlResource()), id, surface, parent);
    if (subSurface == nullptr) {
        LOG_ERROR("no memory");
        return;
    }
}
} // namespace Wayland
} // namespace FT
