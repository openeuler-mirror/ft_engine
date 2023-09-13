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

#include "wayland_zxdg_output_v1.h"
#include "wayland_objects_pool.h"

namespace FT {
namespace Wayland {

namespace {
    constexpr HiLogLabel LABEL = {LOG_CORE, HILOG_DOMAIN_WAYLAND, "WaylandZxdgOutput"};
}

struct zxdg_output_v1_interface IWaylandZxdgOutputV1::impl_ = {.destroy = &WaylandResourceObject::DefaultDestroyResource};

OHOS::sptr<WaylandZxdgOutputV1> WaylandZxdgOutputV1::Create(
    struct wl_client *client,
    uint32_t version,
    uint32_t id,
    struct wl_resource *outputResource)
{
    if (client == nullptr) {
        LOG_ERROR("OEXdgOutput::Create failed: wl_client is nullptr.");
        return nullptr;
    }

    auto output = CastFromResource<WaylandOutputObject>(outputResource);
    if (output == nullptr) {
        LOG_ERROR("OEXdgOutput::Create failed: outputResource is nullptr.");
        return nullptr;
    }

    auto zxdgOutput = OHOS::sptr<WaylandZxdgOutputV1>(new WaylandZxdgOutputV1(client, version, id, output));
    WaylandObjectsPool::GetInstance().AddObject(ObjectId(zxdgOutput->WlClient(), zxdgOutput->Id()), zxdgOutput);

    return zxdgOutput;
}

WaylandZxdgOutputV1::WaylandZxdgOutputV1(
    struct wl_client *client,
    uint32_t version,
    uint32_t id,
    const OHOS::sptr<WaylandOutputObject> &output)
    : WaylandResourceObject(client, &zxdg_output_v1_interface, version, id, &IWaylandZxdgOutputV1::impl_), wlOutput_(output)
{
}

WaylandZxdgOutputV1::~WaylandZxdgOutputV1() noexcept
{
}

OHOS::sptr<WaylandOutputObject> WaylandZxdgOutputV1::GetOutput() const
{
    return wlOutput_.promote();
}

} // namespace Wayland
} // namespace FT