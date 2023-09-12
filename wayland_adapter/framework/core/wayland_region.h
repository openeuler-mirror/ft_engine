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
#include "wayalnd_utils.h"

namespace FT {
namespace Wayland {
struct IWaylandRegion {
    static void Destroy(struct wl_client *client, struct wl_resource *resource);
    static void Add(struct wl_client *client, struct wl_resource *resource,
        int32_t x, int32_t y, int32_t width, int32_t height);
    static void Subtract(struct wl_client *client, struct wl_resource *resource,
        int32_t x, int32_t y, int32_t width, int32_t height);
    static struct wl_region_interface impl_;
};

class WaylandRegion final : public WaylandResourceObject {
    friend struct IWaylandRegion;

public:
    static OHOS::sptr<WaylandRegion> Create(struct wl_client *client, struct wl_resource *parent,
        uint32_t version, uint32_t id);
    ~WaylandRegion() noexcept override;

    Rect GetRect();

private:
    WaylandRegion(struct wl_client *client, struct wl_resource *parent, uint32_t version, uint32_t id);

    void Destroy(struct wl_client *client, struct wl_resource *resource);
    void Add(int32_t x, int32_t y, int32_t width, int32_t height);
    void Subtract(int32_t x, int32_t y, int32_t width, int32_t height);

    struct wl_resource *parent_ = nullptr;
    Rect rect_;
};
} // namespace Wayland
} // namespace FT
