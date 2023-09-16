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


#include <xdg-shell-server-protocol.h>
#include "wayland_global.h"

namespace FT {
namespace Wayland {
struct IWaylandDataDeviceManagerInterface {
    /**
     * create a new data source
     *
     * Create a new data source.
     * @param id data source to create
     */
    static void CreateDataSource(struct wl_client *client, struct wl_resource *resource, uint32_t id);
    /**
     * create a new data device
     *
     * Create a new data device for a given seat.
     * @param id data device to create
     * @param seat seat associated with the data device
     */
    static void GetDataDevice(
        struct wl_client *client,
        struct wl_resource *resource,
        uint32_t id,
        struct wl_resource *seat);

    static struct wl_data_device_manager_interface impl_;
};

class WaylandDataDeviceManager final : public WaylandGlobal {
public:
    static OHOS::sptr<WaylandDataDeviceManager> Create(struct wl_display *display);
    ~WaylandDataDeviceManager() noexcept override;

private:
    WaylandDataDeviceManager(struct wl_display *display);
    class WaylandDataDeviceManagerResourceObject final : public WaylandResourceObject {
    public:
        WaylandDataDeviceManagerResourceObject(struct wl_client *client, uint32_t version, uint32_t id);
        ~WaylandDataDeviceManagerResourceObject() noexcept;

    private:
        friend struct IWaylandDataDeviceManagerInterface;
        // IWaylandDataDeviceManagerInterface will call these member functions.
        void CreateDataSource(struct wl_client *client, struct wl_resource *resource, uint32_t id);
        void GetDataDevice(
            struct wl_client *client,
            struct wl_resource *resource,
            uint32_t id,
            struct wl_resource *seat);
    };
    friend struct IWaylandDataDeviceManagerInterface;

    void Bind(struct wl_client *client, uint32_t version, uint32_t id) override;
};
} // namespace wayland
} // namespace FT

