/*
 * Copyright (c) 2022 Huawei Technologies Co., Ltd.
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

#include "drm_common.h"
#include "drm_device.h"
#include "event_loop/event_loop_thread.h"
#include "udev_object_helper.h"

namespace oewm {
namespace HDI {
namespace DISPLAY {
class DeviceEventMonitor : NonCopyable {
public:
#ifdef USE_LIBUDEV
    using HotPlugEventCallback = std::function<void(TimeStamp timeStamp, struct udev_device *)>;
#endif // USE_LIBUDEV
    using VsyncEventCallback = std::function<void(TimeStamp timeStamp, int fd, uint32_t frame, uint32_t crtcId)>;
    // If drm device do not support atomic caps, we will use soft vsync,
    // and the soft refresh rate would be solid to 60Hz.
    static constexpr TimeType DEFAULT_SOFTWARE_VSYNC_PERIOD = 16667; // micro seconds.

public:
#ifdef USE_LIBUDEV
    DeviceEventMonitor(
        struct udev *udevice,
        std::shared_ptr<drm::DrmDevice> drmDevice,
        HotPlugEventCallback hotPlugEventCallback,
        TimeType softVsyncPeriod = DEFAULT_SOFTWARE_VSYNC_PERIOD);
#else  // USE_LIBUDEV
    DeviceEventMonitor(
        std::shared_ptr<drm::DrmDevice> drmDevice,
        TimeType softVsyncPeriod = DEFAULT_SOFTWARE_VSYNC_PERIOD);
#endif // USE_LIBUDEV
    ~DeviceEventMonitor() noexcept;

    bool Init();

private:
#ifdef USE_LIBUDEV
    void OnUdevMonitorEvent(TimeStamp timeStamp);
    bool RegisterUdevMonitorEventHandler();
#endif // USE_LIBUDEV
    static void DrmPageFlipHandler(int fd, uint32_t sequence, uint32_t sec, uint32_t usec, void *data);
    void OnDrmVsyncEvent(TimeStamp timeStamp);
    void OnSoftVsyncEvent(TimeStamp monotonicTimeStamp);
    void RegisterVsyncEventHandler();
    bool CheckDeviceStatus();

    EventLoopThread loopThread_;
    EventLoop *loop_ = nullptr;

    std::shared_ptr<drm::DrmDevice> drmDevice_;
    OHOS::UniqueFd dupDrmFd_;
    std::unique_ptr<EventChannel> hotplugChannel_;

#ifdef USE_LIBUDEV
    struct udev *udev_ = nullptr;
    OHOS::UniqueFd udevMonitorFd_;
    UdevObject<struct udev_monitor> udevMonitor_;
    HotPlugEventCallback hotplugEventCallback_ = nullptr;
#endif // USE_LIBUDEV

    // for drm vsync event if drm device supported atomic modesetting.
    std::unique_ptr<drmEventContext> drmEventContext_;
    std::unique_ptr<EventChannel> vsyncChannel_;
    // for soft vsync.
    TimeType softVsyncPeriod_ = DEFAULT_SOFTWARE_VSYNC_PERIOD;
    TimerId softVsyncTimer_;
};
} // namespace DISPLAY
} // namespace HDI
} // namespace oewm
