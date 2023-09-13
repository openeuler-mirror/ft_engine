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

#include "wayland_resource_object.h"

namespace FT {
namespace Wayland {
struct IWaylandSubSurface {
    static void SetPosition(struct wl_client *client, struct wl_resource *resource, int32_t x, int32_t y);
    static void PlaceAbove(struct wl_client *client, struct wl_resource *resource, struct wl_resource *sibling);
    static void PlaceBelow(struct wl_client *client, struct wl_resource *resource, struct wl_resource *sibling);
    static void SetSync(struct wl_client *client, struct wl_resource *resource);
    static void SetDesync(struct wl_client *client, struct wl_resource *resource);
    static struct wl_subsurface_interface impl_;
};

class WaylandSubSurface : public WaylandResourceObject {
public:
    static OHOS::sptr<WaylandSubSurface> Create(struct wl_client *client, uint32_t version, uint32_t id,
        struct wl_resource *surface, struct wl_resource *parent);
    ~WaylandSubSurface() noexcept override;

private:
    WaylandSubSurface(struct wl_client *client, uint32_t version, uint32_t id,
        struct wl_resource *surface, struct wl_resource *parent);
};
} // namespace Wayland
} // namespace FT
