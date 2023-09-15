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
#include "wayland_data_source.h"

namespace FT {
namespace Wayland {
struct IWaylandDataOfferInterface {
    /**
     *@brief accept one of the offered mime types
     *
     * @param client
     * @param resource
     * @param serial serial number of the accept request
     * @param mimeType mime type accepted by the client
     */
    static void Accept(struct wl_client *client, struct wl_resource *resource, uint32_t serial, const char *mimeType);
    /**
     *@brief request that the data is transferred
     *
     * @param client
     * @param resource
     * @param mimeType mime type desired by receiver
     * @param fd file descriptor for data transfer
     */
    static void Receive(struct wl_client *client, struct wl_resource *resource, const char *mimeType, int32_t fd);
    /**
     * the offer will no longer be used
     *
     * @param client
     * @param resource
     */
    static void Finish(struct wl_client *client, struct wl_resource *resource);
    /**
     * set the available/preferred drag-and-drop actions
     *
     * @param client
     * @param resource
     * @param dndActions actions supported by the destination client
     * @param preferredAction action preferred by the destination client
     */
    static void SetActions(
        struct wl_client *client,
        struct wl_resource *resource,
        uint32_t dndActions,
        uint32_t preferredAction);
    static struct wl_data_offer_interface impl_;
};

class WaylandDataOffer final : public WaylandResourceObject {
public:
    static OHOS::sptr<WaylandDataOffer> Create(struct wl_client *client, uint32_t version, uint32_t id);
    ~WaylandDataOffer() noexcept override;
    friend struct IWaylandDataOfferInterface;
    // IWaylandDataOfferInterface will call these member functions.
    void Accept(uint32_t serial, const char *mimeType);
    void Receive(const char *mimeType, int32_t fd);
    void Finish();
    void SetActions(uint32_t dndActions, uint32_t preferredAction);

    OHOS::sptr<WaylandDataSource> dataSource_;
    uint32_t dndActions_;
    wl_data_device_manager_dnd_action preferredDndAction_;
    bool inAsk_;

private:
    WaylandDataOffer(struct wl_client *client, uint32_t version, uint32_t id);
};
} // namespace wayland
} // namespace FT

