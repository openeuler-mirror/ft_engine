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

namespace FT {
namespace Wayland {
struct IWaylandDataSourceInterface {
    /**
     *@brief Add an offered mime type
     *
     * @param client
     * @param resource
     * @param mimeType mime type offered by the data source
     */
    static void Offer(struct wl_client *client, struct wl_resource *resource, const char *mimeType);

    /**
     *@brief Set the available drag-and-drop actions
     *
     * @param client
     * @param resource
     * @param dnd_actions actions supported by the data source
     * @since 3
     */
    static void SetActions(struct wl_client *client, struct wl_resource *resource, uint32_t dndAction);
    static struct wl_data_source_interface impl_;
};

class WaylandDataSource final : public WaylandResourceObject {
public:
    static OHOS::sptr<WaylandDataSource> Create(struct wl_client *client, uint32_t version, uint32_t id);
    ~WaylandDataSource() noexcept override;

    friend struct IWaylandDataSourceInterface;
    // IWaylandDataSourceInterface will call these member functions.
    void Offer(const char *mimeType);
    void SetActions(uint32_t dndAction);
    bool GetActionsSet() const
    {
        return actionsSet_;
    }
    void SetSelectionSet(bool selectionSet);
    bool GetSelectionSet() const
    {
        return selectionSet_;
    }
    struct wl_array mimeTypes_;
    bool accepted_;
    // OHOS::sptr<WaylandDataOffer> dataOffer_;

private:
    WaylandDataSource(struct wl_client *client, uint32_t version, uint32_t id);

    bool actionsSet_;
    bool selectionSet_;
    uint32_t dndActions_;
    enum wl_data_device_manager_dnd_action currentAndAction_;
    enum wl_data_device_manager_dnd_action compositorAction_;
};
} // namespace wayland
} // namespace FT

