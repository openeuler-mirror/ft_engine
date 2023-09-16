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

#include "wayland_data_device_manager.h"
#include "wayland_objects_pool.h"
#include "version.h"
#include "wayland_data_source.h"
#include "wayland_data_device.h"

namespace FT {
namespace Wayland {
namespace {
    constexpr HiLogLabel LABEL = {LOG_CORE, HILOG_DOMAIN_WAYLAND, "WaylandDataDeviceManager"};
}
struct wl_data_device_manager_interface IWaylandDataDeviceManagerInterface::impl_ = {
    .create_data_source = CreateDataSource,
    .get_data_device = GetDataDevice};

void IWaylandDataDeviceManagerInterface::CreateDataSource(struct wl_client *client, struct wl_resource *resource, uint32_t id)
{
    LOG_DEBUG("Request: wl_data_device_manager-->create_data_source.");
    CAST_OBJECT_AND_CALL_FUNC(
        WaylandDataDeviceManager::WaylandDataDeviceManagerResourceObject,
        resource,
        "IWaylandDataDeviceManagerInterface::CreateDataSource: failed to find object.",
        CreateDataSource,
        client,
        resource,
        id);
}

void IWaylandDataDeviceManagerInterface::GetDataDevice(
    struct wl_client *client,
    struct wl_resource *resource,
    uint32_t id,
    struct wl_resource *seat)
{
    LOG_DEBUG("Request: wl_data_device_manager-->get_data_device.");
    CAST_OBJECT_AND_CALL_FUNC(
        WaylandDataDeviceManager::WaylandDataDeviceManagerResourceObject,
        resource,
        "IWaylandDataDeviceManagerInterface::GetDataDevice: failed to find object.",
        GetDataDevice,
        client,
        resource,
        id,
        seat);
}

OHOS::sptr<WaylandDataDeviceManager> WaylandDataDeviceManager::Create(struct wl_display *display)
{
    if (display == nullptr) {
        LOG_ERROR("WaylandDataDeviceManager::Create failed: wl_display is nullptr.");
        return nullptr;
    }

    return OHOS::sptr<WaylandDataDeviceManager>(new WaylandDataDeviceManager(display));
}

WaylandDataDeviceManager::WaylandDataDeviceManager(struct wl_display *display)
    : WaylandGlobal(display, &wl_data_device_manager_interface, WL_DATA_DEVICE_MANAGER_MAX_VERSION)
{
    LOG_DEBUG("WaylandDataDeviceManager ctor.");
}

WaylandDataDeviceManager::~WaylandDataDeviceManager() noexcept
{
    LOG_DEBUG("WaylandDataDeviceManager dtor.");
}

void WaylandDataDeviceManager::Bind(struct wl_client *client, uint32_t version, uint32_t id)
{
    auto object = OHOS::sptr<WaylandDataDeviceManagerResourceObject>(new WaylandDataDeviceManagerResourceObject(client, version, id));
    if (object == nullptr) {
        LOG_ERROR("WaylandDataDeviceManager Bind failed.");
        return;
    }
    WaylandObjectsPool::GetInstance().AddObject(ObjectId(object->WlClient(), object->Id()), object);
}

WaylandDataDeviceManager::WaylandDataDeviceManagerResourceObject::WaylandDataDeviceManagerResourceObject(
    struct wl_client *client,
    uint32_t version,
    uint32_t id)
    : WaylandResourceObject(client, &wl_data_device_manager_interface, version, id, &IWaylandDataDeviceManagerInterface::impl_)
{
    LOG_DEBUG("WaylandDataDeviceManagerResourceObject ctor.");
}

WaylandDataDeviceManager::WaylandDataDeviceManagerResourceObject::~WaylandDataDeviceManagerResourceObject() noexcept
{
    LOG_DEBUG("WaylandDataDeviceManagerResourceObject dtor.");
}

void WaylandDataDeviceManager::WaylandDataDeviceManagerResourceObject::CreateDataSource(
    struct wl_client *client,
    struct wl_resource *resource,
    uint32_t id)
{
    LOG_DEBUG("CreateDataSource.");
    auto dataSource = WaylandDataSource::Create(client, wl_resource_get_version(resource), id);
    if (dataSource == nullptr) {
        LOG_ERROR("CreateDataSource failed.");
        return;
    }
}

void WaylandDataDeviceManager::WaylandDataDeviceManagerResourceObject::GetDataDevice(
    struct wl_client *client,
    struct wl_resource *resource,
    uint32_t id,
    struct wl_resource *seat)
{
    LOG_DEBUG("GetDataDevice.");
    auto dataDevice = WaylandDataDevice::Create(client, wl_resource_get_version(resource), id, seat);
    if (dataDevice == nullptr) {
        LOG_DEBUG("GetDataDevice failed.");
        return;
    }
    if (seat) {
        // auto seatResource = CastFromResource<WaylandSeatObject>((seat));
        // wl_list_insert(&seatResource->dragResourceList_, wl_resource_get_link(dataDevice->WlResource()));
    } else {
        wl_list_init(wl_resource_get_link(dataDevice->WlResource()));
    }
}
} // namespace wayland
} // namespace FT
