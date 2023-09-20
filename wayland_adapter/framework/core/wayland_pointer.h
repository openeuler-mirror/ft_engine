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

#include <mutex>
#include "wayland_resource_object.h"

namespace FT {
namespace Wayland {
struct IWaylandPointer {
    static void SetCursor(struct wl_client *client, struct wl_resource *resource,
        uint32_t serial, struct wl_resource *surface, int32_t hotspot_x, int32_t hotspot_y);
    static struct wl_pointer_interface impl_;
};

class WaylandPointer final : public WaylandResourceObject {
    friend struct IWaylandPointer;

public:
    static OHOS::sptr<WaylandPointer> Create(struct wl_client *client, uint32_t version, uint32_t id);
    ~WaylandPointer() noexcept override;

    void OnPointerLeave(struct wl_resource *surface_resource);
    void OnPointerEnter(int32_t posX, int32_t posY, struct wl_resource *surface_resource);
    void OnPointerButton(uint32_t time, uint32_t button, bool isPressed);
    void OnPointerMotionAbsolute(uint32_t time, int32_t posX, int32_t posY);
    bool IsCursorSurface(struct wl_resource *surface);

private:
    WaylandPointer(struct wl_client *client, uint32_t version, uint32_t id);
    void SetCursor(uint32_t serial, struct wl_resource *surface, int32_t hotsPotx, int32_t hotsPoty);
    struct wl_resource *cursorSurface_ = nullptr;
    mutable std::mutex mutex_;
};
} // namespace Wayland
} // namespace FT