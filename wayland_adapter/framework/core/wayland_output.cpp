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
#include "wayland_output.h"
#include "wayland_objects_pool.h"
#include "version.h"
#include "display_manager.h"

namespace FT {
namespace Wayland {
namespace {
    constexpr HiLogLabel LABEL = {LOG_CORE, HILOG_DOMAIN_WAYLAND, "WaylandCompositor"};
}
struct wl_output_interface IWaylandOutput::impl_ = {.release = &WaylandResourceObject::DefaultDestroyResource};

OHOS::sptr<WaylandOutput> WaylandOutput::Create(struct wl_display *display)
{
    if (display == nullptr) {
        LOG_ERROR("display nullptr");
        return nullptr;
    }

    return OHOS::sptr<WaylandOutput>(new WaylandOutput(display));
}

WaylandOutput::WaylandOutput(struct wl_display *display)
    : WaylandGlobal(display, &wl_output_interface, WL_OUTPUT_MAX_VERSION)
{
}

WaylandOutput::~WaylandOutput()  noexcept
{
}

void WaylandOutput::Send(const OHOS::sptr<WaylandResourceObject> &output)
{
    if (output == nullptr) {
        return;
    }
    wl_output_send_geometry(
        output->WlResource(), 0, 0, 0, 0, 0, "fangtian", "unknown", 0);

    wl_output_send_scale(output->WlResource(), 1);
    auto defaultDisplay = OHOS::Rosen::DisplayManager::GetInstance().GetDefaultDisplay();
    auto displays = OHOS::Rosen::DisplayManager::GetInstance().GetAllDisplays();
    for (auto &dis : displays) {
        if (dis == nullptr) {
            continue;
        }
        uint32_t flags = 0;
        if (dis == defaultDisplay) {
           flags = WL_OUTPUT_MODE_CURRENT | WL_OUTPUT_MODE_PREFERRED;
        }
        wl_output_send_mode(output->WlResource(), flags, dis->GetWidth(), dis->GetHeight(),
            dis->GetRefreshRate() * 1000);
    }
    wl_output_send_done(output->WlResource());
}

void WaylandOutput::Bind(struct wl_client *client, uint32_t version, uint32_t id)
{
    auto object = OHOS::sptr<WaylandOutputObject>(new WaylandOutputObject(client, version, id));
    if (object == nullptr) {
        LOG_ERROR("no memory");
        return;
    }
    WaylandObjectsPool::GetInstance().AddObject(ObjectId(object->WlClient(), object->Id()), object);

    Send(object);
}

WaylandOutputObject::WaylandOutputObject(struct wl_client *client, uint32_t version, uint32_t id)
    : WaylandResourceObject(client, &wl_output_interface, version, id, &IWaylandOutput::impl_)
{
}

WaylandOutputObject::~WaylandOutputObject() noexcept
{
}

} // Wayland
} // FT