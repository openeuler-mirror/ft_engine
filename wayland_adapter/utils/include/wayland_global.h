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

#include "wayland_resource_object.h"

namespace FT {
namespace Wayland {
class WaylandGlobal : NonCopyable, public OHOS::RefBase {
public:
    virtual ~WaylandGlobal() noexcept override;

    const std::string &Name() const
    {
        return name_;
    }
    uint32_t MaxSupportVersion() const
    {
        return maxSupportVersion_;
    }

protected:
    WaylandGlobal(struct wl_display *display, const struct wl_interface *interface, uint32_t maxSupportVersion);
    static void BindCallback(struct wl_client *client, void *data, uint32_t version, uint32_t id);
    virtual void Bind(struct wl_client *client, uint32_t version, uint32_t id) = 0;

    std::string name_;
    uint32_t maxSupportVersion_ = 0;
    struct wl_display *display_ = nullptr;
    const struct wl_interface *interface_ = nullptr;
    struct wl_global *global_ = nullptr;
};
} // namespace Wayland
} // namespace FT
