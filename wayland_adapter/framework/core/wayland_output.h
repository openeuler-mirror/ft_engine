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

#include "wayland_global.h"
#include <functional>

namespace FT {
namespace Wayland {

struct IWaylandOutput {
    static struct wl_output_interface impl_;
};

class WaylandOutput final : public WaylandGlobal {
public:
    static OHOS::sptr<WaylandOutput> Create(struct wl_display *display);
    ~WaylandOutput() noexcept override;
    void Send(const OHOS::sptr<WaylandResourceObject> &output);

private:
    WaylandOutput(struct wl_display *display);
    void Bind(struct wl_client *client, uint32_t version, uint32_t id) override;

};

class WaylandOutputObject final : public WaylandResourceObject {
public:
    WaylandOutputObject(struct wl_client *client, uint32_t version, uint32_t id);
    ~WaylandOutputObject() noexcept;
};
} // namespace Wayland
} // namespace FT