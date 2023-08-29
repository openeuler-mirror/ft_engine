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

#include "wayland_global.h"

namespace FT {
namespace Wayland {
namespace {
    constexpr HiLogLabel LABEL = {LOG_CORE, HILOG_DOMAIN_WAYLAND, "WaylandGlobal"};
}

WaylandGlobal::WaylandGlobal(struct wl_display *display, const struct wl_interface *interface, uint32_t maxSupportVersion)
    : display_(display),
      interface_(interface),
      maxSupportVersion_(maxSupportVersion),
      global_(wl_global_create(display_, interface_, maxSupportVersion_, this, &WaylandGlobal::BindCallback))
{
    name_ = std::string(interface_->name) + "_" + std::to_string(maxSupportVersion_);
}

WaylandGlobal::~WaylandGlobal() noexcept
{
    if (global_ != nullptr) {
        wl_global_destroy(global_);
    }
}

void WaylandGlobal::BindCallback(struct wl_client *client, void *data, uint32_t version, uint32_t id)
{
    if (client == nullptr) {
        LOG_ERROR("wl_client is nullptr");
        return;
    }

    auto global = OHOS::sptr<WaylandGlobal>(static_cast<WaylandGlobal *>(data));
    if (global == nullptr) {
        LOG_ERROR("BindCallback failed");
        return;
    }

    if (version > global->MaxSupportVersion()) {
        LOG_ERROR("Unsupported version");
        return;
    }

    global->Bind(client, version, id);
}
} // namespace Wayland
} // namespace FT
