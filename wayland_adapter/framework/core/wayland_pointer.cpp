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

#include "wayland_pointer.h"

#include "version.h"
#include "wayland_objects_pool.h"

namespace FT {
namespace Wayland {
namespace {
    constexpr HiLogLabel LABEL = {LOG_CORE, HILOG_DOMAIN_WAYLAND, "WaylandPointer"};
}

struct wl_pointer_interface IWaylandPointer::impl_ = {
    .set_cursor = SetCursor,
    .release = WaylandResourceObject::DefaultDestroyResource,
};

void IWaylandPointer::SetCursor(struct wl_client *client, struct wl_resource *resource,
    uint32_t serial,  struct wl_resource *surface, int32_t hotspot_x, int32_t hotspot_y)
{
    CAST_OBJECT_AND_CALL_FUNC(WaylandPointer, resource,  "IWaylandPointer::SetCursor: failed to find object.",
        SetCursor, serial, surface, hotspot_x, hotspot_y);
}

OHOS::sptr<WaylandPointer> WaylandPointer::Create(struct wl_client *client, uint32_t version, uint32_t id)
{
    if (client == nullptr) {
        return nullptr;
    }

    auto pointer = OHOS::sptr<WaylandPointer>(new WaylandPointer(client, version, id));
    WaylandObjectsPool::GetInstance().AddObject(ObjectId(pointer->WlClient(), pointer->Id()), pointer);

    return pointer;
}

WaylandPointer::WaylandPointer(struct wl_client *client, uint32_t version, uint32_t id)
    : WaylandResourceObject(client, &wl_pointer_interface, version, id, &IWaylandPointer::impl_) {}

WaylandPointer::~WaylandPointer() noexcept {}


void WaylandPointer::OnPointerButton(uint32_t time, uint32_t button, bool isPressed)
{
    wl_resource *pointer = WlResource();
    if (pointer == nullptr) {
        return;
    }
    wl_display *display = WlDisplay();
    if (display == nullptr) {
        return;
    }
    uint32_t serial = wl_display_next_serial(display);
    uint32_t state = isPressed ? WL_POINTER_BUTTON_STATE_PRESSED : WL_POINTER_BUTTON_STATE_RELEASED;
    wl_pointer_send_button(pointer, serial, time, button, state);
}

void WaylandPointer::OnPointerMotionAbsolute(uint32_t time, int32_t posX, int32_t posY)
{
    wl_fixed_t posFixedX = wl_fixed_from_int(posX);
    wl_fixed_t posFixedY = wl_fixed_from_int(posY);
    wl_resource *pointer = WlResource();
    if (pointer == nullptr) {
        return;
    }

    wl_pointer_send_motion(pointer, time, posFixedX, posFixedY);
}

void WaylandPointer::OnPointerLeave(struct wl_resource *surface_resource)
{
    LOG_WARN("WaylandPointer::OnPointerLeave in");
    wl_display *display = WlDisplay();
    if (display == nullptr) {
        return;
    }
    uint32_t serial = wl_display_next_serial(display);
    wl_resource *pointer = WlResource();
    if (pointer == nullptr) {
        return;
    }
    wl_pointer_send_leave(pointer, serial, surface_resource);
}
void WaylandPointer::OnPointerEnter(int32_t posX, int32_t posY, struct wl_resource *surface_resource)
{
    LOG_WARN("WaylandPointer::OnPointerEnter in");
    wl_fixed_t posFixedX = wl_fixed_from_int(posX);
    wl_fixed_t posFixedY = wl_fixed_from_int(posY);
    wl_resource *pointer = WlResource();
    if (pointer == nullptr) {
        return;
    }
    wl_display *display = WlDisplay();
    if (display == nullptr) {
        return;
    }
    uint32_t serial = wl_display_next_serial(display);
    wl_pointer_send_enter(pointer, serial, surface_resource, posFixedX, posFixedY);
}

void WaylandPointer::SetCursor(uint32_t serial, struct wl_resource *surface, int32_t hotsPotx, int32_t hotsPoty) {}
} // namespace Wayland
} // namespace FT