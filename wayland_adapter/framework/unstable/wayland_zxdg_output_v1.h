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
#include "wayland_output.h"
#include <xdg-output-unstable-v1-server-protocol.h>

namespace FT {
namespace Wayland {

struct IWaylandZxdgOutputV1 {
    static struct zxdg_output_v1_interface impl_;
};

class WaylandZxdgOutputV1 final : public WaylandResourceObject {
public:
    static OHOS::sptr<WaylandZxdgOutputV1> Create(
        struct wl_client *client,
        uint32_t version,
        uint32_t id,
        struct wl_resource *outputResource);
    ~WaylandZxdgOutputV1() noexcept;
    OHOS::sptr<WaylandOutputObject> GetOutput() const;

private:
    WaylandZxdgOutputV1(struct wl_client *client, uint32_t version, uint32_t id, const OHOS::sptr<WaylandOutputObject> &output);

    OHOS::wptr<WaylandOutputObject> wlOutput_;
};
} // namespace Wayland
} // namespace FT