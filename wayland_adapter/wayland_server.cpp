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

#include "wayland_server.h"

#include <system_ability_definition.h>
#include "wayland_adapter_hilog.h"

namespace FT {
namespace Wayland {
namespace {
    constexpr HiLogLabel LABEL = {LOG_CORE, HILOG_DOMAIN_WAYLAND, "WaylandServer"};
}

const bool REGISTER_RESULT =  OHOS::SystemAbility::MakeAndRegisterAbility(new WaylandServer());

// TODO: replace WINDOW_MANAGER_ID with WAYLAND_ADAPTER_ID
WaylandServer::WaylandServer() : OHOS::SystemAbility(OHOS::WINDOW_MANAGER_ID, true) {}

WaylandServer::~WaylandServer() noexcept {}

void WaylandServer::OnStart()
{
    LOG_INFO("OnStart");
}

void WaylandServer::OnStop()
{
    LOG_INFO("OnStop");
}

void WaylandServer::OnAddSystemAbility(int32_t systemAbilityId, const std::string &deviceId)
{
    LOG_INFO("systemAbilityId: %{public}d, start", systemAbilityId);
}

std::string WaylandServer::GetClassName()
{
    return "WaylandServer";
}
} // namespace Wayland
} // namespace FT
