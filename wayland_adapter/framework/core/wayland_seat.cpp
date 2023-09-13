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

#include <mutex>
#include "wayland_seat.h"

#include "wayland_objects_pool.h"
#include "version.h"
#include "input_manager.h"
#include <struct_multimodal.h>

using namespace OHOS::MMI;
namespace FT {
namespace Wayland {
namespace {
    constexpr HiLogLabel LABEL = {LOG_CORE, HILOG_DOMAIN_WAYLAND, "WaylandSeat"};
}

static OHOS::sptr<WaylandSeat> wl_seat_global = nullptr;
std::mutex wl_seat_global_mutex_;

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
    std::lock_guard<std::mutex> lock(wl_seat_global_mutex_);
    if (display == nullptr) {
        LOG_ERROR("display is nullptr");
        return nullptr;
    }

    if (wl_seat_global != nullptr) {
        return wl_seat_global;
    }

    wl_seat_global = OHOS::sptr<WaylandSeat>(new WaylandSeat(display));
    return wl_seat_global;
}

OHOS::sptr<WaylandSeat> WaylandSeat::GetWaylandSeatGlobal()
{
    std::lock_guard<std::mutex> lock(wl_seat_global_mutex_);
    return wl_seat_global;
}

WaylandSeat::WaylandSeat(struct wl_display *display)
    : WaylandGlobal(display, &wl_seat_interface, WL_SEAT_MAX_VERSION) {}

WaylandSeat::~WaylandSeat() noexcept
{
    if (thread_ != nullptr) {
        if (thread_->joinable()) {
            thread_->join();
        }
        thread_ = nullptr;
    }
}

void WaylandSeat::Bind(struct wl_client *client, uint32_t version, uint32_t id)
{
    auto object = OHOS::sptr<WaylandSeatObject>(new WaylandSeatObject(client, version, id));
    if (object == nullptr) {
        LOG_ERROR("no memory");
        return;
    }
    WaylandObjectsPool::GetInstance().AddObject(ObjectId(object->WlClient(), object->Id()), object);
    seatResourcesMap_[client] = object;

    if (thread_ != nullptr) {
        if (thread_->joinable()) {
            thread_->join();
        }
        thread_ = nullptr;
    }
    thread_ = std::make_unique<std::thread>(&WaylandSeat::UpdateCapabilities, this);
}

OHOS::sptr<WaylandPointer> WaylandSeat::GetPointerResource(struct wl_client *client)
{
    if (seatResourcesMap_.count(client) == 0) {
        return nullptr;
    }

    auto seatResource = seatResourcesMap_.at(client);
    // Erase seat resource from map if it is destroyed.
    if (seatResource == nullptr) {
        seatResourcesMap_.erase(client);
        return nullptr;
    }
    LOG_INFO("get seatResource name=%{public}s, id=%{public}d", seatResource->Name().c_str(), seatResource->Id());
    return seatResource->GetChildPointer();
}

void WaylandSeat::UpdateCapabilities()
{
    LOG_INFO("UpdateCapabilities in");
    uint32_t cap = 0;
    int32_t DevNums = 0;
    int32_t hasGetDevNums = 0;
    bool isGetIds = false;
    int32_t wait_count = 0;

    auto GetDeviceCb = [&hasGetDevNums, &cap](std::shared_ptr<InputDevice> inputDevice) {
        LOG_INFO("Get device success, id=%{public}d, name=%{public}s, type=%{public}d",
            inputDevice->GetId(), inputDevice->GetName().c_str(), inputDevice->GetType());
        if (inputDevice->GetType() == (int32_t)DEVICE_TYPE_MOUSE) {
            cap |= WL_SEAT_CAPABILITY_POINTER;
        } else if (inputDevice->GetType() == (int32_t)DEVICE_TYPE_KEYBOARD) {
            cap |= WL_SEAT_CAPABILITY_KEYBOARD;
        }
        hasGetDevNums++;
    };
    auto GetDeviceIdsCb = [&DevNums, &isGetIds](std::vector<int32_t> ids) {
        DevNums = ids.size();
        isGetIds = true;
    };
    (void)InputManager::GetInstance()->GetDeviceIds(GetDeviceIdsCb);
    while (!isGetIds && wait_count < 100) {
        usleep(3 * 1000); // wait for GetDeviceIdsCb finish
        wait_count++;
    }

    for (int32_t i = 0; i < DevNums; i++) {
        InputManager::GetInstance()->GetDevice(i, GetDeviceCb);
    }

    wait_count = 0;
    while (hasGetDevNums != DevNums && wait_count < 100) {
        usleep(3 * 1000); // wait for GetDeviceCb finish
        wait_count++;
    }

    for (auto iter = seatResourcesMap_.begin(); iter != seatResourcesMap_.end();) {
        auto seatObj = iter->second;
        if (seatObj == nullptr) {
            iter = seatResourcesMap_.erase(iter);
        } else {
            wl_seat_send_capabilities(seatObj->WlResource(), cap);
            ++iter;
        }
    }
}

WaylandSeatObject::WaylandSeatObject(struct wl_client *client, uint32_t version, uint32_t id)
    : WaylandResourceObject(client, &wl_seat_interface, version, id, &IWaylandSeat::impl_) {}

WaylandSeatObject::~WaylandSeatObject() noexcept {}

OHOS::sptr<WaylandPointer> WaylandSeatObject::GetChildPointer()
{
    return pointer_;
}

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
