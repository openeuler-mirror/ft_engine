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

#include "wayland_data_device.h"
#include "wayland_objects_pool.h"
#include "wayland_data_source.h"

namespace FT {
namespace Wayland {
namespace {
    constexpr HiLogLabel LABEL = {LOG_CORE, HILOG_DOMAIN_WAYLAND, "WaylandDataDevice"};
}
struct wl_data_device_interface IWaylandDataDeviceInterface::impl_ = {
    .start_drag = StartDrag,
    .set_selection = SetSelection,
    .release = Release,
};

void IWaylandDataDeviceInterface::StartDrag(
    struct wl_client *client,
    struct wl_resource *resource,
    struct wl_resource *source,
    struct wl_resource *origin,
    struct wl_resource *icon,
    uint32_t serial)
{
    LOG_DEBUG(" Request: wl_data_device-->start_drag.");
    UNUSED(client);
    CAST_OBJECT_AND_CALL_FUNC(
        WaylandDataDevice,
        resource,
        "IWaylandDataDeviceInterface::StartDrag: failed to find object.",
        StartDrag,
        source,
        origin,
        icon,
        serial);
}

void IWaylandDataDeviceInterface::SetSelection(
    struct wl_client *client,
    struct wl_resource *resource,
    struct wl_resource *source,
    uint32_t serial)
{
    LOG_DEBUG("Request: wl_data_device-->set_selection.");
    UNUSED(client);
    CAST_OBJECT_AND_CALL_FUNC(
        WaylandDataDevice,
        resource,
        "IWaylandDataDeviceInterface::SetSelection: failed to find object.",
        SetSelection,
        source,
        serial);
}

void IWaylandDataDeviceInterface::Release(struct wl_client *client, struct wl_resource *resource)
{
    LOG_DEBUG("Request: wl_data_device-->release.");
    UNUSED(client);
    CAST_OBJECT_AND_CALL_FUNC(
        WaylandDataDevice, resource, "IWaylandDataDeviceInterface::Release: failed to find object.", Release);
}

OHOS::sptr<WaylandDataDevice> WaylandDataDevice::Create(
    struct wl_client *client,
    uint32_t version,
    uint32_t id,
    struct wl_resource *seat)
{
    if (client == nullptr) {
        return nullptr;
    }

    auto dataDevice = OHOS::sptr<WaylandDataDevice>(new WaylandDataDevice(client, version, id, seat));
    WaylandObjectsPool::GetInstance().AddObject(ObjectId(dataDevice->WlClient(), dataDevice->Id()), dataDevice);
    return dataDevice;
}

WaylandDataDevice::WaylandDataDevice(struct wl_client *client, uint32_t version, uint32_t id, struct wl_resource *seat)
    : WaylandResourceObject(client, &wl_data_device_interface, version, id, &IWaylandDataDeviceInterface::impl_),
      seatResource_(seat)
{
    LOG_DEBUG("WaylandDataSource ctor.");
}

WaylandDataDevice::~WaylandDataDevice() noexcept
{
    LOG_DEBUG("WaylandDataDevice dtor.");
}

void WaylandDataDevice::StartDrag(
    struct wl_resource *source,
    struct wl_resource *origin,
    struct wl_resource *icon,
    uint32_t serial)
{
    LOG_DEBUG("WaylandDataDevice StartDrag.");
}

int32_t WaylandDataDevice::PointerStartDrag(
    OHOS::sptr<WaylandPointer> pointer,
    OHOS::sptr<WaylandDataSource> dataSource,
    OHOS::sptr<WaylandSurface> iconSurface,
    uint32_t serial)
{
    // pointer clear focus
    wl_pointer_send_leave(pointer->WlResource(), serial, iconSurface->WlResource());
    if (wl_resource_get_version(pointer->WlResource()) >= WL_POINTER_FRAME_SINCE_VERSION) {
        wl_pointer_send_frame(pointer->WlResource());
    }
    auto seatResource = CastFromResource<WaylandSeatObject>((seatResource_));
    if (seatResource) {
        std::list<OHOS::sptr<WaylandKeyboard>> KeyboardList;
        seatResource->GetChildKeyboard(KeyboardList);
        for (auto& keyboardResourceItem : KeyboardList) {
            wl_keyboard_send_leave(keyboardResourceItem->WlResource(), serial, iconSurface->WlResource());
        }
    }

    return 0;
}

void WaylandDataDevice::SetSelection(struct wl_resource *source, uint32_t serial)
{
    LOG_DEBUG("WaylandDataDevice SetSelection.");
}

OHOS::sptr<WaylandDataOffer> WaylandDataDevice::DataSourceSendOffer(OHOS::sptr<WaylandDataSource> source, struct wl_resource *target)
{
    auto dataOffer = WaylandDataOffer::Create(wl_resource_get_client(target), wl_resource_get_version(target), 0);
    if (dataOffer == nullptr) {
        LOG_ERROR("WaylandDataDevice DataSourceSendOffer failed");
        return nullptr;
    }
    dataOffer->inAsk_ = false;
    dataOffer->dndActions_ = 0;
    dataOffer->preferredDndAction_ = WL_DATA_DEVICE_MANAGER_DND_ACTION_NONE;
    dataOffer->dataSource_ = source;
    wl_data_device_send_data_offer(target, dataOffer->WlResource());
    source->accepted_ = false;
    return dataOffer;
}

void WaylandDataDevice::Release()
{
    LOG_DEBUG ("WaylandDataDevice::Release.");
}
} // namespace wayland
} // namespace FT
