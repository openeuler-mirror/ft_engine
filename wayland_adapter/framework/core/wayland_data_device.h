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

#include <wayland-server-protocol.h>
#include "wayland_resource_object.h"
#include "wayalnd_utils.h"
#include "wayland_data_offer.h"
#include "wayland_pointer.h"
#include "wayland_surface.h"
#include "wayland_seat.h"

namespace FT {
namespace Wayland {
struct IWaylandDataDeviceInterface {
    /**
     * start drag-and-drop operation
     *
     * @param source data source for the eventual transfer
     * @param origin surface where the drag originates
     * @param icon drag-and-drop icon surface
     * @param serial serial number of the implicit grab on the origin
     */
    static void StartDrag(
        struct wl_client *client,
        struct wl_resource *resource,
        struct wl_resource *source,
        struct wl_resource *origin,
        struct wl_resource *icon,
        uint32_t serial);

    /**
     * copy data to the selection
     *
     * @param source data source for the selection
     * @param serial serial number of the event that triggered this request
     */
    static void SetSelection(
        struct wl_client *client,
        struct wl_resource *resource,
        struct wl_resource *source,
        uint32_t serial);
    /**
     * destroy data device
     *
     * @since 2
     */
    static void Release(struct wl_client *client, struct wl_resource *resource);
    static struct wl_data_device_interface impl_;
};

class WaylandDataDevice final : public WaylandResourceObject {
public:
    static OHOS::sptr<WaylandDataDevice> Create(struct wl_client *client, uint32_t version, uint32_t id, struct wl_resource *seat);
    ~WaylandDataDevice() noexcept override;

    friend struct IWaylandDataDeviceInterface;
    // IWaylandDataDeviceInterface will call these member functions.
    void StartDrag(struct wl_resource *source, struct wl_resource *origin, struct wl_resource *icon, uint32_t serial);
    void SetSelection(struct wl_resource *source, uint32_t serial);
    void Release();

private:
    WaylandDataDevice(struct wl_client *client, uint32_t version, uint32_t id, struct wl_resource *seat);
    OHOS::sptr<WaylandDataOffer> DataSourceSendOffer(OHOS::sptr<WaylandDataSource> source, struct wl_resource *target);
    int32_t PointerStartDrag(
        OHOS::sptr<WaylandPointer> pointer,
        OHOS::sptr<WaylandDataSource> dataSource,
        OHOS::sptr<WaylandSurface> iconSurface,
        uint32_t serial);
    struct wl_resource *seatResource_ = nullptr;
};
} // namespace wayland
} // namespace FT

