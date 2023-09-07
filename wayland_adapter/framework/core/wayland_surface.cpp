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

#include "wayland_surface.h"

#include "wayland_objects_pool.h"

namespace FT {
namespace Wayland {
namespace {
    constexpr HiLogLabel LABEL = {LOG_CORE, HILOG_DOMAIN_WAYLAND, "WaylandSurface"};
}

struct wl_surface_interface IWaylandSurface::impl_ = {
    .destroy = &WaylandResourceObject::DefaultDestroyResource,
    .attach = Attach,
    .damage = Damage,
    .frame = Frame,
    .set_opaque_region = SetOpaqueRegion,
    .set_input_region = SetInputRegion,
    .commit = Commit,
    .set_buffer_transform = SetBufferTransform,
    .set_buffer_scale = SetBufferScale,
    .damage_buffer = DamageBuffer,
    .offset = Offset};

void IWaylandSurface::Attach(struct wl_client *client, struct wl_resource *surfaceResource,
    struct wl_resource *bufferResource, int32_t x, int32_t y)
{
    CAST_OBJECT_AND_CALL_FUNC(WaylandSurface, surfaceResource,
        "IWaylandSurface::Attach: failed to find object.", Attach, bufferResource, x, y);
}

void IWaylandSurface::Damage(struct wl_client *client, struct wl_resource *resource,
    int32_t x, int32_t y, int32_t width, int32_t height)
{
    CAST_OBJECT_AND_CALL_FUNC(WaylandSurface, resource,
        "IWaylandSurface::Damage: failed to find object.", Damage, x, y, width, height);
}

void IWaylandSurface::Frame(struct wl_client *client, struct wl_resource *resource, uint32_t callback)
{
    CAST_OBJECT_AND_CALL_FUNC(WaylandSurface, resource,
        "IWaylandSurface::Frame: failed to find object.", Frame, callback);
}

void IWaylandSurface::SetOpaqueRegion(struct wl_client *client,
    struct wl_resource *surfaceResource, struct wl_resource *regionResource)
{
    CAST_OBJECT_AND_CALL_FUNC(WaylandSurface, surfaceResource,
        "IWaylandSurface::SetOpaqueRegion: failed to find object.", SetOpaqueRegion, regionResource);
}

void IWaylandSurface::SetInputRegion(struct wl_client *client,
    struct wl_resource *surfaceResource, struct wl_resource *regionResource)
{
    CAST_OBJECT_AND_CALL_FUNC(WaylandSurface, surfaceResource,
        "IWaylandSurface::SetInputRegion: failed to find object.", SetInputRegion, regionResource);
}

void IWaylandSurface::Commit(struct wl_client *client, struct wl_resource *resource)
{
    CAST_OBJECT_AND_CALL_FUNC(WaylandSurface, resource, "IWaylandSurface::Commit: failed to find object.", Commit);
}

void IWaylandSurface::SetBufferTransform(struct wl_client *client, struct wl_resource *resource, int32_t transform)
{
    CAST_OBJECT_AND_CALL_FUNC(WaylandSurface, resource,
        "IWaylandSurface::SetBufferTransform: failed to find object.", SetBufferTransform, transform);
}

void IWaylandSurface::SetBufferScale(struct wl_client *client, struct wl_resource *resource, int32_t scale)
{
    CAST_OBJECT_AND_CALL_FUNC(WaylandSurface, resource,
        "IWaylandSurface::SetBufferScale: failed to find object.", SetBufferScale, scale);
}

void IWaylandSurface::DamageBuffer(struct wl_client *client, struct wl_resource *resource,
    int32_t x, int32_t y, int32_t width, int32_t height)
{
    CAST_OBJECT_AND_CALL_FUNC(WaylandSurface, resource,
        "IWaylandSurface::DamageBuffer: failed to find object.", DamageBuffer, x, y, width, height);
}

void IWaylandSurface::Offset(struct wl_client *client, struct wl_resource *resource, int32_t x, int32_t y)
{
    CAST_OBJECT_AND_CALL_FUNC(WaylandSurface, resource,
        "IWaylandSurface::Offset: failed to find object.", Offset, x, y);
}

OHOS::sptr<WaylandSurface> WaylandSurface::Create(struct wl_client *client,
    struct wl_resource *parent, uint32_t version, uint32_t id)
{
    if (client == nullptr) {
        return nullptr;
    }

    auto surface = OHOS::sptr<WaylandSurface>(new WaylandSurface(client, parent, version, id));
    WaylandObjectsPool::GetInstance().AddObject(ObjectId(surface->WlClient(), surface->Id()), surface);
    return surface;
}

WaylandSurface::WaylandSurface(struct wl_client *client, struct wl_resource *parent, uint32_t version, uint32_t id)
    : WaylandResourceObject(client, &wl_surface_interface, version, id, &IWaylandSurface::impl_),
      parent_(parent) {}

WaylandSurface::~WaylandSurface() noexcept {}

void WaylandSurface::AddCommitCallback(SurfaceCommitCallback callback)
{
    commitCallbacks_.push_back(std::move(callback));
}

void WaylandSurface::AddAttachCallback(SurfaceAttachCallback callback)
{
    attachCallbacks_.push_back(std::move(callback));
}

void WaylandSurface::Attach(struct wl_resource *bufferResource, int32_t x, int32_t y)
{
    wl_shm_buffer *shm = wl_shm_buffer_get(bufferResource);
    if (shm == nullptr) {
        LOG_ERROR("wl_shm_buffer_get fail");
        return;
    }

    wl_shm_buffer_begin_access(shm);

    for (auto &cb : attachCallbacks_) {
        cb(shm);
    }

    wl_shm_buffer_end_access(shm);
    wl_callback_send_done(bufferResource, 0);
}

void WaylandSurface::Damage(int32_t x, int32_t y, int32_t width, int32_t height)
{
}

void WaylandSurface::Frame(uint32_t callback)
{
    auto cb = FrameCallback::Create(WlClient(), WAYLAND_VERSION_MAJOR, callback);
    if (cb == nullptr) {
        LOG_ERROR("no memory");
        return;
    }

    WaylandObjectsPool::GetInstance().AddObject(ObjectId(cb->WlClient(), cb->Id()), cb);
    wl_callback_send_done(cb->WlResource(), 0);
    wl_resource_destroy(cb->WlResource());
}

void WaylandSurface::SetOpaqueRegion(struct wl_resource *regionResource)
{
}

void WaylandSurface::SetInputRegion(struct wl_resource *regionResource)
{
}

void WaylandSurface::Commit()
{
    for (auto &cb : commitCallbacks_) {
        cb();
    }
}

void WaylandSurface::SetBufferTransform(int32_t transform)
{
}

void WaylandSurface::SetBufferScale(int32_t scale)
{
}

void WaylandSurface::DamageBuffer(int32_t x, int32_t y, int32_t width, int32_t height)
{
}

void WaylandSurface::Offset(int32_t x, int32_t y)
{
}
} // namespace Wayland
} // namespace FT
