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
struct IWaylandSubCompositor {
    static void GetSubSurface(struct wl_client *client, struct wl_resource *resource,
        uint32_t id, struct wl_resource *surface, struct wl_resource *parent);
    static struct wl_subcompositor_interface impl_;
};

class WaylandSubCompositor final : public WaylandGlobal {
    friend struct IWaylandSubCompositor;

public:
    static OHOS::sptr<WaylandSubCompositor> Create(struct wl_display *display);
    ~WaylandSubCompositor() noexcept override;

private:
    WaylandSubCompositor(struct wl_display *display);
    void Bind(struct wl_client *client, uint32_t version, uint32_t id) override;
};

class WaylandSubCompositorObject final : public WaylandResourceObject {
    friend struct IWaylandSubCompositor;

public:
    WaylandSubCompositorObject(struct wl_client *client, uint32_t version, uint32_t id);
    ~WaylandSubCompositorObject() noexcept;

private:
    void CreateSubSurface(uint32_t id, struct wl_resource *surface, struct wl_resource *parent);
};
} // namespace Wayland
} // namespace FT
