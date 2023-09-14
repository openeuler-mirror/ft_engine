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

#include "wayland_zxdg_output_manager_v1.h"
#include "display_manager.h"
#include "version.h"
#include "wayland_objects_pool.h"

namespace FT {
namespace Wayland {
namespace {
    constexpr HiLogLabel LABEL = {LOG_CORE, HILOG_DOMAIN_WAYLAND, "WaylandZxdgOutputMgr"};
}

struct zxdg_output_manager_v1_interface IWaylandZxdgOutputManagerV1::impl_ = {
    .destroy = &WaylandResourceObject::DefaultDestroyResource,
    .get_xdg_output = IWaylandZxdgOutputManagerV1::GetZxdgOutput,
};

void IWaylandZxdgOutputManagerV1::GetZxdgOutput(
    struct wl_client *client,
    struct wl_resource *resource,
    uint32_t id,
    struct wl_resource *output)
{
    CAST_OBJECT_AND_CALL_FUNC(WaylandZxdgOutputManagerObject, resource,
        "WaylandZxdgOutputManagerObject::GetZxdgOutput: failed to find object.", GetZxdgOutput, client, resource, id, output);
}

OHOS::sptr<WaylandZxdgOutputManagerV1> WaylandZxdgOutputManagerV1::Create(struct wl_display *display)
{
    if (display == nullptr) {
        LOG_ERROR("display is nullptr");
        return nullptr;
    }

    return OHOS::sptr<WaylandZxdgOutputManagerV1>(new WaylandZxdgOutputManagerV1(display));
}

WaylandZxdgOutputManagerV1::WaylandZxdgOutputManagerV1(struct wl_display *display)
    : WaylandGlobal(display, &zxdg_output_manager_v1_interface, ZXDG_OUTPUT_MANAGER_V1_MAX_VERSION)
{
}

WaylandZxdgOutputManagerV1::~WaylandZxdgOutputManagerV1() noexcept
{
}

void WaylandZxdgOutputManagerV1::Bind(struct wl_client *client, uint32_t version, uint32_t id)
{
    auto object = OHOS::sptr<WaylandZxdgOutputManagerObject>(new WaylandZxdgOutputManagerObject(client, version, id));
    if (object == nullptr) {
        LOG_ERROR("no memory");
        return;
    }
    WaylandObjectsPool::GetInstance().AddObject(ObjectId(object->WlClient(), object->Id()), object);
}

WaylandZxdgOutputManagerObject::WaylandZxdgOutputManagerObject(
    struct wl_client *client, uint32_t version, uint32_t id)
    : WaylandResourceObject(client, &zxdg_output_manager_v1_interface, version, id, &IWaylandZxdgOutputManagerV1::impl_)
{
}

WaylandZxdgOutputManagerObject::~WaylandZxdgOutputManagerObject() noexcept
{
}

void WaylandZxdgOutputManagerObject::GetZxdgOutput(struct wl_client *client, struct wl_resource *resource,
    uint32_t id, struct wl_resource *output)
{
    auto xdgOutput = WaylandZxdgOutputV1::Create(client, wl_resource_get_version(resource), id, output);
    if (xdgOutput == nullptr) {
        LOG_ERROR("WaylandZxdgOutputManagerObject GetZxdgOutput failed.");
        return;
    }
    Send(xdgOutput);
}

void WaylandZxdgOutputManagerObject::Send(const OHOS::sptr<WaylandZxdgOutputV1> &xdgOutput)
{
    zxdg_output_v1_send_logical_position(xdgOutput->WlResource(), 0, 0);
    auto defaultDisplay = OHOS::Rosen::DisplayManager::GetInstance().GetDefaultDisplay();
    if (defaultDisplay != nullptr) {
        zxdg_output_v1_send_logical_size(xdgOutput->WlResource(),
            defaultDisplay->GetWidth(), defaultDisplay->GetHeight());
    }
    zxdg_output_v1_send_name(xdgOutput->WlResource(), "fangtian");
    zxdg_output_v1_send_done(xdgOutput->WlResource());
}

} // Wayland
} // FT