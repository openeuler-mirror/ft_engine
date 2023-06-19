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

#include "device_event_monitor.h"

#include "drm_atomic_committer.h"
#include "hdi_display.h"
#include "log.h"

namespace oewm {
namespace HDI {
namespace DISPLAY {
namespace detail {
// GetFrameCnt for soft vsync.
uint32_t GetFrameCnt()
{
    static std::atomic<uint32_t> frame{0};
    return frame++;
}
} // namespace detail

#ifdef USE_LIBUDEV
DeviceEventMonitor::DeviceEventMonitor(
    struct udev *udevice,
    std::shared_ptr<drm::DrmDevice> drmDevice,
    HotPlugEventCallback hotPlugEventCallback,
    TimeType softVsyncPeriod)
    : loopThread_("DeviceMonitorLoopThread"),
      drmDevice_(std::move(drmDevice)),
      udev_(udevice),
      udevMonitor_(udev_monitor_new_from_netlink(udev_, "udev"), udev_monitor_unref),
      hotplugEventCallback_(std::move(hotPlugEventCallback)),
      drmEventContext_(std::make_unique<drmEventContext>()),
      softVsyncPeriod_(softVsyncPeriod)
{}
#else  // USE_LIBUDEV
DeviceEventMonitor::DeviceEventMonitor(std::shared_ptr<drm::DrmDevice> drmDevice, TimeType softVsyncPeriod)
    : loopThread_("DeviceMonitorLoopThread"),
      drmDevice_(std::move(drmDevice)),
      drmEventContext_(std::make_unique<drmEventContext>()),
      softVsyncPeriod_(softVsyncPeriod)
{}
#endif // USE_LIBUDEV

DeviceEventMonitor::~DeviceEventMonitor() noexcept
{
    if (loop_ == nullptr) {
        return;
    }

    if (softVsyncTimer_ != nullptr) {
        loop_->Cancel(softVsyncTimer_);
    }

    auto future = loop_->Schedule([this]() {
        vsyncChannel_->DisableAll();
        hotplugChannel_->DisableAll();
    });
    future.wait();
}

bool DeviceEventMonitor::CheckDeviceStatus()
{
    if (drmDevice_ == nullptr) {
        LOG_ERROR("DeviceEventMonitor::Init failed: drmDevice_ is nullptr.");
        return false;
    }
    dupDrmFd_ = OHOS::UniqueFd(::dup(drmDevice_->Fd()));

#ifdef USE_LIBUDEV
    if (udev_ == nullptr) {
        LOG_ERROR("DeviceEventMonitor::Init failed: udev_ is nullptr.");
        return false;
    }

    if (udevMonitor_ == nullptr) {
        LOG_ERROR("DeviceEventMonitor::Init failed: udevMonitor_ is nullptr.");
        return false;
    }

    udev_monitor_filter_add_match_subsystem_devtype(udevMonitor_.Get(), "drm", nullptr);
    udevMonitorFd_ = OHOS::UniqueFd(udev_monitor_get_fd(udevMonitor_.Get()));
    if (IsInvalidFd(udevMonitorFd_)) {
        LOG_ERROR("DeviceEventMonitor::Init failed: udevMonitorFd_(%{public}i) is invalid.", udevMonitorFd_.Get());
        return false;
    }
#endif // USE_LIBUDEV

    return true;
}

#ifdef USE_LIBUDEV
void DeviceEventMonitor::OnUdevMonitorEvent(TimeStamp timeStamp)
{
    UdevObject<struct udev_device> event(udev_monitor_receive_device(udevMonitor_.Get()), udev_device_unref);
    if (event == nullptr) {
        return;
    }

    // check if this event is hotplug event.
    const char *val = udev_device_get_property_value(event.Get(), "HOTPLUG");
    if (val == nullptr || strcmp(val, "1") != 0) {
        return;
    }

    if (hotplugEventCallback_ != nullptr) {
        hotplugEventCallback_(timeStamp, event.Get());
    }
}

bool DeviceEventMonitor::RegisterUdevMonitorEventHandler()
{
    hotplugChannel_ = std::make_unique<EventChannel>(udevMonitorFd_, loop_);
    hotplugChannel_->SetReadCallback([this](TimeStamp timestamp) { OnUdevMonitorEvent(timestamp); });
    hotplugChannel_->EnableReading(true);

    auto ret = udev_monitor_enable_receiving(udevMonitor_.Get());
    if (ret < 0) {
        LOG_ERROR("Failed to enable udev-monitor receiving, err: %{public}s", ErrnoToString(errno).c_str());
        return false;
    }

    return true;
}
#endif // USE_LIBUDEV

void DeviceEventMonitor::DrmPageFlipHandler(int fd, uint32_t sequence, uint32_t sec, uint32_t usec, void *data)
{
    auto display = reinterpret_cast<HdiDisplay *>(data);
    if (OE_UNLIKELY(display == nullptr)) {
        LOG_ERROR("DeviceEventMonitor::DrmPageFlipHandler: can not find display instance, user data is nullptr!");
        return;
    }

    UNUSED(fd);

    // DRM timestamp is MONOTONIC and our timestamp is REALTIME.
    TimeStamp timeStamp(
        static_cast<TimeType>(sec) * MICRO_SECS_PER_SECOND + static_cast<TimeType>(usec));
    LOG_DEBUG("DeviceEventMonitor::DrmPageFlipHandler, timestamp: %{public}s", timeStamp.ToFormattedString().c_str());
    display->OnVSync(sequence, static_cast<uint64_t>(timeStamp.Nanos()));
}

void DeviceEventMonitor::OnDrmVsyncEvent(TimeStamp timeStamp)
{
    UNUSED(timeStamp);
    drmHandleEvent(dupDrmFd_, drmEventContext_.get());
}

void DeviceEventMonitor::OnSoftVsyncEvent(TimeStamp monotonicTimeStamp)
{
    auto frame = detail::GetFrameCnt();
    auto displays = drmDevice_->GetDisplays();
    for (auto &[displayId, display] : displays) {
        // LOG_DEBUG("display %{public}u vsync...", displayId);
        display->OnVSync(frame, static_cast<uint64_t>(monotonicTimeStamp.Nanos()));
    }
}

void DeviceEventMonitor::RegisterVsyncEventHandler()
{
#ifdef ENABLE_HARDWARE_VSYNC
    if (drmDevice_->SupportAtomicModeSet()) {
        // TODO: drmEventContext VERSION
        drmEventContext_->version = 3;
        drmEventContext_->page_flip_handler = DrmPageFlipHandler;
        vsyncChannel_ = std::make_unique<EventChannel>(dupDrmFd_, loop_);
        vsyncChannel_->SetReadCallback([this](TimeStamp timestamp) { OnDrmVsyncEvent(timestamp); });
        vsyncChannel_->EnableReading(true);
        LOG_INFO("DeviceEventMonitor::RegisterVsyncEventHandler done by atomic mode setting.");
    } else {
#endif // ENABLE_HARDWARE_VSYNC
        // use soft vsync.
        softVsyncTimer_ = loop_->RunEvery([this]() { 
            TimeStamp timestamp(TimeDiff(TimeStamp::Now(), TimeStamp::SystemStartTime()));
            OnSoftVsyncEvent(timestamp);
        }, softVsyncPeriod_);
        LOG_INFO("DeviceEventMonitor::RegisterVsyncEventHandler done by soft timer.");
#ifdef ENABLE_HARDWARE_VSYNC
    }
#endif // ENABLE_HARDWARE_VSYNC
}

bool DeviceEventMonitor::Init()
{
    loop_ = loopThread_.Start();
    if (loop_ == nullptr) {
        LOG_ERROR("Failed to start loop thread %{public}s", loopThread_.Name().c_str());
        return false;
    }

    auto future = loop_->Schedule([this]() -> bool {
        if (!CheckDeviceStatus()) {
            return false;
        }

#ifdef USE_LIBUDEV
        if (!RegisterUdevMonitorEventHandler()) {
            return false;
        }
#endif // USE_LIBUDEV
        RegisterVsyncEventHandler();
        return true;
    });
    return future.get();
}
} // namespace DISPLAY
} // namespace HDI
} // namespace oewm
