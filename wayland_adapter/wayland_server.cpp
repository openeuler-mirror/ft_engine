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

WaylandServer::~WaylandServer() noexcept
{
    OnStop();
}

void WaylandServer::CreateGlobalObjects()
{
    compositorGlobal_ = WaylandCompositor::Create(display_);
    xdgWmBaseGlobal_ = WaylandXdgWmBase::Create(display_);
    outputGlobal_ = WaylandOutput::Create(display_);
    subCompositorGlobal_ = WaylandSubCompositor::Create(display_);
    wl_display_add_shm_format(display_, WL_SHM_FORMAT_RGBA8888);
    wl_display_init_shm(display_);
}

void WaylandServer::OnStart()
{
    LOG_INFO("OnStart");

    display_ = wl_display_create();
    if (display_ == nullptr) {
        LOG_ERROR("wl_display_create failed");
        return;
    }

    wlDisplayLoop_ = wl_display_get_event_loop(display_);
    if (wlDisplayLoop_ == nullptr) {
        LOG_ERROR("wl_display_get_event_loop failed");
        wl_display_destroy(display_);
        return;
    }

    socketName_ = wl_display_add_socket_auto(display_);
    if (socketName_.empty()) {
        LOG_ERROR("wl_display_add_socket_auto failed");
        wl_display_destroy(display_);
        return;
    }

    CreateGlobalObjects();

    loop_ = std::make_shared<EventLoop>();
    wlDisplayChannel_ = std::make_unique<EventChannel>(wl_event_loop_get_fd(wlDisplayLoop_), loop_.get());
    wlDisplayChannel_->SetReadCallback([this](TimeStamp timeStamp) {
        wl_event_loop_dispatch(wlDisplayLoop_, -1);
        wl_display_flush_clients(display_);
    });
    wlDisplayChannel_->EnableReading(true);
    loop_->Start();
}

void WaylandServer::OnStop()
{
    LOG_INFO("OnStop");

    if (display_ == nullptr || loop_ == nullptr) {
        return;
    }

    auto stopWlDisplay = loop_->Schedule([this]() {
        wl_display_terminate(display_);
        wl_display_destroy_clients(display_);
        wl_display_destroy(display_);
        if (wlDisplayChannel_ != nullptr) {
            wlDisplayChannel_->DisableAll(true);
            wlDisplayChannel_ = nullptr;
        }
    });
    stopWlDisplay.wait();

    display_ = nullptr;
    loop_ = nullptr;
    compositorGlobal_ = nullptr;
    xdgWmBaseGlobal_ = nullptr;
    outputGlobal_ = nullptr;
    subCompositorGlobal_ = nullptr;
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
