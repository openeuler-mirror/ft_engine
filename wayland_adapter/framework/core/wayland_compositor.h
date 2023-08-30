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

#include "wayland_global.h"

namespace FT {
namespace Wayland {
struct IWaylandCompositor {
    static void CreateSurface(struct wl_client *client, struct wl_resource *resource, uint32_t id);
    static void CreateRegion(struct wl_client *client, struct wl_resource *resource, uint32_t id);
    static struct wl_compositor_interface impl_;
};

class WaylandCompositor final : public WaylandGlobal {
    friend struct IWaylandCompositor;

public:
    static OHOS::sptr<WaylandCompositor> Create(struct wl_display *display);
    ~WaylandCompositor() noexcept override;

private:
    WaylandCompositor(struct wl_display *display);
    void Bind(struct wl_client *client, uint32_t version, uint32_t id) override;
};

class WaylandCompositorObject final : public WaylandResourceObject {
    friend struct IWaylandCompositor;

public:
    WaylandCompositorObject(struct wl_client *client, uint32_t version, uint32_t id);
    ~WaylandCompositorObject() noexcept;

private:
    void CreateSurface(struct wl_client *client, struct wl_resource *resource, uint32_t id);
    void CreateRegion(struct wl_client *client, struct wl_resource *resource, uint32_t id);
};
} // namespace Wayland
} // namespace FT
