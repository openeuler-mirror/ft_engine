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

#include "wayland_keyboard.h"

#include "version.h"
#include "wayland_objects_pool.h"

namespace FT {
namespace Wayland {
namespace {
    constexpr HiLogLabel LABEL = {LOG_CORE, HILOG_DOMAIN_WAYLAND, "WaylandKeyboard"};
}

struct wl_keyboard_interface IWaylandKeyboard::impl_ = {
    .release = WaylandResourceObject::DefaultDestroyResource,
};

OHOS::sptr<WaylandKeyboard> WaylandKeyboard::Create(struct wl_client *client, uint32_t version, uint32_t id)
{
    if (client == nullptr) {
        return nullptr;
    }

    auto keyboard = OHOS::sptr<WaylandKeyboard>(new WaylandKeyboard(client, version, id));
    WaylandObjectsPool::GetInstance().AddObject(ObjectId(keyboard->WlClient(), keyboard->Id()), keyboard);

    return keyboard;
}

WaylandKeyboard::WaylandKeyboard(struct wl_client *client, uint32_t version, uint32_t id)
    : WaylandResourceObject(client, &wl_keyboard_interface, version, id, &IWaylandKeyboard::impl_) {}

WaylandKeyboard::~WaylandKeyboard() noexcept {}
} // namespace Wayland
} // namespace FT