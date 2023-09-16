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

#include "wayland_data_source.h"
#include "wayland_objects_pool.h"

namespace FT {
namespace Wayland {
namespace {
    constexpr HiLogLabel LABEL = {LOG_CORE, HILOG_DOMAIN_WAYLAND, "WaylandDataSource"};
}
static uint32_t const ALL_ACTIONS =
    (WL_DATA_DEVICE_MANAGER_DND_ACTION_COPY | WL_DATA_DEVICE_MANAGER_DND_ACTION_MOVE |
     WL_DATA_DEVICE_MANAGER_DND_ACTION_ASK);
struct wl_data_source_interface IWaylandDataSourceInterface::impl_ = {
    .offer = Offer,
    .destroy = &WaylandResourceObject::DefaultDestroyResource,
    .set_actions = SetActions,
};

void IWaylandDataSourceInterface::Offer(struct wl_client *client, struct wl_resource *resource, const char *mimeType)
{
    LOG_DEBUG("Request: wl_data_source-->Offer.");
    UNUSED(client);
    CAST_OBJECT_AND_CALL_FUNC(
        WaylandDataSource, resource, "IWaylandDataSourceInterface::Offer: failed to find object.", Offer, mimeType);
}

void IWaylandDataSourceInterface::SetActions(struct wl_client *client, struct wl_resource *resource, uint32_t dndAction)
{
    LOG_DEBUG("Request: wl_data_source-->SetActions.");
    UNUSED(client);
    CAST_OBJECT_AND_CALL_FUNC(
        WaylandDataSource, resource, "IWaylandDataSourceInterface::SetActions: failed to find object.", SetActions, dndAction);
}

OHOS::sptr<WaylandDataSource> WaylandDataSource::Create(struct wl_client *client, uint32_t version, uint32_t id)
{
    if (client == nullptr) {
        return nullptr;
    }

    auto dataSource = OHOS::sptr<WaylandDataSource>(new WaylandDataSource(client, version, id));
    WaylandObjectsPool::GetInstance().AddObject(ObjectId(dataSource->WlClient(), dataSource->Id()), dataSource);
    return dataSource;
}

WaylandDataSource::WaylandDataSource(struct wl_client *client, uint32_t version, uint32_t id)
    : WaylandResourceObject(client, &wl_data_source_interface, version, id, &IWaylandDataSourceInterface::impl_)
{
    LOG_DEBUG("WaylandDataSource ctor.");
    actionsSet_ = false;
    selectionSet_ = false;
    dndActions_ = 0;
    currentAndAction_ = WL_DATA_DEVICE_MANAGER_DND_ACTION_NONE;
    compositorAction_ = WL_DATA_DEVICE_MANAGER_DND_ACTION_NONE;

    wl_array_init(&mimeTypes_);
}

WaylandDataSource::~WaylandDataSource() noexcept
{
    LOG_DEBUG("WaylandDataSource dtor.");
}

void WaylandDataSource::Offer(const char *mimeType)
{
    char **p;
    p = static_cast<char **>(wl_array_add(&mimeTypes_, sizeof(*p)));
    if (p) {
        *p = strdup(mimeType);
    }
    if (!p || !*p) {
        LOG_DEBUG("WaylandDataSource::Offer nullptr.");
    }
}

void WaylandDataSource::SetActions(uint32_t dndAction)
{
    if (actionsSet_) {
        LOG_DEBUG("WaylandDataSource::SetActions: cannot set actions more than once.");
        return;
    }
    if (dndAction & ~ALL_ACTIONS) {
        LOG_DEBUG("WaylandDataSource::SetActions: cannot invalid action mask.");
    }
    dndActions_ = dndAction;
    actionsSet_ = true;
}

void WaylandDataSource::SetSelectionSet(bool selectionSet)
{
    selectionSet_ = selectionSet;
}
} // namespace wayland
} // namespace FT
