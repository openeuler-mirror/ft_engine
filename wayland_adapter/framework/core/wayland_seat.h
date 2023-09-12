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
#include "wayland_pointer.h"
#include "wayland_keyboard.h"

namespace FT {
namespace Wayland {
struct IWaylandSeat {
    static void GetPointer(struct wl_client *client, struct wl_resource *resource, uint32_t id);
    static void GetKeyboard(struct wl_client *client, struct wl_resource *resource, uint32_t id);
    static void GetTouch(struct wl_client *client, struct wl_resource *resource, uint32_t id);
    static struct wl_seat_interface impl_;
};

class WaylandSeat final : public WaylandGlobal {
    friend struct IWaylandSeat;

public:
    static OHOS::sptr<WaylandSeat> Create(struct wl_display *display);
    ~WaylandSeat() noexcept override;

private:
    WaylandSeat(struct wl_display *display);
    void Bind(struct wl_client *client, uint32_t version, uint32_t id) override;
};

class WaylandSeatObject final : public WaylandResourceObject {
    friend struct IWaylandSeat;

public:
    WaylandSeatObject(struct wl_client *client, uint32_t version, uint32_t id);
    ~WaylandSeatObject() noexcept;

private:
    void GetPointer(uint32_t id);
    void GetKeyboard(uint32_t id);
    void GetTouch(uint32_t id);

    OHOS::wptr<WaylandPointer> pointer_;
    OHOS::wptr<WaylandKeyboard> keyboard_;
};
} // namespace Wayland
} // namespace FT