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

#include <list>

#include <wayland-server-protocol.h>
#include "wayland_resource_object.h"

namespace FT {
namespace Wayland {
struct IWaylandSurface {
    static void Attach(struct wl_client *client, struct wl_resource *surfaceResource,
        struct wl_resource *bufferResource, int32_t x, int32_t y);
    static void Damage(struct wl_client *client, struct wl_resource *resource,
        int32_t x, int32_t y, int32_t width, int32_t height);
    static void Frame(struct wl_client *client, struct wl_resource *resource, uint32_t callback);
    static void SetOpaqueRegion(struct wl_client *client, struct wl_resource *surfaceResource,
        struct wl_resource *regionResource);
    static void SetInputRegion(struct wl_client *client, struct wl_resource *surfaceResource,
        struct wl_resource *regionResource);
    static void Commit(struct wl_client *client, struct wl_resource *resource);
    static void SetBufferTransform(struct wl_client *client, struct wl_resource *resource, int32_t transform);
    static void SetBufferScale(struct wl_client *client, struct wl_resource *resource, int32_t scale);
    static void DamageBuffer(struct wl_client *client, struct wl_resource *resource,
        int32_t x, int32_t y, int32_t width, int32_t height);
    static void Offset(struct wl_client *client, struct wl_resource *resource, int32_t x, int32_t y);
    static struct wl_surface_interface impl_;
};

class WaylandSurface final : public WaylandResourceObject {
    friend struct IWaylandSurface;

public:
    static OHOS::sptr<WaylandSurface> Create(struct wl_client *client, struct wl_resource *parent, uint32_t version, uint32_t id);
    ~WaylandSurface() noexcept override;

    using SurfaceCommitCallback = std::function<void()>;
    void AddCommitCallback(SurfaceCommitCallback callback);
    using SurfaceAttachCallback = std::function<void(struct wl_shm_buffer *shm)>;
    void AddAttachCallback(SurfaceAttachCallback callback);

private:
    WaylandSurface(struct wl_client *client, struct wl_resource *parent, uint32_t version, uint32_t id);

    void Attach(struct wl_resource *bufferResource, int32_t x, int32_t y);
    void Damage(int32_t x, int32_t y, int32_t width, int32_t height);
    void Frame(uint32_t callback);
    void SetOpaqueRegion(struct wl_resource *regionResource);
    void SetInputRegion(struct wl_resource *regionResource);
    void Commit();
    void SetBufferTransform(int32_t transform);
    void SetBufferScale(int32_t scale);
    void DamageBuffer(int32_t x, int32_t y, int32_t width, int32_t height);
    void Offset(int32_t x, int32_t y);

    struct wl_resource *parent_ = nullptr;
    std::list<SurfaceCommitCallback> commitCallbacks_;
    std::list<SurfaceAttachCallback> attachCallbacks_;
};
} // namespace Wayland
} // namespace FT