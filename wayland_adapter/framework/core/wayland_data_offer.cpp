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

#include "wayland_data_offer.h"
#include "wayland_objects_pool.h"

namespace FT {
namespace Wayland {
namespace {
    constexpr HiLogLabel LABEL = {LOG_CORE, HILOG_DOMAIN_WAYLAND, "WaylandDataOffer"};
}
struct wl_data_offer_interface IWaylandDataOfferInterface::impl_ = {
    .accept = Accept,
    .receive = Receive,
    .destroy = &WaylandResourceObject::DefaultDestroyResource,
    .finish = Finish,
    .set_actions = SetActions,
};

void IWaylandDataOfferInterface::Accept(
    struct wl_client *client,
    struct wl_resource *resource,
    uint32_t serial,
    const char *mimeType)
{
    LOG_DEBUG("Request: wl_data_offer-->Accept");
    UNUSED(client);
    CAST_OBJECT_AND_CALL_FUNC(
        WaylandDataOffer, resource, "IWaylandDataOfferInterface::Accept: failed to find object.", Accept, serial, mimeType);
}

void IWaylandDataOfferInterface::Receive(
    struct wl_client *client,
    struct wl_resource *resource,
    const char *mimeType,
    int32_t fd)
{
    LOG_DEBUG("Request: wl_data_offer-->Receive");
    UNUSED(client);
    CAST_OBJECT_AND_CALL_FUNC(
        WaylandDataOffer, resource, "IWaylandDataOfferInterface::Receive: failed to find object.", Receive, mimeType, fd);
}

void IWaylandDataOfferInterface::Finish(struct wl_client *client, struct wl_resource *resource)
{
    LOG_DEBUG("Request: wl_data_offer-->Finish");
    UNUSED(client);
    CAST_OBJECT_AND_CALL_FUNC(WaylandDataOffer, resource, "IWaylandDataOfferInterface::Finish: failed to find object.", Finish);
}

void IWaylandDataOfferInterface::SetActions(
    struct wl_client *client,
    struct wl_resource *resource,
    uint32_t dndActions,
    uint32_t preferredAction)
{
    LOG_DEBUG("Request: wl_data_offer-->SetActions");
    UNUSED(client);
    CAST_OBJECT_AND_CALL_FUNC(
        WaylandDataOffer,
        resource,
        "IWaylandDataOfferInterface::SetActions: failed to find object.",
        SetActions,
        dndActions,
        preferredAction);
}

OHOS::sptr<WaylandDataOffer> WaylandDataOffer::Create(struct wl_client *client, uint32_t version, uint32_t id)
{
    if (client == nullptr) {
        return nullptr;
    }
    auto dataOffer = OHOS::sptr<WaylandDataOffer>(new WaylandDataOffer(client, version, id));
    WaylandObjectsPool::GetInstance().AddObject(ObjectId(dataOffer->WlClient(), dataOffer->Id()), dataOffer);
    return dataOffer;
}

WaylandDataOffer::WaylandDataOffer(struct wl_client *client, uint32_t version, uint32_t id)
    : WaylandResourceObject(client, &wl_data_offer_interface, version, id, &IWaylandDataOfferInterface::impl_)
{
    LOG_DEBUG("WaylandDataOffer ctor.");
}

WaylandDataOffer::~WaylandDataOffer() noexcept
{
    LOG_DEBUG("WaylandDataOffer dtor.");
}

void WaylandDataOffer::Accept(uint32_t serial, const char *mimeType) {}

void WaylandDataOffer::Receive(const char *mimeType, int32_t fd) {}

void WaylandDataOffer::Finish() {}

void WaylandDataOffer::SetActions(uint32_t dndActions, uint32_t preferredAction) {}
} // namespace wayland
} // namespace FT
