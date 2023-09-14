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

#include <thread>
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


class WaylandSeatObject;

class WaylandSeat final : public WaylandGlobal {
    friend struct IWaylandSeat;

public:
    static OHOS::sptr<WaylandSeat> Create(struct wl_display *display);
    static OHOS::sptr<WaylandSeat> GetWaylandSeatGlobal();
    OHOS::sptr<WaylandPointer> GetPointerResource(struct wl_client *client);
    OHOS::sptr<WaylandKeyboard> GetKeyboardResource(struct wl_client *client);
    ~WaylandSeat() noexcept override;

private:
    WaylandSeat(struct wl_display *display);
    void Bind(struct wl_client *client, uint32_t version, uint32_t id) override;
    void UpdateCapabilities();
    std::unordered_map<struct wl_client *, OHOS::sptr<WaylandSeatObject>> seatResourcesMap_;
    std::unique_ptr<std::thread> thread_ = nullptr;
};

class WaylandSeatObject final : public WaylandResourceObject {
    friend struct IWaylandSeat;

public:
    WaylandSeatObject(struct wl_client *client, uint32_t version, uint32_t id);
    ~WaylandSeatObject() noexcept;
    OHOS::sptr<WaylandPointer> GetChildPointer();
    OHOS::sptr<WaylandKeyboard> GetChildKeyboard();

private:
    void GetPointer(uint32_t id);
    void GetKeyboard(uint32_t id);
    void GetTouch(uint32_t id);

    OHOS::sptr<WaylandPointer> pointer_;
    OHOS::sptr<WaylandKeyboard> keyboard_;
};
} // namespace Wayland
} // namespace FT
