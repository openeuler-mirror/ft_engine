/*
 * Copyright (c) 2023 Huawei Device Co., Ltd.
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

#ifndef WAYLAND_SERVER_H
#define WAYLAND_SERVER_H

#include <cstring>
#include <singleton.h>
#include <system_ability.h>

namespace FT {
namespace Wayland {
class WaylandServer : public OHOS::SystemAbility {
public:
    WaylandServer();
    ~WaylandServer() noexcept;

    WaylandServer(const WaylandServer&) = delete;
    WaylandServer& operator=(const WaylandServer&) = delete;

    // sa_main implementation
    void OnStart() override;
    void OnStop() override;
    void OnAddSystemAbility(int32_t systemAbilityId, const std::string &deviceId) override;
    std::string GetClassName() override;
};
} // namespace Wayland
} // namespace FT
#endif // WAYLAND_SERVER_H
