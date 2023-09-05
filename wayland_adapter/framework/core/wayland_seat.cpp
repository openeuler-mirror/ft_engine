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

#include "wayland_seat.h"

#include "wayland_objects_pool.h"
#include "version.h"

namespace FT {
namespace Wayland {
namespace {
    constexpr HiLogLabel LABEL = {LOG_CORE, HILOG_DOMAIN_WAYLAND, "WaylandSeat"};
}

struct wl_seat_interface IWaylandSeat::impl_ = {
    .get_pointer = GetPointer,
    .get_keyboard = GetKeyboard,
    .get_touch = GetTouch,
    .release = WaylandResourceObject::DefaultDestroyResource,};

void IWaylandSeat::GetPointer(struct wl_client *client, struct wl_resource *resource, uint32_t id)
{
    CAST_OBJECT_AND_CALL_FUNC(WaylandSeatObject, resource,
        "IWaylandSeat::GetPointer: failed to find object.", GetPointer, id);
}

void IWaylandSeat::GetKeyboard(struct wl_client *client, struct wl_resource *resource, uint32_t id)
{
    CAST_OBJECT_AND_CALL_FUNC(WaylandSeatObject, resource,
        "IWaylandSeat::GetKeyboard: failed to find object.", GetKeyboard, id);
}

void IWaylandSeat::GetTouch(struct wl_client *client, struct wl_resource *resource, uint32_t id)
{
    CAST_OBJECT_AND_CALL_FUNC(WaylandSeatObject, resource,
        "IWaylandSeat::GetTouch: failed to find object.", GetTouch, id);
}

OHOS::sptr<WaylandSeat> WaylandSeat::Create(struct wl_display *display)
{
    if (display == nullptr) {
        LOG_ERROR("display is nullptr");
        return nullptr;
    }

    return OHOS::sptr<WaylandSeat>(new WaylandSeat(display));
}

WaylandSeat::WaylandSeat(struct wl_display *display)
    : WaylandGlobal(display, &wl_seat_interface, WL_SEAT_MAX_VERSION) {}

WaylandSeat::~WaylandSeat() noexcept {}

void WaylandSeat::Bind(struct wl_client *client, uint32_t version, uint32_t id)
{
    auto object = OHOS::sptr<WaylandSeatObject>(new WaylandSeatObject(client, version, id));
    if (object == nullptr) {
        LOG_ERROR("no memory");
        return;
    }
    WaylandObjectsPool::GetInstance().AddObject(ObjectId(object->WlClient(), object->Id()), object);
}

WaylandSeatObject::WaylandSeatObject(struct wl_client *client, uint32_t version, uint32_t id)
    : WaylandResourceObject(client, &wl_seat_interface, version, id, &IWaylandSeat::impl_) {}

WaylandSeatObject::~WaylandSeatObject() noexcept {}

void WaylandSeatObject::GetPointer(uint32_t id)
{
    auto pointer = WaylandPointer::Create(WlClient(), wl_resource_get_version(WlResource()), id);
    if (pointer == nullptr) {
        LOG_ERROR("no memory");
        return;
    }

    pointer_ = pointer;
}

void WaylandSeatObject::GetKeyboard(uint32_t id)
{
    auto keyboard = WaylandKeyboard::Create(WlClient(), wl_resource_get_version(WlResource()), id);
    if (keyboard == nullptr) {
        LOG_ERROR("no memory");
        return;
    }

    keyboard_ = keyboard;
}

void WaylandSeatObject::GetTouch(uint32_t id)
{
}
} // namespace Wayland
} // namespace FT
