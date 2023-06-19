/*
 * Copyright (c) 2022 Huawei Device Co., Ltd.
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

#include "device_event_monitor.h"
#include "display_device.h"
#include "display_type.h"
#include "hdi_display.h"
#include "log.h"

#ifdef USE_LIBUDEV
#include "udev_object_helper.h"
#endif // USE_LIBUDEV

namespace oewm {
namespace HDI {
namespace DISPLAY {
// singleton
class HdiSession : NonCopyable {
public:
    ~HdiSession() noexcept;
    static HdiSession &GetInstance();

    const std::shared_ptr<drm::DrmDevice> &GetDisplayDevice() const
    {
        return displayDevice_;
    }

    const std::shared_ptr<DeviceEventMonitor> &GetDeviceEventMonitor() const
    {
        return deviceEventMonitor_;
    }

    template <typename... Args>
    int32_t CallDisplayFunction(DisplayId devId, int32_t (HdiDisplay::*member)(Args...), Args... args)
    {
        if (devId == INVALID_DISPLAY_ID) {
            LOG_ERROR("HdiSession::CallDisplayFunction: invalid device id.");
            return DISPLAY_PARAM_ERR;
        }
        auto &displays = displayDevice_->GetMutableDisplays();
        if (displays.count(devId) == 0) {
            LOG_ERROR("HdiSession::CallDisplayFunction: can not find display for device id %{public}" PRIu32, devId);
            return DISPLAY_FAILURE;
        }
        auto display = displays.at(devId).get();
        return (display->*member)(std::forward<Args &&>(args)...);
    }

    template <typename... Args>
    int32_t CallLayerFunction(DisplayId devId, LayerId layerId, int32_t (HdiLayer::*member)(Args...), Args... args)
    {
        if (devId == INVALID_DISPLAY_ID) {
            LOG_ERROR("HdiSession::CallLayerFunction: invalid device id.");
            return DISPLAY_PARAM_ERR;
        }

        const auto &displays = displayDevice_->GetDisplays();
        if (displays.count(devId) == 0) {
            LOG_ERROR("HdiSession::CallLayerFunction: can not find display for device id %{public}" PRIu32, devId);
            return DISPLAY_FAILURE;
        }

        auto &display = displays.at(devId);
        auto layer = display->GetHdiLayer(layerId);
        if (layer == nullptr) {
            LOG_ERROR("HdiSession::CallLayerFunction: can not find layer(id: %{public}" PRIu32 ") for display %{public}" PRIu32, 
                layerId, devId);
            return DISPLAY_FAILURE;
        }

        return (layer->*member)(std::forward<Args>(args)...);
    }

    int32_t RegHotPlugCallback(HotPlugCallback callback, void *data);

private:
    HdiSession();

    void DoHotPlugCallback(uint32_t devId, bool connect);
#ifdef USE_LIBUDEV
    void OnHotPlugEvent(TimeStamp timeStamp, struct udev_device *event);
    UdevObject<struct udev> udev_;
#endif // USE_LIBUDEV
    std::shared_ptr<DeviceEventMonitor> deviceEventMonitor_;
    std::shared_ptr<drm::DrmDevice> displayDevice_;

    HotPlugCallback hotPlugCallback_ = nullptr;
    void *hotPlugUserData_ = nullptr;
};
} // namespace DISPLAY
} // namespace HDI
} // namespace oewm
